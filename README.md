# socket-slect
select高并发服务器

# 文件结构
## server:  
    bin: Server.exe 可直接运行 
  
    src: Server 源码  
## client:  
    bin: Server 启动后 Client.exe 可直接连接  
  
    src: Client 源码  

# 源码说明

## 公共头:
    Data_Server/Data_Client.h: 自定义消息头和消息体,内容应该一致,因为分开开发所以各自保留了一份  
    
    CMD_Server/CMD_Client.h: 自定义命令,内容一致  

## server:  
    ser_tcpSocket.h/cpp:服务器socket实现代码（初始版，未封装成类）  
    
    server.cpp:程序入口  
## client:  
    cli_tcpSocket.h/cpp:客户端socket实现代码（初始版，未封装成类）  
    
    client.cpp:程序入口,客户端需要输入命令时，打开 61 62 行代码（客户端发送线程），客户端运行时可以从键盘输入已经自定义的命令:login,logout,exit
