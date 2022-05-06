//
// Created by zhonghua dong on 2022/5/4.
//

#ifndef ESP32_CAM_VIDEO_CAMERA_H
#define ESP32_CAM_VIDEO_CAMERA_H

#include "esp_camera.h"
#include "ov2640.h"
#include <esp_bt.h>
#include <driver/rtc_io.h>
#include <SD.h>
#include "camera_pins.h"
#include "controller.h"

const int WSINTERVAL = 100;
// frameSync semaphore is used to prevent streaming buffer as it is replaced with the next frame
#define PRO_CPU 0


const char JHEADER[] = "HTTP/1.1 200 OK\r\n" \
                       "Content-disposition: inline; filename=capture.jpg\r\n" \
                       "Content-type: image/jpeg\r\n\r\n"; // Content-disposition 默认inline，filename表示下载内容时的推荐命名。attachment的话就是自动下载
const int jhdLen = strlen(JHEADER);

// ==== STREAMING ======================================================
const char HEADER[] = "HTTP/1.1 200 OK\r\n" \
                      "Access-Control-Allow-Origin: *\r\n" \
                      "Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n";
const char BOUNDARY[] = "\r\n--123456789000000000000987654321\r\n";
const char CTNTTYPE[] = "Content-Type: image/jpeg\r\nContent-Length: ";
const int hdrLen = strlen(HEADER);
const int bdrLen = strlen(BOUNDARY);
const int cntLen = strlen(CTNTTYPE);


// ===== rtos task handles =========================
// Streaming is implemented with 3 tasks:
extern TaskHandle_t tCam;     // handles getting picture frames from the camera and storing them locally
extern SemaphoreHandle_t frameSync;
extern uint8_t noActiveClients;       // number of active clients
extern TaskHandle_t tMjpeg;   // handles client connections to the webserver
// Current frame information
extern volatile uint32_t frameNumber;
extern volatile size_t camSize;    // size of the current frame, byte
extern volatile char *camBuf;      // pointer to the current frame
// We will try to achieve 24 FPS frame rate
const int FPS = 24;

// We will handle web client requests every 100 ms (10 Hz)
#define MAX_CLIENTS   5
#define APP_CPU 1


struct streamInfo {
    uint32_t frame;
    WiFiClient client;
    TaskHandle_t task;
    char *buffer;
    size_t len;
};

char *allocateMemory(char *aPtr, size_t aSize);

void streamCB(void *pvParameters);

void camCB(void *pvParameters);

void mjpegCB(void *pvParameters);


#endif //ESP32_CAM_VIDEO_CAMERA_H
