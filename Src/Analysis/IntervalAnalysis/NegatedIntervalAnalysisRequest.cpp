//
// Created by Mortiferum on 29.02.2024.
//

#include "NegatedIntervalAnalysisRequest.h"

std::vector<TimeInterval> NegatedIntervalAnalysisRequest::get_result() const {
    return !original_request->get_result();
}

NegatedIntervalAnalysisRequest::NegatedIntervalAnalysisRequest(std::shared_ptr<IntervalAnalysisRequest> request) : original_request(request) {

}

std::string NegatedIntervalAnalysisRequest::get_description(MetaInformation &meta_info) const {
    return "Negated: " + original_request->get_description(meta_info);
}

void NegatedIntervalAnalysisRequest::update_parameters(MetaInformation &original_meta_file, MetaInformation &new_meta_file) {
    original_request->update_parameters(original_meta_file, new_meta_file);
}

void NegatedIntervalAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data,
                                                     std::shared_ptr<SoundData> s_data) {
    original_request->process_request(t_data, s_data);
}

std::shared_ptr<IntervalAnalysisRequest> NegatedIntervalAnalysisRequest::clone() const {
    return std::make_shared<NegatedIntervalAnalysisRequest>(original_request);
}
