#include "Hud.hpp"
namespace th17 {
bool Hud::begin_stage_clear(i32 bonus){
    stage_clear_notice=animations.create(resources.front,123,-1,0);if(!check(stage_clear_notice!=0,"Stage clear animation creation failed"))return false;stage_clear_bonus=bonus;flags|=0x100;notice_age.set(0);return true;
}
bool Hud::update_notices(float rate){
    if(flags&0x100)notice_age.tick(&rate);
    if(!(flags&0x1800))return true;
    notice_age.tick(&rate);auto& result=chapter_result;
    if((flags&0x1800)==0x800&&notice_age.current>89){
        if(result.percentage<=0){if(notice_age.current!=90&&(!sound||!sound(47)))return check(false,"HUD result sound unavailable");result.percentage=result.initial_percentage;result.increment=0;result.score=result.target;flags=(flags&~0x800u)|0x1000;}
        else{if(notice_age.current%4==0&&(!sound||!sound(39)))return check(false,"HUD result sound unavailable");result.score=wrapping_add(result.score,result.increment);result.percentage=float(result.percentage-1.f);}
    }
    if(result.duration<=notice_age.current){if(!animations.interrupt(chapter_result_notice,1))return check(false,"HUD result interrupt failed");flags=(flags&~0x1000u)|0x800;notice_age.set(0);flags&=~0x1800u;}
    return true;
}
bool Hud::result(i32 amount,i32 kind){
    if(kind<0||kind>6)return check(false,"HUD result type outside TH17 range");
    if(kind==5)return true;
    const auto create=[&](u32& handle,i32 bank,i32 script){handle=animations.create(bank,script,-1,0);return check(handle!=0,"HUD result animation creation failed");};
    if(kind>=2&&kind<=4){return animations.retire(pickup_banner)&&create(pickup_banner,resources.front,61+kind);}
    if(!animations.retire(result_banner)||!create(result_banner,resources.front,kind==0?61:kind==1?62:66))return false;
    if(kind==6)return true;
    if(kind==0){
        i32 remaining=amount,divisor=10000000;bool significant=false;
        for(i32 i=0;i<8;i++){
            auto& handle=bonus_digits[i];if(!animations.retire(handle)||!create(handle,resources.ascii,i+4))return false;
            const i32 digit=remaining/divisor;remaining%=divisor;divisor/=10;if(digit)significant=true;
            auto* vm=find(handle);if(!check(vm&&vm->select_sprite(digit+239),"HUD bonus digit sprite unavailable")||!visible(handle,significant))return false;
        }
        for(i32 i=0;i<2;i++){auto& handle=bonus_separators[i];if(!animations.retire(handle))return false;if(amount>=(i?1000:1000000)){if(!create(handle,resources.ascii,12+i))return false;auto* vm=find(handle);if(!check(vm&&vm->select_sprite(253),"HUD bonus separator unavailable"))return false;}}
    }
    result_active=1;return create(result_panel,resources.front,96);
}
}
