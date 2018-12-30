#ifndef RUNNABLE
#define RUNNABLE

#include "eventqueue.h"


template <class T,class T2>
class Runnable
{
public:
    Runnable(EventQueue<T> &eq):_eq(eq) {}
    virtual void handleMsgIn(const T &msg)=0;
    virtual void setUp()=0;
    virtual void init(){;}
    virtual void tearDown(){;}
    virtual void executeAlways(){;}
    virtual void setUp( T2 *t){
        _appCtx = t;
        return setUp();
    }

protected:
    EventQueue<T> &_eq;
    T2 *_appCtx;

};
#endif // RUNNABLE
