#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include "SDL3/SDL.h"
#include "WAV.h"

// http://soundfile.sapp.org/doc/WaveFormat/

#define MAX_FP_LEN 401U

static uint16_t read16Bit(const uint8_t *buffer) { return (uint16_t)(buffer[0] | ((uint16_t)buffer[1] << 8)); }
static uint32_t read32Bit(const uint8_t *buffer) { return (uint32_t)(buffer[0] | ((uint32_t)buffer[1] << 8) | ((uint32_t)buffer[2] << 16) | ((uint32_t)buffer[3] << 24)); }

static _Bool parseHeader(const uint8_t *buffer, size_t fileBytes, HEADER *header) {
    size_t offset = 12;
    _Bool fmtFound = 0;
    _Bool dataFound = 0;
    if (fileBytes < 12) {
        SDL_Log("file is too small\n");
        return 1;
    }
    memcpy(header->Riff.ID, &buffer[0], 4);
    header->Riff.fileSize = read32Bit(&buffer[4]);
    memcpy(header->Riff.fileFormatID, &buffer[8], 4);
    if (memcmp(&buffer[0], "RIFF", 4) != 0 || memcmp(&buffer[8], "WAVE", 4) != 0) {
        SDL_Log("found no RIFF/WAVE identifiers\n");
        return 1;
    }
    while (offset + 8 <= fileBytes) {
        const uint8_t *chunkID = &buffer[offset];
        
        uint32_t chunkSize = read32Bit(&buffer[offset + 4]);
        size_t chunkDataOffset = offset + 8;
        if (chunkDataOffset > UINT32_MAX) {
            SDL_Log("chunkDataOffset exceeds supported range\n");
            return 1;
        }
        if (chunkSize > fileBytes - chunkDataOffset) {
            SDL_Log("chunk size exceeds remaining file\n");
            return 1;
        }

        size_t nextOffset = chunkDataOffset + (size_t)chunkSize + (size_t)(chunkSize & 1U);
        if (nextOffset > fileBytes) {
            SDL_Log("WAV chunk size from offset exceeds total file size\n");
            return 1;
        }

        if (memcmp(chunkID, "fmt ", 4) == 0) {
            if (chunkSize < 16) {
                SDL_Log("invalid fmt chunk size\n");
                return 1;
            }
            memcpy(header->Format.ID, chunkID, 4);
            header->Format.size = chunkSize;
            header->Format.audioFormat = read16Bit(&buffer[chunkDataOffset + 0]);
            header->Format.channelsNumber = read16Bit(&buffer[chunkDataOffset + 2]);
            header->Format.frequency = read32Bit(&buffer[chunkDataOffset + 4]);
            header->Format.bytesPerSec = read32Bit(&buffer[chunkDataOffset + 8]);
            header->Format.bytesPerBlock = read16Bit(&buffer[chunkDataOffset + 12]);
            header->Format.bitsPerSample = read16Bit(&buffer[chunkDataOffset + 14]);
            fmtFound = 1;
        }
        else if (memcmp(chunkID, "data", 4) == 0) {
            memcpy(header->Data.ID, chunkID, 4);
            header->Data.size = chunkSize;
            header->Data.dataStart = (uint32_t)chunkDataOffset;
            dataFound = 1;
        }

        if (fmtFound && dataFound)
            return 0;
        offset = nextOffset;
    }

    SDL_Log("required WAV chunks not found (fmt/data)\n");
    return 1;
}

static _Bool loadWAV(FILE **wavFile, HEADER *header, uint8_t **buffer) {
    _Bool isError;
    size_t fileBytes = 0;
    char filePath[MAX_FP_LEN] = { 0 };

    // will replace with UI field for filePath instead of stdin
    printf("Input a WAV file:\n");
    int doScan = scanf_s("%400s", filePath, MAX_FP_LEN);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {} // clears leftover chars in stdin, empty body -> char is ignored
    if (doScan != 1) {
        SDL_Log("unexpected error with chosen filePath\n");
        return 1;
    }
    
    isError = fopen_s(wavFile, filePath, "rb");
    if (isError) {
        SDL_Log("unable to open WAV file\n");
        return 1;
    }

    fseek(*wavFile, 0, SEEK_END);
    fileBytes = (size_t)ftell(*wavFile);

    *buffer = (uint8_t *)malloc(fileBytes * sizeof(uint8_t));
    if (!*buffer) {
        SDL_Log("unable to allocate memory to wav file buffer\n");
        fclose(*wavFile);
        *wavFile = (void *)0;
        return 1;
    }

    rewind(*wavFile);
    if (fread(*buffer, 1, fileBytes, *wavFile) != fileBytes) {
        SDL_Log("unable to read complete WAV file\n");
        fclose(*wavFile);
        *wavFile = (void *)0;
        if (*buffer) {
            free(*buffer);
            *buffer = (void *)0;
        }
        return 1;
    }

    if (fclose(*wavFile) != 0) {
        SDL_Log("wavFile stream not closed\n");
        return 1;
    }
    *wavFile = (void *)0;

    isError = parseHeader(*buffer, fileBytes, header);
    if (isError && *buffer) {
        free(*buffer);
        *buffer = (void *)0;
        return 1;
    }

    // [POST PARSE ERRORS]
    if (header->Format.channelsNumber != 2) {
        SDL_Log("only 2 channels are supported, WAV channels: %u\n", header->Format.channelsNumber);
        if (*buffer) {
            free(*buffer);
            *buffer = (void *)0;
        }
        return 1;
    }
    if (header->Format.bytesPerBlock == 0) {
        SDL_Log("bytesPerBlock in WAV file cannot equal zero, bytesPerBlock: %u\n", header->Format.bytesPerBlock);
        if (*buffer) {
            free(*buffer);
            *buffer = (void *)0;
        }
        return 1;
    }

    return 0;
}

void freeWAV(uint8_t **wavBuffer) {
    if (*wavBuffer) {
        free(*wavBuffer);
        *wavBuffer = (void *)0;
    }
}

int parseWAV(HEADER *header, uint8_t **wavBuffer) {
    FILE *wavFile = (void *)0;
    _Bool isError;

    isError = loadWAV(&wavFile, header, wavBuffer);
    if (isError) {
        SDL_Log("error with loadWAV function\n");
        return 1;
    }

    return 0;
}