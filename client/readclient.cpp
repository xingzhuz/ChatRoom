#include "mytime.h"
#include "readclient.h"

#include <QNetworkProxy>
#include <QThread>

ReadClient::ReadClient()
{
}

ReadClient *ReadClient::readClient = nullptr;
ReadClient *ReadClient::getTcpSocketClient()
{
    if (readClient == nullptr)
    {
        readClient = new ReadClient;
        unordered_map<string, string> configMap = ConfigReader::readServerConfig();
        qDebug() << "从配置文件读取的" << QString::fromStdString(configMap["HOST"]) << "->" << QString::fromStdString(configMap["PORT"]);

        readClient->hostName = QString::fromStdString(configMap["HOST"]);
        readClient->port = static_cast<quint16>(stoi(configMap["PORT"]));

        qDebug() << "------------正在连接服务器--------------";
        QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
        readClient->tcpClient.connectToHost(readClient->hostName, readClient->port);
        qDebug() << "------------连接服务器成功--------------";
    }

    return readClient;
}

// 接收服务器的数据
bool ReadClient::readData()
{
    // 读取数据头
    readHeadData();
    // 确认信息
    tcpClient.write(ACK_PACKET.data(), static_cast<qint64>(ACK_PACKET.length()));

    content = "";
    qDebug() << "----------数据长度-----------" << m_dataLength;

    if (m_dataType == TEXT)
    {
        readTextContent();
        return true;
    }
    else if (m_dataType == IMAGE)
    {
        readImageContent();
        return true;
    }

    return false;
}

// 读取文本数据
void ReadClient::readTextContent()
{
    while (tcpClient.waitForReadyRead(-1))  // 阻塞等待直到有数据可读
    {
        unsigned int size = 0;  // 存储读取的字节数
        qDebug() << "-------readTextContent这一轮需要读取的字节数---------" << m_dataLength;

        // 获取当前可读数据的字节数
        unsigned int splitDataLength = static_cast<unsigned int>(tcpClient.bytesAvailable());

        unsigned int bufsize = TCP_BUFSIZ;  // 缓冲区大小

        while (m_dataLength != 0 && tcpClient.bytesAvailable())  // 只要还有剩余数据需要读取，且有可读数据
        {
            // 读取数据到缓冲区，读取的字节数由剩余数据大小和可用数据大小决定
            size = static_cast<unsigned int>(tcpClient.read(buffer, min(bufsize, m_dataLength)));

            // 更新剩余数据大小
            m_dataLength -= size;

            // 将读取的数据追加到content中
            content = content + string(buffer, size);
        }

        // 编码并发送ACK包，确认已接收到数据，传输的数据长度是splitDataLength
        string SPLIT_ACK_PACKET = DataEncoder().encode(ACK, m_accountId, TEXT, splitDataLength);
        tcpClient.write(SPLIT_ACK_PACKET.data(), static_cast<qint64>(SPLIT_ACK_PACKET.length()));

        // 如果数据已经读取完毕
        if (m_dataLength == 0)
        {
            // 根据协议ID执行不同操作
            switch (m_protocolId)
            {
            case SEND:
                emit textDisplayAble();     // 触发文本显示
                break;

            case NOTICE:
                notice = content;           // 设置通知内容
                emit noticeDisplayAble();   // 触发通知显示
                break;

            case ONLINELIST:
                emit onlineDisplayAble();   // 触发在线列表显示
                break;
            }

            break;  // 退出循环
        }
    }

    qDebug() << "读取TEXT完毕.......";  // 输出读取完成的日志

}

// 读取图片数据
void ReadClient::readImageContent()
{
    // 获取当前时间并生成图片文件的路径，图片命名规则为时间戳.png
    imagePath = IMAGE_PATH + MyTime::getTimeStampStr() + ".png";

    // 打开文件流以二进制形式保存图片数据
    // 文件以追加模式打开，如果文件已存在则不会覆盖
    ofstream os(imagePath, ios::out | ios::binary | ios::app);

    // 等待 TCP 客户端有数据可读
    while (tcpClient.waitForReadyRead(-1))
    {
        unsigned int size = 0;

        // 输出调试信息：当前需要读取的字节数
        qDebug() << "-------readImageContent这一轮需要读取的字节数---------" << m_dataLength;

        // 获取当前可用的字节数（即 tcpClient 中可读的数据量）
        unsigned int splitDataLength = static_cast<unsigned int>(tcpClient.bytesAvailable());

        // 设置缓冲区大小
        unsigned int bufsize = TCP_BUFSIZ;

        // 读取数据直到数据全部被接收
        while (m_dataLength != 0 && tcpClient.bytesAvailable())
        {
            // 从 tcpClient 中读取数据到 buffer，读取的数据量是缓冲区大小与剩余数据量中的较小者
            size = static_cast<unsigned int>(tcpClient.read(buffer, min(bufsize, m_dataLength)));

            // 更新剩余需要读取的字节数
            m_dataLength -= size;

            // 将读取的字节数据写入图片文件
            os.write(buffer, size);
        }

        // 构建 ACK 数据包，表示已接收数据
        // 编码 ACK 包，传入数据包类型 ACK、无账户信息、无数据类型、和当前接收的数据长度
        string SPLIT_ACK_PACKET = DataEncoder().encode(ACK, 0, 0, splitDataLength);

        // 发送 ACK 数据包给服务器，告知已成功接收当前数据
        // data()是转换为 const char*
        tcpClient.write(SPLIT_ACK_PACKET.data(), static_cast<qint64>(SPLIT_ACK_PACKET.length()));

        // 如果所有数据都已经接收完毕，跳出循环
        if (m_dataLength == 0)
        {
            break;
        }
    }

    os.close();

    // 发射信号，表明图片已读取完毕，触发显示操作
    emit imageDisplayAble();

    qDebug() << "图片读取完毕.......";
}


string ReadClient::getTextContent()
{
    return content;
}

string ReadClient::getNoticeContent()
{
    return notice;
}

string ReadClient::getImagePath()
{
    return imagePath;
}

ReadClient::~ReadClient()
{
    delete readClient;
    readClient = nullptr;
}
