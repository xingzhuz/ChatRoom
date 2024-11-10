
#include "MySQLConnector.h"

void MySQLConnector::init()
{
    conn = mysql_init(NULL);
    conn = mysql_real_connect(conn, MYSQL_HOST, USERNAME, PASSWORD, DATABASE, MYSQL_PORT, NULL, 0);
    if (!conn)
    {
        cout << "mysql_real_connect fail" << endl;
        exit(-1);
    }
    mysql_query(conn, "set names utf8");
}

MySQLConnector *MySQLConnector::getMySQLConnector()
{
    if (connector == nullptr)
    {
        connector = new MySQLConnector;
        connector->init();
    }
    return connector;
}

pair<int, string> MySQLConnector::queryUser(const string &accountId, const string &password)
{
    string querySql =
        "select account, username from user where account = " + accountId + " and password = " + "\"" + password +
        "\"";
    pair<int, string> user;

    int res = mysql_query(conn, querySql.data());
    if (!res)
    {
        MYSQL_RES *result = mysql_store_result(conn);
        if (result && mysql_num_rows(result))
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            user.first = atoi(row[0]);
            user.second = row[1];
        }
        mysql_free_result(result);
    }
    return user;
}

bool MySQLConnector::queryUser(const string &accountId)
{
    string querySql = "select account from user where account = " + accountId;
    int res = mysql_query(conn, querySql.data());
    bool flag = false;
    if (!res)
    {
        MYSQL_RES *result = mysql_store_result(conn);
        if (result && mysql_num_rows(result))
        {
            flag = true;
        }
        mysql_free_result(result);
    }
    return flag;
}

bool MySQLConnector::insertUser(const string &accountId, const string &username, const string &password)
{
    string insertSql = "insert into `user` values(" +
                       accountId + ","
                                   "\"" +
                       username + "\"," + "\"" + password + "\"," + "\"" + MyTime::getCurrentFormatTimeStr() + "\")";
    int res = mysql_query(conn, insertSql.data());
    return res == 0;
}

MySQLConnector::~MySQLConnector()
{
    mysql_close(conn);
    delete connector;
}
MySQLConnector *MySQLConnector::connector = nullptr;
