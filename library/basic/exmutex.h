#ifndef _EXMUTEX_H_
#define _EXMUTEX_H_

#include <pthread.h>

namespace vh1981lib {

    /**
     @class exmutex
     @brief 동기화 클래스
     */

    class exmutex {
    /**
     @name 생성자 / 소멸자
     */
    //@{
    public:
        exmutex();
        virtual ~exmutex();
    //@}

    /**
     @name 속성
     */
    //@{
    private:
        // member variables
        pthread_mutex_t _mutex;
        pthread_mutexattr_t _attr;

    public:
        // member functions
        void lock();
        void unlock();
    //@}

    /**
     @brief copy 생성자 및 대입 연산자
     @details 복사되서는 안되므로 private으로 해둔다.
     */
    //@{
    private:
        exmutex(const exmutex&);
        exmutex& operator=(const exmutex&);
    //@}

    };

    class autoexmutex {
    /**
     @name 생성자 / 소멸자
     */
    //@{
    public:
        explicit autoexmutex(exmutex& m);
        ~autoexmutex();
    //@}

    /**
     @name 속성
     */
    //@{
    private:
        exmutex& _mutex;
    //@}

    /**
     @brief copy 생성자 및 대입 연산자
     @details 복사되서는 안되므로 private으로 해둔다.
     */
    //@{
    private:
        autoexmutex(const autoexmutex&);
        autoexmutex& operator=(const autoexmutex&);
    //@}

    };

} // namespace vh1981lib


#endif
