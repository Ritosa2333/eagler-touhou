#include "../th09_web/cpp/game/Ending.hpp"
#include <cassert>
#include <cstdio>
using namespace th09;
extern "C" void SDL_Log(const char*,...){}
namespace th09::sdl {bool read_file(const char*,std::vector<u8>&){return false;}}
struct Reader final:ResourceReader{
    std::vector<u8> script;
    bool read(const char* name,std::vector<u8>& out)override{
        if(std::strcmp(name,"text.anm")){out=script;return true;}
        out.assign(320,0);auto word=[&](u32 p,u32 value){std::memcpy(out.data()+p,&value,4);};
        word(4,29);word(12,1);word(16,1);word(28,296);word(40,3);out[296]='@';
        for(u32 i=0;i<29;++i){word(64+i*8,i);word(68+i*8,312);}out[312]=out[313]=255;return true;
    }
};
struct Textures final:ResourceTextures{TextureAllocation create(const AnmTextureSource&,const u8*,u32)override{return {1,1,1};}void destroy(u32)override{}};
struct Output final:EndingServices{
    bool ending_picture(const char*)override{return true;}void ending_music(i32)override{}void ending_music_fade(i32)override{}
    void ending_text(AnmVm&,const char*,u32)override{}void ending_background(i32,i32)override{}
    void ending_sprite(AnmVm&)override{}void ending_cover(u32)override{}
};
int main(){
    Reader reader;Textures textures;Rng rng;AnmExecutor animations(rng);GameResources resources(reader,textures,animations);Output output;
    const char line[]{"@w600\0" "120\0\n@z\n"},page[]{"@r600\0" "120\0\n@z\n"};
    for(const auto* script:{line,page}){
        auto size=script==line?sizeof(line)-1:sizeof(page)-1;reader.script.assign(script,script+size);Ending ending(resources,output);
        assert(ending.initialize(0));assert(ending.update({}));InputFrame z{};z.held=z.pressed=1;ending.update(z);assert(ending.error.empty());
        assert(ending.state.line_wait.current==0&&ending.state.page_wait.current==0&&ending.state.line_lock==0&&ending.state.page_lock==0);
        assert(ending.initialize(0));assert(ending.update(z));ending.update({});assert(ending.finished&&ending.error.empty());
        assert(ending.initialize(0));InputFrame skip{};skip.held=256;ending.update(skip);assert(ending.finished&&ending.error.empty());
    }
    std::puts("TH09 Ending / minimum wait Z / wait-install Z / Ctrl: PASS");
}
