#ifndef _MOTION_SEARCH_H
#define _MOTION_SEARCH_H
////////////////////////////////////////////////////////////////

/// MS compatible compilers support #pragma once.
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//#include <G2Foundation/G2Type.h>
//#include <HAL/McpStruct.h>
#include <stdint.h>

namespace VideoAnalytics {

    #define MOTION_ZONE_ROW_MAX          28
    #define MOTION_ZONE_COL_MAX          36

    #define MOTION_SEARCH_ROW_COUNT 24
    #define MOTION_SEARCH_COLUMN_COUNT 32    

    //enum { MBO_DETAIL_LEVEL = 4 };
    //enum { MBO_PIXEL_COUNT = (MBO_DETAIL_LEVEL * MBO_DETAIL_LEVEL) };

    struct MotionBlockObject {
        uint8_t pixelData;
    };

    struct MotionResult {
        bool detected;
        uint32_t time;
    };

    /**
     @struct MotionOptions
     @brief motion search에 사용될 option 정보 구조체
     */
    typedef struct {
        uint32_t motionMode;
        uint32_t motionSensitivity;
        uint32_t minBlocks;
        uint8_t motionMask[84];
    } MotionOptions;

    /**
     @class MotionBlockAvgDelegate
     @brief MotionSearch에서 사용할 pixel을 뽑아서 넘겨주는 함수 인터페이스
     */

    class MotionBlockAvgDelegate {
    /**
     @name 생성 및 소멸
    */
    //@{
    public:
        MotionBlockAvgDelegate() {}
        virtual ~MotionBlockAvgDelegate() {}
    //@}

    /**
     @name MotionSearch 인스턴스로 움직임 검색을 사용할 경우 아래 함수 구현을 제공해야 한다.
           사용되는 프레임의 이미지 포맷이 YCbCr 4:2:2인지, YCbCr 4:2:0인지, RGB인지에 따라
           픽셀을 뽑아내는 방법이 달라져야 함.

     @param image           프레임 이미지
     @param motionAvgData   averaged block value(Y).
     @param width           프레임 width
     @param height          프레임 height
     @param stride          number of bytes from one row of pixels.
                            in memory to the next row of pixels in memory.
     @param motionMask      marked area to be searched.

     @return void.
     */
    //@{
    public:
        virtual void getBlockAvg(
                const unsigned char *image,
                MotionBlockObject *motionAvgData,
                unsigned int width,
                unsigned int height,
                unsigned int stride) = 0;
    //@}
    };

    class MotionSearch {
    private:
        uint32_t _width;
        uint32_t _height;

        //stacks for detect connected components.
        uint32_t stackTemporary[MOTION_ZONE_ROW_MAX * MOTION_ZONE_COL_MAX];
        uint32_t stackTemporaryTop;

        uint32_t stackFinal[MOTION_ZONE_ROW_MAX * MOTION_ZONE_COL_MAX];
        uint32_t stackFinalTop;

        uint32_t stackGroup[MOTION_ZONE_ROW_MAX * MOTION_ZONE_COL_MAX];
        uint32_t stackGroupTop;

        //block map used for detecting connected components.
        uint8_t motionMapGroup[MOTION_ZONE_ROW_MAX*MOTION_ZONE_COL_MAX];

        //masked block count refreshes each new motion search. needed for museum search.
        uint32_t maskedBlockCount;

        //bit calculation may be rather slower than using whole byte...
        uint8_t inputMotionMask[MOTION_ZONE_ROW_MAX*MOTION_ZONE_COL_MAX];
        MotionBlockObject refMotionAvgValue[MOTION_ZONE_ROW_MAX*MOTION_ZONE_COL_MAX];
        MotionBlockObject curMotionAvgValue[MOTION_ZONE_ROW_MAX*MOTION_ZONE_COL_MAX];

        //last detected time(sec)
        uint32_t lastDetectedFrameTime;

        MotionBlockAvgDelegate* _delegate;

    public:
        MotionSearch();
        virtual ~MotionSearch() {}

    private:
        inline int32_t pixToBlk(uint32_t x, uint32_t y, uint32_t* pbx, uint32_t* pby);
        void dumpMotionMap(MotionBlockObject* calculatedMotionValue, const char* title);

        int32_t compareMotion(
            MotionBlockObject* motionData1,
            const MotionBlockObject* motionData2,
            uint8_t* motionMask,
            uint32_t threshold);

        int32_t checkConnectedComponent(MotionBlockObject* motionDataDifference, uint32_t motionSearchMinBlock);

    public:
        void setDelegate(MotionBlockAvgDelegate* delegate) { _delegate = delegate; }
        void doMotionSearch(
            uint8_t* vo_buffer,           //_video_out_buffer
            uint32_t width,
            uint32_t height,
            uint32_t stride,
            uint32_t motionZoneRowCount,
            uint32_t motionZoneColCount,
            MotionOptions* motionOptions,  // it has motionMode, motionSensitivity, minBlocks, motionMask.
            bool motionNew,                     // motion search start or ongoing
            uint32_t frameTime,
            //uint32_t *detected,             // if detected, set to 1 inside.
            MotionResult *result);            // if detected, it's used by caller.
    };

}   // end of VideoAnalytics

////////////////////////////////////////////////////////////////
#endif  // _MOTION_SEARCH_H


// <EOF>


