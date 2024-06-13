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

#ifndef RECORDINGPLUGIN_INTERVALANALYSISREQUEST_H
#define RECORDINGPLUGIN_INTERVALANALYSISREQUEST_H

#include "Analysis/AnalysisStructs.h"
#include "Recording/BaseStructure/RecordedIdData.h"
#include "Recording/MetaInformation.h"
#include "Analysis/AnalysisRequest.h"


class IntervalAnalysisRequest : public AnalysisRequest{
protected:
    // time intervals for which the corresponding analysis request yielded positive results
    std::vector<TimeInterval> intervals;

    // interval currently being worked on
    TimeInterval currentInterval;
public:

    virtual std::shared_ptr<IntervalAnalysisRequest> clone() const = 0;

    virtual std::vector<TimeInterval> get_result() const;

    virtual void clear_recent_data() = 0;

    void clear_results();

    friend std::shared_ptr<IntervalAnalysisRequest> operator!(std::shared_ptr<IntervalAnalysisRequest> lhs);

    friend std::shared_ptr<IntervalAnalysisRequest> operator&(std::shared_ptr<IntervalAnalysisRequest> lhs, std::shared_ptr<IntervalAnalysisRequest> rhs);

    friend std::shared_ptr<IntervalAnalysisRequest> operator|(std::shared_ptr<IntervalAnalysisRequest> lhs, std::shared_ptr<IntervalAnalysisRequest> rhs);

};

#endif //RECORDINGPLUGIN_INTERVALANALYSISREQUEST_H
