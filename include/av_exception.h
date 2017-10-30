//av_exception.h

#ifndef AV_EXCEPTION_H
#define AV_EXCEPTION_H

#include <exception>
#include <string>
#include <sstream>

extern "C"{
#include <libavutil/error.h>
}
class AvException:std::exception
{
public:
	AvException(const char* whats, const char* file=__FILE__, int line=__LINE__)
		:_what(whats)
		,_file(file)
		,_line(line)
	{
		std::stringstream ss;
		ss<<_line;
		std::string s;
		ss>>s;
		_what += "\nFile:"+_file+",Line:"+s;
	}

	AvException(int errorCode, const char* file=__FILE__, int line=__LINE__)
		:_file(file)
		,_line(line)
	{
		char errstr[126]={0};
		av_make_error_string(errstr, 125, errorCode);
		_what.append(errstr);
		std::stringstream ss;
		ss<<_line;
		std::string s;
		ss>>s;
		_what += " File:"+_file+", Line:"+s;
	}

#ifdef __GNUC__
	const char* what() const noexcept override
#else
	const char* what() const override
#endif
	{
	
		return _what.c_str();
	}

private:
	std::string _what;
	std::string _file;
	int _line;
};

#endif /*AV_EXCEPTION_H*/
