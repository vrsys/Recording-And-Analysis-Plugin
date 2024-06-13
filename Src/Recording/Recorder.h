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

#ifndef __RECORDINGPLUGIN_RECORDER_H
#define __RECORDINGPLUGIN_RECORDER_H

#include <Recording/Transform/TransformRingBuffer.h>
#include <Recording/Sound/SoundRingBuffer.h>
#include <Recording/Generic/GenericRingBuffer.h>
#include "DebugLog.h"
#include "Config.h"
#include "Recording.h"

#include "../../External/AudioFile/AudioFile.h"
#include "zconf.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>
#include <chrono>
#include <map>
#include <set>
#include <thread>
#include <vector>
#include <atomic>
#include <deque>
#include <memory>
#include <future>

/**
 * @class Recorder
 * @brief This class represents a single recorder which can either playback or record to/from a file.
 *
 * The recorder records transformation, audio and generic data individually and write recorded data to file.
 * For playback the individual files are read and data is returned.
 */
class Recorder {

public:
    Recorder(){

    }

    //Recorder(const Recorder&) = default;

    Recorder& operator=(const Recorder&) = delete;

    Recorder(Recorder&& obj);

    Recorder& operator=(Recorder && obj);

    ~Recorder();

    /**
     * @brief Records transformation data and object information for a certain object and a certain point in time.
     *
     * In this method data is moved to a DTO which is then added to a recording buffer.
     * As soon as the buffer extends a maximum size the buffer data is moved to a writing buffer.
     * Then a thread task for writing the data from the writing buffer to file is created.
     *
     * @param object_name The name of the object in the application
     * @param id The id of the object in the application (should be unique)
     * @param local_matrix A float matrix containing local and global position, orientation and scaling information
     * @param time_stamp  The time stamp for which the data is recorded
     * @param object_information Information about whether the object is currently active in the application and about the id of the current parent in the applicaiton
     * @return true if successful, false otherwise
     */
    bool record_transform(std::string const& object_name, int id, float const * local_matrix, float time_stamp, int const * object_information);

    /**
     * @brief Records audio buffer data for a certain sound source and a certain point in time
     *
     * In this method data is moved to a DTO which is then added to a recording buffer.
     * As soon as the buffer extends a maximum size the buffer data is moved to a writing buffer.
     * Then a thread task for writing the data from the writing buffer to file is created.
     *
     * @param sound_data The buffer containing the audio data
     * @param sound_data_length The size of the audio buffer (should be smaller than the size of the audio buffer of a SoundDTO
     * @param sampling_rate The sampling rate of the sound source
     * @param start_index Potential offset index from which on new data is present (if 0 all data is new, if n>0 then the first n samples are not recorded)
     * @param channel_num The number of channels of the audio source (Note that unity records audio in an interleaved manner e.g. for 2 channel audio L0R0L1R1...)
     * @param time_stamp The timestamp corresponding with the start time for the first sound sample
     * @param sound_id The id referencing the sound source
     * @return true if successful, false otherwise
     */
    bool record_sound(float* sound_data, int sound_data_length, int sampling_rate, int start_index, int channel_num, float time_stamp, int sound_id);

    /**
     * @brief Records audio buffer data for a certain sound source and a certain point in time
     *
     * In this method data is moved to a DTO which is then added to a recording buffer.
     * As soon as the buffer extends a maximum size the buffer data is moved to a writing buffer.
     * Then a thread task for writing the data from the writing buffer to file is created.
     *
     * @param sound_data The buffer containing the audio data
     * @param sound_data_length The size of the audio buffer (should be smaller than the size of the audio buffer of a SoundDTO
     * @param sampling_rate The sampling rate of the sound source
     * @param start_index  Potential offset index from which on new data is present (if 0 all data is new, if n>0 then the first n samples are not recorded)
     * @param channel_num The number of channels of the audio source (Note that unity records audio in an interleaved manner e.g. for 2 channel audio L0R0L1R1...)
     * @param corresponding_gameobject_id The id of the corresponding gameobject. This information is retrieved during playback to attach (and move) the audio source to the correct object
     * @param time_stamp The timestamp corresponding with the start time for the first sound sample
     * @param sound_id The id referencing the sound source
     * @return true if successful, false otherwise
     */
    bool record_sound(float* sound_data, int sound_data_length, int sampling_rate, int start_index, int channel_num, int corresponding_gameobject_id, float time_stamp, int sound_id);

    /**
     * @brief Records generic buffer data for a certain id and a certain point in time
     *
     * In this method data is moved to a DTO which is then added to a recording buffer.
     * As soon as the buffer extends a maximum size the buffer data is moved to a writing buffer.
     * Then a thread task for writing the data from the writing buffer to file is created.
     *
     * @param time_stamp The point in time for which data is recorded
     * @param id The id for which data is recorded (should be unique for different types and instances of data sources)
     * @param int_a A buffer of ints that can contain semantic data (e.g. the current color of a object, the state of a process, ...)
     * @param float_a A buffer of floats that can contain semantic data (e.g. the current distance between users, the current network latency, ...)
     * @param char_a A buffer of characters that can contain semantic data (e.g. the name of an event, ...)
     * @return true if successful, false otherwise
     */
    bool record_generic(float time_stamp, int id, int* int_a, float* float_a, char* char_a);

    /**
     * @brief This function fills array data passed as arguments with retrieved transform information.
     *
     * In this method a ring buffer is tasked with retrieving data for a given id and time.
     * For this the ring buffer searches in every buffer whether the required information is present and loads it if necessary.
     *
     * @param object_name The name of the object for which data is retrieved
     * @param object_name_length The length of the name for which data is retrieved
     * @param object_uuid The id of the object for which data is retrieved
     * @param time_stamp The point in time for which data is retrieved
     * @param matrix A float buffer in which the retrieved data is stored and from where it can be read after the function returned
     * @param object_information A int buffer in which retrieved information about the parent and active state is stored
     * @return true if successful, false otherwise
     */
    bool get_transform(char* object_name, int object_name_length, int object_uuid, float time_stamp, float* matrix, int* object_information);

    int get_sound(int sound_origin, float time_stamp, float* sound_data);

    int get_sound(int sound_origin, float time_stamp, float* sound_data, int* corresponding_go_id);

    bool get_generic(float time_stamp, int id, int* int_a, float* float_a, char* char_a);

    int get_sampling_rate(int sound_origin);

    int get_channel_num(int sound_origin);

    int get_sound_ids(int* ids, int max_id_count);

    bool create_recording_file(std::string const& recording_directory, std::string const& file_name);

    bool stop_recording();

    bool open_existing_recording_file(std::string const& directory, std::string const& file_name);

    bool stop_replay();

    bool set_transform_recording_buffer_size(int max_size);

    bool set_sound_recording_buffer_size(int max_size);

    bool set_replay_buffer_num(int buffer_num);

    bool set_replay_buffer_time_interval(float time_interval);

    int next_positions_for_id(char* object_name, int object_name_length, int object_uuid, float time_stamp, float* positions);

    int get_recording_gameobjects(char* text, int max_size);

    int get_recording_sound_sources(int* ids, int max_size);

    std::map<int,std::string> get_recording_gameobjects();

    bool register_gameobject_path(int uuid, char* path, int path_length, float time);

    int get_gameobject_path(char* text, int max_size, char* name, int name_length);

    bool register_gameobject_prefab(int uuid, char* prefab, int prefab_length, float time);

    int get_gameobject_prefab(char* text, int max_size, char* name, int name_length);

    bool register_gameobject_components(int uuid, char* components, int components_length);

    int get_gameobject_components(char* text, int max_size, char* name, int name_length);

    std::string get_gameobject_components(std::string name);

    std::string get_gameobject_mesh_path(std::string name);

    float get_gameobject_first_time(char* name, int name_length);

    float get_recording_duration();

    float get_recording_start_global_timeoffset();

    int get_original_id(char* object_name, int object_name_length, int object_id);

    int get_original_id(int object_id);

    int get_new_id(int object_id);

    bool register_recording_start_global_timeoffset(float offset_to_global_time);

    /**
     * @brief Template function for writing buffer data in a compressed manner to a file
     * @tparam T The datatype which is written to file
     * @param file_name The file path
     * @param writing_buffer The buffer of objects from type DataType that is written to file
     * @return true if successful
     */
    template <typename T>
    bool write_buffer(const std::string& file_name, std::vector<T>& writing_buffer);

    bool set_debug_mode(bool debug_state);

private:
    /// @brief information about the recording currently active during playback / recording
    Recording current_recording;

    bool sound_rec_active = false;

    /// @brief if true debug output will be generated during recording/playback
    bool debug = false;

    /// @brief if true, the complete recording is loaded into memory
    bool editing_mode = false;

    bool writing_generic_to_file = false;
    bool writing_sound_to_file = false;
    bool writing_transform_to_file = false;

    /// @brief last used time stamp during replay/recording
    float current_time = 0.0f;
    float current_sound_time = 0.0f;

    /// @brief how many transforms are stored during recording before writing them to a file
    int recording_buffer_size = 10000;
    /// @brief how many sound chunks are stored during recording before writing them to a file
    int sound_recording_buffer_size = 100;
    /// @brief how many generic elements are stored during recording before writing them to a file
    int generic_buffer_size = 100;

    int wait_count = 10000;

    // performance measurements
    float last_time;

    /// @brief the last seen transformation matrix for a given object, this can be used to avoid storing data that did not change.
    std::map<int, TransformDTO> last_transforms;

    /// @brief how many buffer are used to store transform data
    int transform_buffer_num = 3;
    /// @brief how many seconds of replay are saved in each buffer at most for transform data
    float transform_buffer_time_interval = 10.0f;
    std::shared_ptr<TransformRingBuffer> transform_ring_buffer = nullptr;

    /// @brief how many buffer are used to store sound data
    int sound_buffer_num = 3;
    /// @brief how many seconds of replay are saved in each sound buffer at most
    float sound_buffer_time_interval = 10.0f;
    std::shared_ptr<SoundRingBuffer> sound_ring_buffer = nullptr;

    /// @brief how many buffer are used to store generic data
    int generic_buffer_num = 3;
    /// @brief how many seconds of replay are saved in each generic buffer at most
    float generic_buffer_time_interval = 10.0f;
    std::shared_ptr<GenericRingBuffer> generic_ring_buffer = nullptr;

    /// @brief stores recorded transform data until it is moved to the writing buffer and written to the file
    std::vector<TransformDTO> transform_recording_buffer;
    /// @brief stores recorded sound data until it is moved to the writing buffer and written to the file
    std::vector<SoundDTO> sound_recording_buffer;
    /// @brief stores recorded generic data until it is moved to the writing buffer and written to the file
    std::vector<GenericDTO> generic_recording_buffer;

    /// @brief stores last sound information for a given id. This is used during recording to compress audio.
    std::map<int, SoundDTO> last_sound_data;

    /// @brief stores recorded transform data that is written to the file
    std::vector<TransformDTO> transform_writing_buffer;
    /// @brief stores recorded sound data that is written to the file
    std::vector<SoundDTO> sound_writing_buffer;
    /// @brief stores recorded generic data that is written to the file
    std::vector<GenericDTO> generic_writing_buffer;


    std::future<void> transform_loading;
    std::future<void> sound_loading;
    std::future<void> generic_loading;
};

#endif //__RECORDINGPLUGIN_RECORDER_H
