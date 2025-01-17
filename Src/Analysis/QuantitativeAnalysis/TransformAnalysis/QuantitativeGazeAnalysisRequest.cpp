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

#include "QuantitativeGazeAnalysisRequest.h"

QuantitativeGazeAnalysisRequest::QuantitativeGazeAnalysisRequest(int id_a, int id_b, float t_sampling_rate)
        : id_a(id_a), id_b(id_b), QuantitativeTransformAnalysisRequest(t_sampling_rate) {

}

void QuantitativeGazeAnalysisRequest::update_parameters(MetaInformation &original_meta_file, MetaInformation &new_meta_file) {
    id_a = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id_a));
    id_b = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id_b));
}

void QuantitativeGazeAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data,
                                                      std::shared_ptr<SoundData> s_data) {
    if(!t_data)
        return;

    float current_t = t_data->time;
    if(t_data->id == id_a){
        last_a = current_a;
        current_a = (*t_data);
        present_a = true;
    } else if (t_data->id == id_b){
        last_b = current_b;
        current_b = (*t_data);
        present_b = true;
    }

    if((t_data->id == id_a || t_data->id == id_b) && present_a && present_b && current_a.time >= current_b.time){
       glm::vec3 dir = current_b.global_position - current_a.global_position;
       glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), current_a.global_scale);
       glm::mat4 translate = glm::translate(glm::identity<glm::mat4>(), current_a.global_position);
       glm::mat4 rotate = glm::toMat4(current_a.global_rotation);
       glm::mat4 TRS = translate * rotate * scale;
       glm::vec4 local_pos = glm::inverse(TRS) * glm::vec4{current_b.global_position,1.0f};
       if(current_t - last_value_time > 1.0f/temporal_sampling_rate){
           std::pair<std::string, float> x = {"Pos b.x local in a",local_pos.x/local_pos.w};
           std::pair<std::string, float> y = {"Pos b.y local in a",local_pos.y/local_pos.w};
           std::pair<std::string, float> z = {"Pos b.z local in a",local_pos.z/local_pos.w};
           values.push_back(TimeBasedValue{current_t, {x,y,z}});
           last_value_time = current_t;
       }
    }
}

std::shared_ptr<QuantitativeAnalysisRequest> QuantitativeGazeAnalysisRequest::clone() const {
    return std::make_shared<QuantitativeGazeAnalysisRequest>(id_a, id_b, temporal_sampling_rate);
}

std::string QuantitativeGazeAnalysisRequest::get_description(MetaInformation &meta_info) const {
    std::string s = "GazeAnalysis";
    s += " idA: " + meta_info.get_object_name(id_a) + "-";
    s += " idB: " + meta_info.get_object_name(id_b) + "-";
    s += " temporal sampling rate: " + std::to_string(temporal_sampling_rate);
    return s;
}

void QuantitativeGazeAnalysisRequest::clear_recent_data() {
    last_value_time = 0.0f;
    present_a = false;
    present_b = false;
    values.clear();
}

TransformAnalysisType QuantitativeGazeAnalysisRequest::get_type() const {
    return GazeAnalysis;
}
