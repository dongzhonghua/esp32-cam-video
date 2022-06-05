import time
from urllib import request

import cv2
import cv2 as cv
import numpy as np

# 创建一个级联分类器 加载一个.xml分类器文件 它既可以是Haar特征也可以是LBP特征的分类器
from simple_pid import PID

face_detect = cv.CascadeClassifier(cv.data.haarcascades + 'haarcascade_frontalface_default.xml')


def read_from_mjpg_stream():
    stream = request.urlopen("http://192.168.31.218/mjpeg/1")
    bytes = b""
    while True:
        bytes += stream.read(1024)
        a = bytes.find(b"\xff\xd8")  # jpeg格式的图片以这两个字节开头以下面两个字节结尾
        b = bytes.find(b"\xff\xd9")
        if a != -1 and b != -1:
            jpg = bytes[a: b + 2]
            bytes = bytes[b + 2:]
            if len(jpg) == 0:
                continue
            np_jpg = np.fromstring(jpg, dtype=np.uint8)
            frame = cv2.imdecode(np_jpg, cv2.IMREAD_COLOR)
            frame = cv2.flip(frame, 0)
            # cv2.imshow("video stream", frame)
            yield frame


if __name__ == "__main__":
    #设置PID的三个参数，以及限制输出
    pid = PID(2, 0.01, 0.1, setpoint=0)
    pid.output_limits = (0, None)

    for frame in read_from_mjpg_stream():
        # 灰度处理
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        # 多个尺度空间进行人脸检测   返回检测到的人脸区域坐标信息
        face_zone = face_detect.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(100, 100))
        print(face_zone)
        image_center = frame.shape[1] / 2
        # 绘制矩形和圆形检测人脸
        for x, y, w, h in face_zone:
            cv.rectangle(frame, pt1=(x, y), pt2=(x + w, y + h), color=[0, 0, 255], thickness=2)
            cv.circle(frame, center=(x + w // 2, y + h // 2), radius=w // 2, color=[0, 255, 0], thickness=2)
            # 获取识别区域举例图片中心的举例
            if x > 0 and w > 0:
                distance = x + w / 2 - image_center
                print(distance)
        # 显示图片
        cv.imshow('video', frame)
        if cv2.waitKey(1) == 27:
            exit(0)
        if cv2.waitKey(1) & 0xFF == ord('q'):  # q退出
            break
    cv2.destroyAllWindows()
