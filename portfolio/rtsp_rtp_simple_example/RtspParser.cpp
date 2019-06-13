#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "RtspParser"
#endif
#include "library/basic/exstring.h"
#include "library/basic/exlog.h"

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>


#include "RtspParser.h"


RtspParser::RtspParser()
{

}

RTSP_CMD_TYPES RtspParser::getDirective(const exstring& line)
{
    EXCLOG(LOG_INFO, "%s", line.to_string().c_str());
    for (auto directive : rtspDirectives) {
        //EXCLOG(LOG_INFO, "%s %d", directive.name.c_str(), directive.name.length());
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

    EXCLOG(LOG_INFO, "RTSP_CMD_TYPE : %d", type);

    _type = type;
    _source = input;

    parseCommonValues(input);

    return true;
#if 0
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
#endif
}

bool RtspParser::parseCommonValues(const exstring& input)
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
				//EXCLOG(LOG_INFO, "line=%s startpos=%d", line.c_str(), startpos);
				boost::regex regexPort("([0-9]+)");
				boost::smatch what;
				string cseq;
				if (boost::regex_search(line, what, regexPort)) {
					string match(what[1].first, what[1].second);
					_cseq = match;
					EXCLOG(LOG_INFO, "cseq=%s", _cseq.to_string().c_str());
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
						_clientPort = std::stoi(match, nullptr, 10);
						EXCLOG(LOG_INFO, "_clientPort:%d", _clientPort);
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
		}

		linenum++;
	}
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
                EXCLOG(LOG_FATAL, "port=%s", port.c_str());
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
                    EXCLOG(LOG_INFO, "cseq=%s", _cseq.to_string().c_str());
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
        	// 할것이 없음.
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
        			EXCLOG(LOG_INFO, "cseq=%s", _cseq.to_string().c_str());
        		}
        		else {
        			EXCLOG(LOG_ERROR, "can't get cseq!!!");
        		}
        	}
        }
        linenum++;
    }
}

bool RtspParser::parse_SETUP(const exstring& input) {
}

bool RtspParser::parse_PLAY(const exstring& input) {
}

bool RtspParser::parse_TEARDOWN(const exstring& input) {
}
