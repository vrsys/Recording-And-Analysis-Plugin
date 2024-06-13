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
