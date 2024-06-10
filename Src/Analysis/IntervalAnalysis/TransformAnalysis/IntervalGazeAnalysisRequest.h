//
// Created by Anton-Lammert on 27.10.2022.
//

#ifndef RECORDINGPLUGIN_INTERVALGAZEANALYSISREQUEST_H
#define RECORDINGPLUGIN_INTERVALGAZEANALYSISREQUEST_H

#include "IntervalTransformAnalysisRequest.h"

class IntervalGazeAnalysisRequest : public IntervalTransformAnalysisRequest{

private:
    int id_a;
    int id_b;
    // angular threshold defining the cone that is used to determine if object with id b might have been visible by object with id a
    float threshold;
    // 0 = -z, 1 = z, 2 = -y, 3 = y, 4 = -x, 5 = x
    int axis;

    float cone_height = 100.0f;
    float cone_radius = -1.0f;

    bool present_a = false;
    bool present_b = false;

    TransformData current_a;
    TransformData current_b;
    TransformData last_a;
    TransformData last_b;

public:
    IntervalGazeAnalysisRequest(int a, int b, float t, float max_d, int axis = 0);

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


#endif //RECORDINGPLUGIN_INTERVALGAZEANALYSISREQUEST_H
