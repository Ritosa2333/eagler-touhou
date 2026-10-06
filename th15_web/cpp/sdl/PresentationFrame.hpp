#pragma once
#include "../../../portable/sdl/Renderer.hpp"
#include "../game/AnmVm.hpp"
#include <map>
#include <tuple>
namespace th15::sdl {
// The authored Draw transaction runs once per tick. Extra frames submit its
// immutable GPU transaction; they never call game callbacks or retain VM pointers.
class PresentationFrame {
public:
 using Key=std::tuple<uintptr_t,u64,u32,u32>;
 struct Sample {
  Key key{};u32 first=0,count=0,fields=0,flags=0;bool world=false;
  i32 script=0,sprite=0,age=0;uintptr_t resource=0;
  Vec3 position{},rotation{};Vec2 scale{},size{},offset{};
 };
 struct Command {
  touhou::sdl::State state;bool clear=false,direct=false,camera=false;
  touhou::graphics::Topology topology{};u32 count=0,stride=0,flags=0,color=0;
  std::vector<u8> bytes;std::vector<Sample> samples;std::vector<i32> rect;
 };
 bool enabled=false,recording=false,ready=false,camera=false;
 Vec2 offset{};
 uintptr_t observed=0;bool negative_control=false;
 std::array<float,5> reference(uintptr_t);
 u32 sampled=0;float last_alpha=1,last_x=0;
 void begin();
 void finish(){current.resize(written);recording=false;ready=enabled&&written>0;}
 void reset(){recording=ready=false;current.clear();previous.clear();pending.clear();occurrences.clear();endpoints.clear();written=0;}
 void range(const AnmVm&,u32 first,u32 count,u32 instance=0);
 void draw(const touhou::sdl::State&,touhou::graphics::Topology,u32,const void*,u32,bool);
 void clear(const touhou::sdl::State&,u32 flags,u32 color,const i32* rect);
 bool present(touhou::sdl::Renderer&,float alpha,bool frozen);
private:
 std::vector<Command> current,previous;u32 written=0;
 std::vector<std::pair<Key,std::pair<u32,u32>>> endpoints;
 Command& next();
 std::vector<Sample> pending;
 std::map<std::tuple<uintptr_t,u64,u32>,u32> occurrences;
 std::vector<u8> scratch;
};
}
