// VRSYS plugin of Virtual Reality and Visualization Group (Bauhaus-University Weimar)
//  _    ______  _______  _______
// | |  / / __ \/ ___/\ \/ / ___/
// | | / / /_/ /\__ \  \  /\__ \
// | |/ / _, _/___/ /  / /___/ /
// |___/_/ |_|/____/  /_//____/
//
//  __                            __                       __   __   __    ___ .  . ___
// |__)  /\  |  | |__|  /\  |  | /__`    |  | |\ | | \  / |__  |__) /__` |  |   /\   |
// |__) /~~\ \__/ |  | /~~\ \__/ .__/    \__/ | \| |  \/  |___ |  \ .__/ |  |  /~~\  |
//
//       ___               __
// |  | |__  |  |\/|  /\  |__)
// |/\| |___ |  |  | /~~\ |  \
//
// Copyright (c) 2024 Virtual Reality and Visualization Group
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//-----------------------------------------------------------------
//   Authors:        Anton Lammert
//   Date:           2024
//-----------------------------------------------------------------
#include "AnalysisManager.h"
#include "Analysis/IntervalAnalysis/TransformAnalysis/IntervalPositionAdjustmentAnalysisRequest.h"

void AnalysisManager::add_interval_analysis_request(int analysis_id, std::shared_ptr<IntervalAnalysisRequest> request, int logical_operation) {
    if(interval_analysis_queries.size() > analysis_id){
        if(interval_analysis_queries[analysis_id].get() == nullptr){
            switch(logical_operation) {
                case 0: {
                    interval_analysis_queries[analysis_id] = request;
                    break;
                }
                case 1: {
                    interval_analysis_queries[analysis_id] = request;
                    break;
                }
                case 2: {
                    interval_analysis_queries[analysis_id] = !request;
                    break;
                }
                case 3: {
                    interval_analysis_queries[analysis_id] = !request;
                    break;
                }
            }
        } else {
            switch(logical_operation){
                case 0:{
                    interval_analysis_queries[analysis_id] = interval_analysis_queries[analysis_id] & request;
                    break;
                }
                case 1:{
                    interval_analysis_queries[analysis_id] = interval_analysis_queries[analysis_id] | request;
                    break;
                }
                case 2:{
                    interval_analysis_queries[analysis_id] = interval_analysis_queries[analysis_id] & (!request);
                    break;
                }
                case 3:{
                    interval_analysis_queries[analysis_id] = interval_analysis_queries[analysis_id] | (!request);
                    break;
                }
            }
        }
    } else {
        int tmp = interval_analysis_queries.size();
        for (int i = tmp - 1; i < analysis_id; ++i) {
            if (i != analysis_id - 1)
                interval_analysis_queries.push_back(nullptr);
            else {
                switch (logical_operation) {
                    case 0: {
                        interval_analysis_queries.push_back(request);
                        break;
                    }
                    case 1: {
                        interval_analysis_queries.push_back(request);
                        break;
                    }
                    case 2: {
                        interval_analysis_queries.push_back(!request);
                        break;
                    }
                    case 3: {
                        interval_analysis_queries.push_back(!request);
                        break;
                    }
                }
            }
        }
    }
}

int AnalysisManager::process_interval_analysis_request(std::shared_ptr<IntervalAnalysisRequest> analysis_request,
                                                       float *intervals, std::string const &file,
                                                       std::vector<TimeInterval> const &intervals_to_investigate) const {

    std::ifstream transform_file(file + ".txt", std::ios::in | std::ios::binary);
    std::ifstream sound_file(file + "_sound.txt", std::ios::in | std::ios::binary);
    MetaInformation meta_info{file + ".recordmeta"};
    std::vector<TimeInterval> time_intervals;

    analysis_request->clear_results();
    analysis_request->clear_recent_data();

    // TODO: this has to be modified to also support analysis of compressed data
    if (transform_file.good()) {
        std::cout << "Reading transforms for analysis\n";
        TransformDTO transform;

        transform_file.seekg(0, std::ifstream::end);
        unsigned long size = transform_file.tellg() / sizeof(TransformDTO);
        transform_file.seekg(0, std::ifstream::beg);

        Debug::Log(analysis_request->get_description(meta_info));

        for (unsigned long i = 0; i < size; i++) {
            transform_file.read((char *) &transform, sizeof(TransformDTO));
            auto transform_data = std::make_shared<TransformData>(transform);
            std::shared_ptr<SoundData> sound_data(nullptr);
            analysis_request->process_request(transform_data, sound_data);
        }

        transform_file.close();
        Debug::Log("Reading transforms for analysis finished\n");
    }

    if (sound_file.good()) {
        SoundDTO sound;

        std::cout << "Reading sounds for analysis\n";
        sound_file.seekg(0, std::ifstream::end);
        unsigned long size = sound_file.tellg() / sizeof(SoundDTO);
        sound_file.seekg(0, std::ifstream::beg);

        Debug::Log(analysis_request->get_description(meta_info));

        for (unsigned long p = 0; p < size; p++) {
            sound_file.read((char *) &sound, sizeof(SoundDTO));
            auto sound_data = std::make_shared<SoundData>(sound);
            std::shared_ptr<TransformData> transform_data(nullptr);
            analysis_request->process_request(transform_data, sound_data);

        }

        sound_file.close();
        Debug::Log("Reading sounds for analysis finished\n");
    }

    time_intervals = analysis_request->get_result();
    if (debug) {
        for (auto &time_interval: time_intervals) {
            std::cout << "Start: " << time_interval.start << ", End: " << time_interval.end << "\n";
        }
        std::cout << "----------------------------------------\n";
    }

    std::vector<TimeInterval> relevant_and_combined_intervals;
    for (auto &interval: time_intervals) {
        if (!intervals_to_investigate.empty()) {
            for (auto const &interval_of_interest: intervals_to_investigate) {
                if (interval_of_interest.contains(interval))
                    relevant_and_combined_intervals.push_back(interval_of_interest && interval);
            }
        } else {
            relevant_and_combined_intervals.push_back(interval);
        }
    }

    int i = 0;
    for (auto &relevant_and_combined_interval: relevant_and_combined_intervals) {
        intervals[i] = relevant_and_combined_interval.start;
        intervals[i + 1] = relevant_and_combined_interval.end;
        i += 2;
    }
    return relevant_and_combined_intervals.size();
}

AnalysisManager::AnalysisManager() {

}

void AnalysisManager::clear_requests() {
    interval_analysis_queries.clear();
}

int
AnalysisManager::get_object_positions(int object_id, float start_time, float end_time, float *positions,
                                      int max_positions) {
    if (recording_file_paths.empty())
        return -1;

    Debug::Log("Processing position retrieval request for recording file: " + recording_file_paths.front());
    std::ifstream transform_file(recording_file_paths.front() + ".txt", std::ios::in | std::ios::binary);

    std::vector<glm::vec3> all_positions;
    std::vector<float> all_times;

    if (transform_file.good()) {
        TransformDTO transform;

        std::cout << "Reading transforms for analysis\n";
        transform_file.seekg(0, std::ifstream::end);
        unsigned long size = transform_file.tellg() / sizeof(TransformDTO);
        transform_file.seekg(0, std::ifstream::beg);

        for (unsigned long i = 0; i < size; i++) {
            transform_file.read((char *) &transform, sizeof(TransformDTO));
            auto transform_data = std::make_shared<TransformData>(transform);
            if (transform_data->id == object_id && transform_data->time >= start_time &&
                transform_data->time <= end_time) {
                all_positions.emplace_back(transform_data->global_position);
                all_times.emplace_back(transform_data->time);
            }
        }

        int offset = 1;
        if (all_positions.size() > max_positions) {
            offset = std::ceil(all_positions.size() / (float) max_positions);
        }

        int index = 0;
        for (int i = 0; i < all_positions.size(); i += offset) {
            positions[index] = all_positions[i].x;
            positions[index + 1] = all_positions[i].y;
            positions[index + 2] = all_positions[i].z;
            positions[index + 3] = all_times[i];
            index += 4;
        }

        transform_file.close();
        Debug::Log("Reading transforms for analysis finished\n");

        return index / 4;
    }

    return 0;
}

void AnalysisManager::add_recording_path(char *path, int path_length) {
    std::string file_path = "";
    for (int i = 0; i < path_length; ++i) {
        file_path += path[i];
    }

    bool found = false;
    for (auto &recording_file_path: recording_file_paths) {
        if (recording_file_path == file_path)
            found = true;
    }

    if (!found)
        recording_file_paths.push_back(file_path);
}

void AnalysisManager::clear_recording_paths() {
    recording_file_paths.clear();
}

int AnalysisManager::process_interval_analysis_requests_for_all_files() {
    if (recording_file_paths.empty())
        return -1;

    MetaInformation meta_information{recording_file_paths.front() + ".recordmeta"};

    for (auto const &file: recording_file_paths) {
        //process_interval_analysis_requests_for_file(file, meta_information);
        std::vector<TimeInterval> intervals_to_investigate;
        if (intervals_of_interest.count(file) != 0) {
            intervals_to_investigate = intervals_of_interest[file];
        }
        getThreadPool().submit_task([this, &file, &meta_information, &intervals_to_investigate]{this->process_interval_analysis_requests_for_file(file, meta_information,intervals_to_investigate);});
    }

    return 1;
}

int AnalysisManager::process_interval_analysis_requests_for_primary_file(int analysis_id, float *intervals,
                                                                         std::vector<TimeInterval> intervals_of_interest) {
    if (recording_file_paths.empty())
        return -1;
    if(interval_analysis_queries.size() > analysis_id && analysis_id >= 0)
        return process_interval_analysis_request(interval_analysis_queries[analysis_id], intervals, recording_file_paths.front(), intervals_of_interest);
    return -1;
}

std::string AnalysisManager::get_primary_file() const {
    return recording_file_paths.front();
}

void
AnalysisManager::process_interval_analysis_requests_for_file(std::string const &file, MetaInformation &meta_information,
                                                             std::vector<TimeInterval> const &intervals_of_interest) const {
    MetaInformation current_meta_information{file + ".recordmeta"};

    std::fstream out;

    auto pos = file.find_last_of('\\');
    std::string file_only = file;
    if (pos != std::string::npos)
        file_only = file.substr(pos + 1);

    std::string primary_file = get_primary_file();
    std::string path =
            primary_file.substr(0, primary_file.find_last_of("\\")) + "\\" + file_only +
            "_interval_requests_results.csv";
    out.open(path, std::fstream::out);
    if (!out.good())
        return;

    std::vector<std::shared_ptr<IntervalAnalysisRequest>> adapted_queries;

    for (auto const &request: interval_analysis_queries) {
        std::shared_ptr<IntervalAnalysisRequest> adapted_request = request->clone();
        adapted_request->update_parameters(meta_information, current_meta_information);
        adapted_queries.push_back(adapted_request);
    }

    for (auto request: adapted_queries) {
        float intervals[5000];
        int interval_count = process_interval_analysis_request(request, intervals, file, intervals_of_interest);
        std::cout << "Interval count: " << interval_count << "\n";

        out << "AnalysisQuery,";
        out << request->get_description(current_meta_information) << ",";

        out << "\n\n";
        out << "StartTime,";
        out << "EndTime\n";

        for (int i = 0; i < interval_count; ++i) {
            float start_time = intervals[i * 2];
            float end_time = intervals[i * 2 + 1];
            if (start_time > end_time) {
                std::cout << "Error: " << start_time << ", " << end_time << "\n";
            }

            if (i < interval_count - 1) {
                float next_start_time = intervals[(i + 1) * 2];
                if (end_time >= next_start_time) {
                    std::cout << "Error for next: " << end_time << ", " << next_start_time << "\n";
                }
            }
            out << start_time << "," << end_time << "\n";
        }

        out << "\n\n";
    }
    out.close();
}

void AnalysisManager::add_intervals_to_investigate(std::string path, std::vector<TimeInterval> intervals) {
    intervals_of_interest[path] = intervals;
}

void AnalysisManager::add_quantitative_analysis_request(std::shared_ptr<QuantitativeAnalysisRequest> request) {
    quantitative_analysis_queries.push_back(request);
}

int AnalysisManager::process_quantitative_analysis_request(std::shared_ptr<QuantitativeAnalysisRequest> analysis_request,
                                                       float *values, const std::string &file,
                                                       const std::vector<TimeInterval> &intervals_to_investigate) const {
    std::ifstream transform_file(file + ".txt", std::ios::in | std::ios::binary);
    std::ifstream sound_file(file + "_sound.txt", std::ios::in | std::ios::binary);
    MetaInformation meta_info{file + ".recordmeta"};
    std::vector<TimeBasedValue> result_values;

    analysis_request->clear_results();
    // TODO: this has to be modified to also support analysis of compressed data
    if (transform_file.good()) {
        std::cout << "Reading transforms for analysis\n";
        TransformDTO transform;

        transform_file.seekg(0, std::ifstream::end);
        unsigned long size = transform_file.tellg() / sizeof(TransformDTO);
        transform_file.seekg(0, std::ifstream::beg);

        Debug::Log(analysis_request->get_description(meta_info));

        for (unsigned long i = 0; i < size; i++) {
            transform_file.read((char *) &transform, sizeof(TransformDTO));
            auto transform_data = std::make_shared<TransformData>(transform);
            std::shared_ptr<SoundData> sound_data(nullptr);
            analysis_request->process_request(transform_data, sound_data);
        }

        transform_file.close();
        Debug::Log("Reading transforms for analysis finished\n");
    }

    if (sound_file.good()) {
        SoundDTO sound;

        std::cout << "Reading sounds for analysis\n";
        sound_file.seekg(0, std::ifstream::end);
        unsigned long size = sound_file.tellg() / sizeof(SoundDTO);
        sound_file.seekg(0, std::ifstream::beg);

        Debug::Log(analysis_request->get_description(meta_info));

        for (unsigned long p = 0; p < size; p++) {
            sound_file.read((char *) &sound, sizeof(SoundDTO));
            auto sound_data = std::make_shared<SoundData>(sound);
            std::shared_ptr<TransformData> transform_data(nullptr);
            analysis_request->process_request(transform_data, sound_data);

        }

        sound_file.close();
        Debug::Log("Reading sounds for analysis finished\n");
    }

    result_values = analysis_request->get_result();
    int i = 0;
    for (auto &time_based_value: result_values) {
        if (!intervals_to_investigate.empty()) {
            for (auto const &interval_of_interest: intervals_to_investigate) {
                if (interval_of_interest.contains(time_based_value.time)) {
                    values[i] = time_based_value.time;
                    // TODO: handle return of more return values
                    values[i + 1] = time_based_value.values[0];
                    i += 2;
                }
            }
        } else {
            values[i] = time_based_value.time;
            values[i + 1] = time_based_value.values[0];
            i += 2;
        }
    }
    return result_values.size();
}

int AnalysisManager::process_quantitative_analysis_requests_for_all_files() {
    if (recording_file_paths.empty())
        return -1;

    MetaInformation meta_information{recording_file_paths.front() + ".recordmeta"};

    for (auto const &file: recording_file_paths) {
        //process_interval_analysis_requests_for_file(file, meta_information);
        std::vector<TimeInterval> intervals_to_investigate;
        if (intervals_of_interest.count(file) != 0) {
            intervals_to_investigate = intervals_of_interest[file];
        }
        getThreadPool().submit_task([this, &file, &meta_information, &intervals_to_investigate]{this->process_quantitative_analysis_requests_for_file(file,
                                  meta_information, intervals_to_investigate);});
    }

    return 0;
}

void AnalysisManager::process_quantitative_analysis_requests_for_file(const std::string &file,
                                                                      MetaInformation &meta_information,
                                                                      const std::vector<TimeInterval> &intervals_of_interest) const {
    MetaInformation current_meta_information{file + ".recordmeta"};

    std::fstream out;

    auto pos = file.find_last_of('\\');
    std::string file_only = file;
    if (pos != std::string::npos)
        file_only = file.substr(pos + 1);

    std::string primary_file = get_primary_file();
    std::string path = primary_file.substr(0, primary_file.find_last_of("\\")) + "\\" + file_only +
                       "_quantitative_requests_results.csv";
    out.open(path, std::fstream::out);
    if (!out.good())
        return;

    std::vector<std::shared_ptr<QuantitativeAnalysisRequest>> adapted_queries;

    for (auto const &request: quantitative_analysis_queries) {
        std::shared_ptr<QuantitativeAnalysisRequest> adapted_request = request->clone();
        adapted_request->update_parameters(meta_information, current_meta_information);
        adapted_queries.push_back(adapted_request);
    }

    for (auto request: adapted_queries) {
        float values[5000];
        int value_count = process_quantitative_analysis_request(request, values, file, intervals_of_interest);
        std::cout << "Value count: " << value_count << "\n";

        out << "AnalysisQuery,";
        out << request->get_description(current_meta_information) << ",";

        out << "\n\n";
        out << "Time,";
        out << "Value\n";

        for (int i = 0; i < value_count; ++i) {
            float time = values[i * 2];
            float value = values[i * 2 + 1];
            out << time << "," << value << "\n";
        }

        out << "\n\n";
    }
    out.close();
}

int AnalysisManager::process_quantitative_analysis_requests_for_primary_file(int analysis_id, float *values,
                                                                             std::vector<TimeInterval> intervals_of_interest) {
    if (recording_file_paths.empty())
        return -1;
    return process_interval_analysis_request(interval_analysis_queries[analysis_id], values,
                                             recording_file_paths.front(), intervals_of_interest);
}
