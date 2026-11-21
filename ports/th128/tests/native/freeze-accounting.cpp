#include "game/FreezeAccounting.hpp"
#include <cassert>
#include <iostream>
#include <iomanip>
int main(){
    assert(!th128::freeze::account(172033,0));assert(!th128::freeze::account(0,-1));
    unsigned pixels;int score;
    while(std::cin>>pixels>>score){const auto result=th128::freeze::account(pixels,score);assert(result);
        std::cout<<std::setprecision(17)<<result->percent<<' '<<result->bonus<<' '<<result->score<<'\n';}
}
