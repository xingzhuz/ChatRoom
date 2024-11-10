
#include "DataEncoder.h"

DataEncoder::DataEncoder()
{
}

// 编码成大端数据，编码成一个字符串
void DataEncoder::encodeElement(unsigned int data, unsigned int len)
{
    // 将指针 c 指向 hp 加上 len-1 的位置，即从最后一个字节开始写入
    char *c = hp + len - 1;

    // 循环将数据的每一个字节依次存储到缓冲区中
    for (int i = len; i > 0; i--)
    {
        // 将 `data` 的最低 8 位（即一个字节）存入当前位置
        *c = (char)(data & 0xff);

        // 移动指针到前一个字节位置
        c--;

        // 右移 `data`，将下一个字节移到最低 8 位
        data >>= 8;
    }

    // 更新 hp 指针，移动到下一个可以写入数据的位置
    hp = hp + len;
}

// 用于编码整个数据包，包含协议ID、账户、数据类型和数据长度
string DataEncoder::encode(unsigned int protocolId, unsigned int accountId, unsigned int dataType, unsigned int dataLength)
{
    hp = head;
    encodeElement(protocolId, PROTOCOL_ID_SIZE);
    encodeElement(accountId, ACCOUNT_SIZE);
    encodeElement(dataType, DATA_TYPE_SIZE);
    encodeElement(dataLength, DATA_SIZE);
    return string(head, sizeof(head)); // 并将 head 数组中的前 sizeof(head) 个字符（包括其中的空字符）转换为字符串
}
