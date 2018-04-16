#ifndef _VH1981_EXLOG_
#define _VH1981_EXLOG_

#include <string>
#include <cstdarg>
#include <unordered_set>
#include "exstring.h"

using namespace std;

namespace vh1981lib {

    class exlogops {
    public:
        enum Color {
            ansi_default = 0,
            red_black,
            grey_black,
            green_yellow,
            red_gray
        };

    public:
        exlogops() :_name(""), _funcName(""), _line(0), _color(ansi_default) {}

        exlogops(const exstring& name, const exstring& funcName = "", \
                 size_t line = 0, Color color = ansi_default)
        {
            _name = name;
            _funcName = funcName;
            _line = line;
            _color = color;
        }

    protected:
        exstring _name;
        exstring _funcName;
        size_t _line;
        Color _color;

    public:
        const exstring& name() const { return _name; }
        const exstring& funcName() const { return _funcName; }
        size_t line() const { return _line; }
        Color color() const { return _color; }
    };

    void exvlog(const exlogops& ops, const exstring& fmt, va_list args);
    void exvlog(const exstring& name, const exstring& fmt, va_list args);

    namespace {

        class extraceNames {
        private:
            unordered_set<exstring> _names;

        public:
            extraceNames() {
#if defined(G2TRACE_NAMES)
                exstring name(G2TRACE_NAMES);
                _names.insert(name);
#endif
            }

            bool contains(const exstring& name) const
            {
                return _names.empty() || _names.end() != _names.find(name);
            }
        };

        extraceNames s_lognames;

        void exlog(const exstring& name, const exstring* format, ...)
        {
            if (!name.empty() && s_lognames.contains(name)) {
                va_list args;
                va_start(args, format);
                exvlog(name, *format, args);
                va_end(args);
            }
        }

        void exlog(const exlogops& option, const exstring* format, ...)
        {
            if (!option.name().empty() && s_lognames.contains(option.name())) {
                va_list args;
                va_start(args, format);
                exvlog(option, *format, args);
                va_end(args);
            }
        }

        #ifndef EXLOG_NAME
        #define EXLOG_NAME ""
        #endif

        #define _PARMS_   EXLOG_NAME,exstring(__FUNCTION__),__LINE__
        #define LOG_DEBUG exlogops(_PARMS_, exlogops::ansi_default)
        #define LOG_INFO exlogops(_PARMS_, exlogops::red_black)
        #define LOG_WARN exlogops(_PARMS_, exlogops::grey_black)
        #define LOG_ERROR exlogops(_PARMS_, exlogops::green_yellow)
        #define LOG_FATAL exlogops(_PARMS_, exlogops::red_gray)

        #define EXCLOG(option, format, ...)\
            do { \
                if (EXLOG_NAME[0] != L'\0') { \
                    exstring fmt(format); \
                    exlog(option, &fmt, ## __VA_ARGS__); \
                } \
            } while(0)
        #define EXLOG(format, ...) EXCLOG(LOG_DEBUG, format, ## __VA_ARGS__)
    }

} // namespace vh1981lib

#endif
