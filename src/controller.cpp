//
// Created by zhonghua dong on 2022/5/4.
//

#include "controller.h"

void controller::handleNotFound() {
    String message = "Server is running!\n\n";
    message += "URI: ";
    message += server->uri();
    message += "\nMethod: ";
    message += (server->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server->args();
    message += "\n";
    server->send(200, "text / plain", message);
}

void controller::handleFavicon() //回調函數
{
    server->send(200, "text/plain", "");
}

void controller::handleAxis() {
    int args = server->args();
    printf("server args num: %d \n", args);
    if (server->hasArg("x") && server->hasArg("y")) {
        float x = server->arg("x").toFloat();
        float y = server->arg("y").toFloat();
        printf("receive axis: %f, %f \n", x, y);
        device->set_axis(x, y);
        server->sendHeader("Access-Control-Allow-Origin", "*");
        server->sendHeader("Access-Control-Allow-Methods", "*");
        server->send(200, "text / plain", "set success");
    } else {
        server->sendHeader("Access-Control-Allow-Origin", "*");
        server->sendHeader("Access-Control-Allow-Methods", "*");
        server->send(200, "text / plain", "success");
    }
}

void controller::init() {
    server->on("/favicon.ico", HTTP_GET, handleFavicon);
    server->onNotFound(handleNotFound);
    server->on("/axis", HTTP_GET, handleAxis);
    //  Starting webserver
    server->begin();
    Serial.println("server init success...");
}

