#include "game/EnemyRules.hpp"
#include <bit>
#include <cassert>
#include <iostream>
float scalar(){std::uint32_t bits=0;std::cin>>bits;return std::bit_cast<float>(bits);}
void vector(th128::enemy::Position& value){value={scalar(),scalar(),scalar()};}
int main(){char command=0;
    while(std::cin>>command){
        th128::enemy::PositionInterpolation interpolation;th128::enemy::Position position;
        if(command=='I'){
            vector(position);const auto x=scalar(),y=scalar();int duration=0,mode=0;std::cin>>duration>>mode;
            assert(th128::enemy::beginPositionInterpolation(interpolation,position,duration,mode,x,y));
            std::cout<<interpolation.duration<<' '<<interpolation.mode<<' '<<interpolation.previous<<' '<<interpolation.elapsed;
            for(const float value:{interpolation.elapsedFloat,interpolation.start.x,interpolation.start.y,interpolation.start.z,
                interpolation.target.x,interpolation.target.y,interpolation.target.z})std::cout<<' '<<std::bit_cast<std::uint32_t>(value);
        }else if(command=='T'){
            vector(interpolation.start);vector(interpolation.target);std::cin>>interpolation.duration>>interpolation.elapsed;
            interpolation.previous=interpolation.elapsed-1;interpolation.elapsedFloat=float(interpolation.elapsed);interpolation.mode=4;
            const auto result=th128::enemy::stepPositionInterpolation(interpolation);assert(result);
            std::cout<<interpolation.duration<<' '<<interpolation.previous<<' '<<interpolation.elapsed<<' '<<std::bit_cast<std::uint32_t>(interpolation.elapsedFloat);
            for(const float value:{result->x,result->y,result->z})std::cout<<' '<<std::bit_cast<std::uint32_t>(value);
        }else return 2;
        std::cout<<'\n';
    }
    th128::enemy::PositionInterpolation invalid;assert(!th128::enemy::beginPositionInterpolation(invalid,{},1,17,0,0));
    assert(th128::enemy::beginPositionInterpolation(invalid,{},0,17,0,0));
    assert(!th128::enemy::stepPositionInterpolation(invalid));
}
