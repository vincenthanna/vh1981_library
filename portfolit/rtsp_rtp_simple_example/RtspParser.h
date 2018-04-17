#ifndef _RTSP_PARSER_H_
#define _RTSP_PARSER_H_

#include "library/basic/exstring.h"
#include "library/basic/exlog.h"

#include "RtspTypes.h"
#include "JPEGSamples.h"

using namespace std;
using namespace vh1981lib;

class RtspParser {
public:
    RtspParser();
    virtual ~RtspParser() {}

public:
    RTSP_CMD_TYPES getDirective(const exstring& line);

public:
    bool parse(const exstring& input);

private:
    bool parse_OPTIONS(const exstring& input);
    bool parse_DESCRIBE(const exstring& input);
    bool parse_SETUP(const exstring& input);
    bool parse_PLAY(const exstring& input);
    bool parse_TEARDOWN(const exstring& input);

public:
    exstring _source;
    exstring _path;
    RTSP_CMD_TYPES _type;
    ushort _hostPort;
    ushort _clientPort;
    exstring _cseq;

};

#endif
