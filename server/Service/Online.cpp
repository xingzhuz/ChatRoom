

#include "Online.h"

// 获取在线用户列表的字符串表示，返回包含用户信息的JSON格式字符串
string Online::getOnlineListStr()
{
    Json::Value onlineList; // 创建一个Json对象，用于存储在线用户列表

    // 遍历所有用户，将每个用户的信息加入到在线列表中
    for (auto &item : userMap)
    {
        Json::Value userJson;                          // 创建一个Json对象来表示单个用户的信息
        userJson["accountId"] = item.second.accountId; // 用户账户
        userJson["username"] = item.second.username;   // 用户名
        userJson["loginTime"] = item.second.loginTime; // 登录时间

        // 将单个用户的Json信息添加到在线用户列表
        onlineList.append(userJson); // 变成JSON数组类似的形式
    }

    // 返回在线用户列表的JSON字符串表示
    return onlineList.toStyledString();
}

// 向用户表中添加新用户
bool Online::appendUser(int accountId, string username)
{
    // 创建一个用户对象 `u`，包含账户、用户名和当前的时间戳（作为登录时间）
    user u = {accountId, std::move(username), MyTime::getCurrentFormatTimeStr()};

    // 将用户信息添加到 userMap中，使用账户作为键
    userMap[accountId] = u;

    // 返回 `true` 表示成功添加用户
    return true;
}

// 从在线用户列表中移除指定账户的用户
bool Online::removeUser(int accountId)
{
    // 从 userMap 中删除指定账户的用户
    userMap.erase(accountId);

    // 从 writeFdToReadFd 中删除与该账户对应的文件描述符映射
    LoginFdToChatFd.erase(accountToFd[accountId]);

    // 从 accountToFd 中删除该账户与文件描述符的映射
    accountToFd.erase(accountId);

    // 返回 true 表示用户成功被移除
    return true;
}

bool Online::appendUser(pair<int, string> &user)
{
    return appendUser(user.first, user.second);
}

int Online::getChatFd(int loginFd)
{
    return LoginFdToChatFd[loginFd];
}

vector<int> Online::getAllChatFd()
{
    vector<int> v;
    for (auto &item : LoginFdToChatFd)
    {
        v.push_back(item.second);
    }
    return v;
}

bool Online::appendLoginFd(int accountId, int fd)
{
    accountToFd[accountId] = fd;
    return true;
}

bool Online::appendChatFd(int accountId, int fd)
{
    LoginFdToChatFd[accountToFd[accountId]] = fd;
    return true;
}

string Online::getUserName(int accountId)
{
    return userMap[accountId].username;
}

bool Online::isLogin(int accountId)
{
    return userMap.count(accountId) != 0;
}
