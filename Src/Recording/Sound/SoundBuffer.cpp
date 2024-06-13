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

#include "SoundBuffer.h"

std::shared_ptr<RecordableData> SoundBuffer::get_data(float time, int id) {
    bool current_found = false;
    bool next_found = false;
    int buffer_index = -1;
    int current_index = -1;

    if(get_min_time() > time || get_max_time() + 0.2f <= time){
        if(debug)
            Debug::Log("Sound data not contained in current buffer! Required time: " + std::to_string(time) + ", minT: " + std::to_string(get_min_time()) + ", maxT: " + std::to_string(get_max_time()) + "\n");
        return nullptr;
    }

    for(int i = 0; i < loaded_data_for_ids.size(); i++){
        auto id_data = loaded_data_for_ids[i];
        if(id_data != nullptr && id_data->get_id() == id){
            if(debug)
                Debug::Log("Sound id: " + std::to_string(id) + ", sound data buffer size: " + std::to_string(id_data->loaded_data.size()));

            buffer_index = i;
            for(int j = 0; j < loaded_data_for_ids[i]->loaded_data.size(); j++){
                std::shared_ptr<SoundData> data = std::dynamic_pointer_cast<SoundData>(loaded_data_for_ids[i]->loaded_data[j]);
                if(data != nullptr && data->start_time <= time && time <= data->end_time){
                    current_index = j;
                    current_found = true;
                    current_data = data;
                    break;
                }
            }
            break;
        }
    }

    if(!current_found && get_min_time() < time && get_max_time() < time && buffer_index != -1 && !loaded_data_for_ids[buffer_index]->loaded_data.empty()){
        current_data = std::dynamic_pointer_cast<SoundData>(loaded_data_for_ids[buffer_index]->loaded_data.back());
        if(current_data != nullptr) {
            current_found = true;
            current_index = loaded_data_for_ids[buffer_index]->loaded_data.size() - 1;
        }
    }

    if(current_found){
        if(debug)
            Debug::Log("Current startT: " + std::to_string(current_data->start_time) + ", endT: " + std::to_string(current_data->end_time) + ", sampleNum: " + std::to_string(current_data->sample_num) + "\n");

        if(current_index + 1 < loaded_data_for_ids[buffer_index]->loaded_data.size()) {
            next_data = std::dynamic_pointer_cast<SoundData>(loaded_data_for_ids[buffer_index]->loaded_data[current_index + 1]);
            if(next_data != nullptr)
                next_found = true;
        }
        if(next_found){
            if(debug)
                Debug::Log("Current and next found, next sampleNum: " + std::to_string(next_data->sample_num)+ "\n");

            SoundData data = current_data->interpolate(next_data, time);

            std::shared_ptr<SoundData> in_between_data = std::make_shared<SoundData>(data);

            if(debug)
                Debug::Log("Interpolated sample num: " + std::to_string(in_between_data->sample_num));
            return in_between_data;
        } else {
            if(debug) {
                std::cout << "Could not find next chunk for origin " << id << " at time " << time << "\n";
                std::cout << "___________________________________________________\n";
                for (int j = 0; j < loaded_data_for_ids[buffer_index]->loaded_data.size(); j++) {
                    std::shared_ptr<SoundData>data = std::dynamic_pointer_cast<SoundData>(loaded_data_for_ids[buffer_index]->loaded_data[j]);
                    std::cout << "startT: " << data->start_time << ", endT: " << data->end_time << "\n";
                }
                std::cout << "___________________________________________________\n";
            }
            return  current_data;
        }
    } else {
        if(debug)
            Debug::Log("Error! Could not find sound data for time " + std::to_string(time) + " in current buffer! Index: " + std::to_string(buffer_index), Color::Red);
    }

    return nullptr;
}

void SoundBuffer::reload_index_maps() {

}

SoundBuffer::SoundBuffer(float start_time, float end_time) : Buffer(start_time, end_time) {
    empty_data = std::make_shared<SoundData>();
    empty_data->sample_num = 1000;
}

SoundBuffer::~SoundBuffer() {

}

std::shared_ptr<RecordableData>SoundBuffer::get_first_data(int id) {
    for(int i = 0; i < loaded_data_for_ids.size(); i++){
        auto id_data = loaded_data_for_ids[i];
        if(id_data->get_id() == id){
            if(!id_data->loaded_data.empty())
                return id_data->loaded_data[0];
            else
                return empty_data;
        }
    }

    return empty_data;
}

std::shared_ptr<RecordableData>SoundBuffer::get_any_data(int id) {
    for(int i = 0; i < loaded_data_for_ids.size(); i++){
        auto id_data = loaded_data_for_ids[i];
        if(id_data->get_id() == id){
            if(debug)
                Debug::Log("Sound id: " + std::to_string(id) + ", sound data buffer size: " + std::to_string(id_data->loaded_data.size()));
            if(!loaded_data_for_ids[i]->loaded_data.empty())
                return loaded_data_for_ids[i]->loaded_data.front();
            break;
        }
    }

    return nullptr;
}
