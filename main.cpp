#include "Echo.h"
#include "Reverser.h"
#include "Normalizer.h"
#include <vector>

#define PRINTX(x) std::cout << x << std::endl
#define PRINT std::cout
#define ENDL std::endl

// typedef struct wav_header
// {
//     char chunkID[4];
//     int chunkSize;
//     char format[4];

//     char subchunk1ID[4];
//     int subchunk1Size;
//     short audioFormat;
//     short numChannels;
//     int sampleRate;
//     int byteRate;
//     short blockAlign;
//     short bitsPerSample;

//     char subchunk2ID[4];
//     int subchunk2Size;
// } WAV_HEADER;

typedef struct WAV_HEADER
{
    /* RIFF Chunk Descriptor */
    uint8_t RIFFHeader[4]; // RIFF Header Magic header
    uint32_t ChunkSize;    // RIFF Chunk Size
    uint8_t WAVE[4];       // WAVE Header
    /* "fmt" sub-chunk */
    char fmt[4];         // FMT header
    int Subchunk1Size;   // Size of the fmt chunk
    short AudioFormat;   // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    short numChannels;   // Number of channels 1=Mono 2=Sterio
    int sampleRate;      // Sampling Frequency in Hz
    int byteRate;        // bytes per second
    short blockAlign;    // 2=16-bit mono, 4=16-bit stereo
    short bitsPerSample; // Number of bits per sample
    /* "data" sub-chunk */
    uint8_t subchunk2ID[4]; // "data"  string
    uint32_t subchunk2Size; // Sampled data length
} wav_header;

std::vector<int> AskForUserEffectsToApply()
{
    std::vector<int> effects_to_apply = {0,0,0};
    PRINTX("What Audio Effects would you like to apply to the file: ");
    PRINTX("1. Echo");
    PRINTX("2. Reverse");
    PRINTX("3. Normalize");

    bool user_is_selecting = true;
    std::string input;
    while (user_is_selecting)
    {
        PRINTX("Select an option 1-3... Press y to continue");
        std::cin >> input;
        if (input.size() > 0)
        {
            if (input[0] == '1')
                (effects_to_apply)[0] = 1;
            if (input[0] == '2')
                (effects_to_apply)[1] = 1;
            if (input[0] == '3')
                (effects_to_apply)[2] = 1;
            if (input[0] == 'y')
                user_is_selecting = false;
            input.clear();
        }
    }
    return effects_to_apply;
}

std::string AskForUserWaveFile()
{
    std::string input;

    PRINTX("Input wave file name: ");
    std::cin >> input;
    return input;
}

// find the file size
int getFileSize(FILE *inFile)
{
    int fileSize = 0;
    fseek(inFile, 0, SEEK_END);

    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    return fileSize;
}

int *reverse(int arr[], int n)
{
    for (int low = 0, high = n - 1; low < high; low++, high--)
    {
        std::swap(arr[low], arr[high]);
    }
    return arr;
}

bool DisplayFileContents(FILE *wavFile)
{
    // Read the header
    wav_header wavHeader;
    int headerSize = sizeof(wav_header), filelength = 0;

    size_t bytesRead = fread(&wavHeader, 1, headerSize, wavFile);
    std::cout << "Header Read " << bytesRead << " bytes." << ENDL;
    if (bytesRead > 0)
    {
        // Read the data
        uint16_t bytesPerSample = wavHeader.bitsPerSample / 8;      // Number     of bytes per sample
        uint64_t numSamples = wavHeader.ChunkSize / bytesPerSample; // How many samples are in the wav file?
        static const uint16_t BUFFER_SIZE = 4096;
        int *buffer = new int[BUFFER_SIZE];
        while ((bytesRead = fread(buffer, sizeof buffer[0], BUFFER_SIZE / (sizeof buffer[0]), wavFile)) > 0)
        {
            /** DO SOMETHING WITH THE WAVE DATA HERE **/
            PRINTX(buffer);
        }
        delete[] buffer;
        buffer = nullptr;
        filelength = getFileSize(wavFile);

        std::cout << "File is                    :" << filelength << " bytes." << ENDL;
        std::cout << "RIFF header                :" << wavHeader.RIFFHeader[0] << wavHeader.RIFFHeader[1] << wavHeader.RIFFHeader[2] << wavHeader.RIFFHeader[3] << ENDL;
        std::cout << "WAVE header                :" << wavHeader.WAVE[0] << wavHeader.WAVE[1] << wavHeader.WAVE[2] << wavHeader.WAVE[3] << ENDL;
        std::cout << "FMT                        :" << wavHeader.fmt[0] << wavHeader.fmt[1] << wavHeader.fmt[2] << wavHeader.fmt[3] << ENDL;
        std::cout << "Data size                  :" << wavHeader.ChunkSize << ENDL;

        // Display the sampling Rate from the header
        std::cout << "Sampling Rate              :" << wavHeader.sampleRate << ENDL;
        std::cout << "Number of bits used        :" << wavHeader.bitsPerSample << ENDL;
        std::cout << "Number of channels         :" << wavHeader.numChannels << ENDL;
        std::cout << "Number of bytes per second :" << wavHeader.byteRate << ENDL;
        std::cout << "Data length                :" << wavHeader.subchunk2Size << ENDL;
        std::cout << "Audio Format               :" << wavHeader.AudioFormat << ENDL;
        // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

        std::cout << "Block align                :" << wavHeader.blockAlign << ENDL;
        std::cout << "Data string                :" << wavHeader.subchunk2ID[0] << wavHeader.subchunk2ID[1] << wavHeader.subchunk2ID[2] << wavHeader.subchunk2ID[3] << ENDL;
    }
    fclose(wavFile);
    return true;
}

bool GetUserWaveFile()
{
    std::string filePath;
    bool does_wav_file_exist = false;
    std::vector<int> effects_to_apply;

    filePath = AskForUserWaveFile();
    PRINTX("File received from user: " << filePath);

    // std::ifstream file(filePath, std::ios::binary | std::ios::in);
    // short *buffer = nullptr;
    FILE *wavFile = fopen(filePath.c_str(), "r");
    if (wavFile != nullptr)
    {
        PRINTX("File Successfully opened");
        effects_to_apply = AskForUserEffectsToApply();

        DisplayFileContents(wavFile);
        return true;
    }
    else
    {
        PRINTX("File: " << filePath << " does not exist");
    }
    return false;
}

int main()
{
    std::string filePath;
    bool does_wav_file_exist = false;

    while (!does_wav_file_exist)
    {
        does_wav_file_exist = GetUserWaveFile();
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