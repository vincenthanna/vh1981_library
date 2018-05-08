#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "Mule"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "Mule.h"
#include "RTSPTestServer.h"

using namespace std;
using namespace vh1981lib;

Mule::Mule() : exthread(exstring("Mule")),
		_stopped(false)
{

}

void Mule::threadFunc()
{
	fd_set readFds;
	fd_set writeFds;
	fd_set exceptFds;
	int ret = 0;
	while(!_stopped) {
		int fd_max = makeAllFds(&readFds, &writeFds, &exceptFds);
		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		ret = select(fd_max + 1, &readFds, &writeFds, &exceptFds, &tv);
		if (ret < 0) {
			//TODO: all disconnect
			continue;
		}

		if (ret == 0) {

		}

		processAllFds(&readFds, &writeFds, &exceptFds);
	}
}

int Mule::makeAllFds(fd_set* readFds, fd_set* writeFds, fd_set* exceptFds)
{
    // TODO:
    RTSPTestServer::SessionsList& sessionsList = _server->sessionsList();
    for (auto session : sessionsList) {
        if (session->isListening()) {

        }
        else {
            if (session->socket() != Session::INVALID_SOCKET) {
                FD_SET(session->socket(), readFds);
                FD_SET(session->socket(), writeFds);
                FD_SET(session->socket(), exceptFds);
            }
        }
    }
}

void Mule::processAllFds(fd_set* readFds, fd_set* writeFds, fd_set* exceptFd)
{
	for (auto session : _server->sessionsList()) {
		if (session.get()) {
		    // fd를 socket등록할때 서버면 serversocket, 아니면 자기것.
		    // session에서 serverSocket을 가지고 있도록
            int fd = session->getSocket();

            if (FD_ISSET(fd, readFds)) {
                /**
                 fd를 socket등록할때 서버면 serversocket, 아니면 자기것.
                 session 상태를 확인하고 처리
                 */
                if (session->isListening()) {
                    // server session에서 처리할것이 현재 없음
                }
                else {
                    if (session->socket() == Session::INVALID_SOCKET) {
                        struct sockaddr_in addr;
                        ::memset(&addr, 0, sizeof(sockaddr_in));
                        socklen_t addrlen = sizeof(struct sockaddr);
                        int acceptedFd = accept(fd, (struct sockaddr*)&addr, &addrlen);
                        if (acceptedFd > 0) {
                            int flags = fcntl(acceptedFd, F_GETFL, 0);
                            fcntl(acceptedFd, F_SETFL, flags | O_NONBLOCK);
                            session->setSocket(acceptedFd);
                        }
                        FD_CLR(fd, readFds);
                        continue;
                    }
                    else {
                        // read packet
                        shared_ptr<Packet> packet(new Packet());
                        int numBytes = recv(session->socket(), packet->buffer(), packet->bufSize(), 0);
                        packet->setDataLen(numBytes);
                        session->putRecvPacket(packet);
                        FD_CLR(fd, readFds);
                    }
                }
            }

            if (FD_ISSET(fd, writeFds)) {
                shared_ptr<Packet> packet = session->getSendPacket();
                size_t remains = packet->dataLen();
                while (remains) {
                    int sent = send(fd, &(packet->buffer()[packet->dataLen() - remains]), remains, 0);
                    if (!sent) {
                        // FIXME : 연결 끊어진 것에 대한 추가 처리를 진행.
                        session->setSocket(Session::INVALID_SOCKET);
                        // TODO: clear all buffers
                    }
                    else {
                        remains -= sent;
                    }
                }

                FD_CLR(fd, writeFds);
            }
		}
	}
}
