#include "WaveFileController.hpp"//okay chekc that the data being read into the first file for sounda data is going into a char * vector

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
  bufferNew.reserve(wavHeader.dataBytes);
  for (int it = 0; it < soundData.size(); it++)
  {
    bufferNew.push_back(soundData[it]);
  }
  std::reverse(bufferNew.begin(), bufferNew.end());
  return (bufferNew);
}

// NEED TO HANDLE STEREO :,(
static std::vector<float> normalize(std::vector<float> soundData, wav_header wavHeader)
{
  std::vector<float> bufferNew;
  float maxValue = 0, percentageOfChange, sum;
  bufferNew.reserve(wavHeader.dataBytes);
  for (int it = 0; it < soundData.size(); it++)
    if (soundData[it] && std::abs(soundData[it]) > std::abs(maxValue))
      maxValue = soundData[it];
  sum = 255 - maxValue;
  percentageOfChange = (2.5 / sum) * 100;
  for (int it = 0; it < soundData.size(); it++)
  {
    bufferNew.push_back((short)(((soundData[it] * percentageOfChange) + soundData[it])));
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

  uint8_t buffer8Bit, buffer8sBit;
  int16_t buffer16Bit, buffer16sBit;
  std::vector<float> soundData;
  std::vector<float> soundData2;

  filePath = consoleManager_->AskForUserWaveFile();
  PRINTX("File received from user: " << filePath);
  std::ifstream wavFile(filePath, std::ios::binary | std::ios::in);
  
  if (wavFile.is_open())
  {
    PRINTX("File Successfully opened");
    PRINTDIVIDER
    wavFile.read((char *)&wavHeader, sizeof(wav_header));
    int numSamples = wavHeader.dataBytes/wavHeader.sampleAlignment;
    if (wavHeader.numChannels == 1) {
      if (wavHeader.bitsPerSample == 8) {
        for (int i = 0; i < numSamples; i++) {
           wavFile.read((char*) &buffer8Bit, 1);
           soundData.push_back(((float)buffer8Bit-128)/127);
        }
      } else if (wavHeader.bitsPerSample == 16) {
        for (int it = 0; it < numSamples; it++) {
          wavFile.read((char *) &buffer16Bit, 2);
          soundData.push_back((float)buffer16Bit/32768);
        }
      }
    } else if (wavHeader.numChannels == 2) {
      if (wavHeader.bitsPerSample == 8) {
        for (int i = 0; i < numSamples; i++) {
          wavFile.read((char*) &buffer8Bit, 1);
          soundData.push_back(((float)buffer8Bit-128)/127);
          soundData2.push_back(((float)buffer8Bit-128)/127);
        }
      } else if (wavHeader.bitsPerSample == 16) {
        for (int it = 0; it < numSamples; it++) {
          wavFile.read((char *) &buffer16Bit, 2);
          soundData.push_back((float)buffer16sBit/32768);
          soundData2.push_back((float)buffer16sBit/32768);
        }
      }
    }
    effects_to_apply = consoleManager_->AskForUserEffectsToApply();
    // switch (effects_to_apply) {//just add if statements here
    //     case 1:
    //       echo_params params;
    //       params.gain = 0.07f;
    //       params.delay = 6.0f;
    //       bufferFinal = echo(soundData, wavHeader, params); break;
    //     case 2:
    //       bufferFinal = reverse(soundData, wavHeader); break;
    //     case 3:
    //       bufferFinal = normalize(soundData, wavHeader); break;
    //     default:
    //       throw "\nThis shouldn't have happened\n"; exit(0);
    // }
    CreateOutputFile(filePath, soundData, wavHeader, effects_to_apply);
    consoleManager_->DisplayFileContents(wavHeader);
    return true;
  }
  else {
    PRINTX("File: " << filePath << " does not exist");
  }
  return false;
}

void WaveFileController::write_bit16_mono(std::vector<float> buffer, std::string outputFile, wav_header wavHeader) {
  union {
    uint16_t bufferOutput;
    char b[2];
  } bufferSpot;
  std::ofstream of(outputFile, std::ios::binary | std::ios::out);
  if (of.is_open())
  {
    of.write((char *)&wavHeader, sizeof(wav_header));
    // std::cout << buffer.size() << std::endl;
    for (int it = 0; it < buffer.size(); it++){
      buffer[it] = (buffer[it] * 32768);
      if (buffer[it] > 32767)//need to change this possibly
        buffer[it] = 32767;
      if (buffer[it] < -32767)
        buffer[it] = -32767;
      bufferSpot.bufferOutput = (uint16_t)buffer[it];
      of.write(bufferSpot.b, 2);
    }
    std::cout << "wrote the file successfully!" << std::endl;
    of.flush();
    of.close();
    } else {
    std::cerr << "Failed to open file : " << outputFile << std::endl;
  }
}

void WaveFileController::write_bit8_mono(std::vector<float> buffer, std::string outputFile, wav_header wavHeader) {
  union {
    uint8_t bufferOutput;
    char b[2];
  } bufferSpot;
  std::ofstream of(outputFile, std::ios::binary | std::ios::out);
  if (of.is_open())
  {
    of.write((char *)&wavHeader, sizeof(wav_header));
    // std::cout << buffer.size() << std::endl;
    for (int it = 0; it < buffer.size(); it++){
      buffer[it] = ((buffer[it] * 127) + 128);
      if (buffer[it] > 255)
        buffer[it] = 255;
      if (buffer[it] < 0)
        buffer[it] = 0;
      bufferSpot.bufferOutput = (uint8_t)buffer[it];
      of.write(bufferSpot.b, 1);
    }
    std::cout << "wrote the file successfully!" << std::endl;
    of.flush();
    of.close();
    } else {
    std::cerr << "Failed to open file : " << outputFile << std::endl;
  }
}
// void WaveFileController::write_bit16_mono() {}
// void WaveFileController::write_bit8_stero() {}
// void WaveFileController::write_bit16_stereo() {}

void WaveFileController::CreateOutputFile(
    std::string fileName, std::vector<float> buffer,
    wav_header wavHeader, int effects_to_apply)
{
  std::string outputFile;
  for (char c : fileName) {
    if (c != '.')
      outputFile.push_back(c);
  }
  switch (effects_to_apply) {
    case 1:
      outputFile += "Echo.wav"; break;
    case 2:
      outputFile += "Reversed.wav"; break;
    case 3:
      outputFile += "Normalized.wav"; break;
    default:
      outputFile += "Copy.wav"; break;
  }
  if (wavHeader.numChannels == 1 && wavHeader.bitsPerSample == 8)
    write_bit8_mono(buffer, outputFile, wavHeader);
  else if (wavHeader.numChannels == 1 && wavHeader.bitsPerSample == 16)
    write_bit16_mono(buffer, outputFile, wavHeader);
  else if (wavHeader.numChannels == 2 && wavHeader.bitsPerSample == 8)
    write_bit8_stereo(buffer, outputFile, wavHeader);
  // else if (wavHeader.numChannels == 2 && wavHeader.bitsPerSample == 16)
  //   write_bit16_stereo(buffer, outputFile, wavHeader);
}




  // std::cout << "swagging here\n";
    // //


        // for (int i = 0; i < wavHeader.sdataBytes/wavHeader.sampleAlignment; i++) {//
        //   wavFile.read((char*) &buffer, 1);
        //   soundData.push_back(((float)buffer)/32768);//scaling is done here
        //   std::cout << soundData[i] << ENDL;