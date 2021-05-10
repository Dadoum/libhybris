# libhybris

## Kesako ?

libhybris est une bibliothèque C permettant de charger des bibliothèques android, càd basé sur la libc bioniC.

Ce fork a plusieurs objectifs:
 - rendre libhybris portable, sans nécessiter d'installation
 - utiliser CMake
 - l'alléger

## Compilation

Clonez le projet et compilez le avec CMake:

```bash
git clone https://github.com/Dadoum/libhybris
cd libhybris
mkdir build && cd build
cmake ..
make
```

## Utiliser dans son projet

Ajoutez ces lignes à votre CMakeLists.txt:

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

## Utilisation

### Invoquer une fonction

```c
#include <hybris/common/dlfcn.h>

int main() {
    auto handle = hybris_dlopen("libhelloworld.so", RTLD_NOW); // ouvrir la bibliothèque libhelloworld.so (dans les faits, il faudra toujours charger la libc.so et le ld-android.so avant)
    auto sayHello = (void(*)()) hybris_dlsym(handle, "sayHello");  // charge la fonction sayHello() de la bibliothèque
    sayHello(); // invoque sayHello
}
```

Pour plus de détail, voir l'utilisation de dlfcn sur Linux.

### Modifier une fonction ou en proposer une implémentation

```c++
#include <string.h>
#include <hybris/common/dlfcn.h>
#include <hybris/common/hooks.h>

bool customIsHooked() {
	return true;
}

void* hookCallback(const char *symbol_name, const char *requester) {
	if (strcmp(symbol_name, "isHooked") == 0 && strcmp(requester, "libhelloworld.so") == 0) { // si le symbole demandé est isHooked et que c'est de la bibliothèque libhelloworld.so
        return (void*) customIsHooked;
	}
	return NULL; 
}

int main() {
	hybris_set_hook_callback(hookCallback);
    auto handle = hybris_dlopen("libhelloworld.so", RTLD_NOW); // ouvrir la bibliothèque libhelloworld.so (dans les faits, il faudra toujours charger la libc.so et le ld-android.so avant)
    auto sayHello = (void(*)()) hybris_dlsym(handle, "isHooked");  // charge la fonction isHooked() de la bibliothèque
    assert(isHooked()); // invoque isHooked, qui est forcément vrai vu qu'on l'a modifiée.
}
```
