//
// Created by Anton-Lammert on 27.10.2022.
//

#ifndef RECORDINGPLUGIN_INTERVALROTATIONANALYSISREQUEST_H
#define RECORDINGPLUGIN_INTERVALROTATIONANALYSISREQUEST_H

#include "IntervalTransformAnalysisRequest.h"
#include <cmath>
#include <deque>

class IntervalRotationAnalysisRequest : public IntervalTransformAnalysisRequest{

private:
    int id;
    float search_interval;
    // rotation threshold in degree
    float rotation_threshold;
    float position_threshold;

    std::deque<std::shared_ptr<TransformData>> recent_data;

public:
    IntervalRotationAnalysisRequest(int i, float t, float rot_threshold, float pos_threshold = -1.0f);

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    std::string get_description(MetaInformation& meta_info) const override;

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    std::shared_ptr<IntervalAnalysisRequest> clone() const override;

    void clear_recent_data() override;

    TransformAnalysisType get_type() const override;

    int get_id() const;

    void set_id(int id);
};


#endif //RECORDINGPLUGIN_INTERVALROTATIONANALYSISREQUEST_H
