#ifndef _MULE_H_
#define _MULE_H_

#include "library/basic/exthread.h"

namespace vh1981lib {

    /**
     @class Mule
     @brief 소켓 통신 처리 클래스
     */

    class Mule : public exthread {
    /**
     @name 생성자 / 소멸자
     */
    //@{
    public:
    	Mule();
        virtual ~Mule() {}
    //@}

    /**
     @name 속성
     */
    //@{
    private:
        // member variables

    public:
        // member functions
    //@}

    /**
     @name exthread function override
     */
    //@{
    private:
        void threadFunc();
    //@}

    };

} // namespace vh1981lib


#endif
