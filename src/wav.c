#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "SDL3/SDL.h"

#define TEMP_FILE_PATH "C:/Users/super/Desktop/Progetti/Oscille/temp/badapple.wav"

// http://soundfile.sapp.org/doc/WaveFormat/

typedef struct
{
    // RIFF (MAIN) CHUNK
    struct
    {
        char ID[5]; // big endian
        size_t fileSize;
        char fileFormatID[5]; // big endian
    } Riff;
    // FORMAT CHUNK
    struct
    {
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
    struct
    {
        char ID[5]; // big endian
        uint32_t size;
        size_t dataStart;
    } Data;
} HEADER;

static uint16_t read16Bit(const uint8_t *buffer) { return (uint16_t)(buffer[0] | ((uint16_t)buffer[1] << 8)); }
static uint32_t read32Bit(const uint8_t *buffer) { return (uint32_t)(buffer[0] | ((uint32_t)buffer[1] << 8) | ((uint32_t)buffer[2] << 16) | ((uint32_t)buffer[3] << 24)); }

static _Bool parseHeader(const uint8_t *buffer, size_t fileBytes, HEADER *header)
{
    size_t offset = 12;
    _Bool fmtFound = 0;
    _Bool dataFound = 0;
    if (fileBytes < 12)
    {
        SDL_Log("file is too small to be a RIFF/WAVE file\n");
        return 0;
    }

    memcpy(header->Riff.ID, &buffer[0], 4);
    header->Riff.ID[4] = '\0';

    header->Riff.fileSize = read32Bit(&buffer[4]);
    memcpy(header->Riff.fileFormatID, &buffer[8], 4);
    header->Riff.fileFormatID[4] = '\0';
    if (memcmp(&buffer[0], "RIFF", 4) != 0 || memcmp(&buffer[8], "WAVE", 4) != 0)
    {
        SDL_Log("invalid RIFF/WAVE identifiers\n");
        return 0;
    }

    while (offset + 8 <= fileBytes)
    {
        const uint8_t *chunkID = &buffer[offset];
        uint32_t chunkSize = read32Bit(&buffer[offset + 4]);
        size_t chunkDataOffset = offset + 8;
        size_t nextOffset = chunkDataOffset + (size_t)chunkSize + (size_t)(chunkSize & 1U);
        if (nextOffset > fileBytes)
        {
            SDL_Log("corrupted WAV chunk size\n");
            return 0;
        }

        if (memcmp(chunkID, "fmt ", 4) == 0)
        {
            if (chunkSize < 16)
            {
                SDL_Log("invalid fmt chunk size\n");
                return 0;
            }
            memcpy(header->Format.ID, chunkID, 4);
            header->Format.ID[4] = '\0';

            header->Format.size = chunkSize;
            header->Format.audioFormat = read16Bit(&buffer[chunkDataOffset + 0]);
            header->Format.channelsNumber = read16Bit(&buffer[chunkDataOffset + 2]);
            header->Format.frequency = read32Bit(&buffer[chunkDataOffset + 4]);
            header->Format.bytesPerSec = read32Bit(&buffer[chunkDataOffset + 8]);
            header->Format.bytesPerBlock = read16Bit(&buffer[chunkDataOffset + 12]);
            header->Format.bitsPerSample = read16Bit(&buffer[chunkDataOffset + 14]);
            fmtFound = 1;
        }
        else if (memcmp(chunkID, "data", 4) == 0)
        {
            memcpy(header->Data.ID, chunkID, 4);
            header->Data.ID[4] = '\0';

            header->Data.size = chunkSize;
            header->Data.dataStart = offset;
            dataFound = 1;
        }

        if (fmtFound && dataFound)
            return 1;

        offset = nextOffset;
    }

    SDL_Log("required WAV chunks not found (fmt/data)\n");
    return 0;
}

static _Bool loadWAV(FILE **wavFile, HEADER *header, uint8_t **buffer)
{
    size_t fileBytes = 0;

    *wavFile = fopen(TEMP_FILE_PATH, "rb");
    if (!*wavFile)
    {
        SDL_Log("Unable to open WAV file\n");
        return 0;
    }

    fseek(*wavFile, 0, SEEK_END);
    fileBytes = (size_t)ftell(*wavFile);

    *buffer = (uint8_t *)malloc(fileBytes * sizeof(uint8_t));
    if (!*buffer)
    {
        SDL_Log("Unable to allocate memory to wav file buffer\n");
        fclose(*wavFile);
        *wavFile = (void *)0;
        return 0;
    }

    rewind(*wavFile);
    if (fread(*buffer, 1, fileBytes, *wavFile) != fileBytes)
    {
        SDL_Log("Unable to read complete WAV file\n");
        fclose(*wavFile);
        *wavFile = (void *)0;
        free(*buffer);
        *buffer = (void *)0;
        return 0;
    }

    if (fclose(*wavFile) != 0)
    {
        SDL_Log("wavFile stream not closed\n");
        return 0;
    }
    *wavFile = (void *)0;

    if (!parseHeader(*buffer, fileBytes, header))
    {
        free(*buffer);
        *buffer = (void *)0;
        return 0;
    }

    return 1;
}

int parseWAV(void)
{
    HEADER header = {0};
    uint8_t *buffer = (void *)0;
    FILE *wavFile = (void *)0;
    if (!loadWAV(&wavFile, &header, &buffer))
        return 0;

    printf("%zu BYTES\n", header.Riff.fileSize);
    printf("RiffID: %s\n", header.Riff.ID);
    printf("fileFormatID: %s\n", header.Riff.fileFormatID);
    printf("FormatID: %s\n", header.Format.ID);

    for (size_t i = header.Data.dataStart; i < header.Data.size; i++)
    {
        //printf("%02X ", buffer[i]);

    }

    free(buffer);
    return 1;
}