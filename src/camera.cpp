//
// Created by zhonghua dong on 2022/5/4.
//

#include "camera.h"

TaskHandle_t tCam;     // handles getting picture frames from the camera and storing them locally
SemaphoreHandle_t frameSync = nullptr;
uint8_t noActiveClients;       // number of active clients
TaskHandle_t tMjpeg;   // handles client connections to the webserver
// Current frame information
volatile uint32_t frameNumber;
volatile size_t camSize;    // size of the current frame, byte
volatile char *camBuf;      // pointer to the current frame


// ==== Memory allocator that takes advantage of PSRAM if present =======================
char *allocateMemory(char *aPtr, size_t aSize) {

    //  Since current buffer is too smal, free it
    if (aPtr != nullptr) free(aPtr);

    char *ptr = nullptr;
    ptr = (char *) ps_malloc(aSize);

    // If the memory pointer is NULL, we were not able to allocate any memory, and that is a terminal condition.
    if (ptr == nullptr) {
        Serial.println("Out of memory!");
        delay(5000);
        ESP.restart();
    }
    return ptr;
}

// ==== Actually stream content to all connected clients ========================
void streamCB(void *pvParameters) {
    char buf[16];
    TickType_t xLastWakeTime;
    TickType_t xFrequency;

    streamInfo *info = (streamInfo *) pvParameters;

    if (info == nullptr) {
        Serial.println("streamCB: a NULL pointer passed");
    }
    //  Immediately send this client a header
    info->client.write(HEADER, hdrLen);
    info->client.write(BOUNDARY, bdrLen);
    taskYIELD();

    xLastWakeTime = xTaskGetTickCount();
    xFrequency = pdMS_TO_TICKS(1000 / FPS);

    for (;;) {
        //  Only bother to send anything if there is someone watching
        if (info->client.connected()) {

            if (info->frame != frameNumber) {
                xSemaphoreTake(frameSync, portMAX_DELAY);
                if (info->buffer == NULL) {
                    info->buffer = allocateMemory(info->buffer, camSize);
                    info->len = camSize;
                } else {
                    if (camSize > info->len) {
                        info->buffer = allocateMemory(info->buffer, camSize);
                        info->len = camSize;
                    }
                }
                memcpy(info->buffer, (const void *) camBuf, info->len);
                xSemaphoreGive(frameSync);
                taskYIELD();

                info->frame = frameNumber;
                info->client.write(CTNTTYPE, cntLen);
                sprintf(buf, "%d\r\n\r\n", info->len);
                info->client.write(buf, strlen(buf));
                info->client.write((char *) info->buffer, (size_t) info->len);
                info->client.write(BOUNDARY, bdrLen);
                info->client.flush();
            }
        } else {
            //  client disconnected - clean up.
            noActiveClients--;
            Serial.printf("streamCB: Stream Task stack wtrmark  : %d\n", uxTaskGetStackHighWaterMark(info->task));
            Serial.flush();
            info->client.flush();
            info->client.stop();
            if (info->buffer) {
                free(info->buffer);
                info->buffer = nullptr;
            }
            delete info;
            info = nullptr;
            vTaskDelete(nullptr);
        }
        //  Let other tasks run after serving every client
        taskYIELD();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// ==== RTOS task to grab frames from the camera =========================
void camCB(void *pvParameters) {

    TickType_t xLastWakeTime;

    //  A running interval associated with currently desired frame rate
    const TickType_t xFrequency = pdMS_TO_TICKS(1000 / FPS);

    //  Pointers to the 2 frames, their respective sizes and index of the current frame
    char *fbs[2] = {nullptr, nullptr};
    size_t fSize[2] = {0, 0};
    int ifb = 0;
    frameNumber = 0;

    //=== loop() section  ===================
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {

        //  Grab a frame from the camera and query its size
        camera_fb_t *fb = nullptr;

        fb = esp_camera_fb_get();
        size_t s = fb->len;

        //  If frame size is more that we have previously allocated - request  125% of the current frame space
        if (s > fSize[ifb]) {
            fSize[ifb] = s + s;
            fbs[ifb] = allocateMemory(fbs[ifb], fSize[ifb]);
        }

        //  Copy current frame into local buffer
        char *b = (char *) fb->buf;
        memcpy(fbs[ifb], b, s);
        esp_camera_fb_return(fb);

        //  Let other tasks run and wait until the end of the current frame rate interval (if any time left_wheel)
        taskYIELD();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        //  Only switch frames around if no frame is currently being streamed to a client
        //  Wait on a semaphore until client operation completes
        //    xSemaphoreTake( frameSync, portMAX_DELAY );

        //  Do not allow frame copying while switching the current frame
        xSemaphoreTake(frameSync, xFrequency);
        camBuf = fbs[ifb];
        camSize = s;
        ifb++;
        ifb &= 1;  // this should produce 1, 0, 1, 0, 1 ... sequence
        frameNumber++;
        //  Let anyone waiting for a frame know that the frame is ready
        xSemaphoreGive(frameSync);

        //  Immediately let other (streaming) tasks run
        taskYIELD();

        //  If streaming task has suspended itself (no active clients to stream to)
        //  there is no need to grab frames from the camera. We can save some juice
        //  by suspedning the tasks
        if (noActiveClients == 0) {
            Serial.printf("mjpegCB: free heap           : %d\n", ESP.getFreeHeap());
            Serial.printf("mjpegCB: min free heap)      : %d\n", ESP.getMinFreeHeap());
            Serial.printf("mjpegCB: max alloc free heap : %d\n", ESP.getMaxAllocHeap());
            Serial.printf("mjpegCB: tCam stack wtrmark  : %d\n", uxTaskGetStackHighWaterMark(tCam));
            Serial.flush();
            vTaskSuspend(nullptr);  // passing NULL means "suspend yourself"
        }
    }
}

// ======== Server Connection Handler Task ==========================
void mjpegCB(void *pvParameters) {
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(WSINTERVAL);

    // Creating frame synchronization semaphore and initializing it
    frameSync = xSemaphoreCreateBinary();
    xSemaphoreGive(frameSync);

    //=== setup section  ==================

    //  Creating RTOS task for grabbing frames from the camera
    xTaskCreatePinnedToCore(
            camCB,        // callback
            "cam",        // name
            10 * 1024,       // stacj size
            nullptr,         // parameters
            2,            // priority
            &tCam,        // RTOS task handle
            PRO_CPU);     // core



    noActiveClients = 0;

    Serial.printf("\nmjpegCB: free heap (start)  : %d\n", ESP.getFreeHeap());
    //=== loop() section  ===================
    xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        webServer.handleClient();

        //  After every webServer client handling request, we let other tasks run and then pause
        taskYIELD();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}