#include "Utils.hpp"
// #include "WaveFileController.hpp"
class ConsoleManager
{
public:
  ConsoleManager() = default;

  std::string AskForUserWaveFile();
  std::vector<int> AskForUserEffectsToApply();

  bool DisplayFileContents(wav_header wavHeader, std::ifstream &wavFile);

private:
};