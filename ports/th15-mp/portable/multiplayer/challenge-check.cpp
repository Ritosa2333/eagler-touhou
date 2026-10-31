#include "../rollback/battle-digest.hpp"
#include "native-hud.hpp"
#include "../../web/cpp/game/MultiplayerHud.hpp"
static void miss(World& w,const std::array<MultiplayerInput,3>& held){
    const auto drops=w.battle->items->requests;
    for(unsigned seat=0;seat<w.battle->mp_options.seat_count;++seat)require(w.battle->pilots[seat]->player->life.hit(),"native hit");
    for(unsigned frame=0;frame<108;++frame)w.step(held);
    require(w.battle->items->requests-drops==7*int(w.battle->mp_options.seat_count),"native death drops seven Power items per seat");
}
int main(int argc,char** argv){
    require(argc==2,"Pass TH15_TEST_ARCHIVE");RetailAssets files(argv[1]);
    for(unsigned count:{2u,3u})for(auto selection:{std::array<int,3>{0,1,2},std::array<int,3>{3,3,0}}){
        World w(files,count,count-1,selection,true);auto& b=*w.battle;std::array<MultiplayerInput,3> input{};
        for(unsigned seat=0;seat<count;++seat){auto& p=*b.pilots[seat];require(p.session.challenge&&p.session.bombs==0,"challenge starts without Bombs");
            p.session.extra_lives=0;p.session.power=400;const auto nodes=w.animations.registry.count();
            require(!p.allow_bomb()&&!p.bomb->allowed()&&!p.bomb->begin(),"challenge blocks all native Bomb entry points");
            if(p.character==3)require(!static_cast<BombReisen&>(*p.bomb).practice_shields(3),"challenge rejects direct Reisen shield setup");
            require(!p.session.bomb_state&&w.animations.registry.count()==nodes,"blocked Bomb has no animation or shield");input[seat].held=2;
        }
        for(unsigned death=1;death<=12;++death){miss(w,input);for(unsigned seat=0;seat<count;++seat){const auto& p=*b.pilots[seat];require(p.session.challenge_misses==death&&p.session.extra_lives==0&&!p.ghost&&p.player->life.state==1,"zero-stock challenge Miss respawns and counts once past nine");}require(!w.output.game_overs,"simultaneous challenge deaths never wipe");}
        BattleFrameState saved;require(saved.capture(b),saved.error);const auto before=digest(w);miss(w,input);require(saved.restore(b),saved.error);require(digest(w)==before,"rollback restores challenge stocks and cumulative Miss");
        for(unsigned seat=0;seat<count;++seat){auto& p=*b.pilots[seat];ItemState item;item.kind=5;require(p.collection->award(item),p.collection->error);require(p.session.extra_lives==1&&p.session.challenge_misses==12,"hidden life award does not alter Miss");
            for(int type:{6,7}){item.kind=type;require(p.collection->award(item),p.collection->error);}require(!p.session.bombs&&!p.session.bomb_pieces,"Bomb rewards stay disabled");require(p.reset_stage()&&p.player->reset_for_stage(),"native next-stage player reset");require(p.session.challenge_misses==12,"stage reset retains cumulative Miss");p.session.challenge_misses+=seat;}
        GameHud hud(w.animations,w.progress,b.session,b.score,w.assets.front,w.assets.ascii,w.assets.logo);require(hud.initialize({}),hud.error);MultiplayerHud multi(b,hud);
        HudGraphics graphics;AnmRenderer renderer(graphics);HudCapture text;require(multi.prepare(renderer)&&multi.queue_text(text),multi.error);unsigned rows=0;
        for(const auto& t:text.texts)if(t.position.x==520&&t.style.font==3&&t.text==std::to_string(12+unsigned((t.position.y-104)/(count==3?78:120))))++rows;
        require(rows==count,"every life row renders its own numeric Miss");
        unsigned hidden_stars=0;for(auto* vm:renderer.multiplayer_hidden_hud)if(vm&&vm->source_script>=30&&vm->source_script<38)++hidden_stars;
        require(hidden_stars>=8*(count+1),"challenge hides native and multiplayer life stars");
        for(unsigned seat=0;seat<count;++seat){auto& p=*b.pilots[seat];p.player->motion.position={0,400,0};p.keys={};}
        auto& giver=*b.pilots[0];giver.session.extra_lives=2;
        for(unsigned tap=0;tap<5;++tap){giver.keys.held=giver.keys.pressed=8;require(b.mp_update_rules(),b.error);giver.keys={};require(b.mp_update_rules(),b.error);}
        unsigned gifts=0;for(unsigned id=1;id<=ItemManager::pool_size;++id)if(b.mp_gift_targets[id-1]==1){auto* item=b.items->find(id);require(item&&item->state.kind==5,"life transfer creates addressed native life item");require(b.pilots[1]->collection->award(item->state),"collect native life gift");++gifts;}
        require(gifts==1&&giver.session.extra_lives==1&&b.pilots[1]->session.extra_lives==2,"challenge gift uses hidden stock");
        for(unsigned seat=0;seat<count;++seat)require(b.pilots[seat]->session.challenge_misses==12+seat,"life donation does not change any Miss counter");
        b.pilots[0]->session.challenge_misses=0xffffffffu;require(b.pilots[0]->player->life.commit_death(),"saturating native death");require(b.pilots[0]->session.challenge_misses==0xffffffffu,"Miss saturates without wrapping");
    }
    World ordinary(files,2,0,{0,1,0});auto& p=*ordinary.battle->pilots[0];p.session.extra_lives=0;require(p.player->life.hit(),"ordinary native hit");std::array<MultiplayerInput,3> input{};for(unsigned frame=0;frame<55;++frame)ordinary.step(input);
    require(p.ghost&&p.session.extra_lives==-1&&p.session.challenge_misses==0,"ordinary final death still becomes a ghost");
    World fresh(files,2,0,{0,1,0},true);require(fresh.battle->pilots[0]->session.challenge_misses==0,"new run clears Miss");
    std::puts("PASS TH15 challenge: all loadouts, native Bomb/shield gates, >9 zero-stock Misses, respawn/drop lifecycle, HUD, stock awards, stage reset, rollback, new run and ordinary control");
}
