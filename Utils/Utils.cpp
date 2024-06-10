//
// Created by Mortiferum on 24.11.2023.
//

#include "Utils.h"

void Utils::export_transform_data_to_CSV(std::string const& transform_file_path) {
    Debug::Log("Writing transform data to CSV file");
    std::string transform_data_file = transform_file_path;

    std::ifstream record_file(transform_data_file, std::ios::in | std::ios::binary);
    record_file.seekg(0, std::ifstream::end);
    unsigned long size = record_file.tellg() / sizeof(TransformDTO);

    Debug::Log("Recording transform file can be accessed. Number of transform chunks: " + std::to_string(size));
    TransformDTO current_data{};
    record_file.seekg(0, std::ifstream::beg);

    std::string file_ending = ".txt";
    std::string file_name = transform_data_file.replace(transform_data_file.find(file_ending), file_ending.length(),
                                                        "");
    std::fstream out(file_name + ".csv", std::fstream::in | std::fstream::out | std::fstream::app);

    out << "Time,ID,ParentID,ActiveState,LocPosX,LocPosY,LocPosZ,GloPosX,GloPosY,GloPosZ,LocScaX,LocScaY,LocScaZ,GloScaX,GloScaY,GloScaZ,LocRotX,LocRotY,LocRotZ,LocRotW,GloRotX,GloRotY,GloRotZ,GloRotW\n";

    for (unsigned long i = 0; i < size; i++) {
        record_file.read((char *) &current_data, sizeof(TransformDTO));
        out << current_data.t << ",";
        out << current_data.id << ",";
        out << current_data.p_id << ",";
        out << current_data.act << ",";

        out << current_data.lp[0] << "," << current_data.lp[1] << "," << current_data.lp[2] << ",";
        out << current_data.gp[0] << "," << current_data.gp[1] << "," << current_data.gp[2] << ",";

        out << current_data.ls[0] << "," << current_data.ls[1] << "," << current_data.ls[2] << ",";
        out << current_data.gs[0] << "," << current_data.gs[1] << "," << current_data.gs[2] << ",";

        out << current_data.lr[0] << "," << current_data.lr[1] << "," << current_data.lr[2] << ","
            << current_data.lr[3] << ",";
        out << current_data.gr[0] << "," << current_data.gr[1] << "," << current_data.gr[2] << ","
            << current_data.gr[3] << "\n";
    }


    out.close();
    record_file.close();


    Debug::Log("Writing transform data to CSV file finished");
}

void Utils::export_generic_data_to_CSV(const std::string &generic_file_path) {
    Debug::Log("Writing arbitrary  data to CSV file");
    std::string generic_data_file = generic_file_path;

    std::ifstream record_file(generic_data_file, std::ios::in | std::ios::binary);
    record_file.seekg(0, std::ifstream::end);
    unsigned long size = record_file.tellg() / sizeof(GenericDTO);

    Debug::Log("Recording arbitrary file can be accessed. Number of arbitrary chunks: " + std::to_string(size));
    GenericDTO current_data{};
    record_file.seekg(0, std::ifstream::beg);

    std::string file_ending = ".txt";
    std::string file_name = generic_data_file.replace(generic_data_file.find(file_ending),file_ending.length(),"");
    std::fstream out(file_name + ".csv", std::fstream::in | std::fstream::out | std::fstream::app);

    out << "Time,ID,";
    for(int i = 0; i < num_c; ++i)
        out << "Int" << i << ",";
    for(int i = 0; i < num_c; ++i)
        out << "Float" << i << ",";
    for(int i = 0; i < char_c; ++i)
        out << "Char" << i << ",";
    out << "\n";

    for (unsigned long i = 0; i < size; i++) {
        record_file.read((char *) &current_data, sizeof(GenericDTO));
        out << current_data.t << ",";
        out << current_data.id << ",";

        for(int i : current_data.i)
            out << i << ",";

        for(float i : current_data.f)
            out << i << ",";

        for(char i : current_data.c)
            out << i << ",";
        out << "\n";
    }


    out.close();
    record_file.close();
}

void Utils::export_sound_data_to_WAV(std::string const& sound_file_path) {
    Debug::Log("Writing sound data to WAV file");

    std::string sound_data_file = sound_file_path;
    //std::set<int> sound_ids = current_recording.meta_information.get_sound_ids();

    std::ifstream record_file(sound_data_file, std::ios::in | std::ios::binary);
    record_file.seekg(0, std::ifstream::beg);
#if (COMPRESSION == 1)
    // Create a buffer for the compressed data
        std::vector<Bytef> compressed_data;
        // Create a buffer for the decompressed data
        std::vector<Bytef> decompressed_data;

        compressed_data.reserve(100000);
        decompressed_data.reserve(100000);

        uint32_t chunk_compressed_size;
        uint32_t chunk_dto_count;

        bool finished_loading_for_buffer = false;

        for(auto id : sound_ids) {
            AudioFile<float> audioFile;
            AudioFile<float>::AudioBuffer buffer;
            int sampling_rate = -1;
            int channel_num = -1;

            unsigned long int total_size = 0;
            float duration = 0.0f;

            z_stream strm;
            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;

            if (inflateInit(&strm) != Z_OK) {
                throw std::runtime_error("Failed to initialize decompression");
            }
            record_file.clear();
            record_file.seekg(0, std::ifstream::beg);
            while (!record_file.eof()) {
                std::streampos read_position = record_file.tellg();
                // Read the number of DTO elements of the next compressed block from the header
                record_file.read((char *) &chunk_dto_count, HEADER_SIZE);
                // Read the size of the next compressed block from the header
                record_file.read((char *) &chunk_compressed_size, HEADER_SIZE);

                if (record_file.gcount() != HEADER_SIZE) {
                    break;  // End of file or read error
                }

                // Resize the compressed_data buffer and read the compressed block
                compressed_data.resize(chunk_compressed_size);
                record_file.read((char *) compressed_data.data(), chunk_compressed_size);

                decompressed_data.resize(chunk_dto_count * sizeof(SoundDTO));

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
                        break;
                    case Z_STREAM_END:
                        // End of the compressed stream. This might be expected in some cases.
                        inflateReset(&strm);
                        break;
                    case Z_NEED_DICT:
                        throw std::runtime_error("Decompression failed: Need dictionary");
                    case Z_ERRNO:
                        throw std::runtime_error("Decompression failed: File error");
                    case Z_STREAM_ERROR:
                        throw std::runtime_error(
                                "Decompression failed: Stream state was inconsistent (e.g., next_in or next_out was nullptr)");
                    case Z_DATA_ERROR:
                        throw std::runtime_error("Decompression failed: The data was corrupted or incomplete");
                    case Z_MEM_ERROR:
                        throw std::runtime_error("Decompression failed: Insufficient memory");
                    case Z_BUF_ERROR:
                        throw std::runtime_error(
                                "Decompression failed: No progress was possible or there was not enough room in the output buffer");
                    case Z_VERSION_ERROR:
                        throw std::runtime_error(
                                "Decompression failed: The version of zlib.h and the version of the library linked do not match");
                    default:
                        throw std::runtime_error("Decompression failed: Unknown error");
                }
                // Now, decompressed_data contains the decompressed data.

                int index = 0;
                for (int i = 0; i < chunk_dto_count; ++i) {
                    // Ensure there's enough data left to read a TransformDTO
                    if (index + sizeof(SoundDTO) > decompressed_data.size()) {
                        throw std::runtime_error("Not enough data left in the vector");
                    }

                    // Get a pointer to the current location in the vector
                    const auto *dto_ptr = reinterpret_cast<const SoundDTO *>(&decompressed_data[index]);

                    // Create a std::make_shared<TransformData> object from the DTO
                    std::shared_ptr<RecordableData> result = std::make_shared<SoundData>(*dto_ptr);

                    if (dto_ptr->id == id) {
                        if(dto_ptr->s_r > 0 && dto_ptr->c_n > 0) {
                            total_size += dto_ptr->s_n;
                            sampling_rate = dto_ptr->s_r;
                            channel_num = dto_ptr->c_n;
                            duration = dto_ptr->e_t;
                        }
                    }

                    // Advance the current_index by the size of the DTO
                    index += sizeof(SoundDTO);
                }

                compressed_data.clear();
                decompressed_data.clear();
            }

            inflateEnd(&strm);

            //int channel_sample_num = total_size / channel_num;
            int channel_sample_num = (int)std::ceil(sampling_rate * duration);

            Debug::Log("Number of samples that will be written to the WAV file: " + std::to_string(channel_sample_num));
            Debug::Log("Sound id: " + std::to_string(id) + ", Channel num: " + std::to_string(channel_num) + ", Sampling rate: " + std::to_string(sampling_rate));

            audioFile.setSampleRate(sampling_rate);
            audioFile.setBitDepth(24);
            // number of channels in the audio file
            buffer.resize(channel_num);
            // number of samples for the current channel
            for(int i = 0; i < channel_num; ++i) {
                buffer[i].resize(channel_sample_num);
            }

            Debug::Log("WAV file buffer created");

            int current_index = 0;
            std::vector<SoundDTO> sound_buffer;

            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;

            if (inflateInit(&strm) != Z_OK) {
                throw std::runtime_error("Failed to initialize decompression");
            }

            bool first = true;
            std::ofstream write_file{sound_data_file + std::to_string(id), std::ios_base::app | std::ios_base::out | std::ios::binary};
            record_file.clear();
            record_file.seekg(0, std::ifstream::beg);
            while (!record_file.eof()) {
                std::streampos read_position = record_file.tellg();
                // Read the number of DTO elements of the next compressed block from the header
                record_file.read((char *) &chunk_dto_count, HEADER_SIZE);
                // Read the size of the next compressed block from the header
                record_file.read((char *) &chunk_compressed_size, HEADER_SIZE);

                if (record_file.gcount() != HEADER_SIZE) {
                    break;  // End of file or read error
                }

                // Resize the compressed_data buffer and read the compressed block
                compressed_data.resize(chunk_compressed_size);
                record_file.read((char *) compressed_data.data(), chunk_compressed_size);

                decompressed_data.resize(chunk_dto_count * sizeof(SoundDTO));

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
                        break;
                    case Z_STREAM_END:
                        // End of the compressed stream. This might be expected in some cases.
                        inflateReset(&strm);
                        break;
                    case Z_NEED_DICT:
                        throw std::runtime_error("Decompression failed: Need dictionary");
                    case Z_ERRNO:
                        throw std::runtime_error("Decompression failed: File error");
                    case Z_STREAM_ERROR:
                        throw std::runtime_error(
                                "Decompression failed: Stream state was inconsistent (e.g., next_in or next_out was nullptr)");
                    case Z_DATA_ERROR:
                        throw std::runtime_error("Decompression failed: The data was corrupted or incomplete");
                    case Z_MEM_ERROR:
                        throw std::runtime_error("Decompression failed: Insufficient memory");
                    case Z_BUF_ERROR:
                        throw std::runtime_error(
                                "Decompression failed: No progress was possible or there was not enough room in the output buffer");
                    case Z_VERSION_ERROR:
                        throw std::runtime_error(
                                "Decompression failed: The version of zlib.h and the version of the library linked do not match");
                    default:
                        throw std::runtime_error("Decompression failed: Unknown error");
                }
                // Now, decompressed_data contains the decompressed data.

                int index = 0;
                for (int i = 0; i < chunk_dto_count; ++i) {
                    // Ensure there's enough data left to read a TransformDTO
                    if (index + sizeof(SoundDTO) > decompressed_data.size()) {
                        throw std::runtime_error("Not enough data left in the vector");
                    }

                    // Get a pointer to the current location in the vector
                    const auto *dto_ptr = reinterpret_cast<const SoundDTO *>(&decompressed_data[index]);

                    if (dto_ptr->id == id) {
                        sound_buffer.push_back(*dto_ptr);

                        for (int k = 0; k < dto_ptr->s_n; k++) {
                            if (k + dto_ptr->s_i < 4800 && current_index < channel_sample_num) {
                                buffer[k % channel_num][current_index] = dto_ptr->s_d[k + dto_ptr->s_i];
                                if(k % channel_num == channel_num - 1) {
                                    current_index++;
                                }
                            } else if(current_index >= channel_sample_num){
                                Debug::Log("Error! Cannot add more samples!", Color::Red);
                                break;
                            } else {
                                Debug::Log("Error! Invalid sound data! Trying to access data at index " + std::to_string(k + dto_ptr->s_i), Color::Red);
                                break;
                            }
                        }
                    }

                    // Advance the current_index by the size of the DTO
                    index += sizeof(SoundDTO);
                }

                compressed_data.clear();
                decompressed_data.clear();

                if (finished_loading_for_buffer)
                    break;
            }

            inflateEnd(&strm);
            if(sound_buffer.size() > 0)
                write_file.write((char *) &sound_buffer[0], sound_buffer.size() * sizeof(SoundDTO));
            write_file.close();

            Debug::Log("Finished reading the sound recording file");

            bool ok = audioFile.setAudioBuffer(buffer);

            if (!ok) {
                Debug::Log("Could not set the audio buffer for the WAV file creation!", Color::Red);
            }

            audioFile.save(current_recording.sound_file + "_" + std::to_string(id) + ".wav");
            Debug::Log("WAV file for sound origin " + std::to_string(id) + " was created");

            buffer.clear();
        }
#else
    if(!record_file.good()) {
        Debug::Log("Sound file could not be accessed!", Color::Red);
        return;
    }

    record_file.seekg(0, std::ifstream::end);
    unsigned long size = record_file.tellg() / sizeof(SoundDTO);

    Debug::Log("Recording sound file can be accessed. Number of sound chunks: " + std::to_string(size));
    SoundDTO current_data;
    record_file.seekg(0, std::ifstream::beg);

    std::set<int> sound_origins;
    std::vector<int> sound_chunk_count(100, 0);

    for (unsigned long i = 0; i < size; i++) {
        record_file.read((char *) &current_data, sizeof(SoundDTO));
        sound_origins.insert(current_data.id);
        if(current_data.id >= sound_chunk_count.size()){
            Debug::Log("Error! Sound data seems to be corrupted! Detected sound with id: " + std::to_string(current_data.id), Color::Red);
            record_file.close();
            return;
        }
        sound_chunk_count[current_data.id] += 1;
    }

    Debug::Log("Detected " + std::to_string(sound_origins.size()) + " different audio sources.");

    for (auto j : sound_origins) {
        AudioFile<float> audioFile;
        AudioFile<float>::AudioBuffer buffer;

        record_file.seekg(0, std::ifstream::beg);

        int sampling_rate = -1;
        int channel_num = -1;

        unsigned long int total_size = 0;
        float duration = 0.0f;

        for (unsigned long i = 0; i < size; i++) {
            record_file.read((char *) &current_data, sizeof(SoundDTO));
            if (current_data.id == j) {
                if(current_data.s_r > 0 && current_data.c_n > 0) {
                    total_size += current_data.s_n;
                    sampling_rate = current_data.s_r;
                    channel_num = current_data.c_n;
                    duration = current_data.e_t;
                }
            }
        }

        //int channel_sample_num = total_size / channel_num;
        int channel_sample_num = (int)std::ceil(sampling_rate * duration);

        Debug::Log("Number of samples that will be written to the WAV file: " + std::to_string(channel_sample_num));
        Debug::Log("Sound id: " + std::to_string(j) + ", Channel num: " + std::to_string(channel_num) + ", Sampling rate: " + std::to_string(sampling_rate));

        audioFile.setSampleRate(sampling_rate);
        audioFile.setBitDepth(24);
        // number of channels in the audio file
        buffer.resize(channel_num);
        // number of samples for the current channel
        for(int i = 0; i < channel_num; ++i) {
            buffer[i].resize(channel_sample_num);
        }

        Debug::Log("WAV file buffer created");

        int current_index = 0;
        record_file.seekg(0, std::ifstream::beg);
        std::vector<SoundDTO> sound_buffer;

        bool first = true;
        std::ofstream write_file{sound_data_file + std::to_string(j), std::ios_base::app | std::ios_base::out | std::ios::binary};
        for (unsigned long i = 0; i < size; i++) {
            record_file.read((char *) &current_data, sizeof(SoundDTO));
            if (current_data.id == j) {
                sound_buffer.push_back(current_data);

                //int new_index = (int)std::round(current_data.s_t * current_data.s_r);
                //if(new_index < current_index) {
                //    new_index = current_index;
                //} else if(new_index - current_index < 1000){
                //    current_index = current_index;
                //} else {
                //    Debug::Log("Current index: " + std::to_string(current_index) + ", new index: " + std::to_string(new_index), Color::Red);
                //    current_index = new_index;
                //}
                //if(first) {
                //    for (int k = 0; k < current_index; k++)
                //        for(int j = 0; j < channel_num; ++j)
                //            buffer[j][k] = 0.0f;
                //    first = false;
                //}

                //std::cout << "Current chunk start: " << current_chunk.start_time << ", end: " << current_chunk.end_time << "\n";
                for (int k = 0; k < current_data.s_n; k++) {
                    if (k + current_data.s_i < 4800 && current_index < channel_sample_num) {
                        buffer[k % channel_num][current_index] = current_data.s_d[k + current_data.s_i];
                        if(k % channel_num == channel_num - 1) {
                            current_index++;
                        }
                    } else if(current_index >= channel_sample_num){
                        Debug::Log("Error! Cannot add more samples!", Color::Red);
                        break;
                    } else {
                        Debug::Log("Error! Invalid sound data! Trying to access data at index " + std::to_string(k + current_data.s_i), Color::Red);
                        break;
                    }
                }
            }
        }

        write_file.write((char *) &sound_buffer[0], sound_buffer.size() * sizeof(SoundDTO));
        write_file.close();

        Debug::Log("Finished reading the sound recording file");

        bool ok = audioFile.setAudioBuffer(buffer);

        if (!ok) {
            Debug::Log("Could not set the audio buffer for the WAV file creation!", Color::Red);
        }

        audioFile.save(sound_file_path + "_" + std::to_string(j) + ".wav");
        Debug::Log("WAV file for sound origin " + std::to_string(j) + " was created");

        buffer.clear();
    }
#endif
    record_file.close();
}