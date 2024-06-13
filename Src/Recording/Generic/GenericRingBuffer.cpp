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
#include "GenericRingBuffer.h"


GenericRingBuffer::GenericRingBuffer(int buffer_num, float buffer_time, float record_duration) : RingBuffer(buffer_time, buffer_num, record_duration) {
    for(int i = 0; i < buffer_num; i++){
        std::shared_ptr<Buffer> buffer = std::make_shared<GenericBuffer>(i*buffer_time, (i+1)*buffer_time);
        buffers.push_back(buffer);
    }
}

void GenericRingBuffer::reload_index_maps(){
    for(auto buffer : buffers){
        buffer->reload_index_maps();
    }
}

bool GenericRingBuffer::data_at_time(int id, float time, int* i_arr, float* f_arr, char* c_arr){
    wait_for_loading(time);
    int current_buffer = find_buffer_for_time(time);

    //std::cout << "Current buffer: " << current_buffer << "\n";
    if(current_buffer == -1){
        Debug::Log("Error! Could not find the generic buffer containing time: " + std::to_string(time), Color::Red);
        return false;
    }

    int i = 0;
    if(!buffers[current_buffer]->stored_in_memory()) {
        Debug::Log("Error! Current generic buffer not stored in memory!", Color::Red);
        int j = 0;
        while(!buffers[current_buffer]->stored_in_memory()){
            if(i % 10000 == 0) {
                Debug::Log("waiting until current generic buffer for time: " + std::to_string(time) + " stored in memory");
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
            Debug::Log("waiting until generic buffer preparation is finished");
        ++i;
    }

    std::shared_ptr<GenericData> data = std::dynamic_pointer_cast<GenericData>(buffers[current_buffer]->get_data(time, id));
    if(data != nullptr) {
        data->update_data(i_arr, f_arr, c_arr);
        //std::cout << "Found data for time: " << time << ",RecT: " << data->time  << ", value: " << i_arr[0] << "\n";
        return true;
    } else {
        if(debug) {
            Debug::Log("Did not find generic data for id: " + std::to_string(id) + " and time: " + std::to_string(time));
            Debug::Log("Searching for last generic data in previous buffers");
            if(currently_loading)
                Debug::Log("Waiting to access last known generic data until loading is finished.");
        }

        std::shared_ptr<GenericData> last_data = nullptr;
        for(int i = 0; i < buffer_num; i++){
            if(i != current_buffer && buffers[i]->get_max_time() <= time){
                std::shared_ptr<GenericData> data = std::dynamic_pointer_cast<GenericData>(buffers[i]->get_data(time, id));
                if(data != nullptr){
                    if(last_data == nullptr){
                        last_data = data;
                    } else if(last_data->time < data->time) {
                        if(last_data->interpolated_data){
                            last_data = nullptr;
                        }
                        last_data = data;
                    }
                }
            }
        }

        if(last_data != nullptr) {
            last_data->update_data(i_arr, f_arr, c_arr);
            return true;
        }
    }
    return false;
}

DataType GenericRingBuffer::get_recorded_data_type() const {
    return DataType::Generic;
}

int GenericRingBuffer::get_dto_size() {
    return sizeof(GenericDTO);
}

void GenericRingBuffer::log_loading_data_type() const {
    Debug::Log("Loading generic recording into memory!");
}

void GenericRingBuffer::log_loading_finished() const {
    Debug::Log("Loading generic recording into memory finished!");
}

std::shared_ptr<RecordableData>GenericRingBuffer::read_single_data_from_file(std::ifstream &record_file) {
    record_file.read((char *) &s_generic_dto, sizeof(GenericDTO));
    return std::make_shared<GenericData>(s_generic_dto);
}

std::shared_ptr<RecordableData>GenericRingBuffer::duplicate_data(std::shared_ptr<RecordableData>data) {
    return std::make_shared<GenericData>(std::dynamic_pointer_cast<GenericData>(data));
}

void GenericRingBuffer::read_chunk_from_file(std::ifstream &record_file, std::shared_ptr<RecordableData>*current_data,
                                               int current_chunk_size) {
    record_file.read((char *) &generic_dto, get_dto_size() * current_chunk_size);
    for (int k = 0; k < current_chunk_size; k++) {
        current_data[k] = std::make_shared<GenericData>(generic_dto[k]);
    }
}

bool GenericRingBuffer::load_data(const LoadingInformation &load_info, std::shared_ptr<RecordableData>data) const {
    bool dont_load = data->time < load_info.min_loading_time;
    return !dont_load;
}

std::shared_ptr<RecordableData>GenericRingBuffer::read_single_data_from_vector(const std::vector<Bytef> &data_vector, int &index) {
    // Ensure there's enough data left to read a TransformDTO
    if (index + sizeof(GenericDTO) > data_vector.size()) {
        throw std::runtime_error("Not enough data left in the vector");
    }

    // Get a pointer to the current location in the vector
    const GenericDTO* dto_ptr = reinterpret_cast<const GenericDTO*>(&data_vector[index]);

    // Create a std::make_shared<TransformData> object from the DTO
    std::shared_ptr<RecordableData> result = std::make_shared<GenericData>(*dto_ptr);

    // Advance the current_index by the size of the DTO
    index += sizeof(GenericDTO);

    return result;
}
