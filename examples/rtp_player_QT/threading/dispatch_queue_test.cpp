#include "executable_queue.h"
#include "thread.h"
#include "dispatcher.h"

#include <iostream>

using namespace std;
using namespace arsee;

void f(int i)
{
    cout<<"f:"<<"i="<<i<<endl;
}

void g(int i, const char* a)
{
    cout<<"g:"<<"i="<<i<<",a="<<a<<endl;
}

int main(int argc, char *argv[])
{
    ExecutableQueue q1, q2;
    Thread<ExecutableQueue> t1( &q1 );
    Thread<ExecutableQueue> t2( &q2 );
    dispatch_sync(&q1, f, 10);
    dispatch_asyn(&q2, g, 11, "abc");
    t1.join();
}
