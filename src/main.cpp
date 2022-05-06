/*

  This is a simple MJPEG streaming webserver implemented for AI-Thinker ESP32-CAM
  and ESP-EYE modules.
  This is tested to work with VLC and Blynk video widget and can support up to 10
  simultaneously connected streaming clients.
  Simultaneous streaming is implemented with dedicated FreeRTOS tasks.

  Inspired by and based on this Instructable: $9 RTSP Video Streamer Using the ESP32-CAM Board
  (https://www.instructables.com/id/9-RTSP-Video-Streamer-Using-the-ESP32-CAM-Board/)

  Board: AI-Thinker ESP32-CAM or ESP-EYE
  Compile as:
   ESP32 Dev Module
   CPU Freq: 240
   Flash Freq: 80
   Flash mode: QIO
   Flash Size: 4Mb
   Partrition: Minimal SPIFFS
   PSRAM: Enabled
*/

// ESP32 has two cores: APPlication core and PROcess core (the one that runs ESP32 SDK stack)
#define CAMERA_MODEL_AI_THINKER

#include "esp_camera.h"
#include "ov2640.h"
#include <WiFi.h>
#include <WiFiClient.h>

#include <esp_wifi.h>
#include <driver/rtc_io.h>
#include "camera.h"

#include "camera_pins.h"
#include "network.h"

Network network;

// ==== SETUP method ==================================================================
void setup() {

    // Setup Serial connection:
    Serial.begin(115200);
    delay(1000); // wait for a second to let Serial connect
    Serial.printf("setup: free heap  : %d\n", ESP.getFreeHeap());


    //  Configure and connect to WiFi
    network.init(WIFINAME, WIFIPW);
    Serial.print("Stream Link: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/mjpeg/1");
    //  Registering webserver handling routines

    controller::init();

    static camera_config_t camera_config = {
            .pin_pwdn       = PWDN_GPIO_NUM,
            .pin_reset      = RESET_GPIO_NUM,
            .pin_xclk       = XCLK_GPIO_NUM,
            .pin_sscb_sda   = SIOD_GPIO_NUM,
            .pin_sscb_scl   = SIOC_GPIO_NUM,
            .pin_d7         = Y9_GPIO_NUM,
            .pin_d6         = Y8_GPIO_NUM,
            .pin_d5         = Y7_GPIO_NUM,
            .pin_d4         = Y6_GPIO_NUM,
            .pin_d3         = Y5_GPIO_NUM,
            .pin_d2         = Y4_GPIO_NUM,
            .pin_d1         = Y3_GPIO_NUM,
            .pin_d0         = Y2_GPIO_NUM,
            .pin_vsync      = VSYNC_GPIO_NUM,
            .pin_href       = HREF_GPIO_NUM,
            .pin_pclk       = PCLK_GPIO_NUM,
            .xclk_freq_hz   = 20000000,
            .ledc_timer     = LEDC_TIMER_0,
            .ledc_channel   = LEDC_CHANNEL_0,
            .pixel_format   = PIXFORMAT_JPEG,
            .frame_size     = FRAMESIZE_XGA,
            .jpeg_quality   = 16,
            .fb_count       = 2
    };

    if (esp_camera_init(&camera_config) != ESP_OK) {
        Serial.println("Error initializing the camera");
        delay(10000);
        ESP.restart();
    }


    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, true);
    // Start mainstreaming RTOS task
    xTaskCreatePinnedToCore(
            mjpegCB,
            "mjpeg",
            10 * 1024,
            nullptr,
            2,
            &tMjpeg,
            APP_CPU);

    Serial.printf("setup complete: free heap  : %d\n", ESP.getFreeHeap());
}

void loop() {
    // this seems to be necessary to let IDLE task run and do GC
    vTaskDelay(1000);
}
