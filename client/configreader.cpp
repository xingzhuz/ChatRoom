#include "configreader.h"
#include <QDebug>

ConfigReader::ConfigReader()
{
}

string ConfigReader::SERVER_CONFIG_PATH = "./config/server.conf";

// 读取服务器配置文件
unordered_map<string, string> ConfigReader::readServerConfig()
{
    ifstream in(SERVER_CONFIG_PATH, ios::in);  // 打开配置文件读取流
    unordered_map<string, string> configMap;   // 用于存储配置文件中键值对的哈希表

    if (in.is_open())  // 检查文件是否成功打开
    {
        string line;

        // 按行读取配置文件
        while (getline(in, line))
        {
            unsigned long long index = line.find('=');                  // 查找等号的位置，分隔键和值
            configMap[line.substr(0, index)] = line.substr(index + 1);  // 分割键和值并保存到map中
        }
    }
    else
    {
        qDebug() << "打开配置文件失败";
    }

    // 输出配置映射到调试窗口
    return configMap;  // 返回配置映射
}



