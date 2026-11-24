#include "replay_session.hpp"
#include "practice_session.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>

using namespace soew;
#define CHECK(...) do { if (!(__VA_ARGS__)) { std::cerr << __LINE__ << ": " #__VA_ARGS__ "\n"; std::exit(1); } } while (false)

static unsigned count(const CollisionFrame &frame,CollisionSource source){
    return unsigned(std::count_if(frame.hazards.begin(),frame.hazards.begin()+frame.hazard_count,
        [=](const auto &h){return h.source==source;}));
}

static const CollisionFrame::Hazard &hazard(const CollisionFrame &frame,CollisionSource source){
    const auto end=frame.hazards.begin()+frame.hazard_count;
    const auto found=std::find_if(frame.hazards.begin(),end,[=](const auto &h){return h.source==source;});
    CHECK(found!=end);
    return *found;
}

static void projectile_sampling(){
    CollisionFrame f;f.begin(HitboxMode::All,8,1);
    constexpr CollisionPoint player{200,300};
    constexpr auto small=bullet_hit_window(player,true),big=bullet_hit_window(player,false);
    CHECK(small.x.last()-small.x.first()+1==10&&small.y.last()-small.y.first()+1==11);
    CHECK(big.x.last()-big.x.first()+1==22&&big.y.last()-big.y.first()+1==21);
    CHECK(small.contains({207,322})&&!small.contains({217,322})&&!small.contains({207,323}));
    CHECK(big.contains({197,304})&&big.contains({218,324})&&!big.contains({219,324}));
    BulletCore b;
    const auto set=[&](unsigned slot,int x,int y){auto &s=b.bullets[slot];s.flag=1;s.position[1]={std::uint16_t(x*16),std::uint16_t(y*16)};};
    set(0,206,312);b.bullets[0].velocity={16,0}; // Hits and is reclaimed after being sampled.
    set(1,40,360);b.bullets[1].kind=EnemyBulletRequest::Bullet16;b.bullets[1].motion=0x88;b.bullets[1].velocity={0,32};
    set(2,415,60);b.bullets[2].velocity={16,0}; // Screen exit is not checked for collision.
    CHECK(b.update(1,player.x,player.y,8,&f));
    CHECK(f.bullet_count==2&&f.bullets[0].position==CollisionPoint{207,312}&&f.bullets[0].pellet);
    CHECK(b.bullets[0].flag==2&&b.bullets[2].flag==2);
    CHECK(f.bullets[1].position==CollisionPoint{40,362}&&!f.bullets[1].pellet&&b.bullets[1].velocity[1]==-32);
    CHECK(f.player[static_cast<unsigned>(CollisionSample::Projectile)]==player);
    f.begin(HitboxMode::Self,9,1);CHECK(!b.update(1,player.x,player.y,9,&f));
    CHECK(f.bullet_count==0&&f.player[static_cast<unsigned>(CollisionSample::Projectile)]==player);
    b.clear(0);f.begin(HitboxMode::All,10,0);CHECK(!b.update(0,player.x,player.y,10,&f)&&f.bullet_count==0);

    LaserCore laser;
    auto &a=laser.slots[0];a.flag=1;a.state=3;a.charge_age=4;a.active_frames=2;a.x=232;a.y=70;
    laser.slots[1]=a;laser.slots[1].state=4;laser.slots[1].charge_age=3;
    laser.slots[2]=a;laser.slots[2].state=4;laser.slots[2].active_frames=1;
    f.begin(HitboxMode::All,8,0);
    CHECK(!laser.update(8,209,70,true,{},&f)); // Already-hit still shows an active beam.
    CHECK(count(f,CollisionSource::Laser)==1);
    const auto &beam=hazard(f,CollisionSource::Laser).bounds;
    CHECK(beam.contains({209,70})&&!beam.contains({208,70})&&!beam.contains({240,70})&&!beam.contains({209,69}));
    f.begin(HitboxMode::All,9,1);CHECK(!laser.update(9,208,70,false,{},&f)&&count(f,CollisionSource::Laser)==1);
    f.begin(HitboxMode::All,10,0);CHECK(!laser.update(10,209,70,false,{},&f)&&count(f,CollisionSource::Laser)==0);
}

static void original_projectile_window(){
    // Independent oracle from the ordinary-bullet comparisons in the TH02
    // PC-98 executable (routine offsets 11BB–1213). Include both rejected
    // edges, the opposite VRAM page, and fractional post-update positions.
    constexpr CollisionPoint player{208,336};
    BulletCore bullets;
    auto &shot=bullets.bullets[0];
    for(const bool pellet:{false,true})for(const unsigned page:{0u,1u})
        for(int y=2;y<=26;++y)for(int x=-5;x<=21;++x){
            const bool expected=pellet?x>=7&&x<17&&y>=12&&y<=22:x>=-3&&x<19&&y>=4&&y<=24;
            shot.flag=1;
            shot.kind=pellet?EnemyBulletRequest::Pellet:EnemyBulletRequest::Bullet16;
            shot.velocity={3,-5};
            shot.position[page]={std::uint16_t((player.x+x)*16+12),std::uint16_t((player.y+y)*16+5)};
            CHECK(bullets.update(page,player.x,player.y)==expected);
            CHECK(shot.flag==(expected?2u:1u));
        }
}

static void vertical_projectile_contact(){
    constexpr CollisionPoint player{208,336};
    constexpr int marker_y=player.y+player_projectile_offset.y;
    for(bool pellet:{false,true})for(int dy:{1,-1}){
        const int center_offset=pellet?4:8;
        const int first_hit=dy>0?(pellet?-8:-12):(pellet?2:8);
        BulletCore bullets;
        auto &shot=bullets.bullets[0];
        shot.flag=1;
        shot.kind=pellet?EnemyBulletRequest::Pellet:EnemyBulletRequest::Bullet16;
        shot.position[0]={std::uint16_t((player.x+player_projectile_offset.x-center_offset)*16),
                          std::uint16_t((marker_y+first_hit-center_offset-2*dy)*16)};
        shot.velocity={0,dy*16};
        CHECK(!bullets.update(0,player.x,player.y)&&shot.flag==1);
        CHECK(bullets.update(0,player.x,player.y)&&shot.flag==2);
        CHECK(BulletCore::pixel(shot.position[0][1])+center_offset==marker_y+first_hit);
    }
}

static void focus_projectile_presentation(const std::filesystem::path &assets){
    constexpr CollisionPoint player{208,336};
    constexpr int cx=player.x+player_projectile_offset.x,cy=player.y+player_projectile_offset.y;
    constexpr int offset=bullet_center_offset(true);
    constexpr std::uint32_t pellet_color=0xfffdfb;
    std::vector<std::uint32_t> pixels(640*400),without(pixels.size());
    const auto at=[](int x,int y){return y*640+x;};
    for(const int speed:{2,6}){
        auto scene=EncounterScene::load(assets,0,0,1,2,3);
        scene.survival_enabled=true;
        scene.script.formations.clear();scene.script.declared_formations=0;
        scene.background.palette[15]={255,253,251};
        auto &pellet=scene.bullet_core.bullets[0];
        constexpr int first_hit=cy-8;
        const int last_visible=first_hit-speed,old=last_visible-speed;
        pellet.flag=1;pellet.velocity={0,speed*16};
        pellet.position.fill({std::uint16_t((cx-offset)*16),std::uint16_t((old-offset)*16)});
        MotionFrame previous;scene.capture_motion(previous);
        scene.tick(Focus);
        CHECK(!scene.life.hit&&pellet.flag==1&&!scene.collision_snapshot());
        for(const unsigned phase:{0u,128u,256u}){
            scene.render(pixels,true,true,phase,&previous);
            CHECK(pixels[at(cx,cy)]==0xffffff);
            CHECK(pixels[at(cx,last_visible)]==pellet_color);
            pellet.flag=0;scene.render(without,true,true,phase,&previous);pellet.flag=1;
            CHECK(without[at(cx,last_visible)]!=pellet_color);
            // The whole visible pellet, not just the center, is on the sampled frame.
            for(int y=old-offset-1;y<last_visible+offset+1;++y)
                for(int x=cx-offset-1;x<cx+offset+1;++x)
                    if(y<last_visible-offset||y>=last_visible+offset||x<cx-offset||x>=cx+offset)
                        CHECK(pixels[at(x,y)]==without[at(x,y)]);
        }
        scene.capture_motion(previous);
        scene.tick(Focus);
        CHECK(scene.life.hit&&pellet.flag==2);
        CHECK(BulletCore::pixel(pellet.position[scene.rendered_page][1])+offset==first_hit);
        scene.render(pixels,true,true,0,&previous);
        CHECK(pixels[at(cx,first_hit)]!=pellet_color); // Original removal precedes rendering, even with Shift.
        scene.tick(Focus);
        CHECK(scene.life.stage_misses==1&&pellet.flag==0);
        scene.render(pixels);
        CHECK(pixels[at(cx,first_hit)]!=pellet_color); // No extra live or rendered frame.
    }
}

static void boss_sampling(){
    CollisionFrame f;
    MeiraCore meira;MeiraCore::Hooks mh;mh.collision_display=&f;
    CHECK(meira.add_effect(190,300,0,0,0,1));
    CHECK(meira.add_effect(210,310,0,0,0,0));
    f.begin(HitboxMode::All,0,0);meira.update(0,160,240,388,mh);
    CHECK(count(f,CollisionSource::MeiraBody)==1&&count(f,CollisionSource::MeiraMarker)==1);
    const auto &marker=hazard(f,CollisionSource::MeiraMarker).bounds;
    CHECK(marker.contains({174,285})&&marker.contains({190,299})&&!marker.contains({190,300}));
    CHECK(meira.effect_draws[0].top==288); // Rolled paint does not move collision coordinates.
    f.begin(HitboxMode::All,1,1);meira.begin_frame(1);meira.update(1,160,240,388,mh);
    CHECK(count(f,CollisionSource::MeiraMarker)==0);

    MarisaCore marisa;MarisaCore::Hooks ah;ah.collision_display=&f;
    marisa.phase=MarisaCore::Phase::DeployOrbs;marisa.phase_frame=129;
    f.begin(HitboxMode::All,0,0);marisa.update(ah,{10,300,0});
    CHECK(count(f,CollisionSource::MarisaOrb)==4&&count(f,CollisionSource::MarisaBody)==1);
    const auto &orb=hazard(f,CollisionSource::MarisaOrb);
    CHECK(orb.sample==CollisionSample::Front&&orb.bounds.x.low==marisa.left+16);
    CHECK(marisa.orbs[0].x!=orb.bounds.x.low+16);
    marisa.defeated=true;f.begin(HitboxMode::All,1,1);marisa.update(ah,{10,300,0});
    CHECK(f.hazard_count==0);

    MimaCore mima;MimaCore::Hooks wh;wh.random8=[](){return 1u;};wh.collision_display=&f;
    mima.start_practice_phase(1,wh);CHECK(mima.selected_attack()==MimaCore::Attack::Beam);
    mima.phase_frame=169;
    f.begin(HitboxMode::All,169,0);mima.update(wh,{50,45,100,300});
    CHECK(count(f,CollisionSource::MimaBeam)==0);
    f.begin(HitboxMode::All,170,1);mima.update(wh,{50,45,100,300});
    const auto &wide=hazard(f,CollisionSource::MimaBeam);
    CHECK(wide.sample==CollisionSample::Current&&wide.bounds.contains({mima.left+17,16}));
    CHECK(wide.bounds.contains({mima.left+95,383})&&!wide.bounds.contains({mima.left+96,60}));
    CHECK(f.player[static_cast<unsigned>(CollisionSample::Front)]==CollisionPoint{50,45});
    CHECK(f.player[static_cast<unsigned>(CollisionSample::Current)]==CollisionPoint{100,300});
    mima.phase_frame=198;f.begin(HitboxMode::All,198,0);mima.update(wh,{50,45,100,300});
    CHECK(count(f,CollisionSource::MimaBeam)==0);
    wh.random8=[](){return 2u;};mima.start_practice_phase(1,wh);
    CHECK(mima.selected_attack()==MimaCore::Attack::Orbit);
    for(unsigned i=1;i<=80;++i){
        const unsigned page=i&1;f.begin(HitboxMode::All,i,page);
        mima.update(wh,{50,45,224,375,1,i,true,page});
        CHECK(count(f,CollisionSource::MimaOrb)==(i==80?4u:0u));
    }
    mima.phase_frame=500;f.begin(HitboxMode::All,501,1);mima.update(wh,{50,45,224,375,1,501,true,1});
    CHECK(count(f,CollisionSource::MimaOrb)==0); // Removed by the attack before collision.

    SigmaCore sigma;SigmaCore::Hooks sh;sh.collision_display=&f;
    sh.bullet=[](const auto &){};sh.damage=[](int,int,int,int){return 0u;};
    sh.score=sh.sound=sh.turns_max=[](unsigned){};sh.randring2_next=[](){return 0u;};
    sh.player_hit=[](){};sh.laser=[](int,int,int,unsigned,unsigned){};
    sigma.dying=true;
    const auto step=[&](unsigned page){f.begin(HitboxMode::All,sigma.phase_frame,page);sigma.begin_frame(page);sigma.update(page,0,sh);};
    for(unsigned i=1;i<=221;++i){
        step(i&1);
        CHECK(count(f,CollisionSource::SigmaBeam)==unsigned((i>=91&&i<=99)||(i>=101&&i<=220)));
        if(i==91){const auto &beam=hazard(f,CollisionSource::SigmaBeam).bounds;CHECK(beam.x.low==200&&beam.x.high==216&&!beam.x.low_closed&&!beam.x.high_closed);}
    }
    sigma.init();sigma.bursts[0]={1,0,26,64,128,320,64};step(0);
    CHECK(count(f,CollisionSource::SigmaBurst)==0);
    sigma.bursts[0]={2,0,26,16,128,320,64};step(0);
    const auto &burst=hazard(f,CollisionSource::SigmaBurst).bounds;
    CHECK(burst.contains({97,289})&&!burst.contains({96,289})&&!burst.contains({128,289}));
    sigma.bursts[0].age=32;step(0);CHECK(count(f,CollisionSource::SigmaBurst)==0&&sigma.bursts[0].state==4);
}

static void render_bounds(){
    constexpr std::uint32_t cyan=0x00ffff;
    constexpr CollisionPoint player{100,200},center{116,224};
    CollisionFrame f;f.begin(HitboxMode::Self,0,0);
    f.sample_player(CollisionSample::Projectile,player);
    f.sample_player(CollisionSample::Front,{95,185});
    std::vector<std::uint32_t> pixels(640*400);
    const auto px=[&](int x,int y){return pixels[y*640+x];};
    const auto clear=[&]{std::fill(pixels.begin(),pixels.end(),0);};
    const auto edge=[](int x,int y,int x1,int y1,int x2,int y2){
        return ((y==y1||y==y2)&&x>=x1&&x<=x2)||((x==x1||x==x2)&&y>=y1&&y<=y2);
    };
    draw_collision_frame(pixels,f);
    const auto self=pixels;
    for(int y=0;y<400;++y)for(int x=0;x<640;++x){
        const bool expected=CollisionPoint{x,y}==center||edge(x,y,111,216,120,226)||edge(x,y,105,212,126,232);
        CHECK(px(x,y)==(expected?cyan:0));
    }
    // Check every inside/outside position around both independent expected ranges.
    for(bool pellet:{true,false})for(int y=-14;y<=10;++y)for(int x=-13;x<=12;++x){
        const bool inside=pellet?(x>=-5&&x<=4&&y>=-8&&y<=2):(x>=-11&&x<=10&&y>=-12&&y<=8);
        const int offset=pellet?4:8;
        CHECK(bullet_hit_window(player,pellet).contains({center.x+x-offset,center.y+y-offset})==inside);
    }

    f.begin(HitboxMode::All,1,1);
    f.hazard(CollisionSource::Laser,{CollisionAxis::open(21,42),CollisionAxis::at_least(8)},CollisionSample::Projectile,{100,200});
    f.hazard(CollisionSource::MimaBeam,{CollisionAxis::open(100,117),CollisionAxis::any()},CollisionSample::Current,{102,200});
    f.hazard(CollisionSource::Road,CollisionBounds::open(20,15,44,28),CollisionSample::FrontXCurrentY,{106,218});
    f.hazard(CollisionSource::MarisaBody,CollisionBounds::open(135,240,200,305),CollisionSample::Front,{96,196});
    f.hazard(CollisionSource::MarisaOrb,CollisionBounds::open(204,249,221,266),CollisionSample::Front,{102,202});
    f.bullet({60,100},true);f.bullet({70,109},false);f.bullet({28,100},true);f.bullet({411,100},false);
    clear();draw_collision_frame(pixels,f); // No bullet check means no common reference, even with a laser.
    CHECK(std::all_of(pixels.begin(),pixels.end(),[](auto p){return p==0;}));
    f.sample_player(CollisionSample::Projectile,player);
    // Check strict and unbounded beam boundaries in both coordinate systems.
    for(int y=4;y<=12;++y)for(int x=17;x<=46;++x){
        const CollisionPoint raw{x,y};
        const CollisionPoint shown{x+16,y+24};
        CHECK(f.hazards[0].bounds.contains(raw)==(shown.x>=38&&shown.x<=57&&shown.y>=32));
    }
    for(int y:{-400,0,383,800})for(int x=96;x<=121;++x){
        const CollisionPoint raw{x,y};
        const CollisionPoint shown{x+14,y+24};
        CHECK(f.hazards[1].bounds.contains(raw)==(shown.x>=115&&shown.x<=130));
    }
    for(int y=12;y<=31;++y)for(int x=17;x<=47;++x){
        const CollisionPoint raw{x,y};
        const CollisionPoint shown{x+10,y+6};
        CHECK(f.hazards[2].bounds.contains(raw)==(shown.x>=31&&shown.x<=53&&shown.y>=22&&shown.y<=33));
    }
    draw_collision_frame(pixels,f);
    CHECK(px(38,32)==cyan&&px(57,32)==cyan&&px(47,32)==cyan&&!px(47,31));
    CHECK(px(38,383)==cyan&&px(57,383)==cyan&&!px(47,383)&&!px(38,384));
    CHECK(px(115,16)==cyan&&px(130,383)==cyan&&!px(114,16)&&!px(131,383)&&!px(120,16));
    CHECK(px(32,22)==cyan&&px(53,30)==cyan&&!px(54,30)&&!px(32,30)&&!px(32,21));
    CHECK(px(32,33)==cyan&&px(53,33)==cyan&&!px(32,34)&&!px(53,34));
    CHECK(px(156,269)==cyan&&px(219,332)==cyan&&!px(155,269)&&!px(219,333));
    CHECK(px(225,272)==cyan&&px(234,287)==cyan&&!px(225,271)&&!px(234,288));
    CHECK(px(64,104)==cyan&&px(78,117)==cyan&&px(32,104)==cyan&&!px(60,100)&&!px(70,109)&&!px(419,108));
    const auto normal=pixels;draw_collision_frame(pixels,f);CHECK(normal==pixels);

    clear();f.publish(&f,1,true);draw_collision_frame(pixels,f);
    CHECK(px(406,32)==cyan&&px(48,104)==cyan&&px(416-16,104)==cyan&&px(100,224)==cyan);
    clear();draw_collision_frame(pixels,f,true);CHECK(pixels==normal);
    clear();f.publish(&f,1,false);draw_collision_frame(pixels,f,false,-4,-8);
    CHECK(px(34,24)==cyan&&px(60,96)==cyan&&px(112,216)==cyan&&!px(32,22)&&!px(112,16));
    CHECK(px(415,100)==cyan); // A centered bullet enters view after the presentation shift.
    for(int y=0;y<400;++y)for(int x=0;x<640;++x)if(x<32||x>=416||y<16||y>=384)CHECK(!px(x,y));
    f.mode=HitboxMode::Self;clear();draw_collision_frame(pixels,f);CHECK(pixels==self);
    f.mode=HitboxMode::Off;clear();draw_collision_frame(pixels,f);
    CHECK(std::all_of(pixels.begin(),pixels.end(),[](auto p){return p==0;}));
}

static void scene_lifetime(const std::filesystem::path &assets){
    auto base=EncounterScene::load(assets,0,0,1,2,3);
    base.combat_enabled=base.survival_enabled=true;base.enable_effects(235);
    base.script.enemies[0].raw[22]=100;base.script.enemies[0].raw[23]=0;base.script.enemies[0].raw[24]=0;
    base.script.paths[base.script.enemies[0].path_slot].bytes={0,255};
    auto &enemy=base.runtime.enemies.slots[0];enemy.raw[0xe]=1;enemy.word(0xc,0);enemy.word(4,208);enemy.word(6,336);
    auto &disabled=base.runtime.enemies.slots[1];disabled=enemy;disabled.raw[0x23]=1;
    base.life.invincibility=250;
    auto all=base,self=base;all.set_hitbox_mode(HitboxMode::All);self.set_hitbox_mode(HitboxMode::Self);
    CHECK(!all.collision_snapshot());
    std::vector<std::uint32_t> plain(640*400),drawn(640*400);
    for(unsigned i=0;i<240;++i){
        const auto input=std::uint16_t(i%2?Left|Shot:Right|Shot);
        base.tick(input);all.tick(input);self.tick(input);
        CHECK(replay_state_digest(base)==replay_state_digest(all)&&replay_state_digest(base)==replay_state_digest(self));
        CHECK(base.sound_requests==all.sound_requests&&base.sound_requests==self.sound_requests);
        const auto *capture=all.collision_snapshot(),*self_capture=self.collision_snapshot();
        CHECK(capture&&self_capture&&!base.collision_snapshot());
        CHECK(capture->completed_tick==all.ticks&&capture->page==all.rendered_page&&capture->logic_frame==i);
        CHECK(self_capture->hazard_count==0&&self_capture->bullet_count==0&&self_capture->player==capture->player);
        if(i!=0)continue;
        CHECK(count(*capture,CollisionSource::Road)==1&&all.runtime.enemies.slots[0].raw[0x10]==1);
        const auto &road=hazard(*capture,CollisionSource::Road);
        CHECK(road.sample==CollisionSample::FrontXCurrentY&&road.player==CollisionPoint{208,336});
        const auto &projectile=capture->player[static_cast<unsigned>(CollisionSample::Projectile)];
        CHECK(projectile&&projectile->x>road.player.x&&projectile->y==road.player.y);
        const CollisionPoint center{projectile->x+16,projectile->y+24};
        CHECK(all.life.stage_misses==0); // Invincibility did not hide the contact box.
        base.render(plain);drawn=plain;base.render_hitboxes(drawn);CHECK(plain==drawn);
        all.render(drawn);CHECK(plain==drawn);all.render_hitboxes(drawn);CHECK(plain!=drawn);
        CHECK(drawn[center.y*640+center.x]==0x00ffff);
        CHECK(drawn[(road.bounds.y.first()+24)*640+(road.bounds.x.first()+center.x-road.player.x)]==0x00ffff);
        auto copy=all;CHECK(!copy.collision_snapshot()); // Scene identity is not copied.
        all.rendered_page^=1;CHECK(!all.collision_snapshot());all.rendered_page^=1;
        all.paused=true;all.tick(0);CHECK(all.collision_snapshot()==capture);all.paused=false;
    }
    all.set_hitbox_mode(HitboxMode::Off);CHECK(!all.collision_snapshot());
    all.set_hitbox_mode(HitboxMode::All);CHECK(!all.collision_snapshot());all.tick(0);CHECK(all.collision_snapshot());
    all.dialogue.phase=StageDialogue::Phase::Text;CHECK(!all.collision_snapshot());
    all.dialogue=StageDialogue{};all.runtime.blocked=EnemyPathRuntime::Block{};all.tick(0);CHECK(!all.collision_snapshot());
}

static void player_reference_lifetime(const std::filesystem::path &assets){
    auto off=EncounterScene::load(assets,0,0,1,2,3);
    off.combat_enabled=off.survival_enabled=true;off.life.invincibility=4;
    off.script.formations.clear();off.script.declared_formations=0;
    auto self=off,all=off;self.set_hitbox_mode(HitboxMode::Self);all.set_hitbox_mode(HitboxMode::All);
    std::vector<std::uint32_t> pixels(640*400);
    constexpr std::uint32_t cyan=0x00ffff;
    const auto draw=[&](const EncounterScene &scene,int x,int y){
        std::fill(pixels.begin(),pixels.end(),0u);scene.render_hitboxes(pixels);return pixels[y*640+x];
    };
    const auto step=[&](std::uint16_t input){
        off.tick(input);self.tick(input);all.tick(input);
        CHECK(replay_state_digest(off)==replay_state_digest(self)&&replay_state_digest(off)==replay_state_digest(all));
    };
    step(0);
    CHECK(!all.life.pattern&&all.life.invincibility==3&&all.collision_snapshot());
    CHECK(draw(all,224,360)==cyan); // Blinking is not a change of reference.

    for(auto *scene:{&off,&self,&all}){
        scene->life.invincibility=0;
        auto &b=scene->bullet_core.bullets[0];b.flag=1;
        b.position={{{218*16,351*16},{218*16,351*16}}};
    }
    step(0);
    CHECK(all.life.hit&&!all.life.miss_active&&all.bullet_core.bullets[0].flag==2);
    CHECK(all.collision_snapshot()&&all.collision_snapshot()->bullet_count==1);
    CHECK(draw(all,222,355)==cyan&&!draw(self,222,355)); // Hit and reclaimed on this same frame.
    for(unsigned frame=1;frame<=68;++frame){
        step(0);
        CHECK(all.life.miss_frame==frame%68&&!all.collision_snapshot()&&!self.collision_snapshot());
        draw(all,224,360);
        CHECK(std::all_of(pixels.begin(),pixels.end(),[](auto p){return p==0;}));
    }
    CHECK(all.life.stage_misses==1&&!all.life.miss_active);
    step(Right);
    CHECK(all.collision_snapshot()&&self.collision_snapshot());
    const auto &sample=all.collision_snapshot()->player[static_cast<unsigned>(CollisionSample::Projectile)];
    CHECK(sample==CollisionPoint{all.life.current.x,all.life.current.y});
    CHECK(draw(all,sample->x+16,sample->y+24)==cyan);
}

static void marisa_practice_overlay(const std::filesystem::path &assets){
    PracticeSpec spec;spec.stage=3;
    PracticeBuilder builder(assets,spec);
    CHECK(builder.status()==PracticeBuilder::Status::Ready);
    auto scene=builder.release();
    scene->set_hitbox_mode(HitboxMode::All);
    const auto set_bullet=[&](unsigned slot,int x,int y){
        auto &b=scene->bullet_core.bullets[slot];b.flag=1;b.kind=EnemyBulletRequest::Bullet16;
        b.position={{{std::uint16_t(x*16),std::uint16_t(y*16)},{std::uint16_t(x*16),std::uint16_t(y*16)}}};
    };
    set_bullet(0,50,299);set_bullet(1,27,277);set_bullet(2,414,260);
    scene->tick(Right);
    const auto *f=scene->collision_snapshot();CHECK(f);
    CHECK(hazard(*f,CollisionSource::MarisaBody).player==CollisionPoint{208,336});
    CHECK(f->player[static_cast<unsigned>(CollisionSample::Projectile)]==CollisionPoint{212,336});
    std::vector<std::uint32_t> pixels(640*400);
    constexpr std::uint32_t cyan=0x00ffff;
    const auto px=[&](int x,int y){return pixels[y*640+x];};
    scene->render_hitboxes(pixels);
    CHECK(px(228,360)==cyan&&px(223,352)==cyan&&px(232,362)==cyan);
    CHECK(px(217,348)==cyan&&px(238,368)==cyan&&px(58,307)==cyan&&!px(50,299));
    CHECK(px(35,285)==cyan&&!px(422,268));
    CHECK(px(181,73)==cyan&&px(243,135)==cyan&&!px(180,73)&&!px(181,72));
    CHECK(!px(212,336)&&!px(208,336));

    scene->marisa.defeated=true;scene->marisa.death_frame=33;
    scene->tick(0);
    CHECK(scene->collision_snapshot());
    std::fill(pixels.begin(),pixels.end(),0);scene->render_hitboxes(pixels);
    CHECK(px(212,360)==cyan&&px(42,307)==cyan&&!px(228,360)&&!px(58,307));
    CHECK(!px(403,285)&&!px(19,285)&&px(406,268)==cyan); // Translation and clipping, not Sigma's rotation.
    scene->reduced_effects=true; // Same completed tick and collision sample.
    std::fill(pixels.begin(),pixels.end(),0);scene->render_hitboxes(pixels);
    CHECK(px(228,360)==cyan&&px(58,307)==cyan&&!px(212,360)&&!px(42,307));
    CHECK(px(35,285)==cyan&&!px(406,268)&&!px(422,268));
}

static void marisa_contact_edges(const std::filesystem::path &assets){
    PracticeSpec spec;spec.stage=3;
    for(bool locked:{false,true})for(bool outside:{false,true})for(bool protected_player:{false,true}){
        PracticeBuilder builder(assets,spec);CHECK(builder.status()==PracticeBuilder::Status::Ready);
        auto s=builder.release();s->set_hitbox_mode(HitboxMode::All);
        s->marisa.body_damage_enabled=!locked;
        const int x=s->marisa.left+16,y=s->marisa.top+47+int(outside);
        s->player.x=x;s->player.y=y;s->life.draw=s->life.current={x,y};
        s->life.positions={s->life.current,s->life.current};
        s->life.invincibility=protected_player?1:0;
        s->tick(0);
        if(outside||protected_player){
            CHECK(!s->life.stage_misses&&s->collision_snapshot());
            const auto &body=hazard(*s->collision_snapshot(),CollisionSource::MarisaBody);
            CHECK(body.bounds.contains(body.player)==!outside);
            if(outside){
                const auto small=bullet_hit_window({x,y},true);
                CHECK(small.y.first()+4<=body.bounds.y.last()+24&&y+24>body.bounds.y.last()+24);
            }
        }else CHECK(s->life.stage_misses==1&&s->life.miss_active&&!s->collision_snapshot());
    }
}

static void dense_pool_timings(){
    using Clock=std::chrono::steady_clock;
    constexpr unsigned steps=20000,draws=4000;
    BulletCore seed;
    for(unsigned i=0;i<seed.bullets.size();++i){
        auto &s=seed.bullets[i];s.flag=1;
        s.position={{{std::uint16_t((40+(i%25)*14)*16),std::uint16_t((32+(i/25)*46)*16)}}};
        s.kind=i%2?EnemyBulletRequest::Pellet:EnemyBulletRequest::Bullet16;
    }
    LaserCore laser_seed;
    for(unsigned i=0;i<laser_seed.slots.size();++i){
        auto &s=laser_seed.slots[i];s.flag=1;s.state=4;s.charge_age=4;s.active_frames=int(steps)+1;
        s.x=48+int(i)*31;s.y=32+int(i)*4;
    }
    for(auto mode:{HitboxMode::Off,HitboxMode::Self,HitboxMode::All}){
        auto bullets=seed;auto lasers=laser_seed;
        CollisionFrame frame;auto *capture=mode==HitboxMode::Off?nullptr:&frame;
        const auto start=Clock::now();
        for(unsigned i=0;i<steps;++i){
            if(capture)capture->begin(mode,i,0);
            bullets.update(0,208,336,i,capture);lasers.update(i,208,336,true,{},capture);
        }
        const auto captured=Clock::now();
        CHECK(bullets.removed==0&&lasers.slots[0].active_frames==1);
        if(mode==HitboxMode::All)CHECK(frame.bullet_count==150&&frame.hazard_count==12);
        std::vector<std::uint32_t> pixels(640*400);
        const auto draw_start=Clock::now();
        for(unsigned i=0;i<draws;++i){frame.shake=(i&1)!=0;draw_collision_frame(pixels,frame);}
        const auto drawing=Clock::now()-draw_start;
        CHECK(bool(pixels[360*640+208])==(mode!=HitboxMode::Off));
        const auto micros=[](Clock::duration elapsed,unsigned count){return std::chrono::duration<double,std::micro>(elapsed).count()/count;};
        std::cout<<"Dense pool mode "<<int(mode)<<": "<<micros(captured-start,steps)<<" us/update, "<<micros(drawing,draws)<<" us/overlay\n";
    }
}

static void v2_playback(const std::filesystem::path &assets,const std::filesystem::path &path){
    const auto run=load_native_replay(path);
    CHECK(run.summary&&!run.events.empty());
    for(auto mode:{HitboxMode::Off,HitboxMode::Self,HitboxMode::All}){
        auto scene=load_replay_scene(assets,run.config);scene.set_hitbox_mode(mode);
        ReplaySession playback(run.config);
        std::size_t cursor=0;
        while(cursor<run.events.size()){
            const auto previous=cursor;
            const auto step=playback.step_playback(scene,run,cursor);
            if(step.status==ReplayStepStatus::Failed)std::cerr<<step.error<<'\n';
            CHECK(step.status!=ReplayStepStatus::Failed&&cursor>previous);
        }
        CHECK(playback.summary()==run.summary);
    }
    std::cout<<"Prior v2 replay matched in all hitbox modes ("<<run.summary->ticks<<" ticks)\n";
}

int main(int argc,char **argv){
    CHECK(argc>=2&&argc<=4);
    projectile_sampling();original_projectile_window();vertical_projectile_contact();focus_projectile_presentation(argv[1]);
    boss_sampling();render_bounds();scene_lifetime(argv[1]);player_reference_lifetime(argv[1]);
    marisa_practice_overlay(argv[1]);marisa_contact_edges(argv[1]);
    if(argc==3){CHECK(std::string_view(argv[2])=="--timing");dense_pool_timings();}
    if(argc==4){CHECK(std::string_view(argv[2])=="--replay");v2_playback(argv[1],argv[3]);}
    std::cout<<"Collision display contract passed\n";
}
