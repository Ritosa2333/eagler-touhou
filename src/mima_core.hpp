#pragma once
#include "midboss1_core.hpp"
#include "collision_display.hpp"

namespace soew {
// ReC98 th02_main.asm:17874–21477. Coordinates and sprite IDs are original;
// the scene owns projectile allocation, collision/score side effects, and drawing.
struct MimaCore {
    enum class Phase { Opening, First, Shift1, Second, Shift2, Third, Shift3, Mixed, Form1Dialogue, Final, FinalDialogue };
    enum class Event { None, Form1Defeated, FinalDefeated };
    enum class Blocked { None, MissingRng, MissingProjectileSink };
    enum class Attack { Sweep, Beam, Orbit, Spiral, Symmetric, BurstOrbit, Bounce, Fan, Satellites, FinalSpin, FinalBurst, FinalAimed };
    struct Result {
        bool enter_form2=false,ending=false,good_ending=false;
        unsigned bonus=0;
    };
    struct Hooks : Midboss1Core::Hooks {
        std::function<unsigned()> random8;
        std::function<void(int,int,unsigned)> item;
        std::function<void()> player_hit,clear_bullets;
        std::function<void(unsigned)> invincibility,stack_multiplier,turns_max;
        std::function<void(int,int,int,int,unsigned)> line;
        std::function<void(int,int,unsigned,unsigned)> circle,dot,fill_circle;
        std::function<void(unsigned,unsigned,unsigned)> palette0; // Raw RGB8; hardware uses the high nibble.
        std::function<void(unsigned)> tone;
        CollisionFrame *collision_display=nullptr;
    };
    struct Player {
        int front_x=0,front_y=384,x=208,y=336;
        unsigned rank=1,stage_frame=0;
        bool reduce_effects=false;
        unsigned page=0;
    };
    struct Orb {
        int x=0,y=0;
        unsigned pattern=137;
        bool active=false;
    };

    static constexpr std::array<Attack,3> first={Attack::Sweep,Attack::Beam,Attack::Orbit};
    static constexpr std::array<Attack,3> second={Attack::Spiral,Attack::Symmetric,Attack::BurstOrbit};
    static constexpr std::array<Attack,3> third={Attack::Bounce,Attack::Fan,Attack::Satellites};
    static constexpr std::array<Attack,9> mixed={Attack::Bounce,Attack::Orbit,Attack::Spiral,Attack::Beam,Attack::Symmetric,Attack::BurstOrbit,Attack::Sweep,Attack::Fan,Attack::Satellites};
    static constexpr std::array<Attack,3> final={Attack::FinalSpin,Attack::FinalBurst,Attack::FinalAimed};
    Phase phase=Phase::Opening;
    Event event=Event::None;
    Blocked blocked=Blocked::None;
    const char *blocked_reason() const {
        switch(blocked){
        case Blocked::MissingRng:return "Mima attack RNG unavailable";
        case Blocked::MissingProjectileSink:return "Mima projectile sink unavailable";
        default:return nullptr;
        }
    }
    int left=144,top=64,target_x=-1,target_y=-1;
    unsigned phase_frame=0,pattern=128,selection=0,rounds=0;
    std::uint16_t damage_taken=0;
    bool body_damage_enabled=false,body_flash=false,form2=false,finished=false;
    std::array<Orb,8> orbs{};
    std::uint8_t bg_radius=0,bg_fill_radius=0,bg_fill_base=0,bg_fill_tick=0;
    unsigned bg_ring_color=13,bg_shadow_color=12,bg_fill_color=3;

    // ASM 17977–18092. Called before update; the scene clears the playfield.
    void render_background(const Hooks &h,bool reduce_effects){
        const unsigned step=reduce_effects?4:1;
        for(unsigned half=0;half<2;++half){
            for(unsigned layer=0;layer<3;++layer){
                const unsigned lag=layer==0?16:layer==1?8:0;
                for(unsigned i=half*128;i<(half+1)*128;i+=step){
                    const int x=224+enemy_vector(i,bg_radius)[0];
                    const int y=200+enemy_vector(unsigned(i+bg_angle-lag),bg_radius)[1];
                    draw_dot(h,x,y,bg_radius,layer==2?bg_ring_color:bg_shadow_color);
                    // The fill follows the first sample of the second half's first layer.
                    if(half==1&&layer==0&&i==128&&h.fill_circle)h.fill_circle(224,200,bg_fill_radius,bg_fill_color);
                }
            }
        }
        ++bg_angle;
    }

    void update(const Hooks &h,const Player &p) try {
        if(finished||event!=Event::None||blocked!=Blocked::None)return;
        if(p.page>1||p.rank>4)throw std::runtime_error("Invalid Mima page or rank");
        for(unsigned i=0;i<orbs.size();++i){
            const int prev_y=orb_position[p.page][i][1];
            if(orb_flag[i]==2&&prev_y>-16&&prev_y<382)orb_flag[i]=0;
        }
        ++phase_frame;
        const int x=left,y=top; // Source caches the firing offsets before any motion.
        target_x=x+72;target_y=y+56;
        body_flash=false;
        if(!(p.stage_frame&1)){
            if(!p.reduce_effects){
                outer_radius=std::uint8_t(outer_radius+8);
                for(unsigned r:{unsigned(outer_radius),unsigned(std::uint8_t(outer_radius+128))})
                    for(unsigned a=0;a<256;a+=16){const auto d=enemy_vector(a,r);draw_dot(h,224+d[0],200+d[1],r,3);}
            }
            if(unsigned(phase)&1){
                bg_fill_tick=std::uint8_t((bg_fill_tick+1)&63);
                bg_fill_radius=std::uint8_t(bg_fill_base-bg_fill_tick/4);
            }
        }
        switch(phase){
        case Phase::Opening:
            bg_radius=std::uint8_t(phase_frame/2);bg_fill_radius=std::uint8_t(phase_frame/3);
            palette(h,phase_frame/2,0,phase_frame/2);
            if(phase_frame>100)enter_attack(h,Phase::First,600,10,2);
            break;
        case Phase::Shift1:case Phase::Shift2:case Phase::Shift3:transition(h,x,y);break;
        case Phase::First:case Phase::Second:case Phase::Third:case Phase::Mixed:case Phase::Final:{
            const bool was_final=phase==Phase::Final;
            attack(h,p,selected_attack(),x,y);
            finish_round(h);
            if(was_final){if(phase_frame==1)top=64;if(phase_frame<30)left+=x+64<p.x?1:-1;}
            break;
        }
        case Phase::Form1Dialogue:case Phase::FinalDialogue:break;
        }
        collide(h,p);
        if(phase==Phase::Form1Dialogue){if(h.clear_bullets)h.clear_bullets();event=Event::Form1Defeated;}
        else if(phase==Phase::FinalDialogue)event=Event::FinalDefeated;
    } catch(Blocked reason) {
        blocked=reason;
    }
    bool queries_body_damage() const { return !body_defeated; }
    unsigned damage_unlock_round() const { return vulnerable_round; }
    Attack selected_attack() const {
        switch(phase){
        case Phase::First:return first.at(selection);
        case Phase::Second:return second.at(selection);
        case Phase::Third:return third.at(selection);
        case Phase::Mixed:return mixed.at(selection);
        case Phase::Final:return final.at(selection);
        default:throw std::logic_error("Mima is not in an attack phase");
        }
    }
    // Native Practice entry: use the same phase initializer as the encounter,
    // without the preceding attacks, drops, dialogue or score grants.
    void start_practice_phase(unsigned chapter,const Hooks &h){
        if(chapter<1||chapter>5||!h.random8)throw std::invalid_argument("Invalid Mima practice phase");
        *this=MimaCore{};
        if(chapter==5){event=Event::Form1Defeated;resolve_form1(h,0);return;}
        constexpr std::array phases={Phase::First,Phase::Second,Phase::Third,Phase::Mixed};
        constexpr std::array<unsigned,4> damage={600,700,800,1500},limit={10,12,12,200};
        bg_radius=std::uint8_t(50+25*(chapter-1));bg_fill_radius=std::uint8_t(30+20*(chapter-1));
        constexpr std::array<std::array<unsigned,3>,4> colors={{{50,0,50},{1,50,50},{50,1,1},{1,0,0}}};
        const auto rgb=colors[chapter-1];palette(h,rgb[0],rgb[1],rgb[2]);
        enhanced=chapter==4;if(enhanced)bg_shadow_color=2;
        enter_attack(h,phases[chapter-1],damage[chapter-1],limit[chapter-1],enhanced?3:2);
    }
    Result resolve_form1(const Hooks &h,unsigned continues_used){
        if(event!=Event::Form1Defeated||blocked!=Blocked::None)return {};
        if(!continues_used&&!h.random8){blocked=Blocked::MissingRng;return {};}
        event=Event::None;
        if(continues_used){finished=true;return {false,true,false,50000};}
        form2=true;enhanced=false;pattern=128;
        enter_attack(h,Phase::Final,1100,200,2);
        bg_fill_color=1;bg_shadow_color=2;
        bg_radius=200;bg_fill_radius=bg_fill_base=150;
        return {true,false,false,0};
    }
    Result resolve_final(unsigned continues_used){
        if(event!=Event::FinalDefeated)return {};
        event=Event::None;finished=true;
        return {false,true,continues_used==0,100000};
    }

private:
    std::uint8_t bg_angle=0,outer_radius=0;
    unsigned threshold=0,max_rounds=0,vulnerable_round=0;
    int vertical_step=0;
    bool enhanced=false,body_defeated=false,flash_pending=false;
    std::uint8_t flash_tick=0,angle=0,orbit_angle=0,satellite_angle=0,beam_open=0,beam_tone=100;
    int speed=0,turn=0,charge_radius=0;
    int orbit_x=0,orbit_y=0,orbit_dx=0,orbit_dy=0,orbit_r=0,orbit_step=0;
    bool orbit_burst=false;
    std::array<unsigned,8> orb_flag{};
    std::array<std::array<std::array<int,2>,8>,2> orb_position{};

    static unsigned random_byte(const Hooks &h){
        if(!h.random8)throw Blocked::MissingRng;
        return h.random8()&255;
    }
    static void sound(const Hooks &h,unsigned n){if(h.sound)h.sound(n);}
    static void palette(const Hooks &h,unsigned r,unsigned g,unsigned b){if(h.palette0)h.palette0(r,g,b);}
    static void draw_dot(const Hooks &h,int x,int y,unsigned r,unsigned color){
        if(h.dot&&x>24&&x<416&&y>8&&y<384)h.dot(x,y,r/64+1,color);
    }
    static void bullet(const Hooks &h,int x,int y,int a,unsigned group,unsigned speed,unsigned pat=0){
        if(!h.bullet)throw Blocked::MissingProjectileSink;
        EnemyBulletRequest b;b.left=x;b.top=y;b.angle=std::uint8_t(a);b.group=std::uint8_t(group);b.speed=speed;
        if(pat){b.kind=EnemyBulletRequest::Bullet16;b.pattern=std::uint8_t(pat);}
        h.bullet(b);
    }
    void pose(const Hooks &h,unsigned pat,unsigned se){pattern=pat;sound(h,se);}
    void enter_attack(const Hooks &h,Phase next,unsigned hp,unsigned rounds_limit,unsigned vulnerable_at){
        phase=next;phase_frame=rounds=0;
        selection=next==Phase::Mixed?(random_byte(h)&7):(random_byte(h)%3);
        damage_taken=0;threshold=hp;max_rounds=rounds_limit;vulnerable_round=vulnerable_at;
        bg_fill_base=bg_fill_radius;
    }
    void transition(const Hooks &h,int x,int y){
        const unsigned f=phase_frame;
        if(phase==Phase::Shift1){bg_radius=std::uint8_t(50+f/4);bg_fill_radius=std::uint8_t(30+f/5);}
        if(phase==Phase::Shift2){bg_radius=std::uint8_t(75+f/4);bg_fill_radius=std::uint8_t(50+f/5);}
        if(phase==Phase::Shift3){bg_radius=std::uint8_t(100+f/4);bg_fill_radius=std::uint8_t(70+f/5);}
        if(f==10)pose(h,131,9);
        if(f==40||f==60||f==80){
            if(f==60)sound(h,10);else pose(h,f==40?134:128,10);
            const unsigned item=f==40&&phase==Phase::Shift3?2:f==80&&phase==Phase::Shift1?4:3;
            if(h.item)h.item(x+64,y+114,item);
        }
        if(phase==Phase::Shift1)palette(h,51-f/2,f/2,50);
        else if(phase==Phase::Shift2)palette(h,f/2,51-f/2,51-f/2);
        else palette(h,51-f/2,0,0);
        if(f<=100)return;
        if(phase==Phase::Shift1)enter_attack(h,Phase::Second,700,12,2);
        else if(phase==Phase::Shift2)enter_attack(h,Phase::Third,800,12,2);
        else {enhanced=true;enter_attack(h,Phase::Mixed,1500,200,3);bg_shadow_color=2;}
    }
    void finish_round(const Hooks &h){
        if(!phase_frame){
            // Phase advancement uses the damage BEFORE this tick's body query.
            if(enemy_s16(damage_taken)>=int(threshold)||(!h.lock_timeout&&rounds>max_rounds)){
                phase=Phase(unsigned(phase)+1);body_damage_enabled=false;rounds=0;
            }else{
                if(++rounds>=vulnerable_round)body_damage_enabled=true;
                selection=(selection+1)%(enhanced?8:3); // Mixed case 8 exists but is unreachable from its selector.
                vertical_step=top<48?1:top>64?-1:1-int(random_byte(h)%3);
            }
        }
        if(phase_frame<10)top+=vertical_step;
    }
    void collide(const Hooks &h,const Player &p){
        if(!body_defeated){
            const unsigned n=h.damage?h.damage(left+48,top+32,64,64):0;
            if(n){
                flash_pending=true;
                if(body_damage_enabled)damage_taken=std::uint16_t(damage_taken+n);
                if(enemy_s16(damage_taken)>=6000){
                    sound(h,2);body_defeated=true;if(h.score)h.score(500000);if(h.invincibility)h.invincibility(200);
                }
            }
            const auto bounds=CollisionBounds::open(left+16,top,left+112,top+80);
            const CollisionPoint front{p.front_x,p.front_y};
            if(h.collision_display)h.collision_display->hazard(CollisionSource::MimaBody,bounds,CollisionSample::Front,front);
            if(h.player_hit&&bounds.contains(front))h.player_hit();
        }
        if(flash_pending&&!(++flash_tick&3)){body_flash=true;flash_pending=false;}
        for(unsigned i=0;i<orbs.size();++i){
            const auto &pos=orb_position[p.page][i];
            orbs[i]={pos[0],pos[1],137+(i&3),orb_flag[i]==1};
            if(!orbs[i].active)continue;
            if(h.damage)h.damage(pos[0]+4,pos[1],32,24);
            const auto bounds=CollisionBounds::open(pos[0]-16,pos[1]-16,pos[0]+16,pos[1]+16);
            const CollisionPoint front{p.front_x,p.front_y};
            if(h.collision_display)h.collision_display->hazard(CollisionSource::MimaOrb,bounds,CollisionSample::Front,front);
            if(h.player_hit&&bounds.contains(front))h.player_hit();
        }
    }
    void approach(const Player &p){pattern=128;left+=left+64<p.x?2:-2;}
    void orb_coordinates(unsigned page,int x,int y,int r,unsigned a,bool activate=false){
        for(unsigned i=0;i<4;++i){
            const auto d=enemy_vector(a+i*64,r);
            const std::array<int,2> q={x+d[0],y+d[1]};
            orb_position[page][i]=q;
            if(activate){orb_position[page^1][i]=q;orb_flag[i]=1;}
        }
    }
    static int subpixel(int v){v=enemy_s16(unsigned(v));return v>=0?v/16:-((-v+15)/16);}

    void attack(const Hooks &h,const Player &p,Attack id,int x,int y){
        const unsigned f=phase_frame;
        const int pellet_x=x+64,pellet_y=y+114;
        switch(id){
        case Attack::Sweep: // ASM 18599–18676
            if(f<10)break;
            if(f==10){if(h.stack_multiplier)h.stack_multiplier(0);pattern=131;}
            if(f<66){left-=2;if(left<=32)phase_frame=66;}
            else if(f<178){left+=2;if(left>=256)phase_frame=178;}
            else if(f<234){left-=2;if(left<=144)phase_frame=234;}
            else {phase_frame=0;pattern=128;if(h.stack_multiplier)h.stack_multiplier(1);}
            if(!(phase_frame%32)){
                bullet(h,x+40,y+16,56+(random_byte(h)&15),0x25,60);
                bullet(h,x+40,y+16,0,enhanced?(p.rank?0x27:5):(p.rank?0x17:0x19),40);
            }
            break;
        case Attack::Beam: // ASM 18683–18879
            if(f<20)break;
            if(f<70){approach(p);break;}
            if(f==70){pose(h,131,9);beam_open=0;beam_tone=100;break;}
            if(f<170){
                const unsigned step=unsigned(std::uint8_t(64-beam_open))/4;
                if(h.line)for(unsigned i=0;i<9;++i){const auto d=enemy_vector(beam_open+i*step,400);h.line(x+40,y+16,x+40+d[0],y+16+d[1],13);}
                ++beam_open;if(f&1)++beam_tone;
            }else if(f==170){pose(h,134,5);break;}
            else if(f<=198){
                top=272-int((f&3)*60);
                const int lx=left+8+int(random_byte(h)&127);
                if(h.line)h.line(lx,16,lx,383,13);
                const CollisionBounds bounds{CollisionAxis::open(left+16,left+96),CollisionAxis::any()};
                if(h.collision_display)h.collision_display->hazard(CollisionSource::MimaBeam,bounds,CollisionSample::Current,{p.x,p.y});
                if(h.player_hit&&bounds.x.contains(p.x))h.player_hit();
                beam_tone=100; // Source tests frame & 0, so the 150 branch never executes.
            }else if(f<=262){
                pattern=128;top-=4;if(top<=64)phase_frame=263;
                if(p.rank&&!(phase_frame%16))bullet(h,x+32,y+96,0,enhanced?0x27:0x25,60);
                break;
            }else {phase_frame=0;break;}
            if(h.tone)h.tone(beam_tone);
            break;
        case Attack::Orbit:case Attack::BurstOrbit: // ASM 18886–19392, 19636–19669
            move_orbit(h,p,id==Attack::BurstOrbit,x,y);break;
        case Attack::Spiral: // ASM 19399–19508
            if(f<10)break;
            if(f<70){approach(p);break;}
            if(f==70){pose(h,131,9);speed=20;break;}
            if(f<100)break;
            if(f==100){pose(h,134,10);angle=std::uint8_t(random_byte(h));break;}
            if(f<=160)--angle;
            else if(f<=240){++angle;++speed;}
            else if(enhanced&&f<=320){++angle;--speed;}
            else if(enhanced&&f<=400){--angle;++speed;}
            else {phase_frame=0;pattern=128;break;}
            if(!(f%(8-p.rank))){bullet(h,pellet_x,pellet_y,angle,p.rank?0x21:0x22,speed);sound(h,3);}
            break;
        case Attack::Symmetric: // ASM 19515–19629
            if(f<10)break;
            if(f<70){approach(p);break;}
            if(f==70){pose(h,131,9);angle=216;break;}
            if(f<100)break;
            if(f==100){pose(h,134,10);break;}
            if(f<=150){bullet(h,pellet_x,pellet_y,angle,0x23,90);angle=std::uint8_t(angle+(f<=132?3:-3));}
            else if(f>226){phase_frame=0;pattern=128;}
            else if(f%8==1){
                if(!enhanced)bullet(h,pellet_x,pellet_y,64,7,90);
                else for(unsigned i=0;i<2;++i)bullet(h,pellet_x,pellet_y,32+(random_byte(h)&63),0x20,80);
            }
            break;
        case Attack::Bounce: // ASM 19742–19901
            if(f<10)break;
            if(f<70){approach(p);break;}
            if(f==70){pose(h,131,9);break;}
            if(f<100)break;
            if(f==100){
                pose(h,134,10);angle=std::uint8_t(enemy_atan2(p.y-pellet_y,p.x-pellet_x));turn=p.x>208?1:-1;
                if(h.turns_max)h.turns_max(enhanced?1:0);
                break;
            }
            if(f>300){phase_frame=0;pattern=128;break;}
            if(!(f%8)){
                sound(h,10);
                if(p.rank){for(int a:{10,-10,30,-30})bullet(h,pellet_x,pellet_y,int(angle)+a,0x88,80,85);}
                else for(int a:{15,-15})bullet(h,pellet_x,pellet_y,int(angle)+a,0x88,80,85);
            }
            if(f>=150&&(f&1))angle=std::uint8_t(angle+(f<210?turn:-turn));
            break;
        case Attack::Fan: // ASM 19908–19997
            if(f<30)break;
            if(f<70){approach(p);break;}
            if(f==70){pose(h,131,9);break;}
            if(f<120)break;
            if(f>160){phase_frame=0;pattern=128;break;}
            if(f==120){pose(h,134,3);angle=4;}
            else if(f%10)break;else sound(h,3);
            bullet(h,pellet_x,pellet_y,angle+8,7,80);
            bullet(h,pellet_x,pellet_y,120-angle,7,80);
            angle=std::uint8_t(angle+10);
            break;
        case Attack::Satellites: // ASM 20004–20255
            if(f<40)break;
            if(f==40){pose(h,131,9);orbit_x=x+80;orbit_y=y+80;orbit_r=4;satellite_angle=std::uint8_t(random_byte(h));}
            else if(f<80)break;
            else if(f==80){pose(h,134,10);orb_coordinates(p.page,subpixel(orbit_x),subpixel(orbit_y),orbit_r,satellite_angle,true);}
            else {
                orbit_r+=2;orb_coordinates(p.page,orbit_x,orbit_y,orbit_r,satellite_angle);
                if(orbit_r>180-int(p.rank)*16&&!(f&3))for(unsigned i=0;i<4;++i){
                    sound(h,3);const auto &q=orb_position[p.page][i];
                    if(q[0]>0&&q[0]<416&&q[1]>0&&q[1]<400)bullet(h,q[0],q[1],satellite_angle+i*64+(enhanced?96:64),0x20,128);
                }
                if(orbit_r>460){for(unsigned i=0;i<4;++i)orb_flag[i]=0;phase_frame=0;pattern=128;}
            }
            satellite_angle=std::uint8_t(satellite_angle-3);
            break;
        case Attack::FinalSpin: // ASM 20262–20298
            if(f==50)angle=0;
            if(f>=150)phase_frame=0;
            else if(f>50&&(f&1)){
                sound(h,3);bullet(h,x+64,y+44,angle,0x23,50,86);angle=std::uint8_t(angle+(p.rank?6:8));
            }
            break;
        case Attack::FinalBurst: // ASM 20305–20429
            if(f<50)break;
            if(f==50){angle=std::uint8_t(enemy_atan2(p.y+12-(y+44),p.x+12-(x+64)));charge_radius=30;sound(h,9);break;}
            if(f<=80){
                if(f&1){
                    if(h.circle)for(unsigned scale:{1u,2u,4u})h.circle(x+64,y+44,unsigned(charge_radius)*scale,0);
                    charge_radius-=2;
                    if(h.circle)for(unsigned scale:{1u,2u,4u})h.circle(x+64,y+44,unsigned(charge_radius)*scale,13);
                }
                break;
            }
            if(f>=200){phase_frame=0;break;}
            if(f%8==0||(p.rank&&f%8==4)){
                bullet(h,x+64,y+44,2,0x1c,60,86);bullet(h,x+64,y+44,2,0x1c,60);
            }
            if(f%8==0)sound(h,3);
            break;
        case Attack::FinalAimed: // ASM 20436–20475
            if(f>=190)phase_frame=0;
            else if(f>=50&&!(f%16))bullet(h,x+64,y+44,0,f%32?0x11:0x17,90,86);
            break;
        }
    }
    void move_orbit(const Hooks &h,const Player &p,bool burst,int x,int y){
        const unsigned f=phase_frame;
        if(f<40)return;
        if(f==40){
            pose(h,131,9);orbit_x=(x+80)*16;orbit_y=(y+64)*16;orbit_r=4;
            const auto d=enemy_vector(enemy_atan2(p.y-(y+64),p.x-(x+80)),burst?52:48);
            orbit_dx=d[0];orbit_dy=d[1];orbit_step=-1;orbit_burst=false;
        }else if(f<80)return;
        else if(f<96){
            if(f==80)pose(h,134,10);else orbit_r+=4;
            orb_coordinates(p.page,subpixel(orbit_x),subpixel(orbit_y),orbit_r,orbit_angle,f==80);
        }else {
            orbit_x=enemy_s16(unsigned(orbit_x+orbit_dx));orbit_y=enemy_s16(unsigned(orbit_y+orbit_dy));orbit_r+=orbit_step;
            bool end=false;
            if(burst){
                if(orbit_burst)orbit_angle=std::uint8_t(orbit_angle-(enhanced?3:5));
                if(orbit_r>=400)end=true;
                else if(orbit_r<20){
                    orbit_dx=orbit_dy=0;orbit_burst=true;sound(h,3);
                    for(unsigned i=0;i<4;++i){const auto &q=orb_position[p.page][i];
                        for(int j=0;j<48;j+=12)bullet(h,q[0]+12,q[1]+12,orbit_angle+int(i)*64+8+j,0x20,35+j);
                    }
                    pattern=128;orbit_step=5;
                }
            }else if(enhanced){
                if(orbit_r==196)phase_frame=0;
                if(orbit_r<20){
                    orbit_burst=false;sound(h,3);if(h.stack_multiplier)h.stack_multiplier(0);
                    for(unsigned i=0;i<4;++i){const auto &q=orb_position[p.page][i];
                        for(int j=0;j<16;++j)bullet(h,q[0]+12,q[1]+12,orbit_angle+int(i)*64-64+j*8,0x20,35);
                        for(int j=0;j<16;++j)bullet(h,q[0]+12,q[1]+12,orbit_angle+int(i)*64-60+j*8,0x20,35-j);
                        orb_flag[i]=2;
                    }
                    if(h.stack_multiplier)h.stack_multiplier(1);
                    orbit_r=320;pattern=128;
                }
            }else if(orbit_r<32)orbit_step=2;
            else if(orbit_r>128)orbit_step=-2;
            const int cx=subpixel(orbit_x),cy=subpixel(orbit_y);
            if(end||cx+orbit_r<0||cx-orbit_r>416||cy+orbit_r< -16||cy-orbit_r>386||phase_frame>500){
                for(unsigned i=0;i<4;++i)orb_flag[i]=2;
                phase_frame=0;pattern=128;
            }
            orb_coordinates(p.page,cx,cy,orbit_r,orbit_angle);
        }
        orbit_angle=std::uint8_t(orbit_angle+5);
    }
};
}
