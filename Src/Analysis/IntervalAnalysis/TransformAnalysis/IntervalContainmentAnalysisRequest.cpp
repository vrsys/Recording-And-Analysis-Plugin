//
// Created by Anton-Lammert on 27.10.2022.
//

#include "IntervalContainmentAnalysisRequest.h"

IntervalContainmentAnalysisRequest::IntervalContainmentAnalysisRequest(int i, glm::vec3 min, glm::vec3 max) : id{i} {
    box_min = glm::vec3{std::min(min.x, max.x), std::min(min.y, max.y), std::min(min.z, max.z)};
    box_max = glm::vec3{std::max(min.x, max.x), std::max(min.y, max.y), std::max(min.z, max.z)};
}

// identify all time intervals for which object with the given id is within the bounding box defined by box_min & box_max
void IntervalContainmentAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) {
    if(!t_data)
        return;
    if(t_data->id == id){
        last = current;
        current = (*t_data);

        if(current.global_position.x <= box_max.x && box_min.x <= current.global_position.x &&
            current.global_position.y <= box_max.y && box_min.y <= current.global_position.y &&
            current.global_position.z <= box_max.z && box_min.z <= current.global_position.z){
            if(currentInterval.start < 0.0f){
                currentInterval.start = current.time;
            }
        } else {
            if(currentInterval.start >= 0.0f){
                currentInterval.end = last.time;
                intervals.push_back(currentInterval);
                currentInterval.start = -1.0f;
                currentInterval.end = -1.0f;
            }
        }
    }
}

std::string IntervalContainmentAnalysisRequest::get_description(MetaInformation& meta_info) const {
    std::string s = "ContainmentAnalysis";
    s += "Id: " + meta_info.get_object_name(id);
    s += " min: " + std::to_string(box_min.x) + "-" + std::to_string(box_min.y) + "-" + std::to_string(box_min.z);
    s += " max: " + std::to_string(box_min.x) + "-" + std::to_string(box_min.y) + "-" + std::to_string(box_min.z);
    return s;
}

void IntervalContainmentAnalysisRequest::clear_recent_data() {
    current = {};
    last = {};
}

std::shared_ptr<IntervalAnalysisRequest> IntervalContainmentAnalysisRequest::clone() const {
    return std::make_shared<IntervalContainmentAnalysisRequest>(id, box_min, box_max);
}

TransformAnalysisType IntervalContainmentAnalysisRequest::get_type() const {
    return ContainmentAnalysis;
}

int IntervalContainmentAnalysisRequest::get_id() const {
    return id;
}

void IntervalContainmentAnalysisRequest::set_id(int new_id) {
    id = new_id;
}

void IntervalContainmentAnalysisRequest::update_parameters(MetaInformation &original_meta_file,
                                                           MetaInformation &new_meta_file) {
    id = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id));
}
