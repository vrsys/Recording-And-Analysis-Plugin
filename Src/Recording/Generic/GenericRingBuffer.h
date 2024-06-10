//
// Created by Mortiferum on 05.01.2022.
//

#ifndef RECORDINGPLUGIN_GENERICRINGBUFFER_H
#define RECORDINGPLUGIN_GENERICRINGBUFFER_H

#include "GenericBuffer.h"
#include "Recording/BaseStructure/RingBuffer.h"
#include "DebugLog.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"

#include <map>
#include <chrono>
#include <ctime>
#include <fstream>
#include <filesystem>

class GenericRingBuffer : public RingBuffer{
public:

    GenericRingBuffer(int buffer_num, float buffer_time, float record_duration);

    virtual ~GenericRingBuffer() {
        std::cout << "Destroying Generic Ring Buffer\n";
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

    bool data_at_time(int uuid, float time, int* i_arr, float* f_arr, char* c_arr);

    int get_dto_size() override;

private:
    GenericDTO generic_dto[read_chunk_size];
    GenericDTO s_generic_dto;
};


#endif //RECORDINGPLUGIN_GENERICRINGBUFFER_H
