import sounddevice as sd
import wavio
import multiprocessing
import time

def record_audio(duration, filename, fs=16000, channels=1):
    while True:  # 无限循环录音
        recording = sd.rec(int(duration * fs), samplerate=fs, channels=channels, dtype='int16')
        sd.wait()
        wavio.write(filename, recording, fs, sampwidth=2)
        print(f"完成录制: {filename}")

if __name__ == "__main__":
    duration = 10  # 每次录制时长（秒）

    # 创建进程列表

    processes = []

    # 启动n个录音进程
    for i in range(1):
        time.sleep(0.2)
        filename = f"recording.mp3"
        p = multiprocessing.Process(target=record_audio, args=(duration, filename))
        p.start()
        processes.append(p)
        print(f"录制线程启动: {i}")

    # 等待所有进程完成
    for p in processes:
        p.join()

    print("所有录音完成")
