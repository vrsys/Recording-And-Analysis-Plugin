//
// Created by Mortiferum on 26.02.2024.
//

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
