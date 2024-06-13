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

#include "TransformData.h"

std::ofstream& operator << (std::ofstream& ofs, glm::vec3 const& data)
{
    ofs << data.x << ' ' << data.y  << ' ' << data.z;
    return ofs;
}

std::ofstream& operator << (std::ofstream& ofs, glm::quat const& data)
{
    ofs << data.w  << ' ' << data.x << ' ' << data.y << ' ' << data.z;
    return ofs;
}

std::ifstream& operator >> (std::ifstream& ifs, glm::vec3& data)
{
    ifs >> data.x >> data.y >> data.z;
    return ifs;
}

std::ifstream& operator >> (std::ifstream& ifs, glm::quat& data)
{
    ifs >> data.w >> data.x >> data.y >> data.z;
    return ifs;
}

TransformData::TransformData(const float *transform_data, const int *object_information, float t, int uuid) : RecordableData(uuid, t){
    local_position.x = transform_data[0];
    local_position.y = transform_data[1];
    local_position.z = transform_data[2];
    local_rotation.w = transform_data[3];
    local_rotation.x = transform_data[4];
    local_rotation.y = transform_data[5];
    local_rotation.z = transform_data[6];
    local_scale.x = transform_data[7];
    local_scale.y = transform_data[8];
    local_scale.z = transform_data[9];
    global_position.x = transform_data[10];
    global_position.y = transform_data[11];
    global_position.z = transform_data[12];
    global_rotation.w = transform_data[13];
    global_rotation.x = transform_data[14];
    global_rotation.y = transform_data[15];
    global_rotation.z = transform_data[16];
    global_scale.x = transform_data[17];
    global_scale.y = transform_data[18];
    global_scale.z = transform_data[19];

    active = object_information[0];
    parent_uuid = object_information[1];
}

TransformData::TransformData(glm::vec3 const local_pos, glm::quat const local_rot, glm::vec3 const local_sca, int act,int parent)
        : RecordableData(-1, -1.0f), local_position(local_pos), local_rotation(local_rot), local_scale(local_sca) {
    active = act;
    parent_uuid = parent;
}

TransformData::TransformData(glm::vec3 const local_pos, glm::quat const local_rot, glm::vec3 const local_sca,
                             glm::vec3 const global_pos, glm::quat const global_rot, glm::vec3 const global_sca,
                             int act, int parent)
        : RecordableData(-1, -1.0f), local_position(local_pos), local_rotation(local_rot), local_scale(local_sca), global_position(global_pos),
          global_rotation(global_rot), global_scale(global_sca) {
    active = act;
    parent_uuid = parent;
}

TransformData::TransformData(TransformData const & data) : RecordableData(data.id, data.time){
    local_rotation = data.local_rotation;
    local_position = data.local_position;
    local_scale = data.local_scale;
    global_position = data.global_position;
    global_rotation = data.global_rotation;
    global_scale = data.global_scale;

    active = data.active;
    parent_uuid = data.parent_uuid;
};


void TransformData::update_matrix_and_object_info(float *matrix, int *object_information) {
    matrix[0] = local_position.x;
    matrix[1] = local_position.y;
    matrix[2] = local_position.z;
    matrix[3] = local_rotation.w;
    matrix[4] = local_rotation.x;
    matrix[5] = local_rotation.y;
    matrix[6] = local_rotation.z;
    matrix[7] = local_scale.x;
    matrix[8] = local_scale.y;
    matrix[9] = local_scale.z;
    matrix[10] = global_position.x;
    matrix[11] = global_position.y;
    matrix[12] = global_position.z;
    matrix[13] = global_rotation.w;
    matrix[14] = global_rotation.x;
    matrix[15] = global_rotation.y;
    matrix[16] = global_rotation.z;
    matrix[17] = global_scale.x;
    matrix[18] = global_scale.y;
    matrix[19] = global_scale.z;

    object_information[0] = active;
    object_information[1] = parent_uuid;
}

glm::mat4 TransformData::trs_matrix() {
    glm::mat4 translation = glm::translate(glm::identity<glm::mat4>(), local_position);
    glm::mat4 scaling = glm::scale(glm::identity<glm::mat4>(), local_scale);
    glm::mat4 rotate = glm::toMat4(local_rotation);
    return translation * rotate * scaling;
}

TransformData TransformData::interpolate(TransformData next_transform, float t) {
    if(t >= 1.0f)
        t = 1.0f;
    if(t <= 0.0f)
        t = 0.0f;

    glm::vec3 res_pos = glm::mix(local_position, next_transform.local_position, t);
    glm::quat res_rot = glm::slerp(local_rotation, next_transform.local_rotation, t);
    glm::vec3 res_sca = glm::mix(local_scale, next_transform.local_scale, t);

    glm::vec3 res_glo_pos = glm::mix(global_position, next_transform.global_position, t);
    glm::quat res_glo_rot = glm::slerp(global_rotation, next_transform.global_rotation, t);
    glm::vec3 res_glo_sca = glm::mix(global_scale, next_transform.global_scale, t);

    TransformData interpolated_transform{res_pos, res_rot, res_sca, res_glo_pos, res_glo_rot, res_glo_sca, active, parent_uuid};
    interpolated_transform.time = time + t * (next_transform.time - time);
    interpolated_transform.id = id;
    return interpolated_transform;
}

TransformData TransformData::interpolate(std::shared_ptr<TransformData>next_transform, float t) {
    return interpolate(*next_transform, t);
}

const glm::vec3& TransformData::get_local_position() const {
    return local_position;
}

const glm::quat& TransformData::get_local_rotation() const {
    return local_rotation;
}

const glm::vec3& TransformData::get_local_scale() const {
    return local_scale;
}

const glm::vec3& TransformData::get_global_position() const {
    return global_position;
}

const glm::quat& TransformData::get_global_rotation() const {
    return global_rotation;
}

const glm::vec3& TransformData::get_global_scale() const {
    return global_scale;
}

int TransformData::get_active() const {
    return active;
}

int TransformData::get_parent_uuid() const {
    return parent_uuid;
}

TransformData::TransformData() : RecordableData(-1, -1.0f){

}

TransformData::TransformData(std::shared_ptr<TransformData> data)  : RecordableData(data->id, data->time){
    local_rotation = data->local_rotation;
    local_position = data->local_position;
    local_scale = data->local_scale;
    global_position = data->global_position;
    global_rotation = data->global_rotation;
    global_scale = data->global_scale;
    active = data->active;
    parent_uuid = data->parent_uuid;
}

std::ofstream& TransformData::write(std::ofstream& ofs) const {
    ofs << id  << ' ' << time  << ' ';
    ofs << local_position << ' ';
    ofs << local_rotation << ' ';
    ofs << local_scale  << ' ';
    ofs << global_position << ' ';
    ofs << global_rotation << ' ';
    ofs << global_scale  << ' ';
    ofs << active << ' ' << parent_uuid << "\n";
    return ofs;
}

std::ifstream &TransformData::read(std::ifstream &ifs) {
    ifs >> id  >> time;
    ifs >> local_position;
    ifs >> local_rotation;
    ifs >> local_scale;
    ifs >> global_position;
    ifs >> global_rotation;
    ifs >> global_scale;
    ifs >> active >> parent_uuid;
    return ifs;
}

TransformData::TransformData(const TransformDTO &dto) : RecordableData(dto.id, dto.t){
    local_position.x = dto.lp[0];
    local_position.y = dto.lp[1];
    local_position.z = dto.lp[2];
    local_rotation.w = dto.lr[0];
    local_rotation.x = dto.lr[1];
    local_rotation.y = dto.lr[2];
    local_rotation.z = dto.lr[3];
    local_scale.x = dto.ls[0];
    local_scale.y = dto.ls[1];
    local_scale.z = dto.ls[2];
    global_position.x = dto.gp[0];
    global_position.y = dto.gp[1];
    global_position.z = dto.gp[2];
    global_rotation.w = dto.gr[0];
    global_rotation.x = dto.gr[1];
    global_rotation.y = dto.gr[2];
    global_rotation.z = dto.gr[3];
    global_scale.x = dto.gs[0];
    global_scale.y = dto.gs[1];
    global_scale.z = dto.gs[2];

    active = dto.act;
    parent_uuid = dto.p_id;
}

TransformData::~TransformData() {
    //std::cout << "Destroying Transform Data\n";
}

void TransformData::write_to_stream(std::ofstream &file) const {
    TransformDTO dto = to_dto();
    file.write((const char*) &dto, sizeof(TransformDTO));
}

TransformDTO TransformData::to_dto() const {
    TransformDTO dto;
    dto.id = id;
    dto.t = time;
    dto.lp[0] = local_position.x;
    dto.lp[1] = local_position.y;
    dto.lp[2] = local_position.z;
    dto.lr[0] = local_rotation.w;
    dto.lr[1] = local_rotation.x;
    dto.lr[2] = local_rotation.y;
    dto.lr[3] = local_rotation.z;
    dto.ls[0] = local_scale.x;
    dto.ls[1] = local_scale.y;
    dto.ls[2] = local_scale.z;
    dto.gp[0] = global_position.x;
    dto.gp[1] = global_position.y;
    dto.gp[2] = global_position.z;
    dto.gr[0] = global_rotation.w;
    dto.gr[1] = global_rotation.x;
    dto.gr[2] = global_rotation.y;
    dto.gr[3] = global_rotation.z;
    dto.gs[0] = global_scale.x;
    dto.gs[1] = global_scale.y;
    dto.gs[2] = global_scale.z;

    dto.act = active;
    dto.p_id = parent_uuid;
    return dto;
}
