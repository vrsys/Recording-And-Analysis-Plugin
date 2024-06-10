//
// Created by Mortiferum on 26.02.2024.
//

#ifndef RECORDINGPLUGIN_QUANTITATIVEVELOCITYANALYSISREQUEST_H
#define RECORDINGPLUGIN_QUANTITATIVEVELOCITYANALYSISREQUEST_H

#include <deque>
#include "Recording/MetaInformation.h"
#include "QuantitativeTramsformAnalysisRequest.h"

class QuantitativeVelocityAnalysisRequest  : public QuantitativeTransformAnalysisRequest{

private:
    int id;

    std::deque<TransformData> recent_data;

public:
    QuantitativeVelocityAnalysisRequest(int i, float t_sampling_rate);

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    std::shared_ptr<QuantitativeAnalysisRequest> clone() const override;

    std::string get_description(MetaInformation& meta_info) const override;

    void clear_recent_data() override;

    TransformAnalysisType get_type() const override;

    int get_id() const;

    void set_id(int id);
};



#endif //RECORDINGPLUGIN_QUANTITATIVEVELOCITYANALYSISREQUEST_H
