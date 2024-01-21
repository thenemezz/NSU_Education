#include <iostream>
#include "app_logic/App.h"

int main(int argc, char** argv) {
    try {
        App app(argc, argv);
        app.start();
    } catch (std::invalid_argument err) {
        std::cout << err.what();
    }
}
