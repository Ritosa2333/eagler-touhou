#include "practice_session.hpp"
#include <iostream>
#include <source_location>

namespace {
unsigned checks=0;
void check(bool value,const std::source_location at=std::source_location::current()){
    ++checks;
    if(!value)throw std::runtime_error("Direct practice check="+std::to_string(checks)+" line="+std::to_string(at.line()));
}
std::unique_ptr<soew::EncounterScene> build(const std::filesystem::path &root,const soew::PracticeSpec &spec,
                                          std::optional<soew::PracticeCheckpoint> checkpoint=std::nullopt){
    soew::PracticeBuilder builder(root,spec,checkpoint);
    if(builder.status()!=soew::PracticeBuilder::Status::Ready)throw std::runtime_error(builder.error());
    check(builder.prepared_ticks()==0);
    check(builder.advance(0)==soew::PracticeBuilder::Status::Ready);
    check(builder.advance(50000)==soew::PracticeBuilder::Status::Ready&&builder.prepared_ticks()==0);
    auto s=builder.release();
    check(s&&s->ticks==0&&s->resident_frame==0&&s->runtime.ticks==0&&s->rika_stage_frame==0);
    check(s->runtime.page==0&&!s->runtime.frame_prepared);
    check(!s->preparation_only&&s->practice_mode);
    check(!s->runtime.blocked&&!s->weapon_blocked&&s->transition_error.empty());
    check(s->runtime.spawns==0&&s->runtime.enemies.high_water==0&&s->track_tiles.empty());
    for(const auto &enemy:s->runtime.enemies.slots)for(const auto byte:enemy.raw)check(byte==0);
    for(const auto &bullet:s->bullet_core.bullets)check(!bullet.flag);
    for(const auto &item:s->items.slots)check(!item.flag);
    for(const auto &number:s->numbers.slots)check(!number.flag);
    for(const auto &spark:s->sparks.slots)check(!spark.flag);
    for(const auto &shot:s->shots.slots)check(!shot.flag);
    check(s->life.effects.empty()&&s->kills_recorded.empty()&&s->bomb.sounds.empty());
    check(s->life.lives==spec.lives&&s->life.bombs==spec.bombs&&s->life.start_bombs==spec.respawn_bombs);
    check(s->run_start_lives==unsigned(std::min(spec.lives,4))&&s->run_start_bombs==unsigned(spec.respawn_bombs));
    check(s->life.playperf==spec.dynamic_rank&&s->bullet_core.playperf==spec.dynamic_rank);
    check(s->life.power==spec.power&&s->pickups.power==unsigned(spec.power));
    check(!s->life.hit&&!s->life.miss_active&&!s->life.game_over&&s->life.invincibility==0);
    check(s->runtime.scroll.attempted_pixels==0&&s->runtime.scroll.pending_progress==(checkpoint&&checkpoint->dispatch_on_first_tick));
    return s;
}
void scroll_check(const soew::EncounterScene &s,unsigned p,bool boss=false){
    const auto &v=s.runtime.scroll;
    check(v.progress==p&&v.done==boss);
    check(v.top_row==23+(p+1)/2);
    check(v.tile_line==((p&1)?8:0));
    check(v.line==int((400-(p%50)*8)%400));
    check(v.display_pixels==p*8); // Rendering coordinate, never elapsed time.
    check(v.speed==1&&v.interval==4);
    std::size_t cursor=0;
    if(p)while(cursor<s.script.formations.size()&&(v.pending_progress?s.script.formations[cursor].trigger<p:s.script.formations[cursor].trigger<=p))++cursor;
    check(s.runtime.dispatch.cursor==cursor&&s.encounter_observer.cursor==cursor);
    check(s.scroll.progress==v.progress&&s.scroll.line==v.line&&s.scroll.top_row==v.top_row);
}
void first_attack(soew::EncounterScene &s,unsigned phase){
    if(s.stage==0){
        unsigned rings=0,lasers=0;
        soew::RikaCore::Hooks h;
        h.bullet=[&](const auto &){++rings;};
        h.laser=[&](int,int,int,unsigned,unsigned){++lasers;};
        if(phase==0){
            for(unsigned i=0;i<3;++i)s.rika.update(i&1,i,h);
            check(rings==0);s.rika.update(1,3,h);check(rings==1);
        }else if(phase==1){
            s.rika.update(0,0,h);check(rings==4&&s.rika.state==0);
        }else{
            s.rika.update(0,0,h);check(s.rika.state==1&&s.rika.attack_frame==1&&s.rika.sweep_angle==0&&lasers==0);
            for(unsigned i=1;i<99;++i)s.rika.update(i&1,i,h);
            check(lasers==0);s.rika.update(1,99,h);check(lasers==2);
        }
    }else{
        unsigned bullets=0;
        soew::MeiraCore::Hooks h;
        h.bullet=[&](const auto &){++bullets;};h.random8=[](){return 17u;};
        h.random16=[](){return 123u;};
        if(phase==0){
            for(unsigned i=0;i<10;++i)s.meira.update(i&1,208,336,unsigned(s.scroll.line),h);
            check(s.meira.pattern==142&&s.meira.attack_index==0&&bullets==0);
        }else if(phase==1){
            for(unsigned i=0;i<100;++i)s.meira.update(i&1,208,336,unsigned(s.scroll.line),h);
            check(bullets==0);
            s.meira.update(0,208,336,unsigned(s.scroll.line),h);check(bullets==1);
        }else{
            for(unsigned i=0;i<98;++i)s.meira.update(i&1,208,336,unsigned(s.scroll.line),h);
            check(bullets==0&&s.meira.special_turns_max==5);
            s.meira.update(0,208,336,unsigned(s.scroll.line),h);
            check(bullets==3&&s.meira.clones_enabled&&s.meira.attack_index==1);
        }
    }
}
}
int main(int argc,char **argv){
    try{
        if(argc!=2)throw std::runtime_error("Usage: test_practice_direct <assets>");
        const std::filesystem::path root=argv[1];
        for(int stage=0;stage<2;++stage){
            soew::PracticeSpec spec;spec.stage=stage;spec.section=soew::PracticeSection::StageStart;
            spec.lives=5;spec.bombs=5;spec.respawn_bombs=1;spec.dynamic_rank=-6;spec.seed=0x629ea175;
            const auto fresh=build(root,spec);
            scroll_check(*fresh,0);
            for(const auto &entry:soew::practice_chapters){
                if(entry.stage!=stage)continue;
                spec.section=soew::PracticeSection::Chapter;spec.chapter_id=entry.id;
                auto s=build(root,spec,soew::practice_checkpoint(spec));
                scroll_check(*s,entry.progress);
                const auto &scroll=s->runtime.scroll;
                check(scroll.cycle==(scroll.pending_progress?std::uint8_t((entry.progress*8/scroll.speed-1)*scroll.interval+1):0));
                check(s->runtime.random.bytes==fresh->runtime.random.bytes&&s->runtime.random.cursor==fresh->runtime.random.cursor);
                check(s->midboss.phase_frame==0&&s->midboss2.phase_frame==0);
                check(stage==0?s->midboss.present==(entry.progress<=116):s->midboss2.present==(entry.progress<=80));
                std::vector<std::uint32_t> image(640*400);s->render(image);
            }
            // The published chapters are even; exercise the original odd half-row contract too.
            spec.section=soew::PracticeSection::Chapter;spec.chapter_id=99;
            soew::PracticeCheckpoint odd;odd.stage=stage;odd.chapter_id=99;odd.progress=59;
            auto half=build(root,spec,odd);scroll_check(*half,59);
            std::vector<std::uint32_t> image(640*400);half->render(image);
            spec.section=soew::PracticeSection::Midboss;spec.chapter_id=-1;
            auto mid=build(root,spec);scroll_check(*mid,stage==0?116:80);
            check(mid->runtime.dispatch.cursor==unsigned(stage==0?34:32));
            check(stage==0?!mid->midboss.active&&mid->midboss.present:!mid->midboss2.active&&mid->midboss2.present);
            check(mid->runtime.random.bytes==fresh->runtime.random.bytes&&mid->runtime.random.cursor==fresh->runtime.random.cursor);
            mid->tick(0);check(mid->ticks==1&&mid->runtime.spawns==0);
            check(stage==0?mid->midboss.active&&mid->midboss.phase_frame==1:mid->midboss2.active&&mid->midboss2.phase_frame==1);
            spec.section=soew::PracticeSection::Boss;spec.dialogue=soew::PracticeDialogue::Show;
            auto shown=build(root,spec);scroll_check(*shown,unsigned(stage==0?272:288),true);
            check(shown->preboss_ready&&!shown->preboss_started&&!shown->dialogue.active());
            shown->tick(0);check(shown->preboss_started&&shown->dialogue.active()&&shown->ticks==0);
            spec.dialogue=soew::PracticeDialogue::Skip;
            for(int rank=0;rank<4;++rank)for(int shot=0;shot<3;++shot)for(int selected=0;selected<=3;++selected){
                spec.rank=rank;spec.shot=shot;spec.boss_phase=selected;
                auto s=build(root,spec);scroll_check(*s,unsigned(stage==0?272:288),true);
                const unsigned phase=selected>0?unsigned(selected-1):0;
                check(s->runtime.scroll.cycle==255);
                check(s->runtime.random.bytes==fresh->runtime.random.bytes&&s->runtime.random.cursor==fresh->runtime.random.cursor);
                if(stage==0){
                    check(s->rika_enabled&&s->rika.damage_taken==phase*700);
                    check(s->rika.state==0&&s->rika.phase_frame==0&&s->rika.cached_x==192&&s->rika.sweep_angle==0);
                    check(s->rika.render_y==(s->runtime.scroll.line+48)%400);
                }else{
                    constexpr std::array<unsigned,3> damage{0,701,1501};
                    check(s->meira_enabled&&s->meira.damage_taken==damage[phase]);
                    check(s->meira.state==phase&&s->meira.phase_frame==0&&s->meira.attack_index==0);
                    check(s->meira.origin_x==192&&s->meira.origin_y==48&&s->meira.destination_x==192&&s->meira.destination_y==48);
                    check(!s->meira.clones_enabled&&s->meira.special_turns_max==2);
                    for(const auto &page:s->meira.clones)for(const auto &point:page)check(point.left==192&&point.top==48);
                    for(const auto &effect:s->meira.effects)check(!effect.flag&&!effect.delay&&!effect.age);
                }
                if(rank==1&&shot==0)first_attack(*s,phase);
            }
            // Current five-bomb practice inventory must not become the death baseline.
            auto life=fresh->life;life.register_hit();
            for(unsigned i=0;i<43;++i){life.begin_frame(i&1);life.player_phase_end(i&1,0);}
            check(life.lives==4&&life.bombs==1&&life.start_bombs==1);
        }
        std::cout<<"Direct practice checks="<<checks<<'\n';return 0;
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
}
