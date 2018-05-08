#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "BasicServer"
#endif

#include <sys/socket.h>

#include "library/basic/exlog.h"

#include "Session.h"

using namespace std;
using namespace vh1981lib;

const int Session::INVALID_SOCKET = -1;

Session::Session(SessionType type) : _type(type),
    _socket(INVALID_SOCKET),
    _serverSocket(INVALID_SOCKET)
{
}

int Session::getSocket() const
{
    if (_socket == INVALID_SOCKET) {
        return _serverSocket;
    }
    return _socket;
}

std::shared_ptr<Packet> Session::getRecvPacket()
{
    autoexmutex a(_recvPacketQueueMutex);
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
    autoexmutex a(_recvPacketQueueMutex);
    if (packet.get()) {
        _recvPacketQueue.push_back(packet);
        return true;
    }
    return false;
}

std::shared_ptr<Packet> Session::getSendPacket()
{
    autoexmutex a(_sendPacketQueueMutex);
    return shared_ptr<Packet>(nullptr);
}

bool Session::putSendPacket(std::shared_ptr<Packet> packet)
{
    autoexmutex a(_sendPacketQueueMutex);
    return true;
}

int Session::recvPacketCount()
{
    autoexmutex a(_recvPacketQueueMutex);
    return _recvPacketQueue.size();
}

int Session::sendPacketCount()
{
    autoexmutex a(_sendPacketQueueMutex);
    return _sendPacketQueue.size();
}
