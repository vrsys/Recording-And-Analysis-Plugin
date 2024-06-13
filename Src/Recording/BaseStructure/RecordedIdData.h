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

#ifndef RECORDINGPLUGIN_RECORDEDIDDATA_H
#define RECORDINGPLUGIN_RECORDEDIDDATA_H

#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>

class RecordableData {
public:
    float time = -1.0f;
    int id = -1;
    bool interpolated_data = false;

    RecordableData() = default;

    virtual ~RecordableData() = default;

    virtual std::ofstream& write(std::ofstream& ofs) const = 0;

    virtual std::ifstream& read(std::ifstream& ifs) = 0;

    RecordableData(int i, float t) : id(i), time(t){}

    virtual void write_to_stream(std::ofstream& file) const = 0;

    bool operator < (RecordableData const& data) const
    {
        return time < data.time;
    }
};

/**
 * @class Buffer
 * @brief This class is used to store data which was recorded for a certain id.
 */
class RecordedIdData {
private:
    /// @brief id for which data is stored
    int id = -1;
    bool debug = false;
public:
    /// @brief stores all data with the corresponding id
    std::vector<std::shared_ptr<RecordableData>> loaded_data;

    ~RecordedIdData(){
        loaded_data.clear();
    }

    RecordedIdData(int new_id) : id(new_id){

    }

    void add_data(std::shared_ptr<RecordableData> data){
        loaded_data.push_back(data);
    }

    int get_id() const{
        return id;
    }

    void sort_data_by_time(){
        std::sort(loaded_data.begin(), loaded_data.end(), [](std::shared_ptr<RecordableData> a, std::shared_ptr<RecordableData>  b){return a->time < b->time;});
    }

    friend std::ostream& operator << (std::ostream& os, const RecordedIdData& recorded_id_data){
        os << "Id: " << recorded_id_data.id << ", Elements: " << recorded_id_data.loaded_data.size();
        return os;
    }
};


#endif //RECORDINGPLUGIN_RECORDEDIDDATA_H
