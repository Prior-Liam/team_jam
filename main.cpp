#include "Echo.h"
#include "Reverser.h"
#include "Normalizer.h"
#include <vector>

#define PRINTX(x) std::cout << x << std::endl
#define PRINT std::cout
#define ENDL std::endl
#define MAX_16BIT 65535

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
//     short sampleAlignment;
//     short bitsPerSample;

//     char subchunk2ID[4];
//     int subchunk2Size;
// } WAV_HEADER;

typedef struct WAV_HEADER
{
    /* RIFF Chunk Descriptor */
    char RIFFHeader[4]; // RIFF Header Magic header
    int ChunkSize;      // RIFF Chunk Size
    char WAVE[4];       // WAVE Header
    /* "fmt" sub-chunk */
    char fmt[4];           // FMT header
    int Subchunk1Size;     // Size of the fmt chunk
    short AudioFormat;     // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    short numChannels;     // Number of channels 1=Mono 2=Sterio
    int sampleRate;        // Sampling Frequency in Hz
    int byteRate;          // bytes per second
    short sampleAlignment; // 2=16-bit mono, 4=16-bit stereo ( numChannels * bytesPerSample )
    short bitsPerSample;   // Number of bits per sample
    /* "data" sub-chunk */
    uint8_t subchunk2ID[4]; // "data"  string
    uint32_t subchunk2Size; // Sampled data length
} wav_header;

// struct header_struct {
//   char chunk_id[4];
//   union int_data chunk_size;
//   char format[4];
//   char subchunk1_id[4];
//   union int_data subchunk1_size;
//   union short_data audio_format;
//   union short_data num_channels;
//   union int_data sample_rate;
//   union int_data byte_rate;
//   union short_data block_align;
//   union short_data bits_per_sample;
//   char subchunk2_id[4];
//   union int_data subchunk2_size;
// };

std::vector<int> AskForUserEffectsToApply()
{
    std::vector<int> effects_to_apply = {0, 0, 0};
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

void CreateOutputFile(std::string fileName)
{
    std::ofstream of(fileName);
    if (of.is_open())
    {
        of << "Some text here" << std::endl;
        of.flush();
        of.close();
        std::cout << "wrote the file successfully!" << std::endl;
    }
    else
    {
        std::cerr << "Failed to open file : " << std::endl;
    }
}

bool DisplayFileContents(std::ifstream &wavFile)
{
    // Read the header
    wav_header wavHeader;
    size_t bytes_read = 1;
    short *buffer = nullptr;

    wavFile.read((char *)&wavHeader, sizeof(wav_header));
    buffer = new short[wavHeader.subchunk2Size];
    wavFile.read((char *)buffer, wavHeader.subchunk2Size);

    // std::ofstream of("TEST.wav");
    // if(of.is_open()) {
    //     of << sizeof(wav_header)wavHeader;
    //     of << buffer;
    // }

    if (bytes_read > 0)
    {
        // Read the data
        uint16_t bytesPerSample = wavHeader.bitsPerSample / 8;      // Number     of bytes per sample
        uint64_t numSamples = wavHeader.ChunkSize / bytesPerSample; // How many samples are in the wav file?

        std::vector<float> soundData;
        for (int i = 0; i < wavHeader.subchunk2Size / wavHeader.sampleAlignment; i++)
        {
            soundData.push_back((float)buffer[i] / MAX_16BIT);
        }
        // PRINTX("soundData Length: " << soundData.size());
        // for (size_t i = 0; i < soundData.size(); i++)
        // {
        //     PRINTX("| " << soundData[i] << " |");
        // }

        delete[] buffer;
        buffer = nullptr;

        // std::cout << "File is                    :" << filelength << " bytes." << ENDL;
        PRINTX("RIFF header                :" << wavHeader.RIFFHeader[0] << wavHeader.RIFFHeader[1] << wavHeader.RIFFHeader[2] << wavHeader.RIFFHeader[3]);
        PRINTX("WAVE header                :" << wavHeader.WAVE[0] << wavHeader.WAVE[1] << wavHeader.WAVE[2] << wavHeader.WAVE[3]);
        PRINTX("FMT                        :" << wavHeader.fmt[0] << wavHeader.fmt[1] << wavHeader.fmt[2] << wavHeader.fmt[3]);
        PRINTX("Chunk Data size            :" << wavHeader.ChunkSize);

        // Display the sampling Rate from the header
        PRINTX("Sampling Rate              :" << wavHeader.sampleRate);
        PRINTX("Number of bits used        :" << wavHeader.bitsPerSample);
        PRINTX("Number of channels         :" << wavHeader.numChannels);
        PRINTX("Number of bytes per second :" << wavHeader.byteRate);
        PRINTX("Data length                :" << wavHeader.subchunk2Size);
        PRINTX("Audio Format               :" << wavHeader.AudioFormat);
        // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

        std::cout << "Block align          :" << wavHeader.sampleAlignment << ENDL;
        std::cout << "Data string          :" << wavHeader.subchunk2ID[0] << wavHeader.subchunk2ID[1] << wavHeader.subchunk2ID[2] << wavHeader.subchunk2ID[3] << ENDL;
        std::cout << "Subchunk2Size size   :" << wavHeader.subchunk2Size << ENDL;
    }
    // fclose(wavFile);
    return true;
}

bool GetUserWaveFile()
{
    std::string filePath;
    bool does_wav_file_exist = false;
    std::vector<int> effects_to_apply;

    filePath = AskForUserWaveFile();
    PRINTX("File received from user: " << filePath);

    std::ifstream wavFile(filePath, std::ios::binary | std::ios::in);
    if (wavFile.is_open())
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