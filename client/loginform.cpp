#include "loginform.h"
#include "ui_loginform.h"
#include "widget.h"
#include "writeclient.h"
#include "readclient.h"
#include "registerform.h"

#include <QMessageBox>
#include <QTimer>

LoginForm::LoginForm(QWidget *parent) : QWidget(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);

    // 将登录按钮点击事件与connectServer槽函数连接
    connect(ui->loginPushButton, &QPushButton::clicked, this, &LoginForm::connectServer);

    // 将注册按钮点击事件与Lambda表达式连接
    connect(ui->registerPushButton, &QPushButton::clicked, this, [ = ]()
    {
        // 创建注册窗口的实例
        RegisterForm * rf = new RegisterForm;
        rf->loginForm = this;    // 将当前登录窗口对象的指针传递给注册窗口
        rf->show();
        this->hide();
    });
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::connectServer()
{
    qDebug() << "-------------开始连接服务器----------------";

    // 获取 WriteClient 实例（用于发送登录请求）
    writeClient = WriteClient::getTcpSocketClient();

    // 检查 writeClient 是否成功获取，如果为 nullptr，说明连接超时
    if (writeClient == nullptr)
    {
        qDebug() << "连接服务器超时";
        QMessageBox::information(this, "登录提示", "连接服务器超时");
        return;
    }

    // 获取用户在界面输入的账号和密码
    QString accountId = ui->userIdLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    qDebug() << "账号:" << accountId;
    qDebug() << "密码:" << password;

    // 创建 JSON 对象，用于发送登录信息到服务器
    QJsonObject information;
    information.insert("accountId", accountId);
    information.insert("password", password);
    QJsonDocument doc;
    doc.setObject(information);

    // 将 JSON 对象转为字符串并发送给服务器
    // QJsonDocument::Compact 表示将 JSON 数据以紧凑的格式进行编码（即没有额外的空格或换行）
    string loginMsg = doc.toJson(QJsonDocument::Compact).toStdString();
    writeClient->writeText(accountId.toUtf8().toUShort(), loginMsg, LOGIN);

    // 等待服务器的响应（阻塞等待，直到接收到数据）
    writeClient->getTcpClient()->waitForReadyRead(-1);
    qDebug() << "收到后台确认信息---------------------";

    // 从服务器读取登录的响应信息
    QJsonObject serverInfoObj = writeClient->readServerMsg();
    int status = serverInfoObj["status"].toInt();

    // 根据登录状态进行处理
    if (status == LOGIN_SUCCESS)
    {
        // 登录成功，获取 ReadClient 实例（用于读取数据）
        readClient = ReadClient::getTcpSocketClient();

        // 创建主窗口 Widget 实例，并传递用户信息
        Widget *widget = new Widget;
        widget->loginForm = this; // 保存当前登录窗体的指针

        // 创建用户对象并传递账号和用户名
        user = new User(accountId.toUtf8().toUShort(), serverInfoObj["username"].toString());
        widget->user = user;
        qDebug() << "accountId:" << accountId << "->" << "username:" << serverInfoObj["username"].toString();

        // 显示用户信息并展示主窗口
        widget->displayUserInfo();
        widget->show();

        // 通知服务器进行更新在线用户列表和公告
        readClient->writeText(accountId.toUtf8().toUShort(), "", UPDATE_DATA);

        // 隐藏登录窗体
        this->hide();
    }
    else if (status == LOGIN_FAIL)
    {
        // 登录失败，账号或密码错误
        QMessageBox::information(this, "登录提示", "账号或密码错误,请重试");
    }
    else if (status == LOGIN_EXIST)
    {
        // 登录失败，该账号已在线
        QMessageBox::information(this, "登录提示", "该账号已登录，不可重复登录");
    }
}


void LoginForm::closeEvent(QCloseEvent *e)
{
    qDebug() << "loginform关闭了";

    // 获取当前用户的用户ID，如果用户为空，则设置为0
    unsigned int userId = (user != nullptr ? user->getUserId() : 0);

    // 区分以下的if语句是因为这个窗口的关闭有可能还没进入聊天界面就关闭窗口了
    // 还有可能是聊天主界面关闭的时候调用的这个窗口关闭，此时需要关闭写和读两个
    // 只有这个登录窗口还没登录成功的时候只有写端

    // 如果 readClient 不为空，表示当前有读客户端连接
    if (readClient != nullptr)
    {
        qDebug() << "readClient关闭了";

        // 发送登出消息到服务端，告知服务器用户即将退出
        writeClient->writeText(userId, "", LOGOUT);

        // 等待一段时间，确保登出信息能够及时发到服务器
        sleep(300);

        // 断开 writeClient 的 TCP 连接
        writeClient->getTcpClient()->disconnectFromHost();

        // 等待一段时间，确保断开连接操作完成
        sleep(100);

        // 断开 readClient 的 TCP 连接
        readClient->getTcpClient()->disconnectFromHost();

        // 等待一段时间，确保断开连接操作完成
        sleep(100);
    }
    // 如果 readClient 为空，但 writeClient 不为空，表示只有写客户端连接
    else if (writeClient != nullptr)
    {
        qDebug() << "writeClient关闭了";

        // 发送关闭连接消息到服务端，告知服务器用户正在关闭连接
        writeClient->writeText(userId, "", CLOSE);

        // 等待一段时间，确保关闭信息能够及时发到服务器
        sleep(300);

        // 断开 writeClient 的 TCP 连接
        writeClient->getTcpClient()->disconnectFromHost();

        // 等待一段时间，确保断开连接操作完成
        sleep(100);
    }
}


void LoginForm::sleep(int msec)
{
    // 创建一个局部的事件循环对象，用于实现延时
    QEventLoop eventloop;

    // 使用单次定时器，在 msec 毫秒后触发事件循环的 quit() 槽函数
    QTimer::singleShot(msec, &eventloop, SLOT(quit()));

    // 开始事件循环，阻塞当前函数的执行，直到 quit() 被触发
    eventloop.exec();
}
