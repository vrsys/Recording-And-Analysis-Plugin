//
// Created by Mortiferum on 29.02.2024.
//

#include "OrCombinedIntervalAnalysisRequest.h"

std::vector<TimeInterval> OrCombinedIntervalAnalysisRequest::get_result() const {
    return left_hand_side->get_result() || right_hand_side->get_result();
}

OrCombinedIntervalAnalysisRequest::OrCombinedIntervalAnalysisRequest(std::shared_ptr<IntervalAnalysisRequest>lhs, std::shared_ptr<IntervalAnalysisRequest> rhs)
        : left_hand_side(lhs), right_hand_side(rhs) {

}

std::string OrCombinedIntervalAnalysisRequest::get_description(MetaInformation &meta_info) const {
    return "(Or combined: " + left_hand_side->get_description(meta_info) + " - " + right_hand_side->get_description(meta_info) + " )";
}

void OrCombinedIntervalAnalysisRequest::update_parameters(MetaInformation &original_meta_file,
                                                          MetaInformation &new_meta_file) {
    left_hand_side->update_parameters(original_meta_file, new_meta_file);
    right_hand_side->update_parameters(original_meta_file, new_meta_file);
}

void OrCombinedIntervalAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data,
                                                        std::shared_ptr<SoundData> s_data) {
    left_hand_side->process_request(t_data, s_data);
    right_hand_side->process_request(t_data, s_data);
}

std::shared_ptr<IntervalAnalysisRequest> OrCombinedIntervalAnalysisRequest::clone() const {
    return std::make_shared<OrCombinedIntervalAnalysisRequest>(left_hand_side, right_hand_side);
}
