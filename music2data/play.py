import winsound
import time

# 简谱与频率对应的字典，频率以整数表示（单位：Hz * 100）
note_freq = {
    'C4': 261, 'D4': 293, 'E4': 329, 'F4': 349, 'G4': 392, 'A4': 440, 'B4': 493,
    'C5': 523, 'D5': 587, 'E5': 659, 'F5': 698, 'G5': 784, 'A5': 880, 'B5': 987
}

# 简化的《See You Again》乐谱（音符序列）
score_see_you_again = ['E4', 'G4', 'A4', 'E5', 'D5', 'C5', 'B4', 'C5', 'E4', 'G4', 'A4', 'E5', 'D5', 'C5', 'B4', 'A4']

# 播放乐谱的函数
def play_score(score):
    for note in score:
        if note in note_freq:
            frequency = note_freq[note]
            duration = 300  # 每个音符的持续时间（毫秒）
            winsound.Beep(frequency, duration)
            time.sleep(0.1)  # 可选的延迟，用于音符之间的间隔

# 播放《See You Again》的简化乐谱
play_score(score_see_you_again)
