//
// Created by Admin on 07.03.2024.
//

#include "QuantitativeDistanceAnalysisRequest.h"

QuantitativeDistanceAnalysisRequest::QuantitativeDistanceAnalysisRequest(int id_a, int id_b, float t) : id_a(id_a), id_b(id_b), QuantitativeTransformAnalysisRequest(t) {

}

std::string QuantitativeDistanceAnalysisRequest::get_description(MetaInformation& meta_info) const {
    std::string s = "DistanceAnalysis";
    s += " id_a: " + meta_info.get_object_name(id_a) + "-";
    s += " id_b: " + meta_info.get_object_name(id_b) + "-";
    s += " temporal sampling rate: " + std::to_string(temporal_sampling_rate);
    return s;
}

void QuantitativeDistanceAnalysisRequest::clear_recent_data() {
    present_a = false;
    present_b = false;
    last_value_time = -1;
    values.clear();
}

std::shared_ptr<QuantitativeAnalysisRequest> QuantitativeDistanceAnalysisRequest::clone() const {
    return std::make_shared<QuantitativeDistanceAnalysisRequest>(id_a, id_b, temporal_sampling_rate);
}

TransformAnalysisType QuantitativeDistanceAnalysisRequest::get_type() const {
    return VelocityAnalysis;
}

void QuantitativeDistanceAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data,
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
        if(current_t - last_value_time > 1.0f/temporal_sampling_rate){
            values.push_back(TimeBasedValue{current_t, {glm::length(dir)}});
            last_value_time = current_t;
        }
    }
}

void QuantitativeDistanceAnalysisRequest::update_parameters(MetaInformation &original_meta_file, MetaInformation &new_meta_file) {
    id_a = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id_a));
    id_b = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id_b));
}
