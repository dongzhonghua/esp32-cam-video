//
// Created by zhonghua dong on 2022/5/1.
//

#ifndef ESP32_CAM_VIDEO_WHEEL_H
#define ESP32_CAM_VIDEO_WHEEL_H


class wheel {
public:
    int l;
    int l_channel;
    int r;
    int r_channel;
    int which_wheel;
    float throttle{}; // 油门

    wheel(int l, int r, int wheel); // wheel 0左 1右
    void set_throttle(float throttle);
};


#endif //ESP32_CAM_VIDEO_WHEEL_H
