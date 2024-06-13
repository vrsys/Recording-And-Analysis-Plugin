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

#include "IntervalPositionAdjustmentAnalysisRequest.h"

IntervalPositionAdjustmentAnalysisRequest::IntervalPositionAdjustmentAnalysisRequest(int i, float t, float dist_threshold) : id(i), search_interval(t), distance_threshold(dist_threshold) {}

void IntervalPositionAdjustmentAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) {
    if(!t_data)
        return;
    if (t_data->id == id) {
        recent_data.push_back(t_data);

        while (t_data->time - recent_data.front()->time > search_interval) {
            recent_data.pop_front();
        }

        float max_pos_diff = -1.0f;
        for (int i = 0; i < recent_data.size() - 1; ++i) {
            float pos_diff = glm::length(recent_data[i]->global_position - t_data->global_position);
            if (pos_diff > max_pos_diff)
                max_pos_diff = pos_diff;
        }


        if (distance_threshold > 0.0f && max_pos_diff >= distance_threshold) {
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

std::string IntervalPositionAdjustmentAnalysisRequest::get_description(MetaInformation& meta_info) const {
    std::string s = "PositionAdjustmentAnalysis";
    s += " id: " + meta_info.get_object_name(id) + "-";
    s += " distanceThreshold: " + std::to_string(distance_threshold);
    return s;
}

void IntervalPositionAdjustmentAnalysisRequest::clear_recent_data() {
    recent_data.clear();
}

std::shared_ptr<IntervalAnalysisRequest> IntervalPositionAdjustmentAnalysisRequest::clone() const {
    return std::make_shared<IntervalPositionAdjustmentAnalysisRequest>(id, search_interval, distance_threshold);
}

TransformAnalysisType IntervalPositionAdjustmentAnalysisRequest::get_type() const {
    return PositionAdjustmentAnalysis;
}

int IntervalPositionAdjustmentAnalysisRequest::get_id() const {
    return id;
}

void IntervalPositionAdjustmentAnalysisRequest::set_id(int new_id) {
    id = new_id;
}

void IntervalPositionAdjustmentAnalysisRequest::update_parameters(MetaInformation &original_meta_file,
                                                                  MetaInformation &new_meta_file) {
    id = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id));
}
