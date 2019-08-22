#pragma once

#include <iostream>
#include <string>
using namespace std;

// 日志级别对应值
#define NORMAL 0
#define WARNING 1
#define ERROR 2

// 宏替换函数，方便获取日志的所在文件和所在行
#define LOG(msg, level) Log(msg, level, __FILE__, __LINE__)

// 用日志级别的值作为下标对应输出
const char *log_level[]={
	"Normal",
	"Warning",
	"Error",
	NULL
};

// 打印日志的函数，输出日志信息及所在文件、所在行
void Log(string msg, int level, string file, int line)
{
	cout << '[' << msg << ']' << '[' << log_level[level] << ']' << "：" << \
	file << "：" << line << endl;
}



