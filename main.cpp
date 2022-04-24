#include "Echo.h"
#include "Reverser.h"
#include "Normalizer.h"

#define PRINTX(x) std::cout << x << std::endl
#define PRINT std::cout
#define ENDL std::endl

std::string GetUserWaveFile()
{
    std::string input;
    const char *filePath;

    std::cout << "Input wave file name: " << ENDL;
    std::cin >> input;
    return input;
}

int main()
{
    std::string filePath;
    bool does_wav_file_exist = false;

    while (!does_wav_file_exist)
    {
        filePath = GetUserWaveFile();
        PRINTX("File received from user: " << filePath);

        std::ifstream file(filePath, std::ios::binary | std::ios::in);
        short *buffer = nullptr;
        if (file.is_open()) {
            PRINTX("File Successfully opened");
            does_wav_file_exist = true;
        } else {
            PRINTX("File: " << filePath << " does not exist");
        }
    }

    return 0;
}

// void WaveFileManager::read(const std::string &fileName)
// {
// std::ifstream file(fileName, std::ios::binary | std::ios::in);
// short* buffer = nullptr;
// if (file.is_open())
// {
//     file.read((char*) &waveHeader, sizeof(wav_header));
//     buffer = new short [waveHeader.data_bytes];
//     file.read((char*) buffer, waveHeader.data_bytes);

//     for (int i = 0; i < waveHeader.data_bytes / waveHeader.sample_alignment; i++)
//     {
//         soundData.push_back((float) buffer[i] / MAX_16BIT);
//     }
// file.close();
// }
// else
// {
//     std::cout << "File not found" << std::endl;
// }
// delete[] buffer;
// }

typedef struct wav_header
{
    char chunkID[4];
    int chunkSize;
    char format[4];

    char subchunk1ID[4];
    int subchunk1Size;
    short audioFormat;
    short numChannels;
    int sampleRate;
    int byteRate;
    short blockAlign;
    short bitsPerSample;

    char subchunk2ID[4];
    int subchunk2Size;
} WAV_HEADER;
