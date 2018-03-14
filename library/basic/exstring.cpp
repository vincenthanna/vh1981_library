#include <string>
#include <algorithm>
#include <locale>
#include <vector>
#include <stdexcept>    // runtime_error
#include <iostream>     // wcerr
#include <cstring>
#include <stdarg.h>
#include <stdio.h>

#include "exstring.h"

using namespace std;

namespace vh1981lib {
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
        printf("s=%s\n", buffer);
        return s;
    }

} //end of vh1981lib

using namespace vh1981lib;

void exstring::format(const exstring &format, ...)
{
    va_list args;
    va_start(args, reinterpret_cast<const exstring*>(&format));
    //_g2_wstring_vformat(&_string, format._string, args);
    _string = format_arg_list(format.to_string().c_str(), args);
    va_end(args);
}

void exstring::vformat(const exstring &format, va_list args) {

}

void exstring::appendf(const exstring &format, ...) {

}

void exstring::appendvf(const exstring &format, va_list args) {

}

void exstring::reserve(uint32_t capacity) { 

}

exstring::exstring(const wchar_t *wcs) { 

}

exstring::exstring(const wchar_t *wcs, size_t len) { 

}

exstring::exstring(const std::string &cs) { 

}

exstring::exstring(const char *cs) { 

}

exstring::exstring(const char *cs, size_t len) { 

}


