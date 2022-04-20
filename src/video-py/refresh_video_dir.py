import datetime
import os

from apscheduler.schedulers.background import BackgroundScheduler

root_path = "/tmp/监控/"


def get_dir_size(dir):
    size = 0
    for root, dirs, files in os.walk(dir):
        size += sum([os.path.getsize(os.path.join(root, name)) for name in files])
    return size


def refresh_video_dirs(days_to_live, max_size):
    # 打印当前日期
    print(datetime.datetime.now())
    # 删除过期文件夹
    listdir = os.listdir(root_path)
    for d in listdir:
        if (datetime.datetime.strptime(d, "%Y-%m-%d") - datetime.datetime.today()).days < -days_to_live:
            del_dir = root_path + d
            os.rmdir(del_dir)
            print('过期文件夹已删除: ' + del_dir)

    # 判断是否超过文件夹阈值
    size = get_dir_size(root_path) / 1024 / 1024 / 1024
    print('Total size is: %.2f GB' % size)
    if size >= max_size:
        listdir.sort()
        del_dir = root_path + listdir[0]
        os.rmdir(del_dir)
        print('存储超过限制，文件夹已删除: ' + del_dir)

    # 创建文件夹，要多一天，避免下一天创建的时候没有文件夹
    time_now = datetime.date.today()
    dirs = []
    for i in range(2):
        temp = time_now + datetime.timedelta(days=i)
        dir_name = root_path + str(temp)
        dirs.append(dir_name)
        if not os.path.exists(dir_name):
            os.makedirs(dir_name)
            print('文件夹创建完成: ' + dir_name)


def refresh_video_job():
    schedule = BackgroundScheduler()
    # 周期任务，每6小时执行一次
    schedule.add_job(refresh_video_dirs, "interval", hours=1, args=[7, 10])
    schedule.start()


if __name__ == "__main__":
    refresh_video_job()
