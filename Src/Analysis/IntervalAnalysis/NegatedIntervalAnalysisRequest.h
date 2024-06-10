//
// Created by Mortiferum on 29.02.2024.
//

#ifndef RECORDINGPLUGIN_NEGATEDINTERVALANALYSISREQUEST_H
#define RECORDINGPLUGIN_NEGATEDINTERVALANALYSISREQUEST_H

#include "IntervalAnalysisRequest.h"

class NegatedIntervalAnalysisRequest : public IntervalAnalysisRequest{
private:
    std::shared_ptr<IntervalAnalysisRequest> original_request;
public:
    explicit NegatedIntervalAnalysisRequest(std::shared_ptr<IntervalAnalysisRequest> request);

    std::vector<TimeInterval> get_result() const override;

    std::string get_description(MetaInformation& meta_info) const override;

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    std::shared_ptr<IntervalAnalysisRequest> clone() const override;

    void clear_recent_data() override{};
};


#endif //RECORDINGPLUGIN_NEGATEDINTERVALANALYSISREQUEST_H
