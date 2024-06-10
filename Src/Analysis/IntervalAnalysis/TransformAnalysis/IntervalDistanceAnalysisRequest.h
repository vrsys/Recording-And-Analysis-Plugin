//
// Created by Anton-Lammert on 26.10.2022.
//

#ifndef RECORDINGPLUGIN_INTERVALDISTANCEANALYSISREQUEST_H
#define RECORDINGPLUGIN_INTERVALDISTANCEANALYSISREQUEST_H


#include "Recording/Transform/TransformData.h"
#include "IntervalTransformAnalysisRequest.h"

class IntervalDistanceAnalysisRequest : public IntervalTransformAnalysisRequest{

private:
    int id_a;
    int id_b;
    float threshold;

    bool present_a = false;
    bool present_b = false;

    TransformData current_a;
    TransformData current_b;
    TransformData last_a;
    TransformData last_b;

public:
    IntervalDistanceAnalysisRequest(int a, int b, float t);

    void process_request(std::shared_ptr<TransformData> t_data, std::shared_ptr<SoundData> s_data) override;

    std::string get_description(MetaInformation& meta_info) const override;

    void update_parameters(MetaInformation& original_meta_file, MetaInformation& new_meta_file) override;

    std::shared_ptr<IntervalAnalysisRequest> clone() const override;

    void clear_recent_data() override;

    TransformAnalysisType get_type() const override;

    int get_id_a() const;

    int get_id_b() const;

    void set_id_a(int new_id);

    void set_id_b(int new_id);
};


#endif //RECORDINGPLUGIN_INTERVALDISTANCEANALYSISREQUEST_H
