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

bool WaveFileController::AskForAndReadUserWavFile()//change htis method name.
{
  std::string filePath;
  wav_header wavHeader;
  int effects_to_apply;
  bool does_wav_file_exist = false;
  size_t bytes_read = 1;

  float buffer;
  float buffer2;
  std::vector<float> soundData;
  std::vector<float> soundData2;
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
    int numSamples = wavHeader.subchunk2Size / (wavHeader.numChannels * (wavHeader.bitsPerSample / 8));
    //okay this is where i read the file and store it the vector depending specs make into own method or something.
    if (wavHeader.numChannels == 1) {
      if (wavHeader.bitsPerSample == 8) {
        for (int i = 0; i < numSamples; i++) {
          wavFile.read((char*) &buffer, 1);
          soundData.push_back(((float)buffer-128)/127);
        }
      } else if (wavHeader.bitsPerSample == 16) {
        for (int it = 0; it < numSamples; it++)
          wavFile.read((char *) &buffer, 2);
          soundData.push_back((float)buffer/32768);
      }
      else if (wavHeader.numChannels == 2) {
        if (wavHeader.bitsPerSample == 8) {
        for (int i = 0; i < numSamples; i++) {
          wavFile.read((char*) &buffer, 1);
          soundData.push_back(((float)buffer-128)/127);
          soundData2.push_back(((float)buffer-128)/127);
        }
        } else if (wavHeader.bitsPerSample == 16) {
          for (int it = 0; it < numSamples; it++) {
            wavFile.read((char *) &buffer, 2);
            soundData.push_back((float)buffer/32768);
            soundData2.push_back((float)buffer/32768);
          }
        }
      }
    }//this is user interface stuff
    effects_to_apply = consoleManager_->AskForUserEffectsToApply();
    switch (effects_to_apply) {
        case 1:
          echo_params params;
          params.gain = 0.07f;
          params.delay = 6.0f;
          bufferFinal = echo(soundData, wavHeader, params); break;
        case 2:
          bufferFinal = reverse(soundData, wavHeader); break;
        case 3:
          bufferFinal = normalize(soundData, wavHeader); break;
        default:
          throw "yo what?"; exit(0);
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
    wav_header wavHeader, int effects_to_apply)
{
  std::string outputFile;
  for (char c : fileName)
    if (c != '.')
      outputFile.push_back(c);
  switch (effects_to_apply) {
    case 1:
      outputFile += "Echo.wav";
    case 2:
      outputFile += "Reversed.wav";
    case 3:
      outputFile += "Normalized.wav";
    default:
      outputFile += "Copy.wav";
      break;
  }
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