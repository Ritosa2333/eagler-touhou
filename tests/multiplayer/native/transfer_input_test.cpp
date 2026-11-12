// R-I9/R-G production input coverage: every gesture enters GameInput::update
// and StageBattle::step, never the transfer helper or direct HUD transactions.
#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "multiplayer/WorldState.hpp"
#include <eagler/gameplay/CooperativeRules.hpp>
#include <fstream>
using namespace th13;
using namespace th13test;
namespace {
struct Host final : StageBattleServices {
    bool life_hud(i32,i32) override { return true; }
    bool bomb_hud(i32,i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};
struct Provider final : EclResourceProvider {
    std::vector<u8> stage;
    std::vector<std::string>* animations = nullptr;
    Provider() { std::vector<u8> main; emit(main,0,23,0,0xff,0,{60000});
        emit(main,0,10,0,0xff,0,{}); stage=build_scpt({{"main",main}}, {"enemy.anm","st01enm.anm"}); }
    bool read(const std::string& name,std::vector<u8>& out) override {
        if(name=="transfer.ecl"){out=stage;return true;}
        std::ifstream file("/work/artifacts/reference/raw/"+name,std::ios::binary);
        if(!file)return false;out.assign(std::istreambuf_iterator<char>(file),{});return true;
    }
    bool animation(u32,const std::string& name) override{
        animations->push_back(name);return true;
    }
};
struct Fixture {
    Provider provider; Host host; SessionState progress; HudPlayerState hud;
    HudScoreState score; StageBattle battle;
    bool init(i32 players=2) {
        provider.animations=&battle.sim.anim_list;
        if(!battle.load(provider,"transfer.ecl",2,host,progress,hud,score) ||
           !battle.enable_seats(provider,players,{0,1,2})){
            std::fprintf(stderr,"transfer fixture load: %s\n",battle.error().c_str());return false;
        }
        progress.ending_points=0;
        for(i32 seat=0;seat<players;++seat) {
            auto& state=battle.seats[size_t(seat)];
            state.initialized=true;state.phase=StagePlayerState::Phase::Active;
            state.phase_frame=0;state.invulnerable=10000;place(seat,float(seat*20),400);
            battle.seat_huds[size_t(seat)].power=200;
        }
        return true;
    }
    void place(i32 seat,float x,float y) {
        auto& s=battle.seats[size_t(seat)];s.x=x;s.y=y;
        s.fixed_x=i32(x*128);s.fixed_y=i32(y*128);
    }
    void frame(u32 a=0,u32 b=0,u32 c=0) {
        const u32 held[3]={a,b,c};
        for(i32 seat=0;seat<battle.seat_count;++seat)battle.seat_inputs[size_t(seat)].update(held[seat]);
        CHECK(battle.step(battle.seat_inputs[0]));
    }
    void taps(u32 bit,i32 count=5) { for(i32 tap=0;tap<count;++tap){frame(bit);if(tap+1<count)frame();} }
    Item* gift() {for(auto& item:battle.item_manager.items())if(item.alive&&item.target_seat>=0)return &item;return nullptr;}
    void settle(){for(i32 frame=0;frame<20&&gift();++frame)this->frame();}
};
u32 taps(u8 state){return eagler::cooperative::TapCount(state);}
}
TEST(life_five_edges_targeted_no_broadcast) {
    Fixture f;CHECK(f.init(3));f.place(1,20,400);f.place(2,21,400);
    f.battle.defer_item_side=true;
    f.battle.seat_huds[0].extra_lives=3;f.battle.seat_huds[1].extra_lives=0;
    f.taps(8,4);CHECK(!f.gift());CHECK_EQ(f.battle.seat_huds[0].extra_lives,3);
    f.frame();f.frame(8);CHECK(f.gift());CHECK_EQ(f.gift()->target_seat,1);
    CHECK_EQ(f.battle.seat_huds[0].extra_lives,2);
    f.battle.defer_item_side=false;f.settle();CHECK(!f.gift());
    CHECK_EQ(f.battle.seat_huds[0].extra_lives,2);CHECK_EQ(f.battle.seat_huds[1].extra_lives,1);
    CHECK_EQ(f.battle.seat_huds[2].extra_lives,2);
    for(i32 i=0;i<40;++i)f.frame(8);CHECK_EQ(f.battle.seat_huds[0].extra_lives,2);
    CHECK_EQ(taps(f.battle.seats[0].life_transfer_state),0u);
}
TEST(life_hold_fire_timeout_distance_boundaries) {
    Fixture held;CHECK(held.init());for(i32 i=0;i<40;++i)held.frame(8);
    CHECK(!held.gift());CHECK_EQ(taps(held.battle.seats[0].life_transfer_state),0u);
    Fixture fire;CHECK(fire.init());fire.taps(8,3);CHECK_EQ(taps(fire.battle.seats[0].life_transfer_state),3u);
    fire.frame(1);CHECK_EQ(fire.battle.seats[0].life_transfer_state,0);fire.frame();fire.taps(8,2);
    CHECK_EQ(fire.battle.seat_huds[0].extra_lives,2);
    Fixture late;CHECK(late.init());late.taps(8,4);for(i32 i=0;i<25;++i)late.frame();late.frame(8);
    CHECK_EQ(taps(late.battle.seats[0].life_transfer_state),1u);CHECK_EQ(late.battle.seat_huds[0].extra_lives,2);
    Fixture edge;CHECK(edge.init());for(i32 tap=0;tap<5;++tap){edge.frame(8);if(tap<4)for(i32 i=0;i<23;++i)edge.frame();}
    edge.settle();CHECK_EQ(edge.battle.seat_huds[0].extra_lives,1);CHECK_EQ(edge.battle.seat_huds[1].extra_lives,3);
    Fixture away;CHECK(away.init());away.taps(8,3);away.place(1,20.01f,400);away.frame();
    CHECK_EQ(away.battle.seats[0].life_transfer_state,0);away.place(1,20,400);away.taps(8,2);
    CHECK_EQ(away.battle.seat_huds[0].extra_lives,2);
}
TEST(life_three_seat_priority_and_fixed_donor_order) {
    Fixture tie;CHECK(tie.init(3));tie.place(1,10,400);tie.place(2,-10,400);
    tie.battle.seat_huds[1].extra_lives=1;tie.battle.seat_huds[2].extra_lives=1;
    tie.battle.defer_item_side=true;tie.taps(8);CHECK(tie.gift());CHECK_EQ(tie.gift()->target_seat,1);
    Fixture less;CHECK(less.init(3));less.place(1,10,400);less.place(2,-10,400);
    less.battle.seat_huds[1].extra_lives=2;less.battle.seat_huds[2].extra_lives=0;
    less.battle.defer_item_side=true;less.taps(8);CHECK(less.gift());CHECK_EQ(less.gift()->target_seat,2);
    Fixture same;CHECK(same.init(3));same.place(1,10,400);same.place(2,-10,400);
    same.battle.seats[2].ghost=true;same.battle.seats[2].dead=true;same.battle.seat_huds[2].extra_lives=-1;
    for(i32 tap=0;tap<5;++tap){same.frame(8,8);if(tap<4)same.frame();}
    same.settle();
    CHECK(!same.battle.seats[2].ghost);CHECK_EQ(same.battle.seat_huds[0].extra_lives,1);
    CHECK_EQ(same.battle.seat_huds[1].extra_lives,1); // second donor gifts to now-live seat2
    CHECK_EQ(same.battle.seat_huds[2].extra_lives,1);
}
TEST(ghost_rescue_stock_power_bombs_protection_no_cancel) {
    for(i32 stock:{-1,0,3}) {
        Fixture f;CHECK(f.init(3));f.place(2,100,400);
        auto& ghost=f.battle.seats[1];auto& hud=f.battle.seat_huds[1];
        ghost.ghost=ghost.dead=true;hud.extra_lives=stock;hud.life_pieces=3;hud.life_piece_threshold_index=4;
        hud.power=37;hud.max_power=600;hud.bombs=7;hud.bomb_pieces=5;
        StageBullet bullet;bullet.x=150;bullet.y=100;bullet.speed=0;bullet.collision_ready=false;
        f.battle.sim.bullets.push_back(bullet);
        f.taps(8);CHECK(!ghost.ghost);CHECK(!ghost.dead);
        CHECK_EQ(hud.extra_lives,i32(std::max(0,stock)));CHECK_EQ(hud.life_pieces,3);
        CHECK_EQ(hud.life_piece_threshold_index,4);CHECK_EQ(hud.power,300);CHECK_EQ(hud.bombs,2);
        CHECK_EQ(hud.bomb_pieces,0);CHECK_EQ(ghost.invulnerable,240);
        CHECK_EQ(f.battle.sim.bullets.size(),1u);CHECK(f.battle.sim.bullets[0].alive);
        CHECK_EQ(f.battle.seat_huds[0].extra_lives,1);CHECK_EQ(f.battle.seat_huds[2].extra_lives,2);
    }
    Fixture no_stock;CHECK(no_stock.init());no_stock.battle.seats[1].ghost=true;
    no_stock.battle.seat_huds[0].extra_lives=0;no_stock.taps(8);CHECK(no_stock.battle.seats[1].ghost);
}
TEST(stage_clear_clamps_negative_stock_preserves_rewards) {
    Fixture f;CHECK(f.init(3));for(i32 seat=0;seat<3;++seat){auto& s=f.battle.seats[size_t(seat)];
        auto& h=f.battle.seat_huds[size_t(seat)];s.ghost=s.dead=true;h.extra_lives=seat-1;
        h.life_pieces=seat+1;h.power=0;s.life_transfer_state=s.power_transfer_state=88;}
    f.battle.revive_all_on_stage_clear();for(i32 seat=0;seat<3;++seat){const auto& s=f.battle.seats[size_t(seat)];
        const auto& h=f.battle.seat_huds[size_t(seat)];CHECK(!s.ghost);CHECK(!s.dead);
        CHECK_EQ(h.extra_lives,i32(std::max(0,seat-1)));CHECK_EQ(h.life_pieces,seat+1);
        CHECK_EQ(h.power,h.max_power/2);CHECK_EQ(h.bombs,2);CHECK_EQ(s.invulnerable,120);
        CHECK_EQ(s.life_transfer_state,0);CHECK_EQ(s.power_transfer_state,0);}
}
TEST(power_edges_threshold_priority_and_no_broadcast) {
    Fixture f;CHECK(f.init(3));f.place(1,20,400);f.place(2,-20,400);
    f.battle.seat_huds[0].power=100;f.battle.seat_huds[1].power=150;f.battle.seat_huds[2].power=100;
    f.battle.defer_item_side=true;f.taps(1);CHECK(f.gift());CHECK_EQ(f.gift()->target_seat,2);
    CHECK_EQ(f.battle.seat_huds[0].power,0);f.battle.defer_item_side=false;f.settle();
    CHECK_EQ(f.battle.seat_huds[0].power,0);CHECK_EQ(f.battle.seat_huds[1].power,150);
    CHECK_EQ(f.battle.seat_huds[2].power,200);
    Fixture tie;CHECK(tie.init(3));tie.place(1,10,400);tie.place(2,-10,400);
    tie.battle.defer_item_side=true;tie.taps(1);CHECK(tie.gift());CHECK_EQ(tie.gift()->target_seat,1);
    Fixture held;CHECK(held.init());for(i32 i=0;i<60;++i)held.frame(1);
    CHECK_EQ(held.battle.seat_huds[0].power,200);CHECK_EQ(held.battle.seat_huds[1].power,200);
    Fixture poor;CHECK(poor.init());poor.battle.seat_huds[0].power=99;poor.taps(1);
    CHECK_EQ(poor.battle.seats[0].power_transfer_state,0);CHECK_EQ(poor.battle.seat_huds[0].power,99);
}
TEST(targeted_gift_waits_for_ghost_and_pool_reuse_clears_target) {
    Fixture f;CHECK(f.init(3));f.place(2,150,400);f.battle.defer_item_side=true;f.taps(1);
    Item* gift=f.gift();CHECK(gift);const auto slot=gift-f.battle.item_manager.items().data();
    f.battle.seats[1].ghost=true;f.battle.seats[1].dead=true;f.battle.defer_item_side=false;
    for(i32 i=0;i<12;++i)f.frame();CHECK_EQ(gift->target_seat,1);CHECK(gift->alive);
    CHECK_EQ(f.battle.seat_huds[0].power,100);CHECK_EQ(f.battle.seat_huds[2].power,200);
    f.battle.seats[1].ghost=f.battle.seats[1].dead=false;f.settle();CHECK(!gift->alive);CHECK_EQ(gift->target_seat,-1);
    f.battle.item_manager.spawn_item(160,100,0,ItemPoint,0,0);
    const auto& reused=f.battle.item_manager.items()[size_t(slot)];CHECK(reused.alive);CHECK_EQ(reused.target_seat,-1);
    f.battle.defer_item_side=true;for(i32 i=0;i<600;++i)f.battle.item_manager.spawn_item(0,100,0,ItemPoint,0,0);
    const auto before=f.battle.seat_huds[0].extra_lives;f.taps(8);CHECK_EQ(f.battle.seat_huds[0].extra_lives,before);
}
TEST(power_timeout_range_full_and_ghost_reset) {
    Fixture timeout;CHECK(timeout.init());timeout.taps(1,4);
    for(i32 i=0;i<25;++i)timeout.frame();timeout.frame(1);
    CHECK_EQ(taps(timeout.battle.seats[0].power_transfer_state),1u);
    CHECK_EQ(timeout.battle.seat_huds[0].power,200);
    Fixture range;CHECK(range.init());range.taps(1,3);range.place(1,21,400);range.frame();
    CHECK_EQ(range.battle.seats[0].power_transfer_state,0);range.place(1,20,400);range.taps(1,2);
    CHECK_EQ(range.battle.seat_huds[0].power,200);
    Fixture full;CHECK(full.init());full.battle.seat_huds[1].power=full.battle.seat_huds[1].max_power;
    full.taps(1);CHECK_EQ(full.battle.seat_huds[0].power,200);
    CHECK_EQ(full.battle.seats[0].power_transfer_state,0);
    Fixture ghost;CHECK(ghost.init());ghost.battle.seats[1].ghost=ghost.battle.seats[1].dead=true;
    ghost.taps(1);CHECK_EQ(ghost.battle.seat_huds[0].power,200);
    CHECK_EQ(ghost.battle.seats[0].power_transfer_state,0);
}
TEST(rollback_pending_gesture_and_inflight_gift_matches_forward) {
    Fixture f;CHECK(f.init());GameRandom visual;visual.reseed(129);
    f.battle.defer_item_side=true;f.taps(1,4);
    multiplayer::WorldState checkpoint;CHECK(checkpoint.capture(f.battle,visual));const auto start=checkpoint.digest();
    f.frame();f.frame(1);CHECK(f.gift());CHECK_EQ(f.gift()->target_seat,1);
    f.battle.defer_item_side=false;f.settle();multiplayer::WorldState forward;CHECK(forward.capture(f.battle,visual));
    const auto expected=forward.digest();CHECK(checkpoint.restore(f.battle,visual));
    multiplayer::WorldState restored;CHECK(restored.capture(f.battle,visual));CHECK_EQ(restored.digest(),start);
    f.battle.defer_item_side=true;f.frame();f.frame(1);CHECK(f.gift());
    multiplayer::WorldState inflight;CHECK(inflight.capture(f.battle,visual));const auto target_digest=inflight.digest();
    f.battle.defer_item_side=false;f.settle();CHECK(inflight.restore(f.battle,visual));
    multiplayer::WorldState restored_gift;CHECK(restored_gift.capture(f.battle,visual));CHECK_EQ(restored_gift.digest(),target_digest);
    f.battle.defer_item_side=false;f.settle();multiplayer::WorldState replay;CHECK(replay.capture(f.battle,visual));CHECK_EQ(replay.digest(),expected);
}
// Keep standalone execution while sharing the m3 cumulative test registry.
int __attribute__((weak)) main(){return run_all("transfer.production_input");}
