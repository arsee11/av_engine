#ifndef AV_Dump_H
#define AV_Dump_H

#include "sink.h"

class AVDump: public Sink<AVParam>
{
public:
	AVDump(const std::string& file_name)
	{
		fs.open(file_name, std::ios::out | std::ios::binary);
    }
    
private:
	void put(AVParam* p)override
	{	
   		 fs.write(reinterpret_cast<char*>(p->data_ptr()), p->size());
	}


private:
	std::ofstream fs;
};
#endif /*AV_Dump_H*/
