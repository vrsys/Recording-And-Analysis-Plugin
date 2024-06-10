//
// Created by Mortiferum on 18.03.2022.
//

#ifndef RECORDINGPLUGIN_GENERICBUFFER_H
#define RECORDINGPLUGIN_GENERICBUFFER_H

#include "Recording/BaseStructure/Buffer.h"

#include <map>
#include "GenericData.h"

class GenericBuffer : public Buffer{
private:
    // maps for faster access
    std::map<int,unsigned int> id_record_data_index;
    std::map<int, unsigned int> id_playhead;

    std::shared_ptr<GenericData> previous_data = nullptr;
    std::shared_ptr<GenericData> next_data = nullptr;
public:
    GenericBuffer(float start_time, float end_time);

    virtual ~GenericBuffer();

    std::shared_ptr<RecordableData> get_data(float time, int id) override;

    std::shared_ptr<RecordableData> get_any_data(int id) override;

    void reload_index_maps() override;
};


#endif //RECORDINGPLUGIN_GENERICBUFFER_H
