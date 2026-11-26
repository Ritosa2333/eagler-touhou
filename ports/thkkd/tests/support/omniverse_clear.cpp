#include "GamePlatformServices.hpp"
#include <iostream>
#include <stdexcept>

using namespace kaikeidou;
struct TestPlatform final : GamePlatformServices, ApplicationStartupServices {
    explicit TestPlatform(GameState& state):GamePlatformServices(state,24,true){}
    MenuScenePlatformResult invoke(const MenuSceneRequest& request,MenuReplayFormatMemory* =nullptr)override{
        if(request.api==MenuSceneApi::color)return {0xffffff,0,0,{},{}};
        return {1,0,0,{},{}};
    }
    ApplicationStartupPlatformResult invoke(const ApplicationStartupRequest&)override{return {};}
    bool open(std::string_view,bool)override{return false;}
    std::size_t read(std::uint8_t*,std::size_t)override{return 0;}
    void write(const std::uint8_t*,std::size_t)override{}
    void close()override{}
    void present_gameplay()override{}
};
int main(){try{
    unsigned cases=0;
    for(unsigned character=0;character<3;++character)for(unsigned part=0;part<8;++part)for(unsigned playback=0;playback<2;++playback){
        GameState state;state.prepare_frame(true);TestPlatform platform(state);
        CombinedGameHost game(state.combined_refs(),{platform,platform,platform.pause_platform(),platform,platform,platform},true,24,platform.math);
        platform.memory=&game.memory();state.initialize(game,true);
        auto& memory=game.memory();
        for(auto pair:std::initializer_list<std::pair<unsigned,unsigned>>{{0xa8ce58,100},{0xb3e2ec,7},{0xa7e610,5},{0xa74c0c,character},{0xa77390,part},{0xa733c0,2},{0xb3e2e8,511},{0xb4310c,playback}})memory.word(pair.first,pair.second);
        auto& context=game.gameplay_context();
        OmniverseTimelineState omniverse{context.timeline,state.enemies,state.projectiles,state.effects,state.player,state.options,state.shots,state.items,state.patterns,state.boss_phase,state.boss_allocation,state.protection,state.boss_stage,state.stage_patterns,state.boss_presentation,state.lifecycle,state.runtime,state.shake};
        update_omniverse_timeline(omniverse,state.random,RandomPrecision::bits24,platform.math,context.omni_services,context.fire_input,state.savedata.bindings,state.fire);
        memory.reconcile();const auto expected=playback?10u:16u;
        if(memory.word(0xa8ce58)!=expected||state.runtime.outer_scene!=expected||state.menu.outer_scene!=expected)throw std::runtime_error("Omniverse return scene mismatch");
        if(!playback&&memory.word(0xb41215)!=3)throw std::runtime_error("Replay save prompt missing");
        ++cases;
    }
    std::cout<<"{\"result\":\"passed\",\"cases\":"<<cases<<",\"scope\":\"all Omniverse choices and characters, live clear and Replay return\"}\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
