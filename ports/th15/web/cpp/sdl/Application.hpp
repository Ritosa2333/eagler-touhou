#pragma once
#include "../game/Types.hpp"
#include <memory>
#include <string>
namespace th15::sdl {
struct ApplicationState;
class Application {
 std::unique_ptr<ApplicationState> state;
public:
 Application();~Application();bool initialize(bool audio_device=true);
 bool step(u32 held,u32 pressed,u32 repeated,float fps=60,bool lost_focus=false);
 const std::string& error()const;const i32* projection()const;
};
}
