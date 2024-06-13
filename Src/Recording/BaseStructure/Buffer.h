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

#ifndef RECORDINGPLUGIN_BUFFER_H
#define RECORDINGPLUGIN_BUFFER_H

#include "RecordedIdData.h"
#include "DebugLog.h"
#include <map>
#include <vector>
#include <iostream>
#include <memory>

/**
 * @class Buffer
 * @brief This class is used to store data which was recorded in a certain time period [min_time - max_time].
 */
class Buffer {
protected:
    /// @brief min time for which recorded data is stored in this buffer
    float min_time;
    /// @brief max time for which recorded data is stored in this buffer
    float max_time;
    /// @brief is the current buffer completely loaded into memory
    bool in_memory = false;

    bool debug = false;

    /// @brief stores the last seen data for each id before this buffer (necessary for interpolation)
    std::map<int, std::shared_ptr<RecordableData>> id_last_data;
public:
    /// @brief this vector stores the loaded data
    std::vector<std::shared_ptr<RecordedIdData>> loaded_data_for_ids;

    virtual ~Buffer();

    Buffer(float start_time, float end_time);

    void clear_buffer();

    bool add_element(std::shared_ptr<RecordableData> data);

    virtual std::shared_ptr<RecordableData> get_data(float time, int id) = 0;

    virtual std::shared_ptr<RecordableData> get_any_data(int id) = 0;

    virtual void reload_index_maps() = 0;

    bool contains_time(float time) const;

    bool stored_in_memory() const;

    float get_max_time() const;

    float get_min_time() const;

    void set_max_time(float time);

    void set_min_time(float time);

    void set_memory_status(bool status);

    void add_last_element(std::shared_ptr<RecordableData> data);

    void set_debug_mode(bool debug_state);
};


#endif //RECORDINGPLUGIN_BUFFER_H
