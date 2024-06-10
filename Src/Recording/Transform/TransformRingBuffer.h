//
// Created by Mortiferum on 05.01.2022.
//

#ifndef RECORDINGPLUGIN_TRANSFORMRINGBUFFER_H
#define RECORDINGPLUGIN_TRANSFORMRINGBUFFER_H

#include "Recording/BaseStructure/RingBuffer.h"
#include "TransformData.h"
#include "TransformBuffer.h"
#include "DebugLog.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"

#include <map>
#include <chrono>
#include <ctime>

class TransformRingBuffer : public RingBuffer{
public:

    TransformRingBuffer(int buffer_num, float buffer_time, float record_duration);

    ~TransformRingBuffer() override {
        std::cout << "Destroying Transform Ring Buffer\n";
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

    bool transform_at_time(int uuid, float time, float* matrix, int* object_information);

    int next_positions(int uuid, float time_stamp, float *positions);

    int get_dto_size() override;

private:
    TransformDTO transform_dto[read_chunk_size];
    TransformDTO s_transform_dto;
};


#endif //RECORDINGPLUGIN_TRANSFORMRINGBUFFER_H
