import datetime
from urllib import request

import cv2
import numpy as np

from refresh_video_dir import refresh_video_job, root_path


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
            frame = cv2.imdecode(np.fromstring(jpg, dtype=np.uint8), cv2.IMREAD_UNCHANGED)
            cv2.imshow("video stream", frame)
            yield frame


if __name__ == "__main__":
    for frame in read_from_mjpg_stream():
        # cv2.imshow("video stream", frame)
        if cv2.waitKey(1) == 27:
            exit(0)
        if cv2.waitKey(1) & 0xFF == ord('q'):  # q退出
            break
    cv2.destroyAllWindows()
