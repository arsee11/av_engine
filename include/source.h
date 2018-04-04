#ifndef SOURCE_H
#define SOURCE_H

#include <assert.h>
#include "transformation.h"
#include "av_exception.h"

template<class Param>
class Source :Transformation<Param>
{
public:
	Source(Transformation<Param>* next)
	:_next_trans(next)
	{
		
	}

    ///@return >=0 successed, <0 failed
	int read()
	{
		Param* p = nullptr;
		if ( (p=this->get()) != nullptr)
		{
			if(_next_trans != nullptr)
				_next_trans->put(p);
			else
			{
				//_storage->put(p);
			}
            
            return 0;
		}
        
        
        return -1;
	}

	
private:
	void put(Param* p)override { throw AvException("not implemented!");}
	
	Transformation<Param>* _next_trans = nullptr;
};

#endif/*SOURCE_H*/
