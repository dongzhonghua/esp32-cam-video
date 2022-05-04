//
// Created by zhonghua dong on 2022/5/4.
//

#ifndef ESP32_CAM_VIDEO_CONTROLLER_H
#define ESP32_CAM_VIDEO_CONTROLLER_H
#include <WebServer.h>
#include <WiFiClient.h>
#include "two_wheels.h"

class controller {
public:
    inline static WebServer *server =  new WebServer(80);
    inline static two_wheels *device = new two_wheels();
    static void handleAxis();
    static void handleNotFound();
    static void handleFavicon();
    static void init();

};


#endif //ESP32_CAM_VIDEO_CONTROLLER_H
