import cv2
from cv2 import CAP_PROP_FPS
from flask import Flask, render_template, Response


class VideoCamera:
    def __init__(self):
        # 通过opencv获取实时视频流
        self.video = cv2.VideoCapture(0)

    def __del__(self):
        self.video.release()

    def get_frame(self):
        success, image = self.video.read()
        # 因为opencv读取的图片并非jpeg格式，因此要用motion JPEG模式需要先将图片转码成jpg格式图片
        ret, jpeg = cv2.imencode('.jpg', image)
        return jpeg.tobytes()


app = Flask(__name__)


@app.route('/')  # 主页
def index():
    # jinja2模板，具体格式保存在index.html文件中
    return render_template('index.html')


def gen(camera):
    while True:
        frame = camera.get_frame()
        # 使用generator函数输出视频流， 每次请求输出的content类型是image/jpeg
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n\r\n')


@app.route('/video')  # 这个地址返回视频流响应
def video():
    return Response(gen(VideoCamera()),
                    mimetype='multipart/x-mixed-replace; boundary=frame')


@app.route('/picture')  # 这个地址返回一张照片
def picture():
    response = Response(VideoCamera().get_frame(), mimetype='image/jpeg;')
    # 默认inline，filename表示下载内容时的推荐命名。attachment的话就是自动下载
    response.headers.add_header('Content-Disposition',
                                'inline; filename="picture.jpg"')
    return response


if __name__ == '__main__':
    app.run(host='0.0.0.0', threaded=True, debug=True, port=8889)
