#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "exstring_test"
#endif

#include <gtest/gtest.h>

#include "exstring.h"
#include "exlog.h"

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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
