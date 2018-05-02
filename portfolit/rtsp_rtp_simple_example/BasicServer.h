#ifndef _BASIC_SERVER_
#define _BASIC_SERVER_

#include <netinet/in.h>

namespace vh1981lib {

    /**
     @class BasicServer
     @brief 소켓 서버 클래스 기본구현
     */

    class Session {
    /**
     @name 생성자 / 소멸자
     */
    //@{
    public:
    	Session();
        virtual ~Session() {}
    //@}

    enum SessionType {
    	NORMAL,
    	LISTENING
    };

    /**
     @name 속성
     */
    //@{
    private:
        // member variables
        int _socket;
        sockaddr_in _address;

    public:
        // member functions
    //@}

    };

} // namespace vh1981lib


#endif
