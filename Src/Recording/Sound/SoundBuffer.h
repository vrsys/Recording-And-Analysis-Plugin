//
// Created by Mortiferum on 18.03.2022.
//

#ifndef RECORDINGPLUGIN_SOUNDBUFFER_H
#define RECORDINGPLUGIN_SOUNDBUFFER_H

#include "Recording/BaseStructure/Buffer.h"
#include "SoundData.h"

class SoundBuffer : public Buffer {
private:
    std::shared_ptr<SoundData> next_data;
    std::shared_ptr<SoundData> current_data;
    std::shared_ptr<SoundData> empty_data;
public:
    SoundBuffer(float start_time, float end_time);

    virtual ~SoundBuffer();

    std::shared_ptr<RecordableData> get_data(float time, int id) override;

    std::shared_ptr<RecordableData> get_any_data(int id) override;

    std::shared_ptr<RecordableData> get_first_data(int id);

    void reload_index_maps() override;

};


#endif //RECORDINGPLUGIN_SOUNDBUFFER_H
