#include "../th08_web/cpp/game/Ending.hpp"
#include "../th08_web/cpp/game/TitleMenus.hpp"
#include "../th08_web/cpp/game/Dialogue.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../th08_web/cpp/multiplayer/TextureJournal.hpp"
// No graphics journal is bound in this script/input fixture.
namespace th08::multiplayer {bool TextureJournal::Touch(u32,bool){return true;}bool TextureJournal::MayMutate(){return true;}}
#endif
#include <cassert>
#include <cstdio>
using namespace th08;
// Resource/log ports are unused: this fixture supplies synthetic scripts.
extern "C" void SDL_Log(const char*,...){}
namespace th08 { bool sdl_read_file(const char*,std::vector<u8>&){return false;} }
struct Backend final:SpriteBackend {
    PipelineState state;
    PipelineState& pipeline() override{return state;}
    void bind_texture(u32) override{} void destination_blend(BlendParameter) override{}
    void write_depth(bool) override{} void triangles(const SpriteVertex*,u32) override{}
    void transform(MatrixParameter,const Matrix4&) override{} void set_viewport(const Viewport&) override{}
    void texture_factor(u32) override{} void vertex_format(VertexFormat) override{}
    void draw(Primitive,VertexFormat,const void*,u32) override{} void clear_target(u32,u32,float,u32) override{}
};
struct Text final:TextWriter{bool draw(AnmVm&,TextAlignment,u32,u32,const char*)override{return true;}};
struct Actions final:EndingActions {
    std::vector<u8> bytes;
    std::vector<u8> read_ending(const char*)override{return bytes;}
    bool load_background(const char*)override{return true;}
    void draw_background(i32,i32)override{} void present()override{} void release_background()override{}
    void music(const char*)override{} void fade_music(float)override{} void finished()override{}
};
struct DialogueOutput final:DialogueActions {
    void copy_enemy_name(i32)override{} void clear_bullets()override{} void despawn_enemies()override{}
    void collect_items()override{} void sound(i32)override{} bool play_music(i32,i32)override{return true;}
    void play_audio(const char*,i32)override{} void stop_audio()override{} void fade_music(float)override{}
    void fade_screen(i32,u32,i32)override{} void capture_arcade(const AnmLoadedSprite&)override{}
};
int main(){
    arithmetic_mode(Precision::Single,Rounding::NearestEven);
    TextureStore textures;AnmLibrary library(textures);Rng rng;AnmExecutor animations(rng);
    Backend backend;AnmRenderer renderer(backend);Text text;Actions actions;
    Ending ending(library,animations,renderer,text,actions);
    const char line[]{"@w600\0" "120\0\n@z\n"};
    const char page[]{"@r600\0" "120\0\n@z\n"};
    const auto load=[&](const char* p,u32 n){ending.reset();actions.bytes.assign(p,p+n);assert(ending.load("fixture"));};
    for(const auto* script:{line,page}){
        const auto n=script==line?sizeof(line)-1:sizeof(page)-1;
        load(script,n);assert(ending.update(0,0)==JobResult::Continue);
        assert(ending.state.wait.current==600||ending.state.reset_wait.current==600);
        ending.update(1,0);
        assert(ending.state.wait.current==0&&ending.state.reset_wait.current==0);
        assert(ending.state.minimum_wait==0&&ending.state.minimum_reset==0);
        if(script==page)assert(ending.state.vms[0].pendingInterrupt==2);
        load(script,n);ending.update(1,0); // Z on the tick that installs a wait.
        assert(ending.update(0,1)==JobResult::Remove);
        load(script,n);assert(ending.state.seen==0);
        assert(ending.update(256,0)==JobResult::Remove); // First-view Ctrl.
    }
    load(line,sizeof(line)-1);ending.update(0,0);ending.update(0,1);
    assert(ending.state.wait.current==599); // Releasing Z does not advance.
    ending.update(4097,4096);assert(ending.state.wait.current==0); // Z while Enter held.
    TitleContext title{};const auto before=title;
    assert(title.IsExtraUnlocked()&&title.IsSpellPracticeUnlocked()&&title.IsExtraUnlockedWithAllTeams());
    for(int character=0;character<12;++character)assert(title.IsExtraUnlockedForCharacter(character)&&title.IsSpellPracticeUnlockedForCharacter(character));
    for(int card=0;card<222;++card)assert(title.IsSpellCardAvailable(card));
    assert(!title.IsSpellCardAvailable(-1)&&!title.IsSpellCardAvailable(222));
    assert(!title.IsExtraUnlockedForCharacter(-1)&&!title.IsExtraUnlockedForCharacter(12));
    assert(std::memcmp(&before,&title,sizeof(title))==0); // Availability writes no records.
    GuiState gui{};GuiImplState display{};DialogueContext context{};GameGlobals globals{};
    GameConfiguration config{};HighScore high{};GameValues values(globals,config,config,high,rng);
    DialogueOutput output;Dialogue dialogue(gui,display,context,globals,values,animations,text,renderer,output);
    std::vector<u8> program(4+11*4+4);u32 count=11,offset=4+11*4;
    std::memcpy(program.data(),&count,4);for(u32 i=0;i<count;++i)std::memcpy(program.data()+4+i*4,&offset,4);
    assert(dialogue.load(program.data(),program.size()));context.stage=5;
    for(int continues:{0,1}){globals.retries=continues;context.flags=0;assert(dialogue.read(10));assert(display.dialogue.message==3);}
    globals.retries=0;context.flags=8;context.replay_clear=0;assert(dialogue.read(10));assert(display.dialogue.message==1);
    context.replay_clear=1;assert(dialogue.read(10));assert(display.dialogue.message==2);
    context.replay_clear=2;assert(dialogue.read(10));assert(display.dialogue.message==3);
    std::puts("TH08 Ending controls / fresh-save availability / Final A-B / recorded replay route: PASS");
}
