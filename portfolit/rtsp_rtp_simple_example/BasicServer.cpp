#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "BasicServer"
#endif

#include <sys/socket.h>

#include "library/basic/exlog.h"

#include "BasicServer.h"

using namespace std;
using namespace vh1981lib;

const int Session::INVALID_SOCKET = -1;

Session::Session(SessionType type) : _type(type),
    _socket(INVALID_SOCKET)
{
}

std::shared_ptr<Packet> Session::getRecvPacket()
{
    if(_recvPacketQueue.size()) {
        auto packet = _recvPacketQueue.front();
        if (packet.get()) {
            _recvPacketQueue.pop_front();
            return packet;
        }
    }
    return shared_ptr<Packet>(nullptr);
}

bool Session::putRecvPacket(std::shared_ptr<Packet> packet)
{
    if (packet.get()) {
        _recvPacketQueue.push_back(packet);
        return true;
    }
    return false;
}

std::shared_ptr<Packet> Session::getSendPacket()
{
    return shared_ptr<Packet>(nullptr);
}

bool Session::putSendPacket(std::shared_ptr<Packet> packet)
{
    return true;
}
