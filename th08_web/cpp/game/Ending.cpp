#include "Ending.hpp"
#include "JapaneseFonts.hpp"
#include "Localization.hpp"
#include <cstdlib>
namespace th08 {
bool Ending::in_file()const{return state.cursor&&state.file&&state.cursor>=state.file&&state.cursor<state.file+data.size();}
const char* Ending::string(){if(!in_file()||!std::memchr(state.cursor,0,state.file+data.size()-state.cursor)){failed=true;return "";}return reinterpret_cast<const char*>(state.cursor);}
i32 Ending::parameter(){const char* s=string();if(failed)return 0;const i32 value=i32(std::strtol(s,nullptr,10));while(in_file()&&*state.cursor)++state.cursor;while(in_file()&&!*state.cursor)++state.cursor;if(!in_file())failed=true;return value;}
bool Ending::newline(){while(in_file()&&*state.cursor!='\n'&&*state.cursor!='\r')++state.cursor;while(in_file()&&(*state.cursor=='\n'||*state.cursor=='\r'))++state.cursor;if(!in_file())failed=true;return !failed;}
bool Ending::load(const char* name){auto bytes=actions.read_ending(name);if(bytes.empty())return false;data=std::move(bytes);state.file=state.cursor=data.data();state.line_delay=8;state.wait.set(0);state.elapsed.set(0);return true;}
bool Ending::start_vm(AnmVm& vm,AnmLoaded* file,i32 script){if(!file||script<0||u32(script)>=file->scriptCount)return false;vm.scriptIndex=i16(script);vm.pos=vm.pos2={};vm.fontWidth=vm.fontHeight=15;animations.start(*file,vm,file->scripts[script]);return !animations.invalid;}
bool Ending::setup(GameplaySession& session,i32 character,i32 difficulty,i32 stage,u32 flags){
    if(u32(character)>=12||u32(difficulty)>=5||u32(stage)>=9)return false;
    renderer.current_texture=0;renderer.current_sprite=nullptr;renderer.current_blend=3;renderer.current_shader=255;
    for(i32 buffer=0;buffer<2;buffer++){renderer.clear_target(3,0xffffffff);actions.present();}
    state.animation=library.get(24);if(!state.animation||!library.get(0))return false;
    auto& clears=session.clears;auto& record=clears[character];
    if(flags&16){
        const u16 bit=stage==7?0x4000:0x8000;++session.statistics.counts[difficulty].clears;++session.statistics.counts[6].clears;state.seen=state.seen_staff=0;
        for(i32 shot=0;shot<4;shot++)for(i32 d=0;d<4;d++)if(stage==7?(clears[shot].without_retries[d]&0x4000):stage==6&&(clears[shot].with_retries[d]&0x8000))state.seen_staff=1;
        const auto* prior=session.numbers.retries?record.with_retries:record.without_retries;for(i32 d=0;d<4;d++)if(prior[d]&bit)state.seen=1;
        if(!session.numbers.retries){record.without_retries[difficulty]|=bit;clears[12].without_retries[difficulty]|=bit;}
        record.with_retries[difficulty]|=bit;clears[12].with_retries[difficulty]|=bit;session.statistics.music_unlocked[18]=session.statistics.music_unlocked[19]=1;
    }else{state.seen=record.unknown;record.unknown=0;session.statistics.music_unlocked[18]=0x12;}
    for(i32 i=0;i<15;i++){if(!start_vm(state.vms[i],library.get(0),i+18))return false;state.vms[i].pos={64,float(i)*16+400,0};}
    constexpr i32 teams[]{0,1,2,3,0,0,1,1,2,2,3,3};char filename[]{'e','n','d','0',char('0'+teams[character]),flags&16?(stage==7?'c':'b'):'a','.','e','n','d',0};return load(filename);
}
bool Ending::parse(){
    auto& s=state;const bool pressed=(input&4097)&&(input&4097)!=(previous&4097),skip=s.seen&&(input&256);
    auto finish=[&](){s.elapsed.tick(animations.timing);s.background.y=Scalar::sub(s.background.y,s.scroll);if(s.background.y<=0){s.background.y=0;s.scroll=0;}return !failed;};
    if(s.reset_wait.current>0){s.reset_wait.decrement(1,animations.timing);if(s.minimum_reset)s.minimum_reset=wrapping_sub(s.minimum_reset,1);else if(pressed||skip)s.reset_wait.set(0);if(s.reset_wait.current<=0){for(i32 i=0;i<15;i++)s.vms[i].pendingInterrupt=2;s.lines=0;}else return finish();}
    if(s.wait.current>0){s.wait.decrement(1,animations.timing);if(s.minimum_wait)s.minimum_wait=wrapping_sub(s.minimum_wait,1);else if(pressed||skip)s.wait.set(0);return finish();}
    char buffer[160]{};u32 size=0;
    // Original data is finite. Bound malformed chains of @F commands to avoid
    // an imported script monopolizing the application thread.
    for(u32 commands=0;commands<65536&&in_file()&&!failed;commands++){
        const u8 value=*s.cursor;
        if(value=='@'){
            ++s.cursor;if(!in_file()){failed=true;break;}const u8 command=*s.cursor;
            switch(command){
            case 'b':{const auto* command=s.cursor++;const std::string name=string();s.cursor=command;if(failed||!actions.load_background(name.c_str()))return false;break;}
            case 'a':{++s.cursor;const i32 index=parameter(),script=parameter(),sprite=parameter();if(failed||u32(index)>=16||!start_vm(s.vms[index],s.animation,script))return false;if(s.animation->SetSprite(&s.vms[index],sprite)<0)return false;break;}
            case 'V':{++s.cursor;const i32 numerator=parameter(),denominator=parameter();s.scroll=(number(Extended::from_int(numerator).to_float())/number(Extended::from_int(denominator).to_float())).to_float();break;}
            case 'v':++s.cursor;s.background.y=Extended::from_int(parameter()).to_float();break;
            case 'F':{const auto* command=s.cursor++;const std::string filename=string();s.cursor=command;if(failed||!load(filename.c_str()))return false;size=0;s.seen=s.seen_staff;[[fallthrough]];}
            case 'R':for(auto& vm:s.vms)vm.scriptIndex=0;break;
            case 'm':{const auto* command=s.cursor++;const std::string name=string();s.cursor=command;if(failed)return false;actions.music(name.c_str());break;}
            case 'M':++s.cursor;actions.fade_music(Extended::from_int(parameter()).to_float());break;
            case 's':++s.cursor;s.line_delay=parameter();s.fast_delay=parameter();break;
            case 'c':++s.cursor;s.text_color=u32(parameter());break;
            case 'r':++s.cursor;s.reset_wait.set(parameter());s.minimum_reset=parameter();s.wait.set(0);s.minimum_wait=0;if(!newline())return false;return finish();
            case 'w':++s.cursor;s.wait.set(parameter());s.minimum_wait=parameter();if(!newline())return false;return finish();
            case '0':case '1':case '2':case '3':++s.cursor;s.fade_mode=command-'0'+1;s.fade_timer=0;s.fade_duration=parameter();break;
            case 'z':return false;
            }
            if(!newline())return false;
        }else if(value==0||value=='\n'||value=='\r'){
            if(size){if(u32(s.lines)>=16)return false;if(!text.draw(s.vms[s.lines],TextAlignment::Left,s.text_color,0xffffffff,buffer))return false;s.vms[s.lines].pendingInterrupt=1;}
            while(in_file()&&(*s.cursor==0||*s.cursor=='\n'||*s.cursor=='\r'))++s.cursor;
            s.wait.set(input&4097?s.fast_delay:s.line_delay);s.minimum_wait=s.fast_delay;s.lines=wrapping_add(s.lines,1);return finish();
        }else{
            // Translated endings are UTF-8 and need up to 3 bytes per glyph,
            // so the line budget grows and the copy unit becomes one code
            // point (thcrap's ending_copy binhacks do the same on Windows).
            // The CP932 path keeps the original 2-byte unit and 66-byte budget.
            const u8* line_end=s.cursor;while(line_end<s.file+data.size()&&*line_end&&*line_end!='\n'&&*line_end!='\r')++line_end;
            if(Localization::Active()&&utf8_valid(s.cursor,u32(line_end-s.cursor))){
                const u8* unit=s.cursor;utf8_next(s.cursor,line_end);const u32 n=u32(s.cursor-unit);
                if(size+n>=sizeof(buffer)){failed=true;break;}
                std::memcpy(buffer+size,unit,n);size+=n;
            }else{
                if(size+2>=68||s.file+data.size()-s.cursor<2){failed=true;break;}buffer[size++]=*s.cursor++;buffer[size++]=*s.cursor++;
            }
        }
    }
    failed=true;return false;
}
JobResult Ending::update(u16 keys,u16 previous_keys){input=keys;previous=previous_keys;for(i32 i=0;i<9;i++){if(!parse())return JobResult::Remove;for(i32 vm=0;vm<15;vm++)animations.execute(state.vms[vm]);if(!state.seen||!(input&256))break;}return invalid()?JobResult::Error:JobResult::Continue;}
void Ending::fade(){auto& s=state;switch(s.fade_mode){
    case 1:case 3:if(s.fade_timer>=s.fade_duration){s.fade_mode=0;s.fade_color=0;}else{const i32 alpha=255-signed_bits(u32(s.fade_timer)*255u)/s.fade_duration;s.fade_color=(s.fade_mode==3?0xffffffu:0u)|(u32(alpha)<<24);s.fade_timer=wrapping_add(s.fade_timer,1);}break;
    case 2:case 4:if(s.fade_timer>=s.fade_duration)s.fade_color=s.fade_mode==2?0xff000000:0xffffffff;else{const i32 alpha=signed_bits(u32(s.fade_timer)*255u)/s.fade_duration;s.fade_color=(s.fade_mode==4?0xffffffu:0u)|(u32(alpha)<<24);s.fade_timer=wrapping_add(s.fade_timer,1);}break;
    case 0:s.fade_color=0;break;
    }if(s.fade_color&0xff000000){const u32 colors[4]{s.fade_color,s.fade_color,s.fade_color,s.fade_color};renderer.draw_rectangle(0,0,640,480,colors);}}
bool Ending::draw(){actions.draw_background(Scalar::truncate(state.background.x),Scalar::truncate(state.background.y));for(i32 i=0;i<15;i++)renderer.draw_2d(state.vms[i]);fade();return !invalid();}
bool Ending::attach(Chain& chain,GameplaySession& session,i32 character,i32 difficulty,i32 stage,u32 flags){if(owner)return false;reset();if(!setup(session,character,difficulty,stage,flags))return false;owner=&chain;calculation.set_callback([](void* p){auto& e=*static_cast<Ending*>(p);return e.update(e.input,e.previous);});calculation.argument=this;calculation.deleted=[](void* p){static_cast<Ending*>(p)->release();return 0;};drawing.set_callback([](void* p){return static_cast<Ending*>(p)->draw()?JobResult::Continue:JobResult::Error;});drawing.argument=this;state.calculation=&calculation;state.drawing=&drawing;chain.add(&calculation,5);chain.add(&drawing,4,true);return true;}
void Ending::release(){renderer.flush();library.release(24);actions.release_background();if(owner)owner->cut(&drawing);state.drawing=nullptr;state.file=state.cursor=nullptr;data.clear();owner=nullptr;actions.finished();}
void Ending::detach(){if(owner)owner->cut(&calculation);}
}
