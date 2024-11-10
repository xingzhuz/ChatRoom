#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <arpa/inet.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/epoll.h>
#include <json/json.h>
#include "ProtocolHead/HeadData.h"
#include "Service/DataProcesser.h"
#include "Service/UserService.h"
#include "Service/Online.h"
#include "config/server_config.h"
using namespace std;

int main()
{
    // 创建监听socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET表示IPv4，SOCK_STREAM表示TCP协议
    sockaddr_in serverAddr{}, clientAddr{};    // 服务器和客户端地址结构体
    int opt = 1;

    // 设置端口复用选项
    if (-1 == setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        cout << "setsockopt fail" << endl;
        exit(-1);
    }

    // 创建epoll实例
    int epfd = epoll_create(1);
    epoll_event ev, events[MAX_CONNECTIONS]; // epoll事件结构体
    ev.data.fd = lfd;                        // 将监听socket文件描述符添加到epoll
    ev.events = EPOLLIN;                     // 设置监听的事件为EPOLLIN（可读）

    // 将监听socket加入epoll事件管理
    if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev))
    {
        cout << "epoll_ctl fail" << endl;
        exit(-1);
    }

    // 配置服务器地址和端口
    serverAddr.sin_port = htons(PORT);              // 端口号，使用htons转换为网络字节序
    serverAddr.sin_family = AF_INET;                // IPv4地址
    inet_pton(AF_INET, HOST, &serverAddr.sin_addr); // 主机字节序的IP地址转换为网络字节序

    // 绑定socket到指定的地址和端口
    if (-1 == bind(lfd, (sockaddr *)&serverAddr, sizeof(serverAddr)))
    {
        cout << "bind fail" << endl;
        exit(-1);
    }

    // 开始监听客户端请求
    if (-1 == listen(lfd, MAX_CONNECTIONS))
    {
        cout << "listen fail" << endl;
        exit(-1);
    }

    cout << "listening..." << endl;

    UserService us; // 用户服务实例
    Online online;  // 在线用户管理实例

    // 主循环，监听客户端的请求
    while (true)
    {
        // 等待epoll事件发生，阻塞直到有事件触发
        int nready = epoll_wait(epfd, events, MAX_CONNECTIONS, -1);
        if (nready < 0)
        {
            cout << "epoll_wait error" << endl;
            exit(-1);
        }

        cout << "收到" << nready << "个请求" << endl;

        // 遍历所有已触发的事件
        for (int i = 0; i < nready; i++)
        {
            int fd = events[i].data.fd; // 获取触发事件的文件描述符

            if (fd == lfd) // 如果是监听socket，表示有新的连接请求
            {
                // 接受新的连接
                socklen_t len = sizeof(clientAddr);
                int cfd = accept(lfd, (sockaddr *)&clientAddr, &len);
                ev.data.fd = cfd;                         // 将新连接的socket添加到epoll管理
                ev.events = EPOLLIN;                      // 监听EPOLLIN事件（可读）
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev); // 添加事件到epoll

                // 如果在 1 秒内没有接收到客户端的数据，相关的读取操作将超时并返回
                struct timeval timeout = {1, 0};
                setsockopt(cfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
            }
            else if (events[i].events & EPOLLIN) // 如果是已连接的客户端socket，表示有数据可读
            {
                HeadData hd(fd);                              // 解析头部数据
                unsigned int protocolId = hd.getProtocolId(); // 获取协议ID
                unsigned int accountId = hd.getAccount();     // 获取账户ID
                unsigned int dataType = hd.getDataType();     // 获取数据类型
                unsigned int dataLength = hd.getDataLength(); // 获取数据长度
                DataProcesser dp;                             // 数据处理器实例

                switch (protocolId) // 根据协议ID处理不同的业务逻辑
                {
                case LOGIN: // 登录请求处理
                {
                    string loginMsg = dp.readTextContent(fd, dataLength); // 从客户端读取登录请求的消息内容
                    Json::Reader jsonReader;
                    Json::Value msg;
                    jsonReader.parse(loginMsg, msg); // 将消息解析为JSON对象，反序列化

                    string accountId = msg["accountId"].asString();              // 获取账号
                    string password = msg["password"].asString();                // 获取密码
                    pair<int, string> user = us.checkLogin(accountId, password); // 调用用户服务检查登录信息的正确性

                    Json::Value loginResult; // 用于构建登录结果的JSON对象
                    if (user.first != 0)     // 登录成功，user.first 是账户ID
                    {
                        if (online.isLogin(user.first)) // 检查用户是否已在线
                        {
                            loginResult["status"] = LOGIN_EXIST; // 用户已在线，返回冲突状态
                        }
                        else
                        {
                            online.appendUser(user);               // 添加用户到用户表中
                            online.appendLoginFd(user.first, fd);  // 添这个登录的文件描述符，用于后续关闭窗口
                            loginResult["status"] = LOGIN_SUCCESS; // 返回登录成功状态
                            loginResult["username"] = user.second; // 用户名
                        }
                    }
                    else // 登录失败
                    {
                        loginResult["status"] = LOGIN_FAIL; // 返回登录失败状态
                    }

                    string loginResultStr = loginResult.toStyledString(); // 将登录结果转换为字符串
                    dp.writeMsg(fd, 0, loginResultStr);                   // 将结果发送回客户端
                }
                break;

                case REGISTER: // 注册请求处理
                {
                    string registerMsg = dp.readTextContent(fd, dataLength); // 从客户端读取注册请求的消息内容
                    Json::Reader jsonReader;
                    Json::Value registerResult; // 用于构建注册结果的JSON对象
                    Json::Value msg;
                    jsonReader.parse(registerMsg, msg); // 将消息解析为JSON对象

                    string accountId = msg["accountId"].asString(); // 获取账号
                    string username = msg["username"].asString();   // 获取用户名
                    string password = msg["password"].asString();   // 获取密码

                    // 检查账号是否已注册或注册是否成功
                    if (us.isRegistered(accountId) || !us.registerUser(accountId, username, password))
                    {
                        registerResult["status"] = REGISTER_FAIL; // 注册失败
                    }
                    else
                    {
                        registerResult["status"] = REGISTER_SUCCESS; // 注册成功
                    }

                    // 将注册结果发送回客户端
                    dp.writeMsg(fd, 0, registerResult.toStyledString(), REGISTER);
                }
                break;

                case SEND: // 消息发送处理
                {
                    string baseMsg = online.getUserName(accountId) + "(" + to_string(accountId) + ")说:"; // 构建消息前缀
                    if (dataType == TEXT)                                                                 // 文本消息处理
                    {
                        dp.writeTextToAllUser(online.getAllChatFd(), accountId, baseMsg); // 向所有用户发送消息表头前缀
                        string content = dp.readTextContent(fd, dataLength);              // 读取消息内容
                        dp.writeTextToAllUser(online.getAllChatFd(), accountId, content); // 向所有用户广播消息内容
                    }
                    else if (dataType == IMAGE) // 图片消息处理
                    {
                        string imagePath = dp.readImageContent(fd, dataLength); // 读取图片

                        if (dp.getFileLength(imagePath) == dataLength) // 校验文件长度
                        {
                            dp.writeTextToAllUser(online.getAllChatFd(), accountId, baseMsg);    // 向所有用户广播消息前缀
                            dp.writeImageToAllUser(online.getAllChatFd(), accountId, imagePath); // 向所有用户发送图片
                        }
                        else // 文件不完整，处理异常
                        {
                            ev.data.fd = fd;
                            ev.events = EPOLLIN;
                            epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);                                                               // 从epoll监听中移除文件描述符
                            close(fd);                                                                                             // 关闭文件描述符
                            close(online.getChatFd(fd));                                                                           // 关闭对应的读取文件描述符
                            string logoutMsg = online.getUserName(accountId) + "(" + to_string(accountId) + ")" + "离开了聊天室!"; // 用户离开消息
                            online.removeUser(accountId);                                                                          // 从在线用户列表中移除用户
                            vector<int> fds = online.getAllChatFd();                                                               // 获取所有在线用户的文件描述符
                            if (!fds.empty())
                            {
                                dp.writeTextToAllUser(fds, accountId, logoutMsg, NOTICE);             // 通知所有用户该用户已退出
                                dp.writeTextToAllUser(fds, 0, online.getOnlineListStr(), ONLINELIST); // 更新在线用户列表
                            }
                        }
                    }
                }
                break;

                case UPDATE_DATA: // 服务器通知所有在线用户更新公告和在线列表
                {
                    ev.data.fd = fd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);                                                              // 从epoll移除该文件描述符
                    online.appendChatFd(accountId, fd);                                                                   // 添加读文件描述符
                    string loginMsg = online.getUserName(accountId) + "(" + to_string(accountId) + ")" + "走进了聊天室!"; // 用户进入消息
                    dp.writeTextToAllUser(online.getAllChatFd(), accountId, loginMsg, NOTICE);                            // 通知所有用户该用户已进入
                    dp.writeTextToAllUser(online.getAllChatFd(), accountId, online.getOnlineListStr(), ONLINELIST);       // 更新在线用户列表
                }
                break;

                case LOGOUT: // 客户端退出处理
                {
                    sleep(1); // 等待处理时间
                    ev.data.fd = fd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);                                                               // 从epoll监听中移除该文件描述符
                    close(fd);                                                                                             // 关闭文件描述符
                    close(online.getChatFd(fd));                                                                           // 关闭读取文件描述符
                    string logoutMsg = online.getUserName(accountId) + "(" + to_string(accountId) + ")" + "离开了聊天室!"; // 用户退出消息
                    online.removeUser(accountId);                                                                          // 从在线用户列表中移除用户
                    vector<int> fds = online.getAllChatFd();                                                               // 获取所有在线用户的文件描述符
                    cout << "当前在线人数:" << fds.size() << endl;                                                         // 打印当前在线人数
                    if (!fds.empty())
                    {
                        dp.writeTextToAllUser(fds, accountId, logoutMsg, NOTICE);             // 通知所有用户该用户已退出
                        dp.writeTextToAllUser(fds, 0, online.getOnlineListStr(), ONLINELIST); // 更新在线用户列表
                    }
                }
                break;

                case CLOSE: // 客户端关闭处理
                {
                    sleep(1); // 等待处理时间
                    ev.data.fd = fd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev); // 从epoll监听中移除该文件描述符
                    close(fd);                               // 关闭文件描述符
                }
                break;

                default: // 未知协议处理
                    break;
                }
            }
        }
    }

    return 0;
}
