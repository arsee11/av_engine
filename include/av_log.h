//av_log.h

#ifndef AV_LOG_H
#define  AV_LOG_H

#include <functional>
#include <string>

enum AvLogLevel
{
	LOGL_INFO,
	LOGL_ERROR
};

extern void av_log_output(AvLogLevel l, const char* str);

typedef std::function<void(AvLogLevel, const char*)> log_callback_t;
extern void av_set_logger(const log_callback_t& logger);

extern void stdout_log(AvLogLevel l, const char* str);

class Logger;
extern Logger& av_log_info(const std::string& file=__FILE__, int line=__LINE__);
extern Logger& av_log_error(const std::string& file=__FILE__, int line=__LINE__);

struct end_log
{
};

class Logger
{
public:
    Logger(AvLogLevel l):_level(l){}
    Logger& operator<<(const char* str);
    Logger& operator<<(const std::string& str);
    Logger& operator<<(int val);
    Logger& operator<<(unsigned int val);
    Logger& operator<<(long val);
    Logger& operator<<(unsigned long val);
    //Logger& operator<<(int64_t val);
    Logger& operator<<(float val);
    Logger& operator<<(double val);
    Logger& operator<<(const end_log&);
    
    void clear(){ _str_buf.clear(); }
    
    std::string file;
    int line;
private:
    std::string _str_buf;
    AvLogLevel _level;

};

#endif /*AV_LOG_H*/
