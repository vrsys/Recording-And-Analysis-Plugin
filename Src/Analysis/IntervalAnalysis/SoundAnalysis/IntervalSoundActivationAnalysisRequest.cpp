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

#include "IntervalSoundActivationAnalysisRequest.h"


IntervalSoundActivationAnalysisRequest::IntervalSoundActivationAnalysisRequest(int i, float s_i, float a_l) : IntervalSoundAnalysisRequest(i),
                                                                                                              search_interval(s_i),
                                                                                                              activation_level(a_l) {}

void IntervalSoundActivationAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) {
    if(!s_data)
        return;

    if (s_data->id == id) {
        recent_data.emplace_back(s_data);

        while(s_data->time - recent_data.front().time > search_interval) {
            recent_data.pop_front();
            recent_data_active.pop_front();
        }

        bool current_active = false;
        for (int j = 0; j < s_data->sample_num; ++j) {
            int offset = s_data->start_index;
            if (std::abs(s_data->sound_data[offset + j]) > activation_level) {
                current_active = true;
                break;
            }
        }

        recent_data_active.emplace_back(current_active);

        bool active = false;
        for (auto i : recent_data_active) {
            if(i) {
                active = true;
                break;
            }
        }

        if(active) {
            if (currentInterval.start < 0.0f) {
                currentInterval.start = recent_data.front().time;
                currentInterval.end = recent_data.back().time;
            } else {
                currentInterval.end = recent_data.back().time;
            }
        } else if(currentInterval.start >= 0.0f && currentInterval.end >= currentInterval.start) {
            if (!intervals.empty() && currentInterval.start <= intervals.back().end) {
                // Update the end time of the last interval instead of creating a new one
                intervals.back().end = currentInterval.end;
            } else {
                intervals.push_back(currentInterval);
            }
            currentInterval.start = -1.0f;
            currentInterval.end = -1.0f;
        }
    }
}

std::string IntervalSoundActivationAnalysisRequest::get_description(MetaInformation& meta_info) const {
    std::string s = "SoundAnalysis";
    s += " id: " + std::to_string(id) + "-";
    s += " threshold: " + std::to_string(activation_level);
    return s;
}

void IntervalSoundActivationAnalysisRequest::clear_recent_data() {
    recent_data.clear();
    recent_data_active.clear();
}

std::shared_ptr<IntervalAnalysisRequest> IntervalSoundActivationAnalysisRequest::clone() const {
    return std::make_shared<IntervalSoundActivationAnalysisRequest>(id, search_interval, activation_level);
}

void IntervalSoundActivationAnalysisRequest::update_parameters(MetaInformation &original_meta_file,
                                                               MetaInformation &new_meta_file) {
    // sound mapping?
}
