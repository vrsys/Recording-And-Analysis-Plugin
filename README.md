# Record, Replay  & Analysis Plugin

[![Build](https://github.com/vrsys/Recording-And-Analysis-Plugin/actions/workflows/cmake.yml/badge.svg)](https://github.com/vrsys/Recording-And-Analysis-Plugin/actions/workflows/cmake.yml)

![Logo](Images/Icon_resized.png)

This plugin enables the recording and replaying of time series data on Windows, Linux, and Android platforms (e.g., Oculus Quest). 
It supports recording of transforms, scene graph structure, sound, and generic time series data. 
Enhanced features include simultaneous multiple playback capabilities and the ability to record new data while replaying existing recordings, facilitated by unique recorder IDs.
In addition, it supports efficient analysis functionalities that allow the analysis of created recordings during playback or outside of the engine using C++.
Different types of analysis queries (distance queries, velocity queries, ...) can be defined individually or combined for the investigation
of complex behaviour.


**Functionalities:**
- Efficient recording and playback of transform, sound and generic data
- Complex analysis of behaviour through composable analysis queries that can be analyzed during playback and using C++

**Possible use cases:**  
- Integration into 3D engines such as Unity or Unreal Engine to record & replay user interactions, studies, meetings, gameplay, etc.
- Usage as a recorder for generic data i.e. to log messages, record float buffer, etc.  


**Requirements**:  
- [git](https://gitforwindows.org/)
- [cmake](https://cmake.org/download/)
- [make for windows](http://gnuwin32.sourceforge.net/packages/make.htm)  
- compiler and cmake generator i.e. [MinGW-w64](https://sourceforge.net/projects/mingw-w64/) 
- NDK for Android cross-compilation [NDK](https://developer.android.com/ndk/downloads)

**Architecture**
![Architecture](Images/Architecture.png)


Setup:
========================
**Step 1**: Clone the repository **recursively** to ensure all submodules are included:
```
git clone --recursive https://github.com/vrsys/recording-plugin
```  


**Step 2**: Update the paths in CMakeLists.txt to match your setup (UNITY_PLUGINS_DIR).

**Step 3**: Configure whether compression and multi-threading should be used (Config.h.in). E.g.:
```
#define COMPRESSION 0 // 1 for on
#define COMPRESSION_LEVEL 5 // 1 (low) to 9 (high but slow)
#define MULTI_THREADED 1 // 0 for off
```

**Step 4**: Build the plugin using the following commands:
```
cd recording-plugin
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target RecordingPlugin
```  


**Step 5**: Integrate with your project by using the exposed endpoints from Src/Recording/RecorderEndpoints.cpp. Example in C#:
```
[DllImport("RecordingPlugin")]
private static extern bool CreateNewRecordingFile(int recorder_id, string directory, int directory_length, string recording_name, int recording_name_length);
```

Setup for Android (.so):
========================
**Step 1**: Clone the repository **recursively** to ensure all submodules are included:
```
git clone --recursive https://github.com/vrsys/recording-plugin
```  


**Step 2**: Update the paths in CMakeLists.txt to match your setup (UNITY_PLUGINS_DIR).


**Step 3**: Configure target architecture in buildAndroid.bat (ANDROID_ABI, ANDROID_PLATFORM)


**Step 4**: Build the plugin for Android using Windows:
```
cd recording-plugin
.\buildAndroid.bat
```  


Record & Replay Pipeline:
========================
Note: The following code is pseudo-code and demonstrates general usage. It cannot be directly copy-pasted into your project.

**Configure Parameters**:
```cs
SetRecordingMaxBufferSize(recorderId, recordingMaxBufferSize);
SetSoundRecordingMaxBufferSize(recorderId, recordingSoundMaxBufferSize);
SetReplayBufferNumber(recorderId, replayBufferNumber);
SetReplayBufferStoredTimeInterval(recorderId, replayBufferTimeInterval);
```
**Begin Recording**:
```cs
CreateNewRecordingFile(recorderId, directory, directory.Length, recordName, recordName.Length);
RecordObjectAtTimestamp(recorderId, name, name.Length, id, matrixDTO, recordTime, infoDTO);
RecordSoundDataAtTimestamp(recorderId, audioData, recordedSamples, sampleRate, startIndex, recordTime, audioSourceId);
RecordGenericAtTimestamp(recorderId, recordTime, genericInfoId, int[] intData, float[] floatData, char[] charData);
StopRecording(recorderId);
```

**Begin Replay**:
```cs
OpenExistingRecordingFile(recorderId, directory, directory.Length, recordName, recordName.Length);

float[] matrixDTO = new float[20];
int[] infoDTO = new int[2];
fixed (float* p = matrixDTO)
{
    fixed (int* u = infoDTO)
    {
        GetTransformAndInformationAtTime(recorderId, name, name.Length, id, loadTime, (IntPtr) p, (IntPtr) u)
    }
}

float[] soundDTO = new float[4800];
fixed (float* p = soundDTO)
{
    GetSoundChunkForTime(recorderId, 0, loadTime, (IntPtr) p);
}

int[] intDTO = new int[10];
float[] floatDTO = new float[10];
byte[] charDTO = new byte[10];
        
fixed (float* f = floatDTO)
{
    fixed (int* i = intDTO)
    {
        fixed (byte* c = charDTO)
        {
            bool result = GetGenericAtTime(recorderId, loadTime, id, (IntPtr)i, (IntPtr)f, (IntPtr)c);
        }
    }
}
                
StopReplay(recorderId);
```

Data Analysis:
========================
For query-based data analysis the analysis endpoints (Src/Analysis/AnalysisEndpoints.cpp) can be used for integration into other software or the analysis_tests target for analysis using C++ can be build using CMake.
Queries for analysis can be combined and extended and applied to multiple recordings.
An example usage is shown in AnalysisTest/Test.cpp.

### Query creation in C++
To create and combine queries logical operators (&, |, !) can be used.
```cpp
std::shared_ptr<IntervalSoundActivationAnalysisRequest> sound_request_1 = std::make_shared<IntervalSoundActivationAnalysisRequest>(microphone_audio, temporal_search_interval, sound_level);
std::shared_ptr<IntervalSoundActivationAnalysisRequest> sound_request_2 = std::make_shared<IntervalSoundActivationAnalysisRequest>(world_audio, temporal_search_interval, sound_level);
manager.add_interval_analysis_request(sound_request_1);
manager.add_interval_analysis_request(sound_request_2);
manager.add_interval_analysis_request(sound_request_1 & sound_request_2);
manager.add_interval_analysis_request(sound_request_1 | sound_request_2);
manager.add_interval_analysis_request(sound_request_1 & (!sound_request_2));
manager.process_interval_analysis_requests_for_all_files();
```

Data Format:
========================
Each recording consists out of 4 different files, storing meta information (.recordmeta), transformation (.txt), audio (_sound.txt) and generic data (_arb.txt) separately.
The meta information file contains various sections, including:

- **Recording Length:** Information about the length of the recording.
- **Temporal Offset To Global Start Time:** Offset in ms to the previously defined global point in time at which the recording should start.
- **Object Data Count:** Information about the number of objects for which transform data was recorded.
- **Object Data:** Information about recorded scene objects, including names, UUIDs, paths, components, and prefabs.
- **Sound IDs:** List of sound identifiers used during the recording.
- **Generic IDs:** List of generic identifiers.
- **Transform IDs:** List of transform identifiers.
- **DTO Layouts:** Definitions for TransformDTO, SoundDTO, and GenericDTO layouts.

### Example File Content

```plaintext
28.3154 0 888
/Audio Source;-110300; ;0.000000;Transform,AudioSource,; 
/DefaultUser5881 [Local User];-155024; ;0.000000;Transform,LineRenderer,; 
...
---------------------------Sound IDs------------------------------
0,1,
---------------------------Generic IDs------------------------------
...
---------------------------Transform IDs------------------------------
-161210,-161196,-161184,...
---------------------------DTO Layouts------------------------------
TransformDTO: int id;float t;float lp[3];float lr[4];float ls[3];float gp[3];float gr[4];float gs[3];int act;int p_id;
SoundDTO: int id;float t;float s_t;float e_t;int s_n;int s_i;int s_r;int c_n;int c_go_id;float s_d[4800];
genericDTO: int id;float t;int i[10];float f[10];char c[10];
```

### Explanation of the First Numbers

The first line of the meta information file contains three numbers:

- **28.3154:** This represents the total recording time in seconds.
- **0:** This indicates the global recording start time offset.
- **888:** This is the size of the name-UUID map, which specifies the number of objects recorded.

Export of data:
========================
To support analysis using other software, utility functions can be used to export recorded data to WAV and CSV.
```cpp
void export_transform_data_to_CSV(std::string const& transform_file_path);

void export_generic_data_to_CSV(std::string const& generic_file_path);

void export_sound_data_to_WAV(std::string const& sound_file_path);
```

Debugging:
========================
**On Windows**:  
Plugin output is logged both in the Unity console and the Unity editor log file, which is located at:
```
%LOCALAPPDATA%\Unity\Editor\Editor.log
```

**On Android**:  
The debug output of the plugin can be read using the [android debug bridge](https://developer.android.com/studio/command-line/adb):
```
adb logcat
```
