#ifndef _RTSP_TEST_SERVER_
#define _RTSP_TEST_SERVER_

#include "Session.h"

namespace vh1981lib {

    /**
     @class RTSPTestServer
     @brief RTPS 테스트용 서버
     */
    class Mule;

    class RTSPTestServer {
    /**
     @name 생성자 / 소멸자
     */
    //@{
    public:
        RTSPTestServer();
        virtual ~RTSPTestServer() {}
    //@}

    public:
        typedef std::list< std::shared_ptr<Session> > SessionsList;

    /**
     @name 속성
     */
    //@{
    private:
        SessionsList _sessionList;
        Mule *_mule;

    // set references :
    public:
        void setMule(Mule* mule) { _mule = mule; }

    // sessions :
    public:
        SessionsList& sessionsList() { return _sessionList; }
        bool addSession(const std::shared_ptr<Session>& sessionPtr);
        void removeSession(Session* session);
    //@}

    };

} // namespace vh1981lib


#endif
