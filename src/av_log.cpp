//av_log.cpp

#include "av_log.h"
#include <iostream>
#include <string>
#include <sstream>

static log_callback_t g_logger = nullptr;

void av_log_output(LogLevel l, const char* str)
{
	if(g_logger != nullptr)
		g_logger(l, str);
}

void av_set_logger(const log_callback_t& logger)
{
	g_logger = logger;
}

void stdout_log(LogLevel l, const char* str)
{
	std::string lstr;
	if (l == LOGL_ERROR)
		lstr = "Error";
	else if (l == LOGL_INFO)
		lstr = "Info";

	std::cout <<"[av_engine log] "<< lstr<<": " << str << std::endl;
}

template<typename T>
static inline std::string t2str(T val)
{
    std::string str;
    std::stringstream ss;
    ss<<val;
    ss>>str;
    return str;
}

Logger& Logger::operator<<(const char* str)
{
    _str_buf += str;
    return *this;
}

Logger& Logger::operator<<(const std::string& str)
{
    _str_buf += str;
    return *this;
}

Logger& Logger::operator<<(int val)
{
    _str_buf += t2str(val);
    return *this;
}

Logger& Logger::operator<<(float val)
{
    _str_buf += t2str(val);
    return *this;
}

Logger& Logger::operator<<(double val)
{
    _str_buf += t2str(val);
    return *this;
}

Logger& Logger::operator<<(const end_log& )
{
    av_log_output(_level, _str_buf.c_str());
    return *this;
}

static Logger info_log(LOGL_INFO);
Logger& av_log_info()
{
    info_log.clear();
    return info_log;
}

static Logger error_log(LOGL_ERROR);
Logger& av_log_error()
{
    error_log.clear();
    return error_log;
}
