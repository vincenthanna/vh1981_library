// MediaLAN 02/2013
// CRtspSession
// - JPEG packetizer and UDP/TCP based streaming

#ifndef _CStreamer_H
#define _CStreamer_H

//#include <Winsock2.h>
//#include <Windows.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "types.h"


class CStreamer
{
public:
    CStreamer(SOCKET aClient);
    ~CStreamer();

    void    InitTransport(uint16_t aRtpPort, uint16_t aRtcpPort, bool TCP);
    uint16_t GetRtpServerPort();
    uint16_t GetRtcpServerPort();
    void    StreamImage(int StreamID);

private:
    void    SendRtpPacket(unsigned char * Jpeg, int JpegLen, int Chn);

    SOCKET  m_RtpSocket;          // RTP socket for streaming RTP packets to client
    SOCKET  m_RtcpSocket;         // RTCP socket for sending/receiving RTCP packages

    uint16_t m_RtpClientPort;      // RTP receiver port on client
    uint16_t m_RtcpClientPort;     // RTCP receiver port on client
    uint16_t m_RtpServerPort;      // RTP sender port on server
    uint16_t m_RtcpServerPort;     // RTCP sender port on server

    uint16_t m_SequenceNumber;
    uint32_t   m_Timestamp;
    int     m_SendIdx;
    bool    m_TCPTransport;
    SOCKET  m_Client;
};

#endif
