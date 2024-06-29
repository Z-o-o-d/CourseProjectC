'''
Author: ZeEE 874074406@qq.com
Date: 2024-06-23 13:33:22
LastEditors: ZeEE 874074406@qq.com
LastEditTime: 2024-06-23 13:48:36
FilePath: \CourseProjectC\PyServer\main.py
Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
'''
from flask import Flask, render_template, jsonify
import json
from datetime import datetime

app = Flask(__name__)

@app.route('/')
def index():
    # 模拟WebSocket返回的JSON数据
    return render_template('index.html')

@app.route('/get_data')
def get_data():
    with open('websocket_message.txt', 'r') as file:
        data = json.load(file)
    return jsonify(data)
``
@app.route('/get_data1')
def get_data1():
    with open('VoiceprintRecognition.txt', 'r') as file:
        data = json.load(file)
    return jsonify(data)
    # 将数据传递给模板

if __name__ == '__main__':
    app.run(debug=True)