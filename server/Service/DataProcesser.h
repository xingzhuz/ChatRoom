
#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include "../ProtocolHead/protocolmsg.h"
#include <fstream>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include "../Util/MyTime.h"
#include "../ProtocolHead/HeadData.h"
#include "../ProtocolHead/DataEncoder.h"

#define IMAGE_PATH "./image/"
#define TCP_BUFSIZ 8192
using namespace std;

class DataProcesser
{
    // 协议号(1B)-账号(2B)-数据类型(1B)-数据长度(4B)-数据
private:
    char buffer[TCP_BUFSIZ];

    // 检测套接字是否连接
    int checkSocketConnected(int sock);

public:
    DataProcesser();

    // 指定的文件描述符 `fd` 读取文本数据
    string readTextContent(int fd, unsigned int dataLength);

    // 指定的文件描述符 `fd` 读取图片数据并保存到文件
    string readImageContent(int fd, unsigned int dataLength);

    // 向指定文件描述符 `fd` 写入文本数据，发送数据头和文本内容，并接收确认信息
    void writeText(int fd, unsigned int accountId, string text, unsigned int protocolId = SEND);

    // 向指定文件描述符 `fd` 写入图像数据，发送数据头和图像内容，并接收确认信息
    void writeImage(int fd, unsigned int accountId, const string &imagePath);

    // 向指定文件描述符 `fd` 发送消息，包括数据头和文本内容
    void writeMsg(int fd, unsigned int accountId, string text, unsigned int protocolId = SEND);

    // 发送文本内容到所有用户
    void writeTextToAllUser(const vector<int> &fds, int accountId, const string &text, unsigned int protocolId = SEND);

    // 发送图片内容到所有用户
    void writeImageToAllUser(const vector<int> &fds, int accountId, const string &imagePath);

    // 获取文件长度
    int getFileLength(const string &fileName);
};
