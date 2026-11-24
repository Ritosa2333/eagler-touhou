#include "practice_session.hpp"
#include <iostream>
#include <source_location>

namespace {
unsigned checks=0;
void check(bool value,const std::source_location at=std::source_location::current()){
    ++checks;
    if(!value)throw std::runtime_error("Practice session check="+std::to_string(checks)+" line="+std::to_string(at.line()));
}
struct Digest {
    std::uint64_t value=1469598103934665603ULL;
    template<class T> void add(T word){
        const auto bits=static_cast<std::uint64_t>(word);
        for(unsigned i=0;i<8;++i){value^=(bits>>(i*8))&255;value*=1099511628211ULL;}
    }
};
std::uint64_t digest(const soew::EncounterScene &s){
    Digest h;
    h.add(s.ticks);h.add(s.resident_frame);h.add(s.runtime.ticks);h.add(s.runtime.page);
    h.add(s.runtime.scroll.progress);h.add(s.runtime.scroll.line);h.add(s.runtime.scroll.cycle);
    h.add(s.runtime.scroll.display_pixels);h.add(s.runtime.scroll.pending_progress);
    h.add(s.runtime.dispatch.cursor);h.add(s.runtime.dispatch.accepted);h.add(s.runtime.spawns);
    h.add(s.runtime.random.cursor);for(auto b:s.runtime.random.bytes)h.add(b);
    for(const auto &e:s.runtime.enemies.slots)for(auto b:e.raw)h.add(b);
    for(const auto &b:s.bullet_core.bullets){h.add(b.flag);h.add(b.kind);h.add(b.pattern);for(const auto &p:b.position)for(auto v:p)h.add(v);}
    for(const auto &i:s.items.slots){h.add(i.flag);h.add(i.type);for(auto v:i.left)h.add(v);for(auto v:i.top)h.add(v);}
    h.add(s.player.x);h.add(s.player.y);h.add(s.life.current.x);h.add(s.life.current.y);
    for(const auto &p:s.life.positions){h.add(p.x);h.add(p.y);}
    h.add(s.life.lives);h.add(s.life.bombs);h.add(s.life.power);h.add(s.life.invincibility);
    h.add(s.pickups.power);h.add(s.midboss.phase_frame);h.add(s.midboss2.phase_frame);
    for(auto v:s.midboss.left)h.add(v);for(auto v:s.midboss.top)h.add(v);
    for(auto v:s.midboss2.left)h.add(v);for(auto v:s.midboss2.top)h.add(v);
    h.add(s.rika_enabled);h.add(s.meira_enabled);h.add(s.rika_stage_frame);
    h.add(s.rika_handoff_context.phase_frame);h.add(s.rika_handoff_context.scroll_line);
    h.add(s.meira_handoff.phase_frame);h.add(s.appearance_frame);
    h.add(s.dialogue.current);h.add(s.dialogue.updates);h.add(s.dialogue.phase);
    for(const auto &[key,tile]:s.track_tiles){h.add(key);h.add(tile);}
    return h.value;
}
std::unique_ptr<soew::EncounterScene> build(const std::filesystem::path &root,soew::PracticeSpec spec,unsigned budget,bool reduced=false){
    soew::PracticeBuilder builder(root,spec,std::nullopt,std::nullopt,reduced);
    while(builder.status()==soew::PracticeBuilder::Status::Preparing){
        const auto before=builder.prepared_ticks();builder.advance(budget);
        check(builder.prepared_ticks()-before<=budget);
    }
    if(builder.status()!=soew::PracticeBuilder::Status::Ready)throw std::runtime_error(builder.error());
    auto scene=builder.release();
    check(scene&&!scene->preparation_only&&scene->practice_mode);
    check(scene->practice_show_dialogue==(spec.dialogue==soew::PracticeDialogue::Show));
    check(scene->transition.opening_caption==(spec.section==soew::PracticeSection::StageStart));
    check(scene->life.power==spec.power&&scene->pickups.power==unsigned(spec.power));
    check(scene->life.lives==spec.lives&&scene->lives==spec.lives);
    check(scene->life.bombs==spec.bombs&&scene->bombs==spec.bombs);
    check(!scene->life.hit&&!scene->life.miss_active&&!scene->life.game_over&&scene->life.invincibility==0);
    check(!scene->sound_output&&scene->sound.current==255&&scene->sound.frame==0);
    check(scene->consumed_shot_sounds==scene->shots.sound_requests);
    check(scene->consumed_bomb_sounds==scene->bomb.sounds.size());
    check(scene->consumed_pickup_sounds==scene->pickups.sounds.size());
    if(spec.section==soew::PracticeSection::Midboss){
        if(spec.stage<2)check(scene->runtime.dispatch.cursor==unsigned(spec.stage==0?34:32));
        check(scene->encounter_observer.cursor==scene->runtime.dispatch.cursor);
        check(scene->life.playperf==spec.dynamic_rank&&scene->bullet_core.playperf==spec.dynamic_rank);
        for(const auto &enemy:scene->runtime.enemies.slots)for(auto byte:enemy.raw)check(byte==0);
        for(const auto &bullet:scene->bullet_core.bullets){check(!bullet.flag);for(const auto &page:bullet.position)for(auto v:page)check(v==0);}
        for(const auto &item:scene->items.slots)check(!item.flag);
        for(const auto &spark:scene->sparks.slots)check(!spark.flag);
        for(const auto &number:scene->numbers.slots)check(!number.flag);
        for(const auto &shot:scene->shots.slots)check(!shot.flag);
        check(scene->shots.target_x==-1&&scene->shots.target_y==-1&&scene->shots.last_target_x==-1);
        check(scene->kills_recorded.empty()&&scene->life.effects.empty()&&scene->bomb.spark_requests.empty());
    }
    return scene;
}
void test_assists(const std::filesystem::path &root){
    using namespace soew;
    using Tool=PracticeTool;
    PracticeAssist ui;
    check(PracticeAssist::row_at(39,24)==-1&&PracticeAssist::row_at(40,24)==0);
    check(PracticeAssist::row_at(247,185)==8&&PracticeAssist::row_at(248,185)==-1);
    ui.toggle_row(8);check(ui.info&&ui.tools==0);
    for(int i=0;i<PracticeAssist::tool_rows;++i)ui.toggle_row(i);
    check(ui.tools==255);for(int i=0;i<PracticeAssist::tool_rows;++i)check(ui.active_row(i));

    PracticeSpec spec;spec.stage=0;spec.section=PracticeSection::Boss;spec.power=52;
    spec.lives=0;spec.bombs=0;spec.dynamic_rank=3;
    const auto scenario=[&]{return build(root,spec,256);};
    {
        auto s=scenario();s->practice_assist.toggle(Tool::Invincible);s->life.register_hit();
        s->tick(0);check(!s->life.miss_active&&s->life.invincibility&&s->life.stage_misses==0);
    }
    {
        auto s=scenario();s->practice_assist.toggle(Tool::Invincible);s->practice_mode=false;
        s->life.register_hit();s->tick(0);check(s->life.miss_active&&s->life.stage_misses==1);
    }
    {
        auto s=scenario();s->practice_assist.toggle(Tool::Lives);s->practice_assist.toggle(Tool::Power);s->practice_assist.toggle(Tool::Rank);
        s->life.register_hit();for(int i=0;i<68;++i)s->tick(0);
        check(!s->life.game_over&&!s->life.miss_active&&s->life.stage_misses==1&&s->life.lives==0);
        check(s->life.power==52&&s->pickups.power==52&&s->life.playperf==3);
        s->rika_stage_frame=1499;s->tick(0);check(s->life.playperf==3);
        s->practice_assist.toggle(Tool::Rank);s->rika_stage_frame=1499;s->tick(0);check(s->life.playperf==4);
    }
    {
        auto s=scenario();s->practice_assist.toggle(Tool::Bombs);s->practice_assist.toggle(Tool::EnemyInvincible);
        s->tick(Bomb);check(s->bomb.active&&s->life.bombs==0&&s->bomb.stage_used==1&&s->rika.damage_taken==0);
        s->practice_assist.toggle(Tool::EnemyInvincible);
        for(int i=0;i<4;++i)s->tick(0);
        check(s->rika.damage_taken>0);
    }
    {
        auto s=scenario();s->practice_assist.toggle(Tool::AutoBomb);s->life.bombs=1;s->life.register_hit();
        s->tick(0);check(s->bomb.active&&s->life.bombs==0&&s->bomb.stage_used==1&&!s->life.miss_active);
        auto empty=scenario();empty->practice_assist.toggle(Tool::AutoBomb);empty->life.register_hit();
        empty->tick(0);check(!empty->bomb.active&&empty->life.miss_active);
    }
    {
        spec.stage=1;spec.section=PracticeSection::Midboss;spec.dialogue=PracticeDialogue::Show;
        auto s=scenario();s->midboss2.active=true;s->midboss2.phase_frame=1600;
        s->practice_assist.toggle(Tool::Timeout);s->tick(0);
        check(!s->midboss2.dying&&s->midboss2.phase_frame==1601);
        s->practice_assist.toggle(Tool::Timeout);s->tick(0);check(s->midboss2.dying);
    }
}
}
int main(int argc,char **argv){
    try {
        if(argc!=2)throw std::runtime_error("Usage: test_practice_session <assets>");
        const std::filesystem::path root=argv[1];
        test_assists(root);
        {
            soew::PracticeSpec spec;spec.stage=4;spec.section=soew::PracticeSection::Boss;
            auto normal=build(root,spec,256);auto reduced=build(root,spec,256,true);
            check(!normal->reduced_effects&&reduced->reduced_effects&&digest(*normal)==digest(*reduced));
            soew::PracticeSession a(spec),b(spec);
            for(unsigned tick=0;tick<128;++tick){a.tick(*normal,0);b.tick(*reduced,0);}
            check(digest(*normal)==digest(*reduced));
            check(reduced->mima_draws.size()<normal->mima_draws.size());
        }
        soew::PracticeSpec spec;spec.stage=0;spec.section=soew::PracticeSection::Midboss;
        spec.seed=0x629ea175;spec.shot=0;spec.rank=1;spec.dynamic_rank=-6;spec.power=37;spec.lives=1;spec.bombs=2;
        {
            using namespace soew;
            unsigned draws=0;
            std::uint32_t candidate=0;
            const auto generate=[&]{++draws;return candidate;};
            check(choose_practice_seed(spec,std::nullopt,generate)==spec.seed&&draws==0);
            check(choose_practice_seed(spec,spec.seed,generate)==spec.seed&&draws==0);
            auto random=spec;random.seed_mode=PracticeSeedMode::Random;
            PracticeBuilder unresolved(root,random);
            check(unresolved.status()==PracticeBuilder::Status::Failed);
            const auto first=choose_practice_seed(random,std::nullopt,generate);
            const auto immediate_retry=choose_practice_seed(random,first,generate);
            check(first==0&&immediate_retry==1&&draws==2);
            candidate=0x56000000u; // Different numeric value, same native random stream as zero.
            const auto retry=choose_practice_seed(random,first,generate);
            check(retry==0x56000001u&&draws==3&&random.seed==spec.seed);
            PracticeBuilder initial(root,random,std::nullopt,first),next(root,random,std::nullopt,retry);
            check(initial.status()==PracticeBuilder::Status::Ready&&next.status()==PracticeBuilder::Status::Ready);
            check(initial.seed()==first&&next.seed()==retry&&initial.recipe()==random&&next.recipe()==random);
            const PracticeSession attempt(next.recipe(),std::nullopt,next.seed());
            check(attempt.recipe==random&&attempt.seed()==retry);
            check(initial.scene()->runtime.random.bytes!=next.scene()->runtime.random.bytes);
            auto fixed=spec;fixed.seed=retry;
            const PracticeBuilder reproducer(root,fixed);
            check(reproducer.status()==PracticeBuilder::Status::Ready&&digest(*reproducer.scene())==digest(*next.scene()));
        }
        {
            auto invalid=spec;invalid.stage=4;soew::PracticeBuilder b(root,invalid);
            check(b.status()==soew::PracticeBuilder::Status::Failed&&!b.error().empty());
            auto chapter=spec;chapter.section=soew::PracticeSection::Chapter;chapter.chapter_id=77;
            soew::PracticeBuilder c(root,chapter);check(c.status()==soew::PracticeBuilder::Status::Failed);
            soew::PracticeBuilder cancelled(root,spec);cancelled.advance(1);cancelled.cancel();
            check(cancelled.status()==soew::PracticeBuilder::Status::Cancelled&&!cancelled.scene());
            check(cancelled.advance()==soew::PracticeBuilder::Status::Cancelled);
            soew::PracticeBuilder zero(root,spec);zero.advance(0);check(zero.prepared_ticks()==0);zero.cancel();
        }
        for(int stage=0;stage<2;++stage){
            spec.stage=stage;
            for(const auto section:{soew::PracticeSection::StageStart,soew::PracticeSection::Midboss,soew::PracticeSection::Boss}){
                spec.section=section;
                for(const auto dialogue:{soew::PracticeDialogue::Show,soew::PracticeDialogue::Skip}){
                    if(section!=soew::PracticeSection::Boss&&dialogue==soew::PracticeDialogue::Skip)continue;
                    spec.dialogue=dialogue;
                    auto a=build(root,spec,256),b=build(root,spec,17);
                    check(digest(*a)!=0&&digest(*a)==digest(*b));
                    if(section==soew::PracticeSection::StageStart)check(a->ticks==0);
                    if(section==soew::PracticeSection::Midboss){
                        check(a->runtime.scroll.progress==unsigned(stage==0?116:80));
                        check(!(stage==0?a->midboss.active:a->midboss2.active));
                        check(stage==0?a->midboss.present:a->midboss2.present);
                        check(a->ticks==0);
                    }
                    if(section==soew::PracticeSection::Boss){
                        check(a->runtime.scroll.done&&a->ticks==0);
                        if(dialogue==soew::PracticeDialogue::Show){
                            check(!a->preboss_started&&!a->dialogue.active()&&a->dialogue_snapshot.empty());
                            a->tick(0);b->tick(0);
                            check(a->dialogue.active()&&a->dialogue_snapshot.size()==640*400);
                        }else check(stage==0?a->rika_enabled:a->meira_enabled);
                    }
                    soew::PracticeSession one(spec),two(spec);
                    for(unsigned i=0;i<24;++i){one.tick(*a,0);two.tick(*b,0);check(digest(*a)==digest(*b));}
                }
            }
        }
        {
            spec.stage=0;spec.section=soew::PracticeSection::StageStart;
            auto a=build(root,spec,256);auto changed=spec;changed.seed^=0x10203040;
            auto b=build(root,changed,256);check(digest(*a)!=digest(*b));
            soew::PracticeSession failed(spec);a->life.miss_active=true;
            failed.tick(*a,0);check(failed.result()==soew::PracticeSession::Result::Running);
            a->life.game_over=true;const auto before=a->ticks;failed.tick(*a,0);
            check(failed.result()==soew::PracticeSession::Result::Failure&&a->ticks==before);
            check(a->practice_completed);
            soew::PracticeSession won(spec);b->rika_enabled=true;b->rika.done=true;
            const auto score_before=b->score_core.sound8;won.tick(*b,0);
            check(won.result()==soew::PracticeSession::Result::Success&&!b->dialogue.postboss);
            check(b->score_core.sound8==score_before);
        }
        {
            spec.section=soew::PracticeSection::Midboss;auto s=build(root,spec,256);
            soew::PracticeSession continued(spec);
            const auto before=s->ticks,prepared_spawns=s->runtime.spawns;continued.tick(*s,0);
            check(continued.result()==soew::PracticeSession::Result::Running&&s->ticks==before+1);
            check(s->midboss.active&&s->runtime.spawns==prepared_spawns);
            for(const auto &enemy:s->runtime.enemies.slots)check(!enemy.raw[0xe]);
            for(unsigned i=0;i<10000&&s->midboss.present;++i){s->life.invincibility=200;continued.tick(*s,0);}
            check(!s->midboss.present&&continued.result()==soew::PracticeSession::Result::Running);
            continued.tick(*s,0);
            check(continued.result()==soew::PracticeSession::Result::Running&&!s->midboss.active);
            // Cleanup is a one-time readiness operation, not a live-tick policy.
            s->items.add(80,80,soew::ItemCore::Point,s->life.lives);
            continued.tick(*s,0);check(s->items.slots[0].flag==1);
            const auto retirement_spawns=s->runtime.spawns;
            for(unsigned i=0;i<1000&&s->runtime.spawns==retirement_spawns;++i){
                s->life.invincibility=200;continued.tick(*s,0);
            }
            check(continued.result()==soew::PracticeSession::Result::Running&&s->runtime.spawns>retirement_spawns);
            bool live_enemy=false;for(const auto &enemy:s->runtime.enemies.slots)live_enemy=live_enemy||enemy.raw[0xe]==1;
            check(live_enemy);
        }
        {
            spec.stage=0;spec.section=soew::PracticeSection::Chapter;spec.chapter_id=1;
            soew::PracticeCheckpoint checkpoint;checkpoint.stage=0;checkpoint.chapter_id=1;
            checkpoint.progress=58;checkpoint.formation_cursor=17;checkpoint.end_progress=112;
            soew::PracticeBuilder builder(root,spec,checkpoint);
            while(builder.status()==soew::PracticeBuilder::Status::Preparing)builder.advance(127);
            if(builder.status()!=soew::PracticeBuilder::Status::Ready)throw std::runtime_error(builder.error());
            auto scene=builder.release();
            check(scene->runtime.scroll.progress==58&&scene->runtime.dispatch.cursor>=17&&!scene->runtime.frame_prepared);
            soew::PracticeSession chapter(spec,checkpoint);
            for(unsigned i=0;i<10000&&scene->runtime.scroll.progress<112;++i){scene->life.invincibility=200;chapter.tick(*scene,0);}
            check(chapter.result()==soew::PracticeSession::Result::Running);
            check(scene->runtime.scroll.progress==112&&scene->runtime.scroll.pending_progress);
            const auto before=scene->ticks;chapter.tick(*scene,soew::Shot);check(scene->ticks==before+1);
            check(chapter.result()==soew::PracticeSession::Result::Running);
            soew::PracticeCheckpoint preboss;preboss.end_at_preboss=true;
            spec.section=soew::PracticeSection::StageStart;spec.chapter_id=-1;
            auto held=build(root,spec,256);held->runtime.scroll.done=true;
            soew::PracticeSession end(spec,preboss);end.tick(*held,0);
            check(end.result()==soew::PracticeSession::Result::Running&&held->preboss_started&&held->dialogue.active());
            spec.section=soew::PracticeSection::Chapter;spec.chapter_id=0;
            soew::PracticeCheckpoint start;start.stage=0;start.chapter_id=0;start.progress=0;start.end_progress=58;
            soew::PracticeBuilder first(root,spec,start);
            check(first.status()==soew::PracticeBuilder::Status::Ready&&first.prepared_ticks()==0);
            check(first.scene()->runtime.scroll.progress==0&&!first.scene()->runtime.scroll.pending_progress);
        }
        for(int stage=0;stage<2;++stage)for(int shot=0;shot<3;++shot)for(int rank=0;rank<4;++rank)for(auto section:{soew::PracticeSection::StageStart,soew::PracticeSection::Midboss,soew::PracticeSection::Boss}){soew::PracticeSpec matrix;matrix.stage=stage;matrix.shot=shot;matrix.rank=rank;matrix.section=section;matrix.seed=0xa0123456u;matrix.power=(rank&1)?1:80;matrix.lives=rank==0?0:4;matrix.bombs=rank==0?0:3;auto s=build(root,matrix,512);check(!s->life.game_over&&s->life.invincibility==0);}
        {
            soew::PracticeSpec third;third.stage=2;third.section=soew::PracticeSection::StageStart;
            auto a=build(root,third,256),b=build(root,third,17);
            check(a->midboss3_enabled&&!a->midboss2_enabled&&a->ticks==0&&digest(*a)==digest(*b));
            soew::PracticeSession first(third),retry(third);
            for(unsigned i=0;i<18000&&first.result()==soew::PracticeSession::Result::Running;++i){
                const bool press=a->dialogue.phase==soew::StageDialogue::Phase::KeyDelay&&a->dialogue.key_delay.waiting_press;
                a->life.invincibility=b->life.invincibility=200;
                first.tick(*a,0,press);retry.tick(*b,0,press);
                check(a->stones.phase==b->stones.phase&&a->stones.phase_frame==b->stones.phase_frame&&a->bullet_core.spawned==b->bullet_core.spawned);
            }
            check(first.result()==soew::PracticeSession::Result::Success&&retry.result()==first.result());
            check(a->stage==2&&a->stones.done()&&a->practice_completed&&!a->dialogue.postboss);
        }
        for(int stage=0;stage<soew::practice_stage_count;++stage){
            auto full=soew::PracticeSpec{};full.stage=stage;full.rank=stage==5?4:1;full.section=soew::PracticeSection::StageStart;
            full.seed=0x76543210;full.dynamic_rank=-6;full.dialogue=soew::PracticeDialogue::Show;
            auto s=build(root,full,256);check(s->ticks==0);
            soew::PracticeSession route(full);
            for(unsigned i=0;i<50000&&route.result()==soew::PracticeSession::Result::Running&&!s->preboss_started;++i){s->life.invincibility=200;route.tick(*s,0);}
            check(route.result()==soew::PracticeSession::Result::Running&&s->runtime.scroll.done&&s->preboss_started&&s->dialogue.active());
            check(s->transition.phase==soew::StageTransition::Phase::Battle&&!s->stage5_result&&!s->extra_settlement.terminal());
        }
        for(int stage=0;stage<soew::practice_stage_count;++stage){
            auto boss=soew::PracticeSpec{};boss.stage=stage;boss.rank=stage==5?4:1;boss.seed=0x76543210;
            for(auto dialogue:{soew::PracticeDialogue::Skip,soew::PracticeDialogue::Show}){
                boss.dialogue=dialogue;auto s=build(root,boss,256);auto retry=build(root,boss,17);
                check(s->runtime.scroll.done&&s->ticks==0&&digest(*s)==digest(*retry));
                const bool show=dialogue==soew::PracticeDialogue::Show;
                const auto enabled=[&]{return s->rika_enabled||s->meira_enabled||s->stones_enabled||s->marisa_enabled||s->mima_enabled||s->sigma_enabled;};
                soew::PracticeSession route(boss);
                check(enabled()==!show);
                s->score_core.score=99990;s->score_core.delta=20;
                const auto score=s->score_core;const auto lives=s->life.lives;
                for(unsigned i=0;i<5000&&!enabled();++i)route.tick(*s,0,false,true);
                check(enabled()&&route.result()==soew::PracticeSession::Result::Running);
                switch(stage){
                case 0:s->rika.done=true;break;
                case 1:s->meira.done=true;break;
                case 2:s->stones.phase=soew::StonesCore::Phase::Complete;break;
                case 3:s->marisa.cleared=true;break;
                case 4: {
                    using Dialogue=soew::StageDialogue;
                    soew::MimaDialogueTap tap;
                    tap.event(nullptr,true); // Z was held to defeat Form 1.
                    s->mima.event=soew::MimaCore::Event::Form1Defeated;
                    route.tick(*s,0);
                    auto &d=s->dialogue;
                    check(d.mima_pre_final_dialogue()==show);
                    if(show){
                        check(d.phase==Dialogue::Phase::SlideIn&&!s->mima.form2);
                        const auto step=[&]{
                            const auto input=tap.poll(d);
                            route.tick(*s,input.accelerate?soew::Shot:0,input.group);
                            return input;
                        };
                        for(unsigned i=0;i<30&&d.phase==Dialogue::Phase::SlideIn;++i)step();
                        check(d.phase==Dialogue::Phase::Text&&d.current==Dialogue::stage5_pre_final_start);
                        for(unsigned box=Dialogue::stage5_pre_final_start;box<Dialogue::stage5_final_start;++box){
                            check(d.current==box&&d.phase==Dialogue::Phase::Text&&!s->mima.form2);
                            tap.event(&d,true,true);
                            const auto idle=step();
                            check(!idle.accelerate&&!idle.group&&d.phase==Dialogue::Phase::Text);
                            tap.event(&d,false);
                            tap.event(&d,true);
                            const auto reveal=step();
                            check(reveal.accelerate&&!reveal.group&&d.current==box&&d.phase==Dialogue::Phase::KeyDelay);
                            unsigned confirms=0;
                            for(unsigned i=0;i<6&&d.current==box;++i)confirms+=step().group;
                            check(confirms==1&&d.current==box+1);
                        }
                        check(!d.active()&&!d.mima_pre_final_dialogue());
                    }else check(!d.active());
                    check(s->mima.form2&&route.result()==soew::PracticeSession::Result::Running);
                    s->mima.event=soew::MimaCore::Event::FinalDefeated;break;
                }
                case 5:s->sigma.done=true;break;
                }
                route.tick(*s,0);
                if(show){
                    check(route.result()==soew::PracticeSession::Result::Running&&s->dialogue.active()&&s->dialogue.postboss);
                    if(stage==1){
                        std::vector<std::uint32_t> appearance(640*400),hidden(640*400);
                        s->meira_appearance=true;s->render(appearance);
                        s->meira_appearance=false;s->render(hidden);
                        check(appearance==hidden);
                    }
                    for(unsigned i=0;i<500&&route.result()==soew::PracticeSession::Result::Running;++i)route.tick(*s,0,false,true);
                }
                check(route.result()==soew::PracticeSession::Result::Success&&s->dialogue.postboss==show&&!s->dialogue.active()&&!s->stage5_result);
                check(s->score_core.score==score.score&&s->score_core.delta==score.delta&&s->score_core.hiscore==score.hiscore);
                check(s->score_core.extends==score.extends&&s->life.lives==lives&&!s->extra_settlement.terminal());
            }
        }
        const auto caps=soew::practice_capabilities();
        for(int stage=2;stage<soew::practice_stage_count;++stage){
            for(int phase=1;caps.allows_phase(stage,phase);++phase){
                auto spec=soew::PracticeSpec{};spec.stage=stage;spec.rank=stage==5?4:1;spec.boss_phase=phase;
                auto s=build(root,spec,256);auto retry=build(root,spec,17);
                check(digest(*s)==digest(*retry)&&!s->dialogue.active()&&s->runtime.scroll.done);
                soew::PracticeSession route(spec);std::vector<std::uint32_t> pixels(640*400);
                for(unsigned i=0;i<240;++i){s->life.invincibility=200;route.tick(*s,0);if(i%40==0)s->render(pixels);}
                check(route.result()==soew::PracticeSession::Result::Running&&!s->dialogue.postboss&&!s->stage5_result);
                check(!s->runtime.blocked&&!s->sigma.blocked&&!s->mima.blocked_reason());
            }
        }
        std::cout<<"Practice session checks="<<checks<<'\n';return 0;
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
}
