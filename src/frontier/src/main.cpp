#include "frontier/states/loading_state.h"
#include <iostream>
#include <motor/host/application.h>

int main(int argc, const char* argv[]) {
    return motor::application(argc, argv).run<frontier::loading_state>();
}
