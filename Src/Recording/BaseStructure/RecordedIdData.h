//
// Created by Mortiferum on 18.03.2022.
//

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
