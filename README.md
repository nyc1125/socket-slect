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
    serMessageHeader.hpp/cliMessageHeader.hpp : 自定义消息头和消息体,内容应该一致,因为分开开发所以各自保留了一份(命令定义也放在这里面了)   

## server:  
    server.h/server.cpp : 服务器封装成Server类  
    
    CellServer.hpp : 服务器子线程类，Server类用来接受客户端连接，CellServer类用来处理客户端消息  
    
    ClientSocket.hpp : 封装服务器接受到的客户端  

    CELLTimestamp.hpp : 计时器类，用来统计每秒钟服务器接收到包的数量
    
    main.cpp : 程序入口  
## client:  
    client.hpp : 服务器封装成Client类   
    
    client.cpp : 程序入口  

# 该服务器在我的电脑上:(windows 环境下)测试数据：
 
    1. 1个子线程，4000个客户端同时连接服务器，3秒钟左右4000个客户端能全部连接成功，每秒钟服务器处理 2w-2.5w 个包左右（每个包100byte）
 
    2. 4个子线程，4000个客户端同时连接服务器，3秒钟左右4000个客户端能全部连接成功，每秒钟服务器处理 14w 个包左右（每个包100byte）
 
    3. 1个子线程，4000个客户端同时连接服务器，3秒钟左右4000个客户端能全部连接成功，每秒钟服务器处理 1.7w-2w 个包左右(每个包1kb)
 
    4. 4个子线程，4000个客户端同时连接服务器，3秒钟左右4000个客户端能全部连接成功，每秒钟服务器处理 10w 个包左右（每个包1kb）
