#include "ConsoleManager.hpp"

class WaveFileController : public ConsoleManager {
public:
  explicit WaveFileController();

  bool AskForAndReadUserWavFile();
  void CreateOutputFile(std::string fileName, std::vector<float> buffer,
                        wav_header wavHeader, int effects_to_apply);

private:
  ConsoleManager *consoleManager_;
};