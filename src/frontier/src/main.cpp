#include "components/serialization.hpp"
#include "frontier/components/base.hpp"
#include "frontier/states/startup_state.hpp"
#include <motor/app/app.hpp>
#include <motor/entity/components.hpp>

namespace frontier {

class app final : public motor::app {
public:
    app(int argc, const char* argv[]) : motor::app(argc, argv) {
        motor::components::define<position,
                                  motor::component_specifier::FINAL>();
        motor::components::define<velocity>();
        motor::components::define<health>();
        motor::components::define<sprite>();
    }
};

} // namespace frontier

int main(int argc, const char* argv[]) {
    return frontier::app(argc, argv).run<frontier::startup_state>();
}
