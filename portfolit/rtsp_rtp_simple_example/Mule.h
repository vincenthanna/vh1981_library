#ifndef _MULE_H_
#define _MULE_H_

#include <sys/select.h>

#include "library/basic/exthread.h"

#include "RTSPTestServer.h"

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
        bool _stopped;
        RTSPTestServer::SessionsList& _sessionList;


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


    private:
        int makeAllFds(fd_set* readFds, fd_set* writeFds, fd_set* exceptFd);
        void processAllFds(fd_set* readFds, fd_set* writeFds, fd_set* exceptFd);


    };

} // namespace vh1981lib


#endif
