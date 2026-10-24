#pragma once
#include <memory>
#include <vector>

namespace th15::sdl {
// A frame remains immutable while any consumer retains it. The pool owns one
// reference; only entries with no consumer can be reused by the next Draw.
template<class Command> class FrameCommandPool {
public:
 using Commands=std::vector<Command>;
 using Writable=std::shared_ptr<Commands>;
 Writable acquire(){
  for(const auto& frame:frames)if(frame.use_count()==1)return frame;
  auto frame=std::make_shared<Commands>();frames.push_back(frame);return frame;
 }
 void clear(){frames.clear();}
private:
 std::vector<Writable> frames;
};
}
