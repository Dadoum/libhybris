/*
 * Copyright (c) 2012 Carsten Munk <carsten.munk@gmail.com>
 * Copyright (c) 2012 Canonical Ltd
 * Copyright (c) 2013 Christophe Chapuis <chris.chapuis@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#define _GNU_SOURCE

#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include <hybris/common/hooks.h>

// this is also used in bionic:
#define bool int


#ifdef WANT_ARM_TRACING
#include "wrappers.h"
#endif

static int locale_inited = 0;
static hybris_hook_cb hook_callback = NULL;

#ifdef WANT_ARM_TRACING
static void (*_android_linker_init)(int sdk_version, void* (*get_hooked_symbol)(const char*, const char*), int enable_linker_gdb_support, void *(_create_wrapper)(const char*, void*, int)) = NULL;
#else
static void android_linker_init(int sdk_version, void* (*get_hooked_symbol)(const char*, const char*), int enable_linker_gdb_support);
#endif

void* android_dlopen(const char* filename, int flag);
char* android_dlerror();
void* android_dlsym(void* handle, const char* symbol);
void* android_dlvsym(void* handle, const char* symbol, const char* version);
int   android_dladdr(const void* addr, void* info);
int   android_dlclose(void* handle);

#include "logging.h"
#define LOGD(message, ...) HYBRIS_DEBUG_LOG(HOOKS, message, ##__VA_ARGS__)

void hybris_set_hook_callback(hybris_hook_cb callback)
{
    hook_callback = callback;
}

#define LINKER_NAME_Q "q"

#define LINKER_VERSION_DEFAULT 29
#define LINKER_NAME_DEFAULT LINKER_NAME_Q


static void* __hybris_get_hooked_symbol(const char *sym, const char *requester)
{
    void *found = NULL;

    if (hook_callback)
    {
        found = hook_callback(sym, requester);
        if (found)
            return (void*) found;
    }

    return NULL;
}

static int linker_initialized = 0;

static void __hybris_linker_init()
{
	LOGD("Linker initialization");

	int enable_linker_gdb_support = 0;
	const char *env = getenv("HYBRIS_ENABLE_LINKER_DEBUG_MAP");
	if (env != NULL) {
		if (strcmp(env, "1") == 0) {
			enable_linker_gdb_support = 1;
		}
	}

	int sdk_version = LINKER_VERSION_DEFAULT;

	char path[PATH_MAX];
	const char *name = LINKER_NAME_DEFAULT;

	/* See https://source.android.com/source/build-numbers.html for
	 * an overview over available SDK version numbers and which
	 * Android version they relate to. */
#if defined(WANT_LINKER_Q)
	if (sdk_version <= 29)
        name = LINKER_NAME_Q;
#endif
#if defined(WANT_LINKER_O)
	if (sdk_version <= 27)
        name = LINKER_NAME_O;
#endif
#if defined(WANT_LINKER_N)
	if (sdk_version <= 25)
        name = LINKER_NAME_N;
#endif
#if defined(WANT_LINKER_MM)
	if (sdk_version <= 23)
        name = LINKER_NAME_MM;
#endif
#if defined(WANT_LINKER_JB)
	if (sdk_version < 21)
        name = LINKER_NAME_JB;
#endif

	android_linker_init(sdk_version, __hybris_get_hooked_symbol, enable_linker_gdb_support);

	linker_initialized = 1;
}

#define ENSURE_LINKER_IS_LOADED() \
    if (!linker_initialized) \
        __hybris_linker_init();

void* hybris_dlopen(const char* filename, int flag)
{
	ENSURE_LINKER_IS_LOADED()
    return android_dlopen(filename, flag);
}

char* hybris_dlerror()
{
	ENSURE_LINKER_IS_LOADED()
    return android_dlerror();
}

void* hybris_dlsym(void* handle, const char* symbol)
{
	ENSURE_LINKER_IS_LOADED()
    return android_dlsym(handle, symbol);
}

void* hybris_dlvsym(void* handle, const char* symbol, const char* version)
{
	ENSURE_LINKER_IS_LOADED()
    return android_dlvsym(handle, symbol, version);
}

int hybris_dladdr(const void* addr, void* info)
{
	ENSURE_LINKER_IS_LOADED()
    return android_dladdr(addr, info);
}

int hybris_dlclose(void* handle)
{
	ENSURE_LINKER_IS_LOADED()
    return android_dlclose(handle);
}
