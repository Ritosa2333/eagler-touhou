#include "game/IceContact.hpp"
#include <iostream>
int main(){int policy=0,age=0;bool overridePolicy=false;while(std::cin>>policy>>age>>overridePolicy){
    std::cout<<int(th128::ice::canFreezeBullet(1,policy,age,overridePolicy))<<'\n';
    if(th128::ice::canFreezeBullet(0,policy,age,overridePolicy))return 2;
}}
