#pragma once
#include <vector>
#include <string>
#include <fstream>

const size_t bitDepth = 16;

const size_t pcmFmtChunkSize = 16;

const size_t pcmAudioFormat = 1;

const size_t numberOfChannels = 1;

const size_t samplingRate = 44100;

const size_t bytesPerSecond = 88200;

const size_t bytesInSample = 2;

const size_t bitsInSample = 16;

const size_t minHeaderLength = 44;

class Sample {
private:
    short value;
public:
    Sample(char* bytes);
    Sample(const Sample& other);
    short getValue() const;
    void setValue(short newValue);
    char getFirstByte() const;
    char getLastByte() const;
};

using Stream = std::vector<Sample>;

class ConfigReader {
private:
    std::ifstream config;
public:
    ConfigReader(const std::string& configFile);
    bool getConfiguration(std::vector<std::string>& configuration);
};