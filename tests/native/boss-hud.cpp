#include "game/BossHud.hpp"
#include "game/RingGeometry.hpp"
#include "game/EnemyPhaseRules.hpp"
#include <bit>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace {
void require(bool value,const std::string& message){if(!value)throw std::runtime_error(message);}
}
int main(int argc,char** argv){try{
    require(argc==2,"front ANM argument required");
    std::ifstream file(argv[1],std::ios::binary);const std::vector<unsigned char> bytes{std::istreambuf_iterator<char>(file),{}};
    char command=0;
    while(std::cin>>command){
        if(command=='F'){
            int life=0,maximum=0;float previous=0;std::cin>>life>>maximum>>previous;
            const auto next=th128::hud::bossFill(life,maximum,previous);require(bool(next),"native fill input rejected");
            std::cout<<std::bit_cast<std::uint32_t>((*next)[0])<<' '<<std::bit_cast<std::uint32_t>((*next)[1])<<'\n';
        }else if(command=='T'){
            int seconds=0,previous=0;bool present=false,message=false,hidden=false;std::cin>>seconds>>previous>>present>>message>>hidden;
            const auto result=th128::hud::bossTimerNotice(seconds,previous,present,message,hidden);
            std::cout<<result.interrupt<<' '<<result.sound<<'\n';
        }else if(command=='M'){
            float angle=0;std::cin>>angle;const auto position=th128::hud::bossMarkerPosition(angle,320,116,0);
            std::cout<<std::bit_cast<std::uint32_t>(position[0])<<' '<<std::bit_cast<std::uint32_t>(position[1])<<' '<<std::bit_cast<std::uint32_t>(position[2])<<'\n';
        }else if(command=='R'){
            th128::anm::State state;std::cin>>state.mode>>state.rx>>state.rotation>>state.sx>>state.sy>>state.geometryCount;state.integers[1]=1;
            th128::anm::SpriteInfo sprite;sprite.u0=0;sprite.u1=.25f;
            const auto result=th128::geometry::ring(state,sprite,320,116,0);require(bool(result),"native ring input rejected");
            bool comma=false;for(const auto& vertex:*result){if(comma)std::cout<<' ';comma=true;
                std::cout<<std::bit_cast<std::uint32_t>(vertex.x)<<' '<<std::bit_cast<std::uint32_t>(vertex.y)<<' '<<std::bit_cast<std::uint32_t>(vertex.z)<<' '
                    <<std::bit_cast<std::uint32_t>(vertex.rhw)<<' '<<vertex.color<<' '<<std::bit_cast<std::uint32_t>(vertex.u)<<' '<<std::bit_cast<std::uint32_t>(vertex.v);}
            std::cout<<'\n';
        }else throw std::runtime_error("unknown fixture command");
    }
    th128::hud::BossHud hud;std::string error;require(hud.load(bytes,error),error);require(hud.boot(error),error);
    th128::hud::BossInput input;input.slots[0]={143,9200,9200,0,0,0,100,0};input.countdown={30,0};input.playerX=0;input.playerY=400;
    input.markers[0][0]={.5f,0xffff8080u,true};
    require(hud.tick(input,error),error);auto state=hud.state();require(state.timerVisible&&state.seconds==30&&state.hundredths==0,"original countdown fields must be retained");
    require(state.fills[0]>0&&state.fills[0]<.026f,"real Boss fill must animate from zero");
    auto draws=hud.snapshots();require(draws.size()==7,"original seven ring/marker roots must exist");
    require(draws[0].state.mode==14&&draws[0].state.geometryCount==38,"front162 must own original 38-pair allocated arc");
    require(draws[0].x==320&&draws[0].y==116,"Boss HP ring must follow original game viewport");
    for(int tick=0;tick<45;++tick)require(hud.tick(input,error),error);
    input.slots[0].life=4600;input.countdown={29,98};require(hud.tick(input,error),error);
    require(hud.state().fills[0]==.5f&&hud.state().hundredths==98,"HP loss is immediate, countdown is not rounded");
    draws=hud.snapshots();require(!(draws[3].state.flags&2u),"marker equal to filled ratio is not drawn");
    input.message=true;require(hud.tick(input,error),error);require(!hud.state().timerVisible&&hud.snapshots().empty(),"dialogue hides timer and retires rings");
    input.message=false;require(hud.tick(input,error),error);require(hud.snapshots().size()==7,"dialogue completion recreates original ring owners");
    input.slots[0]={};require(hud.tick(input,error),error);require(!hud.state().timerVisible&&hud.state().fills[0]==0,"unbound Boss must clear display state");
    th128::enemy::PhaseContext phase;phase.life=9200;phase.records[0]={0,1800,"Health","Timeout"};phase.spell=th128::enemy::PhaseSpell{};
    for(const auto elapsed:{0,1,59,60,1799,1800}){phase.timer.current=elapsed;const auto result=th128::enemy::queryPhase(phase);require(bool(result),"native phase query rejected");
        const auto remaining=1800-elapsed;require(result->next.countdown[0]==std::uint32_t(remaining/60)&&result->next.countdown[1]==std::uint32_t((remaining%60)*100/60),"original 60Hz timer boundary mismatch");
        require((elapsed==1800)==result->timedOut,"original exact timeout frame mismatch");}
    std::cout<<"original Boss HUD lifecycle and frame countdown passed\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
