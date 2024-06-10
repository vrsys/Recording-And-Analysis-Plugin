//
// Created by Mortiferum on 09.07.2023.
//

#ifndef RECORDINGPLUGIN_INTERVALPOSITIONADJUSTMENTANALYSISREQUEST_H
#define RECORDINGPLUGIN_INTERVALPOSITIONADJUSTMENTANALYSISREQUEST_H

#include "IntervalTransformAnalysisRequest.h"
#include <deque>

class IntervalPositionAdjustmentAnalysisRequest : public IntervalTransformAnalysisRequest{
private:
    int id;
    float search_interval;
    float distance_threshold;

    std::deque<std::shared_ptr<TransformData>> recent_data;

public:
    IntervalPositionAdjustmentAnalysisRequest(int i, float t, float dist_threshold = -1.0f);

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    std::string get_description(MetaInformation& meta_info) const override;

    std::shared_ptr<IntervalAnalysisRequest> clone() const override;

    void clear_recent_data() override;

    TransformAnalysisType get_type() const override;

    int get_id() const;

    void set_id(int id);
};


#endif //RECORDINGPLUGIN_INTERVALPOSITIONADJUSTMENTANALYSISREQUEST_H
