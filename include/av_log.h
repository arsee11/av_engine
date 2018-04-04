//av_log.h

#ifndef AV_LOG_H
#define  AV_LOG_H

#include <functional>
#include <string>

enum LogLevel
{
	LOGL_INFO,
	LOGL_ERROR
};

extern void av_log_output(LogLevel l, const char* str);

typedef std::function<void(LogLevel, const char*)> log_callback_t;
extern void av_set_logger(const log_callback_t& logger);

extern void stdout_log(LogLevel l, const char* str);

class Logger;
extern Logger& av_log_info();
extern Logger& av_log_error();

struct end_log
{
};

class Logger
{
public:
    Logger(LogLevel l):_level(l){}
    Logger& operator<<(const char* str);
    Logger& operator<<(const std::string& str);
    Logger& operator<<(int val);
    Logger& operator<<(unsigned int val);
    Logger& operator<<(long val);
    Logger& operator<<(unsigned long val);
    Logger& operator<<(float val);
    Logger& operator<<(double val);
    Logger& operator<<(const end_log&);
    
    void clear(){ _str_buf.clear(); }
    
private:
    std::string _str_buf;
    LogLevel _level;

};

#endif /*AV_LOG_H*/
