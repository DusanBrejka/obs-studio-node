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

#include "osn-video.hpp"
#include <obs.h>
#include "obs-shared.hpp"

uint32_t obs::Video::GetSkippedFrames()
{
    return video_output_get_skipped_frames(obs_get_video());
}

uint32_t obs::Video::GetTotalFrames()
{
    return video_output_get_total_frames(obs_get_video());
}