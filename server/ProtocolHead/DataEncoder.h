
#pragma once
#include "protocolmsg.h"
#include <string>

using namespace std;

// DataEncoder 类用于对数据进行编码，通常用于网络传输前的数据封装。
// 网络中传输的数据大多是大端即网络字节序，主机之间处理是小端数据

class DataEncoder
{
private:
    char head[BASE_BUFFER_SIZE]; // 用于存储数据头的字符数组，大小为 BASE_BUFFER_SIZE
    char *hp;                    // 指向数据头的指针，用于操作 head 数组

    // 用于编码一个元素（如协议标识符、账户等），编码成大端数据，编码成一个字符串
    void encodeElement(unsigned int data, unsigned int len);

public:
    DataEncoder();

    // 用于编码整个数据包，包含协议 ID、账户、数据类型和数据长度
    string encode(unsigned int protocolId, unsigned int accountId, unsigned int dataType, unsigned int dataLength);
};