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

#include "NegatedIntervalAnalysisRequest.h"

std::vector<TimeInterval> NegatedIntervalAnalysisRequest::get_result() const {
    return !original_request->get_result();
}

NegatedIntervalAnalysisRequest::NegatedIntervalAnalysisRequest(std::shared_ptr<IntervalAnalysisRequest> request) : original_request(request) {

}

std::string NegatedIntervalAnalysisRequest::get_description(MetaInformation &meta_info) const {
    return "Negated: " + original_request->get_description(meta_info);
}

void NegatedIntervalAnalysisRequest::update_parameters(MetaInformation &original_meta_file, MetaInformation &new_meta_file) {
    original_request->update_parameters(original_meta_file, new_meta_file);
}

void NegatedIntervalAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data,
                                                     std::shared_ptr<SoundData> s_data) {
    original_request->process_request(t_data, s_data);
}

std::shared_ptr<IntervalAnalysisRequest> NegatedIntervalAnalysisRequest::clone() const {
    return std::make_shared<NegatedIntervalAnalysisRequest>(original_request);
}
