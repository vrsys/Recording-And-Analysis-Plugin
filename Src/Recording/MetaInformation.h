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

#ifndef RECORDINGPLUGIN_METAINFORMATION_H
#define RECORDINGPLUGIN_METAINFORMATION_H
#include <chrono>
#include <map>
#include <set>
#include <string>
#include "DebugLog.h"
#include <iostream>
#include <fstream>
#include <vector>

/**
 * @class MetaInformation
 * @brief This class stores meta-information of a recording. Information includes: which objects are present, which sound sources are present, which generic data ids are present.
 */
class MetaInformation {

private:
    //std::chrono::time_point<std::chrono::system_clock> start_time;
    //std::chrono::time_point<std::chrono::system_clock> end_time;
    float total_recording_time = -1.0f;
    // offset to the globally defined starting time of the recording in ms
    float global_recording_start_time_offset = 0.0f;
    std::map<std::string, int> object_name_uuid_map;
    std::map<int, std::string> uuid_object_name_map;
    std::map<int, int> old_uuid_new_uuid_map;
    std::map<int, int> new_uuid_old_uuid_map;
    std::map<int, bool> uuid_error_notified;
    std::map<int, std::string> uuid_object_path;
    std::map<int, std::string> uuid_object_components;
    std::map<int, std::string> uuid_object_prefab;
    std::map<std::string, std::string> name_object_components;
    std::map<std::string, std::string> name_object_path;
    std::map<std::string, std::string> name_object_prefab;
    std::map<int, float> uuid_object_path_first_seen;
    std::set<int> recordedgenericIDs;
    std::set<int> recordedTransformIDs;
    std::set<int> recordedSoundIDs;
    std::string file_directory;
    bool recording_finished = false;
    bool writing_finished = false;

public:
    MetaInformation();

    MetaInformation(std::string const& directory);

    ~MetaInformation();

    void recording_stopped();

    bool is_finished();

    float get_recording_duration();

    float get_global_time_offset();

    void update_recording_time(float current_recording_time);

    bool time_contained_in_recording(float current_time);

    void add_generic_id_information(int id);

    void add_transform_id_information(int id);

    void add_sound_id_information(int id);

    void update_name_uuid_map(std::string const& object_name, int object_uuid);

    bool register_recording_start_global_timeoffset(float offset_to_global_time);

    // this is being done as the UUIDs can change during unity sessions meaning that one may not be able to find the
    // needed object transforms by their new UUID. Because of this we try to find the old UUID using the object name.
    int get_old_uuid(std::string const& object_name, int new_uuid);

    int get_old_uuid(std::string const& object_name);

    int get_old_uuid(int new_uuid);

    int get_new_uuid(int old_uuid);

    std::string get_object_name(int old_uuid);

    bool read_meta_information();

    bool write_meta_information();

    bool finished_writing();

    void clear();

    std::string get_recording_gameobjects();

    std::set<int> get_recording_sound_sources();

    std::map<int,std::string> get_recording_gameobjects_map();

    bool register_gameobject_path(int uuid, std::string const& path, float time);

    bool register_gameobject_prefab(int uuid, std::string const& prefab_name, float time);

    bool register_gameobject_components(int uuid, std::string const& components);

    std::string get_gameobject_path(std::string const& name);

    std::string get_gameobject_prefab(std::string const& name);

    std::string get_gameobject_components(std::string const& name);

    std::string get_gameobject_mesh_path(std::string const& name);

    std::set<int> get_sound_ids();

    float get_gameobject_first_seen_time(std::string const& name);

};

#endif //RECORDINGPLUGIN_METAINFORMATION_H
