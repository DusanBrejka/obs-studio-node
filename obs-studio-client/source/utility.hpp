/******************************************************************************
    Copyright (C) 2016-2019 by Streamlabs (General Workings Inc)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/

#pragma once
#include <memory>
#include <string>
#include <vector>

#include <napi.h>
#include "shared.hpp"
#include <thread>
#include "data-value.hpp"
#include "thread_pool.hpp"

#ifdef __cplusplus
#define INITIALIZER(f)   \
	static void f(void); \
	struct f##_t_        \
	{                    \
		f##_t_(void)     \
		{                \
			f();         \
		}                \
	};                   \
	static f##_t_ f##_;  \
	static void   f(void)
#elif defined(_MSC_VER)
#pragma section(".CRT$XCU", read)
#define INITIALIZER2_(f, p)                                  \
	static void f(void);                                     \
	__declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
	__pragma(comment(linker, "/include:" p #f "_")) static void f(void)
#ifdef _WIN64
#define INITIALIZER(f) INITIALIZER2_(f, "")
#else
#define INITIALIZER(f) INITIALIZER2_(f, "_")
#endif
#else
#define INITIALIZER(f)                                \
	static void f(void) __attribute__((constructor)); \
	static void f(void)
#endif

#ifndef __FUNCTION_NAME__
#if defined(_WIN32) || defined(_WIN64) //WINDOWS
#define __FUNCTION_NAME__ __FUNCTION__
#else //*NIX
#define __FUNCTION_NAME__ __func__
#endif
#endif

#if defined(_MSC_VER)
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE __attribute__((always_inline))
#endif
#define force_inline FORCE_INLINE

#ifdef __GNUC__
#define __deprecated__ __attribute_deprecated__
#else
#define __deprecated__ __declspec(deprecated)
#endif

#define dstr(s) #s
#define vstr(s) dstr(s)

typedef std::pair<std::string, data::value> settings_value;

namespace utility
{
	template<typename T>
	inline std::string TypeOf(T v)
	{
		return "unknown";
	}

#define AUTO_TYPEOF(x)                \
	template<>                        \
	inline std::string TypeOf<x>(x v) \
	{                                 \
		return dstr(x);               \
	}

#define AUTO_TYPEOF_NAME(x, y)        \
	template<>                        \
	inline std::string TypeOf<x>(x v) \
	{                                 \
		return y;                     \
	}

	AUTO_TYPEOF_NAME(bool, "boolean");
	AUTO_TYPEOF(int8_t);
	AUTO_TYPEOF(int16_t);
	AUTO_TYPEOF(int32_t);
	AUTO_TYPEOF(int64_t);
	AUTO_TYPEOF(uint8_t);
	AUTO_TYPEOF(uint16_t);
	AUTO_TYPEOF(uint32_t);
	AUTO_TYPEOF(uint64_t);
	AUTO_TYPEOF_NAME(const char*, "string");
	AUTO_TYPEOF_NAME(std::string, "string");
	AUTO_TYPEOF(std::vector<char>);

	AUTO_TYPEOF_NAME(Napi::Value, "value");
	AUTO_TYPEOF_NAME(Napi::Object, "object");
	AUTO_TYPEOF_NAME(Napi::Function, "function");

	// This is from enc-amf
#if (defined _WIN32) || (defined _WIN64)
	void SetThreadName(uint32_t dwThreadID, const char* threadName);
	void SetThreadName(const char* threadName);
	//void SetThreadName(std::thread* pthread, const char* threadName);
#else
	void SetThreadName(std::thread* pthread, const char* threadName);
	void SetThreadName(const char* threadName);
#endif
} // namespace utility

std::string  from_utf16_wide_to_utf8(const wchar_t* from, size_t length = -1);
std::wstring from_utf8_to_utf16_wide(const char* from, size_t length = -1);

std::string getSafeOBSstr(const char* obsSTR);

//write detected possible reason of abnormal app close to a file used to submit statistics 
// void ipc_freez_callback(bool freez_detected, std::string app_state_path);

#define MAX_THREADS_POOL 1
extern thread_pool* pool;

void createThreadPool();
void destroyThreadPool();

template <typename F>
void pushTask(const F &task) {
	pool->push_task(task);
};

template <typename F, typename... A>
void pushTask(const F &task, const A &...args) {
	pool->push_task(task, args...);
};