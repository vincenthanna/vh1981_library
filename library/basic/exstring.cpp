#include <string>
#include <cstring>
#include <stdarg.h>
#include <stdio.h>
#include <iostream>

#include "exstring.h"

using namespace std;

namespace vh1981lib {
    /**
     서식 처리 함수
     @param fmt 서식 문자열
     @param args 서식 가변 인자
     */
    std::string format_arg_list(const char *fmt, va_list args)
    {
        if (!fmt) {
            return nullptr;
        }

        int   result = -1, length = 256;
        char *buffer = 0;
        while (result == -1) {
            if (buffer) delete [] buffer;
            buffer = new char [length + 1];
            memset(buffer, 0, length + 1);
            result = vsnprintf(buffer, length, fmt, args);
            length *= 2;
        }
        std::string s(buffer);
        delete [] buffer;
        return s;
    }

    std::ostream& operator<<(std::ostream& os, const exstring& str)
    {
        os << str._string;
        return os;
    }

} //end of vh1981lib

using namespace vh1981lib;

void exstring::format(const exstring &format, ...)
{
    va_list args;
    const char* fmt = format.to_string().c_str();
    va_start(args, reinterpret_cast<const char*>(&fmt));
    _string = format_arg_list(fmt, args);
    va_end(args);
}

void exstring::format(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    _string = format_arg_list(format, args);
    va_end(args);
}

void exstring::appendf(const exstring &format, ...)
{
    va_list args;
    va_start(args, reinterpret_cast<const exstring*>(&format));
    string s;
    const char* fmt = format.to_string().c_str();
    s = format_arg_list(fmt, args);
    va_end(args);

    _string += s;
}

void exstring::appendf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    string s = format_arg_list(format, args);
    va_end(args);

    _string += s;
}

void exstring::reserve(uint32_t capacity) {
    _string.reserve(capacity);
}

exstring::exstring(const std::string &s) {
    _string = s;
}

exstring::exstring(const char *cs) {
    _string = string(cs);
}
