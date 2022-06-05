import time

import requests
from matplotlib import pyplot as plt
from simple_pid import PID


def update_throttle(throttle, cur_pos):
    url = "http://192.168.31.218/axis"
    response = requests.get(url, params={"x": 0, "y": -0})
    print(throttle, response.text)
    if throttle > 0:
        return cur_pos + 1
    else:
        return cur_pos - 1


if __name__ == '__main__':
    # 设置PID的三个参数，以及限制输出
    pid = PID(2, 0.01, 0.1, setpoint=0, output_limits=(-1, 1), sample_time=0.1)
    # 用于设置时间参数
    start_time = time.time()
    last_time = start_time
    cur_pos = -10
    # 用于输出结果可视化
    setpoint, y, x = [], [], []
    # 设置系统运行时间
    while time.time() - start_time < 10:

        # 设置时间变量dt
        current_time = time.time()
        dt = (current_time - last_time)

        # 变量temp在整个系统中作为输出，变量temp与理想值之差作为反馈回路中的输入，通过反馈回路调节变量power的变化。
        throttle = pid(cur_pos)
        cur_pos = update_throttle(throttle, cur_pos)
        # 用于输出结果可视化
        x += [current_time - start_time]
        y += [cur_pos]
        setpoint += [pid.setpoint]
        # 用于变量temp赋初值
        if current_time - start_time > 0:
            pid.setpoint = 0

        last_time = current_time

    # 输出结果可视化
    plt.plot(x, setpoint, label='target')
    plt.plot(x, y, label='PID')
    plt.xlabel('time')
    plt.ylabel('temperature')
    plt.legend()
    plt.show()
