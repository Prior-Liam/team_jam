#include <iostream>
#include <fstream>
// #include <stdlib.h>
#include <vector>
#include <cmath>

#define PRINTX(x) std::cout << x << std::endl
#define PRINTDIVIDER PRINTX("-------------------------------" << ENDL);
#define PRINT std::cout
#define ENDL std::endl

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
    int dataBytes; // Sampled data length
} wav_header;

typedef struct ECHO_PARAMS {
    float gain;
    float delay;
} echo_params;