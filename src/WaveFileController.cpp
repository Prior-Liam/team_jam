#include "WaveFileController.hpp"

static std::vector<float> echo(std::vector<float> &soundData, wav_header wavHeader, echo_params params)
{
  std::vector<float> output;
  auto decay = log(0.001f) / log(params.gain);
  int newFileSize = int(soundData.size() + params.delay * decay);
  std::vector<float> extendedSoundData(newFileSize, 0.0f);
  extendedSoundData.insert(extendedSoundData.begin(), soundData.begin(), soundData.end());
  output.reserve(extendedSoundData.size());
  for (int it = 0; it < soundData.size(); it++)
  {
    if (it > params.delay)
      output.push_back(extendedSoundData[it] + output[it - params.delay] * params.gain);
    else
      output.push_back(extendedSoundData[it]);
  }
  return (output);
}

static std::vector<float> reverse(std::vector<float> soundData, wav_header wavHeader)
{
  std::vector<float> bufferNew;
  float maxValue = 0, percentageOfChange, sum;
  bufferNew.reserve(wavHeader.subchunk2Size);
  for (int it = 0; it < soundData.size(); it++)
  {
    bufferNew.push_back((short)((soundData[it]) * MAX_16BIT));
    // std::cout << "normal = [" << (soundData[it]) << "]"<< std::endl; 
  }
  std::reverse(bufferNew.begin(), bufferNew.end());
  //waves/8bit-mono.wav

  return (bufferNew);
}

// NEED TO HANDLE STEREO :,(
static std::vector<float> normalize(std::vector<float> soundData, wav_header wavHeader)
{
  std::vector<float> bufferNew;
  float maxValue = 0, percentageOfChange, sum;
  bufferNew.reserve(wavHeader.subchunk2Size);
  for (int it = 0; it < soundData.size(); it++)
    if (soundData[it] && std::abs(soundData[it]) > std::abs(maxValue))
      maxValue = soundData[it];
  sum = 255 - maxValue;
  percentageOfChange = (2.5 / sum) * 100;
  for (int it = 0; it < soundData.size(); it++)
  {
    bufferNew.push_back((short)(((soundData[it] * percentageOfChange) + soundData[it]) * MAX_16BIT));
  }
  return (bufferNew);
}

WaveFileController::WaveFileController()
    : consoleManager_(new ConsoleManager()) {}

bool WaveFileController::AskForAndReadUserWavFile()
{
  std::string filePath;
  wav_header wavHeader;
  std::vector<int> effects_to_apply;
  bool does_wav_file_exist = false;
  size_t bytes_read = 1;

  short *buffer = nullptr;
  std::vector<float> soundData;
  std::vector<float> bufferFinal;

  filePath = consoleManager_->AskForUserWaveFile();
  PRINTX("File received from user: " << filePath);
  std::ifstream wavFile(filePath, std::ios::binary | std::ios::in);
  std::cout << "path = " << filePath << std::endl;
  if (wavFile.is_open())
  {
    PRINTX("File Successfully opened");
    PRINTDIVIDER
    wavFile.read((char *)&wavHeader, sizeof(wav_header));
    buffer = new short[wavHeader.subchunk2Size];//think we need to make a split between 8bit and 16bit jere
    wavFile.read((char *)buffer, wavHeader.subchunk2Size);
    if (bytes_read > 0)
    {
      // uint16_t bytesPerSample = wavHeader.bitsPerSample / 8;      // Number     of bytes per sample
      // uint64_t numSamples = wavHeader.ChunkSize / bytesPerSample; // How many samples are in the wav file?
      int chunkSize = 0;
      if (wavHeader.numChannels == 2)
        chunkSize = wavHeader.subchunk2Size * 2;
      else
        chunkSize = wavHeader.subchunk2Size;
      for (int i = 0; i < chunkSize / wavHeader.sampleAlignment; i++)
      {
        soundData.push_back((float)buffer[i] / MAX_16BIT);
        std::cout << "[" << (float)buffer[i] / MAX_16BIT << std::endl;
      }
    }

    delete[] buffer;
    effects_to_apply = consoleManager_->AskForUserEffectsToApply();
    if (effects_to_apply[0])
    {
      echo_params params;
      params.gain = 0.07f;
      params.delay = 6.0f;
      bufferFinal = echo(soundData, wavHeader, params);
    }
    if (effects_to_apply[1])
    {
      bufferFinal = reverse(soundData, wavHeader);
    }
    if (effects_to_apply[2])
    {
      bufferFinal = normalize(soundData, wavHeader);
    }
    CreateOutputFile(filePath, bufferFinal, wavHeader, effects_to_apply);
    consoleManager_->DisplayFileContents(wavHeader, wavFile);
    return true;
  }
  else
  {
    PRINTX("File: " << filePath << " does not exist");
  }
  return false;
}

void WaveFileController::CreateOutputFile(
    std::string fileName, std::vector<float> buffer,
    wav_header wavHeader, std::vector<int> effects_to_apply)
{
  std::string outputFile;
  for (char c : fileName)
    if (c != '.')
      outputFile.push_back(c);
  if (effects_to_apply[0])
    outputFile += "Echo.wav";
  else if (effects_to_apply[1])
    outputFile += "Reversed.wav";
  else if (effects_to_apply[2])
    outputFile += "Normalized.wav";
  else
    outputFile += "Copy.wav";
  std::ofstream of(outputFile);

  auto *outputBuffer = new short[buffer.size()];
  for (int it = 0; it < buffer.size(); it++)
  {
    outputBuffer[it] = (short)(buffer[it] * MAX_16BIT);
  }
  if (of.is_open())
  {
    wavHeader.subchunk2Size = buffer.size() * wavHeader.sampleAlignment;
    wavHeader.ChunkSize = wavHeader.subchunk2Size + 44 - 8;
    of.write((char *)&wavHeader, sizeof(wav_header));
    of.write((char *)outputBuffer, wavHeader.subchunk2Size);
    of.flush();
    of.close();
    std::cout << "wrote the file successfully!" << std::endl;
  }
  else
  {
    std::cerr << "Failed to open file : " << outputFile << std::endl;
  }
}