#ifndef CLIENT_H
#define CLIENT_H

#include<QTcpSocket>
#include<QtAlgorithms>
#include<string>
#include<fstream>
#include <QMessageBox>
#include<QDebug>
#include <QTextCodec>
#include<QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include"dataparser.h"
#include"dataencoder.h"
#include"configreader.h"
#define IMAGE_PATH "./image/"
#define TCP_BUFSIZ 8192
using namespace std;

class Client
{
public:
    QString hostName;
    quint16 port;
    QTcpSocket tcpClient;
    char buffer[TCP_BUFSIZ];
    unsigned int m_protocolId;
    unsigned int m_accountId;
    unsigned int m_dataType;
    unsigned int m_dataLength;
    const string ACK_PACKET = DataEncoder().encode(ACK, 0, 0, 0);

public:
    Client();

    void readHeadData();

    QJsonObject readServerMsg();

    // 发送文本数据，包括头部数据和实际的文本内容
    // accountId:账户ID，text:实际的文件数据，protocolId:协议ID
    void writeText(unsigned int accountId, string text, unsigned int protocolId = SEND);

    void closeTcpSocket();

    QTcpSocket * getTcpClient();

    ~Client();
};

#endif // CLIENT_H
