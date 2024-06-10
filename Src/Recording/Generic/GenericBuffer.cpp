//
// Created by Mortiferum on 18.03.2022.
//

#include "GenericBuffer.h"

GenericBuffer::GenericBuffer(float start_time, float end_time) : Buffer(start_time, end_time) {

}

std::shared_ptr<RecordableData> GenericBuffer::get_data(float time, int id) {
    float previous_time = -1.0f;
    float next_time = -1.0f;
    bool previous_found = false;
    bool next_found = false;
    int current_playhead = -1;
    int data_index = -1;

    // Note: to allow for interpolation between two consecutive buffers the min_time - (max_time - min_time) > time is being used
    if(min_time - (max_time - min_time) > time || time > max_time + (max_time - min_time)){
        Debug::Log("Error! Wrong generic buffer selected! Min t: " + std::to_string(min_time) + ", max t: " + std::to_string(max_time) + ", time: " + std::to_string(time), Color::Red);
        return nullptr;
    }

    if (id_playhead.count(id)){
        current_playhead = id_playhead[id];
    } else {
        //std::cout << "Could not find playhead_index for object with id " << id << "\n";
        id_playhead.insert(std::make_pair(id, 0));
        current_playhead = id_playhead[id];
        //std::cout << "Playhead_index for object with id " << id << " initialized with 0\n";
    }

    if (id_record_data_index.count(id)){
        data_index = id_record_data_index[id];
        if(data_index >= loaded_data_for_ids.size() || loaded_data_for_ids[data_index]->get_id() != id){
            data_index = -1;
        }
    }

    if(data_index == -1 && id_record_data_index.count(id)) {
        Debug::Log("Data_index for generic object with id " + std::to_string(id) + " invalid");
    }


    if (data_index != -1) {
        auto data = loaded_data_for_ids[data_index];

        if (current_playhead != -1 && data->loaded_data[current_playhead]->time <= time) {
            std::shared_ptr<GenericData> current_data = std::dynamic_pointer_cast<GenericData>(data->loaded_data[current_playhead]);
            while (data->loaded_data.size() > current_playhead + 1 && current_data->time <= time) {
                current_playhead += 1;
                current_data = std::dynamic_pointer_cast<GenericData>(data->loaded_data[current_playhead]);
            }
            if(current_data->time >= time) {
                next_time = current_data->time;
                next_data = current_data;
                next_found = true;

                if (current_playhead - 1 >= 0) {
                    previous_time = data->loaded_data[current_playhead - 1]->time;
                    previous_data = std::dynamic_pointer_cast<GenericData>(data->loaded_data[current_playhead - 1]);
                    previous_found = true;
                }
            } else {
                previous_time = current_data->time;
                previous_data = current_data;
                previous_found = true;
            }

            id_playhead[id] = current_playhead;
        }

        if(!next_found && !previous_found){
            //std::cout << "Iterative search for needed data necessary\n";
            int i = 0;
            for (auto const &record : data->loaded_data) {
                if (record->time <= time) {
                    previous_time = record->time;
                    previous_data = std::dynamic_pointer_cast<GenericData>(record);
                    previous_found = true;
                    id_playhead[id] = i;
                }

                if (record->time > time) {
                    next_time = record->time;
                    next_data = std::dynamic_pointer_cast<GenericData>(record);
                    next_found = true;
                    break;
                }

                i++;
            }
        }
    }

    if(!previous_found && id_last_data.count(id) > 0){
        previous_data = std::dynamic_pointer_cast<GenericData>(id_last_data[id]);
        previous_found = true;
        previous_time = previous_data->time;
    }

    if (next_found && previous_found) {
        if(std::abs(time - next_time) <= 0.001f)
            return next_data;
        else if(std::abs(time - previous_time) <= 0.001f)
            return previous_data;
        else {
            float t = (time - previous_time) / (next_time - previous_time);
            std::shared_ptr<GenericData>interpolated_data = std::make_shared<GenericData>(previous_data->interpolate(next_data, t));
            interpolated_data->interpolated_data = true;
            //std::cout << "Interpolated, PrevT: " << previous_time << ", nextT: " << next_time << " \n";
            return interpolated_data;
        }
    } else if (previous_found) {
        //std::cout << "Previous\n";
        return previous_data;
    } else if (next_found) {
        //std::cout << "Next\n";
        return next_data;
    } else {
        return nullptr;
    }
}

void GenericBuffer::reload_index_maps() {
    id_record_data_index.clear();
    for (int i = 0; i < loaded_data_for_ids.size(); i++) {
        id_record_data_index[loaded_data_for_ids[i]->get_id()] = i;
    }

    id_playhead.clear();
    for (auto & i : loaded_data_for_ids) {
        if(i != nullptr)
            id_playhead[i->get_id()] = 0;
    }
}

GenericBuffer::~GenericBuffer() {

}

std::shared_ptr<RecordableData>GenericBuffer::get_any_data(int id) {
    return nullptr;
}
