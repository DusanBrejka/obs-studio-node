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

#include "callback-manager.h"
#include "osn-source.hpp"
#ifdef WIN32
#include <windows.h>
#endif
#include "error.hpp"
#include "shared-server.hpp"
#include "osn-source.hpp"
#include "osn-volmeter.hpp"

std::mutex                             sources_sizes_mtx;
std::map<std::string, SourceSizeInfo*> sources;

void CallbackManager::GlobalQuery(
    void*                          data,
    const int64_t                  id,
    const std::vector<ipc::value>& args,
    std::vector<ipc::value>&       rval)
{
	// if (!sources.empty()) {
	// 	sources_sizes_mtx.lock();
	// 	uint32_t size = 0;

	// 	for (auto item : sources) {
	// 		SourceSizeInfo* si = item.second;
	// 		// See if width or height changed here
	// 		uint32_t newWidth  = obs_source_get_width(si->source);
	// 		uint32_t newHeight = obs_source_get_height(si->source);
	// 		uint32_t newFlags  = obs_source_get_output_flags(si->source);

	// 		if (si->width != newWidth || si->height != newHeight || si->flags != newFlags) {
	// 			si->width = newWidth;
	// 			si->height = newHeight;
	// 			si->flags  = newFlags;

	// 			rval.push_back(ipc::value(obs_source_get_name(si->source)));
	// 			rval.push_back(ipc::value(si->width));
	// 			rval.push_back(ipc::value(si->height));
	// 			rval.push_back(ipc::value(si->flags));

	// 			size++;
	// 		}
	// 	}

	// 	rval.insert(rval.begin() + 1, ipc::value(size));
	// 	sources_sizes_mtx.unlock();
	// } else {
	// 	rval.insert(rval.begin() + 1, ipc::value((uint32_t)0));
	// }
}

void CallbackManager::addSource(obs_source_t* source)
{
	uint32_t flags= obs_source_get_output_flags(source);
	if ((flags & OBS_SOURCE_VIDEO) == 0)
		return;

	if (!source ||
		obs_source_get_type(source) == OBS_SOURCE_TYPE_FILTER ||
		obs_source_get_type(source) == OBS_SOURCE_TYPE_TRANSITION ||
		obs_source_get_type(source) == OBS_SOURCE_TYPE_SCENE)
		return;

	std::unique_lock<std::mutex> ulock(sources_sizes_mtx);

	SourceSizeInfo* si               = new SourceSizeInfo;
	si->source                       = source;
	si->width                        = obs_source_get_width(source);
	si->height                       = obs_source_get_height(source);

	sources.emplace(std::make_pair(std::string(obs_source_get_name(source)), si));
}
void CallbackManager::removeSource(obs_source_t* source)
{
	std::unique_lock<std::mutex> ulock(sources_sizes_mtx);
	
	if (!source)
		return;

	const char* name = obs_source_get_name(source);
	
	if (name)
		sources.erase(name);
}