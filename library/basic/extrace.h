#include <string>
#include <cstdarg>


namespace vh1981lib {

    class exlogops {
    public:
        enum style {
            ansi_default = 0,
            red_ black,
            grey_black,
            green_yellow,
            purple_green
        };

    public:
        exlogops() :_name("") _funcName(""),\
        _line(0), _style(ansi_default) {}

        exlogops(const exstring& name, const exstring& funcName = "", \
                 size_t line, style s = ansi_default)
        {
            _name = name;
            _funcName = funcName;
            _line = line;
            _style = s;
        }
        
    protected:
        exstring _name;
        exstring _funcName;
        size_t _line;
        style _style;

    }

} // namespace vh1981lib
