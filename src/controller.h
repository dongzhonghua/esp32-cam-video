//
// Created by zhonghua dong on 2022/5/4.
//



#ifndef ESP32_CAM_VIDEO_CONTROLLER_H
#define ESP32_CAM_VIDEO_CONTROLLER_H

#include "WebServer.h"
#include "two_wheels.h"

extern WebServer webServer;

class controller {
public:
    static two_wheels device;

    static void handleAxis();

    static void handleNotFound();

    static void handleFavicon();

    static void handleJPGSstream();

    static void handleJPG();

    static void init();

};


#endif //ESP32_CAM_VIDEO_CONTROLLER_H
