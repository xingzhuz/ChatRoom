
#include "DataProcesser.h"

DataProcesser::DataProcesser()
{
}

// 检测套接字是否连接
int DataProcesser::checkSocketConnected(int sock)
{
    // 检查是否传入了有效的套接字描述符
    if (sock <= 0)
        return 0;

    // 定义一个 tcp_info 结构体来保存套接字的状态信息
    tcp_info info{};
    int len = sizeof(info);

    // 通过 getsockopt 函数获取套接字的 TCP 信息
    // IPPROTO_TCP 表示协议类型为 TCP，TCP_INFO 表示获取 TCP 连接的状态信息
    getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);

    // 检查 TCP 连接的状态是否为 ESTABLISHED（已建立连接）
    if (info.tcpi_state == TCP_ESTABLISHED)
    {
        // 如果状态为已建立连接，返回 1
        return 1;
    }
    else
    {
        return 0;
    }
}

// 指定的文件描述符 `fd` 读取文本数据
string DataProcesser::readTextContent(int fd, unsigned int dataLength)
{
    unsigned int count = 0;             // 已读取的字节数
    int size = 0;                       // 读取的数据大小，需为 int 类型以支持 -1 错误返回值
    unsigned int buffSize = TCP_BUFSIZ; // 每次读取的最大缓冲区大小
    string content;                     // 存储读取的文本内容

    while (true)
    {
        // 检测套接字是否连接，若未连接则退出循环
        if (checkSocketConnected(fd) == 0)
        {
            break;
        }
        if ((size = read(fd, buffer, min(buffSize, dataLength - count))) <= 0)
        {
            break;
        }

        // 如果成功读取了数据
        if (size > 0)
        {
            count += size;                   // 累加已读取的数据量
            content += string(buffer, size); // 将当前读取的数据追加到 content
        }

        // 如果已读取的数据量达到 dataLength，退出循环
        if (count == dataLength)
        {
            break;
        }
    }
    return content; // 返回读取到的文本内容
}

// 指定的文件描述符 `fd` 读取图片数据并保存到文件
string DataProcesser::readImageContent(int fd, unsigned int dataLength)
{
    // 获取当前时间戳作为文件名前缀，拼接成完整的图片路径
    string imagePath = IMAGE_PATH + MyTime::getTimeStampStr() + ".png";
    ofstream os(imagePath, ios::out | ios::binary); // 打开文件输出流，以二进制模式写入图片数据

    unsigned int count = 0;             // 已读取的数据字节数
    int size = 0;                       // 每次读取的数据大小
    unsigned int buffSize = TCP_BUFSIZ; // 每次读取的最大缓冲区大小

    while (true)
    {
        // 检测套接字是否仍然连接，如果断开则退出循环
        if (checkSocketConnected(fd) == 0)
        {
            break;
        }

        // 使用 recv 从套接字读取数据，读取字节数不超过 buffSize 和剩余数据量
        size = recv(fd, buffer, min(buffSize, dataLength - count), MSG_WAITALL);

        // 如果读取失败（返回值 <= 0），退出循环
        if (size <= 0)
        {
            break;
        }

        count += size;          // 累加已读取的数据字节数
        os.write(buffer, size); // 将读取到的数据写入文件

        // 如果已读取的数据量达到指定的 dataLength，则退出循环
        if (count == dataLength)
        {
            break;
        }
    }

    os.close();       // 关闭文件流
    return imagePath; // 返回图片文件的保存路径
}

// 向指定文件描述符 `fd` 写入文本数据，发送数据头和文本内容，并接收确认信息
void DataProcesser::writeText(int fd, unsigned int accountId, string text, unsigned int protocolId)
{
    DataEncoder de; // 创建数据编码器
    // 编码数据头信息，包括协议ID、账户信息、数据类型、数据长度等
    string headStr = de.encode(protocolId, accountId, TEXT, text.length());

    // 检查套接字是否连接
    if (checkSocketConnected(fd) == 0)
    {
        return; // 如果连接不可用，返回
    }

    // 发送数据头信息
    send(fd, headStr.data(), headStr.length(), MSG_NOSIGNAL);

    // 读取客户端响应
    read(fd, buffer, BASE_BUFFER_SIZE);

    int count = 0;                           // 已发送的数据字节数
    unsigned int dataLength = text.length(); // 数据总长度
    const char *data = text.data();          // 获取文本内容指针
    unsigned int buffSize = TCP_BUFSIZ;      // 缓冲区大小
    HeadData hd;                             // 数据头解析对象

    // 循环发送数据块
    while (true)
    {
        // 检查套接字是否连接
        if (checkSocketConnected(fd) == 0)
        {
            break; // 如果连接断开，则退出
        }

        // 发送数据块，最大发送 `buffSize` 字节
        ssize_t size = send(fd, data, min(buffSize, dataLength - count), MSG_NOSIGNAL);
        count += size;      // 累加已发送字节数
        data = data + size; // 更新数据指针，指向下一个要发送的部分

        // 接收服务器确认信息
        unsigned int splitDataLength = 0; // 记录已接收的确认数据字节数
        while (true)
        {
            // 检查套接字是否连接
            if (checkSocketConnected(fd) == 0)
            {
                break; // 如果连接断开，则退出
            }

            // 读取客户端的确认消息
            read(fd, buffer, BASE_BUFFER_SIZE);
            hd.parse(buffer);                      // 解析确认消息
            splitDataLength += hd.getDataLength(); // 累加确认的字节数

            // 如果确认数据的长度等于已发送的数据块长度，则退出确认循环
            if (splitDataLength == size)
            {
                break;
            }
        }

        // 如果已发送所有数据，则退出循环
        if (count == dataLength)
        {
            break;
        }
    }
}

// 向指定文件描述符 `fd` 写入图像数据，发送数据头和图像内容，并接收确认信息
void DataProcesser::writeImage(int fd, unsigned int accountId, const string &imagePath)
{
    // 打开图像文件，以二进制方式读取
    ifstream in(imagePath, ios::in | ios::binary);
    if (!in.is_open()) // 如果文件打开失败，输出错误并返回
    {
        cout << "文件打开失败" << endl;
        return;
    }

    // 获取图像文件的大小
    int imageSize = getFileLength(imagePath);

    DataEncoder de; // 创建数据编码器
    // 编码数据头，包含发送类型、账户信息、数据类型（图像）和图像大小
    string headStr = de.encode(SEND, accountId, IMAGE, imageSize);

    // 如果图像文件为空，关闭文件并返回
    if (imageSize == 0)
    {
        in.close();
        return;
    }

    // 将文件指针移动到文件的起始位置
    in.seekg(0);

    // 检查套接字是否连接
    if (checkSocketConnected(fd) == 0)
    {
        return; // 如果连接不可用，返回
    }

    // 发送数据头信息
    send(fd, headStr.data(), headStr.length(), MSG_NOSIGNAL);

    // 读取客户端响应
    read(fd, buffer, BASE_BUFFER_SIZE);

    int count = 0; // 记录已发送的字节数
    HeadData hd;   // 数据头解析对象

    // 循环读取图像数据并分块发送
    while (in.tellg() != -1) // 直到文件读取结束
    {
        // 检查套接字是否连接
        if (checkSocketConnected(fd) == 0)
        {
            break; // 如果连接断开，退出循环
        }

        // 从文件中读取数据块，并发送到服务器
        in.read(buffer, TCP_BUFSIZ);                 // 读取最多 `TCP_BUFSIZ` 字节
        send(fd, buffer, in.gcount(), MSG_NOSIGNAL); // 发送读取的数据块, gcount()返回的是上次从输入流中读取的字节数

        // 接收服务器确认信息
        unsigned int splitDataLength = 0; // 记录接收到的确认数据长度
        while (true)
        {
            // 检查套接字是否连接
            if (checkSocketConnected(fd) == 0)
            {
                break; // 如果连接断开，退出确认循环
            }

            // 读取客户端的确认消息
            read(fd, buffer, BASE_BUFFER_SIZE);
            hd.parse(buffer);                      // 解析确认消息
            splitDataLength += hd.getDataLength(); // 累加确认的字节数

            // 如果确认的数据长度等于已发送的数据块长度，退出确认循环
            // gcount()：返回的是上次从输入流中读取的字节数
            if (splitDataLength == in.gcount())
            {
                break;
            }
        }

        count += in.gcount(); // 累加已发送的字节数
    }

    // 关闭文件流
    in.close();
}

// 向指定文件描述符 `fd` 发送消息，包括数据头和文本内容
void DataProcesser::writeMsg(int fd, unsigned int accountId, string text, unsigned int protocolId)
{
    DataEncoder de; // 创建数据编码器对象
    // 编码数据头，包含协议ID、账户信息、数据类型（文本）和文本长度
    string headStr = de.encode(protocolId, accountId, TEXT, text.length());

    // 将编码后的数据头与文本内容拼接
    text = headStr + text;

    // 发送完整的数据（头 + 文本内容）到服务器
    send(fd, text.data(), text.length(), MSG_NOSIGNAL); // 使用MSG_NOSIGNAL避免发送SIGPIPE信号
}

void DataProcesser::writeTextToAllUser(const vector<int> &fds, int accountId, const string &text, unsigned int protocolId)
{
    for (auto &fd : fds)
    {
        writeText(fd, accountId, text, protocolId);
    }
}

void DataProcesser::writeImageToAllUser(const vector<int> &fds, int accountId, const string &imagePath)
{
    for (auto &fd : fds)
    {
        writeImage(fd, accountId, imagePath);
    }
}

int DataProcesser::getFileLength(const string &fileName)
{
    ifstream in(fileName, ios::in | ios::binary);
    if (!in.is_open())
    {
        cout << "文件打开失败" << endl;
        return 0;
    }
    in.seekg(0, ios::end); // 设置文件指针到文件流的尾部
    int fileLength = in.tellg();
    in.close();
    return fileLength;
}
