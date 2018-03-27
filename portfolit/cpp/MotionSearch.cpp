
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h> 

#include <iostream> 

#include "MotionSearch.h"
#include "MotionZone.h"

using namespace std;
using namespace VideoAnalytics;

//if defined below, regard diagonal position as connected.
#define CONNECTED_COMPONENT_WITH_DIAGONAL

// search directions(with/without diagonal directions).
static int blockAround[8][2] = { {-1,-1}, {0,-1}, {1,-1}, {-1,0}, {1,0}, {-1,1}, {0,1}, {1,1} };
static int blockAroundWithoutDiagonal[4][2] = { {-1,0}, {1,0}, {0,-1}, {0,1} };

inline unsigned char getFlag(unsigned char* array, unsigned int index) {
    unsigned int i = index/8;
    unsigned int pos = index%8;
    switch (pos){
        case 7:return (128 & array[i]) >> 7;
        case 6:return (64 & array[i]) >> 6;
        case 5:return (32 & array[i]) >> 5;
        case 4:return (16 & array[i]) >> 4;
        case 3:return (8 & array[i]) >> 3;
        case 2:return (4 & array[i]) >> 2;
        case 1:return (2 & array[i]) >> 1;
        case 0:return (1 & array[i]);
        default:
            return array[i];
    }
}

/**
 * Linearly searches pArray with argument 'value', and return result.
 *
 * @param value         value to search.
 * @param pArray        pointer to array.
 * @param arraySize     szie of array 'pArray'.
 *
 * @return              1 if found, else 0.
 */
inline int findInArrayUINT(unsigned int value, unsigned int *pArray, unsigned int arraySize) {
    int i;
    for (i=arraySize-1; i>=0; i--){
        if (pArray[i] == value)
            return 1;
    }
    return 0;
}

/**
 * Calculate square root of a unsigned integer.
 *
 * @param n     number you want to get square root of it.
 *
 * @return      square root of n.
 */
inline unsigned int _sqrt(unsigned int n){
    unsigned int a;
    for (a=0;n>=(2*a)+1;n-=(2*a++)+1);
    return a;
}

MotionSearch::MotionSearch() : _delegate(nullptr)
{
    _width = 720;
    _height = 480;

    stackTemporaryTop = 0;
    stackFinalTop = 0;
    stackGroupTop = 0;
    maskedBlockCount = 0;

    lastDetectedFrameTime = 0;

    memset(inputMotionMask, 0x0, sizeof(inputMotionMask));
    memset(refMotionAvgValue, 0x0, sizeof(refMotionAvgValue));
    memset(curMotionAvgValue, 0x0, sizeof(curMotionAvgValue));
}

/**
 * for debug : print motion map or motion mask to console.
 * @param calculatedMotionValue         motion map/mask array.
 * @param title                         this string shows up before motion map/mask be printed.
 *
 */
void MotionSearch::dumpMotionMap(MotionBlockObject* calculatedMotionValue, const char* title)
{
    int32_t i, j;
    printf("start\n");

    char linebuf[8192 * 2];
    ::memset(linebuf, 0x0, sizeof(linebuf));

    char buffer[1024];
    memset(buffer,0x0,sizeof(buffer));
    sprintf(buffer, "[%s]\n", title);
    strcat(linebuf, buffer);

    memset(buffer,0x0,sizeof(buffer));
    sprintf(buffer, "   ");
    strcat(linebuf, buffer);
    for (i=0;i<MOTION_SEARCH_COLUMN_COUNT;i++) {
        memset(buffer,0x0,sizeof(buffer));
        sprintf(buffer, "%2d ", i);
        strcat(linebuf, buffer);
    }
    memset(buffer,0x0,sizeof(buffer));
    sprintf(buffer, "\n");
    strcat(linebuf, buffer);

    memset(buffer,0x0,sizeof(buffer));
    sprintf(buffer, "-------------------------------------------------"\
                    "-------------------------------------------------\n");
    strcat(linebuf, buffer);
    for (i=0;i<MOTION_SEARCH_ROW_COUNT;i++) {
        memset(buffer,0x0,sizeof(buffer));
        sprintf(buffer, "%2d|", i);
        strcat(linebuf, buffer);
        for (j=0;j<MOTION_SEARCH_COLUMN_COUNT;j++){
            memset(buffer,0x0,sizeof(buffer));
            uint32_t backColor = ((calculatedMotionValue[ i*MOTION_SEARCH_COLUMN_COUNT + j ].pixelData)/40);

            sprintf(buffer, "\33[%dm%2x \33[0m", 40+backColor, calculatedMotionValue[ i*MOTION_SEARCH_COLUMN_COUNT + j ].pixelData);
            strcat(linebuf, buffer);

        }
        memset(buffer,0x0,sizeof(buffer));
        sprintf(buffer, "\n");
        strcat(linebuf, buffer);
    }
    memset(buffer,0x0,sizeof(buffer));
    sprintf(buffer, "\n");
    strcat(linebuf, buffer);

    fprintf(stderr, "%s", linebuf);
    printf("linebuf len=%d\n", strlen(linebuf));
    printf("end\n");
}


/**
 * get block position from pixel value.
 *
 */
inline int MotionSearch::pixToBlk(unsigned int x, unsigned int y, unsigned int *pbx, unsigned int *pby)
{
    if (x < _width|| y < _height) {
        *pbx = x / MOTION_SEARCH_ROW_COUNT;
        *pby = y / MOTION_SEARCH_COLUMN_COUNT;
        return 1;
    }
    else{
        return 0;
    }
}

/**
 * compare two motion data and store result to motionData1.
 * if motionMask[i] equals zero, motionData1[i] will be zero too.
 *
 * @param motionData1       motion data. result will be stored here.
 * @param motionData2       motion data.
 * @param motionMask        motion search threshold value.
 * @return                  detected block count.
 */
int MotionSearch::compareMotion(
        MotionBlockObject* motionData1,
        const MotionBlockObject* motionData2,
        unsigned char* motionMask,
        unsigned int threshold)
{

    int i,j,pos;
    int detected=0;
    for (i=0;i<MOTION_SEARCH_ROW_COUNT;i++) {
        for (j=0;j<MOTION_SEARCH_COLUMN_COUNT;j++) {
            pos = i * MOTION_SEARCH_COLUMN_COUNT + j;
            //if (motionMask[pos]) {
            if (true) {
                
                // just save differences to motionData1.
                if (motionData1[pos].pixelData > motionData2[pos].pixelData) {
                    motionData1[pos].pixelData = motionData1[pos].pixelData - motionData2[pos].pixelData;
                }
                else {
                    motionData1[pos].pixelData = motionData2[pos].pixelData - motionData1[pos].pixelData;
                }

                if (motionData1[pos].pixelData >= threshold) {
                    //trace("x=%d y=%d index=%d value=%d \n", j, i, pc, motionData1[pos].pixelData[pc]);
                    detected++;
                    motionData1[pos].pixelData = 0xFF;
                }
                else {
                    motionData1[pos].pixelData = 0x00;
                }
            }
            else { //unselected motion area.
                motionData1[pos].pixelData = 0x00;
            }
        }
    }
    return detected;
}


/**
 * Check connected components.
 *
 * @param motionDataDifference  motion data difference values.
 *                              use 'motionData1' after compareMotion() finished.
 * @param motionSearchMinBlock  motion search detection threshold.
 * @return                      connected component value.
 */
int MotionSearch::checkConnectedComponent(MotionBlockObject* motionDataDifference, unsigned int motionSearchMinBlock){

    unsigned int i,j;
    unsigned int blkRow, blkCol, blkPos;
    unsigned int tmpBlkRow, tmpBlkCol;
    unsigned int curBlkRow, curBlkCol, curBlkPos;

    unsigned int value;
    unsigned int tmpValue;

    // initialize stack.
    stackTemporaryTop = 0;
    stackFinalTop = 0;
    stackGroupTop = 0;

    // initialize motionMapGroup.
    memset(motionMapGroup, 0x0, sizeof(motionMapGroup));

    for (blkRow=0 ; blkRow < MOTION_SEARCH_ROW_COUNT ; blkRow++) {
        for (blkCol=0 ; blkCol < MOTION_SEARCH_COLUMN_COUNT ; blkCol++) {

            // init stackFinal.
            stackFinalTop = 0;

            // get position.
            blkPos = blkRow * MOTION_SEARCH_COLUMN_COUNT + blkCol;

            if (motionDataDifference[blkPos].pixelData) {   // movement detected, threshold exceeded and in motion map area.
                if (!motionMapGroup[ blkPos ]) {    // not checked for other group yet.

                    // init stackTemporary, stackFinal;
                    stackTemporaryTop = 0;
                    stackFinalTop = 0;

                    // push current block to stackTemporary.
                    stackTemporary[stackTemporaryTop++] = (blkRow << 16) | blkCol;

                    // loop until immediate stack not empty.
                    while( stackTemporaryTop > 0 ) {

                        // pop one from stackTemporary.
                        value = stackTemporary[--stackTemporaryTop];

                        // extract row,col from data.
                        tmpBlkRow = value >> 16;
                        tmpBlkCol = value & 0x0000ffff;


                        // check surrounded blocks.
#if defined(CONNECTED_COMPONENT_WITH_DIAGONAL)
                        for (i=0;i<8;i++) {
                            curBlkRow = tmpBlkRow + blockAround[i][0];
                            curBlkCol = tmpBlkCol + blockAround[i][1];
#else
                        for (i=0;i<4;i++) {
                            curBlkRow = tmpBlkRow + blockAroundWithoutDiagonal[i][0];
                            curBlkCol = tmpBlkCol + blockAroundWithoutDiagonal[i][1];
#endif
                            curBlkPos = curBlkRow * MOTION_SEARCH_COLUMN_COUNT + curBlkCol;

                            tmpValue = (curBlkRow << 16) | curBlkCol;

                            // check position :
                            if (curBlkRow >= MOTION_SEARCH_ROW_COUNT || curBlkCol >= MOTION_SEARCH_COLUMN_COUNT ) {
                                continue;
                            }

                            /*
                             * if current position doesn't belong to any group and not in immediate stack,.
                             * push it to temporary stack.
                             */
                            if (motionDataDifference[curBlkPos].pixelData &&      // if threshold exceeded & motion mask checked.
                                    !motionMapGroup[curBlkPos] &&       // if not grouped.
                                    !findInArrayUINT(tmpValue, stackTemporary, stackTemporaryTop) &&        // if not in stackTemporary.
                                    !findInArrayUINT(tmpValue, stackFinal, stackFinalTop)) {                // if not in stackFinal.

                                // push to temporary stack.
                                stackTemporary[stackTemporaryTop++] = (curBlkRow<<16) | curBlkCol;
                            }
                        }

                        //trace("sft=%d stt=%d row=%d col=%d\n", stackFinalTop, stackTemporaryTop, (value>>16), (value & 0x0000ffff)  );

                        // push current block to stackFinal and set checked.
                        stackFinal[stackFinalTop++] = value;

                        //trace("final: row=%d col=%d\n", (value>>16), (value & 0x0000ffff));
                        motionMapGroup[ tmpBlkRow * MOTION_SEARCH_COLUMN_COUNT + tmpBlkCol ] = 1;

                        // Immediate Decision(detected) :
                        // if number of blocks belonging to current group is bigger than.
                        // motionSearchMinBlock, motion's detected.(abort & return true.)
                        if (stackFinalTop >= motionSearchMinBlock) {
                            return 1;
                        }

                    }// while( stackTemporaryTop > 0 ) {

                    // push current group's number to stackGroup.
                    stackGroup[stackGroupTop++] = stackFinalTop;

                    // Immediate Decision(detected) :
                    if (stackFinalTop >= motionSearchMinBlock) {
                        return 1;
                    }

                }// if (!motionMapGroup[ blkRow * MOTION_SEARCH_ROW_COUNT + blkCol])
            }// if (motionDataDifference[ blkRow * MOTION_SEARCH_ROW_COUNT + blkCol])
        }// for (blkCol=0 ; blkCol < MOTION_SEARCH_COLUMN_COUNT ; blkCol++) {
    }// for (blkRow=0 ; blkRow < MOTION_SEARCH_ROW_COUNT ; blkRow++) {

    // for debug(delete later):

#if 0
    trace("detected group=");
    for(i=0;i<stackGroupTop;i++)
        trace("%d ", stackGroup[i]);
    trace(" count=%d\n", stackGroupTop);
#endif

    /**
     * Make Decision(detected or not.):
     * 1. Get biggest group's count --- a.
     * 2. If a >= motionSearchMinBlock, return detected.(1)
     * 3. else, return not.(0)
     */
    value = 0;
    for (i = 0; i < stackGroupTop; i++) {
        if ( value < stackGroup[i] ) {
            value = stackGroup[i];
        }
    }
    if (value >= motionSearchMinBlock) {
        return 1;
    }
    else {
        return 0;
    }

    return 1;
}


/**
 * Function for motion searching on current frame.
 *
 * @param vo_buffer      video out buffer.
 * @param width
 * @param height
 * @param stride         number of bytes from one row of pixels in memory to the next row of pixels in memory.
 * @param play           get setmentId,time,tick from this if motion detected.
 * @param detected       variable for out. motion search result.
 *                       will be here. (1 if detected, 0 else, caller needs this.)
 *
 * @param motionOptions  motion search options.
 * @param motionNew      true if motion search is started.
 * @param motionData     if detected, this will be filled with proper data. (caller needs this.)
 * @return               void.
 */


void MotionSearch::doMotionSearch(
        unsigned char *vo_buffer, // _video_out_buffer.
        unsigned int width,
        unsigned int height,
        unsigned int stride,
        unsigned int motionZoneRowCount,
        unsigned int motionZoneColCount,
        MotionOptions* motionOptions, // it has motionMode, motionSensitivity, minBlocks, motionMask.
        bool motionNew,                    // motion search start or ongoing.
        uint32_t frameTime,                    // segmentId, time, tick needed when motion detected.
        //unsigned int *detected,            // if detected, set to 1 inside.
        MotionResult *results             // if detected, it's used by caller.
    )
{
	bool resolutionChanged = false;
    int result;
    unsigned int motionMinBlocks=0;
    unsigned int threshold = (30 - 5 * (motionOptions->motionSensitivity));

    assert(_delegate != NULL);

    // init as not detected.
    //*detected = 0;

    if (_width != width || _height != height) {
        resolutionChanged = true;
    }

    // set record video size.
    _width = width;
    _height = height;

    // save motion mask & first frame if motionNew not zero.
    if (motionNew || resolutionChanged) {
        _delegate->getBlockAvg(vo_buffer, curMotionAvgValue, width, height, stride);

        // in museum search, refMotionAvgValue holds first frame data and  preserved during search.
        ::memcpy(refMotionAvgValue, curMotionAvgValue, sizeof(refMotionAvgValue));
        goto save_ref;
    }
    else if ( lastDetectedFrameTime == frameTime) {
        // 초 단위로 check하므로, 이미 움직임을 확인했으면 다음 초가 될 때까지 데이터 업데이트만 하고 넘긴다.
        _delegate->getBlockAvg(vo_buffer, refMotionAvgValue, width, height, stride);
        return;
    }

    /**
     * get block Y value from current frame.
     * must be provided by _delegate(MotionBlockAvgDelegate)
     */
    _delegate->getBlockAvg(vo_buffer, curMotionAvgValue, width, height, stride);
    //dumpMotionMap(curMotionAvgValue, "current");

    /**
     * calculate compared motion.
     * refMotionAvgValue를 diff로 덮어쓰고 또 curMotionAvgValue 로 덮어 쓴다.
     */
    result = compareMotion(refMotionAvgValue, curMotionAvgValue, inputMotionMask, threshold);
    motionMinBlocks = motionOptions->minBlocks;
    if (result >= motionMinBlocks) {
        result = checkConnectedComponent(refMotionAvgValue, motionOptions->minBlocks);
        if (result) {
            /**
             * motion detected. fill motionData using play(MCP_PLAY*) and.
             * set detected=1.
             */

            //dumpMotionMap(refMotionAvgValue, "diff");

            lastDetectedFrameTime = frameTime;
            results->time = frameTime;
            results->detected = true;
            //*detected = 1;

            dumpMotionMap(refMotionAvgValue, "detected");

            // 아래 코드는 필요 없음
#if 0
            MotionZone<MOTION_SEARCH_ROW_COUNT,MOTION_SEARCH_COLUMN_COUNT> zone;
            MotionBlockObject *mbo;

            mbo = refMotionAvgValue;

            for (int32_t row = 0; row < motionZoneRowCount; row++) {
                for (int32_t col = 0; col < motionZoneColCount; col++) {
                    uint32_t blkPos = row * motionZoneColCount + col;
                    zone.setZone(row, col, mbo[blkPos].pixelData ? true : false);
                }
            }
            memcpy(motionData->motionMask, zone.zone(), (motionZoneRowCount * motionZoneColCount) >> 3);
#endif

        }
    }

save_ref:
     // write current motion data to ref data
    ::memcpy(refMotionAvgValue, curMotionAvgValue, sizeof(refMotionAvgValue));
    return;
}

// <EOF>
