//
// Created by Mortiferum on 18.03.2022.
//

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
