#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/AnmManager.hpp"
#include <fstream>
#include <iterator>
using namespace th13;
namespace {
std::vector<u8> asset(const char* name){std::ifstream f(std::string("artifacts/reference/raw/")+name,std::ios::binary);return {std::istreambuf_iterator<char>(f),{}};}
}
TEST(real_loading_prayer_uses_ascii_script17_at_original_screen_position) {
 GameRandom random;AnmEnvironment env;AnmManager manager(random,env);
 auto bytes=asset("ascii.anm");CHECK(!bytes.empty());CHECK(manager.load(2,bytes.data(),u32(bytes.size())));
 const u32 handle=manager.create(2,17,-1,0,{480,392,0});CHECK(handle);
 auto* vm=manager.registry.find(handle);CHECK(vm);if(!vm)return;
 CHECK_EQ(vm->source_script,17);CHECK_EQ(vm->visual.translation.x,480.f);CHECK_EQ(vm->visual.translation.y,392.f);
 CHECK_EQ(vm->visual.render_flags & 0x200u,0x200u);
 const auto before=random.index();
 CHECK(manager.update());
 // Retail 46f990 first ticks ascii script16 while spawning it from 17:
 // 12 signed/unit variable reads, each advancing the visual index by two.
 CHECK_EQ(random.index()-before,24u);
 for(int i=1;i<10;++i)CHECK(manager.update());
 const auto children=manager.registry.children(*vm);
 CHECK(!children.empty());
 bool visible=false;for(auto* child:children)visible|=child->sprite && (child->visual.flags&3u)==3u;
 CHECK(visible);
}
int main(){return th13test::run_all("loading prayer");}
