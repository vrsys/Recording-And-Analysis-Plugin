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
#include "Analysis/IntervalAnalysis/TransformAnalysis/IntervalPositionAdjustmentAnalysisRequest.h"
#include "Analysis/QuantitativeAnalysis/TransformAnalysis/QuantitativeTransformAnalysisRequest.h"
#include "Analysis/QuantitativeAnalysis/TransformAnalysis/QuantitativeVelocityAnalysisRequest.h"
#include "Analysis/QuantitativeAnalysis/TransformAnalysis/QuantitativeGazeAnalysisRequest.h"
#include "Analysis/QuantitativeAnalysis/TransformAnalysis/QuantitativeDistanceAnalysisRequest.h"
#include "Analysis/QuantitativeAnalysis/TransformAnalysis/QuantitativeRotationAnalysisRequest.h"
#include "Analysis/QuantitativeAnalysis/TransformAnalysis/QuantitativeSynchronyAnalysisRequest.h"
#include <filesystem>
#include <string>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

#include <cmath>
#include <iostream>


void collaboration_analysis() {
    std::vector<std::string> files;
    std::string directory = "C:\\Users\\Admin\\CLionProjects\\UnityRecordingPlugin\\cmake-build-debug\\";
    if(!std::filesystem::exists(directory))
        return;

    for (const auto &entry: std::filesystem::directory_iterator(directory)) {
        std::string path = entry.path().string();
        if (path.substr(path.find_last_of(".") + 1) == "recordmeta") {
            files.push_back(path.substr(0, path.size() - 11));
        }
    }

    AnalysisManager &manager = AnalysisManager::getInstance();

    for (auto f: files) {
        manager.add_recording_path(f.data(), f.length());
    }

    std::string primary_file = manager.get_primary_file();
    MetaInformation meta_information{primary_file + ".recordmeta"};

    //int participant_a = meta_information.get_old_uuid("/Anton [Remote]");
    //int participant_b = meta_information.get_old_uuid("/Anton [Local]");
    //int participant_a_hand_left = meta_information.get_old_uuid("/Anton [Remote]/OVRCameraRig/TrackingSpace/LeftHandAnchor/LeftHand Controller/LeftHand Geometry/HandLeft");
    //int participant_a_hand_right = meta_information.get_old_uuid("/Anton [Remote]/OVRCameraRig/TrackingSpace/RightHandAnchor/RightHand Controller/RightHand Geometry/HandRight");
    //int participant_b_hand_left = meta_information.get_old_uuid("/Anton [Local]/OVRCameraRig/TrackingSpace/LeftHandAnchor/LeftHand Controller/LeftHand Geometry/HandLeft");
    //int participant_b_hand_right = meta_information.get_old_uuid("/Anton [Local]/OVRCameraRig/TrackingSpace/RightHandAnchor/RightHand Controller/RightHand Geometry/HandRight");

    int participant_a = meta_information.get_old_uuid("Test");
    int participant_b = meta_information.get_old_uuid("Test2");
    int participant_a_hand_left = meta_information.get_old_uuid("Test3");
    int participant_a_hand_right = meta_information.get_old_uuid("Test4");
    int participant_b_hand_left = meta_information.get_old_uuid("Test5");
    int participant_b_hand_right = meta_information.get_old_uuid("Test6");


    int microphone_audio = 0;
    int world_audio = 1;

    float distance = 3.0f;
    float sound_level = 0.005f;
    float velocity_threshold = 0.2f;
    float rotation_threshold = 10.0f;
    float distance_threshold = 1.2f;
    float hand_distance_threshold = 0.4f;
    float temporal_search_interval = 1.0f;
    float position_adjustment_distance = 0.3f;
    float gaze_cone_height = 10.0f;
    float gaze_cone_fov_angle = 60.0f;
    float gaze_cone_angle = gaze_cone_fov_angle / 2.0f;
    float gaze_cone_radius = std::tan(gaze_cone_angle * (3.14159f / 180.0f)) * gaze_cone_height;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // when are users talking
    std::shared_ptr<IntervalSoundActivationAnalysisRequest> sound_request_1 = std::make_shared<IntervalSoundActivationAnalysisRequest>(microphone_audio, temporal_search_interval, sound_level);
    std::shared_ptr<IntervalSoundActivationAnalysisRequest> sound_request_2 = std::make_shared<IntervalSoundActivationAnalysisRequest>(world_audio, temporal_search_interval, sound_level);
    manager.add_interval_analysis_request(sound_request_1);
    manager.add_interval_analysis_request(sound_request_2);
    manager.add_interval_analysis_request(sound_request_1 & sound_request_2);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // when are users looking at each other
    std::shared_ptr<IntervalGazeAnalysisRequest> gaze_request_1 = std::make_shared<IntervalGazeAnalysisRequest>(participant_a, participant_b, gaze_cone_radius, gaze_cone_height);
    std::shared_ptr<IntervalGazeAnalysisRequest> gaze_request_2 = std::make_shared<IntervalGazeAnalysisRequest>(participant_b, participant_a, gaze_cone_radius, gaze_cone_height);
    // when is a user looking at his/her hand?
    std::shared_ptr<IntervalGazeAnalysisRequest> gaze_request_3 = std::make_shared<IntervalGazeAnalysisRequest>(participant_a, participant_a_hand_left, gaze_cone_radius, gaze_cone_height);
    std::shared_ptr<IntervalGazeAnalysisRequest> gaze_request_4 = std::make_shared<IntervalGazeAnalysisRequest>(participant_a, participant_a_hand_right, gaze_cone_radius, gaze_cone_height);
    std::shared_ptr<IntervalGazeAnalysisRequest> gaze_request_5 = std::make_shared<IntervalGazeAnalysisRequest>(participant_b, participant_b_hand_left, gaze_cone_radius, gaze_cone_height);
    std::shared_ptr<IntervalGazeAnalysisRequest> gaze_request_6 = std::make_shared<IntervalGazeAnalysisRequest>(participant_b, participant_b_hand_right, gaze_cone_radius, gaze_cone_height);

    manager.add_interval_analysis_request(gaze_request_1);
    manager.add_interval_analysis_request(gaze_request_2);
    // when are users looking at each other while talking
    manager.add_interval_analysis_request(sound_request_1 & gaze_request_2);
    manager.add_interval_analysis_request(sound_request_2 & gaze_request_1);
    // when are users simultaneously looking at each other
    manager.add_interval_analysis_request(gaze_request_1 & gaze_request_2);
    manager.add_interval_analysis_request(gaze_request_3);
    manager.add_interval_analysis_request(gaze_request_4);
    manager.add_interval_analysis_request(gaze_request_5);
    manager.add_interval_analysis_request(gaze_request_6);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // when are users head/hands exceeding a specified velocity
    std::shared_ptr<IntervalVelocityAnalysisRequest> velocity_request_1 = std::make_shared<IntervalVelocityAnalysisRequest>(participant_a, temporal_search_interval, velocity_threshold);
    std::shared_ptr<IntervalVelocityAnalysisRequest> velocity_request_2 = std::make_shared<IntervalVelocityAnalysisRequest>(participant_a, temporal_search_interval, velocity_threshold);
    std::shared_ptr<IntervalVelocityAnalysisRequest> velocity_request_3 = std::make_shared<IntervalVelocityAnalysisRequest>(participant_a, temporal_search_interval, velocity_threshold);
    std::shared_ptr<IntervalVelocityAnalysisRequest> velocity_request_4 = std::make_shared<IntervalVelocityAnalysisRequest>(participant_a, temporal_search_interval, velocity_threshold);
    std::shared_ptr<IntervalVelocityAnalysisRequest> velocity_request_5 = std::make_shared<IntervalVelocityAnalysisRequest>(participant_a, temporal_search_interval, velocity_threshold);
    std::shared_ptr<IntervalVelocityAnalysisRequest> velocity_request_6 = std::make_shared<IntervalVelocityAnalysisRequest>(participant_a, temporal_search_interval, velocity_threshold);

    manager.add_interval_analysis_request(velocity_request_1);
    manager.add_interval_analysis_request(velocity_request_2);
    manager.add_interval_analysis_request(velocity_request_3);
    manager.add_interval_analysis_request(velocity_request_4);
    manager.add_interval_analysis_request(velocity_request_5);
    manager.add_interval_analysis_request(velocity_request_6);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // when is user a within a distance of user b
    std::shared_ptr<IntervalDistanceAnalysisRequest> distance_request_1 = std::make_shared<IntervalDistanceAnalysisRequest>(participant_a, participant_b, distance_threshold);
    // when are the left and right hand of the users close to each other
    std::shared_ptr<IntervalDistanceAnalysisRequest> distance_request_2 = std::make_shared<IntervalDistanceAnalysisRequest>(participant_a_hand_left, participant_a_hand_right, hand_distance_threshold);
    std::shared_ptr<IntervalDistanceAnalysisRequest> distance_request_3 = std::make_shared<IntervalDistanceAnalysisRequest>(participant_b_hand_left, participant_b_hand_right, hand_distance_threshold);
    // when are the hands of a user close to his/her head?
    std::shared_ptr<IntervalDistanceAnalysisRequest> distance_request_4 = std::make_shared<IntervalDistanceAnalysisRequest>(participant_a, participant_a_hand_left, hand_distance_threshold);
    std::shared_ptr<IntervalDistanceAnalysisRequest> distance_request_5 = std::make_shared<IntervalDistanceAnalysisRequest>(participant_a, participant_a_hand_right, hand_distance_threshold);
    std::shared_ptr<IntervalDistanceAnalysisRequest> distance_request_6 = std::make_shared<IntervalDistanceAnalysisRequest>(participant_b, participant_b_hand_left, hand_distance_threshold);
    std::shared_ptr<IntervalDistanceAnalysisRequest> distance_request_7 = std::make_shared<IntervalDistanceAnalysisRequest>(participant_b, participant_b_hand_right, hand_distance_threshold);

    manager.add_interval_analysis_request(distance_request_1);
    manager.add_interval_analysis_request(distance_request_2);
    manager.add_interval_analysis_request(distance_request_3);
    manager.add_interval_analysis_request(distance_request_4);
    manager.add_interval_analysis_request(distance_request_5);
    manager.add_interval_analysis_request(distance_request_6);
    manager.add_interval_analysis_request(distance_request_7);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // when are user heads/hands changing their orientation?
    std::shared_ptr<IntervalRotationAnalysisRequest> rotation_request_1 = std::make_shared<IntervalRotationAnalysisRequest>(participant_a, temporal_search_interval, rotation_threshold);
    std::shared_ptr<IntervalRotationAnalysisRequest> rotation_request_2 = std::make_shared<IntervalRotationAnalysisRequest>(participant_b, temporal_search_interval, rotation_threshold);
    std::shared_ptr<IntervalRotationAnalysisRequest> rotation_request_3 = std::make_shared<IntervalRotationAnalysisRequest>(participant_a_hand_left, temporal_search_interval, rotation_threshold);
    std::shared_ptr<IntervalRotationAnalysisRequest> rotation_request_4 = std::make_shared<IntervalRotationAnalysisRequest>(participant_b_hand_left, temporal_search_interval, rotation_threshold);
    std::shared_ptr<IntervalRotationAnalysisRequest> rotation_request_5 = std::make_shared<IntervalRotationAnalysisRequest>(participant_a_hand_right, temporal_search_interval, rotation_threshold);
    std::shared_ptr<IntervalRotationAnalysisRequest> rotation_request_6 = std::make_shared<IntervalRotationAnalysisRequest>(participant_b_hand_right, temporal_search_interval, rotation_threshold);

    manager.add_interval_analysis_request(rotation_request_1);
    manager.add_interval_analysis_request(rotation_request_2);
    manager.add_interval_analysis_request(rotation_request_3);
    manager.add_interval_analysis_request(rotation_request_4);
    manager.add_interval_analysis_request(rotation_request_5);
    manager.add_interval_analysis_request(rotation_request_6);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // when does a users head/hand adjust their position?
    std::shared_ptr<IntervalPositionAdjustmentAnalysisRequest> position_adjustment_request_1 = std::make_shared<IntervalPositionAdjustmentAnalysisRequest>(participant_a, 3 * temporal_search_interval, position_adjustment_distance);
    std::shared_ptr<IntervalPositionAdjustmentAnalysisRequest> position_adjustment_request_2 = std::make_shared<IntervalPositionAdjustmentAnalysisRequest>(participant_b, 3 * temporal_search_interval, position_adjustment_distance);
    std::shared_ptr<IntervalPositionAdjustmentAnalysisRequest> position_adjustment_request_3 = std::make_shared<IntervalPositionAdjustmentAnalysisRequest>(participant_a_hand_left, 3 * temporal_search_interval, position_adjustment_distance);
    std::shared_ptr<IntervalPositionAdjustmentAnalysisRequest> position_adjustment_request_4 = std::make_shared<IntervalPositionAdjustmentAnalysisRequest>(participant_b_hand_left, 3 * temporal_search_interval, position_adjustment_distance);
    std::shared_ptr<IntervalPositionAdjustmentAnalysisRequest> position_adjustment_request_5 = std::make_shared<IntervalPositionAdjustmentAnalysisRequest>(participant_a_hand_right, 3 * temporal_search_interval, position_adjustment_distance);
    std::shared_ptr<IntervalPositionAdjustmentAnalysisRequest> position_adjustment_request_6 = std::make_shared<IntervalPositionAdjustmentAnalysisRequest>(participant_b_hand_right, 3 * temporal_search_interval, position_adjustment_distance);

    manager.add_interval_analysis_request(position_adjustment_request_1);
    manager.add_interval_analysis_request(position_adjustment_request_2);
    manager.add_interval_analysis_request(position_adjustment_request_3);
    manager.add_interval_analysis_request(position_adjustment_request_4);
    manager.add_interval_analysis_request(position_adjustment_request_5);
    manager.add_interval_analysis_request(position_adjustment_request_6);

    auto test = (distance_request_1 | rotation_request_2) & rotation_request_2;
    manager.add_interval_analysis_request(test);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<QuantitativeTransformAnalysisRequest> quantitative_velocity_request_1 = std::make_shared<QuantitativeVelocityAnalysisRequest>(participant_a, 1.0f);

    manager.add_quantitative_analysis_request(quantitative_velocity_request_1);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<QuantitativeTransformAnalysisRequest> quantitative_gaze_request_1 = std::make_shared<QuantitativeGazeAnalysisRequest>(participant_a, participant_b, 1.0f);

    manager.add_quantitative_analysis_request(quantitative_gaze_request_1);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<QuantitativeTransformAnalysisRequest> quantitative_distance_request_1 = std::make_shared<QuantitativeDistanceAnalysisRequest>(participant_a, participant_b, 1.0f);

    manager.add_quantitative_analysis_request(quantitative_distance_request_1);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<QuantitativeTransformAnalysisRequest> quantitative_rotation_request_1 = std::make_shared<QuantitativeRotationAnalysisRequest>(participant_a, 1.0f);

    manager.add_quantitative_analysis_request(quantitative_rotation_request_1);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    manager.process_interval_analysis_requests_for_all_files();
    manager.process_quantitative_analysis_requests_for_all_files();
}

void single_user_analysis() {
    std::vector<std::string> files;
    std::string directory = "D:\\Recordings\\";

    for (const auto &entry: std::filesystem::directory_iterator(directory)) {
        std::string path = entry.path().string();
        if (path.substr(path.find_last_of(".") + 1) == "recordmeta") {
            if (path.find("partner_2") != std::string::npos)
                files.push_back(path.substr(0, path.find_last_of(".")));
        }
    }

    AnalysisManager &manager = AnalysisManager::getInstance();

    for (auto f: files) {
        manager.add_recording_path(f.data(), f.length());
    }

    std::string primary_file = manager.get_primary_file();
    MetaInformation meta_information{primary_file + ".recordmeta"};

    int participant_a = meta_information.get_old_uuid("/Participant2 [Local User]/Viewing Setup/Main Camera/Head");
    //manager.add_sound_activation_analysis_request(0, microphone_audio, temporal_search_interval, sound_level);

}

void synchrony_analysis(){
    std::vector<std::string> files;
    std::string directory = "C:\\Users\\Anton-Lammert\\Downloads\\PilotNew";
    if(!std::filesystem::exists(directory))
        return;

    for (const auto &entry: std::filesystem::directory_iterator(directory)) {
        std::string path = entry.path().string();
        if (path.substr(path.find_last_of(".") + 1) == "recordmeta") {
            files.push_back(path.substr(0, path.size() - 11));
        }
    }

    AnalysisManager &manager = AnalysisManager::getInstance();

    for (auto f: files) {
        manager.add_recording_path(f.data(), f.length());
    }

    std::string primary_file = manager.get_primary_file();
    MetaInformation meta_information{primary_file + ".recordmeta"};

    int participant_a_hand_left = meta_information.get_old_uuid("/Lobby-HMDUser/Camera Offset/LeftHand Controller");
    int participant_a_hand_right = meta_information.get_old_uuid("/Lobby-HMDUser/Camera Offset/RightHand Controller");
    int avatar_hand_right = meta_information.get_old_uuid("/__STUDY__/VisualStimulus/HandWaving/StudyHMDAvatar/Camera Offset/RightHand Controller");

    std::shared_ptr<QuantitativeTransformAnalysisRequest> quantitative_synchrony_request_1 = std::make_shared<QuantitativeSynchronyAnalysisRequest>(participant_a_hand_left, avatar_hand_right, 0.3f);
    std::shared_ptr<QuantitativeTransformAnalysisRequest> quantitative_synchrony_request_2 = std::make_shared<QuantitativeSynchronyAnalysisRequest>(participant_a_hand_right, avatar_hand_right, 0.3f);

    manager.add_quantitative_analysis_request(quantitative_synchrony_request_1);
    manager.add_quantitative_analysis_request(quantitative_synchrony_request_2);

    manager.process_quantitative_analysis_requests_for_all_files();
}

int main() {
    synchrony_analysis();
    return 0;
}