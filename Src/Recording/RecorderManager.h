//
// Created by Mortiferum on 30.06.2022.
//

#ifndef RECORDINGPLUGIN_RECORDERMANAGER_H
#define RECORDINGPLUGIN_RECORDERMANAGER_H

#include "Recorder.h"
#include "../../External/thread-pool/include/BS_thread_pool.hpp"

/**
 * @class RecorderManager
 * @brief This class controls multiple recorder, which can playback & record different things at the same time. For this multi-threading is used.
 */
class RecorderManager {
public:
    static RecorderManager &getInstance() {
        static RecorderManager instance;
        return instance;
    }

    static BS::thread_pool &getThreadPool() {
        static BS::thread_pool thread_pool(std::thread::hardware_concurrency() - 1);
        return thread_pool;
    }

    RecorderManager();

    RecorderManager(RecorderManager const &) = delete;

    void operator=(RecorderManager const &) = delete;

    bool record_transform_at_time(int recorder_id, std::string const& object_name, int object_uuid,
                                  float const *local_matrix, float time_stamp, int const *object_information);

    bool
    record_sound_at_time(int recorder_id, float *sound_data, int sound_data_length, int sampling_rate, int start_index,
                         int channel_num, float time_stamp, int sound_origin);

    bool
    record_sound_at_time(int recorder_id, float *sound_data, int sound_data_length, int sampling_rate, int start_index,
                         int channel_num, int corresponding_gameobject_id, float time_stamp, int sound_origin);

    bool record_generic_at_time(int recorder_id, float time_stamp, int id, int *int_a, float *float_a, char *char_a);

    bool
    get_transform_at_time(int recorder_id, char *object_name, int object_name_length, int object_uuid, float time_stamp,
                          float *matrix, int *object_information);

    int get_sound_at_time(int recorder_id, int sound_origin, float time_stamp, float *sound_data);

    int
    get_sound_at_time(int recorder_id, int sound_origin, float time_stamp, float *sound_data, int* corresponding_go_id);

    int get_sampling_rate(int recorder_id, int sound_origin);

    int get_channel_num(int recorder_id, int sound_origin);

    bool get_generic_at_time(int recorder_id, float time_stamp, int id, int *int_a, float *float_a, char *char_a);

    bool create_recording_file(int recorder_id, std::string const& directory, std::string const& file_name);

    bool stop_recording(int recorder_id);

    bool open_existing_recording_file(int recorder_id, std::string const& directory, std::string const& file_name);

    bool stop_replay(int recorder_id);

    bool set_transform_recording_buffer_size(int recorder_id, int max_size);

    bool set_sound_recording_buffer_size(int recorder_id, int max_size);

    bool set_replay_buffer_num(int recorder_id, int buffer_num);

    bool set_replay_buffer_time_interval(int recorder_id, float time_interval);

    int
    next_positions_for_id(int recorder_id, char *object_name, int object_name_length, int object_uuid, float time_stamp,
                          float *positions);

    int get_original_id(int recorder_id, char *object_name, int object_name_length, int object_id);

    int get_original_id(int recorder_id, int object_id);

    int get_new_id(int recorder_id, int old_object_id);

    int get_recording_gameobjects(int recorder_id, char *text, int max_size);

    int get_recording_sound_sources(int recorder_id, int* ids, int max_size);

    std::map<int, std::string> get_recording_gameobjects(int recorder_id);

    bool register_gameobject_path(int recorder_id, int uuid, char *path, int path_length, float time);

    bool register_gameobject_prefab(int recorder_id, int uuid, char *prefab, int prefab_length, float time);

    int get_gameobject_path(int recorder_id, char *text, int max_size, char *name, int name_length);

    int get_gameobject_prefab(int recorder_id, char *text, int max_size, char *name, int name_length);

    bool register_gameobject_components(int recorder_id, int uuid, char *components, int components_length);

    int get_gameobject_components(int recorder_id, char *text, int max_size, char *name, int name_length);

    std::string get_gameobject_components(int recorder_id, std::string name);

    std::string get_gameobject_mesh_path(int recorder_id, std::string name);

    float get_gameobject_first_time(int recorder_id, char *name, int name_length);

    int get_sound_ids(int recorder_id, int* ids, int max_id_count);

    float get_recording_duration(int recorder_id);

    float get_recording_start_global_timeoffset(int recorder_id);

    bool register_recording_start_global_timeoffset(int recorder_id, float offset_to_global_time);

    bool set_debug_mode(int recorder_id, bool debug);

private:
    bool debug = false;

    std::vector<Recorder> recorders;
};


#endif //RECORDINGPLUGIN_RECORDERMANAGER_H
