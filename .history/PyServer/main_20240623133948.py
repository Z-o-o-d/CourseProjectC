from flask import Flask, render_template

app = Flask(__name__)

# 定义路由，当用户访问根路径时，返回一个简单的HTML页面
@app.route('/')
def index():
    return render_template('index.html')

if __name__ == '__main__':
    app.run(debug=True)
