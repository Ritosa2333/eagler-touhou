#include "vm.hpp"
#include <algorithm>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <emmintrin.h>

namespace th20::source::ecl {
namespace {
std::uint32_t read32(const std::uint8_t* p) { std::uint32_t v; std::memcpy(&v,p,4); return v; }
std::uint16_t read16(const std::uint8_t* p) { std::uint16_t v; std::memcpy(&v,p,2); return v; }
std::uint32_t converted(std::uint32_t value, std::uint32_t stored_type, char requested) {
    if (stored_type == 'f' && requested == 'i') return static_cast<std::uint32_t>(truncate_float(bits_float(value)));
    if (stored_type == 'i' && requested == 'f') return float_bits(static_cast<float>(bits_int(value)));
    return value;
}
bool reference(Instruction ins, std::int32_t index) { return (ins.mask() & (1u << (static_cast<unsigned>(index)&31u))) != 0; }
float add(float a,float b) { return _mm_cvtss_f32(_mm_add_ss(_mm_set_ss(a),_mm_set_ss(b))); }
float sub(float a,float b) { return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b))); }
float mul(float a,float b) { return _mm_cvtss_f32(_mm_mul_ss(_mm_set_ss(a),_mm_set_ss(b))); }
float div(float a,float b) { return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b))); }
std::uint32_t resolved_integer(Runtime& r, Instruction ins, std::int32_t index, std::uint32_t raw, bool consume) {
    if (!reference(ins,index)) return raw;
    const auto value=bits_int(raw);
    if (value >= 0) return r.stack.local(value);
    if (value < -100) return static_cast<std::uint32_t>(r.engine->read_integer(value));
    return consume ? r.stack.pop('i') : r.stack.peek(value*8,'i');
}
float resolved_float(Runtime& r, Instruction ins, std::int32_t index, float value, bool consume) {
    if (!reference(ins,index)) return value;
    if (value >= 0) return bits_float(r.stack.local(truncate_float(value)));
    // Original comparisons are float comparisons, not conversion to int first.
    if (value > -1.0f || value < -100.0f) return r.engine->read_float(truncate_float(value));
    return bits_float(consume ? r.stack.pop('f') : r.stack.peek(truncate_float(mul(value,8.0f)),'f'));
}
}
std::uint32_t float_bits(float v) noexcept { std::uint32_t b; std::memcpy(&b,&v,4);return b; }
float bits_float(std::uint32_t b) noexcept { float v;std::memcpy(&v,&b,4);return v; }
std::int32_t bits_int(std::uint32_t b) noexcept { std::int32_t v;std::memcpy(&v,&b,4);return v; }
std::int32_t truncate_float(float v) noexcept { return _mm_cvtt_ss2si(_mm_set_ss(v)); }
std::uint32_t RandomStream::next(){std::lock_guard<std::recursive_mutex> guard(lock);last=th20::recovered::lcg_next(state);if(!maximum)throw std::domain_error("Original RNG DIV exception");return last%maximum;}
float RandomStream::signed_unit(){
    const auto value=static_cast<float>(static_cast<double>(next()));
    const auto limit=static_cast<float>(static_cast<double>(maximum));
    return sub(div(value,sub(div(limit,2.f),1.f)),1.f);
}
std::uint32_t& Stack::absolute(std::int32_t bytes) {
    if (bytes < 0) throw std::out_of_range("Invalid ECL stack address");
    const auto index=static_cast<std::size_t>(bytes/4);
    if (index >= words.size()) words.resize(index+1);
    return words[index];
}
std::uint32_t& Stack::local(std::int32_t bytes) { return absolute(frame_base+bytes); }
void Stack::push(std::uint32_t value,char type) {
    // Original ensures capacity for an extra tag even for raw (type==0) pushes.
    absolute(pointer+4);
    if (type) { absolute(pointer)=static_cast<unsigned char>(type);pointer+=4; }
    absolute(pointer)=value;pointer+=4;
}
std::uint32_t Stack::pop(char type) {
    pointer-=4; auto value=absolute(pointer);
    if (type) { pointer-=4;value=converted(value,absolute(pointer),type); }
    return value;
}
std::uint32_t Stack::peek(std::int32_t relative,char type) {
    if (type) relative+=4;
    const auto value=absolute(pointer+relative);
    return type ? converted(value,absolute(pointer+relative-4),type) : value;
}
bool Stack::enter_frame(std::int32_t bytes) {
    if (static_cast<std::int64_t>(pointer)+bytes >= 0x100) return false;
    const auto previous=pointer;pointer+=bytes;
    absolute(pointer);push(static_cast<std::uint32_t>(frame_base));frame_base=previous;
    return true;
}
void Stack::leave_frame() { const auto previous=frame_base;frame_base=bits_int(pop());pointer=previous; }
void Program::append(const EclDocument& doc) {
    // 0x53fca0/0x53fdb0 insert before the first strictly greater name;
    // equal names retain input order. Load all resources before using indices.
    for (const auto& s:doc.subroutines) {
        const auto at=std::upper_bound(subroutines.begin(),subroutines.end(),s.name,
            [](const std::string& name,const Subroutine& item){return std::strcmp(name.c_str(),item.name.c_str())<0;});
        subroutines.insert(at,{s.name,std::vector<std::uint8_t>(doc.original.begin()+s.file_offset+s.data_offset,doc.original.begin()+s.end_offset)});
    }
}
void Program::append_borrowed(std::string name,std::uint8_t* data,std::size_t bytes) {
    if(bytes&&!data)throw std::invalid_argument("Null borrowed ECL instruction storage");
    const auto at=std::upper_bound(subroutines.begin(),subroutines.end(),name,
        [](const std::string& key,const Subroutine& item){return std::strcmp(key.c_str(),item.name.c_str())<0;});
    subroutines.insert(at,{std::move(name),{},data,bytes});
}
std::int32_t Program::find(const std::string& name) const {
    std::int32_t low=0,high=static_cast<std::int32_t>(subroutines.size())-1;
    while(low<=high){const auto middle=low+(high-low)/2;const auto compare=std::strcmp(name.c_str(),subroutines[middle].name.c_str());
        if(compare==0)return middle;if(compare<0)high=middle-1;else low=middle+1;}
    return -1;
}
std::int32_t Instruction::time() const { return bits_int(read32(bytes)); }
std::uint16_t Instruction::opcode() const { return read16(bytes+4); }
std::uint16_t Instruction::size() const { return read16(bytes+6); }
std::uint16_t Instruction::mask() const { return read16(bytes+8); }
std::uint8_t Instruction::rank() const { return bytes[10]; }
std::uint8_t Instruction::argument_count() const { return bytes[11]; }
std::uint8_t& Instruction::stack_drop_bytes() const { return bytes[12]; }
std::uint32_t Instruction::argument(std::int32_t i) const {
    if (i<0 || static_cast<std::uint64_t>(i)*4+20>size())throw std::out_of_range("ECL operand outside instruction");
    return read32(bytes+16+i*4);
}
Instruction Runtime::current() {
    if(!active())throw std::out_of_range("Inactive ECL runtime has no instruction");
    auto& data=program->subroutines.at(static_cast<std::size_t>(subroutine));
    if(instruction_offset<0 || static_cast<std::uint64_t>(instruction_offset)+16>data.size())throw std::out_of_range("ECL instruction outside subroutine");
    Instruction ins{data.data()+instruction_offset};
    if(ins.size()<16 || static_cast<std::uint64_t>(instruction_offset)+ins.size()>data.size())throw std::out_of_range("Invalid ECL instruction size");
    return ins;
}
std::int32_t Runtime::integer_argument(Instruction ins,std::int32_t i,bool consume) { return bits_int(resolved_integer(*this,ins,i,ins.argument(i),consume)); }
float Runtime::float_argument(Instruction ins,std::int32_t i,bool consume) { return resolved_float(*this,ins,i,bits_float(ins.argument(i)),consume); }
std::uint32_t& Runtime::integer_destination(Instruction ins,std::int32_t i) {
    if(!reference(ins,i))throw std::runtime_error("Original ECL integer destination is null");
    const auto raw=bits_int(ins.argument(i));return raw<0?engine->integer_destination(raw):stack.local(raw);
}
std::uint32_t& Runtime::float_destination(Instruction ins,std::int32_t i) {
    if(!reference(ins,i))throw std::runtime_error("Original ECL float destination is null");
    const auto raw=bits_float(ins.argument(i));return raw<0?engine->float_destination(truncate_float(raw)):stack.local(truncate_float(raw));
}
bool Runtime::call(Instruction ins) {
    return call_into(ins,*this,0);
}
bool Runtime::call_into(Instruction ins,Runtime& target,std::int32_t argument_skip) {
    auto& destination_stack=target.stack;
    const auto old_pointer=destination_stack.pointer;
    auto destination=old_pointer+16;
    if(!old_pointer){destination_stack.push(0);destination=20;}
    auto offset=ins.argument(0)+4u+static_cast<unsigned>(argument_skip)*4;
    for(int i=argument_skip+1;i<ins.argument_count();++i,offset+=8,destination+=4) {
        if(offset+8>static_cast<unsigned>(ins.size()-16))throw std::out_of_range("Invalid ECL call argument");
        const auto from=ins.bytes[16+offset],to=ins.bytes[17+offset];
        const auto raw=read32(ins.bytes+16+((offset+4u)&~3u));
        std::uint32_t result;
        if(from=='f'||from=='g') {
            const auto value=resolved_float(*this,ins,i,bits_float(raw),true);
            result=to=='f'?float_bits(value):static_cast<std::uint32_t>(truncate_float(value));
        } else {
            const auto value=resolved_integer(*this,ins,i,raw,true);
            result=to=='f'?float_bits(static_cast<float>(bits_int(value))):value;
        }
        destination_stack.absolute(destination)=result;
    }
    const auto after_arguments=destination_stack.pointer;
    if(!old_pointer)destination_stack.pointer=4;
    else {const auto saved=destination_stack.pop();destination_stack.pointer=old_pointer;destination_stack.absolute(old_pointer-4)=saved;}
    destination_stack.push(static_cast<std::uint32_t>(after_arguments));
    if(!old_pointer) {destination_stack.push(0xffffffff);destination_stack.push(0xffffffff);destination_stack.push(0xffffffff);}
    else {destination_stack.push(float_bits(time));destination_stack.push(static_cast<std::uint32_t>(instruction_offset));destination_stack.push(static_cast<std::uint32_t>(subroutine));}
    const auto string_bytes=ins.argument(0);
    if(string_bytes>ins.size()-20u)throw std::out_of_range("Invalid ECL callee string");
    const auto* name=reinterpret_cast<const char*>(ins.bytes+20);
    const auto* end=static_cast<const char*>(std::memchr(name,0,string_bytes));
    if(!end)throw std::runtime_error("Unterminated ECL callee");
    target.subroutine=program->find(std::string(name,end));target.instruction_offset=0;target.time=0;
    if(target.subroutine<0){subroutine=-1;instruction_offset=-1;return false;}return true;
}
bool implemented_opcode(std::uint16_t op) noexcept {
    return op==0||op==1||(op>=10&&op<=24)||op==30||op==31||
        (op>=40&&op<=47)||(op>=50&&op<=97);
}
void Runtime::tick_interpolators(){
    for(auto& item:interpolators)if(item.duration!=0){
        auto& bytes=program->subroutines.at(static_cast<std::size_t>(item.subroutine));
        Instruction origin{bytes.data()+item.instruction_offset};
        const auto value=item.sample(clock_rate);
        if(!reference(origin,1))throw std::runtime_error("Original ECL interpolation destination is null");
        const auto offset=bits_float(origin.argument(1));
        auto& target=offset<0?engine->float_destination(truncate_float(offset)):stack.absolute(item.frame_base+truncate_float(offset));
        target=float_bits(value);
    }
}
std::int32_t Runtime::tick(float delta) {
    if(!active())return -1;
    auto ins=current();
    for(;;) {
        if(!(static_cast<float>(ins.time())<=time)) {time=add(time,delta);break;}
        bool drop=false;
        if((ins.rank()&rank_mask)!=0) {
            const auto op=ins.opcode();
            switch(op) {
            case 0:drop=true;break;
            case 1:subroutine=-1;instruction_offset=-1;return -1;
            case 10:
                stack.leave_frame();
                if(stack.pointer) {
                    subroutine=bits_int(stack.pop());instruction_offset=bits_int(stack.pop());time=bits_float(stack.pop());
                    stack.pointer=bits_int(stack.pop());
                    if(instruction_offset>=0){ins=current();drop=true;break;}
                }
                subroutine=-1;instruction_offset=-1;return -1;
            case 11:ins.stack_drop_bytes()=0;if(!call(ins))return -1;ins=current();continue;
            case 12:case 13:case 14: {
                bool jump=op==12;
                if(op!=12){const auto value=stack.pop('i');jump=op==13?value==0:value!=0;}
                if(jump){time=static_cast<float>(bits_int(ins.argument(1)));instruction_offset+=bits_int(ins.argument(0));ins=current();continue;}
                drop=true;break;
            }
            case 15:case 16:{
                if(!scheduler)throw std::runtime_error("ECL async scheduler is not connected");
                std::int32_t id=-1;
                if(op==16){const auto at=(ins.argument(0)+4u)&~3u;const auto raw=read32(ins.bytes+16+at);id=bits_int(resolved_integer(*this,ins,1,raw,true));}
                scheduler->spawn(*this,ins,id,op==16?1:0);break;
            }
            case 17:case 18:case 19:case 20:{
                if(!scheduler)throw std::runtime_error("ECL async scheduler is not connected");
                auto* target=scheduler->find(integer_argument(ins,0));
                if(target){if(op==17)target->instruction_offset=-1;else if(op==18)target->async_flags|=1;else if(op==19)target->async_flags&=~1u;else target->async_field_2c=integer_argument(ins,1);}
                drop=true;break;
            }
            case 21:if(!scheduler)throw std::runtime_error("ECL async scheduler is not connected");scheduler->terminate_async();break;
            case 22:case 30:case 31:drop=true;break;
            case 23:time=sub(time,static_cast<float>(integer_argument(ins,0)));drop=true;break;
            case 24:time=sub(time,float_argument(ins,0));drop=true;break;
            case 40:stack.enter_frame(integer_argument(ins,0));drop=true;break;
            case 41:stack.leave_frame();drop=true;break;
            case 42:stack.push(static_cast<std::uint32_t>(integer_argument(ins,0,true)),'i');break;
            case 43:{const auto value=stack.pop('i');integer_destination(ins,0)=value;break;}
            case 44:stack.push(float_bits(float_argument(ins,0,true)),'f');break;
            case 45:{const auto value=stack.pop('f');float_destination(ins,0)=value;break;}
            case 46:{const auto index=integer_argument(ins,1);const auto value=integer_argument(ins,index*2+2);integer_destination(ins,0)=static_cast<std::uint32_t>(value);drop=true;break;}
            case 47:{const auto index=integer_argument(ins,1);const auto value=float_argument(ins,index*2+2);float_destination(ins,0)=float_bits(value);drop=true;break;}
            case 50:case 52:case 54:case 56:case 58:{
                const auto b=stack.pop('i'),a=stack.pop('i');std::uint32_t value=0;
                if(op==50)value=a+b;else if(op==52)value=a-b;else if(op==54)value=a*b;
                else {
                    const auto sa=bits_int(a),sb=bits_int(b);
                    if(!sb||(sa==std::numeric_limits<std::int32_t>::min()&&sb==-1))throw std::domain_error("Original ECL IDIV exception");
                    value=static_cast<std::uint32_t>(op==56?sa/sb:sa%sb);
                }
                stack.push(value,'i');ins.stack_drop_bytes()=0;break;
            }
            case 51:case 53:case 55:case 57:{
                const auto b=bits_float(stack.pop('f')),a=bits_float(stack.pop('f'));
                const auto value=op==51?add(a,b):op==53?sub(a,b):op==55?mul(a,b):div(a,b);
                stack.push(float_bits(value),'f');ins.stack_drop_bytes()=0;break;
            }
            case 59:case 61:case 63:case 65:case 67:case 69:{
                const auto b=bits_int(stack.pop('i')),a=bits_int(stack.pop('i'));
                const bool v=op==59?a==b:op==61?a!=b:op==63?a<b:op==65?a<=b:op==67?a>b:a>=b;
                stack.push(v,'i');break;
            }
            case 60:case 62:case 64:case 66:case 68:case 70:{
                const auto b=bits_float(stack.pop('f')),a=bits_float(stack.pop('f'));
                const bool v=op==60?a==b:op==62?a!=b:op==64?a<b:op==66?a<=b:op==68?a>b:a>=b;
                stack.push(v,'i');break;
            }
            case 71:stack.push(stack.pop('i')==0,'i');break;
            case 72:stack.push(bits_float(stack.pop('f'))==0.0f,'i');break;
            case 73:case 74:case 75:case 76:case 77:{
                const auto b=stack.pop('i'),a=stack.pop('i');
                const auto v=op==73?std::uint32_t(a!=0||b!=0):op==74?std::uint32_t(a!=0&&b!=0):op==75?a^b:op==76?a|b:a&b;
                stack.push(v,'i');break;
            }
            case 78:{const auto value=static_cast<std::uint32_t>(integer_argument(ins,0));integer_destination(ins,0)=value-1;stack.push(value,'i');break;}
            case 79:stack.push(float_bits(math::sine(bits_float(stack.pop('f')))),'f');break;
            case 80:stack.push(float_bits(math::cosine(bits_float(stack.pop('f')))),'f');break;
            case 81:{const auto angle=math::wrap_angle(float_argument(ins,2));const auto length=float_argument(ins,3);float x,y;math::polar(x,y,angle,length);float_destination(ins,0)=float_bits(x);float_destination(ins,1)=float_bits(y);drop=true;break;}
            case 82:{const auto value=math::wrap_angle(float_argument(ins,0));float_destination(ins,0)=float_bits(value);drop=true;break;}
            case 83:stack.push(0u-stack.pop('i'),'i');break;
            case 84:stack.push(stack.pop('f')^0x80000000u,'f');break;
            case 85:{const auto a=float_argument(ins,1),b=float_argument(ins,2);float_destination(ins,0)=float_bits(add(mul(a,a),mul(b,b)));drop=true;break;}
            case 86:{const auto a=float_argument(ins,1),b=float_argument(ins,2);float_destination(ins,0)=float_bits(math::square_root(add(mul(a,a),mul(b,b))));drop=true;break;}
            case 87:{const auto x0=float_argument(ins,1),y0=float_argument(ins,2),x1=float_argument(ins,3),y1=float_argument(ins,4);float_destination(ins,0)=float_bits(math::arctangent(sub(y1,y0),sub(x1,x0)));drop=true;break;}
            case 88:stack.push(float_bits(math::square_root(bits_float(stack.pop('f')))),'f');break;
            case 89:{const auto a=float_argument(ins,1),b=float_argument(ins,2);float_destination(ins,0)=float_bits(math::angle_difference(b,a));drop=true;break;}
            case 90:{auto x=float_argument(ins,2),y=float_argument(ins,3);const auto angle=math::wrap_angle(float_argument(ins,4));math::rotate(x,y,angle);float_destination(ins,0)=float_bits(x);float_destination(ins,1)=float_bits(y);drop=true;break;}
            case 91:case 92:{
                const auto id=integer_argument(ins,0);
                if(id<0)throw std::out_of_range("Negative ECL interpolation index");
                if(static_cast<std::size_t>(id)>=interpolators.size())interpolators.resize(static_cast<std::size_t>(id)+1);
                auto& item=interpolators[id];
                if(op==91&&integer_argument(ins,2)<1){item.duration=0;if(static_cast<std::size_t>(id)+1==interpolators.size())interpolators.pop_back();}
                else {
                    item.subroutine=subroutine;item.instruction_offset=instruction_offset;item.sample(clock_rate);
                    item.duration=integer_argument(ins,2);item.mode=integer_argument(ins,3);
                    item.start=float_argument(ins,4);item.end=float_argument(ins,5);
                    float_destination(ins,1)=float_bits(item.start);
                    item.tangent_start=op==92?float_argument(ins,6):0.f;item.tangent_end=op==92?float_argument(ins,7):0.f;
                    th20::recovered::timer_set(item.timer,0);item.frame_base=stack.frame_base;
                }
                drop=true;break;
            }
            case 93:{
                if(!random)throw std::runtime_error("ECL shared RNG is not connected");
                const auto low=float_argument(ins,2),high=float_argument(ins,3);
                const auto radius=random->signed_unit();const auto angle=mul(random->signed_unit(),3.1415927410125732421875f);
                float x,y;math::polar(x,y,angle,add(mul(sub(high,low),radius),low));
                float_destination(ins,0)=float_bits(x);float_destination(ins,1)=float_bits(y);drop=true;break;
            }
            case 94:{
                const auto direction=float_argument(ins,2),rotation=float_argument(ins,4),length=float_argument(ins,3);
                const auto angle=math::wrap_angle(sub(direction,rotation));float x,y;math::polar(x,y,angle,length);
                x=mul(float_argument(ins,5),x);math::rotate(x,y,rotation);
                float_destination(ins,0)=float_bits(x);float_destination(ins,1)=float_bits(y);drop=true;break;
            }
            case 95:{const auto a=math::wrap_angle(float_argument(ins,1)),b=math::wrap_angle(float_argument(ins,2));float_destination(ins,0)=float_bits(sub(mul(b,2.f),a));drop=true;break;}
            case 96:{const auto value=math::wrap_angle(float_argument(ins,1));constexpr float pi=3.1415927410125732421875f;float_destination(ins,0)=float_bits(!(value>-pi/2.f)||!(value>pi/2.f)?-1.f:1.f);drop=true;break;}
            case 97:{const auto value=math::wrap_angle(float_argument(ins,1));float_destination(ins,0)=float_bits(!(value<0)?1.f:-1.f);drop=true;break;}
            default:
                {
                    const auto result=engine->execute_entity_opcode(*this,ins);
                    if(result==-1){tick_interpolators();return 0;}
                    if(result==1)continue;
                    drop=true;
                }
            }
            if(drop&&ins.stack_drop_bytes())stack.pointer-=ins.stack_drop_bytes();
        }
        const auto length=ins.size();instruction_offset=bits_int(static_cast<std::uint32_t>(instruction_offset)+length);ins.bytes+=length;
    }
    tick_interpolators();
    return 0;
}
Scheduler::Scheduler(Program& p,Engine& e,RandomStream* random,const float* rate):head_(&main),current(&main){main.program=&p;main.engine=&e;main.scheduler=this;main.random=random;main.clock_rate=rate;}
Scheduler::~Scheduler(){while(head_.next)erase(head_.next);}
void Scheduler::erase(Node* node){node->previous->next=node->next;if(node->next)node->next->previous=node->previous;delete node;}
Runtime& Scheduler::spawn(Runtime& caller,Instruction ins,std::int32_t id,std::int32_t skip){
    auto owned=std::make_unique<Runtime>();auto& r=*owned;
    r.program=main.program;r.engine=main.engine;r.scheduler=this;r.random=main.random;r.clock_rate=main.clock_rate;r.async_id=id;r.rank_mask=caller.rank_mask;
    auto node=std::make_unique<Node>(&r);node->owned=std::move(owned);node->previous=&head_;node->next=head_.next;
    if(head_.next)head_.next->previous=node.get();head_.next=node.release();
    auto* previous=current;current=&r;
    if(caller.call_into(ins,r,skip))current=previous; // original restores only on successful lookup
    return r;
}
Runtime* Scheduler::find(std::int32_t id){for(auto* n=&head_;n;n=n->next)if(n->runtime->async_id==id)return n->runtime;return nullptr;}
void Scheduler::terminate_async(){for(auto* n=head_.next;n;n=n->next){n->runtime->instruction_offset=-1;n->runtime->subroutine=-1;}}
std::int32_t Scheduler::tick(float delta){
    auto* node=&head_;bool first=true;
    while(node){auto* next=node->next;current=node->runtime;const auto result=current->tick(delta);
        if(first){if(result!=0)return -1;first=false;}else if(result!=0)erase(node);node=next;}
    current=&main;return 0;
}
std::vector<Runtime*> Scheduler::task_order()const{std::vector<Runtime*> result;for(auto* n=&head_;n;n=n->next)result.push_back(n->runtime);return result;}
}
