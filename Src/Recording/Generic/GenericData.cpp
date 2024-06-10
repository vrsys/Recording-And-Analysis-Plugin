//
// Created by Mortiferum on 18.03.2022.
//

#include "GenericData.h"


GenericData::GenericData() : RecordableData(-1, -1.0f) {};

GenericData::GenericData(float time, int id, int *i_arr, float *f_arr, char *c_arr) : RecordableData(id, time) {
    std::copy(i_arr, i_arr + num_c, int_ar);
    std::copy(f_arr, f_arr + num_c, float_ar);
    std::copy(c_arr, c_arr + char_c, char_ar);
};

GenericData::GenericData(const GenericData &data) : RecordableData(data.id, data.time) {
    std::copy(data.int_ar, data.int_ar + num_c, int_ar);
    std::copy(data.float_ar, data.float_ar + num_c, float_ar);
    std::copy(data.char_ar, data.char_ar + char_c, char_ar);
}

GenericData GenericData::interpolate(const std::shared_ptr<GenericData>next_data, float t) {
    return interpolate((*next_data), t);
}

GenericData GenericData::interpolate(const GenericData &next_data, float t) {
    if (t >= 1.0f)
        t = 1.0f;
    if (t <= 0.0f)
        t = 0.0f;

    GenericData interpolated_data{*this};
    interpolated_data.time = time;
    interpolated_data.id = id;
    return interpolated_data;
}

void GenericData::update_data(int *i_arr, float *f_arr, char *c_arr) {
    std::copy(int_ar, int_ar + num_c, i_arr);
    std::copy(float_ar, float_ar + num_c, f_arr);
    std::copy(char_ar, char_ar + char_c, c_arr);
}

GenericData::GenericData(const std::shared_ptr<GenericData>data) : RecordableData(data->id, data->time) {
    std::copy(data->int_ar, data->int_ar + num_c, int_ar);
    std::copy(data->float_ar, data->float_ar + num_c, float_ar);
    std::copy(data->char_ar, data->char_ar + char_c, char_ar);
}

std::ofstream &GenericData::write(std::ofstream &ofs) const {
    ofs << id << ' ' << time << ' ';
    for (int i: int_ar) {
        ofs << i << ' ';
    }

    for (float i: float_ar) {
        ofs << i << ' ';
    }

    for (char i: char_ar) {
        ofs << i << ' ';
    }

    ofs << "\n";
    return ofs;
}

std::ifstream &GenericData::read(std::ifstream &ifs) {
    ifs >> id >> time;

    for(int i = 0; i < num_c; i++){
        ifs >> int_ar[i];
    }

    for(int i = 0; i < num_c; i++){
        ifs >> float_ar[i];
    }

    for(int i = 0; i < char_c; i++){
        ifs >> char_ar[i];
    }

    return ifs;
}

GenericData::GenericData(const GenericDTO &dto) : RecordableData(dto.id, dto.t){
    std::copy(dto.i, dto.i + num_c, int_ar);
    std::copy(dto.f, dto.f + num_c, float_ar);
    std::copy(dto.c, dto.c + char_c, char_ar);
}

GenericData::~GenericData() {
    //std::cout << "Destroying Generic Data\n";
}

void GenericData::write_to_stream(std::ofstream &file) const {
    GenericDTO dto = to_dto();
    file.write((const char*) &dto, sizeof(GenericDTO));
}

GenericDTO GenericData::to_dto() const {
    GenericDTO dto;
    dto.id = id;
    dto.t = time;
    std::copy(int_ar, int_ar + num_c, dto.i);
    std::copy(float_ar, float_ar + num_c, dto.f);
    std::copy(char_ar, char_ar + char_c, dto.c);
    return dto;
}
