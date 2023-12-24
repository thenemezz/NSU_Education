#include "App.h"
#include "../factory/ConvertersFactory.h"
#include <list>

//парсим входные аргументы
namespace {
    std::vector<std::string> parsArgs(int argc, char** argv) {
        std::vector<std::string> args;
        args.reserve(argc - 1);
        int pos = 2;
        if (argc < 5 && argc != 2) {
            throw std::invalid_argument("Incorrect number of input parameters entered;");
        } else if (argc == 2) {
            return args;
        }
        std::string argv1 = argv[1], argv2 = argv[2];

        std::string arg = argv[pos++];
        if (arg.size() < 4 || arg.substr(arg.size() - 4, 4) != ".txt") {
            throw std::invalid_argument("The configuration file is missed;");
        }
        args.push_back(arg);
        for (; pos < argc; ++pos) {
            arg = argv[pos];
            if (arg.size() < 4 || arg.substr(arg.size() - 4, 4) != ".wav") {
                throw std::invalid_argument("The file is listed, not in waw format;");
            }
            args.push_back(arg);
        }
        return args;
    }
}

void View::printStart() const {
    std::cout << "The conversion process has started:\n";
}

void View::printHelp() const {
    std::cout << "Help: How to run the program\n";
    std::cout << "Usage: program_name [-h] [-c config_file] [wav_files...]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h            Display this help message\n";
    std::cout << "  -c <config_file>  Specify the configuration file\n";
    std::cout << "  wav_files     List of input WAV files to process\n";
}


void View::printEnd() const{
    std::cout << "The conversion was completed successfully." << std::endl;
}
//продолжаем парсить по нужным местам дату
App::App(int argc, char** argv) {
    std::vector<std::string> args;
    args.reserve(argc);
    try {
        args = parsArgs(argc, argv);
        configs = std::make_unique<ConfigReader>(args[0]);
        wavFiles = std::make_unique<StreamManager>(args[1]);
        for (size_t i = 2; i < args.size(); ++i) {
            wavFiles->pushWavInStream(args[i]);
        }
    } catch (std::invalid_argument& err) {
        std::cout << err.what() << std::endl;
        throw;
    }
}

void App::start() {
    ConvertersFactory factory;
    factory.add<MixConverter>("mix");
    factory.add<MuteConverter>("mute");
    factory.add<InsertConverter>("insert");
    std::list<AbstractConverter*> converters;
    view.printStart();
    {
        std::vector<std::string> configuration;
        while (configs->getConfiguration(configuration)) {
            auto converter = factory.create(configuration[0]);
            if (!converter) {
                throw std::invalid_argument("The listed converter is missing;");
            }
            converters.push_back(converter);
            converters.back()->setConfiguration(configuration);
        }
    }
    for (size_t second = 0; second < wavFiles->getNumberOfSeconds(); ++second) {
        Stream stream = wavFiles->getStream(0);
        bool killedConverter = false;
        for (auto iter = converters.begin(); iter != converters.end() && !converters.empty(); ++iter) {
            if (killedConverter) {
                --iter;
                killedConverter = false;
            }
            if (second >= (*iter)->getStartSecond() && second <= (*iter)->getEndSecond()) {
                ParamsForConvert params(stream);
                params.reserve((*iter)->getCountOfInputs());
                for (size_t i = 0; i < (*iter)->getCountOfInputs(); ++i) {
                    Stream otherStream = wavFiles->getStream((*iter)->getIndexOfInput(i) - 1);
                    params.pushBack(otherStream);
                }
                stream = (*iter)->convert(params);
            }
            else if (second > (*iter)->getEndSecond()) {
                delete *iter;
                iter = converters.erase(iter);
                killedConverter = true;
            }
        }
        wavFiles->setToOutStream(stream);
        wavFiles->updateSecond();
    }
    for (auto item: converters) {
        delete item;
    }
    view.printEnd();
}