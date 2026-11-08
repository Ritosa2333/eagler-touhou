#include "EclVm.hpp"
#include "GameWorld.hpp"
#include "AnmVm.hpp"
#include "Interpolation.hpp"
#include <cmath>
#include <cstdio>
namespace th19 {
void EclVm::rebind_tree(EclVm* root,GameWorld* world){
 tree_root_=root;world_=world;
 for(auto&child:children)child->rebind_tree(root,world);
}
void EclVm::restore_world(GameWorld* world){rebind_tree(this,world);}
EclVm::EclVm(const EclVm& other){*this=other;}
EclVm& EclVm::operator=(const EclVm& other){
 if(this==&other)return *this;
 host_id=other.host_id;
 has_host=other.has_host;
 programs_=other.programs_;
 world_=other.world_;
 prog_=other.prog_;
 sub_=other.sub_;
 ip_=other.ip_;
 bp_=other.bp_;
 time_=other.time_;
 waiting_=other.waiting_;
 wait_target_=other.wait_target_;
 wait_size_=other.wait_size_;
 stack_=other.stack_;
 frames_=other.frames_;
 globals_=other.globals_;
 locals_=other.locals_;
 ended_=other.ended_;
 error_=other.error_;
 float_motions_=other.float_motions_;
 async_id_=other.async_id_;
 children.resize(other.children.size());
 for(size_t i=0;i<children.size();++i){if(children[i])*children[i]=*other.children[i];else children[i]=std::make_unique<EclVm>(*other.children[i]);}
 rebind_tree(this,world_);return *this;
}
EclVm::EclVm(EclVm&& other) noexcept{*this=std::move(other);}
EclVm& EclVm::operator=(EclVm&& other) noexcept{
 if(this==&other)return *this;
 host_id=std::move(other.host_id);
 has_host=std::move(other.has_host);
 programs_=std::move(other.programs_);
 world_=std::move(other.world_);
 prog_=std::move(other.prog_);
 sub_=std::move(other.sub_);
 ip_=std::move(other.ip_);
 bp_=std::move(other.bp_);
 time_=std::move(other.time_);
 waiting_=std::move(other.waiting_);
 wait_target_=std::move(other.wait_target_);
 wait_size_=std::move(other.wait_size_);
 stack_=std::move(other.stack_);
 frames_=std::move(other.frames_);
 globals_=std::move(other.globals_);
 locals_=std::move(other.locals_);
 ended_=std::move(other.ended_);
 error_=std::move(other.error_);
 float_motions_=std::move(other.float_motions_);
 async_id_=std::move(other.async_id_);
 children=std::move(other.children);rebind_tree(this,world_);return *this;
}
void EclVm::attach_program(const EclProgram& program){programs_.push_back(&program);}
i32 EclVm::read_i32(u32 off){if(off+4>stream_len()){error_=true;return 0;}const u8* b=stream()+off;return i32(u32(b[0])|(u32(b[1])<<8)|(u32(b[2])<<16)|(u32(b[3])<<24));}
float EclVm::read_f32(u32 off){i32 b=read_i32(off);float v;std::memcpy(&v,&b,4);return v;}
bool EclVm::decode_header(EclInstruction& ins) const {
    if(ip_>stream_len() || stream_len()-ip_<16)return false;
    const u8* b=stream()+ip_;
    ins.time=u32(b[0])|(u32(b[1])<<8)|(u32(b[2])<<16)|(u32(b[3])<<24);
    ins.id=u16(b[4])|(u16(b[5])<<8);
    ins.size=u16(b[6])|(u16(b[7])<<8);
    ins.param_mask=u16(b[8])|(u16(b[9])<<8);
    ins.rank_mask=b[10];ins.param_count=b[11];
    ins.zero=u32(b[12])|(u32(b[13])<<8)|(u32(b[14])<<16)|(u32(b[15])<<24);
    ins.offset=ip_;ins.param_bytes=ins.size-16;
    if(ins.size<16 || ins.size>stream_len()-ip_)return false;
    return true;
}
i32 EclVm::param_i32(const EclInstruction& ins,u32 arg){
    const i32 raw=read_i32(param_off(arg));
    return param_stack_ref(ins,arg)? var_int(raw) : raw;
}
float EclVm::param_f32(const EclInstruction& ins,u32 arg){
    const float raw=read_f32(param_off(arg));
    return param_stack_ref(ins,arg)?var_float(i32(raw)):raw;
}
bool EclVm::resolve(const std::string& name,i32& out_prog,i32& out_sub) const {
    for(i32 p=0;p<i32(programs_.size());++p){
        const auto& subs=programs_[p]->subs;
        for(u32 i=0;i<subs.size();++i)if(subs[i].name==name){out_prog=p;out_sub=i32(i);return true;}
    }
    return false;
}
bool EclVm::enter_sub(i32 prog_idx,i32 sub_idx){prog_=prog_idx;sub_=sub_idx;ip_=0;return true;}
bool EclVm::start(const char* name){
    if(programs_.empty()){error_=true;ended_=true;return false;}
    i32 p,s;if(!resolve(name?name:"",p,s)){error_=true;ended_=true;return false;}
    ended_=error_=waiting_=false;time_=-1;frames_.clear();children.clear();
    bp_=0;stack_.clear();float_motions_={};
    return enter_sub(p,s);
}
void EclVm::push_int(i32 v){stack_.push_back(0);stack_.push_back(v);}
void EclVm::push_float(float v){stack_.push_back(1);i32 bits;std::memcpy(&bits,&v,4);stack_.push_back(bits);}
i32 EclVm::pop_int(){if(stack_.size()<2){error_=true;return 0;}i32 v=stack_.back();stack_.pop_back();i32 type=stack_.back();stack_.pop_back();if(type==1){float f;std::memcpy(&f,&v,4);return i32(f);}return v;}
float EclVm::pop_float(){if(stack_.size()<2){error_=true;return 0;}i32 bits=stack_.back();stack_.pop_back();i32 type=stack_.back();stack_.pop_back();if(type==0)return float(bits);float v;std::memcpy(&v,&bits,4);return v;}
i32 EclVm::var_int(i32 addr) const {
    // 0x4fdc22..0x4fdc56: boss slot 0's second host integer.
    // Orin's dead fairies wait for this live revival signal.
    if(world_&&addr==-9942){
        const u32 id=world_->boss_slots[0];
        for(const auto&e:world_->enemies)if(e.active&&id&&e.id==id)return e.ecl_ints[1];
        return 0;
    }
    if(world_&&addr==-9896)return world_->boss_escape_result;
    if(world_&&addr==-9887)return world_->story_boss_phase;
    if(world_&&addr==-9886)return world_->story_final_boss;
    if(world_&&addr>=-9926&&addr<=-9923)return world_->field_ints[addr+9926];
    if(world_&&has_host&&addr>=-9985&&addr<=-9978)for(const auto&e:world_->enemies)if(e.id==host_id){
        return addr<=-9982?e.ecl_ints[addr+9985]:i32(e.ecl_floats[addr+9981]);
    }
    if(world_&&addr==-9931)return i32(world_->last_spawned_id); // native 0x4fdb9e
    if(world_&&addr==-9889){ // native 0x4fda8d: count flag 0x10000000
        i32 count=0;for(const auto& e:world_->enemies)if(e.active&&(e.status_flags&0x10000000u))++count;
        return count;
    }
    if(addr>=0){i32 l=bp_+addr/4;return (l>=0&&l<i32(locals_.size()))?locals_[l]:0;}
    if(addr>=-100){
        const i32 idx=i32(stack_.size())+2*addr+1;
        if(idx<1||idx>=i32(stack_.size()))return 0;
        if(stack_[idx-1]==1){float v;std::memcpy(&v,&stack_[idx],4);return i32(v);}
        return stack_[idx];
    }
    auto it=globals_.find(addr);return it!=globals_.end()?it->second:0;
}
float EclVm::var_float(i32 addr) const {
    if(addr==-9942)return float(var_int(addr)); // same alias in 0x4fc530
    // 0x4fc58e / 0x4fc5c6 / 0x4fc5aa use the same RNG at 0x5c6620 for both fields.
    if(addr==-9999)return AnmVm::rand_unit(true);
    if(addr==-9998)return AnmVm::rand_signed_unit(3.14159274101257324219f,true);
    if(addr==-9987)return AnmVm::rand_signed_unit(1.f,true);
    if(world_&&addr==-9896)return world_->boss_escape_result;
    if(world_&&addr==-9887)return world_->story_boss_phase;
    if(world_&&addr==-9886)return world_->story_final_boss;
    if(world_&&addr>=-9926&&addr<=-9923)return float(world_->field_ints[addr+9926]);
    // Native float variable switch 0x4fc530: field player aliases and
    // opposite-field player are live values, not per-VM copied globals.
    if(world_){
        if(addr==-9960)return float(std::clamp(world_->spirit_rank,0,std::max(0,world_->spirit_rank_cap)))/200.f;
        if(addr==-9991||addr==-9965)return world_->player_x;
        if(addr==-9990||addr==-9964)return world_->player_y;
        if(addr==-9884)return world_->opponent_x;
        if(addr==-9883)return world_->opponent_y;
        if(has_host)for(const auto&e:world_->enemies)if(e.id==host_id){
            if(addr==-9997)return e.x;if(addr==-9996)return e.y;
            // EXE 4fcb70: context+12dc = enemy+ac, absolute-motion angle.
            // ECL404 passes enemy+90 to 40fdf0, which writes offset+1c.
            if(addr==-9971)return std::remainder(e.angle,6.28318530717958647692f);
            // 4fc530 case d928 is the real distance, including Chiyari.
            if(addr==-9944)return std::hypot(world_->player_x-e.x,world_->player_y-e.y);
            if(addr==-9989)return std::atan2(world_->player_y-e.y,world_->player_x-e.x);
            break;
        }
    }
    if(world_&&has_host&&addr>=-9985&&addr<=-9978)for(const auto&e:world_->enemies)if(e.id==host_id){
        return addr<=-9982?float(e.ecl_ints[addr+9985]):e.ecl_floats[addr+9981];
    }
    if(addr<0&&addr>=-100){
        const i32 idx=i32(stack_.size())+2*addr+1;
        if(idx<1||idx>=i32(stack_.size()))return 0;
        if(stack_[idx-1]==0)return float(stack_[idx]);
        float v;std::memcpy(&v,&stack_[idx],4);return v;
    }
    i32 b=var_int(addr);float v;std::memcpy(&v,&b,4);return v;
}
void EclVm::set_var_int(i32 addr,i32 v){
    if(world_&&addr>=-9926&&addr<=-9923){world_->field_ints[addr+9926]=v;return;}
    if(world_&&has_host&&addr>=-9985&&addr<=-9978)for(auto&e:world_->enemies)if(e.id==host_id){
        if(addr<=-9982)e.ecl_ints[addr+9985]=v;else e.ecl_floats[addr+9981]=float(v);return;
    }
    if(addr>=0){i32 l=bp_+addr/4;if(l<0||l>1000000)return;if(i32(locals_.size())<=l)locals_.resize(l+1);locals_[l]=v;return;}
    if(addr>=-100){i32 idx=i32(stack_.size())+2*addr+1;if(idx>=0&&idx<i32(stack_.size()))stack_[idx]=v;return;}
    globals_[addr]=v;
}
void EclVm::set_var_float(i32 addr,float v){
    if(world_&&addr>=-9926&&addr<=-9923){world_->field_ints[addr+9926]=i32(v);return;}
    if(world_&&has_host&&addr>=-9985&&addr<=-9978)for(auto&e:world_->enemies)if(e.id==host_id){
        if(addr<=-9982)e.ecl_ints[addr+9985]=i32(v);else e.ecl_floats[addr+9981]=v;return;
    }
    i32 b;std::memcpy(&b,&v,4);set_var_int(addr,b);}

bool EclVm::do_call(const std::string& name,bool async,const EclInstruction* call_ins){
    i32 p,s;
    if(!resolve(name,p,s)){error_=true;return false;}
    std::vector<i32> args;
    if(call_ins){
        u32 off=ip_+20+u32(read_i32(ip_+16));
        u32 arg=1;
        if(call_ins->id==16){off+=4;++arg;} // asynchronous slot ID precedes D arguments
        const u32 end=ip_+call_ins->size;
        if(off>end || (end-off)%8){error_=true;return false;}
        for(;off<end;off+=8,++arg){
            const u8 from=stream()[off],to=stream()[off+1];
            if((from!='i'&&from!='f') || (to!='i'&&to!='f')){error_=true;return false;}
            i32 bits;
            if(from=='f'){
                float v=read_f32(off+4);
                if(param_stack_ref(*call_ins,arg))v=var_float(i32(v));
                if(to=='i')bits=i32(v);else std::memcpy(&bits,&v,4);
            }else{
                bits=read_i32(off+4);
                if(param_stack_ref(*call_ins,arg))bits=var_int(bits);
                if(to=='f'){float v=float(bits);std::memcpy(&bits,&v,4);}
            }
            args.push_back(bits);
        }
    }
    if(async){
        auto child=std::make_unique<EclVm>();
        for(auto* pr:programs_)child->attach_program(*pr);
        child->attach_world(world_);
        child->has_host=has_host;child->host_id=host_id;
        if(call_ins&&call_ins->id==16){
            const u32 slot=ip_+20+u32(read_i32(ip_+16));
            const i32 raw=read_i32(slot);
            child->async_id_=param_stack_ref(*call_ins,1)?var_int(raw):raw;
        }
        child->locals_=std::move(args);
        child->enter_sub(p,s);
        tree_root_->children.push_back(std::move(child));
        return true;
    }
    Frame fr;fr.prog=prog_;fr.sub=sub_;fr.ip=(call_ins? call_ins->offset+call_ins->size : ip_);fr.bp=bp_;
    fr.time=time_;fr.locals_size=locals_.size();
    frames_.push_back(fr);
    bp_=i32(locals_.size());locals_.insert(locals_.end(),args.begin(),args.end());
    prog_=p;sub_=s;ip_=0;time_=0;
    return true;
}
bool EclVm::do_return(){
    if(frames_.empty()){ended_=true;return false;}
    Frame fr=frames_.back();frames_.pop_back();
    prog_=fr.prog;sub_=fr.sub;ip_=fr.ip;bp_=fr.bp;
    time_=fr.time;locals_.resize(fr.locals_size);
    return true;
}

bool EclVm::stop_async_id(i32 id){
    // Native 0x568ac3 marks the matching context ended, without deleting it.
    for(auto it=children.rbegin();it!=children.rend();++it){
        auto& child=*it;
        if(child->async_id_==id&&!child->ended_){child->ended_=true;return true;}
    }
    return false;
}
void EclVm::stop_async_children(){
    // Native 0x568a30 ends every asynchronous context owned by this host.
    // Keep objects alive while a child can still be executing this instruction.
    for(auto& child:children)child->ended_=true;
}
void EclVm::advance_float_motions(){
    for(auto&m:float_motions_)if(m.active){
        float t=nativeEase(m.mode,float(++m.elapsed)/m.duration),v=m.from+(m.to-m.from)*t;
        if(m.elapsed>=m.duration){v=m.to;m.active=false;}
        if(m.dest>=0){const int idx=m.base+m.dest/4;if(idx>=0&&idx<int(locals_.size()))std::memcpy(&locals_[idx],&v,4);}
        else set_var_float(m.dest,v);
    }
}
bool EclVm::step(){
    if(!tree_root_)tree_root_=this;
    const size_t pending=children.size();
    bool active=step_self();
    if(!ended_&&!error_)advance_float_motions();
    // 0x5682d0 prepends to the host's context list. 0x56b4e0 saves the
    // successor before dispatch, so contexts created this tick wait a tick.
    for(size_t i=std::min(pending,children.size());i>0;--i){
        auto* child=children[i-1].get();
        child->tree_root_=tree_root_;
        if(child->step())active=true;
    }
    return active;
}
bool EclVm::step_self(){
    if(ended_||error_||sub_<0||prog_<0||programs_.empty())return false;
    ++time_;
    if(waiting_){
        if(time_<wait_target_)return true;
        waiting_=false;ip_+=wait_size_;
        if(ip_>=stream_len()){ended_=true;return false;}
    }
    for(u32 guard=0;guard<100000;++guard){
        if(ended_||error_)return false;
        EclInstruction ins;
        if(!decode_header(ins)){ended_=true;error_=true;return false;}
        if(double(i32(ins.time))>time_)break;
          // ECL 519 retries the same instruction until MSG 12 pulses +0x1a0,
          // or the message VM has ended. Do not consume operands while blocked.
          if(ins.id==519&&world_&&world_->message_active&&!world_->message_signal){--time_;return true;}
        if(world_){int rank=world_->difficulty;
            if(has_host)for(const auto&e:world_->enemies)if(e.id==host_id){if(e.attack_rank>=0)rank=e.attack_rank;break;}
            if(rank>=0&&rank<8&&!(ins.rank_mask&(1u<<rank))){ip_+=ins.size;continue;}
        }
        if(ins.id==520&&world_){bool busy=false;
            for(int slot=0;slot<3;++slot)for(const auto&e:world_->enemies)
                if(e.active&&e.id&&e.id==world_->boss_slots[slot])busy=true;
            if(busy){--time_;return true;}
        }
        // thtk serializes expression argument consumption as count << 3.
        // Remove the referenced inputs after evaluation, retaining new results.
        struct ConsumeReferences {
            std::vector<i32>& stack;size_t before,count;bool& error;
            ~ConsumeReferences(){
                if(!count)return;
                if(count>before || stack.size()<before){error=true;return;}
                stack.erase(stack.begin()+before-count,stack.begin()+before);
            }
        } consume{stack_,stack_.size(),size_t(ins.zero>>3)*2,error_};
        const u16 id=ins.id;
        switch(id){
            case 0: ip_+=ins.size;continue; // native 0x56af39: nop
            case 1: {
                // ret-big ends the complete host script, including nested calls.
                if(world_&&has_host)for(auto& e:world_->enemies)if(e.id==host_id){e.active=false;break;}
                children.clear();frames_.clear();ended_=true;return false;
            }
            case 10: {do_return();if(ended_)return false;continue;}
            case 11: case 15: case 16: {
                i32 name_len=read_i32(ip_+16);
                std::string name;
                if(ins.size>=20 && name_len>0 && u32(name_len)<=ins.size-20){
                    name.assign(reinterpret_cast<const char*>(stream()+ip_+20),name_len);
                    while(!name.empty()&&name.back()=='\0')name.pop_back();
                }
                if(!do_call(name,id!=11,&ins))return false;
                if(id==11)continue;
                ip_+=ins.size;continue;
            }
            case 12: case 13: case 14: {
                const i32 o=param_i32(ins,0),t=param_i32(ins,1);
                const bool jump=id==12 || (id==13?!pop_int():!!pop_int());
                if(jump){ip_=u32(i32(ip_)+o);time_=t;}else ip_+=ins.size;
                continue;
            }
            case 17:
                tree_root_->stop_async_id(param_i32(ins,0));ip_+=ins.size;
                if(ended_)return false;continue;
            case 21:
                tree_root_->stop_async_children();ip_+=ins.size;
                if(ended_)return false;continue;
            case 23: case 24: {
                double dur=id==24?double(param_f32(ins,0)):double(param_i32(ins,0));
                if(dur<=0){ip_+=ins.size;continue;}
                waiting_=true;wait_target_=time_+dur;wait_size_=ins.size;
                return true;
            }
            case 40: {
                const i32 bytes=ins.param_bytes>=4?param_i32(ins,0):0;
                if(bytes<0 || bytes>4000000 || bytes%4){error_=true;return false;}
                if(locals_.size()<size_t(bp_+bytes/4))locals_.resize(bp_+bytes/4);
                ip_+=ins.size;continue;
            }
            case 42: push_int(param_i32(ins,0));ip_+=ins.size;continue;
            case 43: {i32 a=read_i32(param_off(0));if(param_stack_ref(ins,0))set_var_int(a,pop_int());else pop_int();ip_+=ins.size;continue;}
            case 44: { // loadf: push float (literal, or variable whose addr is float-encoded)
                i32 raw=read_i32(param_off(0));float f;std::memcpy(&f,&raw,4);
                if(param_stack_ref(ins,0))push_float(var_float((i32)f));else push_float(f);
                ip_+=ins.size;continue;
            }
            case 45: { // assignf: pop float -> variable (addr is float-encoded)
                i32 raw=read_i32(param_off(0));float f;std::memcpy(&f,&raw,4);
                if(param_stack_ref(ins,0))set_var_float((i32)f,pop_float());else pop_float();
                ip_+=ins.size;continue;
            }
            case 50: {i32 b=pop_int(),a=pop_int();push_int(a+b);ip_+=ins.size;continue;}
            case 51: {float b=pop_float(),a=pop_float();push_float(a+b);ip_+=ins.size;continue;}
            case 52: {i32 b=pop_int(),a=pop_int();push_int(a-b);ip_+=ins.size;continue;}
            case 53: {float b=pop_float(),a=pop_float();push_float(a-b);ip_+=ins.size;continue;}
            case 54: {i32 b=pop_int(),a=pop_int();push_int(a*b);ip_+=ins.size;continue;}
            case 55: {float b=pop_float(),a=pop_float();push_float(a*b);ip_+=ins.size;continue;}
            case 56: {i32 b=pop_int(),a=pop_int();push_int(b? a/b:0);ip_+=ins.size;continue;}
            case 57: {float b=pop_float(),a=pop_float();push_float(b? a/b:0);ip_+=ins.size;continue;}
            case 58: {i32 b=pop_int(),a=pop_int();push_int(b? a%b:0);ip_+=ins.size;continue;}
            case 59: {i32 b=pop_int(),a=pop_int();push_int(a==b);ip_+=ins.size;continue;}
            case 60: {float b=pop_float(),a=pop_float();push_int(a==b);ip_+=ins.size;continue;}
            case 61: {i32 b=pop_int(),a=pop_int();push_int(a!=b);ip_+=ins.size;continue;}
            case 62: {float b=pop_float(),a=pop_float();push_int(a!=b);ip_+=ins.size;continue;}
            case 63: {i32 b=pop_int(),a=pop_int();push_int(a<b);ip_+=ins.size;continue;}
            case 64: {float b=pop_float(),a=pop_float();push_int(a<b);ip_+=ins.size;continue;}
            case 65: {i32 b=pop_int(),a=pop_int();push_int(a<=b);ip_+=ins.size;continue;}
            case 66: {float b=pop_float(),a=pop_float();push_int(a<=b);ip_+=ins.size;continue;}
            case 67: {i32 b=pop_int(),a=pop_int();push_int(a>b);ip_+=ins.size;continue;}
            case 68: {float b=pop_float(),a=pop_float();push_int(a>b);ip_+=ins.size;continue;}
            case 69: {i32 b=pop_int(),a=pop_int();push_int(a>=b);ip_+=ins.size;continue;}
            case 70: {float b=pop_float(),a=pop_float();push_int(a>=b);ip_+=ins.size;continue;}
            case 71: {push_int(!pop_int());ip_+=ins.size;continue;}
            case 73: {i32 b=pop_int(),a=pop_int();push_int(a||b);ip_+=ins.size;continue;}
            case 74: {i32 b=pop_int(),a=pop_int();push_int(a&&b);ip_+=ins.size;continue;}
            case 78: { // postfix decrement: branch/repeat consumes the OLD value
                const i32 a=read_i32(param_off(0)),old=var_int(a);
                set_var_int(a,wrapping_sub(old,1));push_int(old);ip_+=ins.size;continue;
            }
            case 79: {float a=pop_float();push_float(std::sin(a));ip_+=ins.size;continue;}
            case 80: {float a=pop_float();push_float(std::cos(a));ip_+=ins.size;continue;}
            case 81: { // 0x56a3b2: output x/y from angle and radius.
                float a=param_f32(ins,2),r=param_f32(ins,3);constexpr float pi=3.14159274101257324219f;
                if(a>pi){for(int n=0;n<34&&a>pi;++n)a-=2*pi;}else if(a<-pi){for(int n=0;n<34&&a<-pi;++n)a+=2*pi;}
                if(param_stack_ref(ins,0))set_var_float(i32(read_f32(param_off(0))),std::cos(a)*r);
                if(param_stack_ref(ins,1))set_var_float(i32(read_f32(param_off(1))),std::sin(a)*r);
                ip_+=ins.size;continue;
            }
            case 85: case 86: { // EXE ECL 85/86: length squared / length.
                const float x=param_f32(ins,1),y=param_f32(ins,2);
                if(param_stack_ref(ins,0))set_var_float(i32(read_f32(param_off(0))),id==85?x*x+y*y:std::hypot(x,y));
                ip_+=ins.size;continue;
            }
            case 87: { // EXE ECL 87: angle from (x1,y1) to (x2,y2).
                const float x1=param_f32(ins,1),y1=param_f32(ins,2),x2=param_f32(ins,3),y2=param_f32(ins,4);
                if(param_stack_ref(ins,0))set_var_float(i32(read_f32(param_off(0))),std::atan2(y2-y1,x2-x1));
                ip_+=ins.size;continue;
            }
            case 88: {push_float(std::sqrt(pop_float()));ip_+=ins.size;continue;}
            case 91: { // 0x56a7de: eight float interpolation slots, saved local-frame base.
                const int slot=param_i32(ins,0),dest=i32(read_f32(param_off(1))),duration=param_i32(ins,2),mode=param_i32(ins,3);
                if(slot<0||slot>=8||!param_stack_ref(ins,1)||(dest<0&&dest>=-100)||duration<0||
                   !((mode>=0&&mode<=6)||mode==9||mode==12||mode==28)){if(world_)++world_->unhandled_opcodes[id];ip_+=ins.size;continue;}
                const float from=param_f32(ins,4),to=param_f32(ins,5);
                set_var_float(dest,from);float_motions_[slot]={duration>0,dest,bp_,duration,0,mode,from,to};
                ip_+=ins.size;continue;
            }
            case 82: { // 0x56a535: normalize a float reference in place, at most 34 turns.
                float a=param_f32(ins,0);constexpr float pi=3.14159274101257324219f;
                if(a>pi){for(int n=0;n<34&&a>pi;++n)a-=2*pi;}
                else if(a<-pi){for(int n=0;n<34&&a<-pi;++n)a+=2*pi;}
                if(param_stack_ref(ins,0))set_var_float(i32(read_f32(param_off(0))),a);
                ip_+=ins.size;continue;
            }
            case 95: case 96: { // Native cases 0x5f/0x60: reflection and heading sign.
                auto normalize=[](float a){constexpr float pi=3.14159274101257324219f;
                    if(a>pi){for(int n=0;n<34&&a>pi;++n)a-=2*pi;}
                    else if(a<-pi){for(int n=0;n<34&&a<-pi;++n)a+=2*pi;}return a;};
                const float angle=normalize(param_f32(ins,1));
                // Preserve the EXE's two strict comparisons at 0x56ae36/0x56ae3f.
                const float result=id==95?2*normalize(param_f32(ins,2))-angle:
                    (angle>-1.5707963705f&&angle>1.5707963705f?1.f:-1.f);
                if(param_stack_ref(ins,0))set_var_float(i32(read_f32(param_off(0))),result);
                ip_+=ins.size;continue;
            }
            case 94: { // 0x56ab64: polar vector, stretch local X, rotate back.
                const float angle=param_f32(ins,2),radius=param_f32(ins,3),axis=param_f32(ins,4),scale=param_f32(ins,5);
                float delta=angle-axis;constexpr float pi=3.14159274101257324219f;
                if(delta>pi){for(int n=0;n<34&&delta>pi;++n)delta-=2*pi;}
                else if(delta<-pi){for(int n=0;n<34&&delta<-pi;++n)delta+=2*pi;}
                const float x=std::cos(delta)*radius*scale,y=std::sin(delta)*radius;
                const float c=std::cos(axis),sn=std::sin(axis);
                // Destination operands are float-encoded variable addresses.
                if(param_stack_ref(ins,0))set_var_float(i32(read_f32(param_off(0))),c*x-sn*y);
                if(param_stack_ref(ins,1))set_var_float(i32(read_f32(param_off(1))),sn*x+c*y);
                ip_+=ins.size;continue;
            }
            case 83: {const u32 a=u32(pop_int());push_int(i32(0u-a));ip_+=ins.size;continue;}
            case 84: {const float a=pop_float();push_float(-a);ip_+=ins.size;continue;}
            default:
                if(id>=300){if(!game_object(ins)&&world_)++world_->unhandled_opcodes[id];}
                else if(world_)++world_->unhandled_opcodes[id];
                ip_+=ins.size;
                if(ip_>=stream_len()){ended_=true;return false;}
                continue;
        }
    }
    return true;
}
}
