#include "ConsoleManager.hpp"
// Ask user for what effect they would like to apply
int ConsoleManager::AskForUserEffectsToApply()
{
    int effects_to_apply = 0;
    PRINTX("What Audio Effects would you like to apply to the file: ");
    PRINTX("1. Echo");
    PRINTX("2. Reverse");
    PRINTX("3. Normalize");
    bool user_is_selecting = true;
    std::string input;
    while (user_is_selecting)
    {
        PRINTX("Select an option 1-3.... To continue");
        std::cin >> input;
        if (input == "1" || input == "2" || input == "3")
                user_is_selecting = false;
        else {
            PRINTX("!-- Invalid Selection --!");
            input.clear();
        }
    }
    return (input[0] - 48);
}

std::string ConsoleManager::AskForUserWaveFile()
{
  std::string input;

  PRINTX("Input wave file name: ");
  std::cin >> input;
  return input;
}

bool ConsoleManager::DisplayFileContents(wav_header wavHeader)
{
  PRINTX("RIFF header                :" << wavHeader.RIFFHeader[0] << wavHeader.RIFFHeader[1] << wavHeader.RIFFHeader[2] << wavHeader.RIFFHeader[3]);
  PRINTX("WAVE header                :" << wavHeader.WAVE[0] << wavHeader.WAVE[1] << wavHeader.WAVE[2] << wavHeader.WAVE[3]);
  PRINTX("FMT                        :" << wavHeader.fmt[0] << wavHeader.fmt[1] << wavHeader.fmt[2] << wavHeader.fmt[3]);
  PRINTX("Chunk Data size            :" << wavHeader.ChunkSize);
  PRINTX("Sampling Rate              :" << wavHeader.sampleRate);
  PRINTX("Number of bits used        :" << wavHeader.bitsPerSample);
  PRINTX("Number of channels         :" << wavHeader.numChannels);
  PRINTX("Number of bytes per second :" << wavHeader.byteRate);
  PRINTX("Data length                :" << wavHeader.dataBytes);
  PRINTX("Audio Format               :" << wavHeader.AudioFormat);
  std::cout << "Block align          :" << wavHeader.sampleAlignment << ENDL;
  std::cout << "Data string          :" << wavHeader.subchunk2ID[0] << wavHeader.subchunk2ID[1] << wavHeader.subchunk2ID[2] << wavHeader.subchunk2ID[3] << ENDL;
  std::cout << "SdataBytes size   :" << wavHeader.dataBytes << ENDL;
  return true;
}
