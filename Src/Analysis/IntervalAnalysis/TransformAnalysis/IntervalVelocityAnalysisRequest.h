//
// Created by Anton-Lammert on 06.01.2023.
//

#ifndef RECORDINGPLUGIN_INTERVALVELOCITYANALYSISREQUEST_H
#define RECORDINGPLUGIN_INTERVALVELOCITYANALYSISREQUEST_H

#include "IntervalTransformAnalysisRequest.h"
#include <deque>

class IntervalVelocityAnalysisRequest : public IntervalTransformAnalysisRequest{

private:
    int id;
    float search_interval;
    float velocity_threshold;

    std::deque<TransformData> recent_data;
    std::deque<bool> recent_data_active;

public:
    IntervalVelocityAnalysisRequest(int i, float t, float velocity_threshold);

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    std::string get_description(MetaInformation& meta_info) const override;

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    std::shared_ptr<IntervalAnalysisRequest> clone() const override;

    void clear_recent_data() override;

    TransformAnalysisType get_type() const override;

    int get_id() const;

    void set_id(int id);
};

#endif //RECORDINGPLUGIN_INTERVALVELOCITYANALYSISREQUEST_H
