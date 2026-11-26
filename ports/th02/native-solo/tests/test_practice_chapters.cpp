#include "practice_session.hpp"
#include <chrono>
#include <iostream>
#include <source_location>

unsigned checks=0;
void check(bool value,const std::source_location at=std::source_location::current()){
    ++checks;
    if(!value)throw std::runtime_error("chapter check="+std::to_string(checks)+" line="+std::to_string(at.line()));
}
void check_clean_start(const soew::EncounterScene &s,const soew::PracticeSpec &spec){
    check(!s.preparation_only&&!s.life.invincibility);
    check(s.life.playperf==spec.dynamic_rank&&s.bullet_core.playperf==spec.dynamic_rank);
    if(spec.stage==0){
        check(!s.midboss.active);
        check(s.midboss.present==(s.runtime.scroll.progress<=116));
        check(!s.midboss.visible&&s.midboss.target_x==-1&&s.midboss.target_y==-1);
    }else if(spec.stage==1){
        check(!s.midboss2.active);
        check(s.midboss2.present==(s.runtime.scroll.progress<=80));
        check(!s.midboss2.visible&&s.midboss2.target_x==-1&&s.midboss2.target_y==-1);
    }
    check(s.runtime.enemies.high_water==0);
    for(const auto &enemy:s.runtime.enemies.slots)for(auto byte:enemy.raw)check(byte==0);
    for(const auto &bullet:s.bullet_core.bullets){check(!bullet.flag);for(const auto &page:bullet.position)for(auto v:page)check(v==0);}
    for(const auto &item:s.items.slots){check(!item.flag);for(auto v:item.left)check(v==0);for(auto v:item.top)check(v==0);}
    for(const auto &spark:s.sparks.slots){check(!spark.flag);for(const auto &page:spark.position)for(auto v:page)check(v==0);}
    for(const auto &number:s.numbers.slots)check(!number.flag);
    for(const auto &shot:s.shots.slots)check(!shot.flag);
    for(const auto &pattern:s.death_patterns)check(!pattern);
    check(s.shots.target_x==-1&&s.shots.target_y==-1&&s.shots.last_target_x==-1);
    check(s.kills_recorded.empty()&&s.life.effects.empty());
    check(s.bomb.spark_requests.empty()&&s.bomb.sounds.empty()&&s.pickups.sounds.empty());
    check(s.consumed_life_effects==0&&s.consumed_bomb_sparks==0);
    check(!s.sound_output&&s.sound.current==255&&s.sound.frame==0);
}
int main(int argc,char **argv){
    try{
        if(argc!=2)return 2;
        for(const auto &entry:soew::practice_chapters){
            soew::PracticeSpec spec;spec.stage=entry.stage;spec.section=soew::PracticeSection::Chapter;spec.chapter_id=entry.id;
            spec.dynamic_rank=entry.stage==0?-6:4;spec.rank=entry.stage==5?4:1;
            const auto point=soew::practice_checkpoint(spec);
            const auto begin=std::chrono::steady_clock::now();
            soew::PracticeBuilder builder(argv[1],spec,point);
            while(builder.status()==soew::PracticeBuilder::Status::Preparing)builder.advance();
            if(builder.status()!=soew::PracticeBuilder::Status::Ready)throw std::runtime_error(std::to_string(entry.stage)+" "+entry.name+" "+builder.error());
            auto s=builder.release();
            check(s->runtime.scroll.progress==entry.progress);
            if(point->formation_cursor)check(s->runtime.dispatch.cursor==*point->formation_cursor);
            check_clean_start(*s,spec);
            if(point->dispatch_on_first_tick){
                soew::PracticeBuilder legacy(argv[1],spec,{},spec.seed,false,soew::Language::Japanese,soew::PracticeStartRule::LegacyV4);
                check(legacy.status()==soew::PracticeBuilder::Status::Ready);
                auto old=legacy.release();
                check(old->runtime.dispatch.cursor==s->runtime.dispatch.cursor+1&&!old->runtime.scroll.pending_progress);
                check(s->script.formations[s->runtime.dispatch.cursor].trigger==s->runtime.scroll.progress);
                check(s->runtime.scroll.pending_progress);
                const auto x=s->player.x;
                s->tick(soew::Left);
                check(s->player.x<x&&s->runtime.spawns>0);
                unsigned first=0;
                for(unsigned i=1;i<=96;++i){
                    if(s->bullet_core.spawned){first=i;break;}
                    s->tick(0);
                }
                check(first>1&&first<=96);
                std::cout<<"closing_stage="<<entry.stage+1<<" first_spawn_frame="<<first<<" bullets="<<s->bullet_core.spawned<<'\n';
                soew::PracticeBuilder reset(argv[1],spec,point);
                check(reset.status()==soew::PracticeBuilder::Status::Ready);
                s=reset.release();
            }
            const auto prepared_spawns=s->runtime.spawns;
            bool crossed_boundary=false,saw_dialogue=false,saw_live_enemy=false;
            const auto boss_bound=[&]{return s->rika_enabled||s->meira_enabled||s->stones_enabled||s->marisa_enabled||s->mima_enabled||s->sigma_enabled;};
            soew::PracticeSession session(spec,point);
            for(unsigned i=0;i<50000&&!boss_bound();++i){
                s->life.invincibility=200;
                std::uint16_t input=0;bool group_key=false;
                if(s->dialogue.active()){
                    saw_dialogue=true;
                    // Physical release/press samples acknowledge every key gate.
                    const bool press=s->dialogue.phase!=soew::StageDialogue::Phase::KeyDelay||s->dialogue.key_delay.waiting_press;
                    input=press?soew::OK:0;group_key=press;
                }
                session.tick(*s,input,group_key);
                if(session.result()!=soew::PracticeSession::Result::Running)throw std::runtime_error(std::to_string(entry.stage)+" "+entry.name+" stopped before boss: "+session.error());
                if(entry.end&&s->runtime.scroll.progress>=*entry.end)crossed_boundary=true;
                if(entry.progress>unsigned(entry.stage==0?116:80))check(!(entry.stage==0?s->midboss.active:s->midboss2.active));
                for(const auto &enemy:s->runtime.enemies.slots)if(enemy.raw[0xe]==1)saw_live_enemy=true;
            }
            check(boss_bound());
            check(saw_dialogue&&s->preboss_started);
            check(!entry.end||crossed_boundary);
            check(saw_live_enemy&&s->runtime.spawns>prepared_spawns);
            check(session.result()==soew::PracticeSession::Result::Running);
            check(!s->dialogue.postboss&&s->transition.phase==soew::StageTransition::Phase::Battle);
            // A completed main boss stops before post-boss dialogue/bonus.
            switch(entry.stage){
            case 0:s->rika.done=true;break;
            case 1:s->meira.done=true;break;
            case 2:s->stones.phase=soew::StonesCore::Phase::Complete;break;
            case 3:s->marisa.cleared=true;break;
            case 4:s->mima.event=soew::MimaCore::Event::FinalDefeated;break;
            case 5:s->sigma.done=true;break;
            }
            const auto ticks=s->ticks;const auto score=s->score_core.sound8;
            session.tick(*s,0);
            check(session.result()==soew::PracticeSession::Result::Success&&s->ticks==ticks);
            check(!s->dialogue.postboss&&s->score_core.sound8==score);
            session.tick(*s,soew::Shot);check(s->ticks==ticks);
            std::cout<<"stage="<<entry.stage+1<<" target="<<entry.name<<" progress="<<entry.progress
                     <<" route_ms="<<std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-begin).count()<<'\n';
        }
        std::cout<<"practice_chapter_checks="<<checks<<" failures=0\n";return 0;
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
}
