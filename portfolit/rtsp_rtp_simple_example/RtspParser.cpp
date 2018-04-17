#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "RtspParser"
#endif
#include "library/basic/exlog.h"

#include <iostream>
#include <sstream>
#include <string>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>

#include "RtspParser.h"


RtspParser::RtspParser()
{

}

RTSP_CMD_TYPES RtspParser::getDirective(const exstring& line)
{
    EXCLOG(LOG_INFO, "%s", line.to_string().c_str());
    for (auto directive : rtspDirectives) {
        EXCLOG(LOG_INFO, "%s %d", directive.name.c_str(), directive.name.length());
        if (directive.name.compare(0, directive.name.length(), \
            line.to_string(), 0, directive.name.length()) == 0) {
            return (RTSP_CMD_TYPES)directive.cmdType;
        }
    }
    return RTSP_UNKNOWN;
}

bool RtspParser::parse(const exstring& input)
{
    RTSP_CMD_TYPES type = getDirective(input);
    if (type == RTSP_UNKNOWN) {
        EXCLOG(LOG_ERROR, "can't get directive!");
        return false;
    }

    _source = input;

    bool result = false;
    switch(type) {
    case RTSP_OPTIONS:
        result = parse_OPTIONS(input);
        break;
    case RTSP_DESCRIBE:
        result = parse_DESCRIBE(input);
        break;
    case RTSP_SETUP:
        result = parse_SETUP(input);
        break;
    case RTSP_PLAY:
        result = parse_PLAY(input);
        break;
    case RTSP_TEARDOWN:
        result = parse_TEARDOWN(input);
        break;
    default:
        break;
    }

    return result;
}

bool RtspParser::parse_OPTIONS(const exstring& input)
{
    std::istringstream f(input.to_string().c_str());
    string line;
    int linenum = 0;

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

                EXCLOG(LOG_INFO, "line=%s startpos=%d", line.c_str(), startpos);
                boost::regex regexPort("([0-9]+)");
                boost::smatch what;
                string cseq;
                if (boost::regex_search(line, what, regexPort)) {
                    string match(what[1].first, what[1].second);
                    _cseq = match;
                    EXCLOG(LOG_INFO, "cseq=%s", cseq.c_str());
                }
                else {
                    EXCLOG(LOG_ERROR, "can't get cseq!!!");
                }
            }
        }

        linenum++;
    }
}

bool RtspParser::parse_DESCRIBE(const exstring& input) {
    std::istringstream f(input.to_string().c_str());
    string line;
    int linenum = 0;

    while(getline(f, line)) {
        string url;

        if (linenum == 0) {

        }
    }
}

bool RtspParser::parse_SETUP(const exstring& input) {
}

bool RtspParser::parse_PLAY(const exstring& input) {
}

bool RtspParser::parse_TEARDOWN(const exstring& input) {
}
