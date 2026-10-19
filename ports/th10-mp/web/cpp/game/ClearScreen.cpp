#include "ClearScreen.hpp"
namespace th10 {
void ClearScreen::prepare(ClearScreenEnvironment& env){
    if(*env.animations)env.flush(**env.animations);
    *env.viewport={0,0,640,480,0,1};env.set_viewport(*env.device,*env.viewport);
}
bool ClearScreen::advance(ClearScreenEnvironment& env){
    if(submitted>=2)return true;
    env.clear(*env.device,color);result=env.present(*env.device);
    if(result<0)result=env.reset(*env.device,*env.parameters);
    return ++submitted==2;
}
}
