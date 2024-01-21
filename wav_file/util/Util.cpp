#include "Util.h"
#include <iostream>
#include <iostream>
#include <fstream>
#include <string>

namespace {
    bool checkBit(char byte, int pos) {
        return static_cast<bool>((1 << pos) & byte);
    }

    bool bit(short byte, int pos) {
        return static_cast<bool>((1 << pos) & byte);
    }

    void setTrue(short& byte, int pos) {
        byte |= (1 << pos);
    }

    void aTrue(char& byte, int pos) {
        byte |= (1 << pos);
    }
}


Sample::Sample(char* bytes): value(0) {
    for (int i = 0; i < bitDepth; ++i) {
        if (checkBit(bytes[i / 8], i % 8)) {
            setTrue(value, i);
        }
    }
};

Sample::Sample(const Sample& other) {
    value = other.value;
};

short Sample::getValue() const {
    return value;
}

void Sample::setValue(short newValue) {
    value = newValue;
}

char Sample::getFirstByte() const{
    char out = 0;
    for (int i = 0; i < 8; ++i) {
        if (bit(value, i)) {
            aTrue(out, i % 8);
        }
    }
    return out;
}

char Sample::getLastByte() const {
    char out = 0;
    for (int i = 8; i < 16; ++i) {
        if (bit(value, i)) {
            aTrue(out, i % 8);
        }
    }
    return out;
}


ConfigReader::ConfigReader(const std::string& configFile): config(configFile) {
    if (!config.is_open()) {
        throw std::invalid_argument("The config configFile did not open;");
    }
}

bool ConfigReader::getConfiguration(std::vector<std::string>& configuration) {
    std::string str;
    size_t posInConfig = 0;
    if (getline(config, str)) {
        for (size_t i = 0, j = 0; i < str.size(); ++i) {
            if (str[i] == ' ' || i == str.size() - 1) {
                if (i == str.size() - 1) {
                    ++i;
                }
                if (posInConfig >= configuration.size()) {
                    configuration.push_back(str.substr(j, i - j));
                } else {
                    configuration[posInConfig] = str.substr(j, i - j);
                }
                j = i + 1;
                ++posInConfig;
            }
        }
    } else {
        return false;
    }
    if (posInConfig < configuration.size()) {
        configuration.erase(configuration.begin() + posInConfig, configuration.end());
    }
    return true;
}