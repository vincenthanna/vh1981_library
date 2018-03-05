#ifndef _IDIS_STEP_MOTION_ZONE_H
#define _IDIS_STEP_MOTION_ZONE_H
/////////////////////////////////////////////////////////////

#include <Step/Type.h>
#include <Step/Registry.h>

namespace Idis {

template<uint32 ROW, uint32 COLUMN>
class MotionZone {
public:
    MotionZone() { setAll(); }
    MotionZone(const MotionZone<ROW, COLUMN>& motionZone)
    { setZone(motionZone._zone); }
    MotionZone(const uint8* zone) { setZone(zone); }
    ~MotionZone() {}

public:
    enum {
        ZONE_DATA_COUNT = ((ROW * COLUMN) >> 3) +
                          (((ROW * COLUMN) & 0x00000007) ? 1 : 0),
    };
private:
    uint8 _zone[ZONE_DATA_COUNT];
public:
    void setAll();
    void clear();

    const uint8* zone() const { return _zone; }
    uint32 zoneCount() const;
    bool isAvailableZone(uint32 row, uint32 column) const;
    void setZone(uint32 row, uint32 column, bool set);
    void setZone(const uint8* zone);

    MotionZone<ROW, COLUMN>&
        operator=(const MotionZone<ROW, COLUMN>& zone);

public:
    void addTo(Registry* reg, uint16 id) const;
    void loadFrom(Registry* reg, uint16 id);
    void saveTo(Registry* reg, uint16 id) const;
};

template<uint32 ROW, uint32 COLUMN>
void MotionZone<ROW, COLUMN>::setAll()
{
    for (uint32 r = 0; r < ROW; r++) {
        for (uint32 c = 0; c < COLUMN; c++) {
            setZone(r, c, true);
        }
    }
}

template<uint32 ROW, uint32 COLUMN>
void MotionZone<ROW, COLUMN>::clear()
{
    memset(_zone, 0, sizeof(uint8) * ZONE_DATA_COUNT);
}

template<uint32 ROW, uint32 COLUMN>
void MotionZone<ROW, COLUMN>::setZone(const uint8* zone)
{
    memcpy(_zone, zone, sizeof(uint8) * ZONE_DATA_COUNT);
}

template<uint32 ROW, uint32 COLUMN>
bool MotionZone<ROW, COLUMN>::
    isAvailableZone(uint32 row, uint32 column) const
{
    uint32 index = (row + 1) * COLUMN - column - 1;
    uint8 data = _zone[index >> 3];
    return (data & (0x01 << (index & 0x00000007))) ? true : false;
}

template<uint32 ROW, uint32 COLUMN>
void MotionZone<ROW, COLUMN>::setZone(uint32 row, uint32 column, bool set)
{
    uint32 index = (row + 1) * COLUMN - column - 1;
    uint32 dataIndex = index >> 3;
    uint32 bitIndex = index & 0x00000007;
    uint8 mask = 0x01 << bitIndex;
    uint8 data = (set ? mask : 0x00);
    _zone[dataIndex] = ((_zone[dataIndex] & (~mask)) | data);
}

template<uint32 ROW, uint32 COLUMN>
uint32 MotionZone<ROW, COLUMN>::zoneCount() const
{
    uint32 count = 0;
    for (uint32 row = 0; row < ROW; row++) {
        for (uint32 col = 0; col < COLUMN; col++) {
            if (isAvailableZone(row, col)) {
                count++;
            }
        }
    }
    return count;
}

template<uint32 ROW, uint32 COLUMN>
MotionZone<ROW, COLUMN>&
MotionZone<ROW, COLUMN>::operator=(const MotionZone<ROW, COLUMN>& motionZone)
{
    setZone(motionZone._zone);
    return (*this);
}

template<uint32 ROW, uint32 COLUMN>
void MotionZone<ROW, COLUMN>::addTo(Registry* reg, uint16 id) const
{
    reg->addBinary(id, _zone, sizeof(uint8) * ZONE_DATA_COUNT);
}

template<uint32 ROW, uint32 COLUMN>
void MotionZone<ROW, COLUMN>::loadFrom(Registry* reg, uint16 id)
{
    reg->getBinary(id, _zone, sizeof(uint8) * ZONE_DATA_COUNT);
}

template<uint32 ROW, uint32 COLUMN>
void MotionZone<ROW, COLUMN>::saveTo(Registry* reg, uint16 id) const
{
    reg->setBinary(id, _zone, sizeof(uint8) * ZONE_DATA_COUNT);
}

} // end of Idis

///////////////////////////////////////////////////////////////
#endif  // _IDIS_STEP_MOTION_ZONE_H
