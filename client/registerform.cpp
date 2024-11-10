#include "registerform.h"
#include "ui_registerform.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegExpValidator>
#include <string>
#include <QDebug>
#include <QString>
#include <QMessageBox>

RegisterForm::RegisterForm(QWidget *parent) : QWidget(parent),
    ui(new Ui::RegisterForm)
{
    // 设置UI界面
    ui->setupUi(this);

    // 获取一个写客户端的实例
    writeClient = WriteClient::getTcpSocketClient();

    // 如果获取客户端失败（例如服务器未连接）
    if(writeClient == nullptr)
    {
        qDebug() << "连接服务器超时";
        QMessageBox::information(this, "登录提示", "连接服务器超时");
        return;
    }

    // 设置账号输入框的验证规则，只允许输入3到6位数字
    ui->accountLineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]{3,6}$")));
    // 设置用户名输入框的验证规则，允许输入中文字符、英文字母，长度为1到9位
    ui->usernameLineEdit->setValidator(new QRegExpValidator(QRegExp("[\u4e00-\u9fa5a-zA-Z]{1,9}$")));
    // 设置密码输入框的验证规则，允许输入数字和字母，长度为6到16位
    ui->passwordLineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9a-zA-Z]{6,16}$")));

    // 连接注册按钮的点击事件
    connect(ui->submitPushButton, &QPushButton::clicked, this, [ = ]()
    {
        // 获取用户输入的账号
        QString accountId = ui->accountLineEdit->text();

        // 验证账号长度是否符合要求
        if(accountId.length() < 3)
        {
            QMessageBox::information(this, "注册提示", "账号长度应为3-6位");
            return;  // 账号不符合要求，退出函数
        }

        // 验证账号数字是否大于65535
        if(accountId.toInt() > 65535)
        {
            QMessageBox::information(this, "注册提示", "账号数字不能超过65535");
            return;  // 账号超出范围，退出函数
        }

        // 获取用户输入的用户名
        QString username = ui->usernameLineEdit->text();

        if(username.length() < 3)
        {
            QMessageBox::information(this, "注册提示", "昵称长度应为3-9位");
            return;  // 用户名不符合要求，退出函数
        }

        // 获取用户输入的密码
        QString password = ui->passwordLineEdit->text();

        if(password.length() < 6)
        {
            QMessageBox::information(this, "注册提示", "密码长度应为6-16位");
            return;  // 密码不符合要求，退出函数
        }

        // 将账号、用户名和密码信息封装成JSON对象
        QJsonObject information;
        information.insert("accountId", accountId);
        information.insert("username", username);
        information.insert("password", password);

        // 将JSON对象转为字符串
        QJsonDocument doc;
        doc.setObject(information);
        QString s = doc.toJson(QJsonDocument::Compact);

        // 发送注册请求到服务器
        writeClient->writeText(0, s.toStdString(), REGISTER);

        // 阻塞等待服务器返回结果
        writeClient->getTcpClient()->waitForReadyRead(-1);

        // 读取服务器的响应消息
        QJsonObject serverInfoObj = writeClient->readServerMsg();
        int status = serverInfoObj["status"].toInt();

        // 根据返回的状态码显示注册结果
        if(status == REGISTER_SUCCESS)
        {
            // 注册成功，提示用户并关闭注册界面
            QMessageBox::information(this, "注册提示", "注册成功");
            this->close();
        }
        else if(status == REGISTER_FAIL)
        {
            // 注册失败，提示账号已存在
            QMessageBox::information(this, "注册提示", "账号已存在, 请填写其它账号");
        }
    });
}


RegisterForm::~RegisterForm()
{
    delete ui;
}

void RegisterForm::closeEvent(QCloseEvent *e)
{
    this->loginForm->show();
}
