//
// Created by Mortiferum on 29.02.2024.
//

#ifndef RECORDINGPLUGIN_ORCOMBINEDINTERVALANALYSISREQUEST_H
#define RECORDINGPLUGIN_ORCOMBINEDINTERVALANALYSISREQUEST_H

#include "IntervalAnalysisRequest.h"

class OrCombinedIntervalAnalysisRequest : public IntervalAnalysisRequest{
private:
    std::shared_ptr<IntervalAnalysisRequest> left_hand_side;
    std::shared_ptr<IntervalAnalysisRequest> right_hand_side;

public:
    OrCombinedIntervalAnalysisRequest(std::shared_ptr<IntervalAnalysisRequest> lhs, std::shared_ptr<IntervalAnalysisRequest> rhs);

    std::vector<TimeInterval> get_result() const override;

    std::string get_description(MetaInformation& meta_info) const override;

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    std::shared_ptr<IntervalAnalysisRequest> clone() const override;

    void clear_recent_data() override{};
};


#endif //RECORDINGPLUGIN_ORCOMBINEDINTERVALANALYSISREQUEST_H
