#include "test_services.hpp"
#include <iostream>
#include <stdexcept>
void require(bool condition,const char* message) {if(!condition) throw std::runtime_error(message);}
int main() {
    try {
        {
            RecordedServices env;
            auto* game=gp::create(env,-123);
            env.loading_worker.thread.join();
            require(game==gp::controller && game->restart()==-123 && game->game_flags==4 && env.latch==0,"Create must publish real controller/restart flags before worker");
            require(env.load_calls==1 && env.ordering_valid,"Real jthread did not call required loader");
            require(env.events==std::vector<int>({1,300,301}),"Evict/disable/worker order");
            gp::destroy(env,game);require(!gp::controller && env.latch==0 && env.ordering_valid,"Destroy must clear controller and input latch");
        }
        const auto seen=[](const RecordedServices& env,int code) {return std::find(env.events.begin(),env.events.end(),code)!=env.events.end();};
        unsigned cases=0;
        for(int scene:{0,4,10,11,14,15,16,22}) for(unsigned flags:{0u,1u,4u,16u,32u,5u,17u,0x70u}) for(int mode:{0,2}) {
            RecordedServices env;env.scene_value=scene;env.session_bits=flags;env.mode_value=mode;env.change_selection=true;
            auto* game=new gp::GameController(env);gp::controller=game;
            game->game_flags=0xffffffff;
            gp::destroy(env,game);
            const auto expected_flags=flags|((scene==10||scene==11||scene==14)?1u:0u)|(scene==14?16u:0u);
            require(env.session_bits==expected_flags,"Scene-specific session flag updates");
            require(env.color==((expected_flags&1)?0u:0xff000000u),"Final clear color follows updated session bit0");
            require(seen(env,5)==(scene==4||scene==10||scene==11||scene==14||scene==16),"Screen transition branch");
            require(seen(env,7)==(scene==14),"Continue increment only in changed stage branch");
            const bool music_stop=!(mode==2&&(expected_flags&1)) && !(expected_flags&4) && !(expected_flags&32);
            require(seen(env,9)==music_stop && seen(env,10)==music_stop,"Music cleanup predicates");
            require(seen(env,100)==!(flags&4) && seen(env,106)==bool(flags&4),"Full teardown vs retained-session teardown");
            require(env.events.size()>5 && env.events[0]==2 && env.events[1]==3 && env.events[2]==4,"Common teardown prefix order");
            const auto last=env.events.size();require(env.events[last-3]==11 && env.events[last-2]==12 && env.events[last-1]==208,"Stop effects/color/final owner order");
            require(env.ordering_valid && !gp::controller,"Controller publication during teardown");++cases;
        }
        {
            RecordedServices env;env.scene_value=10;env.transition_changes_scene=true;
            gp::controller=new gp::GameController(env);gp::destroy(env,gp::controller);
            require(env.session_bits==3,"Scene must be re-read after transition callback");
        }
        std::cout<<cases+2<<" GameController lifecycle cases passed with recorded subsystem boundaries and real jthread\n";return 0;
    } catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
}
