//
// Created by Anton-Lammert on 27.10.2022.
//

#ifndef RECORDINGPLUGIN_ANALYSISSTRUCTS_H
#define RECORDINGPLUGIN_ANALYSISSTRUCTS_H

#include <algorithm>
#include <vector>

enum TransformAnalysisType{
    ContainmentAnalysis, DistanceAnalysis, GazeAnalysis, RotationAnalysis, VelocityAnalysis, PositionAdjustmentAnalysis
};

struct TimeBasedValue{
    float time;
    std::vector<float> values;
};

struct TimeInterval{
    float start = -1;
    float end = -1;

    TimeInterval operator && (TimeInterval const& a) const{
        float min_start = std::min(a.start, this->start);
        float max_start = std::max(a.start, this->start);
        float min_end = std::min(a.end, this->end);
        float max_end = std::max(a.end, this->end);

        if(max_start >= min_end){
            return TimeInterval{};
        } else {
            return TimeInterval{max_start, min_end};
        }
    }

    TimeInterval operator || (TimeInterval const& a) const{
        float min_start = std::min(a.start, this->start);
        float max_start = std::max(a.start, this->start);
        float min_end = std::min(a.end, this->end);
        float max_end = std::max(a.end, this->end);

        if(max_start >= min_end){
            return TimeInterval{};
        } else {
            return TimeInterval{min_start, max_end};
        }
    }

    bool operator <(TimeInterval const& a) const{
        return this->start < a.start;
    }

    bool contains(TimeInterval const& a) const{
        return !(a.end < this->start || a.start > this->end);
    }

    bool contains(float time) const{
        return time > this->start && time < this->end;
    }
};

inline std::vector<TimeInterval> operator || (std::vector<TimeInterval> const& a, std::vector<TimeInterval> const& b){
    std::vector<TimeInterval> or_combined_intervals;

    or_combined_intervals.insert(or_combined_intervals.end(), a.begin(), a.end());
    or_combined_intervals.insert(or_combined_intervals.end(), b.begin(), b.end());

    std::sort(or_combined_intervals.begin(), or_combined_intervals.end());

    int index = 0;

    for (int i = 1; i < or_combined_intervals.size(); i++) {
        if (or_combined_intervals[index].end >= or_combined_intervals[i].start)
            or_combined_intervals[index].end = std::max(or_combined_intervals[index].end, or_combined_intervals[i].end);
        else {
            index++;
            or_combined_intervals[index] = or_combined_intervals[i];
        }
    }

    or_combined_intervals.resize(index + 1);
    return or_combined_intervals;
}

inline std::vector<TimeInterval> operator ! (std::vector<TimeInterval> const& a){
    std::vector<TimeInterval> negated_interval;
    float last_end = 0.0f;
    for(auto i : a){
        negated_interval.push_back(TimeInterval{last_end, i.start});
        last_end = i.end;
    }
    negated_interval.push_back(TimeInterval{last_end, 9999999.0f});
    return negated_interval;
}

inline std::vector<TimeInterval> operator && (std::vector<TimeInterval> lhs, std::vector<TimeInterval> rhs){
    if(lhs.empty() || rhs.empty())
        return {};
    return !(!lhs || !rhs);
}
#endif //RECORDINGPLUGIN_ANALYSISSTRUCTS_H
