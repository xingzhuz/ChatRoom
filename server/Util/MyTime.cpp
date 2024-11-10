#include "MyTime.h"

// 获取当前的时间并返回格式化后的时间字符串。
// 格式为：年-月-日 时:分:秒 (例如: "2024-11-06 14:23:45")
string MyTime::getCurrentFormatTimeStr()
{
    time_t timep; // 用于存储当前时间的变量
    time(&timep); // 获取当前时间，并将其存储到 timep 中

    char tmp[64] = {'\0'}; // 存储格式化后的时间字符串，初始化为全零
    // 使用 strftime 格式化时间，指定格式为 "%Y-%m-%d %H:%M:%S"
    // %Y - 四位年份，%m - 两位月份，%d - 两位日期
    // %H - 两位小时，%M - 两位分钟，%S - 两位秒
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));

    // 返回格式化后的时间字符串
    return string(tmp);
}

// 获取当前时间的时间戳（秒+微秒），并将其转化为字符串返回。
// 时间戳格式为：秒部分 + 微秒部分
string MyTime::getTimeStampStr()
{
    timeval stamp{};               // 定义一个 timeval 结构体变量来存储时间戳，结构体成员初始化为 0
    gettimeofday(&stamp, nullptr); // 获取当前时间并存入 stamp，时间戳包含秒和微秒

    // 将秒和微秒部分分别转换为字符串并连接，形成一个完整的时间戳字符串
    return to_string(stamp.tv_sec) + to_string(stamp.tv_usec);
}
