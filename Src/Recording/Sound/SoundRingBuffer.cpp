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

#include "SoundRingBuffer.h"

SoundRingBuffer::SoundRingBuffer(int buffer_num, float buffer_time, float record_duration) : RingBuffer(buffer_time,
                                                                                                        buffer_num,
                                                                                                        record_duration) {
    for (int i = 0; i < buffer_num; i++) {
        std::shared_ptr<Buffer> buffer = std::make_shared<SoundBuffer>(i * buffer_time, (i + 1) * buffer_time);
        buffers.push_back(buffer);
    }
}

int SoundRingBuffer::get_sound_chunk_for_time(int sound_id, float time_stamp, float *sound_data) {
    int empty_go_id[3]{0};
    return get_sound_chunk_for_time(sound_id, time_stamp, sound_data, empty_go_id);
}

int SoundRingBuffer::get_sound_chunk_for_time(int sound_id, float time_stamp, float *sound_data, int* corresponding_go_id) {
    wait_for_loading(time_stamp);
    int current_buffer = find_buffer_for_time(time_stamp);

    if (current_buffer == -1) {
        Debug::Log("Error! Could not find the sound buffer containing time: " + std::to_string(time_stamp), Color::Red);
        return 0;
    }

    int i = 0;

    if (!buffers[current_buffer]->stored_in_memory()) {
        Debug::Log("Error! Current sound buffer not stored in memory!", Color::Red);
        int j = 0;
        while (!buffers[current_buffer]->stored_in_memory()) {
            if (i % 10000 == 0) {
                Debug::Log("waiting until current sound buffer for time: " + std::to_string(time_stamp) +
                           "stored in memory\n");
                ++j;
                if (!currently_loading)
                    requires_loading = true;

                for (int k = 0; k < buffers.size(); ++k) {
                    //NOTE: by uncommenting the following for loop the plugin goes into an endless loop
                    //      a possible reason for this is that the buffer values are else not being refreshed?
                    float min_t = buffers[k]->get_min_time();
                    float max_t = buffers[k]->get_max_time();
                    bool in_memory = buffers[k]->stored_in_memory();
                    if (debug) {
                        std::cout << "Buffer " << k << ", min time: " << min_t << ", max time: " << max_t
                                  << ", in memory: " << in_memory << "\n";
                    }
                }
                if (debug)
                    std::cout
                            << "--------------------------------------------------------------------------------------------------\n";

            }
            ++i;

            if (j == 1000) {
                Debug::Log("Error! Waited for to long for sound buffers to load!", Color::Red);
                return -1;
            }
        }
    }

    if (debug)
        Debug::Log("Required time: " + std::to_string(time_stamp) + ", minT: " +
                   std::to_string(buffers[current_buffer]->get_min_time()) + ", maxT: " +
                   std::to_string(buffers[current_buffer]->get_max_time()) + "\n");

    i = 0;
    while (preparing_buffer) {
        if (i % 10000 == 0)
            Debug::Log("waiting until buffer preparation is finished\n");
        ++i;
    }

    std::shared_ptr<SoundData>chunk = std::dynamic_pointer_cast<SoundData>(buffers[current_buffer]->get_data(time_stamp, sound_id));

    if (chunk != nullptr) {
        if (sampling_rates.count(sound_id) == 0 && chunk->sampling_rate >= 100)
            sampling_rates[sound_id] = chunk->sampling_rate;

        if (channel_nums.count(sound_id) == 0 && chunk->channel_num > 0)
            channel_nums[sound_id] = chunk->channel_num;

        if (debug)
            Debug::Log("Found sound for time: " + std::to_string(time_stamp) + ", startT: " +
                       std::to_string(chunk->start_time) + ", endT: " + std::to_string(chunk->end_time) +
                       ", sample_num: " + std::to_string(chunk->sample_num) + "\n");

        if (!chunk->interpolated_data) {
            if (debug)
                Debug::Log("Trying to find data from the next buffer for completion\n");
            int next = (current_buffer + 1) % buffer_num;

            int j = 0;
            while (!buffers[next]->stored_in_memory()) {
                if (j % 10000 == 0 && debug)
                    Debug::Log("waiting until next buffer is loaded\n");
                ++j;
            }

            std::shared_ptr<SoundData>next_chunk = std::dynamic_pointer_cast<SoundData>(buffers[next]->get_data(chunk->end_time, sound_id));
            if (next_chunk != nullptr) {
                if (debug)
                    Debug::Log("Found next sound chunk in next buffer\n");
                SoundData in_between_chunk = chunk->interpolate(next_chunk, time_stamp);
                std::copy(in_between_chunk.sound_data, in_between_chunk.sound_data + in_between_chunk.sample_num,
                          sound_data);
                corresponding_go_id[0] = in_between_chunk.corresponding_gameobject_id;

                return in_between_chunk.sample_num;
            } else {
                Debug::Log("Next sound chunk not found!\n");
                std::copy(chunk->sound_data, chunk->sound_data + chunk->sample_num, sound_data);
                corresponding_go_id[0] = chunk->corresponding_gameobject_id;

                int sample_num = chunk->sample_num;
                return sample_num;
            }
        }

        if (chunk->sample_num == -1) {
            Debug::Log("Error! Chunk sample num is -1!", Color::Red);
            return -1;
        }

        std::copy(chunk->sound_data, chunk->sound_data + chunk->sample_num, sound_data);
        corresponding_go_id[0] = chunk->corresponding_gameobject_id;

        int result = chunk->sample_num;

        return result;
    } else {
        if (debug)
            Debug::Log("Could not find sound data in current buffer!");

        if (!buffers[(current_buffer - 1 + buffer_num) % buffer_num]->stored_in_memory()) {
            Debug::Log("Error! The previous buffer is not stored in memory!\n");
            return 0;
        }
        std::shared_ptr<SoundData> previous_chunk = std::dynamic_pointer_cast<SoundData>(buffers[(current_buffer - 1 + buffer_num) % buffer_num]->get_data(
                time_stamp, sound_id));

        if (previous_chunk != nullptr)
            chunk = std::dynamic_pointer_cast<SoundData>(buffers[current_buffer]->get_data(previous_chunk->end_time, sound_id));

        if (previous_chunk != nullptr && chunk != nullptr) {
            if (debug)
                Debug::Log("Found sound chunk from previous buffer\n");
            SoundData in_between_chunk = previous_chunk->interpolate(chunk, time_stamp);
            std::copy(in_between_chunk.sound_data, in_between_chunk.sound_data + in_between_chunk.sample_num,
                      sound_data);
            corresponding_go_id[0] = in_between_chunk.corresponding_gameobject_id;

            return in_between_chunk.sample_num;
        } else {

            if (debug)
                Debug::Log("Error! Sound data could not be retrieved!", Color::Red);
            return -1;
        }
    }
}

void SoundRingBuffer::reload_index_maps() {
    for (auto buffer: buffers) {
        buffer->reload_index_maps();
    }
}

DataType SoundRingBuffer::get_recorded_data_type() const {
    return DataType::Sound;
}

int SoundRingBuffer::get_sampling_rate(int sound_origin) {
    Debug::Log("Trying to determine sampling rate for sound with id: " + std::to_string(sound_origin));
    int i = 0;
    while(currently_loading)
    {
        if(i % 10000 == 0)
            Debug::Log("Waiting for loading of sound data for retrieval of sampling rate.");
        ++i;
    }

    if (sampling_rates.count(sound_origin) == 0 && !buffers.empty() && buffers[0] != nullptr) {
        std::shared_ptr<SoundData>data = std::dynamic_pointer_cast<SoundData>(buffers[0]->get_any_data(sound_origin));
        if (data != nullptr && data->sampling_rate > 1000)
            sampling_rates[sound_origin] = data->sampling_rate;
        else{
            if(data == nullptr)
                Debug::Log("Could not retrieve sound data!", Color::Red);
            else
                Debug::Log("Retrieved sampling rate to low!", Color::Red);
        }
    } else if(sampling_rates.count(sound_origin) == 0){
        Debug::Log("Error! Sound buffer not loaded correctly!", Color::Red);
    }

    if (sampling_rates.count(sound_origin) > 0) {
        Debug::Log("Trying to determine sampling rate finished. Current sampling rate: " + std::to_string(sampling_rates[sound_origin]) + " for sound with id: " + std::to_string(sound_origin));
        return sampling_rates[sound_origin];
    }
    return -1;
}

int SoundRingBuffer::get_channel_num(int sound_origin) {
    Debug::Log("Trying to determine channel number for sound with id: " + std::to_string(sound_origin));

    while(currently_loading)
    {
        Debug::Log("Waiting for loading of sound data for retrieval of channel number.");
    }

    if (channel_nums.count(sound_origin) == 0 && !buffers.empty() && buffers[0] != nullptr) {
        std::shared_ptr<SoundData>data = std::dynamic_pointer_cast<SoundData>(buffers[0]->get_data((buffers[0]->get_min_time() + buffers[0]->get_max_time()) / 2.0f, sound_origin));
        if (data != nullptr && data->channel_num > 0)
            channel_nums[sound_origin] = data->channel_num;
        else{
            if(data == nullptr)
                Debug::Log("Could not retrieve sound data!", Color::Red);
            else
                Debug::Log("Retrieved channel num to low!", Color::Red);
        }
    }

    if(channel_nums.count(sound_origin) > 0) {
        Debug::Log("Trying to determine channel number finished. Current channel number: " + std::to_string(channel_nums[sound_origin]) + " for sound with id: " + std::to_string(sound_origin));
        return channel_nums[sound_origin];
    }
    return -1;
}

int SoundRingBuffer::get_dto_size() {
    return sizeof(SoundDTO);
}

void SoundRingBuffer::log_loading_data_type() const {
    Debug::Log("Loading sound recording into memory.");
}

void SoundRingBuffer::log_loading_finished() const {
    Debug::Log("Loading sound recording into memory finished.");
}

std::shared_ptr<RecordableData> SoundRingBuffer::read_single_data_from_file(std::ifstream &record_file) {
    record_file.read((char *) &s_sound_dto, sizeof(SoundDTO));
    return std::make_shared<SoundData>(s_sound_dto);
}

std::shared_ptr<RecordableData>SoundRingBuffer::duplicate_data(std::shared_ptr<RecordableData>data) {
    return std::make_shared<SoundData>(std::dynamic_pointer_cast<SoundData>(data));
}

void SoundRingBuffer::read_chunk_from_file(std::ifstream &record_file, std::shared_ptr<RecordableData>*current_data, int current_chunk_size) {
    record_file.read((char *) &sound_dto, get_dto_size() * current_chunk_size);
    for (int k = 0; k < current_chunk_size; k++) {
        current_data[k] = std::make_shared<SoundData>(sound_dto[k]);
    }
}

bool SoundRingBuffer::load_data(const LoadingInformation &load_info, std::shared_ptr<RecordableData>data) const {
    bool dont_load = std::dynamic_pointer_cast<SoundData>(data)->start_time < load_info.min_loading_time &&
                     std::dynamic_pointer_cast<SoundData>(data)->end_time < load_info.min_loading_time;
    return !dont_load;
}

std::shared_ptr<RecordableData>SoundRingBuffer::read_single_data_from_vector(const std::vector<Bytef> &data_vector, int &index) {
    // Ensure there's enough data left to read a TransformDTO
    if (index + sizeof(SoundDTO) > data_vector.size()) {
        throw std::runtime_error("Not enough data left in the vector");
    }

    // Get a pointer to the current location in the vector
    const SoundDTO* dto_ptr = reinterpret_cast<const SoundDTO*>(&data_vector[index]);

    // Create a std::make_shared<TransformData> object from the DTO
    std::shared_ptr<RecordableData> result = std::make_shared<SoundData>(*dto_ptr);

    // Advance the current_index by the size of the DTO
    index += sizeof(SoundDTO);

    return result;
}
