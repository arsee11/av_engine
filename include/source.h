#ifndef SOURCE_H
#define SOURCE_H

#include <assert.h>
#include "transformation.h"
#include "av_exception.h"

template<class Param>
class Source :Transformation<Param>
{
public:
	Source(Transformation<Param>* next=nullptr)
	:_next_trans(next)
	{
		
	}

   	///@return >=0 successed, <0 failed
	int read()
	{
		_param.clear();
		if(this->get() != nullptr)
		{
			if(_next_trans != nullptr)
				_next_trans->put(&_param);

            		return _param.size();
		}
        
        	return -1;
	}

	void setNext(Transformation<Param>* next){ _next_trans=next; }

protected:
	Param _param;

private:
	void put(Param* p)override { throw AvException("not implemented!");}
	
	Transformation<Param>* _next_trans = nullptr;
};

#endif/*SOURCE_H*/
