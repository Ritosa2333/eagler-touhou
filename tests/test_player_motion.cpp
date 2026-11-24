#include "player_motion.hpp"
#include <iostream>
unsigned checks=0;void check(bool ok){++checks;if(!ok)throw std::runtime_error("TH02 player motion contract failed");}
int main(){try{
    for(int shot=0;shot<3;++shot)for(unsigned page=0;page<2;++page){
        for(unsigned keys=0;keys<16;++keys){soew::PlayerMotion p(shot,page);p.tick(static_cast<std::uint16_t>(keys));const int speed=shot==0?5-int(page):shot==1?4:3;const int diag=shot==0?4-int(page):3;
            int dx=0,dy=0;switch(keys){case 1:dy=-speed;break;case 2:dy=speed;break;case 4:dx=-speed;break;case 8:dx=speed;break;case 5:dx=-diag;dy=-diag;break;case 6:dx=-diag;dy=diag;break;case 9:dx=diag;dy=-diag;break;case 10:dx=diag;dy=diag;break;}
            check(p.x==208+dx&&p.y==336+dy);check(p.frame==1&&p.back_page==(page^1));
        }
        soew::PlayerMotion p(shot,page);for(int i=0;i<1000;++i)p.tick(soew::Left|soew::Up);check(p.x==28&&p.y==16);for(int i=0;i<1000;++i)p.tick(soew::Right|soew::Down);check(p.x==390&&p.y==344);
        p=soew::PlayerMotion(shot,page);p.tick(soew::UpLeft|soew::Right);check(p.x<208&&p.y<336);p=soew::PlayerMotion(shot,page);p.tick(soew::UpLeft|soew::UpRight|soew::Down);check(p.x==208&&p.y>336);
    }
    soew::PlayerMotion a(0);a.tick(soew::Right);a.tick(soew::Right);check(a.x==217);soew::PlayerMotion diag(0);diag.tick(soew::Up|soew::Left);diag.tick(soew::Up|soew::Left);check(diag.x==201&&diag.y==329);
    for(int shot=0;shot<3;++shot)for(unsigned page=0;page<2;++page)
        for(std::uint16_t direction:{soew::Up,soew::Down,soew::Left,soew::Right,soew::UpLeft,soew::UpRight,soew::DownLeft,soew::DownRight}){
            soew::PlayerMotion normal(shot,page),slow(shot,page);
            normal.x=slow.x=200;normal.y=slow.y=200;
            int dx=0,dy=0;
            for(int tick=0;tick<12;++tick){
                normal.tick(direction);slow.tick(direction|soew::Focus);
                dx+=normal.last_dx;dy+=normal.last_dy;
                check(2*(slow.x-200)+slow.focus_rx==dx&&2*(slow.y-200)+slow.focus_ry==dy);
                check(slow.focused&&slow.pose==normal.pose&&slow.back_page==normal.back_page);
            }
            slow.tick(direction);normal.tick(direction);
            check(!slow.focused&&!slow.focus_rx&&!slow.focus_ry&&slow.last_dx==normal.last_dx&&slow.last_dy==normal.last_dy);
        }
    soew::Keys held;held.focus=true;
    soew::PlayerMotion wall(2);wall.x=29;
    check(soew::sense(held)==soew::Focus);
    wall.tick(soew::Left|soew::Focus);check(wall.x==28&&wall.focus_rx==0);
    wall.tick(soew::Right|soew::Focus);check(wall.x==29&&wall.focus_rx==1);
    wall.tick(soew::Focus);check(wall.x==29&&wall.focused);
    wall.tick(soew::Right|soew::Focus);check(wall.x==31);
    wall.tick(soew::Right);check(wall.x==34&&!wall.focused);
    // Replay same raw source masks, deterministic independent state.
    soew::PlayerMotion one(1),two(1);soew::IRand rng(1234);for(int i=0;i<10000;++i){const auto input=static_cast<std::uint16_t>(rng.next());one.tick(input);two.tick(input);check(one.x==two.x&&one.y==two.y&&one.pose==two.pose&&one.back_page==two.back_page);}
    std::cout<<"player_motion_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
