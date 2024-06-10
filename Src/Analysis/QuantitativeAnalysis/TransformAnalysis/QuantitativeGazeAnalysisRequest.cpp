//
// Created by Admin on 04.03.2024.
//

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
           values.push_back(TimeBasedValue{current_t, {local_pos.x/local_pos.w,local_pos.y/local_pos.w,local_pos.z,local_pos.w}});
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
