#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "exstring_test"
#endif

#include <gtest/gtest.h>

#include "exstring.h"
#include "exlog.h"
#include "exthread.h"
#include "exmutex.h"

using namespace std;
using namespace vh1981lib;

int gexstringtest = 0;

TEST(exstring_test, ctor)
{
    {
        const char* teststr = "Hello World";
        exstring str(teststr);
        EXPECT_TRUE(str.to_string() == string("Hello World"));
        str = teststr;
        EXPECT_TRUE(str.to_string() == string("Hello World"));
    }
}

TEST(exstring_test, operators)
{
    {
        const char* teststr = "Hello World";
        const char* teststr2 = "Hello";
        exstring str = teststr;
        EXPECT_TRUE(str.to_string() == string("Hello World"));
        EXPECT_TRUE(exstring(teststr) > exstring(teststr2));
        EXPECT_TRUE(exstring(teststr2) < exstring(teststr));
    }
}

TEST(exstring_test, manipulation)
{
    {
        const char* t1 = "Hello";
        const char* t2 = "World";
        exstring str = t1;
        str.append(exstring(t2));
        EXPECT_TRUE(str == exstring("HelloWorld"));
        exstring str2(t1);
        str2.appendf("%s", t2);
        EXPECT_TRUE(str2 == exstring("HelloWorld"));
    }
}

TEST(exstring_test, log)
{
    {
        EXCLOG(LOG_WARN, exstring("Hello World"));
        EXCLOG(LOG_WARN, "%s %s", "Hello", "World");
        EXCLOG(LOG_WARN, "");

        EXCLOG(LOG_DEBUG, "LOG_DEBUG");
        EXCLOG(LOG_INFO, "LOG_INFO");
        EXCLOG(LOG_WARN, "LOG_WARN");
        EXCLOG(LOG_ERROR, "LOG_ERROR");
        EXCLOG(LOG_FATAL, "LOG_FATAL");

    }
}

unsigned int __val = 0;
class testexthread : public exthread
{
public:
    testexthread(exstring str);

    void threadFunc()
    {
        int cnt = 0;
        while(cnt++ < 100) {
            //EXCLOG(LOG_INFO, "thread running...%d", __val);
            __val++;
        }
    }
};

testexthread::testexthread(exstring str) : exthread(str)
{

}

TEST(exstring_test, exthreadtest)
{
    __val = 0;
    testexthread t(exstring("testthread"));
    t.run();

    ::usleep(10000);

    while(t.id() != 0) {
        ::usleep(1000);
    }
    //EXCLOG(LOG_INFO, "__val=%d", __val);
    EXPECT_TRUE(__val == 100);
}

static exmutex __mutex;
static unsigned int __inc = 0;

class testmutexthread : public exthread
{
public:
    testmutexthread(exstring str);

private:
    void threadFunc()
    {
        int cnt = 0;
        pthread_yield();
        while(cnt++ < 10000) {
            testfunc();
            if (__inc % 64 == 0) { pthread_yield(); } 
        }
    }

    void testfunc()
    {
        autoexmutex a(__mutex);
        if (__inc % 500 == 0) { pthread_yield(); } 
        __inc++;
    }
};

testmutexthread::testmutexthread(exstring str) : exthread(str)
{

}

TEST(exthread, exmutextest)
{
    __inc = 0;
    testmutexthread t1(exstring("testmutexthread"));
    t1.run(); 

    testmutexthread t2(exstring("testmutexthread"));
    t2.run();
    
    ::usleep(10000);

    while(t1.id() != 0 || t2.id() != 0) {
        ::usleep(1000);
    }

    EXPECT_TRUE(__inc == 20000);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
