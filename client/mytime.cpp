#include "mytime.h"

// 获取当前时间的时间戳（秒+微秒），并将其转化为字符串返回。
// 时间戳格式为：秒部分 + 微秒部分
string MyTime::getTimeStampStr()
{
    timeval stamp{};               // 定义一个 timeval 结构体变量来存储时间戳，结构体成员初始化为 0
    gettimeofday(&stamp, nullptr); // 获取当前时间并存入 stamp，时间戳包含秒和微秒

    // 将秒和微秒部分分别转换为字符串并连接，形成一个完整的时间戳字符串
    return to_string(stamp.tv_sec) + to_string(stamp.tv_usec);
}
