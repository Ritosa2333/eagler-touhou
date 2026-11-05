#include "ReplayPlayback.hpp"
namespace th15 {
bool ReplayPlayback::select(u32 stage){if(!file.select(stage)){failure="Replay stage unavailable";return false;}number=stage;clock=-1;input={};touch={};fps=0;finished=false;failure.clear();return true;}
bool ReplayPlayback::tick(bool active){
 if(!failure.empty())return false;if(!active)return true;
 if(clock<0){input.held=input.pressed=input.released=0;touch={};return true;}
 if(finished){input.held=input.pressed=input.released=0;touch={};clock=wrapping_add(clock,1);return true;}
 const auto* stage=file.stage(number);if(!stage){failure="Replay stage became unavailable";return false;}
 input.previous=input.held;
 if(file.frame()<stage->frames){const auto raw=file.tick();touch=raw.touch;if(raw.end){input.held=input.pressed=input.released=0;touch={};finished=true;return true;}input.held=raw.held;input.calculate();fps=raw.fps;}
 else {file.tick();input.held=input.pressed=input.released=0;touch={};}
 clock=wrapping_add(clock,1);return true;
}
}
