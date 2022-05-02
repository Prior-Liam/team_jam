
#include "WaveFileController.hpp"

int main()
{
    bool does_wav_file_exist = false;
    int i = 0;
    WaveFileController *wav_file_controller = new WaveFileController();
    while (!does_wav_file_exist)
        does_wav_file_exist = wav_file_controller->AskForAndReadUserWavFile();

    return 0;
}
