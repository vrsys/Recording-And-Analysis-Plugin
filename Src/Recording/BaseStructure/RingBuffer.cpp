//
// Created by Mortiferum on 18.03.2022.
//

#include "RingBuffer.h"
#include <bitset>

RingBuffer::~RingBuffer() {
    if (debug)
        Debug::Log("Destroying Ring Buffer");

    wait_for_loading();
    buffers.clear();

    for (auto checkpoint: checkpoints) {
        if (checkpoint != nullptr) {
            delete checkpoint;
            checkpoint = nullptr;
        }
    }

    checkpoints.clear();
    if (debug)
        Debug::Log("Destroying Ring Buffer successful");
}

bool RingBuffer::time_in_active_buffer(float current_time) const {
    return buffers[current_active_buffer]->contains_time(current_time);
}

bool RingBuffer::current_active_buffer_in_memory() const {
    return buffers[current_active_buffer]->stored_in_memory();
}

bool RingBuffer::loading_required() const {
    return requires_loading && !currently_loading;
}

float RingBuffer::get_max_stored_time() const {
    float max = -1.0f;
    for (auto &buffer: buffers) {
        if (buffer != nullptr && buffer->get_max_time() > max) {
            max = buffer->get_max_time();
        }
    }
    return max;
}

bool RingBuffer::update_active_buffer(float current_time) {
    //std::cout << "Current active buffer: " << current_active_buffer << ", current time: " << current_time << "\n";
    bool buffer_found = false;
    int original_buffer = current_active_buffer;
    for (int i = 0; i < buffer_num; i++) {
        current_active_buffer = (i + original_buffer) % buffer_num;
        if (time_in_active_buffer(current_time)) {
            if (original_buffer != current_active_buffer && debug) {
                Debug::Log("Buffer switch from buffer " + std::to_string(original_buffer) + " to " + std::to_string((i + original_buffer) % buffer_num));
            }
            buffer_found = true;
            break;
            //std::cout << "Current active buffer now " << current_active_buffer << "\n";
        }
    }

    if (!buffer_found) {
        current_active_buffer = 0;
        if (debug)
            Debug::Log("Current active buffer now " + std::to_string(current_active_buffer));
        return false;
    } else {
        return true;
    }
}

void RingBuffer::clear_old_buffers() {
    if (debug)
        Debug::Log("Clearing data of buffers that are not needed anymore");
    for (int i = 0; i < buffers.size(); i++) {
        if (!buffers[i]->stored_in_memory()) {
            buffers[i]->clear_buffer();
            if (debug)
                Debug::Log("Cleared data of buffer " + std::to_string(i));
        }
    }
}

RingBuffer::RingBuffer(float buffer_time, int buffer_number, float record_duration) : time_per_buffer(buffer_time),
                                                                                      buffer_num(buffer_number),
                                                                                      recording_duration(
                                                                                              record_duration) {
    current_active_buffer = 0;
}

int RingBuffer::find_buffer_for_time(float current_time) const {
    for (int i = 0; i < buffer_num; i++) {
        if (buffers[(i + current_active_buffer) % buffer_num]->contains_time(current_time)) {
            return (i + current_active_buffer) % buffer_num;
        }
    }
    return -1;
}

float RingBuffer::get_min_stored_time() const {
    float min = 99999.0f;
    for (auto &buffer: buffers) {
        if (buffer->get_min_time() < min) {
            min = buffer->get_min_time();
        }
    }
    return min;
}

bool RingBuffer::prepare_buffers(float current_time) {
    preparing_buffer = true;
    // if the current time is not stored in the current active buffer
    if (!time_in_active_buffer(current_time)) {
        if (debug)
            Debug::Log("Trying to find buffer that contains the required time.");
        bool found_required_buffer = update_active_buffer(current_time);
        if (!found_required_buffer) {
            if (debug) {
                Debug::Log("Could not find a buffer that contains time: " + std::to_string(current_time) +
                           ". Updating buffer timings");
            }
            update_buffer_timings(current_time);
            update_active_buffer(current_time);
        }
    }

    // the following is being done to avoid that the current active buffer is the last buffer
    // this is being done because for interpolation the next buffer may be needed to be in memory
    if (buffers[current_active_buffer]->get_max_time() < recording_duration &&
        buffers[current_active_buffer]->get_max_time() >
        buffers[(current_active_buffer + 1) % buffer_num]->get_max_time()) {
        if (debug)
            Debug::Log("Modifying buffer timings because buffer for time " + std::to_string(current_time) +
                       " would be last buffer");
        update_buffer_timings(current_time);
    }

    bool active_buffer_reload = false;
    auto start = std::chrono::high_resolution_clock::now();

    update_active_buffer(current_time);

    if (!current_active_buffer_in_memory()) {
        if (debug)
            Debug::Log("Current buffer not in memory!");
        active_buffer_reload = true;
    }

    preparing_buffer = false;

    bool load = false;
    for (auto const& buffer: buffers) {
        if (!buffer->stored_in_memory() && buffer->get_min_time() <= recording_duration) {
            if (debug)
                Debug::Log("Loading required. Time: " + std::to_string(current_time));
            load = true;
            break;
        }
    }

    requires_loading = load;

    return true;
}

void RingBuffer::update_buffer_timings(float required_time) {
    if (debug) {
        std::string output = "Required time not stored in current buffers.\nCurrent buffer timings: \n";
        for (int i = 0; i < buffers.size(); i++) {
            output += "Buffer " + std::to_string(i) + ", min time: " + std::to_string(buffers[i]->get_min_time()) +
                      ", max time: "
                      + std::to_string(buffers[i]->get_max_time()) + ", in memory: " +
                      std::to_string(buffers[i]->stored_in_memory()) + "\n";
        }

        Debug::Log(output);
    }

    int k = 0;

    while (currently_loading) {
        ++k;
        if (k % 10000 == 0 && debug)
            Debug::Log("Waiting with buffer preparation until loading is finished");
    }

    float min_time = (std::floor(required_time / time_per_buffer)) * time_per_buffer;
    if (buffer_num >= 3) {
        min_time = (std::floor(required_time / time_per_buffer) - 1) * time_per_buffer;
        if (min_time < 0.0f) {
            min_time = 0.0f;
        }
    }

    std::vector<float> min_times;

    // compute new buffer start times such that the buffer which is currently active is not the last buffer
    for (int i = 0; i < buffers.size(); ++i) {
        min_times.push_back(min_time + i * time_per_buffer);
    }

    // get the id of the last buffer which has been loaded for the corresponding min starting times (usually the last buffer)
    int buffer_loaded_index = 0;
    // the corresponding time index
    int min_time_index = 0;

    for (int i = 0; i < buffers.size(); i++) {
        for (int j = 0; j < min_times.size(); j++) {
            if (std::abs(buffers[i]->get_min_time() - min_times[j]) < 0.001f && buffers[i]->stored_in_memory()) {
                buffer_loaded_index = i;
                min_time_index = j;
            }
        }
    }

    if (debug)
        std::cout << "Buffer loaded index: " << buffer_loaded_index << " at min time: " << min_times[min_time_index]
                  << "\n";

    // if a buffer does not have the correct a start time that aligns with the expected start time or the buffer is not loaded modify the buffer
    for (int i = 0; i < buffers.size(); i++) {
        if (std::abs(buffers[(i + buffer_loaded_index) % buffer_num]->get_min_time() -
                     min_times[(i + min_time_index) % buffer_num]) > 0.001f ||
            !buffers[(i + buffer_loaded_index) % buffer_num]->stored_in_memory()) {
            buffers[(i + buffer_loaded_index) % buffer_num]->set_min_time(min_times[(i + min_time_index) % buffer_num]);
            buffers[(i + buffer_loaded_index) % buffer_num]->set_max_time(
                    min_times[(i + min_time_index) % buffer_num] + time_per_buffer);
            buffers[(i + buffer_loaded_index) % buffer_num]->set_memory_status(false);
        }
    }

    current_active_buffer = find_buffer_for_time(required_time);

    if (debug) {
        std::string output = "Buffer times updated. Currently active buffer " + std::to_string(current_active_buffer) +
                             "\nCurrent buffer timings: \n";
        for (int i = 0; i < buffers.size(); i++) {
            output += "Buffer " + std::to_string(i) + ", min time: " + std::to_string(buffers[i]->get_min_time()) +
                      ", max time: "
                      + std::to_string(buffers[i]->get_max_time()) + ", in memory: " +
                      std::to_string(buffers[i]->stored_in_memory()) + "\n";
        }

        Debug::Log(output);
    }
}

bool RingBuffer::preparing_buffers() const {
    return preparing_buffer;
}

bool RingBuffer::read_data(std::string const &current_recording_file) {
    if (currently_loading)
        return false;

    currently_loading = true;
    DataType data_type = get_recorded_data_type();

    if (debug) {
        Debug::Log("Reading into memory started. Recording file: " + current_recording_file);
        log_loading_data_type();
    }

    std::ifstream record_file(current_recording_file, std::ios::in | std::ios::binary);

    // clear data of all buffers that are not needed anymore
    clear_old_buffers();

    LoadingInformation load_info{get_min_stored_time(), get_max_stored_time(), get_min_loading_time(),
                                 get_max_loading_time(), std::vector<bool>(buffers.size(), false)};

    Checkpoint *last_checkpoint = get_most_recent_checkpoint(load_info);

    already_read_elements = 0;
    if (last_checkpoint != nullptr) {
        if (debug)
            Debug::Log("Found previous checkpoint at time: " + std::to_string(last_checkpoint->time)
                       + " for current min contained time: " + std::to_string(load_info.min_contained_time) +
                       ", read lines: "
                       + std::to_string(last_checkpoint->already_read_elements), Color::Red);
        already_read_elements = last_checkpoint->already_read_elements;

        for (auto const& kv: last_checkpoint->last_seen_data) {
            load_info.add_create_new_data(kv.second, data_type);
            load_info.add_last_data(kv.second, data_type);
        }

        for (int i = 0; i < buffer_num; ++i) {
            std::shared_ptr<RecordableData>last_data = nullptr;
            for (auto const& kv: load_info.last_id_data) {
                last_data = duplicate_data(kv.second);
                buffers[i]->add_last_element(last_data);
            }
        }
    } else {
        if (debug)
            Debug::Log("Could not find a checkpoint");
    }

    if (debug) {
        Debug::Log("Min contained time: " + std::to_string(load_info.min_contained_time) + ", max contained time: " + std::to_string(load_info.max_contained_time));
        Debug::Log("Min loading time: " + std::to_string(load_info.min_loading_time) + ", max loading time: " + std::to_string(load_info.max_loading_time));
    }

#if (COMPRESSION == 1)
    if(debug)
        Debug::Log("Starting to read recording file at chunk: " + std::to_string(already_read_elements));

    record_file.seekg(already_read_elements);

    if(debug)
        Debug::Log("Trying to allocated vectors for decompression.");

    // Create a buffer for the compressed data
    std::vector<Bytef> compressed_data;
    // Create a buffer for the decompressed data
    std::vector<Bytef> decompressed_data;

    if(debug)
        Debug::Log("Vectors for decompression created.");

    compressed_data.reserve(100000);
    decompressed_data.reserve(100000);

    if(debug)
        Debug::Log("Vectors for decompression now have reserved memory.");

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    if (inflateInit(&strm) != Z_OK) {
        Debug::Log("Failed to initialize decompression!", Color::Red);
        throw std::runtime_error("Failed to initialize decompression");
    } else {
        if(debug)
            Debug::Log("Decompression initialised.");
    }

    uint32_t chunk_compressed_size;
    uint32_t chunk_dto_count;

    bool finished_loading_for_buffer = false;

    while (!record_file.eof()) {
        std::streampos read_position = record_file.tellg();
        // Read the number of DTO elements of the next compressed block from the header
        record_file.read((char*)&chunk_dto_count, HEADER_SIZE);
        // Read the size of the next compressed block from the header
        record_file.read((char*)&chunk_compressed_size, HEADER_SIZE);

        if(debug) {
            Debug::Log("Size of compression header: " + std::to_string(sizeof(HEADER_SIZE)));
            Debug::Log("Number of DTOs in current compression chunk: " + std::to_string(chunk_dto_count));
            Debug::Log("Number of bytes in current compression chunk: " + std::to_string(chunk_compressed_size));

            const unsigned char* bytePtr = reinterpret_cast<const unsigned char*>(&chunk_dto_count);
            std::string binaryString;
            for (std::size_t i = 0; i < sizeof(uint32_t); ++i) {
                binaryString += std::bitset<8>(bytePtr[i]).to_string();
                if (i < sizeof(uint32_t) - 1) {
                    binaryString += ' ';  // Add a space between byte values
                }
            }
            Debug::Log("Binary string: " + binaryString);
        }

        if (record_file.gcount() != HEADER_SIZE) {
            break;  // End of file or read error
        }

        // Resize the compressed_data buffer and read the compressed block
        compressed_data.resize(chunk_compressed_size);
        record_file.read((char*)compressed_data.data(), chunk_compressed_size);

        if(debug) {
            Debug::Log("Reading compressed data finished.");
        }

        decompressed_data.resize(chunk_dto_count * get_dto_size());

        if(debug) {
            Debug::Log("Resizing vector for storing decompressed data finsihed.");
        }

        // Set up the zlib stream for decompression
        strm.avail_in = chunk_compressed_size;
        strm.next_in = compressed_data.data();
        strm.avail_out = decompressed_data.size();
        strm.next_out = decompressed_data.data();

        // Decompress the data
        int ret = inflate(&strm, Z_NO_FLUSH);
        switch (ret) {
            case Z_OK:
                // All is well, continue as normal
                if(debug)
                    Debug::Log("Decompression worked.", Color::Red);
                break;
            case Z_STREAM_END:
                // End of the compressed stream. This might be expected in some cases.
                if(debug)
                    Debug::Log("Z_STREAM_END, Resetting inflate.", Color::Red);
                inflateReset(&strm);
                break;
            case Z_NEED_DICT:
                Debug::Log("Decompression failed: Need dictionary.", Color::Red);
                throw std::runtime_error("Decompression failed: Need dictionary");
            case Z_ERRNO:
                Debug::Log("Decompression failed: File error.", Color::Red);
                throw std::runtime_error("Decompression failed: File error");
            case Z_STREAM_ERROR:
                Debug::Log("Decompression failed: Stream state was inconsistent (e.g., next_in or next_out was nullptr).", Color::Red);
                throw std::runtime_error("Decompression failed: Stream state was inconsistent (e.g., next_in or next_out was nullptr)");
            case Z_DATA_ERROR:
                Debug::Log("Decompression failed: The data was corrupted or incomplete.", Color::Red);
                throw std::runtime_error("Decompression failed: The data was corrupted or incomplete");
            case Z_MEM_ERROR:
                Debug::Log("Decompression failed: Insufficient memory.", Color::Red);
                throw std::runtime_error("Decompression failed: Insufficient memory");
            case Z_BUF_ERROR:
                Debug::Log("Decompression failed: No progress was possible or there was not enough room in the output buffer.", Color::Red);
                throw std::runtime_error("Decompression failed: No progress was possible or there was not enough room in the output buffer");
            case Z_VERSION_ERROR:
                Debug::Log("Decompression failed: The version of zlib.h and the version of the library linked do not match.", Color::Red);
                throw std::runtime_error("Decompression failed: The version of zlib.h and the version of the library linked do not match");
            default:
                Debug::Log("Decompression failed: Unknown error.", Color::Red);
                throw std::runtime_error("Decompression failed: Unknown error");
        }
        // Now, decompressed_data contains the decompressed data.

        int index = 0;
        for(int i = 0; i < chunk_dto_count; ++i){
            std::shared_ptr<RecordableData>data = read_single_data_from_vector(decompressed_data, index);
            if (load_info.id_finished_loading.count(data->id) == 0) {
                bool result = add_element(data, load_info, read_position, data_type);
                if (!result || load_info.stop_reading) {
                    finished_loading_for_buffer = true;
                    break;
                }
            }
        }

        compressed_data.clear();
        decompressed_data.clear();

        if(finished_loading_for_buffer)
            break;
    }

    inflateEnd(&strm);
#else
    record_file.seekg(0, std::ifstream::end);
    unsigned long size = record_file.tellg() / get_dto_size();

    if (debug) {
        Debug::Log("Determining number of lines. " + std::to_string(size) + " lines found. Already read: " +
                   std::to_string(already_read_elements));
        Debug::Log("Trying to advance to line " + std::to_string(already_read_elements));
    }

    record_file.seekg(0, std::ifstream::beg);
    record_file.seekg(already_read_elements * get_dto_size());

    std::shared_ptr<RecordableData>current_data[read_chunk_size];

    int current_chunk_size = read_chunk_size;

    for (unsigned long i = already_read_elements; i + current_chunk_size < size; i += current_chunk_size) {
        read_chunk_from_file(record_file, current_data, current_chunk_size);

        bool added_element = false;

        if (debug)
            Debug::Log("Chunk reading start");

        float time = -1.0f;

        for (int j = 0; j < current_chunk_size; ++j) {
            int index = i + j;
            time = current_data[j]->time;
            if (load_info.id_finished_loading.count(current_data[j]->id) == 0) {
                if (get_recorded_data_type() == DataType::Sound && debug) {
                    Debug::Log("Trying to add sound with id: " + std::to_string(current_data[j]->id) + " for time: " +
                               std::to_string(current_data[j]->time));
                }

                bool result = add_element(current_data[j], load_info, index, data_type);
                if (result)
                    added_element = true;
            }
        }

        if (debug)
            Debug::Log("Chunk reading finished. Added element: " + std::to_string(added_element) + ", current time: " + std::to_string(time));

        if (!added_element)
            break;
    }

    if (load_info.max_read_line < size && !load_info.stop_reading) {
        for (unsigned long i = load_info.max_read_line + 1; i < size; ++i) {
            std::shared_ptr<RecordableData>data = nullptr;
            data = read_single_data_from_file(record_file);

            if (load_info.id_finished_loading.count(data->id) == 0) {
                if (get_recorded_data_type() == DataType::Sound && debug) {
                    Debug::Log("Trying to add sound with id: " + std::to_string(data->id) + " for time: " +
                               std::to_string(data->time));
                }

                bool result = add_element(data, load_info, i, data_type);
                if (!result || load_info.stop_reading)
                    break;
            }
        }
    }
#endif

    record_file.close();

    if (load_info.max_read_line != -1) {
        already_read_elements = load_info.max_read_line;
    }

    if (debug)
        Debug::Log("Now " + std::to_string(already_read_elements) + " lines have been read.");

    for (int i = 0; i < buffers.size(); i++) {
        if (!buffers[i]->stored_in_memory()) {
            buffers[i]->reload_index_maps();
            buffers[i]->set_memory_status(true);
        }
    }

    if (debug) {
        for (int i = 0; i < buffers.size(); i++) {
            Debug::Log("Buffer " + std::to_string(i) + ", in memory: " + std::to_string(buffers[i]->stored_in_memory()));
        }
    }

    // if last checkpoint is not the checkpoint that stores data for the minimum loading time
    if (last_checkpoint == nullptr || (last_checkpoint != nullptr && std::abs(last_checkpoint->time - load_info.min_loading_time) > 0.001f)) {
        checkpoint_creation(load_info);
        checkpoint_cleanup();
    }

    if (debug) {
        Debug::Log("Recording loaded into memory! Min stored time: " + std::to_string(load_info.min_stored_time)
                  + ", Max stored time: " + std::to_string(load_info.max_stored_time));
        log_loading_finished();
    }
    requires_loading = false;
    currently_loading = false;

    return true;
}

bool RingBuffer::add_element(std::shared_ptr<RecordableData>data, LoadingInformation &load_info, long long index, DataType data_type) {
    if (data == nullptr)
        return false;

    if (data_type == DataType::Sound && debug) {
        Debug::Log("Trying to add sound with id: " + std::to_string(data->id) + " for time: " +
                   std::to_string(data->time));
    }

    if (data->time > load_info.max_loading_time || data->time < 0.0f) {
        load_info.id_finished_loading.insert(data->id);
        load_info.stop_reading = true;
        if (debug) {
            Debug::Log("Stopped reading. Max time that was loaded: " + std::to_string(load_info.max_loading_time) +
                       ", Current time: " + std::to_string(data->time));
        }
        return false;
    }

    bool dont_load = !load_data(load_info, data);

    //load_info.add_last_data(data, data_type);

    for (int i = 0; i < buffer_num; ++i) {
        if(data->time >= buffers[i]->get_min_time())
            continue;
        std::shared_ptr<RecordableData> last_data = duplicate_data(data);
        buffers[i]->add_last_element(last_data);
    }

    if (dont_load) {
        load_info.add_new_data(data, index);
        return true;
    }

    int current_read_buffer = find_buffer_for_time(data->time);
    for (int i = 0; i < buffer_num; ++i) {
        // TODO: for non time sorted playback data this can result in errors during loading. This needs to be taken into account!
        if (data->time > buffers[i]->get_max_time() + 0.1f * time_per_buffer && !buffers[i]->stored_in_memory()) {
            buffers[i]->reload_index_maps();
            buffers[i]->set_memory_status(true);
        }
    }

    bool added = true;

    //for (int i = 0; i < buffer_num; ++i) {
    //    std::shared_ptr<RecordableData>last_data = nullptr;
    //    for (auto kv: load_info.last_id_data) {
    //        switch (data_type) {
    //            case RecordedData::Transform:
    //                last_data = std::make_shared<TransformData>{((std::shared_ptr<TransformData>) kv.second)};
    //                break;
    //            case RecordedData::Sound:
    //                last_data = std::make_shared<SoundData>{((std::shared_ptr<SoundData>) kv.second)};
    //                break;
    //            case RecordedData::Generic:
    //                last_data = std::make_shared<GenericData>{((std::shared_ptr<GenericData>) kv.second)};
    //                break;
    //        }
    //        buffers[i]->add_last_element(last_data);
    //    }
    //}

    if (current_read_buffer != -1 && !buffers[current_read_buffer]->stored_in_memory()) {
        bool result = buffers[current_read_buffer]->add_element(data);

        if (data_type == DataType::Sound && debug) {
            Debug::Log(
                    "Adding sound with id: " + std::to_string(data->id) + " for time: " + std::to_string(data->time) +
                    " into buffer: " + std::to_string(current_read_buffer));
        }

        if (!result) {
            Debug::Log("Error! Could not add data to the corresponding buffer!", Color::Red);
        } else {
            if (load_info.min_stored_time < 0.0f) {
                load_info.min_stored_time = data->time;
            }
            load_info.max_stored_time = data->time;

            if (load_info.id_finished_loading.empty())
                load_info.max_read_line = index;
        }
    } else {
        if (debug) {
            Debug::Log("Error! State should not be reached! Data time: " + std::to_string(data->time) +
                       ", active buffer: " + std::to_string(current_active_buffer));
            std::cout << "Current read buffer: " << current_read_buffer;
            if (current_read_buffer != -1)
                std::cout << " in memory: " << buffers[current_read_buffer]->stored_in_memory() << "\n";
            else
                std::cout << "\n";
        }
        added = false;
    }

    switch (data_type) {
        case DataType::Transform:
            break;
        case DataType::Sound: {
            int next_read_buffer = find_buffer_for_time(std::dynamic_pointer_cast<SoundData>(data)->end_time);
            if (current_read_buffer != next_read_buffer && next_read_buffer != -1 &&
                !buffers[next_read_buffer]->stored_in_memory()) {
                std::shared_ptr<SoundData>new_data = std::make_shared<SoundData>(std::dynamic_pointer_cast<SoundData>(data));
                new_data->time = new_data->end_time;
                bool result = buffers[next_read_buffer]->add_element(new_data);
                if (!result) {
                    Debug::Log("Error! Could not add data to the corresponding buffer!", Color::Red);
                }
            }

            break;
        }
        case DataType::Generic:
            break;
    }

    return true;
}

void RingBuffer::wait_for_loading() const {
    int i = 0;
    while (currently_loading) {
        i++;
        if (i % 10000 == 0) {
            if (debug) {
                DataType data_type = get_recorded_data_type();
                switch (data_type) {
                    case DataType::Sound:
                        Debug::Log("Waiting while sound data is loaded.");
                        break;
                    case DataType::Transform:
                        Debug::Log("Waiting while transform data is loaded.");
                        break;
                    case DataType::Generic:
                        Debug::Log("Waiting while generic data is loaded.");
                        break;
                }

                Debug::Log(
                        "--------------------------------------------------------------------------------------------------");
            }
            for (int j = 0; j < buffers.size(); ++j) {
                //NOTE: by uncommenting the following for loop the plugin goes into an endless loop
                //      a possible reason for this is that the buffer values are else not being refreshed?
                float min_t = buffers[j]->get_min_time();
                float max_t = buffers[j]->get_max_time();
                bool in_memory = buffers[j]->stored_in_memory();
                if (debug) {
                    Debug::Log("Buffer " + std::to_string(j) + ", min time: " + std::to_string(min_t) + ", max time: " +
                               std::to_string(max_t)
                               + ", in memory: " + std::to_string(in_memory));
                }
            }
            if (debug)
                Debug::Log(
                        "--------------------------------------------------------------------------------------------------");
        }
    }
}

void RingBuffer::wait_for_loading(float current_time) const {
    int i = 0;
    bool relevant_buffer_loaded = false;
    while (requires_loading) {
        // TODO: in order to increase performance the loop could be left when the required buffer is stored in memory
        //if(find_buffer_for_time(current_time) != -1 && buffers[find_buffer_for_time(current_time)]->stored_in_memory())
        //    break;

        if (relevant_buffer_loaded)
            return;

        i++;
        if (i % 10000 == 0) {
            if (debug) {
                DataType data_type = get_recorded_data_type();
                switch (data_type) {
                    case DataType::Sound:
                        Debug::Log("Waiting while sound data is loaded.");
                        break;
                    case DataType::Transform:
                        Debug::Log("Waiting while transform data is loaded.");
                        break;
                    case DataType::Generic:
                        Debug::Log("Waiting while generic data is loaded.");
                        break;
                }

                Debug::Log(
                        "--------------------------------------------------------------------------------------------------");
                Debug::Log("Current time: " + std::to_string(current_time) + ", Current active buffer: " +
                           std::to_string(current_active_buffer));
            }
            for (int j = 0; j < buffers.size(); ++j) {
                //NOTE: by uncommenting the following for loop the plugin goes into an endless loop
                //      a possible reason for this is that the buffer values are else not being refreshed?
                float min_t = buffers[j]->get_min_time();
                float max_t = buffers[j]->get_max_time();
                bool in_memory = buffers[j]->stored_in_memory();
                if (debug) {
                    Debug::Log("Buffer " + std::to_string(j) + ", min time: " + std::to_string(min_t) + ", max time: " +
                               std::to_string(max_t)
                               + ", in memory: " + std::to_string(in_memory));

                    for (auto &k: buffers[j]->loaded_data_for_ids) {
                        //if (k != nullptr)
                        //    Debug::Log("Loaded data for ID: " + std::to_string(k->get_id()));
                    }
                }

                if (min_t <= current_time && current_time < max_t && in_memory)
                    relevant_buffer_loaded = true;
            }
            if (debug)
                Debug::Log(
                        "--------------------------------------------------------------------------------------------------");
        }
    }
}

float RingBuffer::get_max_loading_time() const {
    float max = -1.0f;
    for (auto &buffer: buffers) {
        if (buffer != nullptr && !buffer->stored_in_memory() && buffer->get_max_time() > max) {
            max = buffer->get_max_time();
        }
    }
    return max;
}

float RingBuffer::get_min_loading_time() const {
    float min = 99999.0f;
    for (auto &buffer: buffers) {
        if (buffer != nullptr && !buffer->stored_in_memory() && buffer->get_min_time() < min) {
            min = buffer->get_min_time();
        }
    }
    return min;
}

void RingBuffer::set_debug_mode(bool debug_state) {
    debug = debug_state;

    if (!buffers.empty()) {
        for (int i = 0; i < buffer_num; ++i) {
            buffers[i]->set_debug_mode(debug_state);
        }
    }
}

bool RingBuffer::is_currently_loading() const {
    return currently_loading;
}

void RingBuffer::clear() {
    wait_for_loading();
    for (auto &buffer: buffers) {
        buffer->clear_buffer();
    }
}

void RingBuffer::checkpoint_creation(LoadingInformation const& load_info) {
    if (debug)
        Debug::Log("Creating checkpoint for time: " + std::to_string(load_info.min_loading_time) + ", read lines: " + std::to_string(load_info.min_read_line));
    Checkpoint *checkpoint = new Checkpoint{load_info.min_loading_time, load_info.min_read_line, load_info.new_id_data};
    checkpoints.push_back(checkpoint);
}

void RingBuffer::checkpoint_cleanup() {
    if (checkpoints.size() == max_checkpoint_num) {
        float min_t = 100000.0f;
        int min_ind = -1;
        for (int i = 0; i < checkpoints.size(); i++) {
            float t_dist = 0.0f;
            if (i > 0) {
                t_dist += std::abs(checkpoints[i - 1]->time - checkpoints[i]->time);
            }
            if (i < checkpoints.size() - 1) {
                t_dist += std::abs(checkpoints[i + 1]->time - checkpoints[i]->time);
            }
            if (t_dist < min_t) {
                min_t = t_dist;
                min_ind = i;
            }
        }

        if (min_ind != -1) {
            checkpoints.erase(checkpoints.begin() + min_ind);
        }
    }
}

Checkpoint *RingBuffer::get_most_recent_checkpoint(LoadingInformation const& load_info) const {
    Checkpoint* most_recent_checkpoint = nullptr;
    for (Checkpoint *checkpoint: checkpoints) {
        if (checkpoint != nullptr) {
            if (checkpoint->time <= load_info.min_contained_time) {
                if (most_recent_checkpoint == nullptr || (most_recent_checkpoint->time < checkpoint->time)) {
                    most_recent_checkpoint = checkpoint;
                }
            }
        }
    }
    return most_recent_checkpoint;
}