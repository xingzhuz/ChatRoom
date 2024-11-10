#include "dataencoder.h"
#include <QDebug>
DataEncoder::DataEncoder()
{
}

// 转换为大端，即网络字节序
void DataEncoder::encodeElement(unsigned int data, unsigned int len)
{
    char *c = hp + len - 1;

    for (unsigned int i = len; i > 0; i--)
    {
        *c = static_cast<char>(data & 0xFF);
        c--;
        data >>= 8;
    }

    hp = hp + len;
}

string DataEncoder::encode(unsigned int protocolId, unsigned int accountId, unsigned int dataType, unsigned int dataLength)
{
    hp = head;
    encodeElement(protocolId, PROTOCOL_ID_SIZE);
    encodeElement(accountId, ACCOUNT_SIZE);
    encodeElement(dataType, DATA_TYPE_SIZE);
    encodeElement(dataLength, DATA_SIZE);
    return string(head, sizeof(head));
}

