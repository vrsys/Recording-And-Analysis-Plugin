//
// Created by Mortiferum on 01.12.2021.
//

#include "Recorder.h"
#include "RecorderManager.h"
#include "../../External/zlib/zlib.h"

bool Recorder::create_recording_file(std::string const& recording_directory, std::string const& file_name) {
    if(current_recording.is_currently_recording()) {
        Debug::Log("Error! Cannot create a new recording while still recording!", Color::Red);
        return false;
    }

    try {
        Debug::Log("Stopping playback");

        stop_replay();

        Debug::Log("Creating a new recording file: " + file_name);

        current_recording = Recording(recording_directory, file_name);
        current_recording.setup_for_recording();

        transform_recording_buffer.reserve(recording_buffer_size);
        transform_writing_buffer.reserve(recording_buffer_size);
        sound_recording_buffer.reserve(sound_recording_buffer_size);
        sound_writing_buffer.reserve(sound_recording_buffer_size);
        generic_recording_buffer.reserve(generic_buffer_size);
        generic_writing_buffer.reserve(generic_buffer_size);
        return true;
    } catch (const std::exception &e) {
        Debug::Log("Error! Could not create files for recording.", Color::Red);
        return false;
    }
}

bool Recorder::open_existing_recording_file(std::string const& directory, std::string const& file_name) {
    if(current_recording.is_currently_replaying()) {
        Debug::Log("Error! Cannot playback a new recording while still playing back!", Color::Red);
        return false;
    }

    try {
        stop_replay();

        current_sound_time = 0.0f;
        current_time = 0.0f;

        current_recording = Recording(directory, file_name);

        if(current_recording.setup_for_replay()) {
            if (debug) {
                Debug::Log("Size of bool: " + std::to_string(sizeof(bool)));
                Debug::Log("Size of int: " + std::to_string(sizeof(int)));
                Debug::Log("Size of float: " + std::to_string(sizeof(float)));
                Debug::Log("Size of char: " + std::to_string(sizeof(char)));
                Debug::Log("Size of Transform: " + std::to_string(sizeof(TransformData)));
                Debug::Log("Size of Sound: " + std::to_string(sizeof(SoundData)));
                Debug::Log("Size of Generic: " + std::to_string(sizeof(GenericData)));
                Debug::Log("Size of RecordableData: " + std::to_string(sizeof(RecordableData)));
                Debug::Log("Size of TransformdDTO: " + std::to_string(sizeof(TransformDTO)));
                Debug::Log("Size of SoundDTO: " + std::to_string(sizeof(SoundDTO)));
                Debug::Log("Size of genericDTO: " + std::to_string(sizeof(GenericDTO)));
            }

            if(editing_mode){
                Debug::Log("Editing mode! Complete recording will be loaded into memory!");
                transform_buffer_num = 1;
                transform_buffer_time_interval = current_recording.meta_information.get_recording_duration() + 10.0f;
                sound_buffer_num = 1;
                sound_buffer_time_interval = current_recording.meta_information.get_recording_duration() + 10.0f;
                generic_buffer_num = 1;
                generic_buffer_time_interval = current_recording.meta_information.get_recording_duration() + 10.0f;
            }

            transform_ring_buffer = std::make_shared<TransformRingBuffer>(transform_buffer_num, transform_buffer_time_interval,current_recording.duration);
            sound_ring_buffer = std::make_shared<SoundRingBuffer>(sound_buffer_num, sound_buffer_time_interval, current_recording.duration);
            generic_ring_buffer = std::make_shared<GenericRingBuffer>(generic_buffer_num, generic_buffer_time_interval, current_recording.duration);

#if (MULTI_THREADED == 1)
            transform_loading = RecorderManager::getThreadPool().submit_task([this]{transform_ring_buffer->read_data(current_recording.transform_file);});
            sound_loading = RecorderManager::getThreadPool().submit_task([this] {sound_ring_buffer->read_data(current_recording.sound_file);});
            generic_loading = RecorderManager::getThreadPool().submit_task([this] {generic_ring_buffer->read_data(current_recording.generic_file);});
#else
            transform_ring_buffer->read_data(current_recording.transform_file);
            sound_ring_buffer->read_data(current_recording.sound_file);
            generic_ring_buffer->read_data(current_recording.generic_file);
#endif
            return true;
        }

        return false;
    } catch (const std::exception &e) {
        Debug::Log("Error! An exception occurred!", Color::Red);
        return false;
    }
    return false;
}

bool Recorder::record_transform(std::string const& object_name, int id,
                                float const *transform_values, float time_stamp,
                                int const *object_information) {
    try {
        if (time_stamp < 0.0f)
            return false;

        //std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();


        if(current_recording.state != RecordingState::Record)
            return false;

        current_recording.meta_information.update_recording_time(time_stamp);
        current_recording.meta_information.update_name_uuid_map(object_name, id);
        current_recording.meta_information.add_transform_id_information(id);

        //std::cout << "Recorded object with id: " << transform_data.id << " at time: " << transform_data.time << "\n";
        //std::cout << "Transform: " << transform_data << "\n";

        //TransformDTO transform {id, time_stamp,
        //                          transform_values[0],transform_values[1], transform_values[2],
        //                          transform_values[3], transform_values[4], transform_values[5], transform_values[6],
        //                          transform_values[7], transform_values[8], transform_values[9],
        //                          transform_values[10], transform_values[11], transform_values[12],
        //                          transform_values[13], transform_values[14], transform_values[15], transform_values[16],
        //                          transform_values[17], transform_values[18], transform_values[19],
        //                          object_information[0], object_information[1]};
        //if(last_transforms.count(id)){
        //    TransformDTO last_transform = last_transforms[id];
        //    if(last_transform == transform)
        //        return true;
        //}
        //transform_recording_buffer.push_back(transform);


        //last_transforms[id] = transform;

        bool recording_buffer_filled = transform_recording_buffer.size() >= recording_buffer_size - 1;

        int i = 0;
        while (recording_buffer_filled && writing_transform_to_file) {
            if (i % wait_count == 0)
                Debug::Log("Waiting until the current transform data is written to file before recording new data");
            i++;
        }

        transform_recording_buffer.push_back({id, time_stamp,
                                              transform_values[0], transform_values[1], transform_values[2],
                                              transform_values[3], transform_values[4], transform_values[5],
                                              transform_values[6],
                                              transform_values[7], transform_values[8], transform_values[9],
                                              transform_values[10], transform_values[11], transform_values[12],
                                              transform_values[13], transform_values[14], transform_values[15],
                                              transform_values[16],
                                              transform_values[17], transform_values[18], transform_values[19],
                                              object_information[0], object_information[1]});

        if (recording_buffer_filled) {
            Debug::Log("Moving transform buffer to writing state");
            transform_writing_buffer = std::move(transform_recording_buffer);
            transform_recording_buffer.reserve(recording_buffer_size);
        }

        if (!transform_writing_buffer.empty() && !writing_transform_to_file) {
            writing_transform_to_file = true;
#if (MULTI_THREADED == 1)
            Debug::Log("Pushing thread task for writing buffered data to file.");
            RecorderManager::getThreadPool().submit_task([this]() { this->write_buffer<TransformDTO>(this->current_recording.transform_file, this->transform_writing_buffer); });
#else
            write_buffer<TransformDTO>(current_recording.transform_file, transform_writing_buffer);
#endif
        }

        //std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        //if(std::abs(time_stamp - last_time) > 0.0001f){
        //    record_transform.add_measurement();
        //    last_time = time_stamp;
        //}
        //record_transform += end - begin;

    } catch (std::exception &e) {
        Debug::Log("A problem occurred trying to record the transforms for object with uuid " + std::to_string(id) + ". " + e.what(), Color::Red);
        return false;
    }
    return true;
}

bool Recorder::stop_recording() {
    try {
        if(current_recording.state != RecordingState::Record) {
            Debug::Log("Cannot stop recording, because the recorder is not recording");
            return false;
        }

        if(debug) {
            Debug::Log("Stopping the recording");
            Debug::Log("Transform record buffer size: " + std::to_string(transform_recording_buffer.size()));
            Debug::Log("Transform write buffer size: " + std::to_string(transform_writing_buffer.size()));
            Debug::Log("Sound record buffer size: " + std::to_string(sound_recording_buffer.size()));
            Debug::Log("Sound write buffer size: " + std::to_string(sound_writing_buffer.size()));
            Debug::Log("Generic record buffer size: " + std::to_string(generic_recording_buffer.size()));
            Debug::Log("Generic write buffer size: " + std::to_string(generic_writing_buffer.size()));
        }

        last_time = 0.0f;

        int i = 0;
        while(writing_transform_to_file){
            if(i % 10000 == 0){
                Debug::Log("Waiting for transform data to be written to file");
            }
            i++;
        }
        if (!transform_writing_buffer.empty()) {
            write_buffer<TransformDTO>(current_recording.transform_file, transform_writing_buffer);
        }

        if (!transform_recording_buffer.empty()) {
            transform_writing_buffer = std::move(transform_recording_buffer);
            write_buffer<TransformDTO>(current_recording.transform_file, transform_writing_buffer);
        }

        for(auto kv : last_sound_data) {
            sound_recording_buffer.push_back(kv.second);
        }

        last_sound_data.clear();

        while(writing_sound_to_file){
            if(i % 10000 == 0){
                Debug::Log("Waiting for sound data to be written to file");
            }
            i++;
        }
        if (!sound_writing_buffer.empty()) {
            write_buffer<SoundDTO>(current_recording.sound_file, sound_writing_buffer);
        }

        if (!sound_recording_buffer.empty()) {
            sound_writing_buffer = std::move(sound_recording_buffer);
            write_buffer<SoundDTO>(current_recording.sound_file, sound_writing_buffer);
        }

        while(writing_generic_to_file){
            if(i % 10000 == 0){
                Debug::Log("Waiting for generic data to be written to file");
            }
            i++;
        }

        if (!generic_writing_buffer.empty()) {
            write_buffer<GenericDTO>(current_recording.generic_file, generic_writing_buffer);
        }

        if (!generic_recording_buffer.empty()) {
            generic_writing_buffer = std::move(generic_recording_buffer);
            write_buffer<GenericDTO>(current_recording.generic_file, generic_writing_buffer);
        }

        last_transforms.clear();

        current_recording.end_recording();
        current_recording.state = RecordingState::Idle;
        Debug::Log("Recording stopped");
    } catch (const std::exception &e) {
        Debug::Log("Error! An exception occurred!", Color::Red);
        return false;
    }
    return true;
}

bool Recorder::get_transform(char *object_name, int object_name_length, int object_uuid, float time_stamp,
                             float *matrix, int *object_information) {
    try {
        //std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
        current_time = time_stamp;

        if(current_recording.state != RecordingState::Replay)
            return false;

        if (!current_recording.meta_information.time_contained_in_recording(current_time)) {
            Debug::Log("Error! Time not contained in recording. Requested time: " + std::to_string(current_time) + ", Contained time: " + std::to_string(get_recording_duration()), Color::Red);
            return false;
        }

        int id = current_recording.meta_information.get_old_uuid(object_uuid);

        if (id == -1) {
            std::string name;
            for (int i = 0; i < object_name_length; i++)
                name += object_name[i];

            int old_uuid = current_recording.meta_information.get_old_uuid(name, object_uuid);
            if (old_uuid != -1) {
                if (debug)
                    Debug::Log("Name: " + name + ", old UUID: " + std::to_string(object_uuid) + ", new UUID " + std::to_string(old_uuid));
                object_uuid = old_uuid;
            } else {
                Debug::Log("Could not find the old UUID for object: " + name + ", with current UUID: " + std::to_string(object_uuid), Color::Red);
            }
        } else
            object_uuid = id;

        //if (transform_replaying && !transform_recording && transform_ring_buffer != nullptr &&
        //    !transform_ring_buffer->preparing_buffers() && transform_ring_buffer->loading_required() && !transform_ring_buffer->is_currently_loading()) {
        //    Debug::Log("transform data load task submitted to thread pool.");
        //    RecorderManager::getThreadPool().push_task(&TransformRingBuffer::read_data, transform_ring_buffer, current_recording_file);
        //}

        if(transform_ring_buffer == nullptr)
            return false;

        bool result = transform_ring_buffer->prepare_buffers(time_stamp);
        if (!result) {
            Debug::Log("Could not load the transform data for object with id " + std::to_string(object_uuid), Color::Red);
            return false;
        }

        if (!transform_ring_buffer->preparing_buffers() && transform_ring_buffer->loading_required() && !transform_ring_buffer->is_currently_loading()) {
            if(debug)
                Debug::Log("transform data load task submitted to thread pool.");
#if (MULTI_THREADED == 1)
            transform_loading = RecorderManager::getThreadPool().submit_task([this]{transform_ring_buffer->read_data(current_recording.transform_file);});
#else
            transform_ring_buffer->read_data(current_recording.transform_file);
#endif
        }

        result = transform_ring_buffer->transform_at_time(object_uuid, current_time, matrix, object_information);
        if (!result) {
            if(debug)
                Debug::Log("Could not get the transform data for object with id " + std::to_string(object_uuid), Color::Red);
            return false;
        }

        //std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

        //if(std::abs(time_stamp - last_time) > 0.0001f){
        //    get_transform.add_measurement();
        //    last_time = time_stamp;
        //}
        //get_transform += end - begin;

        return result;

    } catch (std::exception &e) {
        Debug::Log(e.what(), Color::Red);
        return false;
    }
    return false;
}

bool Recorder::stop_replay() {
    if(current_recording.state != RecordingState::Replay)
        return false;

    if(debug)
        Debug::Log("Stopping the replay");

    if(debug)
        Debug::Log("Clearing transform buffers");
    if (transform_ring_buffer != nullptr) {
        transform_ring_buffer->clear();
        transform_ring_buffer = nullptr;
    }
    if(debug)
        Debug::Log("Cleared transform buffers");

    if (sound_ring_buffer != nullptr) {
        sound_ring_buffer->clear();
        sound_ring_buffer = nullptr;
    }
    if(debug)
        Debug::Log("Cleared sound buffers");

    if (generic_ring_buffer != nullptr) {
        generic_ring_buffer->clear();
        generic_ring_buffer = nullptr;
    }
    if(debug)
        Debug::Log("Cleared generic buffers");

    //initial_time = -1.0f;
    current_time = 0.0f;
    current_sound_time = 0.0f;

    current_recording.end_replay();

    if(debug)
        Debug::Log("Replay stopped and data cleared");

    last_time = 0.0f;
    current_recording.state = RecordingState::Idle;
    return true;
}

bool Recorder::set_transform_recording_buffer_size(int max_size) {
    if (max_size > 0) {
        recording_buffer_size = max_size;
        if(debug)
            Debug::Log("Set recording max buffer size to " + std::to_string(max_size));
        return true;
    }
    return false;
}

bool Recorder::set_replay_buffer_num(int buffer_num) {
    if (buffer_num > 0) {
        transform_buffer_num = buffer_num;
        if(debug)
            Debug::Log("Set replay buffer number to " + std::to_string(buffer_num));
        return true;
    }
    return false;
}

bool Recorder::set_replay_buffer_time_interval(float time_interval) {
    if (time_interval > 0.0f) {
        transform_buffer_time_interval = time_interval;
        if(debug)
            Debug::Log("Set transform buffer time interval to " + std::to_string(time_interval));
        return true;
    }
    return false;
}

bool Recorder::record_sound(float *sound_data, int sound_data_length, int sampling_rate, int start_index, int channel_num,
                            float time_stamp, int sound_id) {
    return record_sound(sound_data, sound_data_length, sampling_rate, start_index, channel_num, -1, time_stamp, sound_id);
}

bool
Recorder::record_sound(float *sound_data, int sound_data_length, int sampling_rate, int start_index, int channel_num,
                       int corresponding_gameobject_id, float time_stamp, int sound_id) {
    try {
        if(current_recording.state != RecordingState::Record)
            return false;

        if (time_stamp < 0.0f)
            return false;

        int i = 0;
        // simple lock to avoid crashes during simultaneous function calls caused by asynchronous threads
        while(sound_rec_active){
            if (i % wait_count == 0 && debug)
                Debug::Log("Waiting until previous record call finished.");
            i++;
        }
        sound_rec_active = true;

        current_recording.meta_information.update_recording_time(time_stamp);
        current_recording.meta_information.add_sound_id_information(sound_id);

        float duration = (float)(sound_data_length/ channel_num) / (float) sampling_rate;

        if(debug)
            Debug::Log("Recording sound for id: " + std::to_string(sound_id) + ", start time: " + std::to_string(time_stamp) + ", end time: " + std::to_string(time_stamp + duration));

        SoundDTO sound_dto{sound_id, time_stamp, time_stamp,
                           time_stamp + duration, sound_data_length,
                           start_index, sampling_rate, channel_num, corresponding_gameobject_id};
        std::copy(sound_data + start_index, sound_data + start_index + sound_data_length, sound_dto.s_d + start_index);

        bool recording_buffer_filled = sound_recording_buffer.size() >= sound_recording_buffer_size - 1;

        i = 0;
        while (recording_buffer_filled && writing_sound_to_file) {
            if (i % wait_count == 0 && debug)
                Debug::Log("Waiting until the current sound data is written to file before recording new data");
            i++;
        }

        int subsample_factor = 2;
        bool subsampled = false;
        if(sampling_rate % subsample_factor == 0 && sampling_rate/subsample_factor >= 16000){
            for (int i = 0; i < sound_data_length/subsample_factor; i += channel_num) {
                int input_index = i * subsample_factor;
                for(int j = 0; j < channel_num; ++j) {
                    sound_dto.s_d[i + j] = sound_data[input_index + j + start_index];
                }
                subsampled = true;
            }
            sound_dto.s_i = 0;
            sound_dto.s_n = sound_data_length/subsample_factor;
            sound_dto.s_r = sampling_rate/subsample_factor;
        }

        if(debug)
            Debug::Log("Subsampled audio source: " + std::to_string(sound_id));

        if(last_sound_data.count(sound_id) == 0)
            last_sound_data[sound_id] = sound_dto;
        else{
            SoundDTO previousDto = last_sound_data[sound_id];
            float end_time = previousDto.e_t;
            if(debug)
                std::cout << "Sound id: " << sound_id << ",previous start time: " << previousDto.s_t << ", previous end time: " << previousDto.e_t << "\n";
            if(debug)
                std::cout << "Sound id: " << sound_id << ",current start time: " << sound_dto.s_t << ", current end time: " << sound_dto.e_t << "\n";

            int skip_sample_count = (int)((end_time - sound_dto.s_t) * sound_dto.s_r);

            if(skip_sample_count > 0) {
                int power = std::log2(skip_sample_count);
                int lower_pow2 = std::pow(2, power);
                int upper_pow2 = std::pow(2, power + 1);
                skip_sample_count = (skip_sample_count - lower_pow2) < (upper_pow2 - skip_sample_count) ? lower_pow2 : upper_pow2;
            }

            int added_sample_num = 0;
            if(skip_sample_count >= 0)
                added_sample_num = sound_dto.s_n - skip_sample_count;
            else
                added_sample_num = sound_dto.s_n + skip_sample_count;

            if(debug)
                std::cout << "Skip sample num: " << skip_sample_count << ". adding sample num: " <<  added_sample_num << "\n";
            if(previousDto.s_i + previousDto.s_n + added_sample_num < 4800 && added_sample_num > 0){
                if(skip_sample_count >= 0) {
                    std::copy(sound_dto.s_d + sound_dto.s_i + skip_sample_count,
                              sound_dto.s_d + sound_dto.s_i + sound_dto.s_n,
                              previousDto.s_d + previousDto.s_n + previousDto.s_i);
                } else {
                    std::copy(sound_dto.s_d + sound_dto.s_i,
                              sound_dto.s_d + sound_dto.s_i + sound_dto.s_n,
                              previousDto.s_d + previousDto.s_n + previousDto.s_i + skip_sample_count);
                }

                previousDto.s_n = previousDto.s_n + added_sample_num;
                previousDto.e_t = sound_dto.e_t;
                last_sound_data[sound_id] = previousDto;
            } else {
                sound_recording_buffer.push_back(previousDto);
                last_sound_data[sound_id] = sound_dto;
            }
        }

        if (recording_buffer_filled) {
            sound_writing_buffer = std::move(sound_recording_buffer);
            sound_recording_buffer.reserve(sound_recording_buffer_size);
        }

        if (!sound_writing_buffer.empty() && !writing_sound_to_file) {
            writing_sound_to_file = true;
#if (MULTI_THREADED == 1)
            RecorderManager::getThreadPool().submit_task([this]() { this->write_buffer<SoundDTO>(this->current_recording.sound_file, this->sound_writing_buffer); });
#else
            write_buffer<SoundDTO>(current_recording.sound_file, sound_writing_buffer);
#endif
        }

        sound_rec_active = false;

        //std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        //if(std::abs(time_stamp - last_time) > 0.0001f){
        //    record_sound.add_measurement();
        //    last_time = time_stamp;
        //}
        //record_sound += end - begin;

        return true;
    } catch (std::exception &e) {

        Debug::Log(e.what(), Color::Red);
        return false;
    }
    return false;
}

bool Recorder::set_sound_recording_buffer_size(int max_size) {
    if (max_size > 0) {
        sound_recording_buffer_size = max_size;
    }
    return false;
}

int Recorder::get_sound(int sound_origin, float time_stamp, float *sound_data) {
    int empty_go_id[3]{1};
    return get_sound(sound_origin, time_stamp, sound_data, empty_go_id);
}

int Recorder::get_sound(int sound_origin, float time_stamp, float *sound_data, int* corresponding_go_id) {
    try {
        if(current_recording.state != RecordingState::Replay)
            return false;

        current_sound_time = time_stamp;

        if (debug) {
            Debug::Log("Trying to get sound chunk for origin: " + std::to_string(sound_origin) + " and  time: " + std::to_string(time_stamp));
        }

        if (!current_recording.meta_information.time_contained_in_recording(current_sound_time)) {
            Debug::Log("Time for sound not contained in recording!");
            return -1;
        }

        //if (sound_replaying && !sound_recording && sound_ring_buffer != nullptr && !sound_ring_buffer->preparing_buffers() && sound_ring_buffer->loading_required() && !sound_ring_buffer->is_currently_loading()) {
        //    Debug::Log("Sound data load task submitted to thread pool.");
        //    RecorderManager::getThreadPool().push_task(&SoundRingBuffer::read_data, sound_ring_buffer, current_sound_recording_file);
        //}

        if(sound_ring_buffer == nullptr)
            return -1;

        bool res = sound_ring_buffer->prepare_buffers(current_sound_time);
        if (!res) {
            Debug::Log("Could not load the sound data for sound with id " + std::to_string(sound_origin), Color::Red);
            return false;
        }

        if (!sound_ring_buffer->preparing_buffers() && sound_ring_buffer->loading_required() && !sound_ring_buffer->is_currently_loading()) {
            if(debug)
                Debug::Log("Sound data load task submitted to thread pool.");
#if (MULTI_THREADED == 1)
            sound_loading = RecorderManager::getThreadPool().submit_task([this]{sound_ring_buffer->read_data(current_recording.sound_file);});
#else
            sound_ring_buffer->read_data(current_recording.sound_file);
#endif
        }

        int result = sound_ring_buffer->get_sound_chunk_for_time(sound_origin, current_sound_time, sound_data, corresponding_go_id);
        if (result < 1) {
            if (debug)
                Debug::Log(
                        "Could not retrieve the sound chunk for origin " + std::to_string(sound_origin) + " for time " +
                        std::to_string(current_sound_time), Color::Red);
            //std::cout << "Since the time is contained in the recording an empty sound chunk will be returned.\n";
            int empty_length = 1000;
            for (int i = 0; i < empty_length; i++) {
                sound_data[i] = 0.0f;
            };
            result = empty_length;
        }

        //std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        //if(std::abs(time_stamp - last_time) > 0.0001f){
        //    get_sound.add_measurement();
        //    last_time = time_stamp;
        //}
        //get_sound += end - begin;

        return result;

    } catch (std::exception &e) {
        Debug::Log("Getting sound exception for origin " + std::to_string(sound_origin) + " : " + e.what(), Color::Red);
        return -1;
    }
    return -1;
}

int Recorder::get_sampling_rate(int sound_origin) {
    if (sound_ring_buffer != nullptr) {
        try {
            if (!sound_ring_buffer->preparing_buffers() && sound_ring_buffer->loading_required() && !sound_ring_buffer->is_currently_loading()) {
#if (MULTI_THREADED == 1)
                sound_loading = RecorderManager::getThreadPool().submit_task([this]{sound_ring_buffer->read_data(current_recording.sound_file);});
#else
                sound_ring_buffer->read_data(current_recording.sound_file);
#endif
                Debug::Log("Dispatched thread to load sound data for sampling rate retrieval.");
            }

            sound_loading.wait();
            return sound_ring_buffer->get_sampling_rate(sound_origin);
        } catch (std::exception &e) {
            Debug::Log("Getting sound exception for origin " + std::to_string(sound_origin) + " : " + e.what(), Color::Red);
            return -1;
        }
    }
    return -1;
}

int Recorder::next_positions_for_id(char *object_name, int object_name_length, int object_uuid, float time_stamp,
                                    float *positions) {
    if(current_recording.state != RecordingState::Replay)
        return -1;

    Debug::Log("Trying to find next positions for object with uuid " + std::to_string(object_uuid));
    if (!current_recording.meta_information.time_contained_in_recording(time_stamp)) {
        Debug::Log("Error! The required time is not contained in the recording!");
        return -1;
    }

    std::string name;
    for (int i = 0; i < object_name_length; i++) {
        name += object_name[i];
    }

    int old_uuid = current_recording.meta_information.get_old_uuid(name, object_uuid);
    if (old_uuid != -1) {
        std::cout << "Name: " << name << ", old UUID: " << object_uuid << ", new UUID " << old_uuid << "\n";
        object_uuid = old_uuid;
    } else {
        old_uuid = current_recording.meta_information.get_old_uuid(object_uuid);
        if (old_uuid != -1)
            std::cout << "Name: " << name << ", old UUID: " << object_uuid << ", new UUID " << old_uuid << "\n";
        else
            std::cout << "Could not find the old UUID for object: " << name << ", with current UUID: " << object_uuid
                      << "\n";
    }

    return transform_ring_buffer->next_positions(object_uuid, time_stamp, positions);
}

int Recorder::get_recording_gameobjects(char *text, int max_size) {
    if(current_recording.state != RecordingState::Replay)
        return -1;

    Debug::Log("Trying to get game objects needed for replay");
    std::string t = current_recording.meta_information.get_recording_gameobjects();
    Debug::Log("Length of return string: " + std::to_string(t.length()));
    if (t.length() < max_size) {
        snprintf(text, max_size, "%s\n", t.c_str());
        return t.length();
    } else {
        return -t.length();
    }
}

int Recorder::get_recording_sound_sources(int* ids, int max_size) {
    if(current_recording.state != RecordingState::Replay)
        return -1;

    Debug::Log("Trying to get sound source ids needed for replay");
    std::set<int> sound_ids = current_recording.meta_information.get_recording_sound_sources();
    Debug::Log("Length of return string: " + std::to_string(sound_ids.size()));
    if (sound_ids.size() <= max_size) {
        int i = 0;
        for(auto id : sound_ids){
            ids[i] = id;
            i++;
        }
        return sound_ids.size();
    } else {
        return -sound_ids.size();
    }
}

bool Recorder::register_gameobject_path(int uuid, char *path, int path_length, float time) {
    if(current_recording.state != RecordingState::Record)
        return false;

    std::string path_string;
    for (int i = 0; i < path_length; i++)
        path_string += path[i];

    //Debug::Log("Registered object with uuid: " + std::to_string(uuid) + " at time: " + std::to_string(time), Color::Red);
    return current_recording.meta_information.register_gameobject_path(uuid, path_string, time);
}

bool Recorder::register_gameobject_prefab(int uuid, char *prefab, int prefab_length, float time) {
    if(current_recording.state != RecordingState::Record)
        return false;

    std::string prefab_string;
    for (int i = 0; i < prefab_length; i++)
        prefab_string += prefab[i];

    //Debug::Log("Registered object with uuid: " + std::to_string(uuid) + " at time: " + std::to_string(time), Color::Red);
    return current_recording.meta_information.register_gameobject_prefab(uuid, prefab_string, time);
}

int Recorder::get_gameobject_path(char *text, int max_size, char *name, int name_length) {
    if(current_recording.state != RecordingState::Replay)
        return -1;

    std::string object_name;
    for (int i = 0; i < name_length; i++)
        object_name += name[i];

    std::string t = current_recording.meta_information.get_gameobject_path(object_name);
    if (t.length() < max_size) {
        snprintf(text, max_size, "%s\n", t.c_str());
        return t.length();
    } else {
        return -1;
    }
}

int Recorder::get_gameobject_prefab(char *text, int max_size, char *name, int name_length) {
    if(current_recording.state != RecordingState::Replay)
        return -1;

    std::string object_name;
    for (int i = 0; i < name_length; i++)
        object_name += name[i];

    std::string t = current_recording.meta_information.get_gameobject_prefab(object_name);
    if (t.length() < max_size) {
        snprintf(text, max_size, "%s\n", t.c_str());
        return t.length();
    } else {
        return -1;
    }
}

bool Recorder::register_gameobject_components(int uuid, char *components, int components_length) {
    if(current_recording.state != RecordingState::Record)
        return false;

    std::string components_string;
    for (int i = 0; i < components_length; i++)
        components_string += components[i];

    return current_recording.meta_information.register_gameobject_components(uuid, components_string);
}

int Recorder::get_gameobject_components(char *text, int max_size, char *name, int name_length) {
    if(current_recording.state != RecordingState::Replay)
        return -1;

    std::string object_name;
    for (int i = 0; i < name_length; i++)
        object_name += name[i];

    std::string t = current_recording.meta_information.get_gameobject_components(object_name);
    if (t.length() <= max_size) {
        snprintf(text, sizeof(text), "%s", t.c_str());
        //strcpy_s(text, sizeof text, t.c_str());
        return t.length();
    } else {
        return -1;
    }
}

float Recorder::get_gameobject_first_time(char *name, int name_length) {
    if(current_recording.state != RecordingState::Replay)
        return -1.0f;

    std::string object_name;
    for (int i = 0; i < name_length; i++)
        object_name += name[i];

    return current_recording.meta_information.get_gameobject_first_seen_time(object_name);
}

float Recorder::get_recording_duration() {
    if(current_recording.state != RecordingState::Replay)
        return -1.0f;

    return current_recording.meta_information.get_recording_duration();
}

bool Recorder::register_recording_start_global_timeoffset(float offset_to_global_time) {
    if(current_recording.state != RecordingState::Record)
        return false;

    return current_recording.meta_information.register_recording_start_global_timeoffset(offset_to_global_time);
}

template <typename T>
bool Recorder::write_buffer(const std::string& file_name, std::vector<T>& writing_buffer) {
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    if (debug)
        Debug::Log("Writing " + std::to_string(writing_buffer.size()) + " chunks into the recording file.");

    std::ofstream record_file(file_name, std::ios_base::app | std::ios_base::out | std::ios::binary);

    if (record_file.is_open()) {
#if (COMPRESSION == 1)
        uint32_t source_count = writing_buffer.size();
        uint32_t source_size = writing_buffer.size() * sizeof(DataType);
        uLongf dest_size = compressBound(source_size);
        std::vector<Bytef> compressed_data(dest_size);

        // Initialize ZLIB stream
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = source_size;
        strm.next_in = (Bytef*)writing_buffer.data();
        strm.avail_out = dest_size;
        strm.next_out = compressed_data.data();

        if (deflateInit(&strm, COMPRESSION_LEVEL) != Z_OK) {
            Debug::Log("Failed to initialize compression", Color::Red);
            throw std::runtime_error("Failed to initialize compression");
        }

        // Compress data
        if (deflate(&strm, Z_FINISH) != Z_STREAM_END) {
            deflateEnd(&strm);
            Debug::Log("Failed to compress data", Color::Red);
            throw std::runtime_error("Failed to compress data");
        }

        dest_size = strm.total_out; // Get the actual size after compression
        deflateEnd(&strm);

        // Resize the compressed data vector to its actual size after compression
        compressed_data.resize(dest_size);
        // Write the number of elements in the compressed block as a header
        record_file.write((char*)&source_count, HEADER_SIZE);
        // Write the size of the compressed block as a header
        record_file.write((char*)&dest_size, HEADER_SIZE);
        // Write the compressed data
        record_file.write((char*)compressed_data.data(), compressed_data.size());
#else
        record_file.write((const char *) &writing_buffer[0], writing_buffer.size() * sizeof(T));
#endif

        if (record_file.bad())
            Debug::Log("An error occurred trying to write to file: " + file_name, Color::Red);

        record_file.close();
    } else {
        Debug::Log("An error occurred trying to open file: " + file_name, Color::Red);
    }

    writing_buffer.clear();
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    float duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.0f;

    if (std::abs(current_time - last_time) > 0.0001f) {
        last_time = current_time;
    }

    if(std::is_same_v<T,SoundDTO>)
        writing_sound_to_file = false;
    if(std::is_same_v<T,TransformDTO>)
        writing_transform_to_file = false;
    if(std::is_same_v<T,GenericDTO>)
        writing_generic_to_file = false;

    if(debug)
        Debug::Log("Writing chunks into the recording file took: " + std::to_string(duration) + " ms");

    return true;
}

bool Recorder::record_generic(float time_stamp, int id, int *int_a, float *float_a, char *char_a) {
    try {
        if(current_recording.state != RecordingState::Record)
            return false;

        if (time_stamp < 0.0f)
            return false;

        //std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();

        current_recording.meta_information.update_recording_time(time_stamp);
        current_recording.meta_information.add_generic_id_information(id);

        GenericDTO generic_dto{id, time_stamp};

        std::copy(int_a, int_a + num_c, generic_dto.i);
        std::copy(float_a, float_a + num_c, generic_dto.f);
        std::copy(char_a, char_a + char_c, generic_dto.c);

        bool recording_buffer_filled = generic_recording_buffer.size() >= generic_buffer_size - 1;

        int i = 0;
        while (recording_buffer_filled && writing_generic_to_file) {
            if (i % wait_count == 0 && debug)
                Debug::Log("Waiting until the current generic data is written to file before recording new data.");
            i++;
        }

        generic_recording_buffer.push_back(generic_dto);

        if (recording_buffer_filled) {
            generic_writing_buffer = std::move(generic_recording_buffer);
            generic_recording_buffer.reserve(generic_buffer_size);
        }

        if (!generic_writing_buffer.empty() && !writing_generic_to_file) {
            writing_generic_to_file = true;
#if (MULTI_THREADED == 1)
            RecorderManager::getThreadPool().submit_task([this]() { this->write_buffer<GenericDTO>(this->current_recording.generic_file, this->generic_writing_buffer); });
#else
            write_buffer<genericDTO>(current_recording.generic_file, generic_writing_buffer);
#endif
        }


        //std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        //if(std::abs(current_time - last_time) > 0.0001f){
        //    record_generic.add_measurement();
        //    last_time = current_time;
        //}
        //record_generic += end - begin;

    } catch (std::exception &e) {
        Debug::Log("A problem occurred trying to record the generic for object with uuid " + std::to_string(id) + ". " +e.what(), Color::Red);
        return false;
    }
    return true;
}

bool Recorder::get_generic(float time_stamp, int id, int *int_a, float *float_a, char *char_a) {
    try {
        if(current_recording.state != RecordingState::Replay)
            return false;

        //std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
        current_time = time_stamp;

        if (!current_recording.meta_information.time_contained_in_recording(current_time)) {
            return false;
        }

        if(generic_ring_buffer != nullptr) {

            //if (generic_replaying && !generic_recording && generic_ring_buffer != nullptr &&
            //    !generic_ring_buffer->preparing_buffers() && generic_ring_buffer->loading_required() && !generic_ring_buffer->is_currently_loading()) {
            //    Debug::Log("generic data load task submitted to thread pool.");
            //    RecorderManager::getThreadPool().push_task(&GenericRingBuffer::read_data, generic_ring_buffer, current_generic_recording_file);
            //}

            bool result = generic_ring_buffer->prepare_buffers(time_stamp);
            if (!result) {
                Debug::Log("Could not load the generic data for object with id " + std::to_string(id), Color::Red);
                return false;
            }

            if (!generic_ring_buffer->preparing_buffers() && generic_ring_buffer->loading_required() && !generic_ring_buffer->is_currently_loading()) {
                if(debug)
                    Debug::Log("generic data load task submitted to thread pool.");
#if (MULTI_THREADED == 1)
                generic_loading = RecorderManager::getThreadPool().submit_task([this]{generic_ring_buffer->read_data(current_recording.generic_file);});
#else
                generic_ring_buffer->read_data(current_recording.generic_file);
#endif
            }

            result = generic_ring_buffer->data_at_time(id, time_stamp, int_a, float_a, char_a);
            if (!result) {
                Debug::Log("Could not retrieve the generic data for object with id " + std::to_string(id), Color::Red);
            }

            //std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            //if(std::abs(current_time - last_time) > 0.0001f){
            //    get_generic.add_measurement();
            //    last_time = current_time;
            //}
            //get_generic += end - begin;

            return result;
        }
    } catch (std::exception &e) {
        Debug::Log(e.what(), Color::Red);
        return false;
    }
    return false;
}

Recorder::~Recorder() {
    Debug::Log("Destructor of Recorder called.");

    last_transforms.clear();

    Debug::Log("Recorder destroyed.");
}

Recorder::Recorder(Recorder &&recorder) {
    debug = recorder.debug;
    current_recording = recorder.current_recording;
    recording_buffer_size = recorder.recording_buffer_size;
    sound_recording_buffer_size = recorder.sound_recording_buffer_size;
    generic_buffer_size = recorder.generic_buffer_size;
    transform_buffer_num = recorder.transform_buffer_num;
    transform_buffer_time_interval = recorder.transform_buffer_time_interval;
    sound_buffer_num = recorder.sound_buffer_num;
    sound_buffer_time_interval = recorder.sound_buffer_time_interval;
    generic_buffer_num = recorder.generic_buffer_num;
    generic_buffer_time_interval = recorder.generic_buffer_time_interval;
}

Recorder &Recorder::operator=(Recorder &&recorder) {
    debug = recorder.debug;
    current_recording = recorder.current_recording;
    recording_buffer_size = recorder.recording_buffer_size;
    sound_recording_buffer_size = recorder.sound_recording_buffer_size;
    generic_buffer_size = recorder.generic_buffer_size;
    transform_buffer_num = recorder.transform_buffer_num;
    transform_buffer_time_interval = recorder.transform_buffer_time_interval;
    sound_buffer_num = recorder.sound_buffer_num;
    sound_buffer_time_interval = recorder.sound_buffer_time_interval;
    generic_buffer_num = recorder.generic_buffer_num;
    generic_buffer_time_interval = recorder.generic_buffer_time_interval;
    return *this;
}

int Recorder::get_original_id(char *object_name, int object_name_length, int object_id) {
    if(current_recording.state != RecordingState::Replay)
        return 0;

    std::string name = "";
    for (int i = 0; i < object_name_length; ++i)
        name += object_name[i];

    return current_recording.meta_information.get_old_uuid(name, object_id);
}

int Recorder::get_original_id(int object_id) {
    if(current_recording.state != RecordingState::Replay)
        return 0;

    return current_recording.meta_information.get_old_uuid(object_id);
}

bool Recorder::set_debug_mode(bool debug_state) {
    debug = debug_state;

    if(transform_ring_buffer != nullptr)
        transform_ring_buffer->set_debug_mode(debug_state);

    if(sound_ring_buffer != nullptr)
        sound_ring_buffer->set_debug_mode(debug_state);

    if(generic_ring_buffer != nullptr)
        generic_ring_buffer->set_debug_mode(debug_state);

    return true;
}

int Recorder::get_channel_num(int sound_origin) {
    if(current_recording.state != RecordingState::Replay)
        return -1;

    if (sound_ring_buffer != nullptr) {
        try {
            if (!sound_ring_buffer->preparing_buffers() && sound_ring_buffer->loading_required() && !sound_ring_buffer->is_currently_loading()) {
#if (MULTI_THREADED == 1)
                sound_loading = RecorderManager::getThreadPool().submit_task([this]{sound_ring_buffer->read_data(current_recording.sound_file);});
#else
                sound_ring_buffer->read_data(current_recording.sound_file);
#endif
                Debug::Log("Dispatched thread to load sound data for channel retrieval.");
            }

            sound_loading.wait();
            return sound_ring_buffer->get_channel_num(sound_origin);
        } catch (std::exception &e) {
            Debug::Log("Getting sound exception for origin " + std::to_string(sound_origin) + " : " + e.what(), Color::Red);
            return -1;
        }
    }
    return -1;
}

int Recorder::get_new_id(int old_object_id) {
    if(current_recording.state != RecordingState::Replay)
        return 0;

    return current_recording.meta_information.get_new_uuid(old_object_id);
}

float Recorder::get_recording_start_global_timeoffset() {
    if(current_recording.state != RecordingState::Replay)
        return -1;

    return current_recording.meta_information.get_global_time_offset();
}

std::map<int, std::string> Recorder::get_recording_gameobjects() {
    if(current_recording.state != RecordingState::Replay)
        return {};

    Debug::Log("Trying to get game objects needed for replay");
    return current_recording.meta_information.get_recording_gameobjects_map();
}

std::string Recorder::get_gameobject_components(std::string name) {
    if(current_recording.state != RecordingState::Replay)
        return "No information";

    return current_recording.meta_information.get_gameobject_components(name);
}

std::string Recorder::get_gameobject_mesh_path(std::string name) {
    if(current_recording.state != RecordingState::Replay)
        return "No information";

    return current_recording.meta_information.get_gameobject_mesh_path(name);
}

int Recorder::get_sound_ids(int* sound_ids, int max_id_count) {
    if(current_recording.state != RecordingState::Replay)
        return -1;

    std::set<int> ids = current_recording.meta_information.get_sound_ids();
    if(ids.size() < max_id_count){
        int i = 0;
        for(int id : ids) {
            sound_ids[i] = id;
            i+=1;
        }
        return ids.size();
    } else
        return -1;
}