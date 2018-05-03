#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "exstring_test"
#endif

#include <gtest/gtest.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <string>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <memory>


#include "library/basic/exstring.h"
#include "library/basic/exlog.h"
#include "CRtspSession.h"
#include "RtspParser.h"
//#include "ServerBase.h"
#include "Packet.h"
#include "Session.h"

using namespace std;
using namespace vh1981lib;

int gexstringtest = 0;

const char* sample_text_OPTIONS = "OPTIONS rtsp://127.0.0.1:8554/mjpeg/1 RTSP/1.0\n"\
        "CSeq: 2\n"\
        "User-Agent: LibVLC/2.1.6 (LIVE555 Streaming Media v2014.01.13)";
const char* sample_text_DESCRIBE = "DESCRIBE rtsp://127.0.0.1:8554/mjpeg/1 RTSP/1.0\n"\
        "CSeq: 3\n"\
        "User-Agent: LibVLC/2.1.6 (LIVE555 Streaming Media v2014.01.13)\n"\
        "Accept: application/sdp";
const char* sample_text_SETUP = "SETUP rtsp://127.0.0.1:8554/mjpeg/1/ RTSP/1.0\n"\
        "CSeq: 4\n"\
        "User-Agent: LibVLC/2.1.6 (LIVE555 Streaming Media v2014.01.13)\n"\
        "Transport: RTP/AVP;unicast;client_port=60842-60843";
const char* sample_text_PLAY = "PLAY rtsp://127.0.0.1:8554/mjpeg/1/ RTSP/1.0\n"\
        "CSeq: 5\n"\
        "User-Agent: LibVLC/2.1.6 (LIVE555 Streaming Media v2014.01.13)\n"\
        "Session: 2004820934\n"\
        "Range: npt=0.000-";

//TEST(RtspParser_test, ctor)
//{
//
//    CRtspSession session(0, nullptr);
//    session.ParseRtspRequest(sample_text_OPTIONS, strlen(sample_text_OPTIONS));
//    session.ParseRtspRequest(sample_text_SETUP, strlen(sample_text_SETUP));
//    session.ParseRtspRequest(sample_text_DESCRIBE, strlen(sample_text_DESCRIBE));
//    session.ParseRtspRequest(sample_text_PLAY, strlen(sample_text_PLAY));
//
//}

TEST(Server, Packet)
{
    Packet packet(1536);
    unsigned char data[1000];
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = (i % 100);
    }

    memcpy(packet.buffer(), data, sizeof(data));
    packet.setDataLen(sizeof(data));

    for (int i = 0; i < 100; i++) {
        EXPECT_TRUE(packet.buffer()[i] == (i % 100));
    }
}

TEST(Server, Session)
{
    unsigned char testdata[100];

    Session session;
    const int packetCount = 10;
    for (int i = 0; i < packetCount; i++) {
        shared_ptr<Packet> packet(new Packet());
        unsigned char* buffer = packet->buffer();

        for (int j = 0; j < 100; j++) {
            testdata[j] = i;
        }

        memcpy(buffer, testdata, 100);
        session.putRecvPacket(packet);
    }

    for (int i = 0; i < packetCount; i++) {
        shared_ptr<Packet> packet = session.getRecvPacket();

        for (int j = 0; j < 100; j++) {
            EXPECT_EQ(packet->buffer()[j], i);
        }
    }

    EXPECT_EQ(session.recvPacketCount(), 0);
}

TEST(Server, test)
{

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
