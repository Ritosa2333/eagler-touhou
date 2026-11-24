#pragma once
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace th128::hud {
struct RewardGlyph {int sprite=0;float x=0,y=0,scale=1;std::uint32_t color=0;};
class FreezeRewardPanel {
public:
    bool complete(float percent,int bonus,float playerY){
        if(!std::isfinite(percent)||percent<0||percent>100||bonus<0||bonus>1000000||!validY(playerY))return false;
        percent_=percent;bonus_=bonus;age_=16;time_=16;active_=true;near_=false;high_=playerY>128&&playerY<192;++serial_;
        return true;
    }
    bool tick(float playerY){
        if(!validY(playerY))return false;
        if(!active_)return true;
        if(age_>=120)active_=false;
        if(!near_&&playerY<224){near_=true;fade_=0;}
        else if(near_&&playerY>256){near_=false;fade_=0;}
        ++age_;time_+=1;++fade_;return true;
    }
    std::vector<RewardGlyph> glyphs()const{
        std::vector<RewardGlyph> result;if(!active_)return result;
        const float percent=age_<30?float(double(time_)*double(percent_)/30.):percent_;
        const int centis=int(double(percent)*100.);
        const float baseY=high_?64.f:160.f;
        const std::uint8_t areaAlpha=fade_>8?(near_?64:255):near_?std::uint8_t(255-(fade_<<4)):std::uint8_t((fade_+4)<<4);
        const std::uint8_t bonusAlpha=fade_>8?(near_?64:255):near_?std::uint8_t((fade_<<6)-1):std::uint8_t((1-fade_)*64);
        const auto areaColor=(std::uint32_t(areaAlpha)<<24)|0x00b0e0ffu;
        const auto bonusColor=(std::uint32_t(bonusAlpha)<<24)|0x00f0f0d0u;
        const auto integer=std::to_string(centis/100);
        append(result,integer,320-float(integer.size())*40,baseY,2,11,areaColor);
        append(result,".",320,baseY+20,1,11,areaColor);
        const int fraction=centis%100;const auto fractionText=(fraction<10?"0":"")+std::to_string(fraction);
        append(result,fractionText,332,baseY+20,1,11,areaColor);
        append(result,"%",371,baseY,2,11,areaColor);
        if(age_>=30){
            const auto text=bonus_<10?"0"+std::to_string(bonus_):std::to_string(bonus_);
            append(result,text,320-float(text.size())*7,baseY+50,1,5,bonusColor);
        }
        return result;
    }
    std::uint32_t serial()const{return serial_;}
    bool active()const{return active_;}
    bool high()const{return high_;}
    bool near()const{return near_;}
    int age()const{return age_;}
    int fade()const{return fade_;}
    float percent()const{return percent_;}
    int bonus()const{return bonus_;}
private:
    static bool validY(float value){return std::isfinite(value)&&std::abs(value)<=65536;}
    static void append(std::vector<RewardGlyph>& output,const std::string& text,float x,float y,float scale,int style,std::uint32_t color){
        for(const unsigned char character:text){
            const int sprite=style==11?(character=='.'?301:character=='%'?302:int(character)+243):int(character)+195;
            output.push_back({sprite,x,y,scale,color});x+=float(style==11?20:14)*scale;
        }
    }
    std::uint32_t serial_=0;
    bool active_=false,high_=false,near_=false;
    int age_=0,fade_=0,bonus_=0;
    float time_=0,percent_=0;
};
}
