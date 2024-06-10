//
// Created by Anton-Lammert on 26.10.2022.
//

#ifndef RECORDINGPLUGIN_INTERVALTRANSFORMANALYSISREQUEST_H
#define RECORDINGPLUGIN_INTERVALTRANSFORMANALYSISREQUEST_H

#include "Analysis/IntervalAnalysis/IntervalAnalysisRequest.h"
#include "Recording/Transform/TransformData.h"
#include "DebugLog.h"
#include <vector>

class IntervalTransformAnalysisRequest : public IntervalAnalysisRequest{
public:
    virtual void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) = 0;

    virtual TransformAnalysisType get_type() const = 0;
};


#endif //RECORDINGPLUGIN_INTERVALTRANSFORMANALYSISREQUEST_H
