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

#include "QuantitativeVelocityAnalysisRequest.h"

QuantitativeVelocityAnalysisRequest::QuantitativeVelocityAnalysisRequest(int i, float t) : id(i), QuantitativeTransformAnalysisRequest(t) {

}

std::string QuantitativeVelocityAnalysisRequest::get_description(MetaInformation& meta_info) const {
    std::string s = "VelocityAnalysis";
    s += " id: " + meta_info.get_object_name(id) + "-";
    s += " temporal sampling rate: " + std::to_string(temporal_sampling_rate);
    return s;
}

void QuantitativeVelocityAnalysisRequest::clear_recent_data() {
    last_value_time = -1;
    values.clear();
    recent_data.clear();
}

std::shared_ptr<QuantitativeAnalysisRequest> QuantitativeVelocityAnalysisRequest::clone() const {
    return std::make_shared<QuantitativeVelocityAnalysisRequest>(id, temporal_sampling_rate);
}

TransformAnalysisType QuantitativeVelocityAnalysisRequest::get_type() const {
    return VelocityAnalysis;
}

int QuantitativeVelocityAnalysisRequest::get_id() const {
    return id;
}

void QuantitativeVelocityAnalysisRequest::set_id(int new_id) {
    id = new_id;
}

void QuantitativeVelocityAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data,
                                                          std::shared_ptr<SoundData> s_data) {
    if(!t_data)
        return;
    if (t_data->id == id) {
        recent_data.emplace_back(t_data);

        while (recent_data.size() > 100) {
            recent_data.pop_front();
        }

        if(recent_data.size() > 2) {
            TransformData last = recent_data[recent_data.size() - 1];
            TransformData previous = recent_data[recent_data.size() - 2];

            float pos_diff = glm::length(last.global_position - previous.global_position);
            float time_diff = last.time - previous.time;
            float current_velocity = std::abs(pos_diff / time_diff);
            if(last.time > last_value_time + (1.0f /temporal_sampling_rate)) {
                std::pair<std::string, float> vel = {"Velocity",current_velocity};
                values.push_back(TimeBasedValue{last.time, {vel}});
                last_value_time = last.time;
            }
        }
    }
}

void QuantitativeVelocityAnalysisRequest::update_parameters(MetaInformation &original_meta_file,
                                                            MetaInformation &new_meta_file) {
    id = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id));
}
