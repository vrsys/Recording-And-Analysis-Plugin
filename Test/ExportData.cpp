//
// Created by Anton-Lammert on 08.01.2025.
//


#include "../Utils/Utils.h"

int main(int argc, char *argv[]) {
    std::string generic_data_path = "placeholder.txt";
    std::string transform_data_path = "placeholder.txt";
    std::string audio_data_path = "placeholder.txt";

    Utils::export_generic_data_to_CSV(generic_data_path);
    Utils::export_transform_data_to_CSV(transform_data_path);
    Utils::export_transform_data_to_CSV(transform_data_path, "/Lobby-HMDUser/Camera Offset/RightHand Controller");
    Utils::export_transform_data_to_CSV(transform_data_path, "/Lobby-HMDUser/Camera Offset/LeftHand Controller");
    Utils::export_transform_data_to_CSV(transform_data_path, "/Lobby-HMDUser/Camera Offset/Main Camera");
    Utils::export_sound_data_to_WAV(audio_data_path);
    return 1;
}