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

#include "SoundData.h"

SoundData::SoundData(int i, float t, int s_n, int s_i, int s_r, int c_n, int c_go_id,float *data) : RecordableData(i, t), sample_num(s_n), start_index(s_i), sampling_rate(s_r), channel_num(c_n), corresponding_gameobject_id(c_go_id)
{
    start_time = t;
    end_time = start_time + (float)sample_num/(float)sampling_rate;
    std::copy(data + start_index, data + start_index + sample_num, sound_data + start_index);
}

SoundData SoundData::interpolate(SoundData next_sound, float time) {
    //std::cout << "Time: " << time << ", t0:" << start_time << "-t1: " << end_time << ", t2: " << next_sound.start_time << "-t3: " << next_sound.end_time << "\n";
    SoundData in_between_data;

    in_between_data.start_time = time;

    int s_index =(int)((time - start_time) * (float)sampling_rate) + start_index;
    // for interleaved dual channel audio (L[0], R[0], L[1], R[1]) the start index should always be a multiple of 2
    if(s_index % channel_num != 0){
        s_index -= s_index % channel_num;
        if(s_index < 0)
            s_index += channel_num;
    }
    int e_index = start_index + sample_num;
    int index = 0;

    if(e_index - s_index >= 0) {
        std::copy(sound_data + s_index, sound_data + e_index, in_between_data.sound_data + index);
        index += e_index - s_index;
    } else {

        Debug::Log("Cannot use data from current element, Start: " + std::to_string(start_time) + ", End: " + std::to_string(end_time) + ", Required: " + std::to_string(time) + "\n");
    }

    s_index = next_sound.start_index;
    e_index = next_sound.start_index + next_sound.sample_num - index;
    if(e_index - s_index >= 0) {
        std::copy(next_sound.sound_data + s_index, next_sound.sound_data + e_index,in_between_data.sound_data + index);
        index += e_index - s_index;
    } else {
        //Debug::Log("Cannot use data from next element, Start: " + std::to_string(next_sound.start_time) + ", End: " + std::to_string(next_sound.end_time) + ", Required: " + std::to_string(time));
    }

    int i = 1;
    while(i <= index)
        i *= 2;
    int closest_pow_2 = i/2;

    in_between_data.start_index = 0;
    in_between_data.sample_num = closest_pow_2;
    in_between_data.sampling_rate = sampling_rate;
    in_between_data.end_time = in_between_data.start_time + sample_num/(float)in_between_data.sampling_rate;
    in_between_data.interpolated_data = true;
    in_between_data.channel_num = channel_num;
    float t = (time - start_time) / (next_sound.start_time - start_time);
    in_between_data.corresponding_gameobject_id = corresponding_gameobject_id;
    //std::cout << "Interpolated startT:" << in_between_data.start_time << ", endT: " << in_between_data.end_time << "\n";
    //std::cout << "First value: " << in_between_data.sound_data[0] << "\n";
    return in_between_data;
}

SoundData SoundData::interpolate(std::shared_ptr<SoundData>next_sound, float time) {
    if(next_sound != nullptr)
        return interpolate(*next_sound, time);
    else return *this;
}

SoundData::SoundData(std::shared_ptr<SoundData> data) : RecordableData(data->id, data->time){
    start_time = data->start_time;
    end_time = data->end_time;
    sample_num = data->sample_num;
    start_index = data->start_index;
    sampling_rate = data->sampling_rate;
    channel_num = data->channel_num;
    std::copy(data->sound_data, data->sound_data + 4800, sound_data);
}

std::ofstream &SoundData::write(std::ofstream &ofs) const {
    ofs << id << ' ' << time << ' ';
    ofs << start_time << ' ' << end_time << ' ' << sample_num << ' ' << start_index << ' ' << sampling_rate << ' ';

    for(float i : sound_data){
        ofs << i << ' ';
    }
    ofs << "\n";
    return ofs;
}

std::ifstream &SoundData::read(std::ifstream &ifs) {
    ifs >> id >> time;
    ifs >> start_time >> end_time >> sample_num >> start_index >> sampling_rate;
    ;
    for(int i = 0; i < 4800; i++){
        ifs >> sound_data[i];
    }
    return ifs;
}

SoundData::SoundData(const SoundDTO &dto) : RecordableData(dto.id, dto.t){
    start_time = dto.s_t;
    end_time = dto.e_t;
    sample_num = dto.s_n;
    start_index = dto.s_i;
    sampling_rate = dto.s_r;
    channel_num = dto.c_n;
    corresponding_gameobject_id = dto.c_go_id;
    std::copy(dto.s_d, dto.s_d + 4800, sound_data);
}

SoundData::~SoundData() {
    //std::cout << "Destroying Sound Data\n";
}

void SoundData::write_to_stream(std::ofstream &file) const {
    SoundDTO dto = to_dto();
    file.write((const char*) &dto, sizeof(SoundDTO));
}

SoundDTO SoundData::to_dto() const {
    SoundDTO dto;
    dto.id = id;
    dto.t = time;
    dto.s_t = start_time;
    dto.e_t = end_time;
    dto.s_n = sample_num;
    dto.s_i = start_index;
    dto.s_r = sampling_rate;
    dto.c_n = channel_num;
    dto.c_go_id = corresponding_gameobject_id;
    std::copy(sound_data, sound_data + 4800, dto.s_d);
    return dto;
}
