#ifndef WAV_H
#define WAV_H

#include <stdint.h>

typedef struct {
    // RIFF (MAIN) CHUNK
    struct {
        uint32_t fileSize;
        char fileFormatID[4]; // big endian
        char ID[4]; // big endian
    } Riff;
    // FORMAT CHUNK
    struct {
        char ID[4]; // big endian
        uint32_t size;
        uint32_t frequency;     // (sample rate) number of samples per second, or hertz
        uint32_t bytesPerSec;   // frequency * channelsNumber * bitsPerSample / 8
        uint16_t audioFormat;
        uint16_t channelsNumber;
        uint16_t bytesPerBlock; // channelsNumber * bitsPerSample / 8
        uint16_t bitsPerSample;
    } Format;
    // DATA CHUNK
    struct {
        uint32_t dataStart;
        uint32_t size;
        char ID[4]; // big endian
    } Data;
} HEADER;

extern int parseWAV(const char *filePath, HEADER *header, uint8_t **wavBuffer);
extern void freeWAV(uint8_t **wavBuffer);

#endif