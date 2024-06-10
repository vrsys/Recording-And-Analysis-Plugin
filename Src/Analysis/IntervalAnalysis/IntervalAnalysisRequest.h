//
// Created by Mortiferum on 09.07.2023.
//

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
