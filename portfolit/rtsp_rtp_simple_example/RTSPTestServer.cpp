#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "SessionThreadHandlerTest"
#endif

// MediaLAN 02/2013
// CRtspSession
// - Main and streamer master listener socket
// - Client session thread

//#include "stdafx.h"

//#include <Winsock2.h>
//#include <windows.h>
//#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#include "CStreamer.h"
#include "CRtspSession.h"
#include <pthread.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "library/basic/exlog.h"

using namespace vh1981lib;


void* SessionThreadHandler(void* lpParam)
{
    SOCKET Client = *(SOCKET*)lpParam;

    char         RecvBuf[10000];                    // receiver buffer
    int          res;  
    CStreamer    Streamer(Client);                  // our streamer for UDP/TCP based RTP transport
    CRtspSession RtspSession(Client,&Streamer);     // our threads RTSP session and state
    int          StreamID = 0;                      // the ID of the 2 JPEG samples streams which we support
    uint32_t       WaitEvents[2];                     // the waitable kernel objects of our session

    // set frame rate timer
    double T = 40.0;                                       // frame rate
    int iT   = T;

    bool StreamingStarted = false;
    bool Stop             = false;
    struct timeval tv;

    fd_set readsets;
    FD_ZERO(&readsets);
    FD_SET(Client, &readsets);

    /**
     * 서버에서는 RTSP Request를 수신해서 처리하고 영상을 보낸다.
     */
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    while (!Stop) {
        printf("[%s]%d\n", __FUNCTION__,__LINE__);
        fd_set tmpSet = readsets;
        int state = select(Client + 1, &tmpSet, NULL, NULL, &tv);
        if (state > -1) {
            if (FD_ISSET(Client, &tmpSet)) {
                memset(RecvBuf,0x00,sizeof(RecvBuf));
                res = recv(Client, RecvBuf, sizeof(RecvBuf), 0);
            }
        }

        if (res == 0) {
            Stop = true;
        }

        EXCLOG(LOG_INFO, "RecvBuf : %s", RecvBuf);
        // we filter away everything which seems not to be an RTSP command: O-ption, D-escribe, S-etup, P-lay, T-eardown
        if ((RecvBuf[0] == 'O') || (RecvBuf[0] == 'D') || (RecvBuf[0] == 'S') || (RecvBuf[0] == 'P') || (RecvBuf[0] == 'T'))
        {
            RTSP_CMD_TYPES C = RtspSession.Handle_RtspRequest(RecvBuf,res);
            if (C == RTSP_PLAY) {
                StreamingStarted = true;
            }
            else if (C == RTSP_TEARDOWN) {
                Stop = true;
            }
        }


        if (StreamingStarted) {
            Streamer.StreamImage(RtspSession.GetStreamID());
        }

        printf("[%s]%d\n", __FUNCTION__,__LINE__);
    }
    close(Client);
    return 0;
};

int main()
{    
    SOCKET      MasterSocket;                                 // our masterSocket(socket that listens for RTSP client connections)  
    SOCKET      ClientSocket;                                 // RTSP socket to handle an client
    sockaddr_in ServerAddr;                                   // server address parameters
    sockaddr_in ClientAddr;                                   // address parameters of a new RTSP client
    unsigned int         ClientAddrLen = sizeof(ClientAddr);
    //WSADATA     WsaData;
    //DWORD       TID;

    ServerAddr.sin_family      = AF_INET;   
    ServerAddr.sin_addr.s_addr = INADDR_ANY;   
    ServerAddr.sin_port        = htons(8554);                 // listen on RTSP port 8554
    MasterSocket               = socket(AF_INET,SOCK_STREAM,0);

    int iSetOption = 1;
    setsockopt(MasterSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,
            sizeof(iSetOption));


    // bind our master socket to the RTSP port and listen for a client connection
    if (bind(MasterSocket,(sockaddr*)&ServerAddr,sizeof(ServerAddr)) != 0) return 0;
    if (listen(MasterSocket,5) != 0) return 0;

    while (true)  
    {   // loop forever to accept client connections
        ClientSocket = accept(MasterSocket,(struct sockaddr*)&ClientAddr,&ClientAddrLen);
        //CreateThread(NULL,0,SessionThreadHandler,&ClientSocket,0,&TID);
        pthread_t handle;
        size_t status;
        pthread_create(&handle, NULL, SessionThreadHandler, (void**)&ClientSocket);
        printf("Client connected. Client address: %s\r\n",inet_ntoa(ClientAddr.sin_addr));  
    }  

    close(MasterSocket);

    return 0;  
} 
