#ifndef _VH1981_EXTHREAD_
#define _VH1981_EXTHREAD_

#include "exstring.h"

#include <pthread.h>

namespace vh1981lib {

    /**
     @class exthread
     @brief 스레드 클래스.
     */

    class exthread {
    /**
     @name 생성자 / 소멸자
     */
    //@{
    public:
        exthread(const vh1981lib::exstring name);

        virtual ~exthread() {}
    //@}

    public:
        enum Status {
            READY,
            STARTING,
            RUNNING,
            FINISHING
        };

    /**
     @name 속성
     */
    //@{
    private:
        unsigned int _id;
        pthread_t _tid;
        vh1981lib::exstring _name;
        Status _status;

    public:
        unsigned int id() { return _id; }
        vh1981lib::exstring name() { return _name; }
        void setName(vh1981lib::exstring name) { _name = name; }
        void setStatus(Status status);
    //@}

    private:
        void readyThread();
        void initThread();
        void runThread();
        void cleanupThread();
        void _coreThreadFunc();

    public:
        virtual void starting() {}
        virtual void started() {}
        virtual void threadFunc() = 0;
        virtual void finished() {}

        static void* coreThreadFunc(void* param);

    public:
        bool run();
        void quit();
        void join();

    };

} // namespace vh1981lib


#endif
