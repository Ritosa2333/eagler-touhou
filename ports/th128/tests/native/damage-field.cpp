#include "game/DamageFieldPool.hpp"
#include <bit>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace th128::damagefield;
Image readImage(){
    std::string text;std::cin>>text;assert(text.size()==240);Image image{};
    for(std::size_t index=0;index<image.size();++index)image[index]=std::uint8_t(std::stoul(text.substr(index*2,2),nullptr,16));
    return image;
}
void writeImage(const Image& image){
    const auto previous=std::cout.flags();
    for(auto value:image)std::cout<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(value);
    std::cout.flags(previous);
}
float readFloat(){std::uint32_t value=0;std::cin>>value;return std::bit_cast<float>(value);}
int main(){
    char command=0;
    while(std::cin>>command){
        if(command=='B'){
            int duration=0;std::cin>>duration;
            const auto image=circularBirth({0,400,.1f},32,16,duration,150,0);assert(image);
            writeImage(*image);std::cout<<'\n';
        }else if(command=='T'){
            const auto image=readImage();const auto next=growAndExpireUnit(image);assert(next);
            writeImage(*next);std::cout<<'\n';
        }else if(command=='M'){
            const auto image=readImage();std::vector<Image> images(Capacity);images[0]=image;
            DamageFieldPool pool;std::string error;assert(pool.configure(images,0,error));
            assert(pool.tickUnit(error));writeImage(pool.image(0));std::cout<<'\n';
        }else if(command=='Q'){
            const auto image=readImage();const Position position{readFloat(),readFloat(),readFloat()};
            int score=0;std::cin>>score;
            const auto result=queryCircular(image,position,0);assert(result);
            const auto final=finishDamageQuery(0,result->damage,score);assert(final);
            std::cout<<final->damage<<' '<<result->outFlag<<' '<<final->score<<' ';
            writeImage(result->next);std::cout<<'\n';
        }else if(command=='P'){
            std::uint32_t cursor=0,count=0;int full=0;std::cin>>cursor>>full>>count;
            std::vector<Image> images(Capacity);
            if(full)for(auto& image:images)image[0x74]=3;
            for(unsigned index=0;index<count;++index){unsigned slot=0;std::cin>>slot;images[slot][0x74]=3;}
            DamageFieldPool pool;std::string error;assert(pool.configure(images,cursor,error));
            const auto result=pool.spawn({0,400,.1f},32,16,30,150,0,error);assert(result);
            std::cout<<result->slot<<' '<<pool.cursor()<<' ';writeImage(pool.image(result->slot));std::cout<<'\n';
        }else if(command=='F'){
            int shots=0,sources=0,score=0;std::cin>>shots>>sources>>score;
            const auto result=finishDamageQuery(shots,sources,score);assert(result);
            std::cout<<result->damage<<' '<<result->score<<'\n';
        }else return 2;
    }
}
