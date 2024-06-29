详细步骤
1. 连接ESP8266到Wi-Fi

使用串口调试工具（如Arduino IDE的串口监视器、PuTTY、CoolTerm等）与ESP8266通信。设置波特率为115200（或根据你的固件设置），选择正确的COM端口。

    复位ESP8266：

AT+RST

等待复位完成并显示 ready。

设置ESP8266为无线终端模式：

AT+CWMODE=1

连接Wi-Fi：

arduino

AT+CWJAP="Your_SSID","Your_PASSWORD"

如果连接成功，会显示 WIFI CONNECTED 和 WIFI GOT IP。

获取IP地址：

objectivec

    AT+CIFSR

    记下ESP8266的IP地址。

2. 设置ESP8266为TCP服务器

    配置多连接模式：

    objectivec

AT+CIPMUX=1

创建TCP服务器：

objectivec

    AT+CIPSERVER=1,80

3. 配置ESP8266发送网页内容

当浏览器连接到ESP8266的80端口时，ESP8266将发送一个简单的HTML网页。这里是一个例子：

    等待连接请求：
    当浏览器请求连接时，ESP8266会返回类似如下信息：

    makefile

+IPD,0,438:GET / HTTP/1.1
Host: 192.168.1.100
Connection: keep-alive
...

发送网页内容：
假设我们要发送以下HTML网页：

html

<html>
<head><title>ESP8266 Web Server</title></head>
<body><h1>Hello from ESP8266!</h1></body>
</html>

我们可以将其作为HTTP响应发送回浏览器。

发送AT命令：
首先计算响应头和HTML内容的总长度：

php

HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
\r\n
<html>
<head><title>ESP8266 Web Server</title></head>
<body><h1>Hello from ESP8266!</h1></body>
</html>

计算字节长度：

php

"HTTP/1.1 200 OK\r\n"                -> 17 bytes
"Content-Type: text/html\r\n"        -> 25 bytes
"\r\n"                               -> 2 bytes
"<html><head><title>ESP8266 Web Server</title></head><body><h1>Hello from ESP8266!</h1></body></html>" -> 93 bytes

总长度为：

python

17 + 25 + 2 + 93 = 137 bytes

发送AT命令：

objectivec

AT+CIPSEND=0,137

等待响应 >，然后发送以下HTTP响应：

php

HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
\r\n
<html>
<head><title>ESP8266 Web Server</title></head>
<body><h1>Hello from ESP8266!</h1></body>
</html>

关闭连接：

objectivec

    AT+CIPCLOSE=0

完整示例

以下是一个完整的示例，假设你已经连接到Wi-Fi并创建了TCP服务器：

    复位ESP8266：

AT+RST

设置无线终端模式：

AT+CWMODE=1

连接到Wi-Fi：

arduino

AT+CWJAP="Your_SSID","Your_PASSWORD"

获取IP地址：

objectivec

AT+CIFSR

设置多连接模式：

objectivec

AT+CIPMUX=1

创建TCP服务器：

objectivec

AT+CIPSERVER=1,80

等待连接请求并发送响应：

makefile

+IPD,0,438:GET / HTTP/1.1
Host: 192.168.1.100
Connection: keep-alive
...

发送HTTP响应和HTML内容：

objectivec

AT+CIPSEND=0,137

等待响应 >，然后发送：

php

HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
\r\n
<html>
<head><title>ESP8266 Web Server</title></head>
<body><h1>Hello from ESP8266!</h1></body>
</html>

关闭连接：

objectivec

    AT+CIPCLOSE=0

通过这些步骤，你可以使用ESP8266作为TCP服务器，当浏览器连接到其80端口时，它会发送一个简单的网页。确保发送的总字节数计算正确，避免出现长度错误。