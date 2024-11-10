
#include "HeadData.h"

void HeadData::baseParse()
{
    this->protocolId = parseInt(PROTOCOL_ID_SIZE);
    this->accountId = parseInt(ACCOUNT_SIZE);
    this->dataType = parseInt(DATA_TYPE_SIZE);
    this->dataLength = parseInt(DATA_SIZE);
}

// 解析指定长度的整数，并返回其值。
// 这个方法用于从字节流中提取一个整数，处理字节序的转换（通常是大端字节序转小端字节序）。
unsigned int HeadData::parseInt(int len)
{
    unsigned int sum = 0; // 用于存储解析后的整数值
    unsigned int i = 0;   // 用于跟踪每个字节的偏移量

    // 从字节流的末尾开始解析字节，直到解析完指定长度的所有字节
    for (const char *end = bp + len - 1; bp <= end; end--)
    {
        // 将当前字节（*end）转换为无符号字符，并通过位移操作拼接到结果中
        sum = sum + (((unsigned int)((unsigned char)(*end))) << i);
        i += 8; // 每次处理一个字节，偏移量增加8位
    }

    // 更新解析指针，跳过已解析的字节
    bp = bp + len;

    // 返回解析后的整数值
    return sum;
}

HeadData::HeadData(int fd)
{
    read(fd, buffer, BASE_BUFFER_SIZE);
    bp = buffer;
    baseParse();
}

HeadData::HeadData()
{
}

void HeadData::parse(const char *buffer)
{
    bp = buffer;
    baseParse();
}

unsigned int HeadData::getProtocolId()
{
    return this->protocolId;
}

unsigned int HeadData::getAccount()
{
    return this->accountId;
}

unsigned int HeadData::getDataType()
{
    return this->dataType;
}

unsigned int HeadData::getDataLength()
{
    return this->dataLength;
}
