//  Sink.h


#ifndef SINK_H
#define SINK_H

#include "transformation.h"
#include "av_exception.h"

template<class Param>
class Sink : public Transformation<Param>
{

private:
	Param* get()override {
		throw AvException("not implemented!");
	}
};

#endif /* SINK_H */
