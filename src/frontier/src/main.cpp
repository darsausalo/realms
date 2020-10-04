#include "components/serialization.h"
#include "frontier/components/base.h"
#include "frontier/states/loading_state.h"
#include <iostream>
#include <motor/app/app.hpp>
#include <motor/entity/components.hpp>

namespace frontier {

class app : public motor::app {
public:
    app(int argc, const char* argv[]) : motor::app(argc, argv) {
        motor::components::define<position>();
        motor::components::define<velocity>();
        motor::components::define<health>();
        motor::components::define<sprite>();
    }
};

} // namespace frontier

int main(int argc, const char* argv[]) {
    return frontier::app(argc, argv).run<frontier::loading_state>();
}
