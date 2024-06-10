//
// Created by Mortiferum on 18.03.2022.
//

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
