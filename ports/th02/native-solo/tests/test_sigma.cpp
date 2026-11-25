#include "sigma_core.hpp"
#include "core_contract.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string_view>

using soew::SigmaCore;
using soew::EnemyBulletRequest;

#define CHECK(expr) do { if (!(expr)) { std::cerr << __LINE__ << ": " #expr "\n"; std::exit(1); } } while (false)

struct Harness {
    using Laser = std::array<int,5>;
    using Rect = std::array<int,4>;
    SigmaCore sigma;
    SigmaCore::Hooks hooks;
    soew::RandomRing random;
    std::vector<EnemyBulletRequest> bullets;
    std::vector<Laser> lasers;
    std::vector<unsigned> sounds,scores,turns;
    std::vector<Rect> damage_rects;
    unsigned ticks=0,random_calls=0,damage=0,hits=0,last_back=0;
    Harness(){
        soew::IRand seed(0x12345678);random.fill(seed);
        hooks.bullet=[&](auto r){bullets.push_back(r);};
        hooks.damage=[&](int x,int y,int w,int h){damage_rects.push_back({x,y,w,h});return damage;};
        hooks.score=[&](unsigned v){scores.push_back(v);};
        hooks.sound=[&](unsigned v){sounds.push_back(v);};
        hooks.turns_max=[&](unsigned v){turns.push_back(v);};
        hooks.randring2_next=[&]{++random_calls;return unsigned(random.next8());};
        hooks.player_hit=[&]{++hits;};
        hooks.laser=[&](int x,int y,int f,unsigned p,unsigned d){lasers.push_back({x,y,f,int(p),int(d)});};
        hooks.player_x=160;hooks.player_y=190;hooks.front_player_x=32;hooks.front_player_y=350;
    }
    void step(){step(ticks&1);}
    void step(unsigned back){
        last_back=back;sigma.begin_frame(back);sigma.update(back,ticks++,hooks);
    }
    void advance(unsigned n){while(n--)step();}
    // Phase seeding is only for unit-testing individual assembly edges. The full
    // phase progression tests below reach all phases through normal damage calls.
    void at(unsigned phase,unsigned subattack=0){sigma.init();sigma.phase=phase;sigma.subattack=subattack;}
    bool paint(SigmaCore::Paint::Kind kind,int size,unsigned color) const {
        return std::any_of(sigma.paints.begin(),sigma.paints.end(),[&](const auto &p){return p.kind==kind&&p.a==size&&p.color==color;});
    }
};

void intro_damage_and_cycle(){
    Harness h;
    h.hooks.front_player_x=168;h.hooks.front_player_y=49;
    h.advance(50);
    CHECK(h.sigma.phase==0&&h.sigma.phase_frame==50&&h.sigma.sigma_frame==50&&h.hits==0);
    CHECK((h.damage_rects.back()==Harness::Rect{196,80,56,32}));
    h.hooks.front_player_x=169;h.hooks.front_player_y=48;h.step();
    CHECK(h.sigma.phase==1&&h.sigma.phase_frame==0&&h.sigma.sigma_frame==51&&h.hits==0);
    h.hooks.front_player_y=49;h.damage=1799;h.step();h.damage=0;
    CHECK(h.sigma.phase_frame==1&&h.sigma.damage_taken==1799&&h.hits==1);
    CHECK(h.sigma.hit_flash&&h.sigma.draw_pattern==130&&h.sigma.pattern==128);
    h.hooks.front_player_x=248;h.step();CHECK(h.hits==1);
    h.advance(437);CHECK(h.sigma.phase_frame==439);
    h.damage=1;h.step();h.damage=0;
    CHECK(h.sigma.phase==1&&h.sigma.phase_frame==0&&h.sigma.subattack==1&&h.sigma.damage_taken==1800);
    h.advance(200);CHECK(h.sigma.phase==1&&h.sigma.phase_frame==200);
    h.damage=7;h.step();h.damage=0;
    CHECK(h.sigma.phase==2&&h.sigma.phase_frame==0&&h.sigma.subattack==0&&h.sigma.damage_taken==7);
    h.step();CHECK(h.sigma.phase==3&&h.sigma.phase_frame==0&&h.sigma.damage_taken==7);

    Harness no_shots;
    no_shots.advance(51+440+201+306);
    CHECK(no_shots.sigma.phase==1&&no_shots.sigma.subattack==0&&no_shots.sigma.cycled&&!no_shots.sigma.dying);
    CHECK(no_shots.bullets.back().group==SigmaCore::ring_32); // Emitted on motion's reset tick.
}

void rain_and_bursts(){
    Harness h;h.at(1);
    h.advance(19);CHECK(h.bullets.empty()&&h.random_calls==0);
    h.step();CHECK(h.sigma.left[h.last_back]==161&&h.sounds==std::vector<unsigned>{3});
    auto expected=h.random;const auto x=161+int(expected.masked(127));const auto speed=30u+expected.masked(31);
    h.step();CHECK(h.random_calls==2&&h.bullets.size()==1);
    CHECK(h.bullets.back().left==x&&h.bullets.back().top==112&&h.bullets.back().angle==64);
    CHECK(h.bullets.back().speed==speed&&h.bullets.back().group==0x20);
    h.advance(126);CHECK(h.sigma.left[h.last_back]==288);
    h.step();CHECK(h.sigma.left[h.last_back]==284);
    h.advance(63);CHECK(h.sigma.left[h.last_back]==32);
    h.step();CHECK(h.sigma.left[h.last_back]==33);
    h.advance(127);CHECK(h.sigma.left[h.last_back]==160&&h.random_calls==320&&h.bullets.size()==160);
    h.step();CHECK(h.random_calls==327&&h.bullets.size()==163);
    h.advance(99);CHECK(h.random_calls==570&&h.bullets.size()==235);
    h.step();CHECK(h.sounds.size()==106&&h.sigma.subattack==1&&h.sigma.phase_frame==0);
    h.advance(99);CHECK(h.sigma.bursts[0].state==0);
    h.step();CHECK(h.sigma.bursts[0].x==128&&h.sigma.bursts[0].y==320&&h.sigma.bursts[0].radius==64);
    CHECK(h.paint(SigmaCore::Paint::Sprite,26,0));
    h.advance(20);CHECK(h.sigma.bursts[1].x==320&&h.sigma.bursts[1].y==320);
    h.advance(32);CHECK(h.bullets.size()==240&&h.bullets.back().speed==120&&h.bullets.back().group==0x17);
    h.advance(11);CHECK(h.sigma.bursts[0].state==2&&h.sigma.bursts[0].radius==0);
    h.hooks.front_player_x=110;h.hooks.front_player_y=302;
    h.step(1);CHECK(h.sigma.bursts[0].state==3&&h.sigma.bursts[0].radius==0&&h.hits==0);
    h.step(0);CHECK(h.sigma.bursts[0].radius==16&&h.hits==0);
    h.step(0);CHECK(h.sigma.bursts[0].radius==32&&h.hits==1);
    h.hooks.front_player_x=100;h.step(1);CHECK(h.hits==1); // Strict x boundary with margin 4.
    h.advance(29);CHECK(h.sigma.bursts[0].state==4&&h.sigma.bursts[0].radius==8&&h.sigma.bursts[0].target_radius==64);
    h.advance(12);CHECK(h.sigma.bursts[0].state==0);
}

void orbit_and_lasers(){
    Harness orbit;orbit.at(3);orbit.advance(49);
    orbit.step(1);CHECK(orbit.sigma.draw_pattern==132&&orbit.sounds==std::vector<unsigned>{9});
    orbit.advance(49);orbit.step();CHECK(orbit.sigma.draw_pattern==128);
    orbit.advance(4);CHECK(orbit.paint(SigmaCore::Paint::Circle,112,15));
    orbit.advance(2);CHECK(orbit.paint(SigmaCore::Paint::Circle,112,0));
    orbit.advance(50);
    CHECK(orbit.sigma.bursts[0].x==288&&orbit.sigma.bursts[0].y==206&&orbit.sigma.bursts[0].target_radius==24);
    CHECK(orbit.sigma.bursts[1].x==64&&orbit.sigma.bursts[1].y==206);
    orbit.advance(52);CHECK(orbit.bullets.size()==1&&orbit.bullets.back().group==0x27&&orbit.bullets.back().speed==80);
    orbit.advance(241);CHECK(orbit.sigma.phase_frame==449&&orbit.random_calls==16);
    orbit.step(1);CHECK(orbit.sigma.phase_frame==450&&orbit.sigma.subattack==0);
    orbit.step(0);CHECK(orbit.sigma.phase_frame==0&&orbit.sigma.subattack==1);
    orbit.advance(306);CHECK(orbit.sigma.phase==3&&orbit.sigma.cycled&&orbit.bullets.back().group==0x25&&orbit.bullets.back().speed==50);

    Harness lasers;lasers.at(5);lasers.advance(100);CHECK(lasers.turns==std::vector<unsigned>{1});
    lasers.advance(11);CHECK(lasers.bullets.empty());lasers.step();
    CHECK(lasers.bullets.size()==2&&lasers.bullets[0].left==32&&lasers.bullets[1].left==400);
    CHECK(lasers.bullets[0].top==24&&lasers.bullets[0].group==0x87&&lasers.bullets[0].pattern==122);
    lasers.advance(176);CHECK((lasers.lasers==std::vector<Harness::Laser>{{220,108,1,111,32},{204,108,1,111,48},
        {236,108,1,111,48},{188,108,1,111,100},{252,108,1,111,100}}));
    lasers.advance(608);CHECK(lasers.sigma.phase_frame==0&&lasers.sigma.subattack==1);
    CHECK(lasers.bullets.size()==100&&lasers.bullets.back().pattern==123&&lasers.lasers.size()==10);
    lasers.advance(113);CHECK(lasers.lasers.size()==10);lasers.step();
    CHECK((lasers.lasers.back()==Harness::Laser{284,172,1,111,16}));
    lasers.advance(8);CHECK(lasers.lasers.size()==15&&lasers.bullets.size()==100);
    lasers.advance(6);CHECK(lasers.bullets.size()==106&&lasers.bullets[100].angle==0x90&&lasers.bullets.back().angle==0xE0);
    CHECK(lasers.bullets.back().pattern==123&&lasers.bullets.back().speed==60);
    lasers.advance(122);CHECK(lasers.lasers.size()==20);
    lasers.advance(56);CHECK(lasers.sigma.cycled&&lasers.sigma.phase==5&&lasers.sigma.phase_frame==0);
}

void opposed_and_final_patterns(){
    Harness opposed;opposed.at(7);opposed.advance(103);CHECK(opposed.bullets.empty());
    opposed.step();CHECK(opposed.bullets.size()==1&&opposed.bullets.back().group==0x25);
    CHECK(opposed.sigma.bursts[0].x==220&&opposed.sigma.bursts[1].x==220&&opposed.sigma.bursts[0].y==108);
    for(auto &b:opposed.sigma.bursts)b.state=1; // Prove the dual allocation failure path, not an invented timeout.
    opposed.advance(8);CHECK(opposed.bullets.size()==2&&opposed.sigma.phase_frame==0&&opposed.sigma.subattack==1);
    opposed.advance(64);CHECK(opposed.bullets.size()==3&&opposed.bullets.back().group==0x27);
    opposed.advance(32);CHECK(opposed.sigma.bursts[0].state!=0||opposed.sigma.bursts[1].state!=0);
    opposed.advance(210);CHECK(opposed.sigma.cycled&&opposed.bullets.back().group==0x27);

    Harness f;f.at(9);f.advance(49);CHECK(f.sigma.left[f.last_back]==160);
    f.step();CHECK(f.sigma.left[f.last_back]==162);f.advance(6);
    CHECK(f.bullets.size()==1&&f.bullets[0].group==0x26&&f.bullets[0].left==232&&f.bullets[0].speed==70);
    f.advance(8);CHECK(f.sigma.bursts[0].x==176&&f.sigma.bursts[0].target_radius==48);
    f.advance(306);CHECK(f.sigma.subattack==1&&f.bullets.size()==41&&f.sigma.phase_frame==0&&f.sigma.left[f.last_back]==160);
    f.advance(64);CHECK(f.bullets.size()==43&&f.bullets.back().group==0x24&&f.bullets.back().pattern==86);
    CHECK(f.bullets[41].angle==0&&f.bullets[42].angle==8);
    f.advance(306);CHECK(f.sigma.subattack==2&&f.bullets.size()==82&&f.sigma.phase_frame==0);
    f.advance(370);CHECK(f.sigma.subattack==0&&f.sigma.cycled&&f.bullets.size()==92&&f.random_calls==10);
}

void retirement(){
    Harness threshold;threshold.at(9);threshold.advance(369);threshold.sigma.damage_taken=5000;threshold.step();
    CHECK(!threshold.sigma.dying&&threshold.sigma.phase==10&&threshold.sigma.damage_taken==0&&threshold.scores.empty());
    Harness h;h.at(9);h.damage=1299;h.step();h.damage=1;h.step();h.damage=100;
    CHECK(!h.sigma.dying&&h.sigma.damage_taken==1300);h.step();
    CHECK(h.sigma.dying&&h.sigma.phase_frame==0&&h.sigma.sigma_frame==3&&h.scores==std::vector<unsigned>{300000});
    const auto damage_queries=h.damage_rects.size();
    h.advance(9);CHECK(h.sigma.visible&&h.sigma.draw_pattern==128&&h.sounds.empty());
    h.step();CHECK(h.sounds.back()==9);
    h.hooks.front_player_x=207;h.advance(79);CHECK(h.sounds.back()==3&&h.hits==0);
    h.step();CHECK(h.hits==0); // Radius 8 leaves a zero-width collision window.
    h.hooks.front_player_x=200;h.step();CHECK(h.hits==0); // Exactly x+56-16.
    h.hooks.front_player_x=201;h.step();CHECK(h.hits==1);
    h.advance(8);CHECK(!h.sigma.visible&&h.sigma.pattern==134); // f100 changes pattern without drawing body.
    h.step();CHECK(h.sigma.visible&&h.sigma.draw_pattern==134&&h.paint(SigmaCore::Paint::Box,311,15));
    h.advance(39);CHECK(h.sigma.phase_frame==140&&h.sigma.slowdown==1);
    h.step();CHECK(h.sigma.slowdown==2&&h.sigma.shake==1);
    h.advance(9);CHECK(h.sigma.draw_pattern==136&&h.sigma.shake==0);
    h.advance(29);CHECK(h.sigma.draw_pattern==140&&h.sigma.slowdown==2);
    h.step();CHECK(h.sigma.slowdown==1&&!h.sigma.visible);
    h.advance(41);CHECK(h.sigma.phase_frame==221&&h.paint(SigmaCore::Paint::Box,311,0));
    h.advance(2);CHECK(!h.sigma.done);h.step();CHECK(h.sigma.done&&h.sigma.phase_frame==224&&h.sigma.sigma_frame==3);
    h.advance(2);CHECK(h.sigma.phase_frame==224&&h.damage_rects.size()==damage_queries&&h.scores.size()==1);
}

void deterministic_full_core(){
    // From init through all five damage gates; damage is supplied by the host
    // hook instead of overwriting phase state. This is not a playability test.
    const auto run=[](bool reduced){
        Harness h;h.hooks.reduced=reduced;h.damage=5; // First gate takes the first attack; later gates can wrap one subattack.
        std::uint64_t trace=14695981039346656037ull;
        auto mix=[&](unsigned v){trace=(trace^v)*1099511628211ull;};
        unsigned bullet_cursor=0,laser_cursor=0,sound_cursor=0;
        std::array<unsigned,11> first{};
        for(unsigned tick=1;tick<=6500&&!h.sigma.done;++tick){
            h.step();if(!first[h.sigma.phase])first[h.sigma.phase]=tick;
            mix(h.sigma.phase);mix(h.sigma.phase_frame);mix(h.sigma.subattack);mix(h.sigma.damage_taken);
            mix(unsigned(h.sigma.left[h.last_back]));mix(unsigned(h.sigma.top[h.last_back]));mix(h.random_calls);
            while(bullet_cursor<h.bullets.size()){
                const auto &b=h.bullets[bullet_cursor++];
                mix(b.kind);mix(unsigned(b.left));mix(unsigned(b.top));mix(b.angle);mix(b.group);mix(b.pattern);mix(b.speed);
            }
            while(laser_cursor<h.lasers.size())for(int v:h.lasers[laser_cursor++])mix(unsigned(v));
            while(sound_cursor<h.sounds.size())mix(h.sounds[sound_cursor++]);
        }
        CHECK(h.sigma.done&&h.scores==std::vector<unsigned>{300000});
        CHECK(first[1]==51&&first[2]==491&&first[3]==492&&first[7]>first[5]&&first[9]>first[7]);
        CHECK(h.sigma.sigma_frame+224==h.ticks&&h.lasers.size()>=10);
        return trace;
    };
    const auto a=run(false);CHECK(a==0x642680201df59c8cull&&a==run(true));
    // The per-attack assertions independently anchor frame arithmetic and spawn order;
    // the trace locks the mixed fixed-seed run against accidental changes.
    std::cout << "Sigma fixed-seed gameplay fingerprint: " << std::hex << a << std::dec << '\n';

    Harness missing;missing.hooks.bullet={};missing.step();
    CHECK(missing.sigma.blocked&&std::string_view(missing.sigma.blocked)=="sigma: missing gameplay hook");
    CHECK(missing.sigma.sigma_frame==0&&missing.sigma.phase_frame==0&&missing.damage_rects.empty());
    missing.sigma.init();missing.sigma.begin_frame(2);
    CHECK(std::string_view(missing.sigma.blocked)=="sigma: invalid back page");
}

int main(){
    intro_damage_and_cycle();rain_and_bursts();orbit_and_lasers();opposed_and_final_patterns();retirement();deterministic_full_core();
}
