#pragma once
#include "game/NativeReplayNameProfile.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>
namespace th128::record {
class NativeReplaySaveMenu{
    std::array<std::uint8_t,8> name_{32,32,32,32,32,32,32,32};
    std::array<bool,25> occupied_{};
    std::vector<int> sounds_;
    unsigned slot_=0,selected_=0,length_=0,phase_=0,serial_=0,confirm_=1,age_=0,nameEpoch_=0;
    bool gameOver_=false;
    NativeReplayNameProfile profile_;
    static constexpr char Characters[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*";
    void nameEntry(){
        if(nameEpoch_==0xffffffffu)throw std::runtime_error("original name epoch exhausted");
        ++nameEpoch_;phase_=2;age_=0;selected_=0;length_=0;name_.fill(32);
        if(profile_.bound()){
            std::copy_n(profile_.name().begin(),8,name_.begin());length_=8;
            while(length_&&name_[length_-1]==32)--length_;
            if(length_)selected_=90;
        }
    }
    void move(int delta,unsigned count){selected_=unsigned((int(selected_)+delta+int(count))%int(count));}
public:
    void open(bool gameOver,const std::array<bool,25>& occupied){
        if(phase_&&phase_!=5)throw std::runtime_error("replay save menu already active");
        profile_={};occupied_=occupied;gameOver_=gameOver;phase_=1;slot_=0;selected_=0;age_=0;sounds_.clear();
    }
    void tick(std::uint32_t pressed){
        if(!phase_||phase_==5)throw std::runtime_error("replay save input without active owner");
        if(phase_==3)return;
        ++age_;if(age_<10)return;
        if(phase_==1){
            const auto before=slot_;
            if(pressed&0x10u)slot_=(slot_+24)%25;
            else if(pressed&0x20u)slot_=(slot_+1)%25;
            if(slot_!=before)sounds_.push_back(10);
            if(pressed&0x102u){phase_=5;age_=0;sounds_.push_back(9);}
            else if(pressed&0x80201u){sounds_.push_back(7);nameEntry();}
            return;
        }
        const auto before=selected_;
        if(pressed&0x10u)move(-13,91);
        else if(pressed&0x20u)move(13,91);
        else if(pressed&0x40u)move(selected_%13?-1:12,91);
        else if(pressed&0x80u)move(selected_%13==12?-12:1,91);
        if(selected_!=before)sounds_.push_back(10);
        if(pressed&0x102u){
            sounds_.push_back(9);if(length_)name_[--length_]=32;else{phase_=1;age_=0;}
        }else if(pressed&0x80201u){
            if(selected_<88){name_[std::min(length_,7u)]=std::uint8_t(Characters[selected_]);if(length_<8)++length_;if(length_==8)selected_=90;sounds_.push_back(7);}
            else if(selected_==88){name_[std::min(length_,7u)]=32;if(length_<8)++length_;if(length_==8)selected_=90;sounds_.push_back(7);}
            else if(selected_==89){if(length_){name_[--length_]=32;sounds_.push_back(9);}}
            else{phase_=3;++serial_;sounds_.push_back(18);}
        }
    }
    void acceptSaved(unsigned serial,unsigned slot){
        if(phase_!=3||serial!=serial_||slot!=slot_)throw std::runtime_error("native replay save acknowledgement identity rejected");
        if(profile_.bound())profile_.accept(serial);
        occupied_[slot_]=true;phase_=1;age_=0;
    }
    void bindNameProfile(std::span<const std::uint8_t> bytes){
        if(phase_==1&&nameEpoch_==0&&!profile_.bound())profile_.bind(bytes);
        else if(phase_==3&&profile_.bound())profile_.prepare(bytes,serial_,name_);
        else throw std::runtime_error("last-name profile packet has no matching native owner phase");
    }
    bool profileBound()const{return profile_.bound();}
    std::uint64_t profileRevision()const{return profile_.revision();}
    unsigned phase()const{return phase_;}
    unsigned nameAge()const{return age_;}
    unsigned nameEpoch()const{return nameEpoch_;}
    unsigned slot()const{return slot_;}
    unsigned selected()const{return selected_;}
    unsigned length()const{return length_;}
    unsigned serial()const{return serial_;}
    unsigned confirmSelection()const{return confirm_;}
    bool pending()const{return phase_==3;}
    bool finished()const{return phase_==5;}
    bool gameOver()const{return gameOver_;}
    const auto& name()const{return name_;}
    const auto& sounds()const{return sounds_;}
    static unsigned character(unsigned index){if(index<88)return std::uint8_t(Characters[index]);return index==88?0x81:index==89?0x7f:0x80;}
};
}
