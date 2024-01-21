#include "Converters.h"
#include <iostream>

namespace {
    size_t stringToU64I(const std::string& str) {
        size_t ans = 0;
        for (size_t i = 0; i < str.size(); ++i) {
            if (!isdigit(str[i]) || (str.size() > 1 && str[0] == '0')) {
                throw std::invalid_argument("Incorrect timecode;");
            }
            ans *= 10;
            ans += str[i] - '0';
        }
        return ans;
    }
}


ParamsForConvert::ParamsForConvert(const Stream& input1) {
    inputFiles.push_back(input1);
};

Stream& ParamsForConvert::operator[](size_t ind) {
    return inputFiles[ind];
}

const Stream& ParamsForConvert::operator[](size_t ind) const {
    return inputFiles[ind];
}

void ParamsForConvert::pushBack(const Stream& stream) {
    inputFiles.push_back(stream);
}

void ParamsForConvert::reserve(size_t size) {
    inputFiles.reserve(size);
}


size_t AbstractConverter::getStartSecond() const {
    return startSecond;
};

size_t AbstractConverter::getEndSecond() const {
    return endSecond;
};

size_t AbstractConverter::getIndexOfInput(size_t ind) const {
    return numberOfInputs[ind];
};

size_t AbstractConverter::getCountOfInputs() const {
    return numberOfInputs.size();
};


Stream MuteConverter::convert(const ParamsForConvert& parameters) const {
    Stream out = parameters[0];
    for (size_t i = 0; i < out.size(); ++i) {
        out[i].setValue(0);
    }
    return out;
}

void MuteConverter::setConfiguration(const std::vector<std::string>& config) {
    if (config.size() != 3) {
        throw std::invalid_argument("Wrong configuration for mute converter;");
    }
    if (config[1][0] == '$' || config[2][0] == '$') {
        throw std::invalid_argument("Wrong configuration for mute converter;");
    }
    startSecond = stringToU64I(config[1]);
    endSecond = stringToU64I(config[2]);
    if (startSecond > endSecond) {
        throw std::invalid_argument("Incorrect timecode for mute converter;");
    }
};

std::string MuteConverter::getInfo() const {
    std::string info =  "Mute converter. "
                        "Accepts two parameters: startSecond and endSecond "
                        "(startSecond <= endSecond). "
                        "Completely drowns out the interval.";
    return info;
}


Stream MixConverter::convert(const ParamsForConvert& parameters) const {
    Stream out = parameters[0];
    const Stream& input2 = parameters[1];
    for (size_t i = 0; i < out.size() && i < input2.size(); ++i) {
        out[i].setValue((input2[i].getValue() + out[i].getValue()) / 2);
    }
    return out;
}

void MixConverter::setConfiguration(const std::vector<std::string>& config) {
    if (config.size() != 3) {
        throw std::invalid_argument("Wrong configuration for mix converter;");
    }
    if (config[1][0] == '$' && config[2][0] == '$') {
        throw std::invalid_argument("There is no additional stream for mix converter;");
    } else if (config[1][0] != '$' && config[2][0] != '$') {
        throw std::invalid_argument("There is no timecode for the mix converter;");
    } else if (config[1][0] == '$') {
        startSecond = stringToU64I(config[2]);
        numberOfInputs.push_back(stringToU64I(config[1].substr(1, config[1].size() - 1)));
    } else {
        startSecond = stringToU64I(config[1]);
        numberOfInputs.push_back(stringToU64I(config[2].substr(1, config[2].size() - 1)));
    }
    endSecond = SIZE_MAX;
};

std::string MixConverter::getInfo() const {
    std::string info =  "Mix converter. "
                        "Accepts two parameters: $additional flow and initialSecond. "
                        "Takes the average of the two input streams, "
                        "starting from the initial second and writes it to the output stream.";
    return info;
}


Stream InsertConverter::convert(const ParamsForConvert& parameters) const {
    Stream out = parameters[0];
    const Stream& input2 = parameters[1];
    for (size_t i = 0; i < out.size() && i < input2.size(); ++i) {
        out[i].setValue(input2[i].getValue());
    }
    return out;
}

void InsertConverter::setConfiguration(const std::vector<std::string>& config) {
    if (config.size() != 4) {
        throw std::invalid_argument("Wrong configuration for insert converter;");
    }
    int countOfInputs = 0;
    for (size_t i = 1; i < config.size(); ++i) {
        if (config[i][0] == '$') {
            ++countOfInputs;
        }
    }
    if (countOfInputs != 1) {
        throw std::invalid_argument("Incorrect number of input streams for insert converter;");
    }
    if (config[1][0] == '$') {
        startSecond = stringToU64I(config[2]);
        endSecond = stringToU64I(config[3]);
        numberOfInputs.push_back(stringToU64I(config[1].substr(1, config[1].size() - 1)));
    } else if (config[2][0] == '$') {
        startSecond = stringToU64I(config[1]);
        endSecond = stringToU64I(config[3]);
        numberOfInputs.push_back(stringToU64I(config[2].substr(1, config[2].size() - 1)));
    } else {
        startSecond = stringToU64I(config[1]);
        endSecond = stringToU64I(config[2]);
        numberOfInputs.push_back(stringToU64I(config[3].substr(1, config[3].size() - 1)));
    }
    if (startSecond > endSecond) {
        throw std::invalid_argument("Incorrect timecode for insert converter;");
    }
};

std::string InsertConverter::getInfo() const {
    std::string info =  "Insert converter. "
                        "Accepts three parameters: $additional stream, startSecond, "
                        "endSecond (startSecond <= endSecond). "
                        "Inserts samples between startSecond and ensSecond "
                        "from an additional stream into the output stream.";
    return info;
}