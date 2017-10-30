//  Sink.h


#ifndef SINK_H
#define SINK_H

#include "transformation.h"


template<class Param>
class Sink : public Transformation<Param>
{
public:
	Sink()
	{
	}

	void put(Param* p)override {
		
	}

	size_t get(Param* p)override {

	}

protected:
	bool transform(Param*& p)override{};
};

#endif /* SINK_H */
