#include "../ecl-fixture.hpp"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace ecl_fixture;

extern "C" {
void th128_world_reset();
int th128_world_load(const char*,std::uint32_t,const void*,std::uint32_t);
int th128_world_start(const char*,std::uint32_t,int,unsigned);
int th128_world_tick();
const char* th128_world_error();
const char* th128_world_state();
const char* th128_world_bullet_draws();
const char* th128_world_bullet_draw_error();
int th128_world_load_stage_intro(int,const char*,std::uint32_t,const void*,std::uint32_t);
int th128_world_track_caption(int,int);
const char* th128_world_stage_intro_state();
const char* th128_world_stage_intro_draw();
const char* th128_world_stage_intro_draw_error();
}
static std::vector<std::uint8_t> read(const std::string& path) {
    std::ifstream input(path,std::ios::binary);
    if(!input)throw std::runtime_error("missing "+path);
    return {std::istreambuf_iterator<char>(input),{}};
}
static void requireStage(bool condition,const std::string& message) {
    if(!condition)throw std::runtime_error(message);
}
#define require requireStage
static bool has(const std::string& text,const std::string& needle) {
    return text.find(needle)!=std::string::npos;
}
static void tick(int count) {
    for(int index=0;index<count;++index)require(th128_world_tick(),th128_world_error());
    require(std::string(th128_world_stage_intro_draw_error()).empty(),"stage intro draw error");
}
int main(int argc,char** argv) {
    try {
        require(argc==2,"usage: stage-intro-world logo_a1_1.anm");
        const auto logo=read(argv[1]);
        const auto ecl=program({{"main",instruction(1)}});
        const std::string stage="st_a1_1.ecl";
        th128_world_reset();
        require(th128_world_load(stage.c_str(),unsigned(stage.size()),ecl.data(),unsigned(ecl.size())),th128_world_error());
        require(th128_world_start(stage.c_str(),unsigned(stage.size()),1,1),th128_world_error());
        require(th128_world_load_stage_intro(1,"logo_a1_1.anm",13,logo.data(),unsigned(logo.size())),th128_world_error());
        require(has(th128_world_stage_intro_state(),"\"loaded\":true"),th128_world_stage_intro_state());
        tick(20);
        auto status=std::string(th128_world_stage_intro_state());
        require(has(status,"\"frame\":20")&&has(status,"\"introActive\":true"),status);
        require(has(std::string(th128_world_bullet_draws()),"\"id\":50000002")&&
            has(std::string(th128_world_bullet_draws()),"\"script\":1"),
            std::string("title draw missing at frame20 draws=")+th128_world_bullet_draws()+" state="+th128_world_stage_intro_state());
        tick(30);
        require(has(std::string(th128_world_bullet_draws()),"\"script\":1"),"title draw missing at frame50");
        tick(90);
        require(has(std::string(th128_world_bullet_draws()),"\"script\":2"),"hint draw missing at frame140");
        tick(199);
        require(has(std::string(th128_world_bullet_draws()),"\"script\":2"),"hint draw missing at frame339");
        tick(1);
        require(!has(std::string(th128_world_bullet_draws()),"\"script\":2"),"hint must retire at frame340");
        tick(200);
        require(std::string(th128_world_bullet_draws())=="[]","title must retire at frame540 draws="+std::string(th128_world_bullet_draws()));
        require(th128_world_track_caption(0,0),th128_world_error());
        status=th128_world_stage_intro_state();
        require(has(status,"\"trackFile\":0")&&has(status,"\"role\":0")&&has(status,"\"script\":3"),status);
        tick(9460);
        require(th128_world_track_caption(2,1),th128_world_error());
        status=th128_world_stage_intro_state();
        require(has(status,"\"trackFile\":2")&&has(status,"\"role\":1")&&has(status,"\"script\":4"),status);
        tick(1);
        require(has(std::string(th128_world_bullet_draws()),"\"script\":4"),
            std::string("boss caption draw missing draws=")+th128_world_bullet_draws()+" state="+th128_world_stage_intro_state());
        tick(219);
        require(!has(std::string(th128_world_stage_intro_state()),"\"captionActive\":true"),"boss caption must retire");
        th128_world_reset();
        require(has(th128_world_stage_intro_state(),"\"loaded\":false"),"reset must clear stage intro bank");
        require(th128_world_load(stage.c_str(),unsigned(stage.size()),ecl.data(),unsigned(ecl.size())),th128_world_error());
        require(th128_world_start(stage.c_str(),unsigned(stage.size()),1,1),th128_world_error());
        require(!th128_world_track_caption(0,0),"caption before load must fail");
        require(th128_world_load_stage_intro(1,"logo_a1_1.anm",13,logo.data(),unsigned(logo.size())),th128_world_error());
        tick(20);
        require(has(std::string(th128_world_stage_intro_state()),"\"frame\":20"),"restart intro frame must restart at 0");
        require(!th128_world_track_caption(0,2),"illegal caption role must be rejected");
        th128_world_reset();
        const auto fault=program({{"main",instruction(64000)}});
        require(th128_world_load(stage.c_str(),unsigned(stage.size()),fault.data(),unsigned(fault.size())),th128_world_error());
        require(th128_world_start(stage.c_str(),unsigned(stage.size()),1,1),th128_world_error());
        require(th128_world_load_stage_intro(1,"logo_a1_1.anm",13,logo.data(),unsigned(logo.size())),th128_world_error());
        require(!th128_world_tick(),"faulting world tick must fail");
        require(has(std::string(th128_world_stage_intro_state()),"\"frame\":0"),
            "failed world tick must not advance the intro host");
        std::puts("PASS actual WorldExports stage intro tick/draw/status/caption/reset/restart");
    }catch(const std::exception& error){
        std::fprintf(stderr,"%s\n",error.what());
        return 1;
    }
}
