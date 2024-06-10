//
// Created by Anton-Lammert on 13.12.2022.
//

#ifndef RECORDINGPLUGIN_INTERVALSOUNDANALYSISREQUEST_H
#define RECORDINGPLUGIN_INTERVALSOUNDANALYSISREQUEST_H

#include "Analysis/IntervalAnalysis/IntervalAnalysisRequest.h"
#include "Recording/Sound/SoundData.h"

class IntervalSoundAnalysisRequest : public IntervalAnalysisRequest{
public:
    int id;

    IntervalSoundAnalysisRequest(int id);
};


#endif //RECORDINGPLUGIN_INTERVALSOUNDANALYSISREQUEST_H
