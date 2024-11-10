
#include "UserService.h"

// 初始化数据库连接器
UserService::UserService()
{
    connector = MySQLConnector::getMySQLConnector();
}

// 检查账户的合法性
bool UserService::checkAccount(string &accountId)
{
    // 账号长度在3-6位
    if (accountId.length() > 6 || accountId.length() < 3)
    {
        cout << "账号长度有误" << endl;
        return false;
    }
    // 账号只能是数字
    for (char &c : accountId)
    {
        if (!(c >= '0' && c <= '9'))
        {
            cout << "账号只能是数字" << endl;
            return false;
        }
    }
    // 账号不能超过65535
    if (stoi(accountId) > 65535)
    {
        cout << "账号不能超过65535" << endl;
        return false;
    }
    return true;
}

// 检查密码的合法性
bool UserService::checkPassword(string &password)
{
    // 密码长度为6-16位
    if (password.length() < 6 || password.length() > 16)
    {
        cout << "密码长度有误" << endl;
        return false;
    }
    // 密码由字母,数字组成
    for (char &c : password)
    {
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
        {
            cout << "密码有误" << endl;
            return false;
        }
    }
    return true;
}

// 检查用户登录，返回账户ID和用户名
pair<int, string> UserService::checkLogin(string accountId, string password)
{
    pair<int, string> user;
    if (checkAccount(accountId) && checkPassword(password))
    {
        user = connector->queryUser(accountId, password);
    }
    return user;
}

// 检查账户是否已注册
bool UserService::isRegistered(string accountId)
{
    if (!checkAccount(accountId))
    {
        return true;
    }
    return connector->queryUser(accountId);
}

// 注册新用户，传入账户、用户名和密码
bool UserService::registerUser(string accountId, string username, string password)
{
    return checkAccount(accountId) && checkPassword(password) && connector->insertUser(accountId, username, password);
}
