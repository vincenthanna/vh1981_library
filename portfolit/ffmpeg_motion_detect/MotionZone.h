#ifndef _IDIS_STEP_MOTION_ZONE_H
#define _IDIS_STEP_MOTION_ZONE_H
/////////////////////////////////////////////////////////////

#include <stdint.h>

namespace VideoAnalytics {

template<uint32_t ROW, uint32_t COLUMN>
class MotionZone {
public:
    MotionZone() { setAll(); }
    MotionZone(const MotionZone<ROW, COLUMN>& motionZone)
    { setZone(motionZone._zone); }
    MotionZone(const uint8_t* zone) { setZone(zone); }
    ~MotionZone() {}

public:
    enum {
        ZONE_DATA_COUNT = ((ROW * COLUMN) >> 3) +
                          (((ROW * COLUMN) & 0x00000007) ? 1 : 0),
    };
private:
    uint8_t _zone[ZONE_DATA_COUNT];
public:
    void setAll();
    void clear();

    const uint8_t* zone() const { return _zone; }
    uint32_t zoneCount() const;
    bool isAvailableZone(uint32_t row, uint32_t column) const;
    void setZone(uint32_t row, uint32_t column, bool set);
    void setZone(const uint8_t* zone);

    MotionZone<ROW, COLUMN>&
        operator=(const MotionZone<ROW, COLUMN>& zone);
};

template<uint32_t ROW, uint32_t COLUMN>
void MotionZone<ROW, COLUMN>::setAll()
{
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COLUMN; c++) {
            setZone(r, c, true);
        }
    }
}

template<uint32_t ROW, uint32_t COLUMN>
void MotionZone<ROW, COLUMN>::clear()
{
    memset(_zone, 0, sizeof(uint8_t) * ZONE_DATA_COUNT);
}

template<uint32_t ROW, uint32_t COLUMN>
void MotionZone<ROW, COLUMN>::setZone(const uint8_t* zone)
{
    memcpy(_zone, zone, sizeof(uint8_t) * ZONE_DATA_COUNT);
}

template<uint32_t ROW, uint32_t COLUMN>
bool MotionZone<ROW, COLUMN>::
    isAvailableZone(uint32_t row, uint32_t column) const
{
    uint32_t index = (row + 1) * COLUMN - column - 1;
    uint8_t data = _zone[index >> 3];
    return (data & (0x01 << (index & 0x00000007))) ? true : false;
}

template<uint32_t ROW, uint32_t COLUMN>
void MotionZone<ROW, COLUMN>::setZone(uint32_t row, uint32_t column, bool set)
{
    uint32_t index = (row + 1) * COLUMN - column - 1;
    uint32_t dataIndex = index >> 3;
    uint32_t bitIndex = index & 0x00000007;
    uint8_t mask = 0x01 << bitIndex;
    uint8_t data = (set ? mask : 0x00);
    _zone[dataIndex] = ((_zone[dataIndex] & (~mask)) | data);
}

template<uint32_t ROW, uint32_t COLUMN>
uint32_t MotionZone<ROW, COLUMN>::zoneCount() const
{
    uint32_t count = 0;
    for (uint32_t row = 0; row < ROW; row++) {
        for (uint32_t col = 0; col < COLUMN; col++) {
            if (isAvailableZone(row, col)) {
                count++;
            }
        }
    }
    return count;
}

template<uint32_t ROW, uint32_t COLUMN>
MotionZone<ROW, COLUMN>&
MotionZone<ROW, COLUMN>::operator=(const MotionZone<ROW, COLUMN>& motionZone)
{
    setZone(motionZone._zone);
    return (*this);
}

} // end of Idis

///////////////////////////////////////////////////////////////
#endif  // _IDIS_STEP_MOTION_ZONE_H
