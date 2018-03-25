#ifndef _VH1981_EXTOOLS_
#define _VH1981_EXTOOLS_

namespace vh1981lib {
    #define array_access(array_name, idxname) for(size_t idxname = 0; i < sizeof(array_name) / sizeof((array_name)[0]); idxname++)

}

#endif
