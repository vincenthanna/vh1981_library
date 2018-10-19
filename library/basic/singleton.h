#ifndef _VH1981_SINGLETON_

#include <iostream>
#include <memory>
#include <assert.h>

#include "exlog.h"

using namespace vh1981lib;
using namespace std;

template <typename T>
class singleton {
public:
    singleton() {
        assert(_allocatingInstance);
    }
    static T* get();

private:
    static unique_ptr<T> _instance;

    static bool _allocatingInstance;
};

template <typename T>
T* singleton<T>::get()
{
    if (!_instance.get()) {

        _allocatingInstance = true;
        _instance.reset(new T);
        _allocatingInstance = false;

    }
    return _instance.get();
}

template <typename T>
unique_ptr<T> singleton<T>::_instance;

template <typename T>
bool singleton<T>::_allocatingInstance;

#endif
