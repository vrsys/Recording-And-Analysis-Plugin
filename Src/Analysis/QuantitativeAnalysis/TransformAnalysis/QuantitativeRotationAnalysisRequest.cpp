//
// Created by Admin on 07.03.2024.
//

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
