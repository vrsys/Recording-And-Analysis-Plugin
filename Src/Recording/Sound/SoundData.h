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

#ifndef RECORDINGPLUGIN_SOUNDDATA_H
#define RECORDINGPLUGIN_SOUNDDATA_H

#include "Recording/BaseStructure/RecordedIdData.h"
#include "DebugLog.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include <iostream>

struct SoundDTO{
    int id;
    float t;
    float s_t;
    float e_t;
    int s_n;
    int s_i;
    int s_r;
    int c_n;
    int c_go_id;
    float s_d[4800];

    std::string static dto_layout(){
        return "SoundDTO: int id;float t;float s_t;float e_t;int s_n;int s_i;int s_r;int c_n;int c_go_id;float s_d[4800];int layer = 0;";
    }
};

class SoundData : public RecordableData{
public:
    float start_time = -1.0f;
    float end_time = -1.0f;
    int sample_num = -1;
    int start_index = 0;
    int sampling_rate = -1;
    int channel_num = -1;
    int corresponding_gameobject_id;
    // assuming that at a sampling rate of at most 48.000 we record with at least 10 steps per second
    float sound_data [4800];

    SoundData() = default;

    SoundData(SoundDTO const& dto);

    SoundData(int i, float t, int s_n, int s_i, int s_r, int c_n, int c_go_id, float* data);

    SoundData(std::shared_ptr<SoundData> data);

    ~SoundData() override;

    std::ofstream& write(std::ofstream& ofs) const override;

    std::ifstream& read(std::ifstream& ifs) override;

    SoundData interpolate(SoundData next_sound, float time);

    SoundData interpolate(std::shared_ptr<SoundData> next_sound, float time);

    void write_to_stream(std::ofstream& file) const override;

    SoundDTO to_dto() const;
};


#endif //RECORDINGPLUGIN_SOUNDDATA_H
