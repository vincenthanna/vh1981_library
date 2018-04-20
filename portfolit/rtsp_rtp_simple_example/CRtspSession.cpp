#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "CRtspSession"
#endif

// MediaLAN 02/2013
// CRtspSession
// - parsing of RTSP requests and generation of RTSP responses

#include "CRtspSession.h"
#include "RtspParser.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include "library/basic/exlog.h"

#include "RtspParser.h"
#include "JPEGSamples.h"

using namespace std;
using namespace vh1981lib;

CRtspSession::CRtspSession(SOCKET aRtspClient, CStreamer * aStreamer):m_RtspClient(aRtspClient),m_Streamer(aStreamer)
{
    Init();

    m_RtspSessionID  = rand() << 16;         // create a session ID
    m_RtspSessionID |= rand();
    //m_RtspSessionID |= 0x80000000;
    m_StreamID       = -1;
    m_ClientRTPPort  =  0;
    m_ClientRTCPPort =  0;
    m_TcpTransport   =  false;
};

CRtspSession::~CRtspSession()
{
}

void CRtspSession::Init()
{
    m_RtspCmdType   = RTSP_UNKNOWN;
    //memset(m_URLPreSuffix, 0x00, sizeof(m_URLPreSuffix));
    //memset(m_URLSuffix,    0x00, sizeof(m_URLSuffix));
    //memset(m_CSeq,         0x00, sizeof(m_CSeq));
    //memset(m_URLHostPort,  0x00, sizeof(m_URLHostPort));
    m_ContentLength  =  0;
};

RTSP_CMD_TYPES CRtspSession::getDirective(const exstring& line)
{
    //EXCLOG(LOG_INFO, "%s", line.to_string().c_str());
    for (auto directive : rtspDirectives) {
        //EXCLOG(LOG_INFO, "%s %d", directive.name.c_str(), directive.name.length());
        if (directive.name.compare(0, directive.name.length(), \
            line.to_string(), 0, directive.name.length()) == 0) {
            return (RTSP_CMD_TYPES)directive.cmdType;
        }
    }
    return RTSP_UNKNOWN;
}

bool CRtspSession::ParseRtspRequest(char const * aRequest, unsigned aRequestSize)
{
    std::istringstream f(aRequest);
    string line;
    int linenum = 0;

    m_RtspCmdType = getDirective(exstring(aRequest));

    EXCLOG(LOG_INFO, "RTSP_CMD_TYPES : %d", m_RtspCmdType);
    while(getline(f, line)) {
    	string url;

    	if (linenum == 0) {
    		stringstream stream(line);
    		string token;
    		stream >> token; // skip directive
    		stream >> url;

    		boost::regex regexPort(":([0-9]+)");
    		boost::smatch what;
    		string port;
    		if (boost::regex_search(url, what, regexPort)) {
    			string match(what[1].first, what[1].second);
    			port = match;
    			_hostPort = std::stoi(port, nullptr, 10);
    			EXCLOG(LOG_INFO, "_hostPort=%d", _hostPort);
    		}
    		else {
    			EXCLOG(LOG_ERROR, "can't get port number!!!");
    		}

    		size_t startpos = url.find_first_of('/', 8);
    		string path;
    		if (startpos != string::npos) {
    			startpos += 1;
    			string match(url.begin() + startpos, url.end());
    			_path = match;
    			EXCLOG(LOG_INFO, "_path=%s", _path.to_string().c_str());
    		}
    		else {
    			EXCLOG(LOG_ERROR, "can't get port path!!!");
    		}
    	}
    	else {
    		size_t startpos = string::npos;
    		if ((startpos = line.find(string("CSeq"))) != string::npos) {
    			//EXCLOG(LOG_INFO, "line=%s startpos=%d", line.c_str(), startpos);
    			boost::regex regexPort("([0-9]+)");
    			boost::smatch what;
    			string cseq;
    			if (boost::regex_search(line, what, regexPort)) {
    				string match(what[1].first, what[1].second);
    				boost::trim_right(match);
    				boost::trim_left(match);
    				if (match.length()) {
    				    _cseq = std::stoi(match, nullptr, 10);
    				    EXCLOG(LOG_INFO, "cseq=%d", _cseq);
    				}
    				else {
    				    EXCLOG(LOG_INFO, "read cseq failed!!!");
    				}
    			}
    			else {
    				EXCLOG(LOG_ERROR, "can't get cseq!!!");
    			}
    		}
    		else if ((startpos = line.find(string("Accept"))) != string::npos) {
    			string tmpstr = line;
    			string tmp2 = "Accept";
    			tmpstr.erase(tmpstr.find(tmp2), tmp2.length());
    			string chars = ":";
    			tmpstr = exstring::remove_chars(tmpstr, chars);
    			boost::trim_right(tmpstr);
    			boost::trim_left(tmpstr);
    			EXCLOG(LOG_INFO, "%s:%s",tmp2.c_str(), tmpstr.c_str());
    		}
    		else if ((startpos = line.find(string("User-Agent"))) != string::npos) {
    			string tmpstr = line;
    			string tmp2 = "User-Agent";
    			tmpstr.erase(tmpstr.find(tmp2), tmp2.length());
    			string chars = ":";
    			tmpstr = exstring::remove_chars(tmpstr, chars);
    			boost::trim_right(tmpstr);
    			boost::trim_left(tmpstr);
    			EXCLOG(LOG_INFO, "%s:%s",tmp2.c_str(), tmpstr.c_str());
    		}
    		else if ((startpos = line.find(string("client_port"))) != string::npos) {
    			string tmpstr = line;
    			string tmp2 = "client_port";
    			tmpstr.erase(0, startpos + tmp2.length());

    			boost::regex regexNumber("([0-9]+)");
    			boost::smatch what;
    			//EXCLOG(LOG_INFO, "tmpstr=%s", tmpstr.c_str());
    			boost::regex_search(tmpstr, what, regexNumber);
    			for (int i = 1; i < what.size(); i++) {
    				string match(what[i].first, what[i].second);
    				boost::trim_left(match);
    				boost::trim_right(match);
    				if (match.length()) {
    					m_ClientRTPPort = std::stoi(match, nullptr, 10);
    					m_ClientRTCPPort = m_ClientRTPPort + 1;
    					EXCLOG(LOG_INFO, "_clientPort: m_ClientRTPPort=%d m_ClientRTCPPort=%d", \
    							m_ClientRTPPort, m_ClientRTCPPort);
    				}
    				else {
    					EXCLOG(LOG_ERROR, "client_port scan failed!");
    				}
    			}
    		}
    		else if ((startpos = line.find(string("Session"))) != string::npos) {
    			string tmpstr = line;
    			string tmp2 = "Session";
    			tmpstr.erase(tmpstr.find(tmp2), tmp2.length());
    			string chars = ":";
    			tmpstr = exstring::remove_chars(tmpstr, chars);
    			boost::trim_right(tmpstr);
    			boost::trim_left(tmpstr);
    			if (tmpstr.length()) {
    				_session = stoi(tmpstr, nullptr, 10);
    				EXCLOG(LOG_INFO, "%s:%d",tmp2.c_str(), _session);
    			}
    			else {
    				EXCLOG(LOG_ERROR, "can't read %s element!!!", tmp2.c_str());
    			}

    		}
    		else if ((startpos = line.find(string("Range"))) != string::npos) {
    			string tmpstr = line;
    			string tmp2 = "Range";
    			tmpstr.erase(tmpstr.find(tmp2), tmp2.length());
    			string chars = ":";
    			tmpstr = exstring::remove_chars(tmpstr, chars);
    			boost::trim_right(tmpstr);
    			boost::trim_left(tmpstr);
    			if (tmpstr.length()) {
    				_range = tmpstr;
    				EXCLOG(LOG_INFO, "%s:%s",tmp2.c_str(), _range.to_string().c_str());
    			}
    			else {
    				EXCLOG(LOG_ERROR, "can't read %s element!!!", tmp2.c_str());
    			}
    		}
    		else if ((startpos = line.find(string("Content-Length"))) != string::npos) {
    		    string tmpstr = line;
    		    string tmp2 = "Content-Length";
    		    tmpstr.erase(tmpstr.find(tmp2), tmp2.length());
    		    string chars = ":";
    		    tmpstr = exstring::remove_chars(tmpstr, chars);
    		    boost::trim_right(tmpstr);
    		    boost::trim_left(tmpstr);
    		    if (tmpstr.length()) {
    		        _contentLength = stoi(tmpstr, nullptr, 10);
    		        EXCLOG(LOG_INFO, "%s:%d",tmp2.c_str(), _contentLength);
    		    }
    		    else {
    		        EXCLOG(LOG_ERROR, "can't read %s element!!!", tmp2.c_str());
    		    }
    		}
    		else if ((startpos = line.find(string("RTP/AVP"))) != string::npos) {
    		    if (line.find(string("RTP/AVP/TCP")) != string::npos) {
    		        m_TcpTransport = true;
    		    }
    		    else {
    		        m_TcpTransport = false;
    		    }
    		}
    	}

    	linenum++;
    }
    EXCLOG(LOG_INFO, "<<<end>>>");



#if 0
    char     CmdName[RTSP_PARAM_STRING_MAX];
    char     CurRequest[RTSP_BUFFER_SIZE];
    unsigned CurRequestSize; 

    Init();
    CurRequestSize = aRequestSize;
    memcpy(CurRequest,aRequest,aRequestSize);

    // check whether the request contains information about the RTP/RTCP UDP client ports (SETUP command)
    char * ClientPortPtr;
    char * TmpPtr;
    char   CP[1024];
    char * pCP;

    ClientPortPtr = strstr(CurRequest,"client_port");
    if (ClientPortPtr != nullptr)
    {
        TmpPtr = strstr(ClientPortPtr,"\r\n");
        if (TmpPtr != nullptr) 
        {
            TmpPtr[0] = 0x00;
            strcpy(CP,ClientPortPtr);
            pCP = strstr(CP,"=");
            if (pCP != nullptr)
            {
                pCP++;
                strcpy(CP,pCP);
                pCP = strstr(CP,"-");
                if (pCP != nullptr)
                {
                    pCP[0] = 0x00;
                    m_ClientRTPPort  = atoi(CP);
                    m_ClientRTCPPort = m_ClientRTPPort + 1;
                };
            };
        };
    };

    // Read everything up to the first space as the command name
    bool parseSucceeded = false;
    unsigned i;
    for (i = 0; i < sizeof(CmdName)-1 && i < CurRequestSize; ++i) 
    {
        char c = CurRequest[i];
        if (c == ' ' || c == '\t') 
        {
            parseSucceeded = true;
            break;
        }
        CmdName[i] = c;
    }
    CmdName[i] = '\0';
    if (!parseSucceeded) {
        printf("[%s]%d parse failed!\n", __FUNCTION__,__LINE__);
        return false;
    }

    // find out the command type
    if (strstr(CmdName,"OPTIONS")   != nullptr) m_RtspCmdType = RTSP_OPTIONS;  else
    if (strstr(CmdName,"DESCRIBE")  != nullptr) m_RtspCmdType = RTSP_DESCRIBE; else
    if (strstr(CmdName,"SETUP")     != nullptr) m_RtspCmdType = RTSP_SETUP;    else
    if (strstr(CmdName,"PLAY")      != nullptr) m_RtspCmdType = RTSP_PLAY;     else
    if (strstr(CmdName,"TEARDOWN")  != nullptr) m_RtspCmdType = RTSP_TEARDOWN;

    EXCLOG(LOG_INFO, "m_RtspCmdType=%d", m_RtspCmdType);

    // check whether the request contains transport information (UDP or TCP)
    if (m_RtspCmdType == RTSP_SETUP)
    {
        TmpPtr = strstr(CurRequest,"RTP/AVP/TCP");
        if (TmpPtr != nullptr) m_TcpTransport = true; else m_TcpTransport = false;
    };

    // Skip over the prefix of any "rtsp://" or "rtsp:/" URL that follows:
    unsigned j = i+1;
    while (j < CurRequestSize && (CurRequest[j] == ' ' || CurRequest[j] == '\t')) ++j; // skip over any additional white space
    for (; (int)j < (int)(CurRequestSize-8); ++j) 
    {
        if ((CurRequest[j]   == 'r' || CurRequest[j]   == 'R')   && 
            (CurRequest[j+1] == 't' || CurRequest[j+1] == 'T') && 
            (CurRequest[j+2] == 's' || CurRequest[j+2] == 'S') && 
            (CurRequest[j+3] == 'p' || CurRequest[j+3] == 'P') && 
             CurRequest[j+4] == ':' && CurRequest[j+5] == '/') 
        {
            j += 6;
            if (CurRequest[j] == '/') 
            {   // This is a "rtsp://" URL; skip over the host:port part that follows:
                ++j;
                unsigned uidx = 0;
                while (j < CurRequestSize && CurRequest[j] != '/' && CurRequest[j] != ' ') 
                {   // extract the host:port part of the URL here
                    m_URLHostPort[uidx] = CurRequest[j];
                    uidx++;
                    ++j;
                };
            } 
            else --j;
            i = j;
            break;
        }
    }

    // Look for the URL suffix (before the following "RTSP/"):
    parseSucceeded = false;
    for (unsigned k = i+1; (int)k < (int)(CurRequestSize-5); ++k) 
    {
        if (CurRequest[k]   == 'R'   && CurRequest[k+1] == 'T'   && 
            CurRequest[k+2] == 'S'   && CurRequest[k+3] == 'P'   && 
            CurRequest[k+4] == '/') 
        {
            while (--k >= i && CurRequest[k] == ' ') {}
            unsigned k1 = k;
            while (k1 > i && CurRequest[k1] != '/') --k1;
            if (k - k1 + 1 > sizeof(m_URLSuffix)) {
            	return false;
            }
            unsigned n = 0, k2 = k1+1;

            while (k2 <= k) m_URLSuffix[n++] = CurRequest[k2++];
            m_URLSuffix[n] = '\0';

            if (k1 - i > sizeof(m_URLPreSuffix)) {
            	return false;
            }
            n = 0; k2 = i + 1;
            while (k2 <= k1 - 1) m_URLPreSuffix[n++] = CurRequest[k2++];
            m_URLPreSuffix[n] = '\0';
            i = k + 7; 
            EXCLOG(LOG_INFO, "m_URLPreSuffix=%s", m_URLPreSuffix);
            parseSucceeded = true;
            break;
        }
    }
    if (!parseSucceeded) {
        printf("[%s]%d parse failed!\n", __FUNCTION__,__LINE__);
        return false;
    }

    // Look for "CSeq:", skip whitespace, then read everything up to the next \r or \n as 'CSeq':
    parseSucceeded = false;
    for (j = i; (int)j < (int)(CurRequestSize-5); ++j) 
    {
        if (CurRequest[j]   == 'C' && CurRequest[j+1] == 'S' && 
            CurRequest[j+2] == 'e' && CurRequest[j+3] == 'q' && 
            CurRequest[j+4] == ':') 
        {
            j += 5;
            while (j < CurRequestSize && (CurRequest[j] ==  ' ' || CurRequest[j] == '\t')) ++j;
            unsigned n;
            for (n = 0; n < sizeof(m_CSeq)-1 && j < CurRequestSize; ++n,++j) 
            {
                char c = CurRequest[j];
                if (c == '\r' || c == '\n') 
                {
                    parseSucceeded = true;
                    break;
                }
                m_CSeq[n] = c;
            }
            m_CSeq[n] = '\0';
            break;
        }
    }
    if (!parseSucceeded) {
        printf("[%s]%d parse failed!\n", __FUNCTION__,__LINE__);
        return false;
    }

    // Also: Look for "Content-Length:" (optional)
    for (j = i; (int)j < (int)(CurRequestSize-15); ++j) 
    {
        if (CurRequest[j]    == 'C'  && CurRequest[j+1]  == 'o'  && 
            CurRequest[j+2]  == 'n'  && CurRequest[j+3]  == 't'  && 
            CurRequest[j+4]  == 'e'  && CurRequest[j+5]  == 'n'  && 
            CurRequest[j+6]  == 't'  && CurRequest[j+7]  == '-'  && 
            (CurRequest[j+8] == 'L' || CurRequest[j+8]   == 'l') &&
            CurRequest[j+9]  == 'e'  && CurRequest[j+10] == 'n' && 
            CurRequest[j+11] == 'g' && CurRequest[j+12]  == 't' && 
            CurRequest[j+13] == 'h' && CurRequest[j+14] == ':') 
        {
            j += 15;
            while (j < CurRequestSize && (CurRequest[j] ==  ' ' || CurRequest[j] == '\t')) ++j;
            unsigned num;
            if (sscanf(&CurRequest[j], "%u", &num) == 1) m_ContentLength = num;
        }
    }
    return true;
#endif

};

RTSP_CMD_TYPES CRtspSession::Handle_RtspRequest(char const * aRequest, unsigned aRequestSize)
{
    if (ParseRtspRequest(aRequest,aRequestSize)) {
        EXCLOG(LOG_INFO, "m_RtspCmdType=%d", m_RtspCmdType);
        switch (m_RtspCmdType) {
            case RTSP_OPTIONS:  { Handle_RtspOPTION();   break; };
            case RTSP_DESCRIBE: { Handle_RtspDESCRIBE(); break; };
            case RTSP_SETUP:    { Handle_RtspSETUP();    break; };
            case RTSP_PLAY:     { Handle_RtspPLAY();     break; };
            default: {};
        };
    }
    else {
        EXCLOG(LOG_ERROR, "ParseRtspRequest() failed! : %s", aRequest);
    }
    return m_RtspCmdType;
};

void CRtspSession::Handle_RtspOPTION()
{
    char   Response[1024];

    snprintf(Response,sizeof(Response),
        "RTSP/1.0 200 OK\r\nCSeq: %zu\r\n"
        "Public: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n",_cseq);

    //send(m_RtspClient, Response,strlen(Response),0);
    sendResponse(Response, strlen(Response));
}

void CRtspSession::Handle_RtspDESCRIBE()
{
    char   Response[1024];
    char   SDPBuf[1024];
    char   URLBuf[1024];

    // check whether we know a stream with the URL which is requested
    m_StreamID = -1;        // invalid URL
//    if ((strcmp(m_URLPreSuffix,"mjpeg") == 0) && (strcmp(m_URLSuffix,"1") == 0)) m_StreamID = 0; else
//    if ((strcmp(m_URLPreSuffix,"mjpeg") == 0) && (strcmp(m_URLSuffix,"2") == 0)) m_StreamID = 1;
    m_StreamID = 1;
    if (m_StreamID == -1)
    {   // Stream not available
        snprintf(Response,sizeof(Response),
            "RTSP/1.0 404 Stream Not Found\r\nCSeq: %zu\r\n%s\r\n",
            _cseq,
            DateHeader());

        //send(m_RtspClient,Response,strlen(Response),0);
        sendResponse(Response, strlen(Response));
        return;
    };

    // simulate DESCRIBE server response
    char OBuf[256];
    char * ColonPtr;
    //strcpy(OBuf,m_URLHostPort);
    sprintf(OBuf, "%d", _hostPort);
    ColonPtr = strstr(OBuf,":");
    if (ColonPtr != nullptr) ColonPtr[0] = 0x00;

//    snprintf(SDPBuf,sizeof(SDPBuf),
//        "v=0\r\n"
//        "o=- %u 1 IN IP4 %s\r\n"
//        "s=\r\n"
//        "t=0 0\r\n"                                            // start / stop - 0 -> unbounded and permanent session
//        "m=video 0 RTP/AVP 26\r\n"                             // currently we just handle UDP sessions
//        "c=IN IP4 127.0.0.1\r\n",
//        rand(),
//        OBuf);

    snprintf(SDPBuf,sizeof(SDPBuf),
            "v=0\r\n"
            "o=- %u 1 IN IP4 %s\r\n"
            "s=\r\n"
            "t=0 0\r\n"                                            // start / stop - 0 -> unbounded and permanent session
            "m=audio 0 RTP/AVP 0\r\n"                             // currently we just handle UDP sessions
            "c=IN IP4 0.0.0.0\r\n"
            "a=rtpmap:0 PCMA/16000/2",
            rand(),
            OBuf);
    char StreamName[64];
//    switch (m_StreamID)
//    {
//        case 0: strcpy(StreamName,"mjpeg/1"); break;
//        case 1: strcpy(StreamName,"mjpeg/2"); break;
//    };
    memset(StreamName, 0x0, sizeof(StreamName));
    sprintf(StreamName, "%s", _path.to_string().c_str());

    snprintf(URLBuf,sizeof(URLBuf),
        "rtsp://%d/%s",
        _hostPort,
        StreamName);
    snprintf(Response,sizeof(Response),
        "RTSP/1.0 200 OK\r\nCSeq: %zu\r\n"
        "%s\r\n"
        "Content-Base: %s/\r\n"
        "Content-Type: application/sdp\r\n"
        "Content-Length: %zu\r\n\r\n"
        "%s",
        _cseq,
        DateHeader(),
        URLBuf,
        strlen(SDPBuf),
        SDPBuf);

    //send(m_RtspClient,Response,strlen(Response),0);
    sendResponse(Response, strlen(Response));
}

void CRtspSession::Handle_RtspSETUP()
{
    char Response[1024];
    char Transport[255];

    memset(Response, 0x0, sizeof(Response));
    memset(Transport, 0x0, sizeof(Transport));

    // init RTP streamer transport type (UDP or TCP) and ports for UDP transport
    m_Streamer->InitTransport(m_ClientRTPPort,m_ClientRTCPPort,m_TcpTransport);

    // simulate SETUP server response
    if (m_TcpTransport) {
        snprintf(Transport,sizeof(Transport),"RTP/AVP/TCP;unicast;interleaved=0-1");
    }
    else {
        snprintf(Transport,sizeof(Transport),
                "RTP/AVP;unicast;destination=127.0.0.1;source=127.0.0.1;client_port=%i-%i;server_port=%i-%i",
                m_ClientRTPPort,
                m_ClientRTCPPort,
                m_Streamer->GetRtpServerPort(),
                m_Streamer->GetRtcpServerPort());
    }

    snprintf(Response,sizeof(Response),
        "RTSP/1.0 200 OK\r\nCSeq: %zu\r\n"
        "%s\r\n"
        "Transport: %s\r\n"
        "Session: %u\r\n\r\n",
        _cseq,
        DateHeader(),
        Transport,
        m_RtspSessionID);

    //send(m_RtspClient,Response,strlen(Response),0);
    sendResponse(Response, strlen(Response));
}

void CRtspSession::Handle_RtspPLAY()
{
    char   Response[1024];

    // simulate SETUP server response
    snprintf(Response,sizeof(Response),
        "RTSP/1.0 200 OK\r\nCSeq: %zu\r\n"
        "%s\r\n"
        "Range: npt=0.000-\r\n"
        "Session: %u\r\n"
        "RTP-Info: url=rtsp://127.0.0.1:8554/mjpeg/1/\r\n\r\n",
        _cseq,
        DateHeader(),
        m_RtspSessionID);

    //send(m_RtspClient,Response,strlen(Response),0);
    sendResponse(Response, strlen(Response));
}

void CRtspSession::sendResponse(char* data, size_t len)
{
    EXCLOG(LOG_INFO, "Response:\n%s", data);
    send(m_RtspClient,data, len, 0);
}

char const * CRtspSession::DateHeader() 
{
    memset(buf, 0x0, sizeof(buf));
    time_t tt = time(NULL);
    strftime(buf, sizeof buf, "Date: %a, %b %d %Y %H:%M:%S GMT", gmtime(&tt));
    return buf;
}

exstring CRtspSession::directiveString(RTSP_CMD_TYPES type)
{
    switch(type) {
        case RTSP_OPTIONS:      return exstring("OPTIONS");
        case RTSP_DESCRIBE:     return exstring("DESCRIBE");
        case RTSP_SETUP:        return exstring("SETUP");
        case RTSP_PLAY:         return exstring("PLAY");
        case RTSP_TEARDOWN:     return exstring("TEARDOWN");
        default : return exstring("UNKNOWN");
    }
}

int CRtspSession::GetStreamID()
{
    return m_StreamID;
};
