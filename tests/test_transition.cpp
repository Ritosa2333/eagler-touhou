#include "encounter_scene.hpp"
#include "japanese.hpp"
#include <iostream>
#include <source_location>
unsigned checks=0;void check(bool v,const std::source_location loc=std::source_location::current()){++checks;if(!v)throw std::runtime_error("Transition check="+std::to_string(checks)+" line="+std::to_string(loc.line()));}
void acknowledge(soew::EncounterScene&s){for(unsigned i=0;i<4;++i)s.tick(0,false);for(unsigned i=0;i<2;++i)s.tick(soew::OK,true);}
int main(int argc,char**argv){try{if(argc!=2)throw std::runtime_error("need assets");for(int shot=0;shot<3;++shot)for(int rank=0;rank<4;++rank){auto s=soew::EncounterScene::load(argv[1],0,shot,rank,2,3);s.combat_enabled=s.survival_enabled=s.pickup_enabled=true;s.enable_effects();s.enable_bombing();s.enable_preboss();s.transitions_enabled=true;s.dialogue.current=16;s.dialogue.phase=soew::StageDialogue::Phase::Complete;s.bind_rika_battle({0,0,0,0,0,0});s.rika.done=true;s.life.lives=1;s.life.bombs=2;s.pickups.power=52;s.life.total_misses=4;s.life.stage_misses=1;s.life.playperf=2;s.pickups.points_collected=7;s.score_core.score=50000;s.score_core.delta=17;s.resident_frame=12345;s.bomb.stage_used=1;s.bomb.total_used=3;s.items.item_skill=3;s.items.drop_cycle=5;const auto ticks=s.ticks;s.tick(0);check(s.transition.phase==soew::StageTransition::Phase::Postboss&&s.score_core.score==50017);for(unsigned box=16;box<24;++box){while(s.dialogue.phase==soew::StageDialogue::Phase::SlideIn||s.dialogue.phase==soew::StageDialogue::Phase::Text)s.tick(soew::Shot,true);for(unsigned i=0;i<10;++i)s.tick(soew::Shot,true);check(s.dialogue.current==box);acknowledge(s);}check(s.dialogue.current==24&&s.transition.phase==soew::StageTransition::Phase::BonusAck);const auto bonus=s.transition.bonus.internal_bonus;check(bonus==soew::StageBonus::regular(rank,2,1,1,3,2,7).internal_bonus);check(s.score_core.score==50034+bonus&&s.score_core.delta==17+bonus);const auto settled=s.score_core.score;for(unsigned i=0;i<100;++i)s.tick(soew::Shot,true);check(s.score_core.score==settled&&s.transition.phase==soew::StageTransition::Phase::BonusAck);acknowledge(s);check(s.transition.phase==soew::StageTransition::Phase::ExtraAck);acknowledge(s);check(s.transition.phase==soew::StageTransition::Phase::Leave);for(unsigned i=0;i<142;++i)s.tick(0);check(s.transition.phase==soew::StageTransition::Phase::Ready&&s.ticks==ticks&&s.resident_frame==12345);auto n=s.next_stage();check(n.stage==1&&n.player.shot==shot&&n.rank==rank&&n.life.lives==1&&n.life.bombs==2&&n.pickups.power==52);check(n.score_core.score==settled&&n.score_core.delta==0&&n.life.stage_misses==0&&n.life.total_misses==4&&n.bomb.stage_used==0&&n.bomb.total_used==3&&n.pickups.points_collected==0);check(!n.rika_enabled&&!n.midboss_enabled&&n.shots.fired==0&&n.ticks==0&&n.life.invincibility==0&&n.player.x==208&&n.player.y==336);check(n.items.item_skill==3&&n.items.drop_cycle==5&&n.run_start_lives==2&&n.run_start_bombs==3);for(unsigned i=0;i<42;++i)n.tick(soew::Shot);check(n.ticks==0&&n.transition.phase==soew::StageTransition::Phase::Battle);n.tick(soew::Right);check(n.ticks==1&&n.resident_frame==12346);for(unsigned i=0;i<300;++i){n.life.invincibility=200;n.tick(0);}check(!n.runtime.blocked&&n.ticks==301);}
{auto s=soew::EncounterScene::load(argv[1],1,0,1,2,3);s.transitions_enabled=true;s.runtime.scroll.progress=80;s.tick(soew::Shot);check(s.stage2_preview_boundary&&s.ticks==0);}
{
    using P=soew::ExtraSettlement::Phase;
    const auto b=soew::ExtraBonus::calculate(1,2,10001,7);
    check(b.rows==std::array<unsigned,4>{10000,16000,12000,9990});
    check(b.subtotal==4799&&b.point_items==7&&b.internal_bonus==33593);
    check(soew::ExtraBonus::calculate(5,99,20000,13).internal_bonus==13000);
    check(soew::ExtraBonus::calculate(0,0,0,0).internal_bonus==0);
    soew::ExtraSettlement s;
    check(s.begin(b,true)&&s.showing_bonus()&&!s.lose(false)&&!s.begin(b,false));
    for(unsigned i=0;i<16;++i)s.tick(true);
    check(s.phase==P::BonusAck);
    for(const auto next:{P::FinalAck,P::Clear}){
        for(unsigned i=0;i<4;++i)s.tick(false);
        for(unsigned i=0;i<2;++i)s.tick(true);
        check(s.phase==next);
    }
    check(s.terminal()&&!s.showing_bonus()&&!s.lose(false));
    soew::ExtraSettlement loss;
    check(loss.lose(true)&&!loss.terminal()&&!loss.begin(b,false)&&loss.phase==P::GameOver);
    for(unsigned i=0;i<60;++i)loss.tick(true);
    check(!loss.terminal());
    for(unsigned i=0;i<4;++i)loss.tick(false);
    for(unsigned i=0;i<2;++i)loss.tick(true);
    check(loss.terminal()&&loss.phase==P::Lost);
}
{
    namespace o=soew::stage_overlay;
    using L=soew::Language;
    constexpr std::array languages={L::Japanese,L::English,L::ChineseSimplified,L::ChineseTraditional};
    constexpr int expected_wave_x[4][6]={
        {192,168,184,176,168,168},{212,168,184,172,96,160},
        {200,200,200,200,168,176},{200,200,200,200,168,176}};
    // The 81 60 glyph is transparent except for its three middle scanlines.
    constexpr std::array<std::string_view,3> wave_rows={"...####......#..","..##...##...##..","..#......####..."};
    constexpr int width=640,y=208;
    constexpr std::uint32_t background=0x123456,foreground=0xffffff;
    std::vector<std::uint32_t> pixels(width*400,background);
    for(unsigned stage=0;stage<o::subtitles.size();++stage){
        check(jp::cell_width(o::subtitle(L::Japanese,stage))==o::subtitle_half_lengths[stage]*2*o::cell_w);
        for(unsigned language=0;language<languages.size();++language){
            const auto subtitle=o::subtitle(languages[language],stage);
            const auto wave=subtitle.find(L'～');
            check(wave!=std::wstring_view::npos&&subtitle.find(L'～',wave+1)==std::wstring_view::npos&&
                subtitle.find(L'~')==std::wstring_view::npos&&jp::cell_width(subtitle.substr(wave,1))==2*o::cell_w&&
                jp::cell_width(subtitle)<=(o::playfield_right-o::playfield_left)*o::cell_w);
            for(const bool opening:{false,true}){
                const auto layout=o::layout_subtitle(subtitle,stage,opening&&languages[language]==L::Japanese,jp::cell_width);
                const auto x=expected_wave_x[language][stage];
                check(layout.wave_x==x&&layout.wave_at==wave);
                check(layout.text_x+jp::cell_width(subtitle)/2==224);
                std::fill(pixels.begin()+(y-1)*width,pixels.begin()+(y+17)*width,background);
                o::paint_subtitle_wave(pixels,width,layout.wave_x,o::subtitle_row*o::cell_h,foreground);
                for(int row=y-1;row<=y+16;++row)for(int col=x-1;col<=x+16;++col){
                    const bool ink=row>=y+6&&row<=y+8&&col>=x&&col<x+16&&wave_rows[row-y-6][col-x]=='#';
                    check(pixels[row*width+col]==(ink?foreground:background));
                }
            }
        }
    }
    check(o::subtitle(L::English,1)==L"Raijuu ～ Midnight Rainstorm");
    check(o::subtitle(L::English,4)==L"Fuuma ～ Story of Eastern Wonderland and ...");
    for(const auto title:o::bgm_titles)
        check(jp::cell_width(title)==24*o::cell_w);
    check(!o::show_bgm(0)&&o::show_bgm(1)&&o::show_bgm(158)&&!o::show_bgm(159));
    check(o::title_column==24&&o::title_row==4&&o::formula_row==18);
    check(o::result_column(6)==38&&o::result_column(7)==36);
}
{
    using I=soew::StageIntro;using P=soew::StageTransition::Phase;
    I intro;
    intro.arm(0,false);check(!intro.active());
    intro.arm(0,true);check(intro.active());
    intro.advance(500,false);intro.advance(1000,true);check(intro.active()&&intro.elapsed==0);
    for(unsigned i=1;i<I::wait_frames;++i)intro.advance(1000+17*i,true);
    check(intro.active()&&intro.elapsed==99&&intro.showing_eye()&&!intro.revealing());
    intro.advance(1000+17*I::wait_frames,true);check(intro.active()&&intro.elapsed==100&&intro.revealing()&&intro.reveal_frame()==0);
    for(unsigned i=1;i<42;++i){intro.advance(1000+17*(I::wait_frames+i),true);check(intro.revealing()&&intro.reveal_frame()==i);}
    check(intro.active()&&intro.elapsed==141&&intro.reveal_frame()==41);
    intro.advance(1000+17*(I::wait_frames+42),true);check(!intro.active());
    intro.arm(0,true);intro.cancel();check(!intro.active());
    auto with_overlay=soew::StageTransition{},without_overlay=soew::StageTransition{};
    with_overlay.phase=without_overlay.phase=P::Leave;
    constexpr std::uint32_t outside=0xcdcdcd,art=0xabcdef;
    std::vector<std::uint32_t> frame(640*400,outside),eye(I::width*I::height,art);
    for(unsigned i=0;i<soew::StageTransition::fade_frames+100;++i){
        if(with_overlay.phase==P::LoadWait)I::paint(frame,eye);
        with_overlay.tick(i%2==0);without_overlay.tick(i%2==0);
        check(with_overlay.phase==without_overlay.phase&&with_overlay.frame==without_overlay.frame);
    }
    check(with_overlay.phase==P::Ready);
    check(frame[0]==0&&frame[I::y*640+I::x-1]==0&&frame[I::y*640+I::x]==art);
    check(frame[(I::y+I::height-1)*640+I::x+I::width-1]==art&&frame[(I::y+I::height)*640+I::x]==0);
    with_overlay.phase=without_overlay.phase=P::Entry;
    for(unsigned i=0;i<soew::StageTransition::fade_frames;++i){
        with_overlay.tick(i%2==1);without_overlay.tick(i%2==1);
        check(with_overlay.phase==without_overlay.phase&&with_overlay.frame==without_overlay.frame);
    }
    check(with_overlay.phase==P::Battle);
}
std::cout<<"transition_checks="<<checks<<" failures=0\n";return 0;}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
