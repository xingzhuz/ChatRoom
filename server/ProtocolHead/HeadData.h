
#pragma once
#include "protocolmsg.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>

using namespace std;

// HeadData 类用于解析接收到的协议头数据，包括协议号、账号、数据类型、数据长度等信息
class HeadData
{
    // 协议格式：协议号(1B) - 账号(2B) - 数据类型(1B) - 数据长度(4B) - 数据
private:
    char buffer[BASE_BUFFER_SIZE]; // 用于存储接收到的数据缓冲区
    const char *bp;                // 数据解析的指针，指向当前解析位置
    unsigned int protocolId;       // 协议号
    unsigned int accountId;        // 账号
    unsigned int dataType;         // 数据类型
    unsigned int dataLength;       // 数据长度

    // 基本的解析操作，处理协议头中的各个字段
    void baseParse();

    // 用于解析指定长度的整数，处理网络字节序转换
    unsigned int parseInt(int len);

public:
    // 使用文件描述符初始化HeadData对象，通常是网络连接的socket描述符
    HeadData(int fd);

    // 默认构造函数
    HeadData();

    // 解析传入的缓冲区数据，并从中提取协议头信息
    void parse(const char *buffer);

    // 获取协议号
    unsigned int getProtocolId();

    // 获取账号
    unsigned int getAccount();

    // 获取数据类型
    unsigned int getDataType();

    // 获取数据长度
    unsigned int getDataLength();
};