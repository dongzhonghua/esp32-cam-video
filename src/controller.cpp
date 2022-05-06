//
// Created by zhonghua dong on 2022/5/4.
//
#include "controller.h"

WebServer webServer(80);
two_wheels controller::device = two_wheels();

void controller::init() {
    webServer.on("/favicon.ico", HTTP_GET, handleFavicon);
    webServer.onNotFound(handleNotFound);
    webServer.on("/axis", HTTP_GET, handleAxis);
    webServer.on("/mjpeg/1", HTTP_GET, handleJPGSstream);
    webServer.on("/jpg", HTTP_GET, handleJPG);
    //  Starting webserver
    webServer.begin();
    Serial.println("webServer init success...");
}

void controller::handleNotFound() {
    String message = "Server is running!\n\n";
    message += "URI: ";
    message += webServer.uri();
    message += "\nMethod: ";
    message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += webServer.args();
    message += "\n";
    webServer.send(200, "text / plain", message);
}

void controller::handleFavicon() {
    webServer.send(200, "text/plain", "");
}

void controller::handleAxis() {
    int args = webServer.args();
    printf("webServer args num: %d \n", args);
    if (webServer.hasArg("x") && webServer.hasArg("y")) {
        float x = webServer.arg("x").toFloat();
        float y = webServer.arg("y").toFloat();
        printf("receive axis: %f, %f \n", x, y);
        device.set_axis(x, y);
        webServer.sendHeader("Access-Control-Allow-Origin", "*");
        webServer.sendHeader("Access-Control-Allow-Methods", "*");
        webServer.send(200, "text / plain", "set success");
    } else {
        webServer.sendHeader("Access-Control-Allow-Origin", "*");
        webServer.sendHeader("Access-Control-Allow-Methods", "*");
        webServer.send(200, "text / plain", "success");
    }
}


#include "camera.h"


void controller::handleJPGSstream() {
    if (noActiveClients >= MAX_CLIENTS) return;
    Serial.printf("handleJPGSstream start: free heap  : %d\n", ESP.getFreeHeap());

    streamInfo *info = new streamInfo;

    info->frame = frameNumber - 1;
    info->client = webServer.client();
    info->buffer = NULL;
    info->len = 0;

    //  Creating task to push the stream to all connected clients
    int rc = xTaskCreatePinnedToCore(
            streamCB,
            "strmCB",
            10 * 1024,
            (void *) info,
            2,
            &info->task,
            APP_CPU);
    if (rc != pdPASS) {
        Serial.printf("handleJPGSstream: error creating RTOS task. rc = %d\n", rc);
        Serial.printf("handleJPGSstream: free heap  : %d\n", ESP.getFreeHeap());
        //    Serial.printf("stk high wm: %d\n", uxTaskGetStackHighWaterMark(tSend));
        delete info;
    }

    noActiveClients++;

    // Wake up streaming tasks, if they were previously suspended:
    if (eTaskGetState(tCam) == eSuspended) vTaskResume(tCam);
}


void controller::handleJPG() {
    WiFiClient client = webServer.client();

    if (!client.connected()) return;
    camera_fb_t *fb = esp_camera_fb_get();
    client.write(JHEADER, jhdLen);
    client.write((char *) fb->buf, fb->len);
    esp_camera_fb_return(fb);
}


