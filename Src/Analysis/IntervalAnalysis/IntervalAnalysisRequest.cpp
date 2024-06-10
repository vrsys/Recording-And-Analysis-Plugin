//
// Created by Mortiferum on 09.07.2023.
//

#include "IntervalAnalysisRequest.h"
#include "NegatedIntervalAnalysisRequest.h"
#include "OrCombinedIntervalAnalysisRequest.h"
#include "AndCombinedIntervalAnalysisRequest.h"

std::vector<TimeInterval> IntervalAnalysisRequest::get_result() const{
    return intervals;
}

void IntervalAnalysisRequest::clear_results() {
    intervals.clear();
}

std::shared_ptr<IntervalAnalysisRequest> operator!(std::shared_ptr<IntervalAnalysisRequest> lhs) {
    return std::make_shared<NegatedIntervalAnalysisRequest>(lhs);
}

std::shared_ptr<IntervalAnalysisRequest> operator&(std::shared_ptr<IntervalAnalysisRequest> lhs, std::shared_ptr<IntervalAnalysisRequest> rhs) {
    return std::make_shared<AndCombinedIntervalAnalysisRequest>(lhs, rhs);
}

std::shared_ptr<IntervalAnalysisRequest> operator|(std::shared_ptr<IntervalAnalysisRequest> lhs, std::shared_ptr<IntervalAnalysisRequest> rhs) {
    return std::make_shared<OrCombinedIntervalAnalysisRequest>(lhs, rhs);
}
