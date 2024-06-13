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

#include "Analysis/AnalysisManager.h"

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AddAnalysisRecordingPath(char* path, int path_length) {
    AnalysisManager& manager = AnalysisManager::getInstance();
    manager.add_recording_path(path, path_length);
}
/**
 *  @param logical_operation an int defining whether the analysis request will be combined with the current query using &, |, &! or |!
 */
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AddDistanceAnalysisRequest(int analysis_id, int id_a, int id_b, float distance, int logical_operation) {
    AnalysisManager& manager = AnalysisManager::getInstance();
    std::shared_ptr<IntervalTransformAnalysisRequest> request = std::make_shared<IntervalDistanceAnalysisRequest>(id_a, id_b, distance);
    manager.add_interval_analysis_request(analysis_id, request, logical_operation);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AddContainmentAnalysisRequest(int analysis_id, int id_a,  float* min_max, int logical_operation) {
    AnalysisManager& manager = AnalysisManager::getInstance();
    std::shared_ptr<IntervalContainmentAnalysisRequest> request = std::make_shared<IntervalContainmentAnalysisRequest>(id_a, glm::vec3{min_max[0], min_max[1], min_max[2]}, glm::vec3{min_max[3], min_max[4], min_max[5]});
    manager.add_interval_analysis_request(analysis_id, request, logical_operation);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AddGazeAnalysisRequest(int analysis_id, int id_a, int id_b, float cone_angle, float distance, int logical_operation) {
    AnalysisManager& manager = AnalysisManager::getInstance();
    std::shared_ptr<IntervalGazeAnalysisRequest> request = std::make_shared<IntervalGazeAnalysisRequest>(id_a, id_b, cone_angle, distance);
    manager.add_interval_analysis_request(analysis_id, request, logical_operation);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AddGazeAnalysisRequestAx(int analysis_id, int id_a, int id_b, float cone_angle, float distance, int axis, int logical_operation) {
    AnalysisManager& manager = AnalysisManager::getInstance();
    std::shared_ptr<IntervalGazeAnalysisRequest> request = std::make_shared<IntervalGazeAnalysisRequest>(id_a, id_b, cone_angle, distance, axis);
    manager.add_interval_analysis_request(analysis_id, request, logical_operation);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AddRotationAnalysisRequest(int analysis_id, int id_a, float temporal_search_interval, float rotation_threshold, int logical_operation) {
    AnalysisManager& manager = AnalysisManager::getInstance();
    std::shared_ptr<IntervalRotationAnalysisRequest> request = std::make_shared<IntervalRotationAnalysisRequest>(id_a, temporal_search_interval, rotation_threshold);
    manager.add_interval_analysis_request(analysis_id, request, logical_operation);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AddSoundActivationAnalysisRequest(int analysis_id, int sound_id, float temporal_search_interval, float activation_level, int logical_operation) {
    AnalysisManager& manager = AnalysisManager::getInstance();
    std::shared_ptr<IntervalSoundActivationAnalysisRequest> request = std::make_shared<IntervalSoundActivationAnalysisRequest>(sound_id, temporal_search_interval, activation_level);
    manager.add_interval_analysis_request(analysis_id, request, logical_operation);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API AddVelocityAnalysisRequest(int analysis_id, int id_a, float temporal_search_interval, float velocity_threshold, int logical_operation) {
    AnalysisManager& manager = AnalysisManager::getInstance();
    std::shared_ptr<IntervalVelocityAnalysisRequest> request = std::make_shared<IntervalVelocityAnalysisRequest>(id_a, temporal_search_interval, velocity_threshold);
    manager.add_interval_analysis_request(analysis_id, request, logical_operation);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API ProcessAnalysisRequests(int analysis_id, float* intervals) {
    AnalysisManager& manager = AnalysisManager::getInstance();
    // if empty the whole recording is analysed
    std::vector<TimeInterval> intervals_of_interest;
    return manager.process_interval_analysis_requests_for_primary_file(analysis_id, intervals, intervals_of_interest);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API ProcessAnalysisRequestsForIntervals(int analysis_id, float* intervals, std::vector<TimeInterval> intervals_of_interest) {
    AnalysisManager& manager = AnalysisManager::getInstance();
    return manager.process_interval_analysis_requests_for_primary_file(analysis_id, intervals, intervals_of_interest);
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API ProcessAnalysisRequestsForAllFiles() {
    AnalysisManager& manager = AnalysisManager::getInstance();
    return manager.process_interval_analysis_requests_for_all_files();
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetObjectPositions(int analysis_id, int object_id, float start_time, float end_time, float* positions, int max_positions) {
    AnalysisManager& manager = AnalysisManager::getInstance();
    return manager.get_object_positions(object_id, start_time, end_time, positions, max_positions);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API ClearAnalysisRequests(int analysis_id) {
    AnalysisManager& manager = AnalysisManager::getInstance();
    return manager.clear_requests();
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API ClearAnalysisFilePaths() {
    AnalysisManager& manager = AnalysisManager::getInstance();
    return manager.clear_recording_paths();
}