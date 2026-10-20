#include "GameProfile.hpp"
namespace th075 {
namespace {
void put(std::vector<u8>& out,u32 v,u32 n){for(u32 i=0;i<n;i++)out.push_back(u8(v>>(i*8)));}
template<std::size_t N> void put(std::vector<u8>& out,const std::array<u8,N>& bytes){out.insert(out.end(),bytes.begin(),bytes.end());}
template<std::size_t N> void read(ByteReader& in,std::array<u8,N>& bytes){const auto* p=in.take(N);if(p)std::memcpy(bytes.data(),p,N);}
void read_progress(ByteReader& in,DifficultyProgress& p){for(auto& n:p.counters)n=in.dword();for(auto& c:p.cards)c.highestScore=in.dword();for(auto& c:p.cards)c.captures=in.word();for(auto& c:p.cards)c.attempts=in.word();for(auto& c:p.cards)c.perfect=in.byte();p.completedStages=in.byte();read(in,p.reserved);for(auto& r:p.ranking){read(in,r.name);r.month=in.byte();r.day=in.byte();read(in,r.reserved);r.score=in.dword();}}
void write_progress(std::vector<u8>& out,const DifficultyProgress& p){for(const auto n:p.counters)put(out,n,4);for(const auto& c:p.cards)put(out,c.highestScore,4);for(const auto& c:p.cards)put(out,c.captures,2);for(const auto& c:p.cards)put(out,c.attempts,2);for(const auto& c:p.cards)put(out,c.perfect,1);put(out,p.completedStages,1);put(out,p.reserved);for(const auto& r:p.ranking){put(out,r.name);put(out,r.month,1);put(out,r.day,1);put(out,r.reserved);put(out,r.score,4);}}
}
GameProfile::GameProfile(){workingName.fill(100);for(auto& c:characters)for(auto& p:c.difficulty)for(u32 i=0;i<10;i++){p.ranking[i].score=(10-i)*1000;p.ranking[i].name={13,14,100,4,13,19,17,24};}for(auto& r:victories)r.fill(10);}
bool GameProfile::load(const void* source,std::size_t size){
 error.clear();const bool legacy=size<70001;const std::size_t required=legacy?61984:93124;
 if(size<required){error="Truncated score file";return false;}
 ByteReader in(source,size);for(u32 i=0;i<(legacy?10u:15u);i++)for(auto& p:characters[i].difficulty)read_progress(in,p);
 read(in,workingName);for(u32 i=0;i<(legacy?10u:15u);i++)for(u32 j=0;j<(legacy?10u:15u);j++)victories[i][j]=in.dword();for(auto& n:unlockBits)n=in.dword();read(in,noise);return in.valid;
}
std::vector<u8> GameProfile::encode()const{std::vector<u8> out;out.reserve(93124);for(const auto& c:characters)for(const auto& p:c.difficulty)write_progress(out,p);put(out,workingName);for(const auto& r:victories)for(const auto n:r)put(out,n,4);for(const auto n:unlockBits)put(out,n,4);put(out,noise);return out;}
std::vector<u8> GameProfile::save(ReplayRandom& random){for(auto& b:noise)if(random.next()<4096)b=u8(random.next());return encode();}
u8 DifficultyProgress::insert_score(u32 score,u8 month,u8 day){if(ranking[9].score>=score)return 255;u8 rank=9;while(rank>0&&ranking[rank-1].score<score){ranking[rank]=ranking[rank-1];rank--;}auto& r=ranking[rank];r.name.fill(100);r.month=month;r.day=day;r.score=score;return rank;}
void GameProfile::complete_stage(u8 stage){if(i8(selected().completedStages)<i8(stage))selected().completedStages=stage;}
void GameProfile::begin_attempt(){selected().counters[0]++;}
bool GameProfile::unlocked(i16 n)const{if(n<0||n>320)return false;if(n==320)return (selectedCharacter&1)!=0;return (unlockBits[u16(n)/32]&(1u<<(u16(n)%32)))!=0;}
void GameProfile::unlock(i16 n){if(n<0||n>320)return;if(n==320){selectedCharacter|=1;return;}unlockBits[u16(n)/32]|=1u<<(u16(n)%32);}
SelectionUnlocks GameProfile::selection_unlocks()const{SelectionUnlocks flags;for(u32 i=0;i<flags.storyFlags.size();i++)flags.storyFlags[i]=unlocked(i16(i));return flags;}
SelectionProgress GameProfile::selection_progress()const{SelectionProgress progress;progress.character=selectedCharacter;progress.difficulty=selectedDifficulty;for(u32 ch=0;ch<11;ch++)for(u32 d=0;d<4;d++)progress.storyStages[ch][d]=characters[ch].difficulty[d].completedStages;return progress;}
void GameProfile::load_spell_records(RoundRules& rules)const{const auto& cards=characters[selectedCharacter].difficulty[0].cards;for(u32 i=0;i<rules.storyCards.size();i++){const auto& c=cards[i];auto& r=rules.storyCards[i];r.maximumScore=i32(c.highestScore);std::memcpy(&r.captures,&c.captures,2);std::memcpy(&r.attempts,&c.attempts,2);r.perfect=c.perfect!=0;}}
void GameProfile::store_spell_records(const RoundRules& rules){auto& cards=characters[selectedCharacter].difficulty[0].cards;for(u32 i=0;i<rules.storyCards.size();i++){auto& c=cards[i];const auto& r=rules.storyCards[i];c.highestScore=u32(r.maximumScore);c.captures=u16(r.captures);c.attempts=u16(r.attempts);if(r.perfect)c.perfect=1;}}
}
