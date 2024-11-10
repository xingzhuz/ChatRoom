
// 头文件用于定义一个管理在线用户信息的类

#pragma once // 确保该头文件只被包含一次
#include <json/json.h>
#include <string>
#include "../Util/MyTime.h" // 引入自定义时间工具类
#include <unordered_map>
#include <utility>
using namespace std;

class Online
{
private:
    // 定义一个结构体，用于存储用户信息
    struct user
    {
        int accountId;    // 用户账户ID
        string username;  // 用户名
        string loginTime; // 登录时间
    };

    // 成员变量：存储不同类型的映射
    unordered_map<int, int> LoginFdToChatFd; // 存储登录窗口的描述符到和客户端通信描述符的映射
    unordered_map<int, int> accountToFd;     // 存储账户ID到文件描述符的映射
    unordered_map<int, user> userMap;        // 存储账户ID到用户信息的映射

public:
    // 返回所有在线用户的 JSON 格式字符串
    string getOnlineListStr();

    // 向在线用户列表中添加新用户（通过账户和用户名）
    bool appendUser(int accountId, string username);

    // 从在线用户列表中移除指定账户的用户
    bool removeUser(int accountId);

    // 向在线用户列表中添加新用户（通过账户和用户名，以pair传入）
    bool appendUser(pair<int, string> &user);

    // 根据写文件描述符获取对应的读文件描述符
    int getChatFd(int loginFd);

    // 获取所有的读文件描述符，返回一个整数向量
    vector<int> getAllChatFd();

    // 将写文件描述符与账户ID关联，添加到映射中
    bool appendLoginFd(int accountId, int fd);

    // 将读文件描述符与账户ID关联，添加到映射中
    bool appendChatFd(int accountId, int fd);

    // 根据账户ID获取该用户的用户名
    string getUserName(int accountId);

    // 检查指定账户是否已经登录
    bool isLogin(int accountId);
};
