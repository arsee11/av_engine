#ifndef SOURCE_H
#define SOURCE_H

#include <assert.h>
#include "filter.h"


template<class Param>
class Source :public Filter<Param>
{
public:
	Source(Transformation<Param>* ts)
	:_trans(ts)
	{
		
	}

    ///@return >=0 successed, <0 failed
	int read()
	{
		Param* p = nullptr;
		if (this->transform(p))
		{
			if(_trans != nullptr)
				_trans->put(p);
			else
			{
				//_storage->put(p);
			}
            
            return 0;
		}
        
        
        return -1;
	}

	
private:
	void put(Param* p)override {}

	Transformation<Param>* _trans = nullptr;
};

#endif/*SOURCE_H*/
