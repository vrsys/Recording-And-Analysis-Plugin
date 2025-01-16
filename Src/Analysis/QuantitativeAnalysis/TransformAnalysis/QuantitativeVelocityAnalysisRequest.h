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

#ifndef RECORDINGPLUGIN_QUANTITATIVEVELOCITYANALYSISREQUEST_H
#define RECORDINGPLUGIN_QUANTITATIVEVELOCITYANALYSISREQUEST_H

#include <deque>
#include "Recording/MetaInformation.h"
#include "QuantitativeTransformAnalysisRequest.h"

class QuantitativeVelocityAnalysisRequest  : public QuantitativeTransformAnalysisRequest{

private:
    int id;

    std::deque<TransformData> recent_data;

public:
    QuantitativeVelocityAnalysisRequest(int i, float t_sampling_rate);

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    std::shared_ptr<QuantitativeAnalysisRequest> clone() const override;

    std::string get_description(MetaInformation& meta_info) const override;

    void clear_recent_data() override;

    TransformAnalysisType get_type() const override;

    int get_id() const;

    void set_id(int id);
};



#endif //RECORDINGPLUGIN_QUANTITATIVEVELOCITYANALYSISREQUEST_H
