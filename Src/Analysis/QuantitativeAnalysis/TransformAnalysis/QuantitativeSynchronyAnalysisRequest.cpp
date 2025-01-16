//
// Created by Anton-Lammert on 16.01.2025.
//

#include "QuantitativeSynchronyAnalysisRequest.h"

QuantitativeSynchronyAnalysisRequest::QuantitativeSynchronyAnalysisRequest(int id_a, int id_b, float t_sampling_rate)
        : QuantitativeTransformAnalysisRequest(t_sampling_rate) {
    this->id_a = id_a;
    this->id_b = id_b;
}

void QuantitativeSynchronyAnalysisRequest::update_parameters(MetaInformation &original_meta_file,
                                                             MetaInformation &new_meta_file) {
    id_a = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id_a));
    id_b = new_meta_file.get_old_uuid(original_meta_file.get_object_name(id_b));
}

void QuantitativeSynchronyAnalysisRequest::process_request(std::shared_ptr<TransformData> t_data,
                                                           std::shared_ptr<SoundData> s_data) {
    if (!t_data)
        return;

    if (t_data->id == id_a) {
        recent_data_a.emplace_back(t_data);
    }
    if (t_data->id == id_b) {
        recent_data_b.emplace_back(t_data);
    }

    if (t_data->time > current_t) {
        current_t = t_data->time;
        while (recent_data_a.front().time < current_t - temporal_sampling_rate) {
            recent_data_a.pop_front();
        }
        while (recent_data_b.front().time < current_t - temporal_sampling_rate) {
            recent_data_b.pop_front();
        }
        std::vector<float> a_velocities;
        if (recent_data_a.size() > 1) {
            for (int i = 1; i < recent_data_a.size(); i++) {
                TransformData last = recent_data_a[i - 1];
                TransformData current = recent_data_a[i];
                float pos_diff = glm::length(current.global_position - last.global_position);
                float time_diff = current.time - last.time;
                a_velocities.push_back(std::abs(pos_diff / time_diff));
            }
        }

        std::vector<float> b_velocities;
        if (recent_data_b.size() > 1) {
            for (int i = 1; i < recent_data_b.size(); i++) {
                TransformData last = recent_data_b[i - 1];
                TransformData current = recent_data_b[i];
                float pos_diff = glm::length(current.global_position - last.global_position);
                float time_diff = current.time - last.time;
                b_velocities.push_back(std::abs(pos_diff / time_diff));
            }
        }

        std::vector<float> a_ranks = rank_data(a_velocities);
        std::vector<float> b_ranks = rank_data(b_velocities);

        float spearman_corr = correlation(a_ranks, b_ranks);
        if(current_t > last_value_time + (1.0f /temporal_sampling_rate)) {
            values.push_back(TimeBasedValue{current_t, {spearman_corr}});
            last_value_time = current_t;
        }
    }
}

std::shared_ptr<QuantitativeAnalysisRequest> QuantitativeSynchronyAnalysisRequest::clone() const {
    return std::make_shared<QuantitativeSynchronyAnalysisRequest>(id_a, id_b, temporal_sampling_rate);
}

std::string QuantitativeSynchronyAnalysisRequest::get_description(MetaInformation &meta_info) const {
    std::string s = "SynchronyAnalysis";
    s += " id_a: " + meta_info.get_object_name(id_a) + "-";
    s += " id_b: " + meta_info.get_object_name(id_b) + "-";
    s += " temporal sampling rate: " + std::to_string(temporal_sampling_rate);
    return s;
}

void QuantitativeSynchronyAnalysisRequest::clear_recent_data() {
    present_a = false;
    present_b = false;
    recent_data_a.clear();
    recent_data_b.clear();
    current_t = 0;
}

TransformAnalysisType QuantitativeSynchronyAnalysisRequest::get_type() const {
    return SynchronyAnalysis;
}

// see: https://www.geeksforgeeks.org/program-spearmans-rank-correlation/
std::vector<float> QuantitativeSynchronyAnalysisRequest::rank_data(std::vector<float> const &data) {
    std::vector<float> ranks(data.size());
    float threshold = 0.1f;
    for (int i = 0; i < data.size(); i++) {
        int rank = 1, same = 1;

        for (int j = 0; j < i; j++) {
            if (data[j] < data[i]) rank++;
            if (std::abs(data[j] - data[i]) < threshold) same++;
        }

        for (int j = i + 1; j < data.size(); j++) {
            if (data[j] < data[i]) rank++;
            if (std::abs(data[j] - data[i]) < threshold) same++;
        }

        ranks[i] = rank + (same - 1) * 0.5f;
    }
    return ranks;
}

float QuantitativeSynchronyAnalysisRequest::correlation(const std::vector<float> &a, const std::vector<float> &b) {
    if(a.size() != b.size()) {
        Debug::Log("Error: Cannot calculate correlation between vectors of different sizes");
        return 0;
    }
    float sum_a, sum_b, sum_ab, square_sum_a, square_sum_b = 0;

    for (int i = 0; i < a.size(); i++) {
        sum_a = sum_a + a[i];
        sum_b = sum_b + b[i];
        sum_ab = sum_ab + a[i] * b[i];
        square_sum_a = square_sum_a + a[i] * a[i];
        square_sum_b = square_sum_b + b[i] * b[i];
    }

    float corr = (float) (a.size() * sum_ab - sum_a * sum_b) /
                 sqrt((a.size() * square_sum_a - sum_a * sum_a) * (a.size() * square_sum_b - sum_b * sum_b));

    return corr;
}
