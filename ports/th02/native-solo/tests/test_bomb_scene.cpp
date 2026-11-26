#include "encounter_scene.hpp"
#include <iostream>
#include <source_location>
unsigned checks=0;
void check(bool v,std::source_location where=std::source_location::current()){++checks;if(!v)throw std::runtime_error("Bomb scene failed line "+std::to_string(where.line()));}
soew::EncounterScene setup(const char *root,unsigned type){auto s=soew::EncounterScene::load(root,0,int(type),1,2,3);s.combat_enabled=s.survival_enabled=s.pickup_enabled=true;s.enable_effects();s.enable_bombing();return s;}
void check_art(const char *root){
    using soew::BombCore;using soew::BombRenderer;
    std::vector<std::uint32_t> pixels(640*400);
    constexpr std::uint32_t terrain=0x102030;
    const auto reset=[&]{std::fill(pixels.begin(),pixels.end(),terrain);};
    const auto color=[&](int x,int y){return pixels[y*640+x];};
    for(unsigned type=0;type<3;++type){
        auto scene=setup(root,type);
        const auto &art=*scene.bomb_art;
        auto &b=scene.bomb;
        bool mask=false,portrait=false,peak=false,overlay=false,smear=false,pulse=false;
        const auto raw=scene.background.palette;
        constexpr int portrait_frame[]={71,137,101},portrait_x[]={112,32,32},portrait_y[]={90,84,16},recovery[]={136,164,112};
        scene.tick(soew::Bomb);
        while(b.active){
            const auto palette=BombRenderer::palette(raw,b);
            const auto render=[&](bool reduced=false){
                reset();art.render(pixels,palette,b,reduced);art.render_tone(pixels,b);
            };
            const auto lit=[&](unsigned i){return BombRenderer::toned(soew::EncounterScene::stage5_rgb(palette[i]),b.palette_tone);};
            if(b.visual==BombCore::Mask&&b.frame==12&&type==0){
                render();
                check(std::count(pixels.begin(),pixels.end(),0x451045u)==422*256);mask=true;
            }
            if(b.visual==BombCore::Mask&&b.frame==31){
                render();
                check(std::count(pixels.begin(),pixels.end(),lit(type==0?3:type==1?15:2))==46*256);
            }
            if(b.visual==BombCore::Body&&b.frame==32&&type==0){
                render();check(color(32,16)==0x8a008a);
            }
            if(b.visual==BombCore::Body&&b.frame>=64&&b.frame<=69&&type==0){
                render();check(color(32,16)==0xffffff&&color(415,383)==0xffffff);
            }
            if(b.visual==BombCore::Body&&b.frame==70&&type==0){
                render();check(color(112,90)==lit(11)&&color(220,180)==lit(11));
            }
            if(b.visual==BombCore::Body&&b.frame==72&&type==0){
                render(true);check(color(112,90)==lit(11)&&color(220,180)==lit(11));
            }
            if(b.visual==BombCore::Body&&b.frame==36&&type==1){
                render();check(color(32,16)==0xffffff);
                const auto &rows=art.overlays[b.overlay_frame-1];
                unsigned cells=0;
                for(unsigned y=0;y<23;++y)for(unsigned x=0;x<48;++x)cells+=(rows[y]>>x)&1;
                art.render_overlay(pixels,b);
                // First red picture measured from the original BOMB1.BFT.
                check(cells==336&&std::count(pixels.begin(),pixels.end(),BombRenderer::reverse_red)==336*128);
                check(color(32,15)==terrain&&color(416,16)==terrain);overlay=true;
            }
            if(b.visual==BombCore::Body&&b.frame==132&&type==1){
                render();art.render_overlay(pixels,b);
                check(std::count(pixels.begin(),pixels.end(),BombRenderer::reverse_red)==0);
            }
            if(b.visual==BombCore::Body&&b.frame==150&&type==1){
                render(true); // An even reduced-effects frame contains the ring, not the PI.
                check(lit(14)!=0xffffff&&color(286,199)==lit(14)&&
                      color(285,199)==0xffffff&&color(224,200)==0xffffff);
            }
            if(b.visual==BombCore::Body&&b.frame==80&&type==2){
                render();
                check(color(32,16)==0xaa0000&&color(32,383)==0xff0000);
                const int bottom=b.columns[0];
                check(bottom==212&&color(32,bottom+2)==0xff0000&&
                      color(33,bottom+2)==0xaa0000&&color(33,bottom+3)==0xff0000);smear=true;
            }
            if(b.visual==BombCore::Body&&type==2&&b.frame>=100&&b.frame<112&&!(b.frame&1)){
                render();check(color(32,16)==0xffffff&&color(224,200)==0xffffff);pulse=true;
            }
            if(b.visual==BombCore::Body&&b.frame==portrait_frame[type]){
                render();
                // Match decoded PI art over the interior, allowing the moving
                // bomb particles and B's expanding ring to cover some texels.
                unsigned matches=0,nonzero=0;
                for(unsigned y=24;y<96;++y)for(unsigned x=16;x<80;++x){
                    const auto index=art.portrait[y*art.image_w+x];
                    if(index==0)continue;
                    ++nonzero;
                    matches+=color(portrait_x[type]+int(x),portrait_y[type]+int(y))==lit(index);
                }
                check(nonzero&&matches*100>=nonzero*98);portrait=true;
            }
            if(b.visual==BombCore::Body&&b.frame==recovery[type]){
                render();check(color(32,16)==0xffffff&&color(416,16)==terrain);peak=true;
            }
            scene.tick(0);
        }
        check(portrait&&peak&&(type!=0||mask)&&(type!=1||overlay)&&(type!=2||(smear&&pulse)));
    }
}
int main(int argc,char**argv){try{if(argc<2)throw std::runtime_error("need assets root");
 check_art(argv[1]);
 for(unsigned type=0;type<3;++type){auto s=setup(argv[1],type);s.life.bombs=1;
  auto &b=s.bullet_core.bullets[0];b.flag=1;b.position[1]={3200,1600};
  s.tick(soew::Bomb);check(s.bomb.active&&s.life.bombs==0&&s.life.bomb_invincible&&s.bomb.stage_used==1);check(b.flag==2&&s.bullet_core.spawned==0);
  s.paused=true;auto tick=s.ticks;auto frame=s.bomb.frame;s.tick(soew::Bomb);check(s.ticks==tick&&s.bomb.frame==frame);s.paused=false;
  std::vector<std::uint32_t> pixels(640*400),repeat(640*400);
  const unsigned recovery=type==0?136:type==1?164:112;bool recovery_checked=false;
  while(s.bomb.active){
   if(s.bomb.circle_done&&unsigned(s.bomb.frame)==recovery-1){auto &probe=s.bullet_core.bullets[149];probe.flag=1;probe.position[s.runtime.page^1]={1600,1600};}
s.tick(s.ticks<7?soew::Right|soew::Bomb:0);if(s.bomb.circle_done&&unsigned(s.bomb.frame)==recovery){check(s.bullet_core.bullets[149].flag==2);recovery_checked=true;}check(s.bomb.stage_used==1);s.render(pixels);s.render(repeat);check(pixels==repeat);if(s.bomb.active)check(s.life.bomb_invincible&&s.life.stage_misses==0&&s.bullet_core.spawned==0);}
  check(recovery_checked);check(s.bullet_core.removed==2);check(s.ticks==(type==2?162u:190u));check(!s.life.bomb_invincible&&s.life.invincibility==69&&s.player.x>208);
  s.life.register_hit();s.tick(0);check(!s.life.miss_active&&!s.life.hit&&s.life.invincibility==68);
  std::cout<<"bomb_scene_type="<<type<<" ticks="<<s.ticks<<" body_frame="<<s.bomb.frame<<"\n";
 }
 // Exercise the real enemy callback: bomb damage must reach HP and score even
 // with no shot present; an ordinary lethal hit still creates its item drop.
 {auto s=setup(argv[1],0);auto &t=s.script.enemies[0];t.raw[22]=2;t.raw[23]=0;t.raw[24]=1;t.raw[16]=3;t.raw[18]=100;t.raw[19]=t.raw[20]=t.raw[21]=0;s.script.paths[t.path_slot].bytes={0,255};auto &enemy=s.runtime.enemies.slots[0];enemy.raw[0xe]=1;enemy.word(0xc,0);enemy.word(4,100);enemy.word(6,100);
  s.tick(soew::Bomb);check(enemy.word(0x20)==1&&s.damage.kills==0&&s.score_core.score+s.score_core.delta==1);s.tick(0);check(enemy.word(0x20)==1);s.tick(0);check(enemy.word(0x20)==2&&s.damage.kills==1&&s.items.spawned==1&&s.score_core.score+s.score_core.delta==102);}
 // An already latched unprotected hit blocks bomb activation (no deathbomb).
 {auto s=setup(argv[1],1);s.life.register_hit();s.tick(soew::Bomb);check(!s.bomb.active&&s.life.bombs==3&&s.life.miss_frame==1);}
 std::cout<<"bomb_scene_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
