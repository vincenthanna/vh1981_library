#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "SessionThreadHandlerTest"
#endif

using namespace std;

#include "CStreamer.h"
#include "CRtspSession.h"
#include "JPEGSamples.h"
#include <pthread.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "library/basic/exlog.h"

#include "library/jrtplib/rtpsession.h"
#include "library/jrtplib/rtpudpv4transmitter.h"
#include "library/jrtplib/rtpipv4address.h"
#include "library/jrtplib/rtpsessionparams.h"
#include "library/jrtplib/rtperrors.h"
#include "library/jrtplib/rtplibraryversion.h"

#include <iostream>

using namespace vh1981lib;

using namespace jrtplib;

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

        EXCLOG(LOG_INFO, "RecvBuf :\n%s\n", RecvBuf);

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
            sleep(1);
        }

        printf("[%s]%d\n", __FUNCTION__,__LINE__);
    }
    close(Client);
    return 0;
};



////////////////////////////////////////////////////////////////////////////////////////////////////



void* SessionThreadHandler_JRTP(void* lpParam)
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

    // 44100 * 2 * 60
    const unsigned int packetSize = 44100 / 50;
    const unsigned int packetCount = 50 * 2 * 30;
    const unsigned int audioDataSize = (packetSize * packetCount);
    unsigned int packetOffset = 0;
    unsigned char audiobuf[audioDataSize + 1000];
    memset(audiobuf, 0x0, sizeof(audiobuf));
    FILE *f = fopen("./sample.wav", "r");
    if (f) {
        size_t offset = 0;
        int n_size;

        fread(&audiobuf[0], 1, 44, f); //< skip wav header

        while(offset < audioDataSize) {
            if ( 0 < (n_size = fread(&audiobuf[offset], 1, packetSize, f))) {
                //EXCLOG(LOG_INFO, "n_size=%d offset=%d", n_size, offset);
                offset += n_size;
            }
            else {
                break;
            }
        }
    }
    else {
        EXCLOG(LOG_ERROR, "can't open file!");
    }

    //////////////////////////////////////////////////////////////////////

    RTPSession sess;
    uint16_t portbase,destport;
    uint32_t destip;
    std::string ipstr;
    int status,i,num;
    RTPSessionParams sessionparams;
    sessionparams.SetOwnTimestampUnit(1.0/8000.0);
    sessionparams.SetAcceptOwnPackets(true);
    RTPUDPv4TransmissionParams transparams;
    transparams.SetPortbase(9000);


    {
        std::cout << "RTPLibrary : Using version " << RTPLibraryVersion::GetVersion().GetVersionString() << std::endl;
    }


    //////////////////////////////////////////////////////////////////////

    /**
     * 서버에서는 RTSP Request를 수신해서 처리하고 영상을 보낸다.
     */
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    while (!Stop) {
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

        EXCLOG(LOG_INFO, "RecvBuf : \n%s\n<<<<<< end >>>>>>", RecvBuf);
        // we filter away everything which seems not to be an RTSP command: O-ption, D-escribe, S-etup, P-lay, T-eardown
        if ((RecvBuf[0] == 'O') || (RecvBuf[0] == 'D') || (RecvBuf[0] == 'S') || (RecvBuf[0] == 'P') || (RecvBuf[0] == 'T'))
        {
            RTSP_CMD_TYPES C = RtspSession.Handle_RtspRequest(RecvBuf,res);
            if (C == RTSP_PLAY) {
                EXCLOG(LOG_INFO, "Streaming Started!");
                StreamingStarted = true;
            }
            else if (C == RTSP_TEARDOWN) {
                Stop = true;
            }

            //transparams.SetPortbase(RtspSession.clientRtpPort());
        }


        if (StreamingStarted) {
            if (!sess.IsActive()) {
                // create
                int status = sess.Create(sessionparams, &transparams);
                if (status < 0) {
                    EXCLOG(LOG_ERROR, "RTPSession.Create() fail : %s", RTPGetErrorString(status).c_str());
                }
            }

            // add target address
            RTPIPv4Address addr(ntohl(inet_addr("127.0.0.1")), RtspSession.clientRtpPort());
            EXCLOG(LOG_INFO, "RtspSession.clientRtpPort()=%d", RtspSession.clientRtpPort());
            status = sess.AddDestination(addr);
            if (status < 0) {
                EXCLOG(LOG_ERROR, "RTPSession.AddDestination() fail : %s", RTPGetErrorString(status).c_str());
            }
            else {
                EXCLOG(LOG_INFO, "RTPSession.AddDestination() SUCCEEDED! : (%s : %d)","127.0.0.1", RtspSession.clientRtpPort());
            }


            sess.SetDefaultPayloadType(0);
            sess.SetDefaultMark(false);
            sess.SetDefaultTimestampIncrement(3000);

            sess.SetTimestampUnit(1.0f / 44100.0f);
            for (int i = 0; i < 10; i++) {
                status = sess.SendPacket((void *)&audiobuf[packetOffset], packetSize, 0, true, 441);

                if (status < 0) {
                    EXCLOG(LOG_ERROR, "status=%d", status);
                }
                packetOffset += packetSize;
                if (packetOffset >= audioDataSize) {
                    packetOffset = 0;
                }
            }

            // INCOMING DATA PROCESSING //
            sess.BeginDataAccess();                                         // $$ From BeginDataAccess to EndDataAccess: "The processing of incoming data"

            // check incoming packets
            if (sess.GotoFirstSourceWithData())                             // $$
            {
                do
                {
                    RTPPacket *pack;

                    while ((pack = sess.GetNextPacket()) != NULL)
                    {
                        // You can examine the data here
                        EXCLOG(LOG_FATAL, "Got packet !\n");
                        // $$ Here we can do whatever we want with the received package
                        // we don't longer need the packet, so
                        // we'll delete it
                        sess.DeletePacket(pack);
                    }
                } while (sess.GotoNextSourceWithData());
            }

            sess.EndDataAccess();
            //END OF INCOMING DATA PROCESSING //
            RTPTime::Wait(RTPTime(0,100000));

        }

        printf("[%s]%d\n", __FUNCTION__,__LINE__);
    }
    close(Client);
    return 0;
}

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
    if (::bind(MasterSocket,(sockaddr*)&ServerAddr,sizeof(ServerAddr)) != 0) return 0;
    if (listen(MasterSocket,5) != 0) return 0;

    while (true)  
    {   // loop forever to accept client connections
        ClientSocket = accept(MasterSocket,(struct sockaddr*)&ClientAddr,&ClientAddrLen);
        const char* addr = inet_ntoa(ClientAddr.sin_addr);
        EXCLOG(LOG_INFO, "accepted address : %s", addr);
        //CreateThread(NULL,0,SessionThreadHandler,&ClientSocket,0,&TID);
        pthread_t handle;
        size_t status;
        pthread_create(&handle, NULL, SessionThreadHandler_JRTP, (void**)&ClientSocket);
        printf("Client connected. Client address: %s\r\n",inet_ntoa(ClientAddr.sin_addr));  
    }  

    close(MasterSocket);

    return 0;  
} 
