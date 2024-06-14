cmake_minimum_required(VERSION 3.20)

project(sound_processor)

add_executable(sound_processor main.cpp factory/ConvertersFactory.h app_logic/App.h app_logic/App.cpp converters/Converters.h converters/Converters.cpp util/Util.cpp util/Util.h wav_files/WavFiles.h wav_files/WavFiles.cpp)

