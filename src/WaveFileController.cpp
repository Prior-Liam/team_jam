#include "WaveFileController.hpp"//okay chekc that the data being read into the first file for sounda data is going into a char * vector

static void echo(std::vector<float> &soundData, wav_header wavHeader, echo_params params)
{
  float gain = 0.25; 
  params.delay = (int)(0.5 * wavHeader.sampleRate);
  for(int i = 0; i < soundData.size(); i++) {
      if ((i - params.delay) < 0) {
        break;
      } else {
        soundData[i] = soundData[i] - (params.gain*soundData[i - params.delay]);
      }
  }
}

static void reverse(std::vector<float>& soundData, wav_header wavHeader)
{
  soundData.reserve(wavHeader.dataBytes);
  std::reverse(soundData.begin(), soundData.end());
}

static void normalize(std::vector<float>& soundData, wav_header wavHeader)
{
  float maxValue = 0, ratio, sum;
  for (int it = 0; it < soundData.size(); it++)
    if (soundData[it] && std::abs(soundData[it]) > std::abs(maxValue))
      maxValue = soundData[it];
  ratio = 1/maxValue;
  for (int it = 0; it < soundData.size(); it++)
    soundData[it] *= ratio;
}

WaveFileController::WaveFileController()
    : consoleManager_(new ConsoleManager()) {}

bool WaveFileController::AskForAndReadUserWavFile()//change htis method name.
{
  std::string filePath;
  wav_header wavHeader;
  int effects_to_apply;
  bool does_wav_file_exist = false;

  uint8_t buffer8Bit;
  int16_t buffer16Bit;
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
          wavFile.read((char*) &buffer8Bit, 1);
          soundData2.push_back(((float)buffer8Bit-128)/127);
        }
      } else if (wavHeader.bitsPerSample == 16) {
        for (int it = 0; it < numSamples; it++) {
          wavFile.read((char *) &buffer16Bit, 2);
          soundData.push_back((float)buffer16Bit/32768);
          wavFile.read((char*) &buffer16Bit, 2);
          soundData2.push_back((float)buffer16Bit/32768);
        }
      }
    }
    effects_to_apply = consoleManager_->AskForUserEffectsToApply();
    switch (effects_to_apply) {//just add if statements here
        case 1:
          echo_params params;
          params.gain = 0.25f;
          params.delay = 6.0f;
          if (wavHeader.numChannels == 1)
            echo(soundData, wavHeader, params);
          else {
            echo(soundData, wavHeader, params);
            echo(soundData2, wavHeader, params);
          }
          break;
        case 2:
          if (wavHeader.numChannels == 1) {
            reverse(soundData, wavHeader);
          } else {
            reverse(soundData, wavHeader);
            reverse(soundData2, wavHeader);
          }
          break;
        case 3:
          if (wavHeader.numChannels == 1) {
            normalize(soundData, wavHeader);
          } else {
            normalize(soundData, wavHeader);
            normalize(soundData2, wavHeader);
          }
          break;
        default:
          throw "\nThis shouldn't have happened\n"; exit(0);
    }
    CreateOutputFile(filePath, soundData, soundData2, wavHeader, effects_to_apply);
    consoleManager_->DisplayFileContents(wavHeader);
    return true;
  }
  else {
    PRINTX("File: " << filePath << " does not exist");
  }
  return false;
}

void WaveFileController::write_bit16_stereo(std::vector<float> buffer, std::vector<float> buffer2, std::string outputFile, wav_header wavHeader) {
  union {
    uint16_t bufferOutput;
    char b[2];
  } bufferSpot;
  std::ofstream of(outputFile, std::ios::binary | std::ios::out);
  // std::cout << "im bere\n" << outputFile << std::endl;
  if (of.is_open())
  {
    of.write((char *)&wavHeader, sizeof(wav_header));
    // std::cout << buffer.size() << std::endl;
    for (int it = 0; it < buffer.size(); it++){
      buffer[it] = (buffer[it] * 32768);
      buffer2[it] = (buffer2[it] * 32768);
      if (buffer[it] > 32767)//need to change this possibly
        buffer[it] = 32767;
      if (buffer[it] < -32767)
        buffer[it] = -32767;
      if (buffer2[it] > 32767)
        buffer2[it] = 32767;
      if (buffer2[it] < -32767)
        buffer2[it] = -32767;
      bufferSpot.bufferOutput = (uint16_t)buffer[it];
      of.write(bufferSpot.b, 2);
      bufferSpot.bufferOutput = (uint16_t)buffer2[it];
      of.write(bufferSpot.b, 2);
    }
    std::cout << "wrote the file successfully!" << std::endl;
    of.flush();
    of.close();
    } else {
    std::cerr << "Failed to open file : " << outputFile << std::endl;
  }
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
      buffer[it] = (buffer[it] * 32768.0f);
      if (buffer[it] > 32767.0f)//need to change this possibly
        buffer[it] = 32767.0f;
      if (buffer[it] < -32767.0f)
        buffer[it] = -32767.0f;
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

void WaveFileController::write_bit8_stereo(std::vector<float> buffer, std::vector<float> buffer2, std::string outputFile, wav_header wavHeader) {
  union {
    uint8_t bufferOutput;
    char b[2];
  } bufferSpot;
  std::ofstream of(outputFile, std::ios::binary | std::ios::out);
  if (of.is_open())
  {
    of.write((char *)&wavHeader, sizeof(wav_header));
    for (int it = 0; it < buffer.size(); it++){
      buffer[it] = ((buffer[it] * 127.0f) + 128.0f);
      buffer2[it] = ((buffer2[it] * 127.0f) + 128.0f);
      if (buffer[it] > 255)
        buffer[it] = 255;
      if (buffer[it] < 0)
        buffer[it] = 0;
      if (buffer2[it] > 255)
        buffer2[it] = 255;
      if (buffer2[it] < 0)
        buffer2[it] = 0;
      
      bufferSpot.bufferOutput = (uint8_t)buffer[it];
      of.write((char*)&bufferSpot.bufferOutput, 1);
      bufferSpot.bufferOutput = (uint8_t)buffer2[it];
      of.write((char*)&bufferSpot.bufferOutput, 1);
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

void WaveFileController::CreateOutputFile(
    std::string fileName, std::vector<float> buffer, std::vector<float> buffer2,
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
    write_bit8_stereo(buffer, buffer2, outputFile, wavHeader);
  else if (wavHeader.numChannels == 2 && wavHeader.bitsPerSample == 16)
    write_bit16_stereo(buffer, buffer2, outputFile, wavHeader);
}




  // std::cout << "swagging here\n";
    // //


        // for (int i = 0; i < wavHeader.sdataBytes/wavHeader.sampleAlignment; i++) {//
        //   wavFile.read((char*) &buffer, 1);
        //   soundData.push_back(((float)buffer)/32768);//scaling is done here
        //   std::cout << soundData[i] << ENDL;