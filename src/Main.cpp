#include "Utils.hpp"
#include "WaveFileManager.hpp"

int main() // maybe check header later
{
    bool does_wav_file_exist = false;
    wav_file_manager = new WaveFileManager();
    while (!does_wav_file_exist)
    {
        does_wav_file_exist = wav_file_manager->AskForAndReadUserWavFile();
    }
    return 0;
}
