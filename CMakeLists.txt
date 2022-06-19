cmake_minimum_required(VERSION 3.17)
include(FetchContent)
project(hybris C CXX)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 11)
add_library(hybris
		hooks.c
		hooks_shm.c
		strlcpy.c
		strlcat.c
		logging.c
		sysconf.c
		dso_handle_counters.cpp
		strlcpy.c
		strlcat.c
		linker/hybris_compat.cpp
		linker/dlfcn.cpp
		linker/linker_allocator.cpp
		linker/linker_block_allocator.cpp
		linker/linker.cpp
		linker/linker_cfi.cpp
		linker/linker_config.cpp
		linker/linker_dlwarning.cpp
		linker/linker_gdb_support.cpp
		linker/linker_globals.cpp
		linker/linker_logger.cpp
		linker/linker_main.cpp
		linker/linker_mapped_file_fragment.cpp
		linker/linker_memory.cpp
		linker/linker_namespaces.cpp
		linker/linker_phdr.cpp
		linker/linker_sdk_versions.cpp
		linker/linker_soinfo.cpp
		linker/linker_tls.cpp
		linker/linker_utils.cpp
		linker/rt.cpp
		legacy_properties/properties.c
		legacy_properties/runtime_cache.c
		legacy_properties/cache.c)

target_link_libraries(hybris dl rt pthread c)

target_include_directories(hybris PUBLIC include)
target_include_directories(hybris PUBLIC linker/bionic/libc/)
