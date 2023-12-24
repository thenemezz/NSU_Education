#include "WavFiles.h"
#include <iostream>

namespace {
    bool checkBit(char byte, int pos) {
        return static_cast<bool>((1 << pos) & byte);
    }

    void setTrue(size_t& byte, int pos) {
        byte |= (1 << pos);
    }

    bool bit(size_t byte, int pos) {
        return static_cast<bool>((1 << pos) & byte);
    }

    void aTrue(char& byte, int pos) {
        byte |= (1 << pos);
    }

    size_t stringToU64I(const std::string& str) {
        size_t value = 0;
        for (int i = 0; i < str.size() * 8; ++i) {
            if (checkBit(str[i / 8], i % 8)) {
                setTrue(value, i);
            }
        }
        return value;
    };

    std::string u64IToString(size_t num, size_t length) {
        std::string str(length, 0);
        for (int i = 0; i < length * 8; ++i) {
            if (bit(num, i)) {
                aTrue(str[i / 8], i % 8);
            }
        }
        return str;
    }

    size_t checkFormat(std::ifstream& stream, int iteration, size_t constant) {
        std::string str(iteration, 0);
        size_t offset = 0;
        for (int i = 0; i < iteration; stream.get(str[i]), ++i, ++offset);
        if (stringToU64I(str) != constant) {
            throw std::invalid_argument("Incorrect audio format;");
        }
        return offset;
    }
}

WavInFile::WavInFile(const std::string& ifile):
        file(ifile, std::ios::binary)
{
    if (!file.is_open()) {
        throw std::invalid_argument("The input file cannot be opened: " + ifile + ";");
    }

    std::string str(4, 0);
    for (int i = 0; i < 4; file.get(str[i]), ++i, ++currentPosition);
    if (str != "RIFF") {
        throw std::invalid_argument("Incorrect file header;");
    }

    for (int i = 0; i < 4; file.get(str[i]), ++i, ++currentPosition);
    size_t chunkSize = stringToU64I(str);
    if (chunkSize < minHeaderLength - 8) {
        throw std::invalid_argument("Incorrect file header;");
    }

    str.resize(8, 0);
    for (int i = 0; i < 8; file.get(str[i]), ++i, ++currentPosition);

    if (str != "WAVEfmt ") {
        throw std::invalid_argument("Incorrect file header;");
    }

    currentPosition += checkFormat(file, 4, pcmFmtChunkSize);
    currentPosition += checkFormat(file, 2, pcmAudioFormat);
    currentPosition += checkFormat(file, 2, numberOfChannels);
    currentPosition += checkFormat(file, 4, samplingRate);
    currentPosition += checkFormat(file, 4, bytesPerSecond);
    currentPosition += checkFormat(file, 2, bytesInSample);
    currentPosition += checkFormat(file, 2, bitsInSample);

    str.resize(4);
    for (int i = 0; i < 4; file.get(str[i]),  ++i, ++currentPosition);
    while (currentPosition != chunkSize + 8) {
        if (str != "data") {
            for (int i = 0; i < 3; str[i] = str[i+1], ++i);
            file.get(str[3]);
            ++currentPosition;
        } else {
            for (int i = 0; i < 4; file.get(str[i]), ++i, ++currentPosition);
            if (currentPosition + stringToU64I(str) == chunkSize + 8) {
                sizeOfData = stringToU64I(str);
                break;
            }
        }
    }

    sizeOfFile = chunkSize + 8;
}

size_t WavInFile::getSizeOfData() const {
    return sizeOfData;
}

bool WavInFile::isEnd() const {
    return sizeOfFile < currentPosition + 2;
}

Stream WavInFile::getStream() {
    Stream out;
    out.reserve(samplingRate);
    if (gotSecond) {
        currentPosition -= samplingRate * 2;
        file.seekg(currentPosition);
    }
    char* buff = new char[2];
    std::string str(bytesInSample, 0);
    for (size_t i = 0; i < samplingRate && !this->isEnd(); ++i) {
        file.read(buff, static_cast<std::streamsize>(bytesInSample));
        currentPosition += 2;
        out.push_back(buff);
    }
    delete[] buff;
    gotSecond = true;
    return out;
}

void WavInFile::updateSecond() {
    if (gotSecond == false) {
        file.seekg(currentPosition + samplingRate * 2);
        currentPosition += samplingRate * 2;
    }
    gotSecond = false;
}

WavOutFile::WavOutFile(const std::string& ofile): file(ofile, std::ios::binary) {
    if(!file.is_open()) {
        throw std::invalid_argument("The output file cannot be opened;");
    }
    file.seekp(minHeaderLength);
};

void WavOutFile::setStream(const Stream& stream) {
    char* buff = new char[2];
    for (size_t i = 0; i < stream.size(); ++i) {
        buff[0] = stream[i].getFirstByte();
        buff[1] = stream[i].getLastByte();
        file.write(buff, static_cast<std::streamsize>(2));
    }
    delete[] buff;
    sizeOfData += stream.size() * 2;
}

WavOutFile::~WavOutFile() {
    file.seekp(0);
    file <<
         "RIFF" + u64IToString(44 - 8 + sizeOfData, 4) +
         "WAVEfmt " + u64IToString(pcmFmtChunkSize, 4) +
         u64IToString(pcmAudioFormat, 2) +
         u64IToString(numberOfChannels, 2) +
         u64IToString(samplingRate, 4) +
         u64IToString(bytesPerSecond, 4) +
         u64IToString(bytesInSample, 2) +
         u64IToString(bitsInSample, 2) +
         "data" + u64IToString(sizeOfData, 4);
}

StreamManager::StreamManager(const std::string& ofile): outFile(ofile) {};

void StreamManager::pushWavInStream(const std::string& istream) {
    inFiles.push_back(istream);
}

Stream StreamManager::getStream(size_t ind) {
    return inFiles[ind].getStream();
}

void StreamManager::setToOutStream(const Stream& data) {
    outFile.setStream(data);
}

void StreamManager::updateSecond() {
    for (size_t i = 0; i < inFiles.size(); inFiles[i].updateSecond(), ++i);
}

size_t StreamManager::getNumberOfSeconds() const {
    size_t numberOfSeconds = inFiles[0].getSizeOfData();
    numberOfSeconds = numberOfSeconds / (samplingRate * bytesInSample);
    if (inFiles[0].getSizeOfData() % (samplingRate * bytesInSample) != 0) {
        ++numberOfSeconds;
    }
    return numberOfSeconds;
}