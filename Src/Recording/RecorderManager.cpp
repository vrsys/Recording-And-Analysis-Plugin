//
// Created by Mortiferum on 30.06.2022.
//

#include "RecorderManager.h"

RecorderManager::RecorderManager() {
    for(int i = 0; i < 10; ++i){
        recorders.emplace_back();
    }
}

bool RecorderManager::record_transform_at_time(int recorder_id, std::string const &object_name,
                                               int object_uuid, const float *transformation_data, float time_stamp,
                                               const int *object_information) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].record_transform(object_name, object_uuid, transformation_data,
                                                       time_stamp, object_information);
    }
    return false;
}

bool RecorderManager::record_sound_at_time(int recorder_id, float *sound_data, int sound_data_length, int sampling_rate,
                                           int start_index, int channel_num, float time_stamp, int sound_origin) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].record_sound(sound_data, sound_data_length, sampling_rate, start_index,
                                                   channel_num, time_stamp, sound_origin);
    }
    return false;
}

bool RecorderManager::record_sound_at_time(int recorder_id, float *sound_data, int sound_data_length, int sampling_rate,
                                           int start_index, int channel_num, int corresponding_gameobject_id, float time_stamp,
                                           int sound_origin) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].record_sound(sound_data, sound_data_length, sampling_rate, start_index,
                                                   channel_num, corresponding_gameobject_id, time_stamp, sound_origin);
    }
    return false;
}


bool RecorderManager::record_generic_at_time(int recorder_id, float time_stamp, int id, int *int_a, float *float_a,
                                               char *char_a) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].record_generic(time_stamp, id, int_a, float_a, char_a);
    }
    return false;
}

bool RecorderManager::get_transform_at_time(int recorder_id, char *object_name, int object_name_length, int object_uuid,
                                            float time_stamp, float *matrix, int *object_information) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_transform(object_name, object_name_length, object_uuid, time_stamp, matrix,
                                                    object_information);
    }
    return false;
}

int RecorderManager::get_sound_at_time(int recorder_id, int sound_origin, float time_stamp, float *sound_data) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_sound(sound_origin, time_stamp, sound_data);
    }
    return -1;
}

int RecorderManager::get_sound_at_time(int recorder_id, int sound_origin, float time_stamp, float *sound_data,
                                       int* corresponding_go_id) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_sound(sound_origin, time_stamp, sound_data, corresponding_go_id);
    }
    return -1;
    return 0;
}

bool RecorderManager::get_generic_at_time(int recorder_id, float time_stamp, int id, int *int_a, float *float_a,
                                            char *char_a) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_generic(time_stamp, id, int_a, float_a, char_a);
    }
    return false;
}

bool RecorderManager::create_recording_file(int recorder_id, std::string const& directory, std::string const& file_name) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].create_recording_file(directory, file_name);
    }
    return false;
}

bool RecorderManager::stop_recording(int recorder_id) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].stop_recording();
    }
    return false;
}

bool RecorderManager::open_existing_recording_file(int recorder_id, std::string const& directory, std::string const& file_name) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].open_existing_recording_file(directory, file_name);
    }
    return false;
}

bool RecorderManager::stop_replay(int recorder_id) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].stop_replay();
    }
    return false;
}

bool RecorderManager::set_transform_recording_buffer_size(int recorder_id, int max_size) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].set_transform_recording_buffer_size(max_size);
    }
    return false;
}

bool RecorderManager::set_sound_recording_buffer_size(int recorder_id, int max_size) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].set_sound_recording_buffer_size(max_size);
    }
    return false;
}

bool RecorderManager::set_replay_buffer_num(int recorder_id, int buffer_num) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].set_replay_buffer_num(buffer_num);
    }
    return false;
}

bool RecorderManager::set_replay_buffer_time_interval(int recorder_id, float time_interval) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].set_replay_buffer_time_interval(time_interval);
    }
    return false;
}

int RecorderManager::next_positions_for_id(int recorder_id, char *object_name, int object_name_length, int object_uuid,
                                           float time_stamp, float *positions) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].next_positions_for_id(object_name, object_name_length, object_uuid, time_stamp, positions);
    }
    return 0;
}

int RecorderManager::get_recording_gameobjects(int recorder_id, char *text, int max_size) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_recording_gameobjects(text, max_size);
    }
    return 0;
}

int RecorderManager::get_recording_sound_sources(int recorder_id, int* ids, int max_size) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_recording_sound_sources(ids, max_size);
    }
    return 0;
}

bool RecorderManager::register_gameobject_path(int recorder_id, int uuid, char *path, int path_length, float time) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].register_gameobject_path(uuid, path, path_length, time);
    }
    return false;
}

bool RecorderManager::register_gameobject_prefab(int recorder_id, int uuid, char *prefab, int prefab_length, float time) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].register_gameobject_prefab(uuid, prefab, prefab_length, time);
    }
    return false;
}

int RecorderManager::get_gameobject_path(int recorder_id, char *text, int max_size, char *name, int name_length) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_gameobject_path(text, max_size, name, name_length);
    }
    return 0;
}

int RecorderManager::get_gameobject_prefab(int recorder_id, char *text, int max_size, char *name, int name_length) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_gameobject_prefab(text, max_size, name, name_length);
    }
    return 0;
}

bool
RecorderManager::register_gameobject_components(int recorder_id, int uuid, char *components, int components_length) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].register_gameobject_components(uuid, components, components_length);
    }
    return false;
}

int RecorderManager::get_gameobject_components(int recorder_id, char *text, int max_size, char *name, int name_length) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_gameobject_components(text, max_size, name, name_length);
    }
    return 0;
}

float RecorderManager::get_gameobject_first_time(int recorder_id, char *name, int name_length) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_gameobject_first_time(name, name_length);
    }
    return -1.0f;
}

float RecorderManager::get_recording_duration(int recorder_id) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_recording_duration();
    }
    return -1.0f;
}

bool RecorderManager::register_recording_start_global_timeoffset(int recorder_id, float offset_to_global_time) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].register_recording_start_global_timeoffset(offset_to_global_time);
    }
    return false;
}

int RecorderManager::get_sampling_rate(int recorder_id, int sound_origin) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_sampling_rate(sound_origin);
    }
    return -1;
}

int RecorderManager::get_original_id(int recorder_id, char *object_name, int object_name_length, int object_id) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_original_id(object_name, object_name_length, object_id);
    }
    return -1;
}

int RecorderManager::get_original_id(int recorder_id, int object_id) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_original_id(object_id);
    }
    return -1;
}

bool RecorderManager::set_debug_mode(int recorder_id, bool debug) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].set_debug_mode(debug);
    }
    return false;
}

int RecorderManager::get_channel_num(int recorder_id, int sound_origin) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_channel_num(sound_origin);
    }
    return -1;
}

int RecorderManager::get_new_id(int recorder_id, int old_object_id) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_new_id(old_object_id);
    }
    return -1;
}

float RecorderManager::get_recording_start_global_timeoffset(int recorder_id) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_recording_start_global_timeoffset();
    }
    return -1;
}

std::map<int, std::string> RecorderManager::get_recording_gameobjects(int recorder_id) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_recording_gameobjects();
    }
    return std::map<int, std::string>();
}

std::string RecorderManager::get_gameobject_components(int recorder_id, std::string name) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_gameobject_components(name);
    }
    return "No information";
}

std::string RecorderManager::get_gameobject_mesh_path(int recorder_id, std::string name) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_gameobject_mesh_path(name);
    }
    return "No information";
}

int RecorderManager::get_sound_ids(int recorder_id, int *ids, int max_id_count) {
    if(recorder_id >= 0 && recorder_id < recorders.size()){
        return recorders[recorder_id].get_sound_ids(ids, max_id_count);
    }
    return 0;
}