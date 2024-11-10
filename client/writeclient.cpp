#include "writeclient.h"

#include <QNetworkProxy>
#include <QThread>

WriteClient::WriteClient()
{
}

// 获取TCP Socket客户端实例
WriteClient *WriteClient::writeClient = nullptr;  // 初始化静态成员变量
WriteClient *WriteClient::getTcpSocketClient()
{
    // 如果writeClient为nullptr，说明尚未创建客户端实例
    if (writeClient == nullptr)
    {
        writeClient = new WriteClient;

        // 从配置文件中读取服务器配置
        unordered_map<string, string> configMap = ConfigReader::readServerConfig();

        // 输出读取到的服务器地址和端口
        qDebug() << "从配置文件读取的" << QString::fromStdString(configMap["HOST"]) << "->" << QString::fromStdString(configMap["PORT"]);

        // 设置TCP客户端的主机名和端口号，转换为QString和整数类型
        writeClient->hostName = QString::fromStdString(configMap["HOST"]);
        writeClient->port = static_cast<quint16>(stoi(configMap["PORT"]));

        qDebug() << "------------正在连接服务器--------------" << writeClient->hostName;

        // 记录连接开始时间
        long long start;
        time(&start);

        // 发起连接到服务器
        // 首先关闭代理（我的不关闭就连接不上虚拟机ubuntu）
        QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
        writeClient->tcpClient.connectToHost(writeClient->hostName, writeClient->port);

        // 等待最多10秒钟以建立连接，返回连接结果
        bool connResult = writeClient->tcpClient.waitForConnected(10000);

        if (connResult && writeClient->tcpClient.state() == QTcpSocket::ConnectedState)
        {
            qDebug() << "------------连接服务器成功--------------";
        }
        else
        {
            qDebug() << "------------连接服务器失败--------------";
            writeClient = nullptr;
        }
    }

    // 返回TCP客户端实例
    return writeClient;
}

void WriteClient::writeImage(unsigned int accountId, string imagePath)
{
    // 打开图片文件，以二进制方式读取
    ifstream in(imagePath, ios::in | ios::binary);

    // 将文件指针移动到文件尾部，以获取文件大小
    in.seekg(0, ios::end);
    streampos imageSize = in.tellg(); // 获取文件大小（字节数）

    // 创建数据编码器，并生成带有文件信息的消息头
    DataEncoder encoder;
    string headStr = encoder.encode(SEND, accountId, IMAGE, static_cast<unsigned int>(imageSize)); // 包含操作类型、账号、数据类型和文件大小

    // 将文件指针重置到文件头部，准备开始读取文件数据
    in.seekg(0);
    qDebug() << "需要发送的图片的字节数-----" << imageSize;

    // 发送消息头到服务器，告知即将发送的图片数据的相关信息
    tcpClient.write(headStr.data(), static_cast<qint64>(headStr.length()));

    int count = 0; // 用于统计实际发送的字节数

    // 循环读取文件数据并发送
    while (in.tellg() != -1)  // 检查文件流是否已到达文件末尾
    {
        // 从文件中读取固定大小的字节（TCP_BUFSIZ）到缓冲区
        in.read(buffer, TCP_BUFSIZ);

        // 将缓冲区中的数据写入到服务器
        qint64 size = tcpClient.write(buffer, in.gcount());

        // 累加已发送的字节数
        count += size;
    }

    // 输出实际发送的总字节数，用于确认数据传输完整性
    qDebug() << "实际发送的字节数---------" << count;

    // 关闭文件流
    in.close();
}

WriteClient::~WriteClient()
{
    delete writeClient;
    writeClient = nullptr;
}
