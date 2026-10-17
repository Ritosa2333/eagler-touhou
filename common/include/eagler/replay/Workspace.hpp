#pragma once
#include <cstdint>
#include <string>

namespace eagler::replay {
// A read-only description from the title's native decoder. Scores are display
// values (including the title's original units), never inferred by the host.
class Description {
    static std::string quote(const std::string& text) {
        std::string out="\"";
        for(unsigned char c:text) {if(c=='"'||c=='\\')out+='\\';if(c>=32)out+=char(c);}
        return out+'"';
    }
public:
    std::string value="{",stages="[";
    void number(const char* key,double number){field(key,std::to_string(number));}
    void text(const char* key,const std::string& text){field(key,quote(text));}
    // Player names in retail files are CP932, while native labels are UTF-8.
    void encoded_name(const char* bytes,unsigned length){std::string out="[";for(unsigned i=0;i<length&&bytes[i];++i){if(i)out+=',';out+=std::to_string(static_cast<unsigned char>(bytes[i]));}field("nameBytes",out+"]");}
    void field(const char* key,const std::string& raw){if(value.size()>1)value+=',';value+=quote(key)+':'+raw;}
    void stage(unsigned id,double score,std::uint32_t frames=0,const std::string& label=""){
        if(stages.size()>1)stages+=',';
        stages+="{\"id\":"+std::to_string(id)+",\"score\":"+std::to_string(score)+",\"frames\":"+std::to_string(frames);
        if(!label.empty())stages+=",\"label\":"+quote(label);
        stages+='}';
    }
    void stage_without_score(unsigned id,std::uint32_t frames=0,const std::string& label=""){
        if(stages.size()>1)stages+=',';
        stages+="{\"id\":"+std::to_string(id)+",\"score\":null,\"frames\":"+std::to_string(frames);
        if(!label.empty())stages+=",\"label\":"+quote(label);
        stages+='}';
    }
    std::string finish(){field("stages",stages+"]");return value+'}';}
};
}
