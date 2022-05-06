#include "ConsoleManager.hpp"

class WaveFileController : public ConsoleManager {
public:
  explicit WaveFileController();

  bool AskForAndReadUserWavFile();
  void write_bit8_mono(std::vector<float> buffer, std::string outputFile, wav_header wavHeader);
  void write_bit16_mono(std::vector<float> buffer, std::string outputFile, wav_header wavHeader);
  void write_bit8_stereo(std::vector<float> buffer, std::string outputFile, wav_header wavHeader);
  ///write_bit16_mono();
  ///write_bit16_stereo();
  void CreateOutputFile(std::string fileName, std::vector<float> buffer,
                        wav_header wavHeader, int effects_to_apply);

private:
  ConsoleManager *consoleManager_;
};