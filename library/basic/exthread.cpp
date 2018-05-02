#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "exthread"
#endif

#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>

#include "exthread.h"
#include "exlog.h"

using namespace std;
using namespace vh1981lib;

exthread::exthread(const vh1981lib::exstring name) :
    _id(0),
	_tid(0),
    _name(name),
    _status(READY)
{
}

void exthread::setStatus(Status status)
{
    _status = status;
}

void exthread::readyThread()
{
    setStatus(STARTING);
}

void exthread::initThread()
{
    _id = ::getpid();
}

void exthread::runThread()
{
    started();
    setStatus(RUNNING);
    threadFunc();
    setStatus(FINISHING);
    finished();
}

void exthread::cleanupThread()
{
    setStatus(READY);

    _id = 0;
}

void exthread::_coreThreadFunc()
{
    initThread();
    runThread();
    cleanupThread();
}

bool exthread::run()
{
    int errorCode = 0;
    errorCode = ::pthread_create(&_tid, 0, coreThreadFunc, this);

    if (errorCode != 0) {
        setStatus(READY);
        EXCLOG(LOG_ERROR, "exthread::run() failed! %s(%d)", _name.to_string().c_str(), errorCode);
        return false;
    }

    sched_yield();
    return true;
}

void exthread::quit()
{
}

void exthread::join()
{
    if (!_tid) {
        return;
    }

    void* p = 0;
    ::pthread_join(_tid, &p);

    _tid = 0;

    EXCLOG(LOG_INFO, "exthread[%s] is joined", _name.to_string().c_str());
}

void* exthread::coreThreadFunc(void* param)
{
    exthread* thread = reinterpret_cast<exthread*>(param);
    thread->_coreThreadFunc();
    return 0;
}
