#include "services.hpp"
#include "clock.hpp"
#include "../program_entry/unrecovered_dependencies.hpp"
namespace th20::source::program_entry::unrecovered {
bool set_rounding_mode(std::uint32_t mode) { return platform::set_rounding_mode(mode); }
double read_clock(WindowStatePrefix& window) { return platform::read_clock(window); }
int fn_0041b4f0(WindowStatePrefix& window) { return platform::initialize_platform(window,log_buffer); }
int load_configuration(GraphicsStatePrefix& graphics,const char* filename) {
    return platform::load_configuration(graphics,window_state,log_buffer,filename);
}
void save_configuration(ConfigPrefix& config) { platform::save_configuration(config,window_state); }
}
