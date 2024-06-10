//
// Created by Anton-Lammert on 27.10.2022.
//

#include "IntervalGazeAnalysisRequest.h"

IntervalGazeAnalysisRequest::IntervalGazeAnalysisRequest(int a, int b, float t, float max_d, int ax) : id_a(a), id_b(b), threshold(t), cone_height(max_d), axis(ax){
    cone_radius = tan((threshold * 3.14159f)/360.0f) * cone_height;
}

void IntervalGazeAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) {
    if(!t_data)
        return;
    if(t_data->id == id_a){
        last_a = current_a;
        current_a = (*t_data);
        present_a = true;
    } else if (t_data->id == id_b){
        last_b = current_b;
        current_b = (*t_data);
        present_b = true;
    }

    if((t_data->id == id_a || t_data->id == id_b) && present_a && present_b){
        // see https://stackoverflow.com/a/12826333
        glm::mat4 rot = glm::toMat4(current_a.global_rotation);
        
        glm::vec4 axis_dir = glm::vec4{};
        switch (axis) {
            case 0:
                axis_dir = glm::vec4{0.0f,0.0f,-1.0f,1.0f};
                break;
            case 1:
                axis_dir = glm::vec4{0.0f,0.0f,1.0f,1.0f};
                break;
            case 2:
                axis_dir = glm::vec4{0.0f,-1.0f,0.0f,1.0f};
                break;
            case 3:
                axis_dir = glm::vec4{0.0f,1.0f,0.0f,1.0f};
                break;
            case 4:
                axis_dir = glm::vec4{-1.0f,0.0f,0.0f,1.0f};
                break;
            case 5:
                axis_dir = glm::vec4{1.0f,0.0f,0.0f,1.0f};
                break;
        }

        glm::vec4 dir = rot * axis_dir;
        glm::vec3 dir3 {dir.x/dir.w, dir.y/dir.w, dir.z/dir.w};

        float cone_dist = glm::dot(current_b.global_position - current_a.global_position, dir3);

        if(cone_dist < 0.0f || cone_dist >= cone_height)
            return;

        float current_radius = (cone_dist / cone_height) * cone_radius;
        float orthogonal_distance = glm::length((current_b.global_position - current_a.global_position) - cone_dist * dir3);

        if(orthogonal_distance <= current_radius){
            if(currentInterval.start <= 0.0f){
                currentInterval.start = current_a.time > current_b.time ? current_a.time : current_b.time;
            }
        } else {
            currentInterval.end = last_a.time < last_b.time ? last_a.time : last_b.time;
            if(currentInterval.start >= 0.0f && currentInterval.end >= currentInterval.start){
                intervals.push_back(currentInterval);
            }
            currentInterval.start = -1.0f;
            currentInterval.end = -1.0f;
        }
    }
}

std::string IntervalGazeAnalysisRequest::get_description(MetaInformation& meta_info) const{
    std::string s = "GazeAnalysis";
    s += " idA: " + meta_info.get_object_name(id_a) + "-";
    s += " idB: " + meta_info.get_object_name(id_b) + "-";
    s += " coneDistance: " + std::to_string(cone_height);
    s += " coneRadius: " + std::to_string(threshold);
    return s;
}

void IntervalGazeAnalysisRequest::clear_recent_data() {
    current_a = {};
    current_b = {};
    last_a = {};
    last_b = {};
}

std::shared_ptr<IntervalAnalysisRequest> IntervalGazeAnalysisRequest::clone() const {
    return std::make_shared<IntervalGazeAnalysisRequest>(id_a, id_b, threshold, cone_height, axis);
}

TransformAnalysisType IntervalGazeAnalysisRequest::get_type() const {
    return GazeAnalysis;
}

int IntervalGazeAnalysisRequest::get_id_a() const {
    return id_a;
}

int IntervalGazeAnalysisRequest::get_id_b() const {
    return id_b;
}

void IntervalGazeAnalysisRequest::set_id_a(int new_id) {
    id_a = new_id;
}

void IntervalGazeAnalysisRequest::set_id_b(int new_id) {
    id_b = new_id;
}

void
IntervalGazeAnalysisRequest::update_parameters(MetaInformation &original_meta_file, MetaInformation &new_meta_file) {
    id_a = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id_a));
    id_b = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id_b));
}
