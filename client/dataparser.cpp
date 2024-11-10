#include "dataparser.h"

DataParser::DataParser(char *buffer)
{
    bp = buffer;
}

bool DataParser::baseParse()
{
    this->protocolId = parseInt(PROTOCOL_ID_SIZE);
    this->accountId = parseInt(ACCOUNT_SIZE);
    this->dataType = parseInt(DATA_TYPE_SIZE);
    this->dataLength = parseInt(DATA_SIZE);

    // 假设parseInt函数返回值为0时表示解析失败
    if (protocolId == 0 || accountId == 0 || dataType == 0 || dataLength == 0)
    {
        return false;  // 如果有任何一个字段解析失败，返回false
    }

    return true;  // 所有字段解析成功，返回true
}

// 按小端解析数据
unsigned int DataParser::parseInt(int len)
{
    unsigned int sum = 0;
    unsigned int i = 0;

    for (char *end = bp + len - 1; bp <= end; end--)
    {
        sum += static_cast<unsigned int>(static_cast<unsigned char>(*end)) << i;
        i += 8;
    }

    bp = bp + len;
    return sum;
}

unsigned int DataParser::getProtocolId()
{
    return this->protocolId;
}

unsigned int DataParser::getAccount()
{
    return this->accountId;
}

unsigned int DataParser::getDataType()
{
    return this->dataType;
}

unsigned int DataParser::getDataLength()
{
    return this->dataLength;
}

DataParser::~DataParser()
{
}
