#include "encounter_scene.hpp"
#include "ending_core.hpp"
#include <iostream>
#include <source_location>

using namespace soew;
void check(bool ok, std::source_location where=std::source_location::current()){
    if(!ok)throw std::runtime_error("EX integration, line "+std::to_string(where.line()));
}

void check_sigma_bomb_layers(const std::filesystem::path &root,int shot){
    auto s=EncounterScene::load(root,5,shot,4,2,1);
    s.enable_extra();s.enable_bombing();s.sigma_enabled=true;
    s.rendered_page=1;s.sigma.left[1]=144;s.sigma.top[1]=64;
    s.sigma.visible=false;
    std::vector<std::uint32_t> idle(640*400),bomb_only(idle.size()),expected(idle.size()),pixels(idle.size());
    s.render(idle);
    check(s.bomb.start(s.life.bombs,unsigned(shot)));
    IRand rng(1234);RandomRing random;random.fill(rng);
    // Each selected body frame includes a full playfield fill and the shot's portrait.
    constexpr std::array portrait_frame={71,137,101};
    for(unsigned tick=0;tick<256&&!(s.bomb.circle_done&&s.bomb.frame==portrait_frame[shot]);++tick)
        s.bomb.update(random);
    check(s.bomb.active&&s.bomb.visual==BombCore::Body&&s.bomb.frame==portrait_frame[shot]);
    s.render(bomb_only);
    check(bomb_only!=idle);
    const auto tone=unsigned(s.bomb.palette_tone);
    const auto palette=BombRenderer::palette(s.background.palette,s.bomb);
    const int left=s.sigma.left[1],top=s.sigma.top[1];
    for(const unsigned cel:{134u,136u,138u,140u,132u,130u,128u}){
        s.sigma.visible=true;s.sigma.draw_pattern=cel;s.sigma.hit_flash=cel==130;
        expected=bomb_only;
        for(unsigned half=0;half<2;++half){
            const auto &sprite=*s.sigma_art->sprites.at(cel+half);
            unsigned changed=0,transparent=0;
            for(unsigned y=0;y<sprite.height;++y)for(unsigned x=0;x<sprite.width;++x){
                auto &pixel=expected[(top+int(y))*640+left+int(64*half+x)];
                const auto color=sprite.pixels[y*sprite.width+x];
                if(color==sprite.transparent){++transparent;continue;}
                const auto rgb=BombRenderer::toned(EncounterScene::stage5_rgb(palette[color]),tone);
                changed+=pixel!=rgb;pixel=rgb;
            }
            check(changed&&transparent);
        }
        s.render(pixels);
        check(pixels==expected);
    }
    // Preserve Sigma's own command order: paint behind the body, then paint in front.
    const auto &sprite=*s.sigma_art->sprites.at(s.sigma.draw_pattern);
    auto opaque=std::find_if(sprite.pixels.begin(),sprite.pixels.end(),[&](auto p){return p!=sprite.transparent&&p!=15;});
    check(opaque!=sprite.pixels.end());
    const auto offset=unsigned(opaque-sprite.pixels.begin());
    const int x=left+int(offset%sprite.width),y=top+int(offset/sprite.width);
    s.sigma.paints={{SigmaCore::Paint::Dot,x,y,1,0,15}};
    s.sigma.body_paint_index=1;
    s.render(pixels);check(pixels==expected);
    s.sigma.body_paint_index=0;
    expected[y*640+x]=BombRenderer::toned(EncounterScene::stage5_rgb(palette[15]),tone);
    s.render(pixels);check(pixels==expected);
}

int wmain(int argc,wchar_t **argv){try{
    if(argc!=2)throw std::runtime_error("Expected original asset directory");
    const std::filesystem::path root=argv[1];
    for(int shot=0;shot<3;++shot){
        check_sigma_bomb_layers(root,shot);
        auto s=EncounterScene::load(root,5,shot,4,2,1);
        s.enable_extra();s.combat_enabled=s.survival_enabled=s.pickup_enabled=true;
        s.reduced_effects=shot==2;s.enable_effects();s.enable_bombing();
        std::vector<std::uint32_t> pixels(640*400),second(pixels.size());
        unsigned damage_phases=0,boss_ticks=0;
        bool cue=false,bombed=false;
        for(unsigned i=0;i<60000&&!s.extra_settlement.terminal();++i){
            s.life.invincibility=200; // Protected deterministic test, not a human survival run.
            const auto phase=s.dialogue.phase;
            const bool key=s.extra_settlement.showing_bonus()?s.extra_settlement.gate.waiting_press:
                phase==StageDialogue::Phase::Text||(phase==StageDialogue::Phase::KeyDelay&&s.dialogue.key_delay.waiting_press);
            std::uint16_t input=s.dialogue.active()?(key?Shot:0):0;
            if(s.sigma_enabled&&!s.dialogue.active()&&!s.extra_settlement.showing_bonus()){
                ++boss_ticks;
                const int dx=s.sigma.left[s.runtime.page^1]+60-(s.player.x+16);
                input=Shot|(dx>4?Right:dx<-4?Left:0)|(s.player.y>230?Up:0);
                if(!bombed&&s.sigma.phase==3){input|=Bomb;bombed=true;}
            }
            const auto damage=s.sigma.damage_taken;
            s.tick(input,key);
            if(s.runtime.blocked)throw std::runtime_error(s.runtime.blocked->result.reason);
            if(s.sigma.blocked)throw std::runtime_error(s.sigma.blocked);
            check(!s.weapon_blocked&&!s.life.game_over&&!s.continue_prompt());
            if(s.sigma.damage_taken>damage&&s.sigma.phase<10)damage_phases|=1u<<s.sigma.phase;
            if(s.sigma_music&&!cue){check(s.dialogue.current==StageDialogue::extra_intro_end&&!s.sigma_enabled);cue=true;}
            if(i%257==0){const auto tick=s.ticks;s.render(pixels);s.render(second);check(pixels==second&&s.ticks==tick);}
        }
        std::cout<<"extra shot="<<shot<<" ticks="<<s.ticks<<" sigma_ticks="<<boss_ticks<<" phase="<<s.sigma.phase<<" frame="<<s.sigma.phase_frame<<" hit_phases="<<damage_phases<<" score="<<s.score_core.score<<'\n';
        check(s.runtime.scroll.done&&s.extra_midboss.finished&&cue&&s.sigma.done&&bombed);
        check(s.extra_settlement.phase==ExtraSettlement::Phase::Clear&&s.score_core.score>=300000&&s.bomb.stage_used==1);
        check((damage_phases&0x2aa)==0x2aa);
        const auto score=s.score_core.score;s.tick(Shot,true);check(s.score_core.score==score);
        bool rejected=false;try{(void)s.next_stage();}catch(const std::runtime_error&){rejected=true;}check(rejected);
    }
    auto loss=EncounterScene::load(root,5,1,4,2,1);loss.enable_extra();loss.survival_enabled=true;
    loss.life.lives=0;loss.life.register_hit();
    for(unsigned i=0;i<70;++i)loss.tick(0);
    check(loss.extra_settlement.phase==ExtraSettlement::Phase::GameOver&&!loss.continue_prompt());
    for(unsigned i=0;i<4;++i)loss.tick(Shot,true);
    check(loss.extra_settlement.phase==ExtraSettlement::Phase::Lost&&!loss.continue_prompt()&&!loss.continues_used);
    const auto result=make_ending_script({.rank=4});
    for(const auto &e:result.events)check(e.kind!=EndingEventKind::FullPicture&&e.kind!=EndingEventKind::VerdictRow);
    return 0;
}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
