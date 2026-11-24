#pragma once
#include "midboss1_core.hpp"
namespace soew {
// Partial Five Magic Stones implementation. Intentionally not wired into playable routes yet.
struct StonesCore {
    enum class Flag { Dormant, Active, Dying, Remove, Removed };
    enum class Phase { Opening, Inner, Outer, NorthOpening, NorthFirstAttacks, NorthClosing, NorthSecondAttacks, NorthReopening, NorthFinalAttacks, NorthDeath, Complete };
    struct Stone {int x,y;unsigned pattern=148;Flag flag=Flag::Dormant;std::uint16_t damage=0;unsigned death_frame=0;bool flash=false;};
    struct Draw {int x=0,y=0;unsigned pattern=0;bool visible=false,flash=false,dying=false;};
    struct Hooks : Midboss1Core::Hooks {
        std::function<void(int,int,unsigned)> item;
        std::function<unsigned()> random8;
        // Screen-space stripe requests; the scene adapter owns the VRAM conversion.
        std::function<void(int,int,unsigned)> stripe_tile;
        // left, top, active frames, base cel, initial delay (sub_12A19).
        std::function<void(int,int,int,unsigned,unsigned)> laser;
        std::function<void(unsigned)> invincibility;
        std::function<void(unsigned)> bullet_stack_multiplier;
    };
    std::array<Stone,5> stones={{{128,32},{288,32},{48,48},{368,48},{208,32}}};
    std::array<Draw,5> draw{};
    Phase phase=Phase::Opening;
    unsigned phase_frame=0,elapsed=0,attack_frame=0;
    int target_x=-1,target_y=-1;
    unsigned stripe_completed=0, north_attack=0;
    std::uint8_t sweep_angle=0, second_angle=0, laser_stage=0;
    int laser_x=32;
    std::array<std::uint8_t,5> aimed_angles{};
    // Ordered requests for sub_FFF8. The scene owns the page/palette effects.
    struct DeathEffect {int x,y;unsigned frame;std::uint8_t angle;};
    std::array<DeathEffect,2> death_effects{};
    unsigned death_effect_count=0;
    std::uint8_t death_angle=0x20;
    bool done()const{return phase==Phase::Complete;}
    bool terminal_death_frame()const{
        return phase==Phase::NorthDeath&&stones.back().flag==Flag::Remove&&death_effect_count==2&&death_effects.front().frame==95;
    }
private:
    bool open_north_stripe(const Hooks &h){
        // stones_122B5 and stones_121F3: frame 49 initializes the row. From
        // frame 51, every odd frame writes a west/east pair; each pass takes 12 pairs.
        if(phase_frame==49)stripe_completed=0;
        if(phase_frame>=51&&phase_frame<=121&&phase_frame%2){
            const unsigned step=(phase_frame-51)/2;
            const unsigned column=step%12,tile=41+step/12;
            h.stripe_tile(32+int(column)*16,96,tile);
            h.stripe_tile(32+int(23-column)*16,96,tile);
            if(tile==43)stripe_completed+=2;
        }
        return phase_frame>=49&&stripe_completed==24;
    }
public:
    void init(){*this=StonesCore{};}
    void update(unsigned rank,int player_y,const Hooks &h){update(rank, -32768, player_y, h);}
    void update(unsigned rank,int player_x,int player_y,const Hooks &h){
        if(done())return;
        death_effect_count=0;
        if(player_y<96&&!h.random8)throw std::runtime_error("Stones RNG unavailable");
        if((phase==Phase::NorthOpening||phase==Phase::NorthReopening)&&!h.stripe_tile)throw std::runtime_error("Stones stripe renderer unavailable");
        if(phase==Phase::NorthClosing&&!h.stripe_tile)throw std::runtime_error("Stones stripe renderer unavailable");
        const unsigned next=phase_frame+1;
        if(phase==Phase::NorthFirstAttacks){
            const bool aimed=north_attack==2&&next>=24&&next%(28-3*rank)==0;
            if(aimed&&player_x==-32768)throw std::runtime_error("Stones player X unavailable");
            const bool laser=(north_attack==0&&(next==50||next==80))||aimed||(north_attack==5&&(next==36||next==72||next==100));
            const bool rng=(north_attack==3&&next%16==0)||(north_attack==4&&next%4==0)||(north_attack==5&&(next==36||next==72||next==100));
            if(laser&&!h.laser)throw std::runtime_error("Stones laser renderer unavailable");
            if(rng&&!h.random8)throw std::runtime_error("Stones RNG unavailable");
        }
        if(phase==Phase::NorthSecondAttacks&&north_attack==3){
            if(next>=64&&next%64==0&&player_x==-32768)throw std::runtime_error("Stones player X unavailable");
            if(((next>=384&&next%4==0)||next>=447)&&!h.bullet_stack_multiplier)throw std::runtime_error("Stones bullet stack multiplier unavailable");
        }
        if(phase==Phase::NorthFinalAttacks){
            const bool laser=north_attack==0&&next>=16&&next%8==0&&(next==16||laser_stage==0||laser_stage>36);
            const bool aimed=north_attack==2&&next<102&&next%6==0;
            const bool rng=(aimed&&next<40)||(north_attack==3&&next%16==0)||(north_attack==4&&next%4==0)||(north_attack==5&&(next==36||next==72||next==100));
            const bool random_laser=north_attack==5&&(next==36||next==72||next==100);
            if((laser||random_laser)&&!h.laser)throw std::runtime_error("Stones laser renderer unavailable");
            if(aimed&&player_x==-32768)throw std::runtime_error("Stones player X unavailable");
            if(rng&&!h.random8)throw std::runtime_error("Stones RNG unavailable");
        }
        ++phase_frame;++elapsed;++attack_frame;
        for(auto &s:stones)if(s.flag==Flag::Remove)s.flag=Flag::Removed;
        const auto emit=[&](int x,int y,unsigned angle,unsigned group,unsigned speed,unsigned pat=0){
            if(!h.bullet)return;
            EnemyBulletRequest r;r.left=x;r.top=y;r.angle=std::uint8_t(angle);r.group=std::uint8_t(group);r.speed=speed;
            if(pat){r.kind=EnemyBulletRequest::Bullet16;r.pattern=std::uint8_t(pat);}h.bullet(r);
        };
        for(unsigned i=0;i<stones.size();++i){auto &s=stones[i];
            if(s.flag!=Flag::Dormant&&s.flag!=Flag::Active)continue;
            const unsigned d=h.damage?h.damage(s.x,s.y,32,40):0;
            // Dormant stones consume the collision call but do not accrue damage.
            if(!d||s.flag!=Flag::Active)continue;
            s.flash=true;s.damage=std::uint16_t(s.damage+d);
            if(enemy_s16(s.damage)<(i==4?930:i<2?140:120))continue;
            s.flag=Flag::Dying;if(h.score)h.score(30000);
            if(i==4){if(h.invincibility)h.invincibility(200);phase=Phase::NorthDeath;}
            else {if(h.item)h.item(s.x+8,s.y+12,(i==1||i==2)?3:2);if(h.sound)h.sound(2);}
        }
        for(const auto &s:stones)if(s.flag==Flag::Active){target_x=s.x+8;target_y=s.y+8;break;}
        if(player_y<96)emit(220,40,(h.random8()&15u)-7u,0x19,77);
        unsigned removed=0;
        for(unsigned i=0;i<stones.size();++i){auto &s=stones[i];auto &d=draw[i];d={s.x,s.y,s.pattern,false,false};
            if(s.flag==Flag::Dormant||s.flag==Flag::Active){d.visible=true;d.flash=s.flash;if(s.flash&&h.sound)h.sound(4);s.flash=false;}
            else if(s.flag==Flag::Removed)++removed;
            else if(s.flag==Flag::Dying){
                d.dying=true;
                if(h.spark)h.spark({s.x+16,s.y+12,80,2,false});
                const auto frame=s.death_frame;
                if(i==4){
                    // ASM 7323-7388: both effects use the old angle on frame 56.
                    const auto request=[&](unsigned t){
                        death_effects[death_effect_count++]={s.x+8,s.y+8,t,death_angle};
                        if(h.sound&&t==1)h.sound(18);
                        if(h.sound&&t==32)h.sound(14);
                    };
                    request(frame);
                    if(frame>=24)request(frame-24);
                    if(frame==56)death_angle=0xe0;
                    d.pattern=frame<32?s.pattern:10+(frame-32)/8;
                }else d.pattern=10+frame/8;
                if(++s.death_frame>=(i==4?96u:64u)){s.death_frame=0;s.flag=Flag::Remove;}else d.visible=true;
            }
        }
        // Remove becomes Removed at the next background step, before counting.
        if(removed==stones.size()){phase=Phase::Complete;target_x=target_y=-1;return;}
        const auto timeout_pair=[&](unsigned first){if(elapsed>1300)for(unsigned i=first;i<first+2;++i)if(stones[i].flag==Flag::Active)stones[i].flag=Flag::Dying;};
        const auto reset_phase=[&](Phase next){phase=next;phase_frame=attack_frame=0;};
        switch(phase){
        case Phase::Opening:
            switch(phase_frame){case 50:case 52:case 54:case 58:case 60:case 62:case 64:++stones[0].pattern;++stones[1].pattern;break;default:break;}
            if(phase_frame==64){stones[0].flag=stones[1].flag=Flag::Active;reset_phase(Phase::Inner);}break;
        case Phase::Inner:
            if(phase_frame>=48&&phase_frame<180&&phase_frame%24==0){for(unsigned i=0;i<2;++i){const auto &s=stones[i];if(s.flag==Flag::Active)emit(s.x+12,s.y+8,0,rank==0?0x0c:0x18,38);}}
            if(phase_frame>=200&&phase_frame<310&&phase_frame%16==0){for(unsigned i=0;i<2;++i){const auto &s=stones[i];if(s.flag==Flag::Active)emit(s.x+12,s.y+8,0,0x19,80,81);}}
            if(phase_frame>350)phase_frame=0;
            timeout_pair(0);
            if(removed>=2&&phase_frame%3==0){
                for(unsigned i=2;i<4;++i){auto &s=stones[i];if(s.pattern<152)++s.pattern;if(s.pattern==152)s.pattern=156;else ++s.pattern;if(s.pattern>=159)s.flag=Flag::Active;}
                if(stones[3].flag==Flag::Active)reset_phase(Phase::Outer);
                elapsed=0;
            }break;
        case Phase::Outer:
            timeout_pair(2);
            if(removed>=4){reset_phase(Phase::NorthOpening);target_x=stones[4].x+8;target_y=32;break;}
            if(phase_frame>=30&&phase_frame%((phase_frame<200?110u:90u)-20u*removed)==0){for(unsigned i=2;i<4;++i){const auto &s=stones[i];if(s.flag==Flag::Active)for(unsigned n=0;n<6;++n)emit(s.x+12,s.y+8,n*42,0x82,56,82);}}break;
        case Phase::NorthOpening:
            if(open_north_stripe(h)&&phase_frame%3==0){
                auto &north=stones[4];
                if(north.pattern<152)++north.pattern;
                if(north.pattern==152)north.pattern=160;else ++north.pattern;
                if(north.pattern>=163){north.flag=Flag::Active;reset_phase(Phase::NorthFirstAttacks);elapsed=0;}
            }
            break;
        case Phase::NorthFirstAttacks:
            // Phase 4 attack sequence, as dispatched at th02_main.asm:9038-9094.
            if(north_attack==0){
                if(phase_frame==50||phase_frame==80){
                    const int a[4]={phase_frame==50?48:80,phase_frame==50?192:160,phase_frame==50?240:272,phase_frame==50?384:352};
                    for(int x:a)h.laser(x,96,36,103,16);
                }
                if(phase_frame==80){phase_frame=attack_frame=0;++north_attack;}
            }else if(north_attack==1){
                if(phase_frame%4==0){
                    emit(220,40,sweep_angle,0x23,70);
                    sweep_angle=std::uint8_t(sweep_angle+8);
                    if(sweep_angle>126){sweep_angle=0;phase_frame=attack_frame=0;++north_attack;}
                }
            }else if(north_attack==2){
                if(phase_frame>=24&&phase_frame%(28-3*rank)==0)h.laser((player_x+16)&~15,96,30,103,0);
                if(phase_frame>=140){phase_frame=attack_frame=0;++north_attack;}
            }else if(north_attack==3){
                if(phase_frame%16==0)for(unsigned i=rank==0?2:0;i<4;++i)emit(stones[i].x,stones[i].y,h.random8(),0x83,32,82);
                if(phase_frame>110){phase_frame=attack_frame=0;++north_attack;}
            }else if(north_attack==4){
                if(phase_frame%4==0)emit(220,40,h.random8(),rank==0?0x22:0x21,30+phase_frame/2);
                if(phase_frame>130){phase_frame=attack_frame=0;++north_attack;}
            }else if(north_attack==5){
                if(phase_frame==36||phase_frame==72||phase_frame==100){
                    for(unsigned i=0;i<(phase_frame==100?3u:1u);++i)h.laser(int((h.random8()%24+2)*16),96,30,111,12);
                }
                if((phase_frame==40)&&rank!=0)emit(220,40,0,0x19,70);
                if(phase_frame>=120){phase_frame=attack_frame=0;north_attack=0;stripe_completed=0;phase=Phase::NorthClosing;}
            }
            if(enemy_s16(stones[4].damage)>=500)stones[4].damage=500;
            break;
        case Phase::NorthClosing:
            if(phase_frame%3==0&&north_attack==0){
                auto &north=stones[4];
                if(north.pattern>=160)--north.pattern;
                if(north.pattern==159){north.flag=Flag::Dormant;north.pattern=151;}else --north.pattern;
                if(north.pattern<=148)north_attack=1;
            }
            if(phase_frame==49)stripe_completed=0;
            if(phase_frame>=51&&phase_frame<=121&&phase_frame%2){
                unsigned step=(phase_frame-51)/2,column=step%12,tile=42-step/12;
                h.stripe_tile(32+int(column)*16,96,tile);
                h.stripe_tile(32+int(23-column)*16,96,tile);
                if(tile==40)stripe_completed+=2;
            }
            if(stripe_completed==24){phase=Phase::NorthSecondAttacks;phase_frame=attack_frame=elapsed=0;north_attack=0;}
            break;
        case Phase::NorthSecondAttacks:
            if(north_attack==0||north_attack==2){
                if(phase_frame==2)second_angle=62;
                if(phase_frame%8==0){
                    emit(220,40,0u-second_angle,rank==0?0x21:0x24,64);
                    emit(220,40,second_angle,rank==0?0x21:0x24,64);
                    second_angle=std::uint8_t(second_angle-3);
                    if(second_angle>200){phase_frame=attack_frame=0;++north_attack;}
                }
            }else if(north_attack==1){
                if(phase_frame==16)second_angle=41;
                if(phase_frame>=16){
                    if(phase_frame%30==0){
                        for(unsigned i=0;i<4;++i){emit(stones[i].x,stones[i].y,second_angle,0x19,60,82);emit(stones[i].x,stones[i].y,0u-second_angle,0x19,60,82);}
                        second_angle=std::uint8_t(second_angle-3);
                    }
                    if(second_angle>=200){phase_frame=attack_frame=0;++north_attack;}
                }
            }else if(north_attack==3){
                if(phase_frame>=60){
                    if(phase_frame%64==0)for(unsigned i=0;i<5;++i)aimed_angles[i]=std::uint8_t(enemy_atan2(enemy_s16(unsigned(player_y-32)),enemy_s16(unsigned(player_x-stones[i].x))));
                    if(phase_frame%4==0){
                        if(phase_frame<384){
                            if((phase_frame%64)/4<=8){for(unsigned i=0;i<5;++i)emit(stones[i].x+12,stones[i].y+12,aimed_angles[i],0x20,112);if(h.sound)h.sound(3);}
                        }else{
                            h.bullet_stack_multiplier(0);
                            for(unsigned i=0;i<5;++i)emit(stones[i].x+8,stones[i].y+8,aimed_angles[4],0x21,112,86);
                            if(h.sound)h.sound(3);
                        }
                    }
                    if(phase_frame>=447){h.bullet_stack_multiplier(1);phase=Phase::NorthReopening;phase_frame=attack_frame=0;++north_attack;}
                }
            }
            break;
        case Phase::NorthReopening:
            // ASM 9155-9176: restore tile stripe then activate the north stone.
            // Unlike the first opening, elapsed is reset only after activation.
            if(open_north_stripe(h)&&phase_frame%3==0){
                auto &north=stones[4];
                if(north.pattern<152)++north.pattern;
                if(north.pattern==152)north.pattern=160;else ++north.pattern;
                if(north.pattern>=163){north.flag=Flag::Active;phase=Phase::NorthFinalAttacks;phase_frame=attack_frame=elapsed=0;north_attack=0;}
            }
            break;
        case Phase::NorthFinalAttacks:
            if(north_attack==0){
                if(phase_frame==16){laser_x=32;laser_stage=0;}
                if(phase_frame>=16){
                    if(laser_stage==0&&phase_frame%8==0){
                        h.laser(laser_x,96,16,111,3);h.laser(432-laser_x,96,16,111,3);
                        laser_x+=16;if(laser_x>176){laser_stage=1;laser_x+=16;}
                    }
                    if(laser_stage>36){
                        if(phase_frame%8==0){
                            h.laser(laser_x,96,16,111,1);h.laser(432-laser_x,96,16,111,1);
                            laser_x-=16;if(laser_x<64){laser_stage=0;laser_x=32;phase_frame=attack_frame=0;++north_attack;}
                        }
                    }else if(laser_stage!=0)++laser_stage;
                }
            }else if(north_attack==1){
                if(phase_frame%4==0){emit(220,40,sweep_angle,0x23,70);sweep_angle=std::uint8_t(sweep_angle+8);
                    if(sweep_angle>126){sweep_angle=0;phase_frame=attack_frame=0;++north_attack;}}
            }else if(north_attack==2){
                if(phase_frame>=102){phase_frame=attack_frame=0;++north_attack;}
                else if(phase_frame%6==0){
                    const unsigned angle=std::uint8_t(enemy_atan2(enemy_s16(unsigned(player_y-32)),enemy_s16(unsigned(player_x-stones[4].x))));
                    const unsigned offset=phase_frame<40?(h.random8()&31u)+16u:95u-phase_frame;
                    emit(220,40,angle+offset,0x20,80);emit(220,40,angle-offset,0x20,80);
                }
            }else if(north_attack==3){
                if(phase_frame%16==0)for(unsigned i=rank==0?2:0;i<4;++i)emit(stones[i].x,stones[i].y,h.random8(),0x83,32,82);
                if(phase_frame>110){phase_frame=attack_frame=0;++north_attack;}
            }else if(north_attack==4){
                if(phase_frame%4==0)emit(220,40,h.random8(),rank==0?0x22:0x21,30+phase_frame/2);
                if(phase_frame>130){phase_frame=attack_frame=0;++north_attack;}
            }else if(north_attack==5){
                if(phase_frame==36||phase_frame==72||phase_frame==100)
                    for(unsigned i=0;i<(phase_frame==100?3u:1u);++i)h.laser(int((h.random8()%24+2)*16),96,30,111,12);
                if(phase_frame==40&&rank!=0)emit(220,40,0,0x19,70);
                if(phase_frame>=120){phase_frame=attack_frame=0;north_attack=1;}
            }
            // ASM 9222-9234 tests timeout only after an attack resets the phase timer.
            if(phase_frame==0&&elapsed>2000){stones[4].flag=Flag::Dying;phase=Phase::NorthDeath;}
            break;
        case Phase::NorthDeath:case Phase::Complete:break;
        }
    }
};
}
