#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include "../converters/Converters.h"
#include "../wav_files/WavFiles.h"

class View {
private:
    size_t progress = 0;
public:
    void printStart() const;
    void printInfo(const std::vector<std::string>& info) const;
    void printEnd() const;
    void printProgress(size_t totalSecond, size_t second);
};

class App {
private:
    View view;
    std::unique_ptr<ConfigReader> configs;
    bool helpKey = false;
    std::unique_ptr<StreamManager> wavFiles;
public:
    App(int argc, char** argv);
    void start();
};
