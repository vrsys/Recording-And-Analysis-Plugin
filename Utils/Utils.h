//
// Created by Mortiferum on 24.11.2023.
//

#ifndef RECORDINGPLUGIN_UTILS_H
#define RECORDINGPLUGIN_UTILS_H

#include "Recording/Transform/TransformData.h"
#include "Recording/Generic/GenericData.h"
#include "DebugLog.h"
#include "Recording/Sound/SoundData.h"
#include "Recording/MetaInformation.h"

#include "../External/AudioFile/AudioFile.h"

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>


namespace Utils {

    void export_transform_data_to_CSV(std::string const& transform_file_path);

    void export_generic_data_to_CSV(std::string const& generic_file_path);

    void export_sound_data_to_WAV(std::string const& sound_file_path);
};


#endif //RECORDINGPLUGIN_UTILS_H
