#include "SessionSetup.hpp"
namespace th11::multiplayer {
bool SessionSetup::valid()const{
    if(!session_id||player_count<2||player_count>3||local_player>=player_count||difficulty>4||seed>65535||
       requested_delay>9||input_delay>9||prediction_reserve<1||prediction_reserve>2||
       challenge||(automatic&&requested_delay)||!(build[0]|build[1]|build[2]|build[3]))return false;
    for(unsigned s=0;s<3;++s)if(selections[s]>5||(s>=player_count&&selections[s]))return false;
    return true;
}
bool DecodeSessionSetup(SessionSetup& out,const std::uint32_t* w,std::size_t n){
    if(!w||n!=SessionSetup::Words||w[0]!=SessionSetup::Version||w[14]!=1||w[15]>1||w[21]>1)return false;
    SessionSetup s;s.player_count=w[1];s.local_player=w[2];s.difficulty=w[3];s.seed=w[4];
    s.session_id=w[5]|(std::uint64_t(w[6])<<32);s.requested_delay=s.input_delay=w[7];
    for(unsigned i=0;i<3;++i){if(w[8+i*2]>1||w[9+i*2]>2)return false;s.selections[i]=w[8+i*2]*3+w[9+i*2];}
    s.automatic=w[15];s.prediction_reserve=w[16];for(unsigned i=0;i<4;++i)s.build[i]=w[17+i];s.challenge=w[21];
    if(!s.valid())return false;out=s;return true;
}
std::array<std::uint32_t,SessionSetup::Words> EncodeSessionSetup(const SessionSetup& s){
    std::array<std::uint32_t,SessionSetup::Words> w{};
    w[0]=SessionSetup::Version;w[1]=s.player_count;w[2]=s.local_player;w[3]=s.difficulty;w[4]=s.seed;
    w[5]=std::uint32_t(s.session_id);w[6]=std::uint32_t(s.session_id>>32);w[7]=s.requested_delay;
    for(unsigned i=0;i<3;++i){w[8+i*2]=s.selections[i]/3;w[9+i*2]=s.selections[i]%3;}
    w[14]=1;w[15]=s.automatic;w[16]=s.prediction_reserve;for(unsigned i=0;i<4;++i)w[17+i]=s.build[i];w[21]=s.challenge;return w;
}
std::uint32_t GameplayAbi(const SessionSetup& s){
    std::uint32_t h=2166136261u;const auto word=[&](std::uint32_t n){for(unsigned i=0;i<4;++i){h^=(n>>(i*8))&255;h*=16777619u;}};
    word(0x54483131u);word(SessionSetup::GameplayVersion);word(s.player_count);word(s.difficulty);
    for(auto selection:s.selections)word(selection);word(s.challenge);word(s.automatic);word(s.requested_delay);
    word(s.input_delay);word(s.prediction_reserve);for(auto build:s.build)word(build);return h?h:1;
}
}
