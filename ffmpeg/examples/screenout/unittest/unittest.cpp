#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "StreamQueue_test"
#endif

#include <gtest/gtest.h>

#include "../StreamQueue.h"
#include "library/basic/exlog.h"
#include <memory>

using namespace std;
using namespace vh1981lib;

int gexstringtest = 0;

TEST(exstring_test, ctor)
{
    {
        StreamQueue queue(2048);

        shared_ptr<unsigned char> data(new unsigned char[256*4], [](unsigned char *p) { delete[] p; });
        shared_ptr<unsigned char> out(new unsigned char[2048 + 256], [](unsigned char *p) { delete[] p; });
        for (int i = 0; i < 256*4; i++) {
            data.get()[i] = i;
        }
        queue.put(data.get(), 256*3);
        queue.put(data.get(), 256*3);
        queue.get(out.get(), 500);
        int idx = 0;
        for (int i = 0; i < 500; i++) {
            EXPECT_TRUE(idx == out.get()[i]);
            idx = (idx + 1) % 256;
        }

        queue.put(data.get(), 256*3);

        queue.get(out.get(), 256*3);
        for (int i = 0; i < 256*3; i++) {
            EXPECT_TRUE(idx == out.get()[i]);
            idx = (idx + 1) % 256;
        }

        queue.get(out.get(), 256*3);
        for (int i = 0; i < 256*3; i++) {
            EXPECT_TRUE(idx == out.get()[i]);
            idx = (idx + 1) % 256;
        }

        EXCLOG(LOG_INFO, "queue.freeSize()=%d", queue.freeSize());

        int pcnt = (queue.freeSize() / 256);
        if (pcnt) {
            for (int i = 0; i < pcnt; i++) {
                EXCLOG(LOG_INFO, "i=%d", i);
                queue.put(data.get(), 256);
            }
        }

        queue.get(out.get(), 200);
        for (int i = 0; i < 200; i++) {
            //EXCLOG(LOG_INFO, "idx=%d out.get[%d]=%d", idx, i, out.get()[i]);
            EXPECT_EQ(idx , out.get()[i]);
            idx = (idx + 1) % 256;
        }

        queue.get(out.get(), 300);
        for (int i = 0; i < 300; i++) {
            EXPECT_TRUE(idx == out.get()[i]);
            idx = (idx + 1) % 256;
        }

        queue.get(out.get(), 120);
        for (int i = 0; i < 120; i++) {
            EXPECT_TRUE(idx == out.get()[i]);
            idx = (idx + 1) % 256;
        }

        int dataSize = queue.dataSize();
        queue.get(out.get(), dataSize);
        for (int i = 0; i < dataSize; i++) {
            EXPECT_TRUE(idx == out.get()[i]);
            idx = (idx + 1) % 256;
        }
        EXPECT_EQ(0, queue.dataSize());
        EXPECT_TRUE(queue.empty());

        pcnt = (queue.freeSize() / 256);
        if (pcnt) {
            for (int i = 0; i < pcnt; i++) {
                EXCLOG(LOG_INFO, "i=%d", i);
                queue.put(data.get(), 256);
            }
        }

        queue.get(out.get(), 700);
        for (int i = 0; i < 700; i++) {
            EXPECT_TRUE(idx == out.get()[i]);
            idx = (idx + 1) % 256;
        }

        queue.put(data.get(), 256*2);

        queue.get(out.get(), 600);
        for (int i = 0; i < 600; i++) {
            EXPECT_TRUE(idx == out.get()[i]);
            idx = (idx + 1) % 256;
        }

        queue.get(out.get(), 300);
        for (int i = 0; i < 300; i++) {
            EXPECT_TRUE(idx == out.get()[i]);
            idx = (idx + 1) % 256;
        }

        queue.put(data.get(), 256*2);

        queue.get(out.get(), 1200);
        for (int i = 0; i < 1200; i++) {
            EXPECT_TRUE(idx == out.get()[i]);
            idx = (idx + 1) % 256;
        }

        dataSize = queue.dataSize();
        queue.get(out.get(), dataSize);
        for (int i = 0; i < dataSize; i++) {
            EXPECT_TRUE(idx == out.get()[i]);
            idx = (idx + 1) % 256;
        }
        EXPECT_EQ(0, queue.dataSize());
        EXPECT_TRUE(queue.empty());


        EXCLOG(LOG_INFO, "datasize=%d empty=%s", queue.dataSize(), queue.empty() ? "yes":"no");

    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
