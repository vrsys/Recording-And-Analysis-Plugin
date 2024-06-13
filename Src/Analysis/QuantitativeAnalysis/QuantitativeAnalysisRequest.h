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

#ifndef RECORDINGPLUGIN_QUANTITATIVEANALYSISREQUEST_H
#define RECORDINGPLUGIN_QUANTITATIVEANALYSISREQUEST_H

#include <vector>
#include "Recording/MetaInformation.h"
#include "Analysis/AnalysisStructs.h"
#include "Analysis/AnalysisRequest.h"

class QuantitativeAnalysisRequest : public AnalysisRequest{
protected:
    // analysis metric values
    std::vector<TimeBasedValue> values;
    // sampling rate for which values are computed (e.g. one value for every second)
    float temporal_sampling_rate;
    // point in time for which the last value was computed
    float last_value_time = 0.0f;
public:
    QuantitativeAnalysisRequest(float t_sampling_rate) : temporal_sampling_rate(t_sampling_rate){};

    std::vector<TimeBasedValue> get_result() const;

    virtual std::shared_ptr<QuantitativeAnalysisRequest> clone() const = 0;

    virtual std::string get_description(MetaInformation& meta_info) const = 0;

    void clear_results();
};

#endif //RECORDINGPLUGIN_QUANTITATIVEANALYSISREQUEST_H
