import time

import cv2 as cv

# 识别电脑摄像头并打开
cap = cv.VideoCapture(0)
time.sleep(1)
# 创建一个级联分类器 加载一个.xml分类器文件 它既可以是Haar特征也可以是LBP特征的分类器
face_detect = cv.CascadeClassifier(cv.data.haarcascades + 'haarcascade_frontalface_default.xml')

while True:
    # 读取视频片段q
    flag, frame = cap.read()
    frame = cv.flip(frame, 1)
    if not flag:  # 读完视频后falg返回False
        break
    # 灰度处理
    gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    # 多个尺度空间进行人脸检测   返回检测到的人脸区域坐标信息
    face_zone = face_detect.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5)
    print(face_zone)
    # 绘制矩形和圆形检测人脸
    for x, y, w, h in face_zone:
        cv.rectangle(frame, pt1=(x, y), pt2=(x + w, y + h), color=[0, 0, 255], thickness=2)
        cv.circle(frame, center=(x + w // 2, y + h // 2), radius=w // 2, color=[0, 255, 0], thickness=2)
    # 显示图片
    cv.imshow('video', frame)
    # 设置退出键q 展示频率
    if ord('q') == cv.waitKey(30):
        break

# 释放资源
cv.destroyAllWindows()
cap.release()
