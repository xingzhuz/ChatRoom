#include "readthread.h"
#include "readclient.h"
#include <QTcpSocket>
ReadThread::ReadThread()
{
}

void ReadThread::run()
{
    ReadClient *readClient = ReadClient::getTcpSocketClient();
    QTcpSocket *tcpSocket = readClient->getTcpClient();

    // 将 QTcpSocket 移动到当前线程
    tcpSocket->moveToThread(this);

    // 连接信号槽，监听 readyRead 信号
    connect(tcpSocket, &QTcpSocket::readyRead, readClient, &ReadClient::readData);

    // 启动子线程的事件循环
    exec();
}
