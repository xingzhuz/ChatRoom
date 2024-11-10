#include "client.h"

Client::Client()
{
}

QJsonObject Client::readServerMsg()
{
    // 读取消息头，获取消息的长度等信息
    readHeadData();

    // 从服务器读取具体的数据内容，存入 buffer 中
    tcpClient.read(buffer, m_dataLength);

    // 将接收到的数据从 char* 转换为 QString
    QString serverMsg = QString::fromStdString(std::string(buffer, m_dataLength));

    // 将 QString 格式的 JSON 字符串解析为 QJsonObject
    QJsonObject serverInfoObj = QJsonDocument::fromJson(serverMsg.toUtf8()).object();

    // 返回解析后的 QJsonObject 对象，包含服务器的消息内容
    return serverInfoObj;
}

void Client::readHeadData()
{
    tcpClient.read(buffer, BASE_BUFFER_SIZE);
    DataParser parser(buffer);
    parser.baseParse();
    m_protocolId = parser.getProtocolId();
    m_accountId = parser.getAccount();
    m_dataType = parser.getDataType();
    m_dataLength = parser.getDataLength();
}

// 发送文本数据，包括头部数据和实际的文本内容
// accountId:账户ID，text:实际的文件数据，protocolId:协议ID
void Client::writeText(unsigned int accountId, string text, unsigned int protocolId)
{
    DataEncoder encoder;
    qDebug() << "------------输入的字节数-------------" << text.length();
    string headStr = encoder.encode(protocolId, accountId, TEXT, static_cast<unsigned int>(text.length()));

    tcpClient.write(headStr.data(), static_cast<qint64>(headStr.length()));

    qDebug() << "------------头部大小-------------" << headStr.length();

    if (text.length() != 0)
    {
        tcpClient.write(text.data(), static_cast<qint64>(text.length()));
    }

    qDebug() << "------------发送成功,总数据大小-------------" << text.length();
}

QTcpSocket *Client::getTcpClient()
{
    return &tcpClient;
}

void Client::closeTcpSocket()
{
    tcpClient.disconnectFromHost();
    tcpClient.close();
}

Client::~Client()
{
}
