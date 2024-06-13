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
#include "TransformRingBuffer.h"


TransformRingBuffer::TransformRingBuffer(int buffer_num, float buffer_time, float record_duration) : RingBuffer(buffer_time, buffer_num, record_duration) {
    for (int i = 0; i < buffer_num; i++) {
        std::shared_ptr<Buffer> buffer = std::make_shared<TransformBuffer>(i * buffer_time, (i + 1) * buffer_time);
        buffers.push_back(buffer);
    }
}

void TransformRingBuffer::reload_index_maps(){
    for(auto const& buffer : buffers){
        buffer->reload_index_maps();
    }
}

bool TransformRingBuffer::transform_at_time(int uuid, float time, float* matrix, int* object_information){
    wait_for_loading(time);
    int current_buffer = find_buffer_for_time(time);

    if(current_buffer == -1){
        Debug::Log("Error! Could not find the transform buffer containing time: " + std::to_string(time), Color::Red);
        return false;
    }

    int i = 0;

    if(!buffers[current_buffer]->stored_in_memory()) {
        Debug::Log("Error! Current transform buffer not stored in memory!", Color::Red);
        int j = 0;
        while(!buffers[current_buffer]->stored_in_memory()){
            if(i % 1000 == 0) {
                Debug::Log("Waiting until current buffer is stored in memory.");
                std::cout << "waiting until current buffer with time: " << time << " stored in memory\n";
                std::cout << "Loading: " << currently_loading << ", preparing buffer: " << preparing_buffer << "\n";
                if(!currently_loading)
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
                    std::cout << "--------------------------------------------------------------------------------------------------\n";

                ++j;
                if(j == 1000)
                    return false;
            }
            ++i;
        }
    }

    i = 0;
    while(preparing_buffer){
        if(i % 10000 == 0)
            Debug::Log("waiting until buffer preparation is finished.");
        ++i;
    }

    std::shared_ptr<TransformData> transform_data = std::dynamic_pointer_cast<TransformData>(buffers[current_buffer]->get_data(time, uuid));
    std::shared_ptr<TransformData> interpolated_data = nullptr;

    if(transform_data != nullptr) {
        //if(debug)
        //    std::cout << "Trying to work with data\n";
        //if(debug)
        //    std::cout << "delta t: " << time - transform_data->time << "\n";

        if(std::abs(transform_data->time - time) > 0.01f) {
           int next = (current_buffer + 1) % buffer_num;
           if (transform_data->time < time &&
                       buffers[next]->get_min_time() > transform_data->time &&
                       buffers[next]->get_min_time() - time_per_buffer < transform_data->time) {
               // interpolate for data values in between two buffers
               int j = 0;
               int k = 0;
               while (!buffers[next]->stored_in_memory()) {
                   if (j % 10000 == 0 && debug) {
                       Debug::Log("waiting until next buffer is loaded.");
                       ++k;
                   }
                   if (k == 1000) {
                       break;
                   }
                   ++j;
               }

               if (buffers[next]->stored_in_memory()){
                    std::shared_ptr<TransformData>next_transform_data = std::dynamic_pointer_cast<TransformData>(buffers[next]->get_data(time, uuid));
                    if (debug)
                        Debug::Log("Got transform data in between buffers");
                    if (next_transform_data != nullptr && next_transform_data->time - transform_data->time > 0.0001f) {
                        float t = (time - transform_data->time) / (next_transform_data->time - transform_data->time);
                        interpolated_data = std::make_shared<TransformData>(transform_data->interpolate(next_transform_data, t));
                        interpolated_data->interpolated_data = true;
                    }
                }
            }
        }

        //if(debug)
        //    std::cout << "was able to work with data\n";

        if(interpolated_data == nullptr)
            transform_data->update_matrix_and_object_info(matrix, object_information);
        else
            interpolated_data->update_matrix_and_object_info(matrix, object_information);

        return true;
    }
    else {
        //Debug::Log("Transform for object with uuid: " + std::to_string(uuid) + " not found!\n", Color::Blue);
        if(debug)
            Debug::Log("last transform for object with uuid: " + std::to_string(uuid) + " not found! Time: " + std::to_string(time), Color::Red);
        for (int i = 0; i < 10; i++) {
            matrix[i] = -1.0f;
        }
        object_information[0] = 0;
        object_information[1] = 0;
        return false;
    }
}

int TransformRingBuffer::next_positions(int uuid, float time_stamp, float *positions) {
    std::cout << "Finding required buffer\n";
    int buffer_index  = find_buffer_for_time(time_stamp);
    std::cout << "Found transform buffer that contains the currently needed transforms. Buffer id: " << buffer_index << "\n";
    int current_index = 0;
    float matrix [20];
    int object_info [2];

    std::vector<float> times;

    int current_uuid = uuid;
    int parent_uuid = 0;

    while(current_uuid != 0) {
        for (int k = 0; k < buffers.size(); k++) {
            int cur_buf_i = (buffer_index + k) % buffers.size();

            for (auto &transform : buffers[cur_buf_i]->loaded_data_for_ids) {
                if (transform->get_id() == current_uuid) {
                    std::cout << "Found transforms for object with uuid: " << current_uuid << " in buffer " << cur_buf_i << "\n";

                    for (auto &time_transform : transform->loaded_data) {
                        if (time_transform->time >= time_stamp) {
                            bool contained = false;
                            for(auto& t : times){
                                if(t == time_transform->time){
                                    contained = true;
                                }
                            }
                            if(!contained){
                                times.push_back(time_transform->time);
                            }
                        }
                    }

                    if(!transform->loaded_data.empty()){
                        parent_uuid = std::dynamic_pointer_cast<TransformData>(transform->loaded_data[0])->get_parent_uuid();
                        std::cout << "Found parent with uuid: " << parent_uuid << "\n";
                    }
                }
            }
        }
        if(current_uuid == parent_uuid){
            //current_uuid = ((std::shared_ptr<TransformData>)id_last_data[current_uuid])->get_parent_uuid();
        } else {
            current_uuid = parent_uuid;
        }
    }
    std::sort(times.begin(), times.end(), std::less<float>());

    for(auto& time : times) {
        if (time >= time_stamp) {
            if (current_index < 300) {
                std::cout << "Trying to get global transform at time: " << time << "\n";
                bool result = transform_at_time(uuid, time, matrix, object_info);
                if (!result) {
                    std::cout << "Error! Could not compute global transform for object with uuid: " << uuid << " at time: " << time << "\n";
                    return -10;
                } else {
                    positions[current_index * 4 + 0] = time;
                    positions[current_index * 4 + 1] = matrix[10];
                    positions[current_index * 4 + 2] = matrix[11];
                    positions[current_index * 4 + 3] = matrix[12];
                    current_index++;
                }
            } else {
                break;
            }
        }
    }
    std::cout << "Found " << current_index << " next positions.\n";
    return current_index;
}

DataType TransformRingBuffer::get_recorded_data_type() const{
    return DataType::Transform;
}

int TransformRingBuffer::get_dto_size() {
    return sizeof(TransformDTO);
}

void TransformRingBuffer::log_loading_data_type() const {
    Debug::Log("Loading transform recording into memory!");
}

void TransformRingBuffer::log_loading_finished() const {
    Debug::Log("Loading transform recording into memory finished.");
}

std::shared_ptr<RecordableData>TransformRingBuffer::read_single_data_from_file(std::ifstream &record_file) {
    record_file.read((char *) &s_transform_dto, sizeof(TransformDTO));
    return std::make_shared<TransformData>(s_transform_dto);
}

std::shared_ptr<RecordableData>TransformRingBuffer::duplicate_data(std::shared_ptr<RecordableData>data) {
    return std::make_shared<TransformData>(std::dynamic_pointer_cast<TransformData>(data));
}

void TransformRingBuffer::read_chunk_from_file(std::ifstream &record_file, std::shared_ptr<RecordableData>*current_data,
                                               int current_chunk_size) {
    record_file.read((char *) &transform_dto, get_dto_size() * current_chunk_size);
    for (int k = 0; k < current_chunk_size; k++) {
        current_data[k] = std::make_shared<TransformData>(transform_dto[k]);
    }
}

bool TransformRingBuffer::load_data(const LoadingInformation &load_info, std::shared_ptr<RecordableData>data) const {
    bool dont_load = data->time < load_info.min_loading_time;
    return !dont_load;
}

std::shared_ptr<RecordableData>TransformRingBuffer::read_single_data_from_vector(const std::vector<Bytef> &data_vector, int& index) {
    // Ensure there's enough data left to read a TransformDTO
    if (index + sizeof(TransformDTO) > data_vector.size()) {
        throw std::runtime_error("Not enough data left in the vector");
    }

    // Get a pointer to the current location in the vector
    const TransformDTO* dto_ptr = reinterpret_cast<const TransformDTO*>(&data_vector[index]);

    // Create a std::make_shared<TransformData> object from the DTO
    std::shared_ptr<RecordableData> result = std::make_shared<TransformData>(*dto_ptr);

    // Advance the current_index by the size of the DTO
    index += sizeof(TransformDTO);

    return result;
}