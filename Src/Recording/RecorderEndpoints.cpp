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

#include "Config.h"
#include "Recording/RecorderManager.h"

/**
 * @file RecorderEndpoints.cpp
 * @brief This file contains all endpoints that can be used from the outside (Unity, Unreal Engine, ...).
 */

/**
 * @brief Returns the version number of the plugin
 *
 * This function can be used to retrieve information about the current plugin version.
 * This can be crucial because data formats might be different between plugin versions
 * potentially leading to crashes when recordings created with different version should be played back.
 *
 * @param text string into which the version number is written
 * @param max_length max length of the string
 * @return length of the string or -1 if the string would exceed capacity
 */
extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVersionNumber(char* text, int max_length) {
    std::string version_info =  "project name: " + std::string{PROJECT_NAME} + " version: " + std::string{PROJECT_VER};
    Debug::Log(version_info, Color::Black);
    if(version_info.length() <= max_length){
        strcpy(text, version_info.c_str());
        return version_info.length();
    }
    return -1;
}

/**
 * @brief Specify whether a recorder should print debug output during recording/replay.
 *
 * @param recorder_id id of the recorder for which the directory is specified
 * @param debug should debug output be printed?
 * @return true if successful, false otherwise
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetDebugMode(int recorder_id, bool debug) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.set_debug_mode(recorder_id, debug);
}

/**
 * @brief Specify whether maximum number of transform elements stored during recording before writing to file
 *
 * @param recorder_id id of the recorder for which the directory is specified
 * @param max_size maximum number of transform elements stored in memory before written to file
 * @return true if successful, false otherwise
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetRecordingMaxBufferSize(int recorder_id, int max_size) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.set_transform_recording_buffer_size(recorder_id, max_size);
}

/**
 * @brief Specify whether maximum number of sound elements stored during recording before writing to file
 *
 * @param recorder_id id of the recorder for which the directory is specified
 * @param max_size maximum number of sound elements stored in memory before written to file
 * @return true if successful, false otherwise
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetSoundRecordingMaxBufferSize(int recorder_id, int max_size) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.set_sound_recording_buffer_size(recorder_id, max_size);
}

/**
 * @brief Specify whether number of buffers used per ring buffer during playback.
 *
 * @param recorder_id id of the recorder for which the directory is specified
 * @param buffer_number number of buffers in a ring buffer
 * @return true if successful, false otherwise
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetReplayBufferNumber(int recorder_id, int buffer_number) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.set_replay_buffer_num(recorder_id, buffer_number);
}

/**
 * @brief Specify how many seconds of data are stored in each buffer of a ring buffer
 *
 * @param recorder_id id of the recorder for which the directory is specified
 * @param time_interval time in seconds for which data is stored in each buffer
 * @return true if successful, false otherwise
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetReplayBufferStoredTimeInterval(int recorder_id, float time_interval) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.set_replay_buffer_time_interval(recorder_id, time_interval);
}

/**
 * @brief Endpoint to record 3d transform object information
 *
 * @param recorder_id id of the recorder which is used to record the data
 * @param object_name the scene graph hierarchy name of the object for which data is recorded e.g. "Scene/User/Head"
 * @param object_name_length length of the object name string
 * @param object_id the id of the object (for unity object UUIDs can be used)
 * @param transformation_data array containing all transformation data (local & global position, rotation, scaling)
 * @param time_stamp point in time for which data is recorded
 * @param object_information array containing additional object informations (current state (active/deactivated), id of current parent)
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RecordObjectAtTimestamp(int recorder_id, char* object_name, int object_name_length, int object_uuid, float* transformation_data, float time_stamp, int* object_information) {
    RecorderManager& manager = RecorderManager::getInstance();
    std::string object_name_st;
    for (int i = 0; i < object_name_length; i++)
        object_name_st += object_name[i];
    return manager.record_transform_at_time(recorder_id, object_name_st, object_uuid, transformation_data, time_stamp, object_information);
}

/**
 * @brief Endpoint to record generic information
 *
 * @param recorder_id id of the recorder which is used to record the data
 * @param time_stamp point in time for which data is recorded
 * @param id the id of the instance for which generic data is recorded (used later for retrieval). Note, this id can be identical to existing sound or transform ids
 * @param int_a array containing integer data which will be recorded (for max size see definition of genericDTO)
 * @param float_a array containing float data which will be recorded (for max size see definition of genericDTO)
 * @param char_a array containing char data which will be recorded (for max size see definition of genericDTO)
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RecordGenericAtTimestamp(int recorder_id, float time_stamp, int id, int* int_a, float* float_a, char* char_a) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.record_generic_at_time(recorder_id, time_stamp, id, int_a, float_a, char_a);
}

/**
 * @brief Endpoint used for recording sound data
 *
 * @param recorder_id id of the recorder which is used to record data
 * @param sound_data array containing float data representing sound. Values usually between [-1,1]
 * @param sound_data_length size of the sound_data array
 * @param sampling_rate sampling rate of the sound source
 * @param start_index the index from which on in the sound_data buffer new data is present which will be recorded. 0 if only new data is present.
 * @param channel_num number of channels of the sound contained in the array. Note, that data is assumed to be in interleaved fashion e.g. for 2 channel C1S0,C2S0,C1S1,C2S1, ...
 * @param time_stamp point in time for which data is recorded
 * @param sound_id id of the sound source
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RecordSoundDataAtTimestamp(int recorder_id, float* sound_data, int sound_data_length, int sampling_rate, int start_index, int channel_num, float time_stamp, int sound_id) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.record_sound_at_time(recorder_id, sound_data, sound_data_length, sampling_rate, start_index, channel_num,time_stamp, sound_id);
}

/**
 * @brief Endpoint used for recording sound data
 *
 * @param recorder_id id of the recorder which is used to record data
 * @param sound_data array containing float data representing sound. Values usually between [-1,1]
 * @param sound_data_length size of the sound_data array
 * @param sampling_rate sampling rate of the sound source
 * @param start_index the index from which on in the sound_data buffer new data is present which will be recorded. 0 if only new data is present.
 * @param channel_num number of channels of the sound contained in the array. Note, that data is assumed to be in interleaved fashion e.g. for 2 channel C1S0,C2S0,C1S1,C2S1, ...
 * @param time_stamp point in time for which data is recorded
 * @param sound_id id of the sound source
 * @param corresponding_gameobject_id id of the object which is the origin of the sound source (this id should be used to record transform data of the object)
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RecordSoundDataWithGOInfoAtTimestamp(int recorder_id, float* sound_data, int sound_data_length, int sampling_rate, int start_index, int channel_num, int corresponding_gameobject_id, float time_stamp, int sound_id) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.record_sound_at_time(recorder_id, sound_data, sound_data_length, sampling_rate, start_index, channel_num, corresponding_gameobject_id, time_stamp, sound_id);
}

/**
 * @brief Endpoint to retrieve recorded transform data using the recording file which was specified beforehand
 *
 * @param recorder_id id of the recorder which is used to replay data
 * @param object_name the scene graph hierarchy name of the object for which data is retrieved e.g. "Scene/User/Head"
 * @param object_name_length length of the object name string
 * @param object_id the id of the object (for unity object UUIDs can be used)
 * @param transformation_data array into which all transformation data will be writted(local & global position, rotation, scaling)
 * @param time_stamp point in time for which data is retrieved
 * @param object_information array into which additional object information (current state (active/deactivated), id of current parent) will be written
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetTransformAndInformationAtTime(int recorder_id, char* object_name, int object_name_length, int object_id, float time_stamp, float* transformation_data, int* object_information) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_transform_at_time(recorder_id, object_name, object_name_length, object_id, time_stamp, transformation_data, object_information);
}

/**
 * @brief Endpoint to retrieve generic data using the recording file which was specified beforehand
 *
 * @param recorder_id id of the recorder which is used to replay data
 * @param time_stamp point in time for which data is retrieved
 * @param int_a int array into which retrieved data will be written
 * @param float_a float array into which retrieved data will be written
 * @param char_a char array into which retrieved data will be written
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetGenericAtTime(int recorder_id, float time_stamp, int id, int* int_a, float* float_a, char* char_a) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_generic_at_time(recorder_id, time_stamp, id, int_a, float_a, char_a);
}

/**
 * @brief Endpoint to retrieve sound data using the recording file which was specified beforehand
 *
 * @param recorder_id id of the recorder which is used to replay data
 * @param sound_id id of the sound source for which data is retrieved
 * @param time_stamp time for which sound data is retrieved
 * @param sound_data float array into which retrieved sound data is written
 *
 * @return number of samples written into the sound_data array
 */
extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetSoundChunkForTime(int recorder_id, int sound_id, float time_stamp, float* sound_data) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_sound_at_time(recorder_id, sound_id, time_stamp, sound_data);
}

/**
 * @brief Endpoint to retrieve sound data using the recording file which was specified beforehand
 *
 * @param recorder_id id of the recorder which is used to replay data
 * @param sound_id id of the sound source for which data is retrieved
 * @param time_stamp time for which sound data is retrieved
 * @param sound_data float array into which retrieved sound data is written
 * @param corresponding_go_id int array into which the id of the object, which was the origin of the sound during recording, is written
 *
 * @return number of samples written into the sound_data array
 */
extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetSoundChunkAndGOInformationForTime(int recorder_id, int sound_id, float time_stamp, float* sound_data, int* corresponding_go_id) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_sound_at_time(recorder_id, sound_id, time_stamp, sound_data, corresponding_go_id);
}

/**
 * @brief Endpoint which creates a new recording file and set the recorder into the state for recording
 *
 * @param recorder_id id of the recorder which will start a new recording
 * @param directory string containing the path at which the new recording file will be created
 * @param directory_length length of the directory string
 * @param recording_name string containing the name of the recording file to be created
 * @param recording_name_length length of the recording_name string
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API CreateNewRecordingFile(int recorder_id, char* directory, int directory_length, char* recording_name, int recording_name_length) {
    RecorderManager& manager = RecorderManager::getInstance();
    std::string file_name;
    for (int i = 0; i < recording_name_length; i++)
        file_name += recording_name[i];

    std::string dir;
    for (int i = 0; i < directory_length; i++)
        dir += directory[i];
    return manager.create_recording_file(recorder_id, dir, file_name);
}

/**
 * @brief Endpoint which stops a running recording and writes data, not yet written to file, to file
 *
 * @param recorder_id id of the recorder which should stop the recording process
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API StopRecording(int recorder_id) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.stop_recording(recorder_id);
}

/**
 * @brief Endpoint which tries to open an existing recording file, initiates data loading and set recorder into replay state
 *
 * @param recorder_id id of the recorder which should start playback
 * @param directory string containing the path at which the new recording file will be created
 * @param directory_length length of the directory string
 * @param recording_name string containing the name of the recording file to be created
 * @param recording_name_length length of the recording_name string
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API OpenExistingRecordingFile(int recorder_id, char* directory, int directory_length, char* recording_name, int recording_name_length) {
    RecorderManager& manager = RecorderManager::getInstance();
    std::string file_name;
    for (int i = 0; i < recording_name_length; i++)
        file_name += recording_name[i];

    std::string dir;
    for (int i = 0; i < directory_length; i++)
        dir += directory[i];
    return manager.open_existing_recording_file(recorder_id, dir, file_name);
}

/**
 * @brief Endpoint which stops a running replay
 *
 * @param recorder_id id of the recorder which should stop playback
 */
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API StopReplay(int recorder_id) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.stop_replay(recorder_id);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API NextPositionsForUUID(int recorder_id, char* object_name, int object_name_length, int object_uuid, float time_stamp, float* positions) {
    std::cout << "next_positions_for_id called from external code. UUID: " << object_uuid << "\n";
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.next_positions_for_id(recorder_id, object_name, object_name_length, object_uuid, time_stamp, positions);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRecordingGameObjects(int recorder_id, char* text, int max_size) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_recording_gameobjects(recorder_id, text, max_size);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRecordingSoundSources(int recorder_id, int* ids, int max_size) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_recording_sound_sources(recorder_id, ids, max_size);
}

extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRecordingDuration(int recorder_id) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_recording_duration(recorder_id);
}

extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRecordingStartGlobalTimeOffset(int recorder_id) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_recording_start_global_timeoffset(recorder_id);
}

extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RegisterObjectPath(int recorder_id, int uuid, char* path, int path_length, float time) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.register_gameobject_path(recorder_id, uuid, path, path_length, time);
}

extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RegisterObjectPrefab(int recorder_id, int uuid, char* prefab, int prefab_length, float time) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.register_gameobject_prefab(recorder_id, uuid, prefab, prefab_length, time);
}

extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RegisterObjectComponents(int recorder_id, int uuid, char* components, int components_length) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.register_gameobject_components(recorder_id, uuid, components, components_length);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetGameObjectPath(int recorder_id, char* text, int max_size, char* name, int name_length) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_gameobject_path(recorder_id, text, max_size, name, name_length);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetGameObjectPrefab(int recorder_id, char* text, int max_size, char* name, int name_length) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_gameobject_prefab(recorder_id, text, max_size, name, name_length);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetGameObjectComponents(int recorder_id, char* text, int max_size, char* name, int name_length) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_gameobject_components(recorder_id, text, max_size, name, name_length);
}

extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetGameObjectFirstSeenTime(int recorder_id, char* name, int name_length){
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_gameobject_first_time(recorder_id, name, name_length);
}

extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RegisterRecordingStartGlobalTimeOffset(int recorder_id, float offset_to_global_time){
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.register_recording_start_global_timeoffset(recorder_id, offset_to_global_time);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetSamplingRate(int recorder_id, int sound_origin) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_sampling_rate(recorder_id, sound_origin);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetChannelNum(int recorder_id, int sound_origin) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_channel_num(recorder_id, sound_origin);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetSoundIDs(int recorder_id, int* ids, int max_id_count) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_sound_ids(recorder_id, ids, max_id_count);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetOriginalID(int recorder_id, char* object_name, int object_name_length, int object_uuid) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_original_id(recorder_id, object_name, object_name_length, object_uuid);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetOriginalID2(int recorder_id, int new_object_uuid) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_original_id(recorder_id, new_object_uuid);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetNewID(int recorder_id, int old_object_uuid) {
    RecorderManager& manager = RecorderManager::getInstance();
    return manager.get_new_id(recorder_id, old_object_uuid);
}