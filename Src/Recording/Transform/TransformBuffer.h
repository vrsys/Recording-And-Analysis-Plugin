//
// Created by Mortiferum on 18.03.2022.
//

#ifndef RECORDINGPLUGIN_TRANSFORMBUFFER_H
#define RECORDINGPLUGIN_TRANSFORMBUFFER_H

#include "TransformData.h"
#include "Recording/BaseStructure/Buffer.h"

#include <map>

class TransformBuffer : public Buffer{
private:
    // maps for faster access
    std::map<int,unsigned int> uuid_record_data_index;
    // store position of the last used matrix in the sorted list for each object
    // this allows for a faster search for the next needed transformation matrix
    std::map<int, unsigned int> uuid_playhead;

    std::shared_ptr<TransformData> previous_transform = nullptr;
    std::shared_ptr<TransformData> next_transform = nullptr;
public:
    TransformBuffer(float start_time, float end_time);

    virtual ~TransformBuffer();

    std::shared_ptr<RecordableData> get_data(float time, int id) override;

    std::shared_ptr<RecordableData> get_any_data(int id) override;

    void reload_index_maps() override;
};


#endif //RECORDINGPLUGIN_TRANSFORMBUFFER_H
