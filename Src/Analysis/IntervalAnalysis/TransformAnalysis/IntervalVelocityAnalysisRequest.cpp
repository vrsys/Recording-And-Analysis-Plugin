//
// Created by Anton-Lammert on 06.01.2023.
//

#include "IntervalVelocityAnalysisRequest.h"


IntervalVelocityAnalysisRequest::IntervalVelocityAnalysisRequest(int i, float t, float v_threshold) : id(i), search_interval(t),
                                                                                                      velocity_threshold(v_threshold) {

}

void IntervalVelocityAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) {
    if(!t_data)
        return;
    if (t_data->id == id) {
        recent_data.emplace_back(t_data);

        while (t_data->time - recent_data.front().time > search_interval) {
            recent_data.pop_front();
            recent_data_active.pop_front();
        }

        bool current_active = false;
        if(recent_data.size() > 2) {
            float pos_diff = glm::length(recent_data[recent_data.size() - 1].global_position - recent_data[recent_data.size() - 2].global_position);
            float time_diff = recent_data[recent_data.size() - 1].time - recent_data[recent_data.size() - 2].time;
            float current_velocity = std::abs(pos_diff / time_diff);
            if (current_velocity > velocity_threshold)
                current_active = true;
        }

        recent_data_active.emplace_back(current_active);

        bool active = false;
        for (auto i: recent_data_active) {
            if (i) {
                active = true;
                break;
            }
        }

        if (active) {
            if (currentInterval.start < 0.0f) {
                currentInterval.start = recent_data.front().time;
                currentInterval.end = recent_data.back().time;
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

std::string IntervalVelocityAnalysisRequest::get_description(MetaInformation& meta_info) const {
    std::string s = "VelocityAnalysis";
    s += " id: " + meta_info.get_object_name(id) + "-";
    s += " threshold: " + std::to_string(velocity_threshold);
    return s;
}

void IntervalVelocityAnalysisRequest::clear_recent_data() {
    recent_data.clear();
}

std::shared_ptr<IntervalAnalysisRequest> IntervalVelocityAnalysisRequest::clone() const {
    return std::make_shared<IntervalVelocityAnalysisRequest>(id, search_interval, velocity_threshold);
}

TransformAnalysisType IntervalVelocityAnalysisRequest::get_type() const {
    return VelocityAnalysis;
}

int IntervalVelocityAnalysisRequest::get_id() const {
    return id;
}

void IntervalVelocityAnalysisRequest::set_id(int new_id) {
    id = new_id;
}

void IntervalVelocityAnalysisRequest::update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) {
    id = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id));
}

