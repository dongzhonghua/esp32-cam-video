//
// Created by zhonghua dong on 2022/5/1.
//

#include <cstdio>
#include "algorithm"
#include "two_wheels.h"

two_wheels::two_wheels() : left_wheel(2, 14, 0), right_wheel(15, 13, 1) {
}

void two_wheels::set_axis(float x, float y) {
    float left = x + y;
    float right = y - x;

    if (std::abs(left) < 0.1f) {
        left = 0.0f;
    }
    if (std::abs(right) < 0.1f) {
        right = 0.0f;
    }
    printf("set throttle, left: %f, right: %f \n", left, right);
    this->left_wheel.set_throttle(left);
    this->right_wheel.set_throttle(right);
}
