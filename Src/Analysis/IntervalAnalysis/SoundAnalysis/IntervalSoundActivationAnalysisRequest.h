//
// Created by Anton-Lammert on 13.12.2022.
//

#ifndef RECORDINGPLUGIN_INTERVALSOUNDACTIVATIONANALYSISREQUEST_H
#define RECORDINGPLUGIN_INTERVALSOUNDACTIVATIONANALYSISREQUEST_H

#include "IntervalSoundAnalysisRequest.h"
#include <deque>

class IntervalSoundActivationAnalysisRequest : public IntervalSoundAnalysisRequest{

private:

    float search_interval;
    float activation_level;
    std::deque<SoundData> recent_data;
    std::deque<bool> recent_data_active;
public:
    IntervalSoundActivationAnalysisRequest(int id, float search_interval, float activation_level);

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    std::string get_description(MetaInformation& meta_info) const override;

    std::shared_ptr<IntervalAnalysisRequest> clone() const override;

    void clear_recent_data() override;
};

#endif //RECORDINGPLUGIN_INTERVALSOUNDACTIVATIONANALYSISREQUEST_H
