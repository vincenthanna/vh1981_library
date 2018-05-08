#ifndef _SESSION_H_
#define _SESSION_H_

#include <netinet/in.h>

#include <list>

#include "library/basic/exmutex.h"

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
        int _serverSocket;
        sockaddr_in _address;
        exmutex _recvPacketQueueMutex;
        std::list< std::shared_ptr<Packet> > _recvPacketQueue;
        exmutex _sendPacketQueueMutex;
        std::list< std::shared_ptr<Packet> > _sendPacketQueue;

    public:
        int socket() const { return _socket; }
        int serverSocket() const { return _serverSocket; }
        int getSocket() const;
        void setSocket(int socket) { _socket = socket; }
        void setServerSocket(int socket) { _serverSocket = socket; }
        SessionType sessionType() const { return _type; }
        bool isListening() const { return _type == LISTENING; }
        void setSessionType(SessionType type) { _type = type; }
        std::shared_ptr<Packet> getRecvPacket();
        bool putRecvPacket(std::shared_ptr<Packet> packet);
        std::shared_ptr<Packet> getSendPacket();
        bool putSendPacket(std::shared_ptr<Packet> packet);

        int recvPacketCount();
        int sendPacketCount();
        // member functions
    //@}

    };

} // namespace vh1981lib


#endif
