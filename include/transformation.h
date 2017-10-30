///transformation.h

#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

template<class Param>
class Transformation
{
public:
	virtual void put(Param* p)=0;
	virtual void get(Param* p)=0;
};

#endif /*TRANSFORMATION_H*/
