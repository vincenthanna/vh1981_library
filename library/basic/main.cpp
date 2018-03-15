#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exstring.h"

using namespace std;
using namespace vh1981lib;

int main(int argc, char** argv) {
    exstring str;
    exstring format("%d %d %s");
    str.format(format, 10, 20, "Hello World");
    printf("test1 : %s\n", str.to_string().c_str());

    str.appendf(" data1:%d data2:%02x", 100, 100);
    printf("test2:%s", str.to_string().c_str());
    return 0;
}
