#include <iostream>
#include "exlog.h"

#include <stdio.h>

using namespace std;

namespace vh1981lib {

    std::string format_arg_list(const char *fmt, va_list args);

    void exvlog(const exlogops& ops, const exstring& format, va_list args)
    {
        string s = format_arg_list(format.to_string().c_str(), args);
        cout.imbue(std::locale(""));

        bool clearStyle = (exlogops::ansi_default != ops.color());

        if (exlogops::ansi_default == ops.color()) {
            cout << "\x1B[41m\x1B[1;34m";
        }
        else if (exlogops::red_black == ops.color()) {
            cout << "\x1B[47m\x1B[1;30m";
        }
        else if (exlogops::grey_black == ops.color()) {
            cout << "\x1B[42m\x1B[1;33m";
        }
        else if (exlogops::green_yellow == ops.color()) {
            cout << "\x1B[45m\x1B[1;35m";
        }

        cout << "<";
        cout << ops.name() << ":" << ops.funcName() << ":" << ops.line();
        cout << ">";

        if (clearStyle) { // clear ANSI Colors
            cout << "\x1B[0m";
        }

        cout << " " << s << std::endl;
    }

    void exvlog(const exstring& name, const exstring& format, va_list args)
    {
        exlogops ops(name, "???");
        exlog(ops, &format, args);
    }

} // namespace vh1981lib
