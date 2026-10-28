#include "ReplayRecorder.hpp"
#include <algorithm>

namespace th13 {
namespace {
void put16(std::vector<u8>& v, size_t p, u16 x) { v[p]=u8(x); v[p+1]=u8(x>>8); }
void put32(std::vector<u8>& v, size_t p, u32 x) { for(int i=0;i<4;i++) v[p+i]=u8(x>>(8*i)); }
void append16(std::vector<u8>& v, u16 x) { v.push_back(u8(x)); v.push_back(u8(x>>8)); }
}
std::vector<u8> ReplayRecorder::top_snapshot(const SessionState& p) {
    std::vector<u8> v(0x74,0);
    put32(v,0x5c,u32(p.character)); put32(v,0x60,0); put32(v,0x64,u32(p.difficulty));
    // +0x70 stores DAT_004be7d4 (selected spell id), not practice stage.
    put32(v,0x70,u32(p.selected_spell_id)); return v;
}
std::vector<u8> ReplayRecorder::stage_snapshot(const SessionState& p,
        const HudPlayerState& player,const HudScoreState& score,u16 seed,
        i32 x128,i32 y128,bool focused) {
    std::vector<u8> v(0xc4,0); put16(v,0,u16(p.stage)); put16(v,2,seed);
    put32(v,0x0c,u32(x128)); put32(v,0x10,u32(y128));
    put32(v,0x14,u32(p.character)); put32(v,0x1c,u32(score.score));
    put32(v,0x20,u32(p.difficulty)); put32(v,0x24,u32(p.continues));
    put32(v,0x2c,u32(p.graze)); put32(v,0x30,u32(p.selected_spell_id));
    put32(v,0x38,u32(p.max_point));
    // 42b990 writes the immutable difficulty base to DAT_004be7e0.
    static constexpr i32 base[]={5000,10000,15000,20000,20000,100000};
    if(p.difficulty>=0 && p.difficulty<6) put32(v,0x3c,u32(base[p.difficulty]*100));
    put32(v,0x40,u32(p.max_point_cap));
    put32(v,0x44,u32(player.power)); put32(v,0x48,u32(player.max_power));
    put32(v,0x4c,u32(player.power_step)); put32(v,0x50,u32(player.extra_lives));
    put32(v,0x54,u32(player.life_pieces)); put32(v,0x58,u32(p.life_piece_threshold_index));
    put32(v,0x5c,u32(player.bombs)); put32(v,0x60,u32(player.bomb_pieces));
    put32(v,0x64,u32(p.ending_points)); put32(v,0x6c,focused?1:0); return v;
}
bool ReplayRecorder::start(const std::vector<u8>& top) {
    if(top.size()!=0x74) return false;
    top_=top; stages_.clear(); return true;
}
bool ReplayRecorder::begin_stage(const std::vector<u8>& header) {
    if(top_.size()!=0x74 || header.size()!=0xc4) return false;
    const u16 number=u16(header[0]) | u16(header[1])<<8;
    // 449070 owns one stage block per stage index; retry replaces that block.
    for(auto it=stages_.begin();it!=stages_.end();++it) if(it->number==number) {
        stages_.erase(it); break;
    }
    if(stages_.size()>=8) return false;
    ReplayStage stage; stage.number=number; stage.header=header; stages_.push_back(std::move(stage)); return true;
}
bool ReplayRecorder::record(const GameInput& input,u8 slow_rate) {
    if(stages_.empty()) return false;
    auto& stage=stages_.back();
    if(stage.frame_count%30==0) stage.events.push_back(slow_rate);
    append16(stage.input,u16(input.held)); append16(stage.input,u16(input.pressed));
    append16(stage.input,u16(input.released)); ++stage.frame_count; return true;
}
bool ReplayRecorder::encode(std::vector<u8>& out,const std::string& name,i32 score) const {
    if(!ready()) return false;
    auto top=top_; std::fill(top.begin(),top.begin()+12,u8(' '));
    const size_t n=std::min<size_t>(name.size(),8); std::copy_n(name.begin(),n,top.begin());
    top[8]=0; put32(top,0x14,u32(score)); top[0x58]=u8(stages_.size());
    std::vector<u8> plain=top;
    std::vector<const ReplayStage*> ordered;
    for(const auto& stage:stages_) ordered.push_back(&stage);
    std::sort(ordered.begin(),ordered.end(),[](const auto* a,const auto* b){ return a->number<b->number; });
    for(size_t i=0;i<ordered.size();++i) {
        const auto& s=*ordered[i]; auto input=s.input; auto events=s.events;
        // 4484d0 appends EOF to the current (last) stage on save.
        if(i+1==ordered.size()) {
            if(s.frame_count%30==0) events.push_back(0);
            for(int j=0;j<6;j++) input.push_back(0xff);
        }
        auto header=s.header; const u32 count=u32(input.size()/6);
        put32(header,4,count); put32(header,8,u32(input.size()+events.size()));
        plain.insert(plain.end(),header.begin(),header.end());
        plain.insert(plain.end(),input.begin(),input.end());
        plain.insert(plain.end(),events.begin(),events.end());
    }
    // Valid LZ77 stream using only literal tokens and the zero-address terminator.
    std::vector<u8> packed; unsigned bits=0; u8 byte=0;
    auto bit=[&](u32 b){byte=u8((byte<<1)|(b&1)); if(++bits==8){packed.push_back(byte);byte=0;bits=0;}};
    for(u8 value:plain){bit(1);for(int i=7;i>=0;--i) bit(value>>i);}
    for(int i=0;i<14;i++) bit(0);
    if(bits) packed.push_back(u8(byte<<(8-bits)));
    ReplayFile::xor_encrypt(packed.data(),packed.size(),0x7d,0x3a,0x100);
    ReplayFile::xor_encrypt(packed.data(),packed.size(),0x5c,0xe1,0x400);
    out.assign(0x24,0); out[0]='t';out[1]='1';out[2]='3';out[3]='r';put16(out,4,2);
    put32(out,0x0c,u32(0x24+packed.size())); put32(out,0x10,0x100);
    put32(out,0x1c,u32(packed.size())); put32(out,0x20,u32(plain.size()));
    out.insert(out.end(),packed.begin(),packed.end());
    // 4484d0 appends a USER summary for the native replay browser.
    const std::string summary="Name " + name.substr(0,8) + "\r\n";
    const size_t user=out.size(); out.resize(user+((12+summary.size()+1+3)&~size_t(3)),0);
    out[user]='U';out[user+1]='S';out[user+2]='E';out[user+3]='R';
    put32(out,user+4,u32(out.size()-user));
    std::copy(summary.begin(),summary.end(),out.begin()+user+12);
    return true;
}
}
