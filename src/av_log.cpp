//av_log.cpp

#include "av_log.h"
#include <iostream>
#include <string>
#include <sstream>

static log_callback_t g_logger = nullptr;

void av_log_output(AvLogLevel l, const char* str)
{
	if(g_logger != nullptr)
		g_logger(l, str);
}

void av_set_logger(const log_callback_t& logger)
{
	g_logger = logger;
}

void stdout_log(AvLogLevel l, const char* str)
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

Logger& Logger::operator<<(unsigned int val)
{
    _str_buf += t2str(val);
    return *this;
}

Logger& Logger::operator<<(long val)
{
    _str_buf += t2str(val);
    return *this;
}

Logger& Logger::operator<<(unsigned long val)
{
    _str_buf += t2str(val);
    return *this;
}

Logger& Logger::operator<<(size_t val)
{
    _str_buf += t2str(val);
    return *this;
}

/*
Logger& Logger::operator<<(int64_t val)
{

		_str_buf += std::to_string(val);
		return *this;
}
*/

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
    _str_buf = file +":"+t2str(line)+" "+_str_buf;
    av_log_output(_level, _str_buf.c_str());
    return *this;
}

static Logger info_log(LOGL_INFO);
Logger& av_log_info(const std::string& file, int line)
{
    info_log.clear();
    info_log.file = file;
    info_log.line = line;
    return info_log;
}

static Logger error_log(LOGL_ERROR);
Logger& av_log_error(const std::string& file, int line)
{
    error_log.clear();
    error_log.file = file;
    error_log.line = line;
    return error_log;
}
