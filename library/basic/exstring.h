#ifndef _VH1981_EXSTRING_
#define _VH1981_EXSTRING_

#include <string>
#include <cstdarg>
#include <functional>

namespace vh1981lib {

    /**
     @class exstring
     @brief 문자열을 쉽게 다루기 위한 클래스.
     */

    class exstring {
    /**
     @name 생성자 / 소멸자
     */
    //@{
    public:
        exstring() {}
        exstring(const exstring& s) : _string(s._string) {}
        exstring(const wchar_t* wcs);
        exstring(const wchar_t* wcs, size_t len);
        exstring(const std::string& cs);
        exstring(const char* cs);
        exstring(const char* cs, size_t len);

        virtual ~exstring() {}
    //@}

    /**
     @name 속성
     */
    //@{
    private:
        std::string _string;
        
    public:
        bool empty() const { return _string.empty(); }
        size_t length() const { return _string.length(); }
        size_t capacity() const { return _string.capacity(); }
        void reserve(uint32_t capacity);
    //@}

    public:
        void clear() { _string.clear(); }
        void resize(size_t size);
        void assign(const exstring& s) { _string.assign(s._string); }
        void append(const exstring& s) { _string.append(s._string); }

        exstring& operator=(const exstring& s) { _string.assign(s._string); return *this; }
        exstring& operator+=(const exstring& s) { _string.append(s._string); return *this; }
        exstring& operator=(const char* s) {
            _string = s; return *this;
        }
        void swap(exstring& s) { _string.swap(s._string); }

    /**
     @name formatting:
     */
    //@{
    public:
        void format(const exstring& format, ...);
        void format(const char* format, ...);
    //@}
        void appendf(const exstring& format, ...);
        void appendf(const char* format, ...);

    /**
     @name indexing:
     */
    //@{
    public:
        const char operator[](size_t index) const { return _string[index]; }
        char operator[](size_t index)       { return _string[index]; }
    //@}

    /**
     @name compare operator overloading
     */
    //@{
    public:
        bool operator==(const exstring& s)  const { return _string == s._string; }
        bool operator!=(const exstring& s)  const { return _string != s._string; }
        bool operator<=(const exstring& s)  const { return _string <= s._string; }
        bool operator< (const exstring& s)  const { return _string <  s._string; }
        bool operator>=(const exstring& s)  const { return _string >= s._string; }
        bool operator> (const exstring& s)  const { return _string >  s._string; }
        bool operator==(const char* str) const { return _string == str; }
        bool operator!=(const char* str) const { return _string != str; }
        bool operator<=(const char* str) const { return _string <= str; }
        bool operator< (const char* str) const { return _string <  str; }
        bool operator>=(const char* str) const { return _string >= str; }
        bool operator> (const char* str) const { return _string >  str; }
    //@}

    /**
     @name stream operator overloading

     @brief ostream를 상속받은 cout등의 객체에도 shift 연산에 대한 오버로딩이 없으므로
     단순한 멤버 함수 오버로딩으론 안되고 전역함수 오버로딩으로 처리해야 함.


     */
    //@{
    public:
        friend std::ostream& operator<<(std::ostream& os, const exstring& str);
    //@}

    public:
        const std::string& to_string() const { return _string; }
        void from_string(const std::string& s) { _string = s; }

    public:
        virtual size_t hash() const { return std::hash<std::string>()(_string); }
    };

} // namespace vh1981lib

namespace std
{
    template <>
    struct hash<vh1981lib::exstring> {
        size_t operator()(const vh1981lib::exstring& s) const { return s.hash(); }
    };

}   // namespace std

#endif
