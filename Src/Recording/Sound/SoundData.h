//
// Created by Mortiferum on 18.03.2022.
//

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
