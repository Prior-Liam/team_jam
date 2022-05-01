class WaveFileController
{
public:
  explicit WaveFileController();

  bool AskForAndReadUserWavFile();
  void CreateOutputFile(std::string fileName, std::vector<float> buffer,
                        wav_header wavHeader, std::vector<int> effects_to_apply);

private:
  ConsoleManager *console_manager_;
};