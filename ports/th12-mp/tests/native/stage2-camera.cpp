#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../../src/stage_logic.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

int main(int argc, char** argv) {
  assert(argc == 2);
  std::ifstream input(argv[1]);
  std::string json((std::istreambuf_iterator<char>(input)), {});
  assert(json.find("99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417") != std::string::npos);
  const std::regex row(R"rx("frame":\s*(\d+),\s*"offsetX":\s*([-+.\deE]+),\s*"upX":\s*([-+.\deE]+))rx");
  th12::stage_logic::Program program;
  program.commands.push_back({740, 12, 0, {1}});
  th12::stage_logic::State state;
  state.reset(program);
  th12::stage_logic::World world;
  int frame = -1;
  int checked = 0;
  for (std::sregex_iterator at(json.begin(), json.end(), row), end; at != end; ++at) {
    const int target = std::stoi((*at)[1]);
    while (frame < target) {
      assert(state.tick(world));
      ++frame;
    }
    assert(std::abs(state.camera.offset[0] - std::stof((*at)[2])) < .00002f);
    assert(std::abs(state.camera.up[0] - std::stof((*at)[3])) < .000001f);
    ++checked;
  }
  assert(checked == 16);
  std::cout << "Stage 2 camera matches " << checked << " native samples\n";
}
