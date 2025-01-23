#include <stdio.h>

#define COLOR_RED "\033[31m"
#define COLOR_BLUE "\033[34m"
#define COLOR_GREEN "\033[32m"
#define RESET "\033[0m"

#include "header/proxy.h"

int main() {
    run_proxy();
    printf(COLOR_GREEN "Proxy succesful finished! \n" RESET);
}
