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

#ifndef RECORDINGPLUGIN_TRANSFORMDATA_H
#define RECORDINGPLUGIN_TRANSFORMDATA_H

#include "Recording/BaseStructure/RecordedIdData.h"

#include "glm/mat4x4.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/matrix_interpolation.hpp"


struct TransformDTO{
    int id;
    float t;
    float lp[3];
    float lr[4];
    float ls[3];
    float gp[3];
    float gr[4];
    float gs[3];
    int act;
    int p_id;

    bool friend  operator==(TransformDTO const& lhs, TransformDTO const& rhs)
    {
        if(lhs.id != rhs.id || lhs.act != rhs.act || lhs.p_id != rhs.p_id)
            return false;

        float eps = 0.001f;

        if(std::abs(lhs.gp[0] - rhs.gp[0]) > eps || std::abs(lhs.gp[1] - rhs.gp[1]) > eps || std::abs(lhs.gp[2] - rhs.gp[2]) > eps)
            return false;

        if(std::abs(lhs.gr[0] - rhs.gr[0]) > eps || std::abs(lhs.gr[1] - rhs.gr[1]) > eps || std::abs(lhs.gr[2] - rhs.gr[2]) > eps || std::abs(lhs.gr[3] - rhs.gr[3]) > eps)
            return false;

        if(std::abs(lhs.gs[0] - rhs.gs[0]) > eps || std::abs(lhs.gs[1] - rhs.gs[1]) > eps || std::abs(lhs.gs[2] - rhs.gs[2]) > eps)
            return false;

        if(std::abs(lhs.lp[0] - rhs.lp[0]) > eps || std::abs(lhs.lp[1] - rhs.lp[1]) > eps || std::abs(lhs.lp[2] - rhs.lp[2]) > eps)
            return false;

        if(std::abs(lhs.lr[0] - rhs.lr[0]) > eps || std::abs(lhs.lr[1] - rhs.lr[1]) > eps || std::abs(lhs.lr[2] - rhs.lr[2]) > eps || std::abs(lhs.lr[3] - rhs.lr[3]) > eps)
            return false;

        if(std::abs(lhs.ls[0] - rhs.ls[0]) > eps || std::abs(lhs.ls[1] - rhs.ls[1]) > eps || std::abs(lhs.ls[2] - rhs.ls[2]) > eps)
            return false;

        return true;
    }

    std::string static dto_layout(){
        return "TransformDTO: int id;float t;float lp[3];float lr[4];float ls[3];float gp[3];float gr[4];float gs[3];int act;int p_id;int layer = 0;";
    }
};


class TransformData : public RecordableData {
public:
    glm::vec3 local_position{0.0f,0.0f,0.0f};
    glm::quat local_rotation{0.0f,0.0f,0.0f,0.0f};
    glm::vec3 local_scale{1.0f,1.0f,1.0f};
    glm::vec3 global_position{0.0f,0.0f,0.0f};
    glm::quat global_rotation{0.0f,0.0f,0.0f,0.0f};
    glm::vec3 global_scale{1.0f,1.0f,1.0f};
    int active = -1;
    int parent_uuid = -1;

    TransformData();

    ~TransformData() override;

    std::ofstream& write(std::ofstream& ofs) const override;

    std::ifstream& read(std::ifstream& ifs) override;

    TransformData(const float* transform_data, const int* object_information, float time, int id);

    TransformData(TransformDTO const& dto);

    TransformData(glm::vec3 local_pos, glm::quat local_rot, glm::vec3 local_sca, int act, int parent);

    TransformData(glm::vec3 local_pos, glm::quat local_rot, glm::vec3 local_sca, glm::vec3 global_pos, glm::quat global_rot, glm::vec3 global_sca, int act, int parent);

    TransformData(TransformData const & data);

    TransformData(std::shared_ptr<TransformData> data);

    void update_matrix_and_object_info(float* matrix, int* object_information);

    glm::mat4 trs_matrix();

    TransformData interpolate(TransformData next_transform, float t);

    TransformData interpolate(std::shared_ptr<TransformData> next_transform, float t);

    const glm::vec3& get_local_position() const;

    const glm::quat& get_local_rotation() const;

    const glm::vec3& get_local_scale() const;

    const glm::vec3& get_global_position() const;

    const glm::quat& get_global_rotation() const;

    const glm::vec3& get_global_scale() const;

    int get_active() const;

    int get_parent_uuid() const;

    bool friend operator == (const TransformData& lhs, const TransformData& rhs)
    {
        return lhs.local_position.x == rhs.local_position.x && lhs.local_position.y == rhs.local_position.y && lhs.local_position.z == rhs.local_position.z &&
               lhs.local_rotation.x == rhs.local_rotation.x && lhs.local_rotation.y == rhs.local_rotation.y && lhs.local_rotation.z == rhs.local_rotation.z && lhs.local_rotation.w == rhs.local_rotation.w &&
               lhs.local_scale.x == rhs.local_scale.x && lhs.local_scale.y == rhs.local_scale.y && lhs.local_scale.z == rhs.local_scale.z &&
               lhs.active == rhs.active && lhs.parent_uuid == rhs.parent_uuid;
    }

    void write_to_stream(std::ofstream& file) const override;

    TransformDTO to_dto() const;
};

#endif //RECORDINGPLUGIN_TRANSFORMDATA_H
