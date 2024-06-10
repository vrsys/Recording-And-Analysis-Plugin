//
// Created by Mortiferum on 29.02.2024.
//

#ifndef RECORDINGPLUGIN_ANALYSISREQUEST_H
#define RECORDINGPLUGIN_ANALYSISREQUEST_H


#include <string>
#include "Recording/MetaInformation.h"
#include "Recording/Transform/TransformData.h"
#include "Recording/Sound/SoundData.h"

class AnalysisRequest {
public:
    virtual std::string get_description(MetaInformation& meta_info) const = 0;

    virtual void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) = 0;

    virtual void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) = 0;
};


#endif //RECORDINGPLUGIN_ANALYSISREQUEST_H
