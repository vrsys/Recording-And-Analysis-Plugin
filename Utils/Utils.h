// VRSYS plugin of Virtual Reality and Visualization Group (Bauhaus-University Weimar)
//  _    ______  _______  _______
// | |  / / __ \/ ___/\ \/ / ___/
// | | / / /_/ /\__ \  \  /\__ \
// | |/ / _, _/___/ /  / /___/ /
// |___/_/ |_|/____/  /_//____/
//
//  __                            __                       __   __   __    ___ .  . ___
// |__)  /\  |  | |__|  /\  |  | /__`    |  | |\ | | \  / |__  |__) /__` |  |   /\   |
// |__) /~~\ \__/ |  | /~~\ \__/ .__/    \__/ | \| |  \/  |___ |  \ .__/ |  |  /~~\  |
//
//       ___               __
// |  | |__  |  |\/|  /\  |__)
// |/\| |___ |  |  | /~~\ |  \
//
// Copyright (c) 2024 Virtual Reality and Visualization Group
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//-----------------------------------------------------------------
//   Authors:        Anton Lammert
//   Date:           2024
//-----------------------------------------------------------------

#ifndef RECORDINGPLUGIN_UTILS_H
#define RECORDINGPLUGIN_UTILS_H

#include "Recording/Transform/TransformData.h"
#include "Recording/Generic/GenericData.h"
#include "DebugLog.h"
#include "Recording/Sound/SoundData.h"
#include "Recording/MetaInformation.h"

#include "../External/AudioFile/AudioFile.h"

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>


namespace Utils {

    void export_transform_data_to_CSV(std::string const& transform_file_path, std::vector<std::string> const& objects_of_interest = {});

    void export_transform_data_to_CSV(std::string const& transform_file_path, std::string target_object_path);

    void export_generic_data_to_CSV(std::string const& generic_file_path);

    void export_sound_data_to_WAV(std::string const& sound_file_path);

    std::string get_meta_file_path_from_transform(std::string const& transform_file_path);
};


#endif //RECORDINGPLUGIN_UTILS_H
