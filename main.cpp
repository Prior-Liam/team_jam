#include "Echo.h"
#include "Reverser.h"
#include "Normalizer.h"
#include <vector>

#define PRINTX(x) std::cout << x << std::endl
#define PRINTDIVIDER PRINTX("-------------------------------" << ENDL);
#define PRINT std::cout
#define ENDL std::endl
#define MAX_16BIT 65535

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

// Ask user for what effect they would like to apply
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
            {
                (effects_to_apply)[0] = 1;
                break;
            }
            else if (input[0] == '2')
            {
                (effects_to_apply)[1] = 1;
                break;
            }
            else if (input[0] == '3')
            {
                (effects_to_apply)[2] = 1;
                break;
            }
            else if (input[0] == 'y')
                user_is_selecting = false;
            else
                PRINTX("!-- Invalid Selection --!");
            input.clear();
        }
    }
    PRINTDIVIDER
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

std::vector<float> normalize(std::vector<float> soundData, wav_header wavHeader)
{
    std::vector<float> bufferNew;
    float maxValue = 0, percentageOfChange, sum;
    bufferNew.reserve(wavHeader.subchunk2Size);
    for (int it = 0; it < soundData.size(); it++)
        if (soundData[it] && std::abs(soundData[it]) > std::abs(maxValue))
            maxValue = soundData[it];
    sum = 255 - maxValue;
    percentageOfChange = (2.5 / sum) * 100;
    for (int it = 0; it < soundData.size(); it++)
    {
        bufferNew.push_back((short)(((soundData[it] * percentageOfChange) + soundData[it]) * MAX_16BIT));
    }
    return (bufferNew);
}

void CreateOutputFile(std::string fileName, std::vector<float> buffer, wav_header wavHeader)
{
    std::string outputFile;
    for (char c : fileName)
        if (c != '.')
            outputFile.push_back(c);
    outputFile += "Normalized.wav";
    std::ofstream of(outputFile);

    auto *outputBuffer = new short[buffer.size()];
    for (int it = 0; it < buffer.size(); it++)
    {
        outputBuffer[it] = (short)(buffer[it] * MAX_16BIT);
    }
    if (of.is_open())
    {
        wavHeader.subchunk2Size = buffer.size() * wavHeader.sampleAlignment;
        wavHeader.ChunkSize = wavHeader.subchunk2Size + 44 - 8;
        of.write((char *)&wavHeader, sizeof(wav_header));
        of.write((char *)outputBuffer, wavHeader.subchunk2Size);
        of.flush();
        of.close();
        std::cout << "wrote the file successfully!" << std::endl;
    }
    else
    {
        std::cerr << "Failed to open file : " << std::endl;
    }
}

bool DisplayFileContents(wav_header wavHeader, std::ifstream &wavFile)
{
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
    // fclose(wavFile);
    return true;
}

bool AskForAndReadUserWavFile()
{
    std::string filePath;
    wav_header wavHeader;
    std::vector<int> effects_to_apply;
    bool does_wav_file_exist = false;
    size_t bytes_read = 1;

    std::vector<float> soundData;
    short *buffer = nullptr;
    std::vector<float> bufferFinal;

    filePath = AskForUserWaveFile();
    PRINTX("File received from user: " << filePath);
    std::ifstream wavFile(filePath, std::ios::binary | std::ios::in);
    if (wavFile.is_open())
    {
        PRINTX("File Successfully opened");
        PRINTDIVIDER
        wavFile.read((char *)&wavHeader, sizeof(wav_header));
        buffer = new short[wavHeader.subchunk2Size];
        wavFile.read((char *)buffer, wavHeader.subchunk2Size);
        if (bytes_read > 0)
        {
            // uint16_t bytesPerSample = wavHeader.bitsPerSample / 8;      // Number     of bytes per sample
            // uint64_t numSamples = wavHeader.ChunkSize / bytesPerSample; // How many samples are in the wav file?
            for (int i = 0; i < wavHeader.subchunk2Size / wavHeader.sampleAlignment; i++)
            {
                soundData.push_back((float)buffer[i] / MAX_16BIT);
            }
        }
        delete[] buffer;
        effects_to_apply = AskForUserEffectsToApply();
        if (effects_to_apply[0])
        {
            // bufferFinal =
        }
        if (effects_to_apply[1])
        {
            // bufferFinal =
        }
        if (effects_to_apply[2])
        {
            bufferFinal = normalize(soundData, wavHeader);
        }
        CreateOutputFile(filePath, bufferFinal, wavHeader);
        DisplayFileContents(wavHeader, wavFile);
        return true;
    }
    else
    {
        PRINTX("File: " << filePath << " does not exist");
    }
    return false;
}

int main() // maybe check header later
{
    std::string filePath;
    bool does_wav_file_exist = false;
    while (!does_wav_file_exist)
    {
        does_wav_file_exist = AskForAndReadUserWavFile();
    }
    return 0;
}
