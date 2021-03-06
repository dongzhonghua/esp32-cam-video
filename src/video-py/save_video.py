import datetime
import time
from urllib import request

import cv2
import numpy as np

from refresh_video_dir import refresh_video_job, root_path


def read_from_mjpg_stream():
    url = "http://localhost:8889/video"
    stream = request.urlopen(url)
    bytes = b""
    max_retry = 0
    while True:
        tmp = stream.read(1024)
        print(len(tmp))
        if len(tmp) <= 1:  # 如果服务端重启，则等待一段时间重新连接
            max_retry = max_retry + 1
            print(max_retry)
            if max_retry > 100:
                time.sleep(1)
                try:
                    stream = request.urlopen(url)
                except Exception:
                    print("服务端异常，连接不到服务器")
                    pass
                print("重新连接服务器")
                max_retry = 0
        bytes += tmp
        a = bytes.find(b"\xff\xd8")  # jpeg格式的图片以这两个字节开头以下面两个字节结尾
        b = bytes.find(b"\xff\xd9")
        if a != -1 and b != -1:
            jpg = bytes[a: b + 2]
            bytes = bytes[b + 2:]
            frame = cv2.imdecode(np.fromstring(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
            yield frame


if __name__ == "__main__":
    refresh_video_job()
    cur_hour = datetime.datetime.now().hour
    file_path = root_path + str(datetime.date.today()) + "/" + str(cur_hour) + ".mp4"
    print("初始化文件: " + file_path)
    out = cv2.VideoWriter(file_path, cv2.VideoWriter_fourcc(*"mp4v"), 24, (1280, 720))  # 写入视频
    for frame in read_from_mjpg_stream():
        now = datetime.datetime.now()
        temp_hour = now.hour
        if now.minute == 0 and cur_hour != temp_hour:  # 每小时换一个文件存储视频
            file_path = root_path + str(datetime.date.today()) + "/" + str(temp_hour) + ".mp4"
            cur_hour = temp_hour
            print("更换文件: " + file_path)
            out.release()
            out = cv2.VideoWriter(file_path, cv2.VideoWriter_fourcc(*"mp4v"), 24, (1280, 720))  # 写入视频
        out.write(frame)  # 写入帧
        cv2.imshow("video stream", frame)
        if cv2.waitKey(1) == 27:
            exit(0)
        if cv2.waitKey(1) & 0xFF == ord('q'):  # q退出
            break
    out.release()
    cv2.destroyAllWindows()
