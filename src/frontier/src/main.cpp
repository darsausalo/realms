#include "loading_state.h"
#include <iostream>
#include <motor/core/application.h>

int main(int argc, const char* argv[]) {
    return motor::application(argc, argv).run<frontier::loading_state>();
}
