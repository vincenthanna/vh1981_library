// MediaLAN 02/2013
// CRtspSession
// - JPEG test image simulaton

#include "JPEGSamples.h"

// RGB JPEG images as RTP payload - 48x32 pixel
unsigned char JpegScanDataCh1A[KJpegCh1ScanDataLen] = 
{
    0xf8, 0xbe, 0x8a, 0x28, 0xaf, 0xe5, 0x33, 0xfd, 
    0xfc, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x3f, 0xff, 0xd9                                      
};
unsigned char JpegScanDataCh1B[KJpegCh1ScanDataLen] = 
{
    0xf5, 0x8a, 0x28, 0xa2, 0xbf, 0xca, 0xf3, 0xfc, 
    0x53, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x3f, 0xff, 0xd9
};

// RGB JPEG images as RTP payload - 64x48 pixel
unsigned char JpegScanDataCh2A[KJpegCh2ScanDataLen] =
{
    0xf8, 0xbe, 0x8a, 0x28, 0xaf, 0xe5, 0x33, 0xfd, 
    0xfc, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x3f, 0xff, 0xd9            
};
unsigned char JpegScanDataCh2B[KJpegCh2ScanDataLen] =
{
    0xf5, 0x8a, 0x28, 0xa2, 0xbf, 0xca, 0xf3, 0xfc, 
    0x53, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x0a, 0x28, 0xa2, 
    0x80, 0x0a, 0x28, 0xa2, 0x80, 0x3f, 0xff, 0xd9            
};
