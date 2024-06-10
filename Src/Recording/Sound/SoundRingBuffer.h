//
// Created by Mortiferum on 05.01.2022.
//

#ifndef RECORDINGPLUGIN_SOUNDRINGBUFFER_H
#define RECORDINGPLUGIN_SOUNDRINGBUFFER_H

#include "Recording/BaseStructure/RingBuffer.h"
#include "SoundBuffer.h"
#include "DebugLog.h"
#include <vector>
#include <map>
#include <iostream>
#include <chrono>
#include <fstream>

class SoundRingBuffer : public RingBuffer{
private:
    std::map<int, int> sampling_rates;
    std::map<int, int> channel_nums;
public:
    SoundRingBuffer(int buffer_num, float buffer_time, float record_duration);

    virtual ~SoundRingBuffer() {
        std::cout << "Destroying Sound Ring Buffer\n";
    };

    void log_loading_data_type() const override;

    void log_loading_finished() const override;

    std::shared_ptr<RecordableData> read_single_data_from_file(std::ifstream &record_file) override;

    std::shared_ptr<RecordableData> read_single_data_from_vector(std::vector<Bytef> const& data_vector, int& index) override;

    std::shared_ptr<RecordableData> duplicate_data(std::shared_ptr<RecordableData> data) override;

    void read_chunk_from_file(std::ifstream &record_file, std::shared_ptr<RecordableData> current_data[], int current_chunk_size) override;

    bool load_data(LoadingInformation const& load_info, std::shared_ptr<RecordableData> data) const override;

    void reload_index_maps() override;

    DataType get_recorded_data_type() const override;

    int get_sound_chunk_for_time(int sound_origin, float time_stamp,float* sound_data);

    int get_sound_chunk_for_time(int sound_origin, float time_stamp,float* sound_data, int* corresponding_go_id);

    int get_sampling_rate(int sound_origin);

    int get_channel_num(int sound_origin);

    int get_dto_size() override;

private:
    SoundDTO sound_dto[read_chunk_size];
    SoundDTO s_sound_dto;
};


#endif //RECORDINGPLUGIN_SOUNDRINGBUFFER_H
