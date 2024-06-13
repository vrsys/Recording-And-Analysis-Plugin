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
#ifndef RECORDINGPLUGIN_RINGBUFFER_H
#define RECORDINGPLUGIN_RINGBUFFER_H

#include "Buffer.h"
#include "Recording/Transform/TransformData.h"
#include "Recording/Generic/GenericData.h"
#include "Recording/Sound/SoundData.h"
#include "Recording/DataType.h"
#include "DebugLog.h"

#include "Config.h"

#include "zlib.h"
#include <chrono>
#include <string>
#include <cmath>
#include <map>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <set>
#include <memory>

struct LoadingInformation{
    float min_contained_time;
    float max_contained_time;
    float min_loading_time;
    float max_loading_time;
    std::vector<bool> buffer_loaded;
    long long min_read_line = 0;
    long long max_read_line = -1;
    float min_stored_time = -1.0f;
    float max_stored_time = -1.0f;
    bool stop_reading = false;
    std::map<int, std::shared_ptr<RecordableData>> new_id_data;
    std::map<int, std::shared_ptr<RecordableData>> last_id_data;
    std::set<int> id_finished_loading;

    ~LoadingInformation(){
        new_id_data.clear();
        last_id_data.clear();
    }

    // used to store the most recently seen data before loading data into each buffer
    // this data is later used to create a checkpoint
    void add_create_new_data(std::shared_ptr<RecordableData> data, DataType data_type){
        if (new_id_data.count(data->id) == 0 || new_id_data[data->id]->time < data->time) {
            new_id_data[data->id] = data;
        }
    }

    // used to store the most recently seen data before loading data into each buffer
    // this data is later used to create a checkpoint
    void add_new_data(std::shared_ptr<RecordableData> data, long long index){
        if (new_id_data.count(data->id) == 0 ||
            new_id_data[data->id]->time < data->time) {
            new_id_data[data->id] = data;
        }

        if(id_finished_loading.empty())
            min_read_line = index;
    }

    // used to store the most recently seen data for each id
    // this data is later stored in each buffer
    void add_last_data(std::shared_ptr<RecordableData> data, DataType data_type){
        if (last_id_data.count(data->id) == 0 || last_id_data[data->id]->time < data->time) {
            last_id_data[data->id] = data;
        }
    }
};

/**
 * @struct Checkpoint
 * @brief This struct stores information about the last data seen before a certain point in time as well as the position in the recording file from which on new data is present.
 */
struct Checkpoint {
    float time = -1.0f;
    long long already_read_elements = -1;
    std::map<int, std::shared_ptr<RecordableData>> last_seen_data;

    Checkpoint(float t, long long read_elements, std::map<int, std::shared_ptr<RecordableData>> const& data) : time(t), already_read_elements(read_elements){
        for(auto const & element : data){
            last_seen_data[element.first] = element.second;
        }
    }

    ~Checkpoint(){
        for(auto& element : last_seen_data){
            if(element.second != nullptr) {
                element.second = nullptr;
            } else {
                Debug::Log("Error! checkpoint contained invalid data!", Color::Red);
            }
        }

        last_seen_data.clear();
    }
};

/**
 * @class RingBuffer
 * @brief This class is used to load data from a recording and store it in buffers. It can be extended to load and store custom data-types.
 */
class RingBuffer {
protected:
    /// @brief index of the buffer that is currently being used for replay
    int current_active_buffer = -1;

    float recording_duration = -1.0f;

    /// @brief if true, debug output will be created during replay
    bool debug = false;

    bool requires_loading = true;
    bool preparing_buffer = false;
    bool currently_loading = false;

    /// @brief this is used during replay to access the position in the recording file faster from which we want to continue reading
    unsigned long already_read_elements = 0;

    /**
    // stores data with timestamp for objects identified by an id \n
    // note that the data in a buffer are in sorted time order \n
    //                     Buffer 1                                        Buffer 2                               \n
    //  |                                               |                                               |         \n
    //  | <---          buffer_time_interval       ---> | <---       buffer_time_interval          ---> |         \n
    //  ---------------------------------------------------------------------------------------------------> time \n
     */
    std::vector<std::shared_ptr<Buffer>> buffers;

    /// @brief max time interval for which data is stored in each buffer
    float time_per_buffer = 10.0f;

    /// @brief seconds of data stored in each buffer
    int buffer_num = 3;

    /// @brief checkpoints used for faster file access during loading
    std::vector<Checkpoint*> checkpoints;
    /// @brief Maximum number of checkpoints that can be created
    int max_checkpoint_num = 50;

    /// @brief number of elements which are read during loading in a single read instruction
    static const int read_chunk_size = 10;

public:

    virtual ~RingBuffer();

    RingBuffer(float buffer_time, int buffer_number, float record_duration);

    virtual void log_loading_data_type() const = 0;

    virtual void log_loading_finished() const = 0;

    virtual std::shared_ptr<RecordableData> read_single_data_from_file(std::ifstream& record_file) = 0;

    virtual std::shared_ptr<RecordableData> read_single_data_from_vector(std::vector<Bytef> const& data_vector, int& index) = 0;

    virtual std::shared_ptr<RecordableData> duplicate_data(std::shared_ptr<RecordableData> data) = 0;

    /// @brief returns whether this data should be loaded into this buffer
    virtual bool load_data(LoadingInformation const& load_info, std::shared_ptr<RecordableData> data) const = 0;

    virtual void read_chunk_from_file(std::ifstream &record_file, std::shared_ptr<RecordableData> current_data[], int current_chunk_size) = 0;

    bool time_in_active_buffer(float current_time) const;

    bool current_active_buffer_in_memory() const;

    bool loading_required() const;

    bool is_currently_loading() const;

    bool preparing_buffers() const;

    float get_max_stored_time() const;

    float get_min_stored_time() const;

    float get_max_loading_time() const;

    float get_min_loading_time() const;

    void clear();

    Checkpoint* get_most_recent_checkpoint(LoadingInformation const& load_info) const;

    void checkpoint_creation(LoadingInformation const& load_info);

    void checkpoint_cleanup();

    void update_buffer_timings(float required_time);

    bool prepare_buffers(float current_time);

    bool update_active_buffer(float current_time);

    int find_buffer_for_time(float time) const;

    void clear_old_buffers();

    virtual DataType get_recorded_data_type() const = 0;

    virtual void reload_index_maps() = 0;

    bool read_data(std::string const& current_recording_file);

    void wait_for_loading(float current_time) const;

    void wait_for_loading() const;

    void set_debug_mode(bool debug_state);

    virtual int get_dto_size() = 0;

private:

    bool add_element(std::shared_ptr<RecordableData> data, LoadingInformation& load_info, long long index, DataType data_type);
};

#endif //RECORDINGPLUGIN_RINGBUFFER_H
