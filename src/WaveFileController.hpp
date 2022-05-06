#include "ConsoleManager.hpp"

class WaveFileController : public ConsoleManager {
public:
  explicit WaveFileController();

  bool AskForAndReadUserWavFile();
  void write_bit8_mono(std::vector<float> buffer, std::string outputFile, wav_header wavHeader);
  void write_bit16_mono(std::vector<float> buffer, std::string outputFile, wav_header wavHeader);
  void write_bit8_stereo(std::vector<float> buffer, std::vector<float> buffer2, std::string outputFile, wav_header wavHeader);
  void write_bit16_stereo(std::vector<float> buffer, std::vector<float> buffer2, std::string outputFile, wav_header wavHeader);
  void CreateOutputFile(std::string fileName, std::vector<float> buffer,
    std::vector<float> buffer2, wav_header wavHeader, int effects_to_apply);

private:
  ConsoleManager *consoleManager_;
};