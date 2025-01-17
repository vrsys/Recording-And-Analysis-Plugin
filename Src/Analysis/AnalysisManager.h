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

#ifndef RECORDINGPLUGIN_ANALYSISMANAGER_H
#define RECORDINGPLUGIN_ANALYSISMANAGER_H

#include "Analysis/IntervalAnalysis/TransformAnalysis/IntervalDistanceAnalysisRequest.h"
#include "Analysis/IntervalAnalysis/TransformAnalysis/IntervalContainmentAnalysisRequest.h"
#include "Analysis/IntervalAnalysis/TransformAnalysis/IntervalGazeAnalysisRequest.h"
#include "Analysis/IntervalAnalysis/TransformAnalysis/IntervalRotationAnalysisRequest.h"
#include "Analysis/IntervalAnalysis/SoundAnalysis/IntervalSoundActivationAnalysisRequest.h"
#include "Analysis/IntervalAnalysis/TransformAnalysis/IntervalVelocityAnalysisRequest.h"
#include "Analysis/IntervalAnalysis/TransformAnalysis/IntervalTransformAnalysisRequest.h"
#include "Analysis/IntervalAnalysis/SoundAnalysis/IntervalSoundAnalysisRequest.h"
#include "Recording/MetaInformation.h"
#include "../../External/thread-pool/include/BS_thread_pool.hpp"
#include "Analysis/QuantitativeAnalysis/QuantitativeAnalysisRequest.h"
#include <vector>
#include <set>
#include <filesystem>

/**
 * @class AnalysisManager
 * @brief This class can be used to analyse a recording based on different analysis requests.
 */
class AnalysisManager {
public:
    static AnalysisManager& getInstance(){
        static AnalysisManager instance;
        return instance;
    }
    
    AnalysisManager();

    AnalysisManager(AnalysisManager const&) = delete;

    void operator=(AnalysisManager const&) = delete;

    void add_recording_path(char* path, int path_length);

    void add_intervals_to_investigate(std::string path, std::vector<TimeInterval> intervals);

    void add_interval_analysis_request(int analysis_id, std::shared_ptr<IntervalAnalysisRequest> request, int logical_operation);

    void add_interval_analysis_request(std::shared_ptr<IntervalAnalysisRequest> request);

    int process_interval_analysis_request(std::shared_ptr<IntervalAnalysisRequest> analysis_request, float* intervals, std::string const& file, std::vector<TimeInterval> const& intervals_to_investigate) const;

    int process_interval_analysis_requests_for_all_files();

    void process_interval_analysis_requests_for_file(std::string const& file, MetaInformation& meta_information, std::vector<TimeInterval> const& intervals_of_interest) const;

    int process_interval_analysis_requests_for_primary_file(int analysis_id, float* intervals, std::vector<TimeInterval> intervals_of_interest);

    void add_quantitative_analysis_request(std::shared_ptr<QuantitativeAnalysisRequest> request);

    std::vector<TimeBasedValue> process_quantitative_analysis_request(std::shared_ptr<QuantitativeAnalysisRequest> analysis_request, std::string const& file) const;

    int process_quantitative_analysis_requests_for_all_files();

    void process_quantitative_analysis_requests_for_file(std::string file, MetaInformation meta_information, std::vector<TimeInterval> intervals_of_interest) const;

    int process_quantitative_analysis_requests_for_primary_file(int analysis_id, float* values, std::vector<TimeInterval> intervals_of_interest);

    int get_object_positions(int object_id, float start_time, float end_time, float* positions, int max_positions);

    void clear_requests();

    std::string get_primary_file() const;

    void clear_recording_paths();

private:
    bool debug = false;

    std::vector<std::shared_ptr<IntervalAnalysisRequest>> interval_analysis_queries;
    std::vector<std::shared_ptr<QuantitativeAnalysisRequest>> quantitative_analysis_queries;
    std::vector<std::string> recording_file_paths;
    std::map<std::string, std::vector<TimeInterval>> intervals_of_interest;

    static BS::thread_pool& getThreadPool(){
        static BS::thread_pool thread_pool(16);
        return thread_pool;
    }
};

#endif //RECORDINGPLUGIN_ANALYSISMANAGER_H
