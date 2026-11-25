#pragma once
#include "core_contract.hpp"
#include "enemy_math.hpp"
#include <array>
namespace soew {
// th02/main/spark.cpp, spark_i.asm, bullet/bullet.cpp:91..129.
// No per-spawn random calls: angle/speed defaults are seeded via IRand at init.
struct SparkCore {
    struct Spark {
        unsigned flag=0;
        std::uint8_t age=0,angle=0,base_speed=0;
        bool sprite=false,default_sprite=false,shot_hit=false;
        std::array<std::array<std::uint16_t,2>,2> position{};
        std::array<int,2> velocity{};
    };
    std::array<Spark,64> slots{};
    unsigned ring=0,max_age=48;
    int accel_x=0;
    std::uint64_t generated=0;
    void initialize(IRand &rng,bool reduced=false){
        max_age=reduced?24:48;const unsigned interval=reduced?7:1;
        for(unsigned i=0;i<64;++i){auto &s=slots[i];s.flag=0;s.angle=static_cast<std::uint8_t>(rng.next());s.base_speed=static_cast<std::uint8_t>(rng.next()&63);s.default_sprite=(interval&i)==0;}
        // Original init does NOT reset spark_ring_i or accel_x.
    }
    static int pixel(std::uint16_t raw){const auto v=enemy_s16(raw);return v>=0?v/16:-((-v+15)/16);}
    void begin_frame(unsigned back){if(back>1)throw std::runtime_error("Invalid spark page");for(auto &s:slots){if(s.flag==2)s.flag=0;else if(s.flag==1)s.position[back]=s.position[back^1];}}
    void add(int left,int top,int base,int count,bool sprite,unsigned back,bool shot_hit=false){
        if(back>1||count<0||count>32767)throw std::runtime_error("Invalid spark request");
        unsigned wrapped_top=static_cast<std::uint16_t>(top);if(wrapped_top>=400)wrapped_top-=400;
        for(int i=0;i<count;++i){auto &s=slots[(ring+unsigned(i))&63];s.flag=1;s.age=0;s.sprite=sprite||s.default_sprite;s.shot_hit=shot_hit;
            s.position[back]={static_cast<std::uint16_t>(left*16),static_cast<std::uint16_t>(wrapped_top*16)};
            // Original16-bit shifted group index before signed division.
            const unsigned angle=sprite?unsigned(enemy_s16(unsigned(i)<<8)/count)&255:s.angle;
            const int speed=enemy_s16(unsigned(base+(sprite?0:s.base_speed)));
            s.velocity=enemy_vector(angle,speed);++generated;
        }
        ring=(ring+unsigned(count))&63;
    }
    void update(unsigned back){if(back>1)throw std::runtime_error("Invalid spark page");for(auto &s:slots){if(s.flag!=1)continue;
        s.position[back][0]=static_cast<std::uint16_t>(s.position[back][0]+s.velocity[0]);s.velocity[0]=enemy_s16(unsigned(s.velocity[0]+accel_x));
        s.position[back][1]=static_cast<std::uint16_t>(s.position[back][1]+s.velocity[1]);s.velocity[1]=enemy_s16(unsigned(s.velocity[1]+1));++s.age;
        const int x=pixel(s.position[back][0]),y=pixel(s.position[back][1]);if(x<32||x>=408||s.age>max_age||y>=384||y<=8)s.flag=2;
    }}
};
}
