# esp8266ATPackage
stm32通过wifi方式进行数据通信时通常采用ESP8266模块，最常用的方法就是通过AT指令与esp模块进行交互，本项目对常用的AT指令进行封装成**协议**，使用户调用简单的函数即可实现热点连接、服务器连接、开启热点、创建服务器等功能。并提供了两种数据发送模式，用户可根据项目需要选择101协议数据封装发送或无数据封装发送。数据解析也可以根据项目需要设置解析或不解析。协议具有很强的可扩展性，能够根据自己的项目需求对AT指令进行裁剪和扩展。

使用说明参考文章：[https://blog.csdn.net/qq_26024785/article/details/107352408](https://blog.csdn.net/qq_26024785/article/details/107352408)

###     Version 1.5
* 更新日期：2021-04-25
* 更新日志：
* 1.解决不解析数据时输出数据含有乱码的问题
* 2.解决wifi模块重复RST的问题
* 3.服务器模式下添加设备上下线提示
* 4.分解客户端/服务端初始化代码，实现station模式动态创建、断开服务器连接和ap模式动态建立、关闭服务器
* 5.优化配置项，功能设置全部放在头文件中

###     Version 1.0

* 更新日期：2020-7-12
* 版本功能：
* 1.客户端模式下自动连接WiFi热点  
* 2.客户端模式下自动连接服务器  
* 3.服务器模式下自动创建wifi热点  
* 4.服务器模式下自动开启TCP服务器
* 5.服务器模式下输出TCP服务器的IP地址及端口号，DISPLAY_AT_INFO为1的情况下  
* 6.可实时打开和关闭透传模式  
* 7.可实时更改接收数据的解析模式（需要手动更改dataAnalyzeFlag的值）
* 8.提供两种数据封装模式，101协议格式和无封装格式，分别通过调用sendData101、sendData函数实现
* 9.想起来了再写
* 发送模式及其性能（101协议封装为模式1，无封装为模式2）：
* 在非透传模式下，模式1最快300ms/次、模式2最快200ms/次
* 在透传模式下，模式1与模式2都能达到30ms/次，实测50ms/次时串口能够正常回显数据，小于50ms时可能会出错