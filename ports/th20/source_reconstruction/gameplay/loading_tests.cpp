#include "loading.hpp"
#include "test_services.hpp"
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <fstream>
#include "gameplay_source_hashes.hpp"
namespace ps=gp::player_state;
struct LoadFixture final:gp::LoadingServices {
    th20::source::game_session::Session data;
    std::vector<gp::Profile> profiles=std::vector<gp::Profile>(18);
    RecordedServices lifecycle;
    gp::GameController game{lifecycle};
    int new_state=1,pending_sprite=0,pending_audio=0,pending_effects=0,fail_factory=-1;
    unsigned event_flags=0,query_count=0,created=0,commits=0,registers=0,tracks=0,clear_counter=0;
    bool display_called=false,display_success=false;
    std::vector<gp::Entity> factories;
    std::vector<unsigned> sleeps;
    std::vector<std::tuple<int,int>> surfaces;
    LoadFixture() {
        gp::controller=&game;data.contexts[0].current_player=&data.player_table.players[0];gp::select_stage(data.player_table,1);
        for(auto& profile:profiles)ps::write(profile,0x18,std::uint64_t{123456});
        game.game_flags=0xfffffffbu; // bit2 added by loader; other observed flag clearing must be explicit
    }
    th20::source::game_session::Session& session() override{return data;}
    void begin_frame_interval() override{}
    bool sprite_task_pending() override{return pending_sprite-->0;}
    std::uint32_t graphics_events() override{return event_flags;}
    std::uint32_t new_game_state() override{return new_state;}
    void sleep(std::uint32_t duration) override{sleeps.push_back(duration);}
    void interrupt_surface(unsigned index,bool execute) override{surfaces.emplace_back(index,execute);}
    std::int32_t selected_profile(int slot,int character) override {
        if(slot<0||slot>3||character<0||character>1)throw std::logic_error("Bad selected-profile arguments");++query_count;return slot;
    }
    gp::Profile& profile(int character,int index) override{return profiles.at(character*9+index);}
    void configure_overlay(int character) override{if(character!=0)throw std::logic_error("Bad character");}
    ps::BombObserver* bomb_observer() override{return nullptr;}
    std::int32_t replay_selection() override{return -1;}
    const char* replay_path() override{return "replay/test.rpy";}
    void register_callbacks(gp::GameController&) override{++registers;}
    const th20::source::platform::Configuration& configuration() override{return game.configuration;}
    bool create(gp::Entity entity,int index,const char* path) override{
        if(entity==gp::Entity::fn_0050a930) {if(index!=game.restart_mode||std::string(path)!="replay/test.rpy")throw std::logic_error("Bad replay factory arguments");}
        else if(index!=0)throw std::logic_error("Nonzero player index");
        if(entity==gp::Entity::fn_00477880&&std::string(path)!=gp::stage_background(*gp::selected_stage,data.player_table.players[0]))throw std::logic_error("Wrong background");
        if(entity==gp::Entity::fn_004aba70&&std::string(path)!=gp::selected_stage->ecl_file)throw std::logic_error("Wrong ECL path");
        factories.push_back(entity);return static_cast<int>(created++)!=fail_factory;
    }
    void create_auxiliary_owner() override{}
    void reset_replay_owner() override{}
    void reset_hud_owner() override{}
    void reset_existing_stage() override{}
    void commit_progress() override{++commits;}
    void queue_track(int index,const char* path) override{if(std::string(path)!=gp::selected_stage->tracks[index])throw std::logic_error("Wrong music");++tracks;}
    void reset_frame_statistics() override{}
    bool audio_commands_pending() override{return pending_audio-->0;}
    bool effects_ready() override{return pending_effects--<=0;}
    void finish_entity_initialization() override{}
    void finish_loading_display(bool success) override{display_called=true;display_success=success;}
    void clear_slowdown_counter() override{++clear_counter;}
    double read_clock() override{return 123.5;}
};
int main(int argc,char** argv) {
    unsigned cases=0;
    try {
        auto check=[&](bool condition){++cases;if(!condition)throw std::runtime_error("Loading assertion "+std::to_string(cases));};
        for(int mode=0;mode<3;++mode)for(unsigned flags=0;flags<64;++flags)for(int restart=0;restart<2;++restart)for(int fresh=0;fresh<2;++fresh)for(int stage=1;stage<=7;++stage) {
            LoadFixture host;host.data.mode=mode;host.data.flags=flags;host.game.restart_mode=restart;host.new_state=fresh;
            gp::select_stage(host.data.player_table,stage);host.pending_sprite=2;host.pending_audio=2;host.pending_effects=3;
            const auto result=gp::load_gameplay(host.game,host);
            check(result==0&&host.display_called&&host.display_success&&host.registers==1&&host.commits==1);
            check((host.data.flags&0x15u)==0&&host.game.field_f4==60&&!(host.game.game_flags&4u)&&host.clear_counter==1);
            check(host.query_count==(fresh?9u:8u)&&host.tracks==((flags&0x20u)?0u:2u));
            check(host.surfaces.size()==5&&host.surfaces.front()==std::tuple{0,1}&&host.surfaces.back()==std::tuple{3,0});
            check(host.sleeps==((fresh)?std::vector<unsigned>{1,1,16,16,1,1,1}:std::vector<unsigned>{1,1,60,16,16,1,1,1}));
            check(host.data.contexts[0].current_player==&host.data.player_table.players[0]&&gp::selected_stage->id==stage);
            if(fresh) {
                check(ps::read<int>(host.data.player_table.players[0],0x30)==(mode==2||(stage>=2&&stage!=7)?400:100));
                check(ps::read<int>(host.data.player_table.players[0],0xb8)==(mode==2?0:mode==1?7:2));
            }
            if(!restart)check(ps::read<double>(host.data,0x2b0)==123.5);
        }
        for(int failure=0;failure<12;++failure) {
            LoadFixture host;host.fail_factory=failure;host.game.game_flags=0;
            check(gp::load_gameplay(host.game,host)==-1&&host.created==static_cast<unsigned>(failure+1));
            check(host.display_called&&!host.display_success&&(host.game.game_flags&8)&&host.commits==0);
        }
        {LoadFixture host;host.pending_sprite=1;host.event_flags=0x20;
            check(gp::load_gameplay(host.game,host)==-1&&host.query_count==0&&host.created==0&&host.sleeps.empty());}
        if(argc>1) {
            std::ofstream report(argv[1]);if(!report)throw std::runtime_error("Cannot write report");
            report<<"{\"status\":\"passed\",\"assertions\":"<<cases<<",\"source_sha256\":{";bool first=true;
            for(auto file:gameplay_source_hashes){if(!first)report<<',';first=false;report<<'\"'<<file.path<<"\":\""<<file.sha<<'\"';}
            report<<"},\"scope\":\"Loading control-flow tests with recorded external owners. Not execution of the entire original loading graph, not game equivalence.\"}\n";
        }
        std::cout<<cases<<" loading control-flow assertions passed; external owners are recorded test boundaries\n";return 0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
