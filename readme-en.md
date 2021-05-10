# libhybris

## What is this ?

libhybris is a C library allowing Android library loading, or more generally libraries that uses bioniC libc.

This fork has several objectives :
 - make libhybris portable
 - use CMake
 - debloat it
 
## Compiling

Clone the project and compile it with CMake:

```bash
git clone https://github.com/Dadoum/libhybris
cd libhybris
mkdir build && cd build
cmake ..
make
```

## Use it in our project

Add these lines to your CMakeLists.txt:

```cmake
FetchContent_Declare(
        libhybris
        GIT_REPOSITORY https://github.com/Dadoum/libhybris
        GIT_TAG master
)
FetchContent_MakeAvailable(libhybris)

add_dependencies(Provision hybris)
target_link_libraries(Provision hybris)
```

## Usage

### Invoke a function

```c
#include <hybris/common/dlfcn.h>

int main() {
    auto handle = hybris_dlopen("libhelloworld.so", RTLD_NOW); // opens libhelloworld.so (in real world, you will always have to load ld-android and libc before)
    auto sayHello = (void(*)()) hybris_dlsym(handle, "sayHello");  // loads "sayHello()" function in the library
    sayHello(); // invokes sayHello
}
```

For further details, search documentation about dlfcn on Linux

### Hook functions

```c++
#include <string.h>
#include <hybris/common/dlfcn.h>
#include <hybris/common/hooks.h>

bool customIsHooked() {
	return true;
}

void* hookCallback(const char *symbol_name, const char *requester) {
	if (strcmp(symbol_name, "isHooked") == 0 && strcmp(requester, "libhelloworld.so") == 0) { // if the asked symbol is isHooked and if the library requesting hook is libhelloworld.so
        return (void*) customIsHooked;
	}
	return NULL; 
}

int main() {
	hybris_set_hook_callback(hookCallback);
    auto handle = hybris_dlopen("libhelloworld.so", RTLD_NOW); // opens libhelloworld.so (in real world, you will always have to load ld-android and libc before)
    auto sayHello = (void(*)()) hybris_dlsym(handle, "isHooked");  // loads "isHooked()" function in the library
    assert(isHooked()); // invokes isHooked, which always returns true since we hooked it.
}
```
