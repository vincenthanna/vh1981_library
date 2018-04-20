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

        EXCLOG(LOG_INFO, "RecvBuf :\n%s\n=====================================", RecvBuf);
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

//    unsigned char imgbuf[1024 * 1024];
//    int n_size = 0;
//    size_t offset = 0;
//    FILE *f = fopen("./sample.jpg", "r");
//    if (f) {
//        while( 0 < (n_size = fread(&imgbuf[offset], 1, 1024, f))) {
//            offset += n_size;
//        }
//    }
//    else {
//        EXCLOG(LOG_ERROR, "can't open file!");
//    }
//    size_t imgsize = offset;
//    EXCLOG(LOG_INFO, "imgsize=%d", imgsize);


    unsigned int packetSize = 1600;
    unsigned int packetCount = 10 * 60;
    const unsigned int audioDataSize = (packetSize * packetCount);
    unsigned int packetOffset = 0;
    unsigned char audiobuf[1600 * 10 * 60 + 1000];
    memset(audiobuf, 0x0, sizeof(audiobuf));
    FILE *f = fopen("./sample.wav", "r");
    if (f) {
        size_t offset = 0;
        int n_size;

        fread(&audiobuf[0], 1, 44, f); //< skip wav header

        while(offset < audioDataSize) {
            if ( 0 < (n_size = fread(&audiobuf[offset], 1, packetSize, f))) {
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



#if 0
            {
                static int m_SendIdx = 0;
                unsigned char  * Samples1[2] = { JpegScanDataCh1A, JpegScanDataCh1B };
                unsigned char  * Samples2[2] = { JpegScanDataCh2A, JpegScanDataCh2B };
                unsigned char ** JpegScanData;
                unsigned int     JpegScanDataLen;

                switch (StreamID)
                {
                    case 0:
                    {
                        JpegScanData    = &Samples1[0];
                        JpegScanDataLen = KJpegCh1ScanDataLen;
                        break;
                    };
                    case 1:
                    {
                        JpegScanData    = &Samples2[0];
                        JpegScanDataLen = KJpegCh2ScanDataLen;
                        break;
                    };
                };

                status = sess.SendPacket(JpegScanData[m_SendIdx], JpegScanDataLen);
                m_SendIdx++;
                if (m_SendIdx > 1) m_SendIdx = 0;

#if 0
                unsigned char        RtpBuf[2048];
                {
                    memset(RtpBuf,0x00,sizeof(RtpBuf));
                    // Prepare the first 4 byte of the packet. This is the Rtp over Rtsp header in case of TCP based transport
                    //--------------------------------------------------------------------------------------------------
//                    RtpBuf[0]  = '$';        // magic number
//                    RtpBuf[1]  = 0;          // number of multiplexed subchannel on RTPS connection - here the RTP channel
//                    RtpBuf[2]  = (RtpPacketSize & 0x0000FF00) >> 8;
//                    RtpBuf[3]  = (RtpPacketSize & 0x000000FF);
                    //--------------------------------------------------------------------------------------------------

                    static size_t m_SequenceNumber = 0;
                    static size_t m_Timestamp = 0;
                    // Prepare the 12 byte RTP header
                    RtpBuf[4]  = 0x80;                               // RTP version
                    RtpBuf[5]  = 0x9a;                               // JPEG payload (26) and marker bit
                    RtpBuf[7]  = m_SequenceNumber & 0x0FF;           // each packet is counted with a sequence counter
                    RtpBuf[6]  = m_SequenceNumber >> 8;
                    RtpBuf[8]  = (m_Timestamp & 0xFF000000) >> 24;   // each image gets a timestamp
                    RtpBuf[9]  = (m_Timestamp & 0x00FF0000) >> 16;
                    RtpBuf[10] = (m_Timestamp & 0x0000FF00) >> 8;
                    RtpBuf[11] = (m_Timestamp & 0x000000FF);
                    RtpBuf[12] = 0x13;                               // 4 byte SSRC (sychronization source identifier)
                    RtpBuf[13] = 0xf9;                               // we just an arbitrary number here to keep it simple
                    RtpBuf[14] = 0x7e;
                    RtpBuf[15] = 0x67;
                    // Prepare the 8 byte payload JPEG header
                    RtpBuf[16] = 0x00;                               // type specific
                    RtpBuf[17] = 0x00;                               // 3 byte fragmentation offset for fragmented images
                    RtpBuf[18] = 0x00;
                    RtpBuf[19] = 0x00;
                    RtpBuf[20] = 0x01;                               // type
                    RtpBuf[21] = 0x5e;                               // quality scale factor
                    RtpBuf[22] = 0x28;                           // width  / 8 -> 48 pixel
                    RtpBuf[23] = 0x1E;                           // height / 8 -> 32 pixel

                    m_SequenceNumber++;
                    m_Timestamp += 3600;
                }
                status = sess.SendPacket(&RtpBuf[4], 20);


                if (status < 0) {
                    EXCLOG(LOG_ERROR, "RTPSession.SendPacket() fail : %s", RTPGetErrorString(status).c_str());
                }


                //SendRtpPacket(JpegScanData[m_SendIdx],JpegScanDataLen, StreamID);
                // Send data to session
                {
                    size_t off = 0;
                    size_t sndsize = 128;
                    while(off < imgsize) {
                        if (imgsize - off < sndsize) {
                            sndsize = imgsize - off;
                        }
                        status = sess.SendPacket(&imgbuf[off], sndsize);
                        off += sndsize;

                        if (status < 0) {
                            EXCLOG(LOG_ERROR, "RTPSession.SendPacket() fail : %s", RTPGetErrorString(status).c_str());
                        }
                        else {
                            //EXCLOG(LOG_INFO, "<<<<<<<<<<< RTPSession.SendPacket() SUCCEEDED!!! >>>>>>>>>>");
                        }
                    }

                    //status = sess.SendPacket(imgbuf, imgsize);

                }
#endif

            }
#endif

            char buffer[800];
            memset(buffer, 0x0, sizeof(buffer));
//            for (int i = 0; i < 10; i++) {
//                status = sess.SendPacket((void *)buffer, sizeof(buffer), 0, false, 10);
//                if (status < 0) {
//                    EXCLOG(LOG_ERROR, "RTPSession.SendPacket() fail : %s", RTPGetErrorString(status).c_str());
//                }
//                else {
//                    //EXCLOG(LOG_INFO, "RTPSession.SendPacket() SUCCEEDED!!!");
//                }
//            }
            for (int i = 0; i < 32; i++) {
                status = sess.SendPacket((void *)&audiobuf[packetOffset], packetSize, 0, false, 125);
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


            //Streamer.StreamImage(RtspSession.GetStreamID());
        }

        printf("[%s]%d\n", __FUNCTION__,__LINE__);
    }
    close(Client);
    return 0;
};

void checkerror(int rtperr)
{
    if (rtperr < 0)
    {
        std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
        exit(-1);
    }
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









#if 0
    {

        RTPSession sess;
        uint16_t portbase,destport;
        uint32_t destip;
        std::string ipstr;
        int status,i,num;

        std::cout << "Using version " << RTPLibraryVersion::GetVersion().GetVersionString() << std::endl;

        // First, we'll ask for the necessary information

//        std::cout << "Enter local portbase:" << std::endl;
//        std::cin >> portbase;
//        std::cout << std::endl;

//        std::cout << "Enter the destination IP address" << std::endl;
//        std::cin >> ipstr;
        destip = inet_addr("127.0.0.1");
        if (destip == INADDR_NONE)
        {
            std::cerr << "Bad IP address specified" << std::endl;
            return -1;
        }
        EXCLOG(LOG_INFO, "%s", ipstr.c_str());

        // The inet_addr function returns a value in network byte order, but
        // we need the IP address in host byte order, so we use a call to
        // ntohl
        destip = ntohl(destip);

//        std::cout << "Enter the destination port" << std::endl;
//        std::cin >> destport;
        destport = 10000;

//        std::cout << std::endl;
//        std::cout << "Number of packets you wish to be sent:" << std::endl;
//        std::cin >> num;

        num = 1000000;

        // Now, we'll create a RTP session, set the destination, send some
        // packets and poll for incoming data.

        RTPUDPv4TransmissionParams transparams;
        RTPSessionParams sessparams;

        // IMPORTANT: The local timestamp unit MUST be set, otherwise
        //            RTCP Sender Report info will be calculated wrong
        // In this case, we'll be sending 10 samples each second, so we'll
        // put the timestamp unit to (1.0/10.0)
        sessparams.SetOwnTimestampUnit(1.0/10.0);

        sessparams.SetAcceptOwnPackets(true);
        transparams.SetPortbase(9000);
        status = sess.Create(sessparams,&transparams);
        checkerror(status);

        RTPIPv4Address addr(destip,destport);

        status = sess.AddDestination(addr);
        checkerror(status);

        for (i = 1 ; i <= num ; i++)
        {
            printf("\nSending packet %d/%d\n",i,num);

            // send the packet
            status = sess.SendPacket((void *)"1234567890",10,0,false,10);
            checkerror(status);

            sess.BeginDataAccess();

            // check incoming packets
            if (sess.GotoFirstSourceWithData())
            {
                do
                {
                    RTPPacket *pack;

                    while ((pack = sess.GetNextPacket()) != NULL)
                    {
                        // You can examine the data here
                        printf("Got packet !\n");

                        // we don't longer need the packet, so
                        // we'll delete it
                        sess.DeletePacket(pack);
                    }
                } while (sess.GotoNextSourceWithData());
            }

            sess.EndDataAccess();

#ifndef RTP_SUPPORT_THREAD
            status = sess.Poll();
            checkerror(status);
#endif // RTP_SUPPORT_THREAD

            usleep(10000);
        }

        sess.BYEDestroy(RTPTime(10,0),0,0);

        exit(-1);
    }
#endif


#if 0
    {
        unsigned char imgbuf[1024 * 1024];
        int n_size = 0;
        size_t offset = 0;
        FILE *f = fopen("./sample.jpg", "r");
        if (f) {
            while( 0 < (n_size = fread(&imgbuf[offset], 1, 1024, f))) {
                offset += n_size;
            }
        }
        else {
            EXCLOG(LOG_ERROR, "can't open file!");
        }
        size_t imgsize = offset;
        EXCLOG(LOG_INFO, "imgsize=%d", imgsize);

        RTPSession sess;
        uint16_t portbase,destport;
        uint32_t destip;
        std::string ipstr;
        int status,i,num;
        RTPSessionParams sessionparams;
        sessionparams.SetOwnTimestampUnit(1.0/10.0);
        sessionparams.SetAcceptOwnPackets(true);
        RTPUDPv4TransmissionParams transparams;
        transparams.SetPortbase(9000);

        if (true) {
            //if (!sess.IsActive()) {
                // create
                int status = sess.Create(sessionparams, &transparams);
                if (status < 0) {
                    EXCLOG(LOG_ERROR, "RTPSession.Create() fail : %s", RTPGetErrorString(status).c_str());
                }

                // add target address
                if (inet_addr("127.0.0.1") == INADDR_NONE) {
                    exit(-1);
                }
                RTPIPv4Address addr(ntohl(inet_addr("127.0.0.1")), 10000);
                EXCLOG(LOG_INFO, "RtspSession.clientRtpPort()=%d", 10000);
                status = sess.AddDestination(addr);
                if (status < 0) {
                    EXCLOG(LOG_ERROR, "RTPSession.AddDestination() fail : %s", RTPGetErrorString(status).c_str());
                }

                sess.SetDefaultPayloadType(26);
                sess.SetDefaultMark(false);
                sess.SetDefaultTimestampIncrement(3000);
            //}




            {
                while(1) {
//                    unsigned char        RtpBuf[2048];
//                    {
//                        memset(RtpBuf,0x00,sizeof(RtpBuf));
//                        // Prepare the first 4 byte of the packet. This is the Rtp over Rtsp header in case of TCP based transport
//                        //--------------------------------------------------------------------------------------------------
//                        //                    RtpBuf[0]  = '$';        // magic number
//                        //                    RtpBuf[1]  = 0;          // number of multiplexed subchannel on RTPS connection - here the RTP channel
//                        //                    RtpBuf[2]  = (RtpPacketSize & 0x0000FF00) >> 8;
//                        //                    RtpBuf[3]  = (RtpPacketSize & 0x000000FF);
//                        //--------------------------------------------------------------------------------------------------
//
//                        static size_t m_SequenceNumber = 0;
//                        static size_t m_Timestamp = 0;
//                        // Prepare the 12 byte RTP header
//                        RtpBuf[4]  = 0x80;                               // RTP version
//                        RtpBuf[5]  = 0x9a;                               // JPEG payload (26) and marker bit
//                        RtpBuf[7]  = m_SequenceNumber & 0x0FF;           // each packet is counted with a sequence counter
//                        RtpBuf[6]  = m_SequenceNumber >> 8;
//                        RtpBuf[8]  = (m_Timestamp & 0xFF000000) >> 24;   // each image gets a timestamp
//                        RtpBuf[9]  = (m_Timestamp & 0x00FF0000) >> 16;
//                        RtpBuf[10] = (m_Timestamp & 0x0000FF00) >> 8;
//                        RtpBuf[11] = (m_Timestamp & 0x000000FF);
//                        RtpBuf[12] = 0x13;                               // 4 byte SSRC (sychronization source identifier)
//                        RtpBuf[13] = 0xf9;                               // we just an arbitrary number here to keep it simple
//                        RtpBuf[14] = 0x7e;
//                        RtpBuf[15] = 0x67;
//                        // Prepare the 8 byte payload JPEG header
//                        RtpBuf[16] = 0x00;                               // type specific
//                        RtpBuf[17] = 0x00;                               // 3 byte fragmentation offset for fragmented images
//                        RtpBuf[18] = 0x00;
//                        RtpBuf[19] = 0x00;
//                        RtpBuf[20] = 0x01;                               // type
//                        RtpBuf[21] = 0x5e;                               // quality scale factor
//                        RtpBuf[22] = 0x28;                           // width  / 8 -> 48 pixel
//                        RtpBuf[23] = 0x1E;                           // height / 8 -> 32 pixel
//
//                        m_SequenceNumber++;
//                        m_Timestamp += 3600;
//                    }
//                    status = sess.SendPacket(&RtpBuf[4], 20, 26, false, 0);


                    size_t off = 0;
                    size_t sndsize = 128;
                    while(off < imgsize) {
                        if (imgsize - off < sndsize) {
                            sndsize = imgsize - off;
                        }

                        status = sess.SendPacket(&imgbuf[off], sndsize, 26, sndsize == 128 ? false : true, sndsize == 128 ? 0 :10);
                        off += sndsize;

                        if (status < 0) {
                            EXCLOG(LOG_ERROR, "RTPSession.SendPacket() fail : %s", RTPGetErrorString(status).c_str());
                        }
                        else {
                            EXCLOG(LOG_INFO, "RTPSession.SendPacket() SUCCEEDED!!! sndSize=%d", sndsize);
                        }
                    }


#if 0
                    for (int i = 0; i < 10000; i++) {
                        status = sess.SendPacket((void *)"1234567890",10,0,false,10);
                        if (status < 0) {
                            EXCLOG(LOG_ERROR, "RTPSession.SendPacket() fail : %s", RTPGetErrorString(status).c_str());
                        }
                        else {
                            EXCLOG(LOG_INFO, "RTPSession.SendPacket() SUCCEEDED!!!");
                        }
                    }
#endif

                    EXCLOG(LOG_INFO, "running...");


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

                    status = sess.Poll();

                    //END OF INCOMING DATA PROCESSING //
                    RTPTime::Wait(RTPTime(1,0));
                }
            }

        }
    }
#endif









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
