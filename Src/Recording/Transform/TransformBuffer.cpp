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

#include "TransformBuffer.h"

TransformBuffer::TransformBuffer(float start_time, float end_time) : Buffer(start_time, end_time) {
    debug = false;
}

std::shared_ptr<RecordableData> TransformBuffer::get_data(float time, int uuid) {
    float previous_time = -1.0f;
    float next_time = -1.0f;
    bool previous_found = false;
    bool next_found = false;
    int current_playhead = -1;
    int transform_index = -1;

    // Note: to allow for interpolation between two consecutive buffers the min_time - (max_time - min_time) > time is being used
    if(min_time - (max_time - min_time) > time || time > max_time + (max_time - min_time)){
        Debug::Log("Error! Wrong buffer selected! Min t: " + std::to_string(min_time) + ", max t: " + std::to_string(max_time) + ", time: " + std::to_string(time), Color::Red);
        return nullptr;
    }

    if (uuid_playhead.count(uuid)){
        current_playhead = uuid_playhead[uuid];
    } else {
        //std::cout << "Could not find playhead_index for object with uuid " << uuid << "\n";
        uuid_playhead.insert(std::make_pair(uuid, 0));
        current_playhead = uuid_playhead[uuid];
        //std::cout << "Playhead_index for object with uuid " << uuid << " initialized with 0\n";
    }

    if (uuid_record_data_index.count(uuid)){
        transform_index = uuid_record_data_index[uuid];
        if(transform_index >= loaded_data_for_ids.size() || loaded_data_for_ids[transform_index]->get_id() != uuid){
            transform_index = -1;
        }
    }

    if(transform_index == -1 && uuid_record_data_index.count(uuid)) {
        Debug::Log("Transform index for object with uuid " + std::to_string(uuid) + " invalid");

        for(int i = 0; i < loaded_data_for_ids.size(); ++i){
            if(loaded_data_for_ids[i]->get_id() == uuid){
                uuid_record_data_index[uuid] = i;
                transform_index = i;
                Debug::Log("Found transform index for object with uuid " + std::to_string(uuid));
                break;
            }
        }
    }


    if (transform_index != -1) {
        auto transforms = loaded_data_for_ids[transform_index];

        if (current_playhead != -1 && current_playhead < transforms->loaded_data.size() && transforms->loaded_data[current_playhead]->time <= time) {
            std::shared_ptr<TransformData> current_data = std::dynamic_pointer_cast<TransformData>(transforms->loaded_data[current_playhead]);
            while (transforms->loaded_data.size() > current_playhead + 1 && current_data->time <= time) {
                current_playhead += 1;
                current_data = std::dynamic_pointer_cast<TransformData>(transforms->loaded_data[current_playhead]);
            }

            if(current_data->time >= time) {
                next_time = current_data->time;
                next_transform = current_data;
                next_found = true;

                if (current_playhead - 1 >= 0) {
                    previous_time = transforms->loaded_data[current_playhead - 1]->time;
                    previous_transform = std::dynamic_pointer_cast<TransformData>(transforms->loaded_data[current_playhead - 1]);
                    previous_found = true;
                }
            } else {
                previous_time = current_data->time;
                previous_transform = current_data;
                previous_found = true;
            }

            uuid_playhead[uuid] = current_playhead - 1 >= 0 ? current_playhead : 0;
        }

        //std::cout << "Next found: " << next_found << ", Previous found: " << previous_found << "\n";

        if(!next_found && !previous_found){
            if(debug)
                Debug::Log("Iterative search for needed transforms necessary");
            int i = 0;
            for (auto time_transform : transforms->loaded_data) {
                if (time_transform->time <= time) {
                    previous_time = time_transform->time;
                    previous_transform = std::dynamic_pointer_cast<TransformData>(time_transform);
                    previous_found = true;
                    uuid_playhead[uuid] = i;
                }

                if (time_transform->time > time) {
                    next_time = time_transform->time;
                    next_transform = std::dynamic_pointer_cast<TransformData>(time_transform);
                    next_found = true;
                    break;
                }

                ++i;
            }
        }

        //std::cout << "Next found: " << next_found << ", Previous found: " << previous_found << "\n";

    }

    if(debug)
        for(auto kv : id_last_data)
            Debug::Log("ID: " + std::to_string(kv.first) + ", time: " + std::to_string(kv.second->time));

    bool last_data_used = false;
    if (!previous_found && id_last_data.count(uuid) != 0){
        if(debug)
            Debug::Log("Trying to access last seen transform");
        previous_transform = std::dynamic_pointer_cast<TransformData>(id_last_data[uuid]);
        previous_found = true;
        previous_time = previous_transform->time;
        last_data_used = true;
        if(debug)
            Debug::Log("Last seen transform accessed");
    }

    if (next_found && previous_found) {
        float t = (time - previous_time) / (next_time - previous_time);
        std::shared_ptr<TransformData> interpolated_transform = std::make_shared<TransformData>(previous_transform->interpolate(next_transform, t));
        interpolated_transform->interpolated_data = true;
        if(last_data_used && debug)
            Debug::Log("Last data used successfully");
        return interpolated_transform;
    } else if (previous_found) {
        return previous_transform;
    } else if (next_found) {
        return next_transform;
    } else {
        return nullptr;
    }
}

void TransformBuffer::reload_index_maps() {
    uuid_record_data_index.clear();
    for (int i = 0; i < loaded_data_for_ids.size(); i++) {
        uuid_record_data_index[loaded_data_for_ids[i]->get_id()] = i;
    }

    uuid_playhead.clear();
    for (auto& i : loaded_data_for_ids) {
        if(i != nullptr)
            uuid_playhead[i->get_id()] = 0;
    }
}

TransformBuffer::~TransformBuffer() {
}

std::shared_ptr<RecordableData>TransformBuffer::get_any_data(int id) {
    return nullptr;
}
