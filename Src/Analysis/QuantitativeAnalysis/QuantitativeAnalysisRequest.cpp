//
// Created by Mortiferum on 26.02.2024.
//

#include "QuantitativeAnalysisRequest.h"

std::vector<TimeBasedValue> QuantitativeAnalysisRequest::get_result() const{
    return values;
}

void QuantitativeAnalysisRequest::clear_results() {
    values.clear();
}
