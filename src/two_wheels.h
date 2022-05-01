//
// Created by zhonghua dong on 2022/5/1.
//

#ifndef ESP32_CAM_VIDEO_TWO_WHEELS_H
#define ESP32_CAM_VIDEO_TWO_WHEELS_H


#include "wheel.h"

class two_wheels {
private:
    wheel left_wheel;
    wheel right_wheel;
public:
    two_wheels();
    void set_axis(float x, float y);
};


#endif //ESP32_CAM_VIDEO_TWO_WHEELS_H
