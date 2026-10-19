#include "StageHints.hpp"
#include <charconv>
#include <cstdarg>
namespace th10 {
// 0x418d00 / 0x418d40. Lists have owner-bearing sentinels; retained hints
// remain available for the automatic hint file after displayed copies expire.
void StageHint::initialize() noexcept {std::memset(this,0,sizeof(*this));node.initialize(this);color=0xffffffff;remaining=-1;duration=300;scale=1;}
void StageHints::initialize(StageHints** current) noexcept {std::memset(this,0,sizeof(*this));flags=2;for(auto& head:pending)head.initialize(reinterpret_cast<StageHint*>(this));for(auto& head:retained)head.initialize(reinterpret_cast<StageHint*>(this));*current=this;}
// 0x4198c0 / 0x41ab10.
i32 HintText::lookup(const char* name,const HintName* table,i32 count) noexcept {for(i32 i=0;i<count;++i)if(!std::strcmp(name,table[i].name))return table[i].value;return 0;}
const char* HintText::lookup(i32 value,const HintName* table,i32 count) noexcept {for(i32 i=0;i<count;++i)if(table[i].value==value)return table[i].name;return nullptr;}
void HintText::append(ListNode<StageHint>& node,ListNode<StageHint>& head) noexcept {auto* last=&head;while(last->next)last=last->next;node.insert_after(*last);}
static bool whitespace(char ch) noexcept {return ch==' '||ch=='\t'||ch=='\n'||ch=='\r';}
// 0x41a0a0. Preserve the bytes after the first terminator as well as the text.
char* HintText::trim(char* text) noexcept {while(whitespace(*text))std::memmove(text,text+1,std::strlen(text));auto length=std::strlen(text);while(length&&whitespace(text[length-1]))text[--length]=0;return text;}
// 0x41a050. A line without a colon leaves the previous value untouched.
void HintText::split(const char* text,char* key,char* value) noexcept {std::memcpy(key,text,std::strlen(text)+1);if(auto* colon=std::strchr(key,':')){std::memcpy(value,colon+1,std::strlen(colon+1)+1);*colon=0;trim(key);trim(value);}else trim(key);}
// 0x419f40. The original searches LF before CR and only strips comments and
// whitespace from terminated lines. A final unterminated line is copied raw.
const char* HintText::read_line(const char* input,char* output,u32& remaining,u32 capacity) noexcept {
    if(!capacity)return input;std::memset(output,0,capacity);
    while(!*output){const char* end=std::strchr(input,'\n');if(!end)end=std::strchr(input,'\r');if(!end){const auto bytes=remaining<capacity?remaining:capacity-1;std::memcpy(output,input,bytes);remaining=0;return input;}
        auto bytes=static_cast<u32>(end-input);if(bytes>=capacity)bytes=capacity-1;std::memcpy(output,input,bytes);remaining-=static_cast<u32>(end-input);while(*end=='\n'||*end=='\r'){++end;--remaining;}
        if(auto* comment=std::strchr(output,'#'))std::memset(comment,0,capacity-static_cast<u32>(comment-output));trim(output);input=end;
    }return input;
}
// 0x418e30 / 0x418ee0 / 0x419040 / 0x419090.
i32 StageHints::start(StageHintsEnvironment& env){update_entry=(*env.chain)->add(env.update_callback,this,25,false,false,*env.callbacks);draw_entry=(*env.chain)->add(env.draw_callback,this,44,true,false,*env.callbacks);frames=0;if(*env.enabled){load(env.default_file,false,env);load(env.extra_file,true,env);}return 0;}
void StageHints::clear(StageHintsEnvironment& env){for(u32 i=0;i<8;++i){for(auto* node=pending[i].next;node;){auto* next=node->next;env.release_object(node->value);node=next;}for(auto* node=retained[i].next;node;){auto* next=node->next;env.release_object(node->value);node=next;}}}
void StageHints::shutdown(StageHintsEnvironment& env){(*env.chain)->remove_locked(update_entry,*env.callbacks);(*env.chain)->remove_locked(draw_entry,*env.callbacks);for(auto& id:animations)env.registry->delete_and_clear(id);if(*env.enabled==2)save("hint/hint_auto.txt",env);clear(env);*env.current=nullptr;}
StageHints* StageHints::create(StageHintsEnvironment& env){auto* hints=static_cast<StageHints*>(env.allocate(sizeof(StageHints)));if(!hints)return nullptr;hints->initialize(env.current);if(hints->start(env)){hints->shutdown(env);env.release_object(hints);return nullptr;}return hints;}
// 0x41aad0. Only positive retention counts decay.
void StageHints::decrement_remaining(i32 stage) noexcept {for(auto* node=retained[stage].next;node;node=node->next)if(node->value->remaining>0)--node->value->remaining;}
// 0x41a120. Automatically recorded hints go into the retained stage list.
StageHint* StageHints::record(const char* text,const Vec3& position,HintRecordingEnvironment& env){
    auto* hint=env.allocate_hint();if(!hint)return nullptr;hint->initialize();HintText::append(hint->node,retained[env.game->stage]);hint->remaining=30;hint->position=position;std::strncpy(hint->text,text,64);hint->alignment=0;hint->section=env.game->section;hint->delay=static_cast<i32>(env.game->section?env.game->section_frames:env.game->stage_frames);return hint;
}
// 0x424650. Decay previous records before allocation, then mark the new warning.
StageHint* StageHints::record_caution(const char* text,const Vec3& position,HintRecordingEnvironment& env){
    decrement_remaining(env.game->stage);auto* hint=record(text,position,env);if(!hint)return nullptr;hint->delay=wrapping_add(hint->delay,-60);if(hint->delay<=0)hint->delay=1;hint->duration=90;hint->color=0xffff8080;hint->remaining=30;return hint;
}
static void scale_animation(AnmVm& vm,Vec2 end,Vec2 start,const float* rate) noexcept {auto& curve=vm.scale_interpolation;curve.duration=8;curve.mode=InterpolationMode::Decelerate2;curve.start[0]=start.x;curve.start[1]=start.y;curve.end[0]=end.x;curve.end[1]=end.y;if(!(curve.flags&1)){curve.timer.rate=rate;curve.flags|=1;}curve.timer.previous=-1;curve.timer.current=0;curve.timer.fractional=0;}
// 0x419120. Expired hints are removed using their own links after callbacks;
// the next item to visit is cached before allocation or drawing begins.
i32 StageHints::display(ListNode<StageHint>* head,bool release,StageHintsEnvironment& env){
    while(head){auto* next=head->next;auto& hint=*head->value;if(!hint.section||hint.section==env.game->section)hint.delay=wrapping_add(hint.delay,-1);if(hint.delay<=0){
        env.registry->delete_and_clear(animations[cursor]);hint.position.y=(number(hint.scale)*number(8)+number(hint.position.y)).to_float();
        const bool small=number(hint.scale)<number(1)||number(hint.scale)==number(1),medium=number(hint.scale)<number(2)||number(hint.scale)==number(2);animations[cursor]=env.create_animation(**env.text_animations,cursor+(small?2:12),hint.position);auto& vm=*env.registry->find_and_clear(animations[cursor]);
        if(small){vm.text_settings[0]=vm.text_settings[1]=15;scale_animation(vm,{hint.scale,hint.scale},{Scalar::mul(hint.scale,1.5f),0},env.rate);}
        else{vm.text_settings[0]=vm.text_settings[1]=medium?static_cast<u8>((number(hint.scale)*number(15)).truncate_int()):30;const float half=Scalar::mul(hint.scale,.5f);scale_animation(vm,{half,half},{Scalar::mul(hint.scale,.75f),0},env.rate);}
        if(hint.alignment>=0&&hint.alignment<=2){positions[cursor]=vm.position;if(hint.alignment==1)positions[cursor].x=(number(hint.width)*number(.5f)+number(positions[cursor].x)).to_float();else if(hint.alignment==2)positions[cursor].x=(number(positions[cursor].x)-number(hint.width)*number(.5f)).to_float();widths[cursor]=hint.width;env.draw_text(vm,hint.alignment,0xffffff,hint.text);if(hint.alignment==1)vm.flags=(vm.flags&~0x80000u)|0x40000;else if(hint.alignment==2)vm.flags=(vm.flags&~0x40000u)|0x80000;}
        vm.color=hint.color&0xffffff;vm.integer_variables[0]=hint.duration;vm.integer_variables[1]=hint.color>>24;cursor=wrapping_add(cursor,1)%10;
        // In this routine the original writes the successor before the predecessor.
        if(hint.node.next)hint.node.next->previous=hint.node.previous;if(hint.node.previous)hint.node.previous->next=hint.node.next;hint.node.next=hint.node.previous=nullptr;if(release)env.release_object(&hint);
    }head=next;}return 0;
}
static Extended absolute(Extended value) noexcept {return value<number(0)?-value:value;}
// 0x419650. The horizontal distance remains extended; vertical distance and
// half width are stored to float before the proximity/alpha calculation.
i32 StageHints::update(StageHintsEnvironment& env){
    if(previous_section!=env.game->section)section_frames=0;display(pending[env.game->stage].next,true,env);
    const auto player_x=number((*env.player)->position.x)+number(224),player_y=number((*env.player)->position.y)+number(16);
    for(u32 i=0;i<10;++i){auto* vm=env.registry->find_and_clear(animations[i]);if(!vm||vm->alpha_interpolation.duration)continue;
        const auto x=absolute(player_x-number(positions[i].x));const float y=absolute(player_y-number(positions[i].y)).to_float(),half=Scalar::mul(widths[i],.5f);
        const auto height=number(vm->sprite_size.y)*number(vm->scale.y)*number(.5f);const i32 alpha=vm->integer_variables[1];u8 result;
        if(x<number(half)&&number(y)<height)result=static_cast<u8>(alpha)>>2;
        else{const float outer=Scalar::add(half,32);const auto faded=Extended::from_int(wrapping_add(alpha,wrapping_add(alpha,alpha))/4);
            if(x<number(outer)&&number(y)<height)result=static_cast<u8>((Extended::from_int(alpha)-(number(outer)-x)*faded*number(.03125f)).truncate_int());
            else if(x<number(half)&&number(y)<height+number(32))result=static_cast<u8>((Extended::from_int(alpha)-(height+number(32)-number(y))*faded*number(.03125f)).truncate_int());
            else result=static_cast<u8>(alpha);
        }vm->color=(vm->color&0xffffff)|(static_cast<u32>(result)<<24);
    }section_frames=wrapping_add(section_frames,1);frames=wrapping_add(frames,1);return 1;
}
static i32 integer(const char* text) noexcept {while(whitespace(*text))++text;const bool negative=*text=='-';if(*text=='+'||negative)++text;u32 value=0;while(*text>='0'&&*text<='9')value=value*10+static_cast<u32>(*text++-'0');if(negative)value=0u-value;i32 result;std::memcpy(&result,&value,4);return result;}
static double decimal(const char* text) noexcept {while(whitespace(*text))++text;if(*text=='+')++text;double value=0;std::from_chars(text,text+std::strlen(text),value);return value;}
// The hint writer uses only strings, signed decimal integers and one decimal
// place. Keep those formats independent of libc's FILE/locale machinery.
static void hint_format(char* output,u32 capacity,const char* format,...) noexcept {
    u32 at=0;const auto put=[&](char ch){if(at+1<capacity)output[at++]=ch;};
    const auto unsigned_digits=[&](u64 value,i32 minimum){char digits[32];i32 count=0;do{digits[count++]=static_cast<char>('0'+value%10);value/=10;}while(value);while(count<minimum)digits[count++]='0';while(count)put(digits[--count]);};
    va_list arguments;va_start(arguments,format);
    while(*format){if(*format!='%'){put(*format++);continue;}++format;i32 precision=0;if(*format=='.'){++format;while(*format>='0'&&*format<='9')precision=precision*10+*format++-'0';}
        if(*format=='s'){const auto* text=va_arg(arguments,const char*);if(!text)text="(null)";while(*text)put(*text++);}
        else if(*format=='d'){const i32 value=va_arg(arguments,i32);const u32 magnitude=value<0?0u-static_cast<u32>(value):static_cast<u32>(value);if(value<0)put('-');unsigned_digits(magnitude,precision);}
        else if(*format=='f'){const double value=va_arg(arguments,double);u64 bits;std::memcpy(&bits,&value,8);const bool negative=bits>>63;if(negative)put('-');const double magnitude=negative?-value:value;
            // MSVCRT's old printf rounds the discarded decimal digit up.
            const u64 tenths=static_cast<u64>(magnitude*10+.5);unsigned_digits(tenths/10,1);put('.');put(static_cast<char>('0'+tenths%10));
        }else if(*format)put(*format);if(*format)++format;
    }va_end(arguments);if(capacity)output[at]=0;
}
// 0x419960. The two files share stage queues, with only the first retaining
// copies for later saving. Stage blocks accept at most 255 hints each.
i32 StageHints::load(const char* name,bool transient,StageHintsEnvironment& env){
    u32 remaining=0;auto* bytes=env.read_file(name,remaining);if(!bytes)return -1;
    auto* line=static_cast<char*>(env.allocate_bytes(4096));auto* key=static_cast<char*>(env.allocate_bytes(4096));auto* value=static_cast<char*>(env.allocate_bytes(4096));const char* input=reinterpret_cast<char*>(bytes);i32 stage=-1,count=0;
    while(static_cast<i32>(remaining)>0){input=HintText::read_line(input,line,remaining,4096);HintText::split(line,key,value);HintText::split(line,key,value);
        if(!std::strcmp(key,"Version")){if(std::strcmp(value,"0.0")){clear(env);break;}}
        else if(!std::strcmp(key,"Stage")){stage=integer(value);count=0;if(stage<1||stage>7)stage=-1;}
        else if(!std::strcmp(key,"StageEnd"))stage=-1;
        else if(!std::strcmp(key,"Tips")&&stage>0){if(count>=255){stage=-1;continue;}auto& hint=*static_cast<StageHint*>(env.allocate(sizeof(StageHint)));hint.initialize();++count;
            while(static_cast<i32>(remaining)>0){input=HintText::read_line(input,line,remaining,4096);HintText::split(line,key,value);
                if(!std::strcmp(key,"Pos")){if(auto* comma=std::strchr(value,',')){*comma++=0;HintText::trim(value);HintText::trim(comma);hint.position.x=Extended::from_int(integer(value)).to_float();hint.position.y=Extended::from_int(integer(comma)).to_float();}}
                else if(!std::strcmp(key,"Text")){if(auto* quote=std::strchr(value,'"')){++quote;auto* last=std::strrchr(quote,'"');if(!last)break;*last=0;std::strncpy(hint.text,quote,65);}}
                else if(!std::strcmp(key,"Count"))hint.delay=integer(value);
                else if(!std::strcmp(key,"Time"))hint.duration=integer(value);
                else if(!std::strcmp(key,"Base"))hint.section=HintText::lookup(value,env.section_names,61);
                else if(!std::strcmp(key,"Align"))hint.alignment=HintText::lookup(value,env.alignment_names,3);
                else if(!std::strcmp(key,"Remain"))hint.remaining=integer(value);
                else if(!std::strcmp(key,"Scale")){const auto scale=Extended::from_double(decimal(value));hint.scale=scale.to_float();if(number(4)<scale)hint.scale=4;else if(scale<number(-4))hint.scale=-4;}
                else if(!std::strcmp(key,"Color")){if(auto* comma=std::strchr(value,',')){*comma++=0;HintText::trim(value);hint.color=(hint.color&~0xff0000u)|(static_cast<u32>(static_cast<u8>(integer(value)))<<16);if(auto* last=std::strchr(comma,',')){*last++=0;HintText::trim(comma);hint.color=(hint.color&~0xff00u)|(static_cast<u32>(static_cast<u8>(integer(comma)))<<8);hint.color=(hint.color&~0xffu)|static_cast<u8>(integer(HintText::trim(last)));}}}
                else if(!std::strcmp(key,"Alpha"))hint.color=(hint.color&0xffffff)|(static_cast<u32>(static_cast<u8>(integer(value)))<<24);
                else if(!std::strcmp(key,"End"))break;
            }
            hint.width=(Extended::from_int64(std::strlen(hint.text))*number(hint.scale)*number(7.5f)).to_float();HintText::append(hint.node,pending[stage]);
            if(!transient){auto& copy=*static_cast<StageHint*>(env.allocate(sizeof(StageHint)));copy.initialize();copy=hint;copy.node.initialize(&copy);HintText::append(copy.node,retained[stage]);}
        }
    }env.free_bytes(line);env.free_bytes(key);env.free_bytes(value);env.free_bytes(bytes);return 0;
}
// 0x41a200. The auto writer retains the original "Version =" spelling and
// repeated "Tips" line for unlimited hints, even though the loader uses ':'.
i32 StageHints::save(const char* name,StageHintsEnvironment& env){
    auto* buffer=static_cast<char*>(env.allocate_bytes(4096));env.make_directory("hint");if(env.begin_file(name)){
        const auto emit=[&](bool header){env.write_text(buffer,header);};
        hint_format(buffer,4096,"# ========================================================= \r\n");emit(true);
        hint_format(buffer,4096,"%s",env.save_comments[0]);emit(true);hint_format(buffer,4096,"%s",env.save_separator);emit(true);
        hint_format(buffer,4096,"%s",env.save_comments[1]);emit(true);hint_format(buffer,4096,"%s",env.save_comments[2]);emit(true);hint_format(buffer,4096,"%s",env.save_separator);emit(true);
        const auto date=env.local_time();hint_format(buffer,4096,"#                                Time-stamp: <%.4d/%.2d/%.2d %.2d:%.2d>\r\n",date.year,date.month,date.day,date.hour,date.minute);emit(true);
        hint_format(buffer,4096,"\r\n\r\n");emit(true);hint_format(buffer,4096,"Version = %s\r\n\r\n","0.0");emit(true);
        for(i32 stage=0;stage<8;++stage){i32 count=0;for(auto* node=retained[stage].next;node;){auto* next=node->next;const auto& hint=*node->value;if(hint.remaining){
                if(!count){hint_format(buffer,4096,"# ================================== \r\n");emit(false);hint_format(buffer,4096,"Stage : %d\r\n\r\n",stage);emit(false);}
                hint_format(buffer,4096,"Tips\r\n");emit(false);if(hint.remaining>0)hint_format(buffer,4096,"\tRemain\t: %d\r\n",hint.remaining);emit(false);
                hint_format(buffer,4096,"\tText\t: \"%s\"\r\n",hint.text);emit(false);const i32 y=Scalar::truncate(hint.position.y),x=Scalar::truncate(hint.position.x);hint_format(buffer,4096,"\tPos\t\t: %d, %d\r\n",x,y);emit(false);
                hint_format(buffer,4096,"\tCount\t: %d\r\n",hint.delay);emit(false);const char* section=HintText::lookup(hint.section,env.section_names,61);hint_format(buffer,4096,"\tBase\t: %s\r\n",section?section:"(null)");emit(false);
                const char* align=HintText::lookup(hint.alignment,env.alignment_names,3);hint_format(buffer,4096,"\tAlign\t: %s\r\n",align?align:"(null)");emit(false);hint_format(buffer,4096,"\tTime\t: %d\r\n",hint.duration);emit(false);
                hint_format(buffer,4096,"\tAlpha\t: %d\r\n",hint.color>>24);emit(false);hint_format(buffer,4096,"\tColor\t: %d, %d, %d\r\n",hint.color>>16&255,hint.color>>8&255,hint.color&255);emit(false);hint_format(buffer,4096,"\tScale\t: %.1f\r\n",static_cast<double>(hint.scale));emit(false);hint_format(buffer,4096,"End\r\n\r\n");emit(false);
                if(++count>=255)break;
            }node=next;}if(count){hint_format(buffer,4096,"StageEnd\r\n");emit(false);}}
    }env.end_file();env.free_bytes(buffer);return 0;
}
}
