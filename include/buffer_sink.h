//  BufferSink.h


#ifndef BUFFER_SINK_H
#define BUFFER_SINK_H

#include "sink.h"
#include "av_exception.h"

template<class Param>
class BufferSink : Sink<Param>
{
public:
    void put(Param* p)override;

};

#endif /* BUFFER_SINK_H */
