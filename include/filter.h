//  Filter.h


#ifndef FILTER_H
#define FILTER_H

#include "transformation.h"


template<class Param>
class Filter : public Transformation<Param>
{
public:
    Filter(Transformation<Param>* nf = nullptr)
        :_next_filter(nf)
    {
    }
    
    void put(Param* p)override{
        if( this->transform(p) )
        {
            if(_next_filter != nullptr)
                _next_filter->put(p);
        }
    }
    
	void get(Param* p)override{

	}

protected:
    virtual bool transform(Param*& p)=0;
    
private:
	Transformation<Param>* _next_filter;
};

#endif /* FILTER_H */