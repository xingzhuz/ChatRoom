
#pragma once
#include <iostream>
#include <string>
#include <sys/time.h>
using namespace std;

class MyTime
{
public:
    static string getCurrentFormatTimeStr();
    static string getTimeStampStr();
};
