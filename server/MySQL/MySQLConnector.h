
#include <iostream>
#include <string>
#include <mysql/mysql.h>
#include <unordered_map>
#include <json/json.h>
#include "../Util/MyTime.h"
#include "../config/mysql_config.h"
using namespace std;

class MySQLConnector
{
private:
    MYSQL *conn;
    static MySQLConnector *connector;
    void init();

public:
    static MySQLConnector *getMySQLConnector();

    pair<int, string> queryUser(const string &accountIf, const string &password);

    bool queryUser(const string &accountId);

    bool insertUser(const string &accountId, const string &username, const string &password);

    ~MySQLConnector();
};
