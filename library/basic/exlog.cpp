#include <iostream>
#include "exlog.h"

#include <stdio.h>
#include <string.h>
#include <iomanip>

using namespace std;

namespace vh1981lib {

    std::string format_arg_list(const char *fmt, va_list args);

    void exvlog(const exlogops& ops, const exstring& format, va_list args)
    {
        string s = format_arg_list(format.to_string().c_str(), args);
        cout.imbue(std::locale(""));

        time_t t = time(0);   // get time now
        tm* now = localtime(&t);
        exstring tmp;
        tmp.format("%02d:%02d:%02d : ", now->tm_hour, now->tm_min, now->tm_sec);
        cout <<std::setw(18) << tmp;

        bool clearStyle = exlogops::ansi_default == ops.color() ? false : true;

        exstring str;
        if (exlogops::red_black == ops.color()) {
            str += "\x1B[47m\x1B[1;30m";
        }
        else if (exlogops::red_black == ops.color()) {
            str += "\x1B[47m\x1B[1;30m";
        }
        else if (exlogops::grey_black == ops.color()) {
            str += "\x1B[42m\x1B[1;33m";
        }
        else if (exlogops::green_yellow == ops.color()) {
            str += "\x1B[45m\x1B[1;37m";
        }
        else if (exlogops::red_gray == ops.color()) {
            str += "\x1B[41m\x1B[1;37m";
        }

        str += "<";
        exstring infostr;
        infostr.format("%s:%s:%d",\
            ops.name().to_string().c_str(), ops.funcName().to_string().c_str(), ops.line());
        str += infostr;
        str += ">";

        if (clearStyle) { // clear ANSI Colors
            str += "\x1B[0m";
        }

        //cout << " " << s << std::endl;
        cout << str;
        cout << " " << s << endl;
    }

    void exvlog(const exstring& name, const exstring& format, va_list args)
    {
        exlogops ops(name, "???");
        exlog(ops, &format, args);
    }

} // namespace vh1981lib
