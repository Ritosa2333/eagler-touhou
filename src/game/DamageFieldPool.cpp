#include "DamageFieldPool.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <utility>

namespace th128::damagefield {
namespace {
std::uint32_t word(const Image& image,std::size_t offset){
    return std::uint32_t(image[offset])|std::uint32_t(image[offset+1])<<8|std::uint32_t(image[offset+2])<<16|std::uint32_t(image[offset+3])<<24;
}
int integer(const Image& image,std::size_t offset){return std::bit_cast<std::int32_t>(word(image,offset));}
float scalar(const Image& image,std::size_t offset){return std::bit_cast<float>(word(image,offset));}
void store(Image& image,std::size_t offset,std::uint32_t value){
    for(std::size_t index=0;index<4;++index)image[offset+index]=std::uint8_t(value>>(index*8));
}
void storeFloat(Image& image,std::size_t offset,float value){store(image,offset,std::bit_cast<std::uint32_t>(value));}
bool bounded(float value){return std::isfinite(value)&&std::abs(value)<=65536;}
bool valid(Position position){return bounded(position.x)&&bounded(position.y)&&bounded(position.z);}
bool stationary(const Image& image){
    for(std::size_t offset=0x24;offset<=0x48;offset+=4)if(word(image,offset))return false;
    for(std::size_t offset:{0x18u,0x1cu,0x20u})if(!bounded(scalar(image,offset)))return false;
    return true;
}
}
std::optional<Image> circularBirth(Position position,float radius,float growth,int duration,int damage,std::uint32_t tag) noexcept {
    if(!valid(position)||!bounded(radius)||!bounded(growth)||radius<0||growth<0||duration<1||duration>65535||damage<0||damage>200000)return std::nullopt;
    Image result{};storeFloat(result,0,radius);storeFloat(result,4,growth);
    storeFloat(result,0x18,position.x);storeFloat(result,0x1c,position.y);storeFloat(result,0x20,position.z);
    store(result,0x4c,std::uint32_t(duration-1));store(result,0x50,std::uint32_t(duration));storeFloat(result,0x54,float(duration));
    store(result,0x58,0x4b6f58);store(result,0x5c,1);store(result,0x60,std::uint32_t(damage));
    store(result,0x68,999999);store(result,0x6c,4);store(result,0x70,tag);store(result,0x74,3);return result;
}
std::optional<Image> growAndExpireUnit(const Image& image) noexcept {
    if(!(word(image,0x74)&1u))return image;
    if(!bounded(scalar(image,0))||!bounded(scalar(image,4))||!bounded(scalar(image,8))||!bounded(scalar(image,0xc))||
       word(image,0x58)!=0x4b6f58||!(word(image,0x5c)&1u)||integer(image,0x50)<1||integer(image,0x50)>65535||
       scalar(image,0x54)!=float(integer(image,0x50)))return std::nullopt;
    Image result=image;storeFloat(result,0,float(double(scalar(image,0))+scalar(image,4)));
    storeFloat(result,8,float(double(scalar(image,8))+scalar(image,0xc)));
    store(result,0x4c,word(image,0x50));store(result,0x50,word(image,0x50)-1);
    storeFloat(result,0x54,float(double(scalar(image,0x54))-1.));
    if(integer(result,0x50)<=0)store(result,0x74,word(result,0x74)&~1u);
    return result;
}
std::optional<Contact> queryCircular(const Image& image,Position enemy,std::uint32_t outFlag) noexcept {
    Contact result{image,0,outFlag};
    if(!(word(image,0x74)&1u))return result;
    if(integer(image,0x50)!=integer(image,0x4c)){
        const auto interval=integer(image,0x6c);
        if(interval<=0)return std::nullopt;
        if(integer(image,0x50)%interval==0)return result;
    }
    const Position position{scalar(image,0x18),scalar(image,0x1c),scalar(image,0x20)};
    const auto radius=scalar(image,0);
    if(!(word(image,0x74)&2u)||!valid(position)||!valid(enemy)||!bounded(radius)||radius<0||integer(image,0x60)<0)return std::nullopt;
    const double dx=double(position.x)-enemy.x,dy=double(position.y)-enemy.y;
    const float squaredDistance=float(dx*dx+dy*dy);
    if(double(squaredDistance)>double(radius)*radius)return result;
    if(word(image,0x70))result.outFlag|=2u;
    result.damage=integer(image,0x60);
    store(result.next,0x64,word(image,0x64)+word(image,0x60));
    if(integer(result.next,0x64)>=integer(image,0x68))store(result.next,0x60,0);
    return result;
}
std::optional<FinalDamage> finishDamageQuery(int shotDamage,int sourceDamage,int score) noexcept {
    if(shotDamage<0||sourceDamage<0||score<0||score>999999999||sourceDamage>400000000)return std::nullopt;
    const int damage=std::min(std::min(shotDamage,160)+sourceDamage,2000);
    const int reward=damage?(damage/10+10)/10:0;
    return FinalDamage{damage,std::min(score+reward,999999999)};
}
bool DamageFieldPool::fail(std::string reason,std::string& error){if(failure_.empty())failure_=std::move(reason);error=failure_;return false;}
bool DamageFieldPool::configure(std::span<const Image> images,std::uint32_t cursor,std::string& error){
    error.clear();if(configured_||!failure_.empty()||images.size()!=Capacity||cursor>=Capacity){error="invalid fresh damage-field pool context";return false;}
    images_.assign(images.begin(),images.end());cursor_=cursor;configured_=true;return true;
}
std::optional<Birth> DamageFieldPool::spawn(Position position,float radius,float growth,int duration,int damage,std::uint32_t tag,std::string& error){
    error.clear();if(!configured_||!failure_.empty()){fail(failure_.empty()?"unconfigured damage-field pool":failure_,error);return std::nullopt;}
    const auto born=circularBirth(position,radius,growth,duration,damage,tag);
    if(!born){fail("unsupported circular damage-field birth domain",error);return std::nullopt;}
    std::uint32_t selected=cursor_;cursor_=(cursor_+1)%Capacity;
    for(std::uint32_t checked=0;checked<Capacity;++checked){
        if(!(word(images_[selected],0x74)&1u)){images_[selected]=*born;return Birth{selected,true};}
        selected=cursor_;cursor_=(cursor_+1)%Capacity;
    }
    return Birth{selected,false};
}
bool DamageFieldPool::tickUnit(std::string& error){
    error.clear();if(!configured_||!failure_.empty())return fail(failure_.empty()?"unconfigured damage-field update":failure_,error);
    for(auto& image:images_){
        if(!(word(image,0x74)&1u))continue;
        if(!stationary(image))return fail("unimplemented moving damage-field motion at 0x43ba1a",error);
        for(std::size_t offset:{0x18u,0x1cu,0x20u})storeFloat(image,offset,float(double(scalar(image,offset))+0.));
        for(std::size_t offset:{0x18u,0x1cu}){
            const float scaled=float(double(scalar(image,offset))*100.);
            storeFloat(image,offset,float(std::floor(double(scaled))/100.));
        }
        const auto next=growAndExpireUnit(image);if(!next)return fail("unsupported damage-field unit timer",error);image=*next;
    }
    return true;
}
std::optional<PoolContact> DamageFieldPool::query(Position enemy,std::uint32_t outFlag,std::string& error){
    error.clear();if(!configured_||!failure_.empty()){fail(failure_.empty()?"unconfigured damage-field query":failure_,error);return std::nullopt;}
    PoolContact result{0,outFlag};std::uint32_t total=0;
    for(auto& image:images_){
        const auto contact=queryCircular(image,enemy,result.outFlag);
        if(!contact){fail("unimplemented damage-field shape/contact at 0x43fd66",error);return std::nullopt;}
        image=contact->next;total+=std::uint32_t(contact->damage);result.outFlag=contact->outFlag;
    }
    result.damage=std::bit_cast<std::int32_t>(total);return result;
}
const Image& DamageFieldPool::image(std::uint32_t slot) const{return images_.at(slot);}
std::uint32_t DamageFieldPool::cursor() const{return cursor_;}
std::array<std::uint32_t,Capacity> DamageFieldPool::flags() const{
    std::array<std::uint32_t,Capacity> result{};
    for(std::size_t index=0;index<images_.size();++index)result[index]=word(images_[index],0x74);
    return result;
}
}
