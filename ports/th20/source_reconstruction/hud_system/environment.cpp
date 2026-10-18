#include "hud.hpp"
#include "../program_entry/program_entry.hpp"
#include "../text_renderer/text.hpp"
namespace th20::source::hud::environment {
sprite::Controller& sprites(){return *program_entry::sprite_controller;}
sprite::AnimationFile& notice_file(){return *text::renderer->animation_file;}
}
