//
// Created by Admin on 07.03.2024.
//

#ifndef RECORDINGPLUGIN_QUANTITATIVEDISTANCEANALYSISREQUEST_H
#define RECORDINGPLUGIN_QUANTITATIVEDISTANCEANALYSISREQUEST_H


#include <deque>
#include "QuantitativeTramsformAnalysisRequest.h"

class QuantitativeDistanceAnalysisRequest : public QuantitativeTransformAnalysisRequest{

private:
    int id_a;
    int id_b;

    bool present_a = false;
    bool present_b = false;

    TransformData current_a;
    TransformData current_b;
    TransformData last_a;
    TransformData last_b;

public:
    QuantitativeDistanceAnalysisRequest(int id_a, int id_b, float t_sampling_rate);

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    std::shared_ptr<QuantitativeAnalysisRequest> clone() const override;

    std::string get_description(MetaInformation& meta_info) const override;

    void clear_recent_data() override;

    TransformAnalysisType get_type() const override;
};

#endif //RECORDINGPLUGIN_QUANTITATIVEDISTANCEANALYSISREQUEST_H
