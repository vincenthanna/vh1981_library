#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "exmutex"
#endif

#include "exmutex.h"

using namespace vh1981lib;

exmutex::exmutex()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&_mutex, &attr);
}

exmutex::~exmutex()
{
    pthread_mutex_destroy(&_mutex);
}

void exmutex::lock()
{
    pthread_mutex_lock(&_mutex);
}

void exmutex::unlock()
{
    pthread_mutex_unlock(&_mutex);
}

autoexmutex::autoexmutex(exmutex& m) :_mutex(m)
{
    _mutex.lock();
}

autoexmutex::~autoexmutex()
{
    _mutex.unlock();
}
