#pragma once
#include <bit>
#include <cmath>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace th128::postrun {
struct MediaCommand {
    unsigned kind=0,serial=0;
    int operation=0,identifier=-1,argument=0;
    float position=0;
};
class Media {
    unsigned epoch_,nextSerial_=1,windowSerial_=0,windowCount_=0,windowDigest_=0;
    std::vector<MediaCommand> pending_;
    static void word(unsigned& digest,unsigned value){
        for(unsigned byte=0;byte<4;++byte){digest^=std::uint8_t(value>>(byte*8));digest*=16777619u;}
    }
    void append(unsigned kind,int operation,int identifier,int argument,float position){
        if(windowSerial_||!nextSerial_||pending_.size()>=128||!std::isfinite(position))
            throw std::runtime_error("postrun media producer window or identity budget rejected");
        pending_.push_back({kind,nextSerial_++,operation,identifier,argument,position});
    }
public:
    explicit Media(unsigned epoch):epoch_(epoch){if(!epoch)throw std::runtime_error("postrun media epoch is absent");}
    void music(int operation,int track,int helperArgument){
        if(operation==1){if(track!=7&&track!=8)throw std::runtime_error("unbound postrun music track");append(0,1,track,0,0);}
        else if(operation==2){
            if(track!=7&&track!=8||helperArgument<0||helperArgument>15)throw std::runtime_error("unbound postrun music helper");
            append(0,2,track,0,0);
        }else if(operation==3||operation==4)append(0,operation,-1,0,0);
        else throw std::runtime_error("unbound native postrun music operation");
    }
    void sound(int identifier,float position,int operation){
        if(identifier<0||identifier>=76||operation<0||operation>2)throw std::runtime_error("unbound postrun sound request");
        append(1,operation,identifier,0,position);
    }
    std::string window(){
        if(!pending_.empty()&&!windowSerial_){
            windowSerial_=pending_.front().serial;windowCount_=unsigned(pending_.size());windowDigest_=2166136261u;
            for(const auto& command:pending_){
                word(windowDigest_,command.kind);word(windowDigest_,command.serial);
                word(windowDigest_,std::bit_cast<unsigned>(command.operation));word(windowDigest_,std::bit_cast<unsigned>(command.identifier));
                word(windowDigest_,std::bit_cast<unsigned>(command.argument));word(windowDigest_,std::bit_cast<unsigned>(command.position));
            }
        }
        std::ostringstream output;output<<"{\"epoch\":"<<epoch_<<",\"serial\":"<<windowSerial_
            <<",\"digest\":"<<windowDigest_<<",\"commands\":[";
        for(unsigned index=0;index<pending_.size();++index){
            if(index)output<<',';const auto& command=pending_[index];
            output<<"{\"kind\":"<<command.kind<<",\"serial\":"<<command.serial<<",\"operation\":"<<command.operation
                <<",\"identifier\":"<<command.identifier<<",\"argument\":"<<command.argument<<",\"position\":"<<command.position<<'}';
        }
        output<<"]}";return output.str();
    }
    void acknowledge(unsigned epoch,unsigned serial,unsigned count,unsigned digest){
        if(!windowSerial_||epoch!=epoch_||serial!=windowSerial_||count!=windowCount_||digest!=windowDigest_)
            throw std::runtime_error("postrun media acknowledgement identity rejected");
        pending_.erase(pending_.begin(),pending_.begin()+count);
        windowSerial_=windowCount_=windowDigest_=0;
    }
    bool empty()const{return pending_.empty()&&!windowSerial_;}
};
}
