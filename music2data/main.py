from pydub import AudioSegment
import numpy as np
import matplotlib.pyplot as plt

# 简谱与频率对应的字典，频率以整数表示（单位：Hz * 100）
note_freq = {
    'C2': 65, 'C#2/Db2': 69, 'D2': 73, 'D#2/Eb2': 78, 'E2': 82, 'F2': 87, 'F#2/Gb2': 92, 'G2': 98, 'G#2/Ab2': 104,
    'A2': 110, 'A#2/Bb2': 117, 'B2': 123, 'C3': 130, 'C#3/Db3': 138, 'D3': 146, 'D#3/Eb3': 155, 'E3': 164, 'F3': 174,
    'F#3/Gb3': 185, 'G3': 196, 'G#3/Ab3': 208, 'A3': 220, 'A#3/Bb3': 233, 'B3': 246, 'C4': 261, 'C#4/Db4': 277,
    'D4': 293, 'D#4/Eb4': 311, 'E4': 329, 'F4': 349, 'F#4/Gb4': 370, 'G4': 392, 'G#4/Ab4': 415, 'A4': 440, 'A#4/Bb4': 466,
    'B4': 493, 'C5': 523, 'C#5/Db5': 554, 'D5': 587, 'D#5/Eb5': 622, 'E5': 659, 'F5': 698, 'F#5/Gb5': 740, 'G5': 784,
    'G#5/Ab5': 831, 'A5': 880, 'A#5/Bb5': 932, 'B5': 987
}

# 加载MP3文件
audio_file = "recording.mp3"
audio = AudioSegment.from_file(audio_file)

# 将MP3音频文件转换为numpy数组
data = np.array(audio.get_array_of_samples())

# 显示音频波形图（可选）
plt.figure(figsize=(14, 5))
plt.plot(data)
plt.title('Audio Waveform')
plt.xlabel('Sample')
plt.ylabel('Amplitude')
plt.show()

# 设置采样率和时间间隔（根据需要调整）
sample_rate = audio.frame_rate  # 采样率
time_per_note = 0.2  # 每个音符的持续时间（秒）

# 按时间间隔截取音频片段，并转换为对应的简谱音符序列
notes = []
start_time = 0
while start_time < len(data):
    end_time = int(start_time + sample_rate * time_per_note)
    segment = data[start_time:end_time]

    # 计算该段音频的平均振幅作为音量
    volume = np.mean(np.abs(segment))

    # 根据音量确定音符（示例：简单地选择振幅最大的频率作为音符）
    max_amplitude = np.max(np.abs(segment))
    freq = note_freq['C2']  # 默认为C2
    for note, f in note_freq.items():
        if np.abs(f - max_amplitude) < np.abs(freq - max_amplitude):
            freq = f

    # 将频率转换为对应的简谱音符并添加到列表中
    for note, f in note_freq.items():
        if f == freq:
            notes.append(note)
            break

    start_time = end_time

# 输出乐谱（音符序列）
print("乐谱（音符序列）:")
print(notes)
