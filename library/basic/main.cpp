#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exstring.h"

using namespace std;
using namespace vh1981lib;

int main(int argc, char** argv) {
    exstring str;
    exstring format = "%d %d";
    str.format(format, 10, 20);
    printf(str.to_string().c_str());
    return 0;
}
