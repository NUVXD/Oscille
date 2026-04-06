#ifndef WAV_H
#define WAV_H

typedef struct {
    // RIFF (MAIN) CHUNK
    struct {
        char ID[5]; // big endian
        size_t fileSize;
        char fileFormatID[5]; // big endian
    } Riff;
    // FORMAT CHUNK
    struct {
        char ID[5]; // big endian
        uint32_t size;
        /**
         * 1 = Pulse Code Modulation (PCM):
         * - series of samples at regular time interval
         * - each sample is a specific bit length
         * - value of a sample represents the amplitude of wave at time t
         *
         * 3 = IEEE 754:
         * - not for now
         */
        uint16_t audioFormat;
        uint16_t channelsNumber;
        uint32_t frequency;     // (sample rate) number of samples per second, or hertz
        uint32_t bytesPerSec;   // frequency * channelsNumber * bitsPerSample / 8
        uint16_t bytesPerBlock; // channelsNumber * bitsPerSample / 8
        uint16_t bitsPerSample;
    } Format;
    // DATA CHUNK
    struct {
        char ID[5]; // big endian
        uint32_t size;
        size_t dataStart;
    } Data;
} HEADER;

extern int parseWAV(HEADER *header, uint8_t **wavBuffer);

#endif