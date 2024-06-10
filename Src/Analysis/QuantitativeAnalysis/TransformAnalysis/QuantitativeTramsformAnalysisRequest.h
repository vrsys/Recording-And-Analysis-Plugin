//
// Created by Mortiferum on 26.02.2024.
//

#ifndef RECORDINGPLUGIN_QUANTITATIVETRAMSFORMANALYSISREQUEST_H
#define RECORDINGPLUGIN_QUANTITATIVETRAMSFORMANALYSISREQUEST_H


#include "Recording/Transform/TransformData.h"
#include "Analysis/AnalysisStructs.h"
#include "Analysis/QuantitativeAnalysis/QuantitativeAnalysisRequest.h"

class QuantitativeTransformAnalysisRequest : public QuantitativeAnalysisRequest{
public:
    QuantitativeTransformAnalysisRequest(float t_sampling_rate) : QuantitativeAnalysisRequest(t_sampling_rate){};

    virtual void clear_recent_data() = 0;

    virtual TransformAnalysisType get_type() const = 0;
};



#endif //RECORDINGPLUGIN_QUANTITATIVETRAMSFORMANALYSISREQUEST_H
