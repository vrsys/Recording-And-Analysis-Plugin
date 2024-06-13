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

#include "QuantitativeRotationAnalysisRequest.h"

QuantitativeRotationAnalysisRequest::QuantitativeRotationAnalysisRequest(int i, float t_sampling_rate) : id(i),
                                                                                                         QuantitativeTransformAnalysisRequest(
                                                                                                                 t_sampling_rate) {

}

void QuantitativeRotationAnalysisRequest::update_parameters(MetaInformation &original_meta_file,
                                                            MetaInformation &new_meta_file) {
    id = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id));
}

void QuantitativeRotationAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data,
                                                          std::shared_ptr<SoundData> s_data) {
    if (!t_data)
        return;
    if (t_data->id == id) {
        recent_data.emplace_back(t_data);

        while (recent_data.size() > 100) {
            recent_data.pop_front();
        }

        if (t_data->time - last_value_time > 1.0f / temporal_sampling_rate) {
            float max_angle_diff = -1.0f;
            for (int i = 0; i < recent_data.size() - 1; ++i) {
                if(t_data->time - recent_data[i]->time < 1.0f/temporal_sampling_rate) {
                    glm::quat tmp = recent_data[i]->global_rotation * glm::inverse(t_data->global_rotation);
                    float angle_diff = acos(abs(tmp.w)) * 2.0f;
                    float angle_diff_degrees = angle_diff * (180.0f / 3.141f);
                    if (angle_diff_degrees > max_angle_diff)
                        max_angle_diff = angle_diff_degrees;
                }
            }

            values.push_back(TimeBasedValue{t_data->time, {max_angle_diff}});
            last_value_time = t_data->time;
        }
    }
}

std::shared_ptr<QuantitativeAnalysisRequest> QuantitativeRotationAnalysisRequest::clone() const {
    return std::make_shared<QuantitativeRotationAnalysisRequest>(id, temporal_sampling_rate);
}

std::string QuantitativeRotationAnalysisRequest::get_description(MetaInformation &meta_info) const {
    std::string s = "RotationAnalysis";
    s += " id: " + meta_info.get_object_name(id) + "-";
    s += " temporal sampling rate: " + std::to_string(temporal_sampling_rate);
    return s;
}

void QuantitativeRotationAnalysisRequest::clear_recent_data() {
    last_value_time = -1;
    values.clear();
}

TransformAnalysisType QuantitativeRotationAnalysisRequest::get_type() const {
    return RotationAnalysis;
}
