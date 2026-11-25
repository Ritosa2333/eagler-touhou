#pragma once
#include "core_contract.hpp"
#include "enemy_math.hpp"
#include <array>
#include <cstdint>
#include <vector>
#include <stdexcept>
namespace soew {
// TH02 player_bomb, bomb_update_and_render and A/B/C body state machines.
// Rendering is an output snapshot, not a simulation callback. The original
// pre-update circle invalidation clears circle_frame on the tick AFTER32.
struct BombCore {
    bool active=false,circle_done=false,clear_requested=false,finished=false;
    int frame=0,circle_frame=0;
    unsigned shot_type=0;
    std::uint8_t stage_used=0,total_used=0,overlay_frame=0;
    int palette_tone=100,slowdown=1;
    enum Visual {None,Circle,Mask,Body,Recovery};
    Visual visual=None;
    std::array<int,48> columns{};
    unsigned bft_tile_index=0;
    struct Particle {int x=0,y=0;std::uint8_t state=0,draw_state=0;};
    std::array<Particle,32> particles{};
    std::vector<unsigned> sounds;
    struct Spark {int x,y,speed,count;bool sprite;};
    std::vector<Spark> spark_requests;
    bool start(std::uint8_t &stock,unsigned type,bool infinite=false){
        if(type>2)throw std::runtime_error("Invalid bomb type");
        if(active||(!stock&&!infinite))return false;
        active=true;shot_type=type;if(!infinite)--stock;++stage_used;++total_used;
        frame=0;circle_frame=0;circle_done=false;clear_requested=true;finished=false;
        sounds.push_back(9);return true;
    }
    static int sar(int value,unsigned bits){const int divisor=1<<bits;return value>=0?value/divisor:-((-value+divisor-1)/divisor);}
    void particle_step(RandomRing &rng){
        const unsigned first=unsigned(frame&15)*2;
        for(unsigned i=0;i<2;++i){auto &p=particles[first+i];p.x=32+rng.next16()%384;p.y=16+rng.next16()%368;p.state=1;}
        for(unsigned i=0;i<32;++i){auto &p=particles[i];p.draw_state=0;if(!p.state)continue;if((unsigned(frame)&3)==(i&3)){++p.state;if(p.state>=5){p.state=0;continue;}}p.draw_state=p.state;}
    }
    // C's per-column velocities are the original96 bytes at DS:0BE0. They
    // are supplied by the checked renderer/resource binding, not guessed.
    std::array<int,48> column_speed={4,3,3,2,2,1,1,0,0,2,3,2,1,0,1,2,1,0,0,0,2,3,3,2,3,1,2,0,0,1,1,2,2,3,1,2,3,2,1,0,1,1,2,2,3,3,4,4};
    void init(unsigned type){
        if(type>2)throw std::runtime_error("Invalid bomb type");
        shot_type=type;stage_used=0;active=false;circle_done=false;clear_requested=false;finished=false;frame=0;circle_frame=0;overlay_frame=0;bft_tile_index=0;spark_requests.clear();sounds.clear();
    }
    void update(RandomRing &rng){
        finished=false;visual=None;for(auto &p:particles)p.draw_state=0;
        if(!active)return;
        // sub_E2D9 executes before bomb_update_and_render each game tick.
        if(circle_frame>1&&frame>=33)circle_frame=0;
        ++frame;
        if(!circle_done){
            if(frame<=32){++circle_frame;visual=Circle;}
            else if(circle_frame==0){frame=0;circle_done=true;}
            return;
        }
        visual=frame<32?Mask:Body;
        if(shot_type==0){
            if(frame==32){} // Palette/tile state is an explicit render output.
            else if(frame>32&&frame<136){if(frame==33)bft_tile_index=0;
                if(frame<=64){if(frame==64)palette_tone=200;}
                else {spark_requests.push_back({212,190,50,1,false});if(frame==70)palette_tone=100;}
                if(frame>=86){palette_tone=frame*2-72;if(frame>111)slowdown=2;}
                if(frame<86){if(!(frame&3))sounds.push_back(15);particle_step(rng);}
                if(frame==86)sounds.push_back(17);
            }else if(frame==136){sounds.push_back(16);slowdown=1;palette_tone=200;clear_requested=true;}
            else if(frame>136&&frame<156){visual=Recovery;palette_tone=200-(frame*5-680);}
            if(frame>=156)finish();
        }else if(shot_type==1){
                if(frame==32)overlay_frame=0;
            else if(frame>32&&frame<132){
                if(frame==33){bft_tile_index=0;}
                if(!(frame&3)){if((frame&7)==4)sounds.push_back(16);++overlay_frame;++bft_tile_index;if(overlay_frame>=18)frame=132;}
                // Even on overlay18's jump to132, the current source branch
                // still runs particle_step with frame132 before returning.
                particle_step(rng);
            }else if(frame>=132&&frame<164){slowdown=2;if(!(frame&3))sounds.push_back(15);palette_tone=200-(frame*3-396);}
            else if(frame==164){sounds.push_back(16);slowdown=1;palette_tone=200;clear_requested=true;}
            else if(frame>164&&frame<184){visual=Recovery;palette_tone=200-(frame*5-820);}
            if(frame>=184)finish();
        }else{
            if(frame==32){columns.fill(20);sounds.push_back(17);}
            else if(frame>32&&frame<100){if(frame>=80){palette_tone=frame*5-300;slowdown=2;}particle_step(rng);for(unsigned i=0;i<48;++i)columns[i]=enemy_s16(unsigned(columns[i]+column_speed[i]));}
            else if(frame>=100&&frame<112){palette_tone=100;if(frame&1)sounds.push_back(16);}
            else if(frame==112){slowdown=1;palette_tone=200;clear_requested=true;}
            else if(frame>112&&frame<128){visual=Recovery;palette_tone=200-(frame*5-540);}
            if(frame>=128)finish();
        }
    }
    void finish(){active=false;finished=true;palette_tone=100;visual=None;}
    // sub_1283C adds bomb damage AFTER projectile hits, before lowword score.
    unsigned damage(std::uint32_t stage_frame,unsigned mask=1)const{return active&&!(stage_frame&mask)?1u:0u;}
};
}
