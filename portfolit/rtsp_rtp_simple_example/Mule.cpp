#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "Mule"
#endif

#include "Mule.h"

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

void Mule::processAllFds(fd_set* readFds, fd_set* writeFds, fd_set* exceptFd)
{
	for (auto session : _sessionList) {
		if (session.get()) { //< 이럴일은 없겠지만...

		}
	}
}
