#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "RTSPTestServer"
#endif

#include "RTSPTestServer.h"
#include "Mule.h"

using namespace std;
using namespace vh1981lib;

RTSPTestServer::RTSPTestServer() : _mulePtr(), _sessionList()
{
	_mulePtr.reset();
}

void RTSPTestServer::setMule(std::shared_ptr<Mule>& mule)
{
	_mulePtr = mule;
}

bool RTSPTestServer::addSession(const std::shared_ptr<Session>& sessionPtr)
{
    // need to check assignment ?
    _sessionList.push_back(sessionPtr);
    return true;
}

void RTSPTestServer::removeSession(Session* session)
{
    _sessionList.remove_if([session](shared_ptr<Session> p) {
        return p.get() == session;
    });
}
