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

#ifndef RECORDINGPLUGIN_INTERVALCONTAINMENTANALYSISREQUEST_H
#define RECORDINGPLUGIN_INTERVALCONTAINMENTANALYSISREQUEST_H

#include "IntervalTransformAnalysisRequest.h"

class IntervalContainmentAnalysisRequest : public IntervalTransformAnalysisRequest{

private:
    int id;

    glm::vec3 box_min;
    glm::vec3 box_max;

    bool present = false;
    TransformData current;
    TransformData last;

public:
    IntervalContainmentAnalysisRequest(int i, glm::vec3 min, glm::vec3 max);

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    std::string get_description(MetaInformation& meta_info) const override;

    std::shared_ptr<IntervalAnalysisRequest> clone() const override;

    void clear_recent_data() override;

    int get_id() const;

    void set_id(int id);

    TransformAnalysisType get_type() const override;
};


#endif //RECORDINGPLUGIN_INTERVALCONTAINMENTANALYSISREQUEST_H
