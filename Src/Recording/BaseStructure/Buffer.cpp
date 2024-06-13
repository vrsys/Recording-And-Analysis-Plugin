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

#include "Buffer.h"

Buffer::~Buffer() {
    if(debug)
        std::cout << "Destroying Buffer\n";
    clear_buffer();
}

bool Buffer::add_element(std::shared_ptr<RecordableData>data) {
    if(data->time >= min_time &&  data->time < max_time){
        int index = -1;
        for(int i = 0; i < loaded_data_for_ids.size(); ++i){
            if(loaded_data_for_ids[i] != nullptr && loaded_data_for_ids[i]->get_id() == data->id){
                index = i;
                break;
            }
        }

        if(index != -1){
            loaded_data_for_ids[index]->add_data(data);
        } else {
            auto record_id_data = std::make_shared<RecordedIdData>(data->id);
            loaded_data_for_ids.push_back(record_id_data);
            loaded_data_for_ids[loaded_data_for_ids.size() - 1]->add_data(data);
        }

        return true;
    } else {
        return false;
    }
}

bool Buffer::contains_time(float time) const{
    if(min_time <= time && time < max_time){
        return true;
    } else {
        return false;
    }
}

bool Buffer::stored_in_memory() const {
    return in_memory;
}

float Buffer::get_max_time() const {
    return max_time;
}

float Buffer::get_min_time() const {
    return min_time;
}

void Buffer::set_max_time(float time) {
    max_time = time;
}

void Buffer::set_min_time(float time) {
    min_time = time;
}

void Buffer::set_memory_status(bool status) {
    in_memory = status;
}

void Buffer::clear_buffer() {
    loaded_data_for_ids.clear();
    id_last_data.clear();
    set_memory_status(false);
}

Buffer::Buffer(float start_time, float end_time) : min_time(start_time), max_time(end_time){

}

void Buffer::add_last_element(std::shared_ptr<RecordableData> data) {
    if(data != nullptr) {
        if (id_last_data.count(data->id) == 0) {
            if (data->time <= max_time && data->time < min_time)
                id_last_data[data->id] = data;
        } else {
            bool before_in_buffer = id_last_data[data->id]->time > data->time && id_last_data[data->id]->time > min_time;
            bool after_out_buffer = id_last_data[data->id]->time < data->time && data->time < min_time;
            if (data->time <= max_time && (before_in_buffer || after_out_buffer))
                id_last_data[data->id] = data;
        }
    } else
        Debug::Log("Error data is not valid!", Color::Red);
}

void Buffer::set_debug_mode(bool debug_state) {
    debug = debug_state;
}
