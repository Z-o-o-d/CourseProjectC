from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/sensor-data', methods=['POST'])
def receive_sensor_data():
    if request.method == 'POST':
        data = request.form['sensor_data']
        print(f"Received sensor data: {data}")
        # 在这里可以添加处理接收到的数据的逻辑，比如存储到数据库等
        return jsonify({'message': 'Data received successfully'})

if __name__ == '__main__':
    app.run(debug=True, port=5000)
