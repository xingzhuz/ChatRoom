
// 定义了与用户相关的操作，通常用于处理用户的登录、注册以及账户验证等功能

#pragma once
#include "../MySQL/MySQLConnector.h"

using namespace std;

class UserService
{
private:
    MySQLConnector *connector; // MySQL 连接器，用于执行数据库操作

    // 检查账户的合法性
    bool checkAccount(string &accountId);

    // 检查密码的合法性
    bool checkPassword(string &password);

public:
    // 初始化数据库连接器
    UserService();

    // 检查用户登录，返回账户ID和用户名
    pair<int, string> checkLogin(string accountId, string password);

    // 检查账户是否已注册
    bool isRegistered(string accountId);

    // 注册新用户，传入账户、用户名和密码
    bool registerUser(string accountId, string username, string password);
};