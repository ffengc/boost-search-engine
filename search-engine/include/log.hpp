

#ifndef __YUFC_LOG__
#define __YUFC_LOG__

#include <iostream>
#include <map>
#include <string>

enum STATUES // 日志等级
{
    INFO,
    DEBUG,
    WARNING,
    ERROR,
    FATAL
};

/*
 * Write by Yufc
 * See https://github.com/ffengc/boost-search-engine
 * please cite my project link: https://github.com/ffengc/boost-search-engine when you use this code
 */


// 定义颜色代码
#define RESET "\033[0m"
#define RED "\033[1;31m" // 加粗红色
#define GREEN "\033[1;32m" // 加粗绿色
#define YELLOW "\033[1;33m" // 加粗黄色
#define BLUE "\033[1;34m" // 加粗蓝色
#define MAGENTA "\033[1;35m" // 加粗洋红

// 根据日志等级获取相应颜色
inline const char* GetColor(const std::string& level) {
    std::map<std::string, int> m = { { "INFO", 0 }, { "DEBUG", 1 }, { "WARNING", 2 }, { "ERROR", 3 }, { "FATAL", 4 } };
    switch (m[level]) {
    case INFO:
        return BLUE;
    case DEBUG:
        return GREEN;
    case WARNING:
        return YELLOW;
    case ERROR:
        return MAGENTA;
    case FATAL:
        return RED;
    default:
        return RESET;
    }
}

// LOG() << "message"
inline std::ostream& Log(const std::string& level, const std::string& file_name, int line) {
    // 添加日志等级
    std::string levelTag = std::string("[") + level + "]";
    std::string coloredLevelTag = std::string(GetColor(level)) + levelTag + RESET;
    std::string message = coloredLevelTag;
    // 添加报错文件名称
    message += "[";
    message += file_name;
    message += "]";
    // 添加当前文件的行数
    message += "[";
    message += std::to_string(line);
    message += "]";
    // cout 本质内部是包含缓冲区的
    std::cout << message << " "; // 不要endl进行刷新
    return std::cout;
}

// 这种就是开放式的日志
#define LOG(level) Log(#level, __FILE__, __LINE__)

#endif