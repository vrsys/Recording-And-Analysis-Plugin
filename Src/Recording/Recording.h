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

#ifndef RECORDINGPLUGIN_RECORDING_H
#define RECORDINGPLUGIN_RECORDING_H

#include "MetaInformation.h"
#include <iostream>

enum RecordingState{
    Record, Replay, Idle
};

struct Recording{
    /// @brief current state of the recording
    RecordingState state;

    /// @brief path relative to the current directory of the application under which the recording files will be saved
    std::string recording_directory = R"(\Assets\Recordings\)";
    /// @brief location of the current recording file storing transform data
    std::string transform_file;
    /// @brief location of the current recording file storing sound data
    std::string sound_file;
    /// @brief location of the current recording file storing generic data
    std::string generic_file;
    /// @brief location of the current meta information file
    std::string meta_information_file;

    /// @brief is there currently a replay of transform data? If not get rid of stored data
    bool transform_replaying = false;
    /// @brief is there currently a replay of sound data? If not get rid of stored data
    bool sound_replaying = false;
    /// @brief is there currently a replay of generic data? If not get rid of stored data
    bool generic_replaying = false;

    /// @brief is there currently a recording of transform data?
    bool transform_recording = false;
    /// @brief is there currently a recording of sound data?
    bool sound_recording = false;
    /// @brief is there currently a recording of generic data?
    bool generic_recording = false;

    MetaInformation meta_information;

    float duration;

    Recording(){
        state = RecordingState::Idle;
    }

    Recording(std::string const& recording_name){
        transform_file = recording_name + ".txt";
        sound_file = recording_name + "_sound.txt";
        generic_file = recording_name + "_arb.txt";
        meta_information_file = recording_name + ".recordmeta";
        state = RecordingState::Idle;
    }

    Recording(std::string const& recording_dir, std::string const& recording_name){
        recording_directory = recording_dir;
        if(recording_directory.back() != '/'){
            recording_directory += '/';
        }
        transform_file = recording_directory + recording_name + ".txt";
        sound_file = recording_directory + recording_name + "_sound.txt";
        generic_file = recording_directory + recording_name + "_arb.txt";
        meta_information_file = recording_directory + recording_name + ".recordmeta";
        state = RecordingState::Idle;
    }

    bool setup_for_recording(){
        try {
            std::ofstream outfile{transform_file};
            outfile.close();

            std::ofstream sound_outfile{sound_file};
            sound_outfile.close();

            std::ofstream generic_outfile{generic_file};
            generic_outfile.close();

            std::ofstream meta_outfile{meta_information_file};
            meta_outfile.close();

            meta_information = MetaInformation(meta_information_file);

            Debug::Log("New recording files created at " + recording_directory);
            state = RecordingState::Record;
        } catch (std::exception const& e){
            Debug::Log("Error! Could not create new files for recording.", Color::Red);
            return false;
        }

        return true;
    }

    bool end_recording(){
        if(is_currently_recording())
            Debug::Log("Error! Should not be recording anymore!");

        meta_information.write_meta_information();
        meta_information.clear();

        state = RecordingState::Idle;
        return true;
    }

    bool setup_for_replay(){
        try {
            Debug::Log("Checking recording file existence: " + transform_file);

            std::ifstream outfile{transform_file};
            outfile.close();

            std::ifstream sound_outfile{ sound_file};
            sound_outfile.close();

            std::ifstream generic_outfile{generic_file};
            generic_outfile.close();

            std::ifstream meta_outfile{meta_information_file};
            meta_outfile.close();

            Debug::Log("Recording file exists\nTrying to read recording meta information");
            meta_information = MetaInformation(meta_information_file);
            duration = meta_information.get_recording_duration();
            Debug::Log("Duration of recording: " + std::to_string(duration));

            state = RecordingState::Replay;
        } catch (std::exception const& e){
            Debug::Log("Error! Could not open recording files.", Color::Red);
            return false;
        }

        return true;
    }

    bool end_replay(){
        meta_information.clear();
        state = RecordingState::Idle;
        return true;
    }

    bool is_currently_recording() const{
        return transform_recording || sound_recording || generic_recording;
    }

    bool is_currently_replaying() const{
        return transform_replaying || sound_replaying || generic_replaying;
    }
};
#endif //RECORDINGPLUGIN_RECORDING_H
