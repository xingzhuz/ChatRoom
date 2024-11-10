# 基于C++和epoll实现的聊天室

## 简介

- 实现的功能
   - 注册
   - 单点登录
   - 登出
   - 群聊(支持文本和图片的传送)
   - 上线下线公告
   - 在线用户记录

- 客户端使用方式：
  - ./client文件夹是客户端源码，若安装了QT，则可打开chatroom.pro
  - 若在QT中构建项目，请在`构建`的文件夹下新建一个image文件夹用于保存图片，另外新建一个config文件夹，再在config文件夹中新建一个server.conf用于配置服务端IP和端口，server.conf的格式按照./client/config/server.conf填写
  
- 服务端使用方式
  - 1.进入./server/config目录，将服务器的IP和端口等配置信息换成适合自己的
  - 2.打开./server/config/mysql_config.h，将数据库IP(即数据库服务器所在的IP地址)，port(默认3306)修改为自己的，完善其余选项
  - 3.创建一个数据库，如chatroom
  - 2.利用./user.sql在MySQL中创建一张user表，或者复制其中建表的语法，收到创建一个user表
  - 3.进入./server/, 然后make即可
  - 4.运行步骤生成的可执行文件(./main)

- 通信协议
  - 开始1B表示这个数据包是协议Id，如登录请求、发送请求、登出请求等
  - 接下来2B表示用户的账号Id
  - 接下来1B表示数据包的数据格式，文本或图片
  - 接下来4B表示数据的大小
  - 最后就是真实数据了


## 简略介绍(详细解释查看Explain.md)

- 客户端和服务器端的数据交流，首先都需要先发送数据头，包括了协议ID(`LOGIN, SEND`等)、账户ID、数据类型(`IMAGE, TEXT`)、发送的数据的大小
- 发送的数据头使用编码的方式生成一个字符串，按照协议ID编码成`1B`，账户ID编码成`2B`，数据类型`1B`，数据大小`4B`，总共`8B`的长度
- 数据头的封装使用大端的方式编码，解码按照小端的方式编码，大端即网络字节序，更符合通信的协议，小端即主机字节序，便于主机之间的信息处理。
- 确保数据的完整性，服务器发送数据给客户端，设置了重传机制，确保客户端接收的数据完整
- 让所有用户都显示聊天内容，如图片：是通过客户端发送图片后，服务器接收到，保存在服务器目录中，然后向所有在线用户客户端进行发送数据，客户端做对应的接收显示；文本内容则是服务器临时保存，然后广播发送给所有用户。
- 服务器使用了`epoll`方式监听文件描述符事件的产生，客户端使用QT做的界面，使用信号槽监听事件的产生
  

## 效果展示



登录界面：

<img src="https://bu.dusays.com/2024/11/10/6730a6c9366ae.png" alt="登录界面" width="300" height="200">
<img src="https://bu.dusays.com/2024/11/10/6730a6cd13fa7.png" alt="注册界面" width="300" height="200">
<img src="https://bu.dusays.com/2024/11/10/6730a6d7e5454.png" alt="聊天界面1" width="600" height="420">
<img src="https://bu.dusays.com/2024/11/10/6730a6de97621.png" alt="聊天界面2" width="600" height="420">

> 说明: fork 的这位大佬: [](https://github.com/cswen-scut/chatroom)，我在此基础上改进和修改，加注释

