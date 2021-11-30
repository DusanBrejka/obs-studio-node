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

#include "nodeobs_display.hpp"
#include "utility-v8.hpp"

#include <node.h>
#include <sstream>
#include <string>
#include "shared.hpp"
#include "utility.hpp"
#include "callback-manager.hpp"
#include "server/nodeobs_content.h"

#ifdef WIN32
static BOOL CALLBACK EnumChromeWindowsProc(HWND hwnd, LPARAM lParam)
{
	char buf[256];
	if (GetClassNameA(hwnd, buf, sizeof(buf) / sizeof(*buf))) {
		if (strstr(buf, "Intermediate D3D Window")) {
			LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
			if ((style & WS_CLIPSIBLINGS) == 0) {
				style |= WS_CLIPSIBLINGS;
				SetWindowLongPtr(hwnd, GWL_STYLE, style);
			}
		}
	}
	return TRUE;
}

static void FixChromeD3DIssue(HWND chromeWindow)
{
	(void)EnumChildWindows(chromeWindow, EnumChromeWindowsProc, (LPARAM)NULL);

	LONG_PTR style = GetWindowLongPtr(chromeWindow, GWL_STYLE);
	if ((style & WS_CLIPCHILDREN) == 0) {
		style |= WS_CLIPCHILDREN;
		SetWindowLongPtr(chromeWindow, GWL_STYLE, style);
	}
}
#endif

Napi::Value display::OBS_content_createDisplay(const Napi::CallbackInfo& info)
{
	PROFINY_SCOPE
	Napi::Buffer<void *> bufferData = info[0].As<Napi::Buffer<void*>>();
	uint64_t* windowHandle = static_cast<uint64_t*>(*reinterpret_cast<void **>(bufferData.Data()));

#ifdef WIN32
	FixChromeD3DIssue((HWND)windowHandle);
#endif

	std::string key = info[1].ToString().Utf8Value();
	int32_t mode = info[2].ToNumber().Int32Value();

	OBS_content::OBS_content_createDisplay((uint64_t)windowHandle, key, mode);

	return info.Env().Undefined();
}

Napi::Value display::OBS_content_destroyDisplay(const Napi::CallbackInfo& info)
{
	PROFINY_SCOPE
	std::string key = info[0].ToString().Utf8Value();

	OBS_content::OBS_content_destroyDisplay(key);

	return info.Env().Undefined();
}

Napi::Value display::OBS_content_getDisplayPreviewOffset(const Napi::CallbackInfo& info)
{
	PROFINY_SCOPE
	std::string key = info[0].ToString().Utf8Value();

	Napi::Object previewOffset = Napi::Object::New(info.Env());
	auto offset = OBS_content::OBS_content_getDisplayPreviewOffset(key);

	previewOffset.Set("x", Napi::Number::New(info.Env(), offset.first));
	previewOffset.Set("y", Napi::Number::New(info.Env(), offset.second));
	return previewOffset;
}

Napi::Value display::OBS_content_getDisplayPreviewSize(const Napi::CallbackInfo& info)
{
	PROFINY_SCOPE
	std::string key = info[0].ToString().Utf8Value();

	Napi::Object previewSize = Napi::Object::New(info.Env());
	auto size = OBS_content::OBS_content_getDisplayPreviewSize(key);
	previewSize.Set("width", Napi::Number::New(info.Env(), size.first));
	previewSize.Set("height", Napi::Number::New(info.Env(), size.second));
	return previewSize;
}

Napi::Value display::OBS_content_createSourcePreviewDisplay(const Napi::CallbackInfo& info)
{
	PROFINY_SCOPE
	Napi::Buffer<void *> bufferData = info[0].As<Napi::Buffer<void*>>();
	uint64_t* windowHandle = static_cast<uint64_t*>(*reinterpret_cast<void **>(bufferData.Data()));

#ifdef WIN32
	FixChromeD3DIssue((HWND)windowHandle);
#endif

	std::string sourceName = info[1].ToString().Utf8Value();
	std::string key = info[2].ToString().Utf8Value();

	OBS_content::OBS_content_createSourcePreviewDisplay((uint64_t)windowHandle, sourceName, key);

	return info.Env().Undefined();
}

Napi::Value display::OBS_content_resizeDisplay(const Napi::CallbackInfo& info)
{
	PROFINY_SCOPE
	std::string key = info[0].ToString().Utf8Value();
	uint32_t width = info[1].ToNumber().Uint32Value();
	uint32_t height = info[2].ToNumber().Uint32Value();

	OBS_content::OBS_content_resizeDisplay(key, width, height);
	return info.Env().Undefined();
}

Napi::Value display::OBS_content_moveDisplay(const Napi::CallbackInfo& info)
{
	PROFINY_SCOPE
	std::string key = info[0].ToString().Utf8Value();
	uint32_t x = info[1].ToNumber().Uint32Value();
	uint32_t y = info[2].ToNumber().Uint32Value();

	OBS_content::OBS_content_moveDisplay(key, x, y);
	return info.Env().Undefined();
}

Napi::Value display::OBS_content_setPaddingSize(const Napi::CallbackInfo& info)
{
	PROFINY_SCOPE
	std::string key = info[0].ToString().Utf8Value();
	uint32_t paddingSize = info[1].ToNumber().Uint32Value();

	OBS_content::OBS_content_setPaddingSize(key, paddingSize);
	return info.Env().Undefined();
}

Napi::Value display::OBS_content_setPaddingColor(const Napi::CallbackInfo& info)
{
	PROFINY_SCOPE
	std::string key = info[0].ToString().Utf8Value();
	uint32_t r = info[1].ToNumber().Uint32Value();
	uint32_t g = info[2].ToNumber().Uint32Value();
	uint32_t b = info[3].ToNumber().Uint32Value();
	uint32_t a = 255;

	if (info.Length() > 4)
		a = info[4].ToNumber().Uint32Value();

	OBS_content::OBS_content_setPaddingColor(key, r, g, b, a);
	return info.Env().Undefined();
}

Napi::Value display::OBS_content_setOutlineColor(const Napi::CallbackInfo& info)
{
	PROFINY_SCOPE
	std::string key = info[0].ToString().Utf8Value();
	uint32_t r = info[1].ToNumber().Uint32Value();
	uint32_t g = info[2].ToNumber().Uint32Value();
	uint32_t b = info[3].ToNumber().Uint32Value();
	uint32_t a = 255;

	if (info.Length() > 4)
		a = info[4].ToNumber().Uint32Value();

	OBS_content::OBS_content_setOutlineColor(key, r, g, b, a);
	return info.Env().Undefined();
}

Napi::Value display::OBS_content_setShouldDrawUI(const Napi::CallbackInfo& info)
{
	PROFINY_SCOPE
	std::string key = info[0].ToString().Utf8Value();
	bool drawUI = info[1].ToBoolean().Value();

	OBS_content::OBS_content_setShouldDrawUI(key, drawUI);
	return info.Env().Undefined();
}

Napi::Value display::OBS_content_setDrawGuideLines(const Napi::CallbackInfo& info)
{
	PROFINY_SCOPE
	std::string key = info[0].ToString().Utf8Value();
	bool drawGuideLines = info[1].ToBoolean().Value();

	OBS_content::OBS_content_setDrawGuideLines(key, drawGuideLines);
	return info.Env().Undefined();
}

Napi::Value display::OBS_content_createIOSurface(const Napi::CallbackInfo& info)
{
	// std::string key = info[0].ToString().Utf8Value();

	// auto conn = GetConnection(info);
	// if (!conn)
	// 	return info.Env().Undefined();

	// std::vector<ipc::value> response =
	//     conn->call_synchronous_helper("Display", "OBS_content_createIOSurface", {ipc::value(key)});

	// if (!ValidateResponse(info, response))
	// 	return info.Env().Undefined();

	// return Napi::Number::New(info.Env(), response[1].value_union.ui32);
	return Napi::Number::New(info.Env(), 0);
}

void display::Init(Napi::Env env, Napi::Object exports)
{
	exports.Set(
		Napi::String::New(env, "OBS_content_createDisplay"),
		Napi::Function::New(env, display::OBS_content_createDisplay));
	exports.Set(
		Napi::String::New(env, "OBS_content_destroyDisplay"),
		Napi::Function::New(env, display::OBS_content_destroyDisplay));
	exports.Set(
		Napi::String::New(env, "OBS_content_getDisplayPreviewOffset"),
		Napi::Function::New(env, display::OBS_content_getDisplayPreviewOffset));
	exports.Set(
		Napi::String::New(env, "OBS_content_getDisplayPreviewSize"),
		Napi::Function::New(env, display::OBS_content_getDisplayPreviewSize));
	exports.Set(
		Napi::String::New(env, "OBS_content_createSourcePreviewDisplay"),
		Napi::Function::New(env, display::OBS_content_createSourcePreviewDisplay));
	exports.Set(
		Napi::String::New(env, "OBS_content_resizeDisplay"),
		Napi::Function::New(env, display::OBS_content_resizeDisplay));
	exports.Set(
		Napi::String::New(env, "OBS_content_moveDisplay"),
		Napi::Function::New(env, display::OBS_content_moveDisplay));
	exports.Set(
		Napi::String::New(env, "OBS_content_setPaddingSize"),
		Napi::Function::New(env, display::OBS_content_setPaddingSize));
	exports.Set(
		Napi::String::New(env, "OBS_content_setPaddingColor"),
		Napi::Function::New(env, display::OBS_content_setPaddingColor));
	exports.Set(
		Napi::String::New(env, "OBS_content_setShouldDrawUI"),
		Napi::Function::New(env, display::OBS_content_setShouldDrawUI));
	exports.Set(
		Napi::String::New(env, "OBS_content_setDrawGuideLines"),
		Napi::Function::New(env, display::OBS_content_setDrawGuideLines));
	exports.Set(
		Napi::String::New(env, "OBS_content_createIOSurface"),
		Napi::Function::New(env, display::OBS_content_createIOSurface));
}