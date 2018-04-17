#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "exstring_test"
#endif

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>



#include "library/basic/exstring.h"
#include "library/basic/exlog.h"
#include "CRtspSession.h"
#include "RtspParser.h"

using namespace std;
using namespace vh1981lib;

int gexstringtest = 0;

const char* sample_text_OPTIONS = "OPTIONS rtsp://127.0.0.1:8554/mjpeg/1 RTSP/1.0\n"\
        "CSeq: 2\n"\
        "User-Agent: LibVLC/2.1.6 (LIVE555 Streaming Media v2014.01.13)";
const char* sample_text_DESCRIBE = "DESCRIBE rtsp://127.0.0.1:8554/mjpeg/1 RTSP/1.0\n"\
        "CSeq: 3\n"\
        "User-Agent: LibVLC/2.1.6 (LIVE555 Streaming Media v2014.01.13)\n"\
        "Accept: application/sdp";
const char* sample_text_SETUP = "SETUP rtsp://127.0.0.1:8554/mjpeg/1/ RTSP/1.0\n"\
        "CSeq: 4\n"\
        "User-Agent: LibVLC/2.1.6 (LIVE555 Streaming Media v2014.01.13)\n"\
        "Transport: RTP/AVP;unicast;client_port=60842-60843";
const char* sample_text_PLAY = "PLAY rtsp://127.0.0.1:8554/mjpeg/1/ RTSP/1.0\n"\
        "CSeq: 5\n"\
        "User-Agent: LibVLC/2.1.6 (LIVE555 Streaming Media v2014.01.13)\n"\
        "Session: 2004820934\n"\
        "Range: npt=0.000-";

TEST(RtspParser_test, ctor)
{
//    CRtspSession session(0, NULL);
//    session.ParseRtspRequest(sample_text_OPTIONS, strlen(sample_text_OPTIONS));

#if 0
    std::istringstream f(sample_text_OPTIONS);
    string line;
    int lineNum = 0;

    getline(f, line);
    RtspParser parser;
    auto directive = parser.getDirective(exstring(line));
    //EXCLOG(LOG_INFO, "%d %d", directive, RTSP_CMD_TYPES::RTSP_OPTIONS);
    EXPECT_TRUE(directive == RTSP_CMD_TYPES::RTSP_OPTIONS);

    stringstream stream(line);
    string token;
//    token << stream; // skip directive
//    token << stream;
    stream >> token; // skip directive
    stream >> token;
    EXCLOG(LOG_INFO, "url=%s", token.c_str());
    string url = token;

    boost::regex regexPort(":([0-9]+)");
    boost::smatch what;
    string port;
    if (boost::regex_search(url, what, regexPort)) {
        string match(what[1].first, what[1].second);
        port = match;
        EXCLOG(LOG_INFO, "port=%s", port.c_str());
    }
    else {
        EXCLOG(LOG_ERROR, "can't get port number!!!");
    }

    size_t startpos = url.find_first_of('/', 8);
    string path;
    if (startpos != string::npos) {
        startpos += 1;
        string match(url.begin() + startpos, url.end());
        EXCLOG(LOG_INFO, "path=%s", match.c_str());
        path = match;
    }
    else {
        EXCLOG(LOG_ERROR, "can't get port path!!!");
    }

//    for (int i = 1; i < what.size(); i++) {
//        string match(what[i].first, what[i].second);
//        cout << "port :" << match << endl;
//    }
#endif

    RtspParser parser;
    parser.parse(exstring(sample_text_OPTIONS));

    parser.parse(exstring(sample_text_SETUP));

#if 0
    {
        //boost::regex reg1("([A-Za-z]+)");
        boost::regex reg1("([0-9]+)");

        string target = "123123 21312312 323123123";
        //        boost::match_results<std::string::const_iterator> what;
        boost::smatch what;
        boost::regex_search(target, what, reg1);

        for (int i = 1; i < what.size(); i++) {
            string match(what[i].first, what[i].second);
            cout << "matches :" << match << endl;
        }

        {
            boost::sregex_token_iterator iter(target.begin(), target.end(), reg1, 0);
            boost::sregex_token_iterator end;

            for( ; iter != end; ++iter ) {
                std::cout<<*iter<<'\n';
            }
        }

        {
            boost::sregex_iterator iter(target.begin(), target.end(), reg1);
            boost::sregex_iterator end;
            for( ; iter != end; ++iter ) {
                cout << iter->str() << endl;
            }

        }

        EXCLOG(LOG_INFO, "");
    }
#endif

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
