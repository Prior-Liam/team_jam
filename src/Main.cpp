#include "Utils.hpp"
#include "WaveFileController.hpp"

int main() // maybe check header later
{
    bool does_wav_file_exist = false;
    wav_file_controller = new WaveFileController();
    while (!does_wav_file_exist)
    {
        does_wav_file_exist = wav_file_controller->AskForAndReadUserWavFile();
    }
    return 0;
}
