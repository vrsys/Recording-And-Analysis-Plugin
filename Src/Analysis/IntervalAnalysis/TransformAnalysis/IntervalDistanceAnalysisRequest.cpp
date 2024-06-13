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

#include "IntervalDistanceAnalysisRequest.h"

IntervalDistanceAnalysisRequest::IntervalDistanceAnalysisRequest(int a, int b, float t) : id_a(a), id_b(b), threshold(t){
    Debug::Log("Distance analysis request: id_a: " + std::to_string(a) + ", id_b: " + std::to_string(b) + ", dist: " + std::to_string(t));
}

// identify all time intervals for which object with id a is within a distance of at most the threshold from object with id b
void IntervalDistanceAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) {
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
        glm::vec3 diff = current_a.global_position - current_b.global_position;
        //std::cout << "Dist: " << glm::length(diff) << "\n";
        if(glm::length(diff) <= threshold){
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

std::string IntervalDistanceAnalysisRequest::get_description(MetaInformation& meta_info) const {
    std::string s = "DistanceAnalysis";
    s += " idA: " + meta_info.get_object_name(id_a) + "-";
    s += " idB: " + meta_info.get_object_name(id_b) + "-";
    s += " distance: " + std::to_string(threshold);
    return s;
}

void IntervalDistanceAnalysisRequest::clear_recent_data() {
    current_a = {};
    current_b = {};
    last_a = {};
    last_b = {};
}

std::shared_ptr<IntervalAnalysisRequest> IntervalDistanceAnalysisRequest::clone() const {
    return std::make_shared<IntervalDistanceAnalysisRequest>(id_a, id_b, threshold);
}

TransformAnalysisType IntervalDistanceAnalysisRequest::get_type() const {
    return DistanceAnalysis;
}

int IntervalDistanceAnalysisRequest::get_id_a() const {
    return id_a;
}

int IntervalDistanceAnalysisRequest::get_id_b() const {
    return id_b;
}

void IntervalDistanceAnalysisRequest::set_id_a(int new_id) {
    id_a = new_id;
}

void IntervalDistanceAnalysisRequest::set_id_b(int new_id) {
    id_b = new_id;
}

void IntervalDistanceAnalysisRequest::update_parameters(MetaInformation &original_meta_file,
                                                        MetaInformation &new_meta_file) {
    id_a = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id_a));
    id_b = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id_b));
}
