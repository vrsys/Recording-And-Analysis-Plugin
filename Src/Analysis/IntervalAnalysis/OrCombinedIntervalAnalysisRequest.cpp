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

#include "OrCombinedIntervalAnalysisRequest.h"

std::vector<TimeInterval> OrCombinedIntervalAnalysisRequest::get_result() const {
    return left_hand_side->get_result() || right_hand_side->get_result();
}

OrCombinedIntervalAnalysisRequest::OrCombinedIntervalAnalysisRequest(std::shared_ptr<IntervalAnalysisRequest>lhs, std::shared_ptr<IntervalAnalysisRequest> rhs)
        : left_hand_side(lhs), right_hand_side(rhs) {

}

std::string OrCombinedIntervalAnalysisRequest::get_description(MetaInformation &meta_info) const {
    return "(Or combined: " + left_hand_side->get_description(meta_info) + " - " + right_hand_side->get_description(meta_info) + " )";
}

void OrCombinedIntervalAnalysisRequest::update_parameters(MetaInformation &original_meta_file,
                                                          MetaInformation &new_meta_file) {
    left_hand_side->update_parameters(original_meta_file, new_meta_file);
    right_hand_side->update_parameters(original_meta_file, new_meta_file);
}

void OrCombinedIntervalAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data,
                                                        std::shared_ptr<SoundData> s_data) {
    left_hand_side->process_request(t_data, s_data);
    right_hand_side->process_request(t_data, s_data);
}

std::shared_ptr<IntervalAnalysisRequest> OrCombinedIntervalAnalysisRequest::clone() const {
    return std::make_shared<OrCombinedIntervalAnalysisRequest>(left_hand_side, right_hand_side);
}
