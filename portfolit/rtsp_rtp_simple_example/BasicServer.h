#ifndef _BASIC_SERVER_
#define _BASIC_SERVER_

#include <netinet/in.h>

#include <list>

#include "Packet.h"

namespace vh1981lib {

    /**
     @class Session
     @brief 소켓 통신 정보 관리
     */

    class Session {

    public:
        enum SessionType {
            NORMAL,
            LISTENING
        };

    /**
     @name 생성자 / 소멸자
     */
    //@{
    public:
    	Session(SessionType type = NORMAL);
        virtual ~Session() {}
    //@}



    public:
        static const int INVALID_SOCKET;

    /**
     @name 속성
     */
    //@{
    private:
        // member variables
        SessionType _type;
        int _socket;
        sockaddr_in _address;
        std::list< std::shared_ptr<Packet> > _recvPacketQueue;

    public:
        SessionType sessionType() { return _type; }
        std::shared_ptr<Packet> getRecvPacket();
        bool putRecvPacket(std::shared_ptr<Packet> packet);
        std::shared_ptr<Packet> getSendPacket();
        bool putSendPacket(std::shared_ptr<Packet> packet);
        // member functions
    //@}

    };

} // namespace vh1981lib


#endif
