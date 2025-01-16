//
// Created by Anton-Lammert on 16.01.2025.
//

#ifndef RECORDINGPLUGIN_QUANTITATIVESYNCHRONYANALYSISREQUEST_H
#define RECORDINGPLUGIN_QUANTITATIVESYNCHRONYANALYSISREQUEST_H

#include <deque>
#include "QuantitativeTransformAnalysisRequest.h"

class QuantitativeSynchronyAnalysisRequest : public QuantitativeTransformAnalysisRequest{

private:
    int id_a;
    int id_b;

    bool present_a = false;
    bool present_b = false;

    std::deque<TransformData> recent_data_a;
    std::deque<TransformData> recent_data_b;

    float current_t = 0;

public:
    QuantitativeSynchronyAnalysisRequest(int id_a, int id_b, float t_sampling_rate);

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    std::shared_ptr<QuantitativeAnalysisRequest> clone() const override;

    std::string get_description(MetaInformation& meta_info) const override;

    void clear_recent_data() override;

    TransformAnalysisType get_type() const override;

private:
    std::vector<float> rank_data(std::vector<float> const& data);

    float correlation(std::vector<float> const& a, std::vector<float> const& b);
};


#endif //RECORDINGPLUGIN_QUANTITATIVESYNCHRONYANALYSISREQUEST_H
