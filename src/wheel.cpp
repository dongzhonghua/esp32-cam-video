//
// Created by zhonghua dong on 2022/5/1.
//

#include <esp32-hal-ledc.h>
#include <cstdio>
#include "algorithm"
#include "wheel.h"

// 舵机通道
int channel_PWM_L1 = 3;
int channel_PWM_L2 = 4;
int channel_PWM_R1 = 5;
int channel_PWM_R2 = 6;

// 舵机频率，那么周期也就是1/50，也就是20ms ，PWM一共有16个通道，0-7位高速通道由80Mhz时钟驱动，后面8个为低速通道由1Mhz时钟驱动
int freq_PWM = 50;
// PWM分辨率，取值为 0-20 之间  ，这里填写为10，那么后面的ledcWrite 这个里面填写的pwm值就在 0 - 2的10次方 之间 也就是 0-1024 ，如果是要求不高的东西你可以直接拿1000去算了
int resolution_PWM = 10;

wheel::wheel(int l, int r, int which_wheel) {
    this->l = l;
    this->r = r;
    this->which_wheel = which_wheel;

    if (which_wheel == 0) {
        ledcSetup(channel_PWM_L1, freq_PWM, resolution_PWM); // 设置舵机通道
        ledcAttachPin(l, channel_PWM_L1);  //将 LEDC 通道绑定到指定 IO 口上以实现输出
        this->l_channel = channel_PWM_L1;
        ledcWrite(this->l_channel, 0);

        ledcSetup(channel_PWM_L2, freq_PWM, resolution_PWM); // 设置舵机通道
        ledcAttachPin(r, channel_PWM_L2);  //将 LEDC 通道绑定到指定 IO 口上以实现输出
        this->r_channel = channel_PWM_L2;
        ledcWrite(this->r_channel, 0);

        printf("init wheel: %d %d %d %d %d %d %d\n",
               l, r, which_wheel,
               this->l_channel, this->r_channel,
               ledcRead(this->l_channel), ledcRead(this->r_channel));
    } else if (which_wheel == 1) {
        ledcSetup(channel_PWM_R1, freq_PWM, resolution_PWM); // 设置舵机通道
        ledcAttachPin(l, channel_PWM_R1);  //将 LEDC 通道绑定到指定 IO 口上以实现输出
        this->l_channel = channel_PWM_R1;
        ledcWrite(this->l_channel, 0);

        ledcSetup(channel_PWM_R2, freq_PWM, resolution_PWM); // 设置舵机通道
        ledcAttachPin(r, channel_PWM_R2);  //将 LEDC 通道绑定到指定 IO 口上以实现输出}
        this->r_channel = channel_PWM_R2;
        ledcWrite(this->r_channel, 0);

        printf("init wheel: %d %d %d %d %d %d %d\n",
               l, r, which_wheel,
               this->l_channel, this->r_channel,
               ledcRead(this->l_channel), ledcRead(this->r_channel));
    }
    printf("all channel: %d %d %d %d \n",  ledcRead(3), ledcRead(4), ledcRead(5), ledcRead(6));
}

void wheel::set_throttle(float throttle) {
    this->throttle = throttle;
    if (throttle == 0.0f) {
        ledcWrite(this->l_channel, 0);
        ledcWrite(this->r_channel, 0);
    } else {
        float pwm = std::max(0.0f, std::min(std::fabs(throttle), 1.0f));
        int real_pwm = (int) (pwm * 1024);
        printf("%s wheel, set pwm, %d \n", this->which_wheel == 0 ? "left" : "right", real_pwm);
        if (throttle > 0) {
            ledcWrite(this->l_channel, real_pwm);
            ledcWrite(this->r_channel, 0);
        } else {
            ledcWrite(this->l_channel, 0);
            ledcWrite(this->r_channel, real_pwm);
        }
        printf("get pwm, %d, %d \n", ledcRead(this->l_channel), ledcRead(this->r_channel));
    }
    printf("all channel: %d %d %d %d \n",  ledcRead(3), ledcRead(4), ledcRead(5), ledcRead(6));
}


