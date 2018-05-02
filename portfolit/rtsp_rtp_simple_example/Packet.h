#ifndef _CLASS_TEMPLATE_
#define _CLASS_TEMPLATE_

#include <vector>
#include <memory>

namespace vh1981lib {

    /**
     @class Packet
     @brief 소켓 통신에서 데이터 저장하는 코드
     */

    class Packet {
    /**
     @name 생성자 / 소멸자
     */
    //@{
    public:
        Packet();
        Packet(size_t bufSize);
        virtual ~Packet();
    //@}

    /**
     @name 속성
     */
    //@{
    private:
        // member variables
        unsigned char* _buffer;
        size_t _bufSize;
        size_t _dataLen;

    public:
        unsigned char* buffer() { return _buffer; }
        size_t dataLen() { return _dataLen; }
        void setDataLen(size_t len) { _dataLen = len; }
        void clear();
    //@}

    /**
     @name 내부 함수
     */
    //@{
    private:
        void init();
    //@}

    };

} // namespace vh1981lib


#endif
