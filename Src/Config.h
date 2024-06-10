#ifndef CONFIG_H_IN
#define CONFIG_H_IN

#include <zlib.h>
#include <stdint.h>

#define PROJECT_NAME "RecordingPlugin"
#define PROJECT_VER  "0.8.4"
#define PROJECT_VER_MAJOR "0"
#define PROJECT_VER_MINOR "8"
#define PROJECT_VER_PATCH "4"
#define COMPRESSION 0
#define COMPRESSION_LEVEL 5 // 1 (low) to 9 (high but slow)
#define MULTI_THREADED 1
const size_t HEADER_SIZE = sizeof(uint32_t); // size of compressed data block header

#endif // INCLUDE_GUARD
