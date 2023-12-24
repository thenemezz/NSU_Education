#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include "../converters/Converters.h"
#include "../wav_files/WavFiles.h"

class View {
public:
    void printStart() const;
    void printEnd() const;
    void printHelp() const;
};

class App {
private:
    View view;
    std::unique_ptr<ConfigReader> configs;
    std::unique_ptr<StreamManager> wavFiles;
public:
    App(int argc, char** argv);
    void start();
};
