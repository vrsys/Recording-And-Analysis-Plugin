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


#include <algorithm>
#include "MetaInformation.h"
#include "Transform/TransformData.h"
#include "Sound/SoundData.h"
#include "Generic/GenericData.h"

MetaInformation::MetaInformation() {
    writing_finished = false;
}

MetaInformation::MetaInformation(std::string const& directory) : file_directory(directory) {
    //start_time = std::chrono::system_clock::now();
    writing_finished = false;
    bool read = read_meta_information();
    if(!read){
        Debug::Log("Error occurred trying to read meta information!", Color::Red);
    }
}

void MetaInformation::recording_stopped() {
    //end_time = std::chrono::system_clock::now();
    recording_finished = true;
}

bool MetaInformation::is_finished() {
    return recording_finished;
}

float MetaInformation::get_recording_duration() {
    return total_recording_time;
}

void MetaInformation::update_recording_time(float current_recording_time) {
    if (current_recording_time > total_recording_time)
        total_recording_time = current_recording_time;
}

bool MetaInformation::time_contained_in_recording(float current_time) {
    return current_time <= total_recording_time && current_time >= 0.0f;
}

void MetaInformation::update_name_uuid_map(std::string const& object_name, int object_uuid) {
    if (!object_name_uuid_map.count(object_name) && !uuid_object_name_map.count(object_uuid)) {
        object_name_uuid_map[object_name] = object_uuid;
        uuid_object_name_map[object_uuid] = object_name;
    } else {
        int previous_uuid = object_name_uuid_map[object_name];
        if (previous_uuid != object_uuid && uuid_error_notified.count(previous_uuid) == 0) {
            Debug::Log("Error! Identical object name but different UUID. Duplication of object names? Object name: " + object_name + ", previous UUID: " + std::to_string(previous_uuid) + ", current UUID: " + std::to_string(object_uuid), Color::Red);
            uuid_error_notified[previous_uuid] = true;
        }
    }
}

bool MetaInformation::register_recording_start_global_timeoffset(float offset_to_global_time) {
    global_recording_start_time_offset = offset_to_global_time;
    return true;
}

// this is being done as the UUIDs can change during unity sessions meaning that one may not be able to find the
// needed object transforms by their new UUID. Because of this we try to find the old UUID using the object name.
int MetaInformation::get_old_uuid(std::string const& object_name, int new_uuid) {
    int old_uuid = -1;
    if (!object_name_uuid_map.count(object_name)) {
        return -1;
    } else {
        old_uuid = object_name_uuid_map[object_name];
    }

    if (!old_uuid_new_uuid_map.count(old_uuid)) {
        old_uuid_new_uuid_map[old_uuid] = new_uuid;
    }

    if (!new_uuid_old_uuid_map.count(new_uuid)) {
        new_uuid_old_uuid_map[new_uuid] = old_uuid;
    }
    return old_uuid;
}

int MetaInformation::get_old_uuid(std::string const& object_name) {
    int old_uuid = -1;
    if (!object_name_uuid_map.count(object_name)) {
        return -1;
    } else {
        old_uuid = object_name_uuid_map[object_name];
    }
    return old_uuid;
}

int MetaInformation::get_old_uuid(int new_uuid) {
    if (new_uuid_old_uuid_map.count(new_uuid)) {
        return new_uuid_old_uuid_map[new_uuid];
    } else {
        return -1;
    }
}

int MetaInformation::get_new_uuid(int old_uuid) {
    if (old_uuid_new_uuid_map.count(old_uuid)) {
        return old_uuid_new_uuid_map[old_uuid];
    } else {
        return -1;
    }
}

bool MetaInformation::read_meta_information() {
    try {
        int map_size = -1;

        Debug::Log("Trying to read metainformation at path:" + file_directory);
        std::ifstream record_file(file_directory, std::ios::in);
        //record_file.read((char *) &start_time, sizeof(std::chrono::time_point<std::chrono::system_clock>));
        //record_file.read((char *) &end_time, sizeof(std::chrono::time_point<std::chrono::system_clock>));

        if(record_file.is_open()) {
            record_file >> total_recording_time >> global_recording_start_time_offset >> map_size;

            Debug::Log("Total recording time: " + std::to_string(total_recording_time) + ", name-UUID map size: " + std::to_string(map_size));

            std::string line;
            for (int i = 0; i < map_size; i++) {
                std::getline(record_file, line);

                //std::cout << line << "\n";

                if (line.size() < 3) {
                    i--;
                    continue;
                }
                std::string object_name;
                std::string UUID;
                std::string object_path;
                std::string object_components;
                std::string first_seen_time;
                std::string object_prefab;
                int index = 0;

                for (int j = 0; j < line.size(); j++) {
                    if (line[j] == ';') {
                        index++;
                    } else {
                        if (index == 0) {
                            object_name += line[j];
                        } else if (index == 1) {
                            UUID += line[j];
                        } else if (index == 2) {
                            object_path += line[j];
                        } else if (index == 3) {
                            first_seen_time += line[j];
                        } else if (index == 4) {
                            object_components += line[j];
                        } else if (index == 5) {
                            object_prefab += line[j];
                        }
                    }
                }

                int uuid = std::stoi(UUID);

                object_prefab.erase(std::remove_if(object_prefab.begin(), object_prefab.end(), []( auto const& c ) -> bool {
                    return !(std::isalnum(c) || c == ' ' || c == '\\' || c == '/' || c == '.' || c == '-' || c == '_') || c == ';';
                }), object_prefab.end());

                //std::cout << "uuid : " << uuid << ", object name: " << object_name << "\n";

                if (!object_name_uuid_map.count(object_name)) {
                    object_name_uuid_map[object_name] = uuid;
                } else {
                    Debug::Log("Error! Object name already contained!", Color::Red);
                }

                if (!uuid_object_path.count(uuid)) {
                    uuid_object_path[uuid] = object_path;
                } else {
                    Debug::Log("Error! Object path already contained!", Color::Red);
                }

                if (!name_object_path.count(object_name)) {
                    name_object_path[object_name] = object_path;
                } else {
                    Debug::Log("Error! Object path already contained!", Color::Red);
                }

                if (!uuid_object_path_first_seen.count(uuid)) {
                    uuid_object_path_first_seen[uuid] = std::stof(first_seen_time);
                } else {
                    Debug::Log("Error! Object path already contained!", Color::Red);
                }

                if (!uuid_object_components.count(uuid)) {
                    uuid_object_components[uuid] = object_components;
                } else {
                    Debug::Log("Error! Object components already contained!", Color::Red);
                }

                if(!name_object_components.count(object_name)){
                    name_object_components[object_name] = object_components;
                } else {
                    Debug::Log("Error! Object components already contained!", Color::Red);
                }

                if(!object_prefab.empty() && std::any_of(object_prefab.begin(), object_prefab.end(), ::isalnum)){
                    if(object_prefab.back() == ' '){
                        object_prefab.pop_back();
                    }
                    if (!uuid_object_prefab.count(uuid)) {
                        uuid_object_prefab[uuid] = object_prefab;
                    } else {
                        Debug::Log("Error! Object prefab already contained!", Color::Red);
                    }

                    if (!name_object_prefab.count(object_name)) {
                        name_object_prefab[object_name] = object_prefab;
                    } else {
                        Debug::Log("Error! Object prefab already contained!", Color::Red);
                    }
                }

                if (!uuid_object_name_map.count(uuid)) {
                    uuid_object_name_map[uuid] = object_name;
                } else {
                    Debug::Log("Error! UUID already contained!", Color::Red);
                }
            }

            while(std::getline(record_file, line)) {
                if (line.find("Sound IDs") != std::string::npos) {
                    std::getline(record_file, line);
                    std::stringstream test{line};
                    std::string segment;
                    std::vector<std::string> seglist;

                    while (std::getline(test, segment, ',')) {
                        if(!segment.empty() && std::any_of(segment.begin(), segment.end(), ::isdigit))
                            recordedSoundIDs.insert(std::stoi(segment));
                    }
                }

                if (line.find("Generic IDs") != std::string::npos) {
                    std::getline(record_file, line);
                    std::stringstream test{line};
                    std::string segment;
                    std::vector<std::string> seglist;

                    while (std::getline(test, segment, ',')) {
                        if(!segment.empty() && std::any_of(segment.begin(), segment.end(), ::isdigit))
                            recordedgenericIDs.insert(std::stoi(segment));
                    }
                }
            }

            return true;
        } else {
            Debug::Log("Error! Could not open the recording meta file!", Color::Red);
            return false;
        }
    } catch (std::exception &e) {
        std::string s = e.what();
        Debug::Log("Exception: " + s, Color::Red);
    }
    return false;
}

bool MetaInformation::write_meta_information() {
    try {
        Debug::Log("Writing meta information to file: " + file_directory);
        std::ofstream meta_information_file{file_directory, std::ios_base::out}; // | std::ios::binary};
        //meta_information_file.write((char *) &start_time, sizeof(std::chrono::time_point<std::chrono::system_clock>));
        //meta_information_file.write((char *) &end_time, sizeof(std::chrono::time_point<std::chrono::system_clock>));

        meta_information_file << "\n" << total_recording_time << " " << global_recording_start_time_offset << " " << object_name_uuid_map.size() << "\n";

        for (auto &i : object_name_uuid_map) {
            std::string object_name = i.first;
            int UUID = i.second;
            std::string path;
            std::string components;
            std::string prefab_name;
            float first_seen_time = -1;

            if (uuid_object_path.count(UUID) == 0) {
                path = " ";
            } else {
                path = uuid_object_path[UUID];
            }

            if (uuid_object_components.count(UUID) == 0) {
                components = " ";
            } else {
                components = uuid_object_components[UUID];
            }

            if (uuid_object_path_first_seen.count(UUID) != 0) {
                first_seen_time = uuid_object_path_first_seen[UUID];
            }

            if (uuid_object_prefab.count(UUID) == 0) {
                prefab_name = " ";
            } else {
                prefab_name = uuid_object_prefab[UUID];
            }

            meta_information_file << object_name << ";" << UUID << ";" << path << ";" << std::to_string(first_seen_time)
                                  << ";" << components << ";" << prefab_name << "\n";
        }

        meta_information_file << "\n---------------------------Sound IDs------------------------------\n";
        for(auto id : recordedSoundIDs){
            meta_information_file << id << ",";
        }
        meta_information_file << "\n---------------------------Generic IDs------------------------------\n";
        for(auto id : recordedgenericIDs){
            meta_information_file << id << ",";
        }
        meta_information_file << "\n---------------------------Transform IDs------------------------------\n";
        for(auto id : recordedTransformIDs){
            meta_information_file << id << ",";
        }

        meta_information_file << "\n---------------------------DTO Layouts------------------------------\n";
        meta_information_file << TransformDTO::dto_layout() << "\n";
        meta_information_file << SoundDTO::dto_layout() << "\n";
        meta_information_file << GenericDTO::dto_layout() << "\n";

        meta_information_file.close();
        writing_finished = true;
        Debug::Log("Meta information written to file");
        return true;
    } catch (std::exception &e) {
        std::cout << e.what();
    }
    return false;
}

bool MetaInformation::finished_writing() {
    return writing_finished;
}

void MetaInformation::clear() {
    total_recording_time = -1.0f;
    object_name_uuid_map.clear();
    uuid_object_name_map.clear();
    old_uuid_new_uuid_map.clear();
    new_uuid_old_uuid_map.clear();
    uuid_error_notified.clear();
    uuid_object_path.clear();
    uuid_object_components.clear();
    uuid_object_prefab.clear();
    name_object_components.clear();
    name_object_path.clear();
    name_object_prefab.clear();
    uuid_object_path_first_seen.clear();
    recordedgenericIDs.clear();
    recordedTransformIDs.clear();
    recordedSoundIDs.clear();
    recording_finished = false;
}

std::string MetaInformation::get_recording_gameobjects() {
    std::string names;
    for (const auto &name_uuid : object_name_uuid_map) {
        names += name_uuid.first + ";";
    }
    return names;
}

std::set<int> MetaInformation::get_recording_sound_sources() {
    return recordedSoundIDs;
}

bool MetaInformation::register_gameobject_path(int uuid, std::string const& path, float time) {
    if (uuid_object_path.count(uuid) == 0) {
        uuid_object_path[uuid] = path;
        uuid_object_path_first_seen[uuid] = time;
    }
    return true;
}

bool MetaInformation::register_gameobject_prefab(int uuid, const std::string &prefab_name, float time) {
    if (uuid_object_prefab.count(uuid) == 0) {
        uuid_object_prefab[uuid] = prefab_name;
    }
    return true;
}

std::string MetaInformation::get_gameobject_path(std::string const& name) {
    int oldUUID = get_old_uuid(name);
    Debug::Log("Trying to retrieve prefab path for object: " + name + " with id: " + std::to_string(oldUUID));
    if (uuid_object_path.count(oldUUID) != 0) {
        return uuid_object_path[oldUUID];
    } else {
        return "";
    }
}

bool MetaInformation::register_gameobject_components(int uuid, std::string const& components) {
    if (uuid_object_components.count(uuid) == 0) {
        uuid_object_components[uuid] = components;
    }
    return true;
}

std::string MetaInformation::get_gameobject_components(std::string const& name) {
    if (name_object_components.count(name) != 0) {
        return name_object_components[name];
    } else {
        return "";
    }
}

float MetaInformation::get_gameobject_first_seen_time(std::string const& name) {
    int oldUUID = get_old_uuid(name);
    //Debug::Log("Trying to retrieve first seen time for object: " + name + " with id: " + std::to_string(oldUUID));
    if (uuid_object_path_first_seen.count(oldUUID) != 0) {
        return uuid_object_path_first_seen[oldUUID];
    } else {
        return -1.0f;
    }
}

std::string MetaInformation::get_object_name(int old_uuid){
    if(uuid_object_name_map.count(old_uuid)){
        return uuid_object_name_map[old_uuid];
    }
    return "";
}

float MetaInformation::get_global_time_offset() {
    return global_recording_start_time_offset;
}

MetaInformation::~MetaInformation() {
    Debug::Log("Destroying MetaInformation");
}

void MetaInformation::add_generic_id_information(int id) {
    recordedgenericIDs.insert(id);
}

void MetaInformation::add_transform_id_information(int id) {
    recordedTransformIDs.insert(id);
}

void MetaInformation::add_sound_id_information(int id) {
    recordedSoundIDs.insert(id);
}

std::map<int, std::string> MetaInformation::get_recording_gameobjects_map() {
    return uuid_object_name_map;
}

std::string MetaInformation::get_gameobject_mesh_path(const std::string &name) {
    if (name_object_path.count(name) != 0) {
        return name_object_path[name];
    } else {
        return "";
    }
}

std::string MetaInformation::get_gameobject_prefab(const std::string &name) {
    if (name_object_prefab.count(name) != 0) {
        return name_object_prefab[name];
    } else {
        return "";
    }
}

std::set<int> MetaInformation::get_sound_ids() {
    return recordedSoundIDs;
}



