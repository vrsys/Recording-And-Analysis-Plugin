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

#ifndef RECORDINGPLUGIN_GENERICDATA_H
#define RECORDINGPLUGIN_GENERICDATA_H

#include "Recording/BaseStructure/RecordedIdData.h"

#include "glm/mat4x4.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/matrix_interpolation.hpp"

static const int num_c = 10;
static const int char_c = 10;

struct GenericDTO{
    int id;
    float t;
    int i[num_c];
    float f[num_c];
    char c[char_c];

    std::string static dto_layout() {
        return "genericDTO: int id;float t;int i[10];float f[10];char c[10];int layer = 0;";
    }
};

class GenericData : public RecordableData {
public:
    int int_ar [num_c];
    float float_ar [num_c];
    char char_ar [char_c];

    GenericData();

    GenericData(GenericDTO const& dto);

    GenericData(float time, int id, int* i_arr, float* f_arr, char* c_arr);

    GenericData(const GenericData& data);

    GenericData(std::shared_ptr<GenericData> data);

    ~GenericData() override;

    std::ofstream& write(std::ofstream& ofs) const override;

    std::ifstream& read(std::ifstream& ifs) override;

    void update_data(int* i_arr, float* f_arr, char* c_arr);

    GenericData interpolate(const GenericData& next_data, float t);

    GenericData interpolate(const std::shared_ptr<GenericData> next_data, float t);

    void write_to_stream(std::ofstream& file) const override;

    GenericDTO to_dto() const;
};



#endif //RECORDINGPLUGIN_GENERICDATA_H
