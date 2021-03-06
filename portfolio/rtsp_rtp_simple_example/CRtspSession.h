// MediaLAN 02/2013
// CRtspSession
// - parsing of RTSP requests and generation of RTSP responses


#ifndef _CRtsp_Session_H
#define _CRtsp_Session_H

#include "RtspTypes.h"
#include "CStreamer.h"
#include "library/basic/exstring.h"

#define RTSP_BUFFER_SIZE       10000    // for incoming requests, and outgoing responses
#define RTSP_PARAM_STRING_MAX  500


using namespace std;

class CRtspSession
{
public:
    CRtspSession(SOCKET aRtspClient, CStreamer * aStreamer);
    ~CRtspSession();

    RTSP_CMD_TYPES Handle_RtspRequest(char const * aRequest, unsigned aRequestSize);
    int            GetStreamID();

private:
    void Init();
public:
    RTSP_CMD_TYPES getDirective(const vh1981lib::exstring& line);
    bool ParseRtspRequest(char const * aRequest, unsigned aRequestSize);
private:
    char const * DateHeader();

    // RTSP request command handlers
    void Handle_RtspOPTION();
    void Handle_RtspDESCRIBE();
    void Handle_RtspSETUP(); 
    void Handle_RtspPLAY();

    // RTSP send response
    void sendResponse(char* data, size_t len);

    // Utility function FIXME: 위치를 나중에 정리할것
    vh1981lib::exstring directiveString(RTSP_CMD_TYPES type);

    // global session state parameters
    unsigned int            m_RtspSessionID;
    SOCKET         m_RtspClient;                              // RTSP socket of that session
    int            m_StreamID;                                // number of simulated stream of that session
    u_short        m_ClientRTPPort;                           // client port for UDP based RTP transport
    u_short        m_ClientRTCPPort;                          // client port for UDP based RTCP transport  
    bool           m_TcpTransport;                            // if Tcp based streaming was activated
    CStreamer    * m_Streamer;                                // the UDP or TCP streamer of that session
    uint32_t       m_ipAddr;                                  // ip address(host order)
    char buf[200];

    // parameters of the last received RTSP request

    RTSP_CMD_TYPES m_RtspCmdType;                             // command type (if any) of the current request
    //char           m_URLPreSuffix[RTSP_PARAM_STRING_MAX];     // stream name pre suffix
    //char           m_URLSuffix[RTSP_PARAM_STRING_MAX];        // stream name suffix
    //char           m_CSeq[RTSP_PARAM_STRING_MAX];             // RTSP command sequence number
    //char           m_URLHostPort[RTSP_BUFFER_SIZE];           // host:port part of the URL
    unsigned       m_ContentLength;                           // SDP string size

public:
    vh1981lib::exstring _source;
    vh1981lib::exstring _path;
    vh1981lib::exstring _range;
    RTSP_CMD_TYPES _type;
    ushort _hostPort;
    ushort _clientPort;
    unsigned int _session;
    unsigned int _contentLength;
    size_t _cseq;

public:
    u_short clientRtpPort() { return m_ClientRTPPort; }
};

#endif
