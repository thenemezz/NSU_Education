#include <iostream>
#include "app_logic/App.h"


int main(int argc, char** argv) {
    try {
        if (argc == 2) {
            View().printHelp();
        }
        else {
            App app(argc, argv);
            app.start();
        }
    } catch (std::invalid_argument& err) {
        std::cout << err.what();
    }

    return 0;
}
