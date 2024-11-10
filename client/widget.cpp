#include "widget.h"
#include "ui_widget.h"
#include <QDir>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <readthread.h>
#include <QMap>
#include <QVariantMap>
#include <QImageReader>
Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 设置发送的图片大小
    ui->contentListWidget->setIconSize(QSize(200, 200));

    // 获取客户端写端对象（用于发送数据）
    writeClient = WriteClient::getTcpSocketClient();

    // 获取客户端读端对象（用于接收数据）
    readClient = ReadClient::getTcpSocketClient();

    // 发送文本数据：当点击“发送文本”按钮时，获取用户输入的文本并发送
    connect(ui->sendTextPushButton, &QPushButton::clicked, this, [ = ]()
    {
        // 获取文本框的内容
        QString text = ui->inputTextEdit->toPlainText();

        // 如果文本为空，则弹出提示框提示用户不能发送空内容
        if(text.trimmed().length() == 0)
        {
            QMessageBox::information(this, "发送提示", "不能发送空内容");
            return;
        }

        // 调用写客户端的接口，发送用户输入的文本内容
        writeClient->writeText(user->getUserId(), text.toStdString());

        // 清空输入框内容
        ui->inputTextEdit->clear();
    });

    // 发送图片：当点击“发送图片”按钮时，打开文件选择对话框，选择图片并发送
    connect(ui->sendImagePushButton, &QPushButton::clicked, this, [ = ]()
    {
        // 打开文件选择对话框，选择图片文件
        // tr("Open Image")，"Open Image" 是对话框的标题文本
        // 这是文件对话框打开时默认显示的路径。此处为 "."，表示当前目录
        // 第四个参数是过滤器，只允许用户选择后缀为 .png、.jpg、.jpeg、.bmp 的图像文件
        QString filePath = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files(*.png *.jpg *jpeg *.bmp)"));
        qDebug() << "文件名---------------" << filePath;

        // 将文件路径编码为GB2312，解决中文路径问题
        QTextCodec *code = QTextCodec::codecForName("GB2312");
        string imagePath = code->fromUnicode(filePath).data();

        // 如果选择了文件路径，则发送图片数据
        if(imagePath != "")
        {
            writeClient->writeImage(user->getUserId(), imagePath);
            qDebug() << "发送文件成功";
        }
    });

    // 连接读端的信号：当收到文本数据时，将其添加到文本显示区域
    connect(readClient, &ReadClient::textDisplayAble, this, [ = ]()
    {
        // QString::fromStdString()用于将 std::string 转换为 Qt 的 QString 类型
        ui->contentListWidget->addItem(QString::fromStdString(readClient->getTextContent()));
    });

    // 连接读端的信号：当收到图片数据时，将其显示在界面中
    connect(readClient, &ReadClient::imageDisplayAble, this, [ = ]()
    {
        QListWidgetItem * pic = new QListWidgetItem;
        pic->setIcon(QIcon(QString::fromStdString(readClient->getImagePath())));
        ui->contentListWidget->addItem(pic);
    });

    // 创建并启动读取线程
    thread = new ReadThread;
    thread->start();

    // 连接退出按钮：当点击退出按钮时，退出线程并关闭当前窗口和登录窗口
    connect(ui->exitPushButton, &QPushButton::clicked, this, [ = ]()
    {
        // 退出线程
        thread->exit();
        // 关闭当前窗口
        this->close();
        // 关闭登录窗口
        this->loginForm->close();
    });

    // 连接读端的信号：当收到公告数据时，将其添加到公告显示区域
    connect(readClient, &ReadClient::noticeDisplayAble, this, [ = ]()
    {
        ui->noticeListWidget->addItem(QString::fromStdString(readClient->getNoticeContent()));
    });

    // 连接读端的信号：当收到在线用户列表数据时，更新在线用户信息显示
    connect(readClient, &ReadClient::onlineDisplayAble, this, [ = ]()
    {
        // 清空原有的在线用户列表
        ui->onlineListWidget->clear();

        // 添加表头
        QString header = QString("账号ID") + "\t" + QString("用户名") + "\t" + QString("加入时间");
        ui->onlineListWidget->addItem(header);

        // 设置表头样式，可以通过改变字体、颜色等来区分
        QListWidgetItem *headerItem = ui->onlineListWidget->item(0);
        headerItem->setBackgroundColor(QColor(220, 220, 220)); // 给表头设置背景色
        headerItem->setTextColor(Qt::blue);  // 设置表头文字颜色

        // 解析在线用户列表数据并更新UI
        QVariantList onlineList = QJsonDocument::fromJson(QString::fromStdString(readClient->getTextContent()).toUtf8()).toVariant().toList();

        // 遍历在线用户列表，更新显示内容
        for(auto & user : onlineList)
        {
            QVariantMap map = user.toMap();
            QString element = map["accountId"].toString() + "\t" + map["username"].toString() + "\t" + map["loginTime"].toString();
            ui->onlineListWidget->addItem(element);
        }

        // 更新在线人数显示
        ui->onlineLabel->setText("当前在线人数(" + QString::number(onlineList.size()) + "人)");
    });
}

Widget::~Widget()
{
    delete ui;
    delete user;
}

void Widget::displayUserInfo()
{
    ui->userLabel->setText(user->getUsername() + "(" + QString::number(user->getUserId()) + ")");
}

void Widget::closeEvent(QCloseEvent *e)
{
    thread->exit();
    this->loginForm->close();
    this->close();
}
