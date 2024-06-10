//
// Created by Anton-Lammert on 27.10.2022.
//

#ifndef RECORDINGPLUGIN_INTERVALCONTAINMENTANALYSISREQUEST_H
#define RECORDINGPLUGIN_INTERVALCONTAINMENTANALYSISREQUEST_H

#include "IntervalTransformAnalysisRequest.h"

class IntervalContainmentAnalysisRequest : public IntervalTransformAnalysisRequest{

private:
    int id;

    glm::vec3 box_min;
    glm::vec3 box_max;

    bool present = false;
    TransformData current;
    TransformData last;

public:
    IntervalContainmentAnalysisRequest(int i, glm::vec3 min, glm::vec3 max);

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    std::string get_description(MetaInformation& meta_info) const override;

    std::shared_ptr<IntervalAnalysisRequest> clone() const override;

    void clear_recent_data() override;

    int get_id() const;

    void set_id(int id);

    TransformAnalysisType get_type() const override;
};


#endif //RECORDINGPLUGIN_INTERVALCONTAINMENTANALYSISREQUEST_H
