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

#include "Recording/RecorderManager.h"
#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_all.hpp"
#include "../Utils/Utils.h"

#include <filesystem>
#include <iostream>
#include <chrono>
#include <random>

std::string recording_dir = std::filesystem::current_path().string() + "/";
std::string recording_file = "test";

int transform_buffer_size = 1000;
int sound_buffer_size = 100;
int replay_buffer_num = 3;
float replay_buffer_interval = 10.0f;
int recording_duration = 100;
int stop_time = 15;
int transform_count = 20;
int recording_steps_per_second = 10;

bool forward = true;
bool reverse = true;
bool random = true;

bool first = true;

void fill_transform_dto(glm::vec3 local_pos, glm::vec3 local_sca, glm::quat local_rot,
                        glm::vec3 global_pos, glm::vec3 global_sca, glm::quat global_rot,
                        int active, int parent, float *dto, int *info_dto) {
    dto[0] = local_pos.x;
    dto[1] = local_pos.y;
    dto[2] = local_pos.z;

    dto[3] = local_rot.w;
    dto[4] = local_rot.x;
    dto[5] = local_rot.y;
    dto[6] = local_rot.z;

    dto[7] = local_sca.x;
    dto[8] = local_sca.y;
    dto[9] = local_sca.z;

    dto[10] = global_pos.x;
    dto[11] = global_pos.y;
    dto[12] = global_pos.z;

    dto[13] = global_rot.w;
    dto[14] = global_rot.x;
    dto[15] = global_rot.y;
    dto[16] = global_rot.z;

    dto[17] = global_sca.x;
    dto[18] = global_sca.y;
    dto[19] = global_sca.z;

    info_dto[0] = active;
    info_dto[1] = parent;
}

void initialize_recorder_param() {
    RecorderManager &manager = RecorderManager::getInstance();
    manager.set_transform_recording_buffer_size(0, transform_buffer_size);
    manager.set_sound_recording_buffer_size(0, sound_buffer_size);
    manager.set_replay_buffer_num(0, replay_buffer_num);
    manager.set_replay_buffer_time_interval(0, replay_buffer_interval);
}


TEST_CASE("Transform Replay Test", "[transform]") {
    //std::vector<int> transform_counts{10, 50, 100, 200, 250, 400, 500, 750, 1000, 1250, 1500, 1750, 2000, 2500, 3000, 3500, 4000, 5000, 7500, 10000, 12500};
    std::vector<int> transform_counts{5};

    for (auto t_count: transform_counts) {

        transform_count = t_count;

        initialize_recorder_param();
        RecorderManager &manager = RecorderManager::getInstance();
        manager.stop_recording(0);
        manager.set_debug_mode(0, false);
        manager.create_recording_file(0, recording_dir, recording_file);

        float dto[20] = {};
        int info_dto[2] = {};
        float dto_2[20] = {};
        int info_dto_2[2] = {};

        std::string object_name = "Test";
        int id = 1;

        std::string object_name_2 = "Test2";
        int id_2 = 2;

        int active = 1;
        int parent = 0;

        for (int i = 0; i < recording_duration * recording_steps_per_second; i++) {
            float time = (float) i / (float) recording_steps_per_second;
            glm::vec3 local_pos{time, 0.0f, 0.0f};
            glm::vec3 local_sca{time * 2, 1.0f, 1.0f};
            glm::quat local_rot{time * 3, time, time, time};
            glm::vec3 global_pos{time, 2.0f, 0.0f};
            glm::vec3 global_sca{1.0f, 1.0f, 1.0f};
            glm::quat global_rot{time * 3, time, time, time};

            fill_transform_dto(local_pos, local_sca, local_rot, global_pos, global_sca, global_rot, active, parent,
                               dto, info_dto);

            bool result = manager.record_transform_at_time(0, object_name, id, dto,time, info_dto);
            REQUIRE(result == true);

            if (time <= stop_time) {
                fill_transform_dto(local_pos, local_sca, local_rot, global_pos, global_sca, global_rot, active,
                                   parent, dto_2, info_dto_2);
                result = manager.record_transform_at_time(0, object_name_2, id_2,dto_2, time, info_dto_2);
                REQUIRE(result == true);
            }

            for (int j = 0; j < transform_count; ++j) {
                std::string object_name_i = "Test" + std::to_string(j + id_2 + 1);
                result = manager.record_transform_at_time(0, object_name_i,id_2 + j + 1, dto, time, info_dto);
                REQUIRE(result == true);
            }
        }

        manager.stop_recording(0);

        manager.open_existing_recording_file(0, recording_dir, recording_file);

        if(forward) {
            for (int i = 0; i < recording_duration * recording_steps_per_second; i++) {
                float t = (float) i / (float) recording_steps_per_second;
                bool result = manager.get_transform_at_time(0, object_name.data(), object_name.length(), id, t, dto,
                                                            info_dto);

                //std::cout << "t: " << t << "\n";
                REQUIRE(result == true);
                REQUIRE(dto[0] == Catch::Approx(t));
                REQUIRE(dto[7] == Catch::Approx(2 * t));

                result = manager.get_transform_at_time(0, object_name_2.data(), object_name_2.length(), id_2, t, dto_2,
                                                       info_dto_2);
                REQUIRE(result == true);
                if (t < stop_time) {
                    REQUIRE(dto_2[0] == Catch::Approx(t));
                    REQUIRE(dto_2[7] == Catch::Approx(2 * t));
                } else {
                    REQUIRE(std::abs(dto_2[0] - stop_time) <= 1.0f / (float) recording_steps_per_second + 0.001f);
                    REQUIRE(std::abs(dto_2[7] - 2 * stop_time) <= 2 * 1.0f / (float) recording_steps_per_second + 0.001f);
                    REQUIRE(std::abs(dto_2[3] - 3 * stop_time) <= 3 * 1.0f / (float) recording_steps_per_second + 0.001f);
                }

                for (int j = 0; j < transform_count; ++j) {
                    std::string object_name_i = "Test" + std::to_string(j + id_2 + 1);
                    result = manager.get_transform_at_time(0, object_name_i.data(), object_name_i.length(),
                                                           id_2 + j + 1, t,
                                                           dto, info_dto);

                    REQUIRE(result == true);
                    REQUIRE(dto[0] == Catch::Approx(t));
                    REQUIRE(dto[7] == Catch::Approx(2 * t));
                }
            }
        }

        if(reverse) {
            for (int i = 0; i < recording_duration * recording_steps_per_second; i++) {
                float t = (float) (recording_duration * recording_steps_per_second - i - 1) / (float) recording_steps_per_second;
                bool result = manager.get_transform_at_time(0, object_name.data(), object_name.length(), id, t, dto, info_dto);

                REQUIRE(result == true);
                REQUIRE(dto[0] == Catch::Approx(t));

                result = manager.get_transform_at_time(0, object_name_2.data(), object_name_2.length(), id_2, t, dto_2, info_dto_2);

                REQUIRE(result == true);
                if(t < stop_time) {
                    REQUIRE(dto_2[0] == Catch::Approx(t));
                } else {
                    REQUIRE(std::abs(dto_2[0] - stop_time) <= 1.0f / (float)recording_steps_per_second + 0.001f);
                }

                for(int j = 0; j < transform_count; ++j){
                    std::string object_name_i = "Test" + std::to_string(j + id_2 + 1);
                    bool result = manager.get_transform_at_time(0, object_name_i.data(), object_name_i.length(), id_2 + j + 1, t, dto, info_dto);

                    REQUIRE(result == true);
                    REQUIRE(dto[0] == Catch::Approx(t));
                    REQUIRE(dto[7] == Catch::Approx(2*t));
                }
            }
        }

        if(random) {
            std::vector<float> random_times{};

            std::random_device rd;
            std::default_random_engine eng(rd());
            std::uniform_real_distribution<> distr(0.0f, (float) (recording_duration - 1));

            for (int i = 0; i < recording_duration; i++) {
                random_times.push_back(distr(eng));
            }

            for (int i = 0; i < random_times.size(); i++) {
                //std::cout << "Random time: " << random_times[i] << "\n";
                bool result = manager.get_transform_at_time(0, object_name.data(), object_name.length(), id, random_times[i],
                                                             dto, info_dto);

                REQUIRE(result == true);
                REQUIRE(std::abs(dto[0] - random_times[i]) < 0.01f);
                //REQUIRE(std::abs(dto[7] - 2*random_times[i]) < 0.01f);

                result = manager.get_transform_at_time(0, object_name_2.data(), object_name_2.length(), id_2, random_times[i],dto_2, info_dto_2);
                REQUIRE(result == true);
                if(random_times[i] < stop_time){
                    if(std::abs(dto_2[0] - random_times[i]) >= 0.01f){
                        std::cout << "time: " << random_times[i] << ", results: " << dto_2[0] << "\n";
                    }
                    REQUIRE(std::abs(dto_2[0] - random_times[i]) < 0.01f);
                } else {
                    REQUIRE(std::abs(dto_2[0] - stop_time) <= 1.0f / (float)recording_steps_per_second + 0.001f);
                }

                for(int j = 0; j < transform_count; ++j){
                    std::string object_name_i = "Test" + std::to_string(j + id_2 + 1);
                    bool result = manager.get_transform_at_time(0, object_name_i.data(), object_name_i.length(), id_2 + j + 1, random_times[i], dto, info_dto);

                    REQUIRE(result == true);
                    REQUIRE(std::abs(dto[0] - random_times[i]) < 0.01f);
                    //REQUIRE(std::abs(dto[7] - 2 * random_times[i]) < 0.01f);
                }
            }
        }

        manager.stop_replay(0);
    }
}

TEST_CASE("Generic Replay Test", "[arbitrary]") {
    initialize_recorder_param();
    RecorderManager &manager = RecorderManager::getInstance();
    manager.stop_recording(0);
    manager.create_recording_file(0, recording_dir, recording_file);

    int id = 1;
    int int_dto[10] = {};
    float float_dto[10] = {};
    char char_dto[10] = {};

    int id_2 = 2;
    int int_dto_2[10] = {};
    float float_dto_2[10] = {};
    char char_dto_2[10] = {};

    for (int i = 0; i < recording_duration; i++) {
        float time = (float) i;
        int_dto[0] = i;
        float_dto[0] = (float) i;

        int_dto_2[0] = i;
        float_dto_2[0] = (float) i;

        bool result = manager.record_generic_at_time(0, time, id, int_dto, float_dto, char_dto);
        REQUIRE(result == true);

        if (i < stop_time) {
            result = manager.record_generic_at_time(0, time, id_2, int_dto_2, float_dto_2, char_dto_2);
            REQUIRE(result == true);
        }
    }

    manager.stop_recording(0);

    manager.open_existing_recording_file(0, recording_dir, recording_file);

    for (int i = 0; i < recording_duration; i++) {
        float t = (float) i;
        bool result = manager.get_generic_at_time(0, t, id, int_dto, float_dto, char_dto);

        REQUIRE(result == true);
        REQUIRE(int_dto[0] == Catch::Approx(std::floor(t)));

        if (t > 14.3f)
            int x = 3;
        result = manager.get_generic_at_time(0, t, id_2, int_dto_2, float_dto_2, char_dto_2);
        REQUIRE(result == true);
        if (i < stop_time) {
            REQUIRE(int_dto_2[0] == Catch::Approx(std::floor(t)));
        } else {
            REQUIRE(int_dto_2[0] == Catch::Approx(stop_time - 1));
        }
    }

    for (int i = 0; i < recording_duration; i++) {
        float t = (float) (recording_duration - i - 1);
        bool result = manager.get_generic_at_time(0, t, id, int_dto, float_dto, char_dto);

        REQUIRE(result == true);
        REQUIRE(int_dto[0] == Catch::Approx(std::floor(t)));

        result = manager.get_generic_at_time(0, t, id_2, int_dto_2, float_dto_2, char_dto_2);

        REQUIRE(result == true);
        if (recording_duration - i - 1 < stop_time) {
            REQUIRE(int_dto_2[0] == Catch::Approx(std::floor(t)));
        } else {
            REQUIRE(int_dto_2[0] == Catch::Approx(stop_time - 1));
        }
    }

    std::vector<float> random_times{};

    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<> distr(0.0f, (float) (recording_duration - 1));

    for (int i = 0; i < recording_duration; i++) {
        random_times.push_back(distr(eng));
    }

    for (int i = 0; i < random_times.size(); i++) {
        //std::cout << "Random time: " << random_times[i] << "\n";
        bool result = manager.get_generic_at_time(0, random_times[i], id, int_dto, float_dto, char_dto);

        REQUIRE(result == true);
        if (random_times[i] - std::floor(random_times[i]) >= 0.999f) {
            REQUIRE(int_dto[0] == Catch::Approx(std::ceil(random_times[i])));
        } else {
            REQUIRE(int_dto[0] == Catch::Approx(std::floor(random_times[i])));
        }

        result = manager.get_generic_at_time(0, random_times[i], id_2, int_dto_2, float_dto_2, char_dto_2);
        REQUIRE(result == true);
        if (random_times[i] < stop_time - 1) {
            if (random_times[i] - std::floor(random_times[i]) >= 0.999f) {
                REQUIRE(int_dto_2[0] == Catch::Approx(std::ceil(random_times[i])));
            } else {
                REQUIRE(int_dto_2[0] == Catch::Approx(std::floor(random_times[i])));
            }
        } else {
            REQUIRE(int_dto_2[0] == stop_time - 1);
        }
    }

    manager.stop_replay(0);
}

TEST_CASE("Sound Replay Test", "[sound]") {
    initialize_recorder_param();
    RecorderManager &manager = RecorderManager::getInstance();
    manager.stop_replay(0);
    manager.create_recording_file(0, recording_dir, recording_file);

    float sound_dto[4800] = {};
    int corresponding_go_id[1] = {0};
    int sampling_rate = 32000;
    int sample_num = 2000;
    int start_index = 10;
    int sound_id = 1;
    int channel_num = 1;

    float time = 0.0f;
    float delta_t = (float) sample_num / (float) sampling_rate;

    while (time < (float) recording_duration) {
        for (int i = 0; i < sample_num; i++) {
            sound_dto[start_index + i] = time + ((float) i / (float) sample_num) * delta_t;
        }
        corresponding_go_id[0] = sound_id + 1;
        bool result = manager.record_sound_at_time(0, sound_dto, sample_num, sampling_rate, start_index, channel_num,corresponding_go_id[0], time, sound_id);
        REQUIRE(result == true);
        time += delta_t;
    }

    manager.stop_recording(0);

    manager.open_existing_recording_file(0, recording_dir, recording_file);

    int sampling_rate_retrieved = manager.get_sampling_rate(0,sound_id);
    int channel_num_retrieved = manager.get_channel_num(0,sound_id);

    //REQUIRE(sampling_rate_retrieved == sampling_rate);
    REQUIRE(channel_num_retrieved == channel_num);

    time = 0.0f;
    while (time < (float) (recording_duration - 1)) {
        //std::cout << "Time: " << time << "\n";
        int result = manager.get_sound_at_time(0, sound_id, time, sound_dto, corresponding_go_id);
        REQUIRE(result > 0);
        REQUIRE(std::abs(sound_dto[0] - time) <= 0.001f);
        time += (float) result / (float) sampling_rate;
    }

    time = (float) (recording_duration - 1);
    while (time > 0.0f) {
        //std::cout << "Time: " << time << "\n";
        if (std::abs(time - 70.000f) <= 0.001f)
            int x = 5;
        int result = manager.get_sound_at_time(0, sound_id, time, sound_dto);
        REQUIRE(result > 0);
        //std::cout << "Expected: " << time << ", Received: " << sound_dto[0] << "\n";
        REQUIRE(std::abs(sound_dto[0] - time) <= 0.001f);
        time -= (float) result / (float) sampling_rate;
    }

    std::vector<float> random_times{};

    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<> distr(0.0f, (float) (recording_duration - 1));

    for (int i = 0; i < recording_duration; i++) {
        random_times.push_back(distr(eng));
    }

    for (float random_time: random_times) {
        int result = manager.get_sound_at_time(0, sound_id, random_time, sound_dto);
        REQUIRE(result > 0);
        REQUIRE(std::abs(sound_dto[0] - random_time) <= 0.001f);
    }

    manager.stop_replay(0);
}

int main(int argc, char *argv[]) {
    RecorderManager &manager = RecorderManager::getInstance();
    manager.set_debug_mode(0,true);
    int result = Catch::Session().run(argc, argv);
    return result;
}


