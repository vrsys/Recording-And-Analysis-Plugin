//
// Created by Anton-Lammert on 27.10.2022.
//

#include "IntervalRotationAnalysisRequest.h"

IntervalRotationAnalysisRequest::IntervalRotationAnalysisRequest(int i, float t, float rot_threshold, float pos_threshold) : id(i),
                                                                                                                             search_interval(
                                                                                                                     t),
                                                                                                                             rotation_threshold(
                                                                                                                     rot_threshold),
                                                                                                                             position_threshold(
                                                                                                                     pos_threshold) {}

void IntervalRotationAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) {
    if(!t_data)
        return;
    if (t_data->id == id) {
        recent_data.push_back(t_data);

        while (t_data->time - recent_data.front()->time > search_interval) {
            recent_data.pop_front();
        }

        float max_pos_diff = -1.0f;
        float max_angle_diff = -1.0f;
        for (int i = 0; i < recent_data.size() - 1; ++i) {
            float pos_diff = glm::length(recent_data[i]->global_position - t_data->global_position);
            if (pos_diff > max_pos_diff)
                max_pos_diff = pos_diff;
            glm::quat tmp = recent_data[i]->global_rotation * glm::inverse(t_data->global_rotation);
            float angle_diff = acos(abs(tmp.w)) * 2.0f;
            float angle_diff_degrees = angle_diff * (180.0f / 3.141f);
            if (angle_diff_degrees > max_angle_diff)
                max_angle_diff = angle_diff_degrees;
        }


        if (rotation_threshold > 0.0f && max_angle_diff * 180.0f / 3.14159f >= rotation_threshold) {
            if (currentInterval.start < 0.0f) {
                currentInterval.start = recent_data.front()->time;
                currentInterval.end = recent_data.back()->time;
                if (!intervals.empty() && intervals[intervals.size() - 1].end >= currentInterval.start) {
                    intervals[intervals.size() - 1].end = currentInterval.end;
                } else {
                    intervals.push_back(currentInterval);
                }
                currentInterval.start = -1.0f;
                currentInterval.end = -1.0f;
            }
        }
    }

}

std::string IntervalRotationAnalysisRequest::get_description(MetaInformation& meta_info) const {
    std::string s = "RotationAnalysis";
    s += " id: " + meta_info.get_object_name(id) + "-";
    s += " rotationThreshold: " + std::to_string(rotation_threshold);
    return s;
}

void IntervalRotationAnalysisRequest::clear_recent_data() {
    recent_data.clear();
}

std::shared_ptr<IntervalAnalysisRequest> IntervalRotationAnalysisRequest::clone() const {
    return std::make_shared<IntervalRotationAnalysisRequest>(id, search_interval, rotation_threshold, position_threshold);
}

TransformAnalysisType IntervalRotationAnalysisRequest::get_type() const {
    return RotationAnalysis;
}

int IntervalRotationAnalysisRequest::get_id() const {
    return id;
}

void IntervalRotationAnalysisRequest::set_id(int new_id) {
    id = new_id;
}

void IntervalRotationAnalysisRequest::update_parameters(MetaInformation &original_meta_file,
                                                        MetaInformation &new_meta_file) {
    id = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id));
}

