// Music Room state machine and screen layout recovered from TH08 1.00d.
// Reference naming/algorithms: GensokyoClub/th08 (MIT, see licenses).
#include "MusicRoom.hpp"
#include "MusicText.hpp"
#include "Localization.hpp"
#include <cstdio>
#include <string>
namespace th08 {
namespace {
// Display-side lookups mirror the th07 integration: the catalog keeps the
// original CP932 bytes; translations are resolved only when a line is drawn.
// Track numbers are 1-based in themes.etl/musiccmt.etl.
const char* title_for(const TrackDescriptor& track,i32 number){
    return Localization::Active()?Localization::MusicTitle(u32(number),track.title):track.title;
}
// thcrap resolves the "@" comment line through the "Music Room Numbered
// Title" format ("No. <r$%2d$99>  %s") with the translated theme title.
const char* comment_for(const TrackDescriptor& track,i32 number,i32 line,const char* fallback,std::string& formatted){
    const char* comment=Localization::MusicComment(u32(number),u16(line),fallback);
    if(std::strcmp(comment,"@"))return comment;
    char prefix[32];std::snprintf(prefix,sizeof(prefix),"No. %2u  ",unsigned(number));
    formatted=prefix;formatted+=title_for(track,number);
    return formatted.c_str();
}
// Locked-track comment rows map to the seven stringdefs spoiler records
// (music_locked_warnings row order; verified against the pack contents).
const char* const spoiler_ids[7]={
    "th08 Music Room spoiler 1","th08 Music Room spoiler 2","th08 Music Room spoiler 3",
    "th08 Music Room spoiler 4","th08 Music Room spoiler 5","th08 Music Room spoiler 6",
    "th08 Music Room spoiler 7",
};
}
bool MusicCatalog::load(const u8* data,u32 size){
    std::memset(tracks,0,sizeof(tracks));count=0;if(!data)return false;u32 p=0;
    // Several shipped comments contain 66/68 bytes. The original writes them
    // across the nominal row boundary, then clears the next row before reading
    // it. Keep that byte result inside the bounds of the complete catalog.
    auto* storage=reinterpret_cast<u8*>(&tracks);
    const auto line=[&](u32 offset){while(p<size&&data[p]!='\r'&&data[p]!='\n'){if(offset>=sizeof(tracks))return false;storage[offset++]=data[p++];}while(p<size&&(data[p]=='\r'||data[p]=='\n'))++p;return true;};
    while(p<size){if(data[p++]!='@')continue;if(count>=32)return false;const u32 base=count*sizeof(TrackDescriptor);auto& track=tracks[count++];
        if(!line(base+offsetof(TrackDescriptor,path))||!line(base+offsetof(TrackDescriptor,title)))return false;
        for(u32 i=0;i<7&&p<size&&data[p]!='@';++i){std::memset(track.descriptions[i],0,66);if(!line(base+offsetof(TrackDescriptor,descriptions)+i*66))return false;}
    }
    return true;
}
MusicRoom::~MusicRoom(){detach();}
void MusicRoom::start(AnmVm& vm,AnmLoaded& file,i32 script){vm.scriptIndex=i16(script);executor.start(file,vm,file.scripts[script]);}
bool MusicRoom::initialize(AnmLoaded& music,const u8* comments,u32 size,const i8* unlocked){
    state.music=&music;state.selected=0;start(state.main,music,0);state.frames=0;
    if(!catalog.load(comments,size)||!catalog.count||catalog.count>24)return false;
    state.tracks=catalog.tracks;state.count=catalog.count;
    for(i32 i=0;i<state.count;++i)state.unlocked[i]=unlocked?unlocked[i]:0;
    for(i32 i=0;i<state.count;++i){auto& vm=state.names[i];start(vm,music,1+i);
        const char* title=state.unlocked[i]?title_for(state.tracks[i],i+1)
            :Localization::StringById("th08_????????",reinterpret_cast<const char*>(music_locked_title));
        text.draw(vm,TextAlignment::Left,state.unlocked[i]?0xc0e0ff:0x80a0c0,state.unlocked[i]?0x302080:0x100040,title);
        vm.pos={93,(Extended::from_int((i+1)*18)+number(104)-number(20)).to_float(),0};vm.anchor=3;
    }
    return true;
}
bool MusicRoom::pressed(u16 mask) const noexcept {return (context.keys&mask)&&(context.keys&mask)!=(context.previous&mask);}
bool MusicRoom::scrolling(u16 mask) const noexcept {return pressed(mask)||((context.keys&mask)&&context.scrolling);}
void MusicRoom::selection_interrupts(){for(i32 i=0;i<31;++i)state.names[i].pendingInterrupt=state.cursor==i?1:2;for(u32 i=0;i<7;++i)state.descriptions[i].pendingInterrupt=2;state.frames=0;}
i32 MusicRoom::enable_input(){
    if(!state.frames){for(i32 i=0;i<31;++i)state.names[i].pendingInterrupt=state.cursor==i?1:2;for(u32 i=0;i<7;++i)state.descriptions[i].pendingInterrupt=1;}
    if(state.frames>=8)state.input_state=1;return 0;
}
i32 MusicRoom::process_input(){
    if(!state.tracks||state.count<=0)return 1;
    if(scrolling(16)){--state.cursor;if(state.cursor<0){state.cursor=state.count-1;state.listing_offset=state.count>10?state.count-10:0;}else if(state.listing_offset>state.cursor)state.listing_offset=state.cursor;selection_interrupts();}
    if(scrolling(32)){++state.cursor;if(state.cursor>=state.count)state.cursor=state.listing_offset=0;else if(state.listing_offset<=state.cursor-10)state.listing_offset=state.cursor-9;selection_interrupts();}
    if(state.frames>=10&&state.frames<=22&&!(state.frames&1)&&context.text){const i32 line=(state.frames-10)/2;auto& vm=state.descriptions[line];start(vm,*context.text,10+line);vm.pendingInterrupt=1;
        const bool shown=state.selected==state.cursor||state.unlocked[state.cursor];
        char message[66]{};const void* bytes=shown?static_cast<void*>(state.tracks[state.cursor].descriptions[line]):static_cast<const void*>(music_locked_warnings[line]);std::memcpy(message,bytes,64);
        const char* display=message;std::string formatted;
        if(Localization::Active()){
            display=shown?comment_for(state.tracks[state.cursor],state.cursor+1,line,message,formatted)
                         :Localization::StringById(spoiler_ids[line],message);
        }
        vm.flag1=display[0]!=0;if(display[0])text.draw(vm,TextAlignment::Left,0xffe0c0,0x300000,display);
    }
    if(pressed(4097)){state.selected=state.cursor;if(context.preload)actions.start_bgm();actions.play_audio(state.tracks[state.selected].path);state.frames=0;
        if(context.text){auto& vm=state.descriptions[7];start(vm,*context.text,17);vm.pendingInterrupt=1;char message[66]{};std::memcpy(message,state.tracks[state.cursor].descriptions[0],64);
            if(Localization::Active()){std::string formatted;text.draw(vm,TextAlignment::Left,0xffe0c0,0x300000,comment_for(state.tracks[state.cursor],state.cursor+1,0,message,formatted));}
            else text.draw(vm,TextAlignment::Left,0xffe0c0,0x300000,message);}
    }
    if(pressed(10)){context.supervisor_state=1;actions.capture_loading({500,440,0});return 1;}
    if(pressed(256))actions.fade_music(8);
    if(pressed(16384)){if(context.preload)actions.start_bgm();actions.play_audio(state.tracks[state.selected].path);}
    state.frames=wrapping_add(state.frames,1);return 0;
}
JobResult MusicRoom::update(){
    const i32 before=state.input_state;
    if(state.input_state==0)enable_input();else if(state.input_state==1&&process_input())return JobResult::Remove;
    if(before!=state.input_state)state.frames=0;else state.frames=wrapping_add(state.frames,1);
    executor.execute(state.main);for(auto& vm:state.names)executor.execute(vm);for(auto& vm:state.descriptions)executor.execute(vm);return JobResult::Continue;
}
JobResult MusicRoom::draw(){
    renderer.current_texture=0;actions.background();renderer.draw_no_rotation(state.main);
    for(i32 i=state.listing_offset;i<state.listing_offset+10&&i<state.count;++i){if(i<0||i>=31)continue;auto& vm=state.names[i];ascii.state.color=u32(vm.color1.d3dColor);
        vm.pos={93,(Extended::from_int((i+1-state.listing_offset)*18)+number(104)-number(20)).to_float(),0};renderer.draw_no_rotation(vm);
        Vec3 position=vm.pos;position.x=Scalar::sub(position.x,60);if(state.cursor==i)ascii.add_string(position,"\x7f",context.software_texturing);
        position.x=Scalar::add(position.x,15);ascii.add_format(position,context.software_texturing,"%2d.",i+1);
    }
    for(u32 i=0;i<7;++i)renderer.draw_no_rotation(state.descriptions[i]);ascii.state.color=0xffffffff;ascii.add_format({320,32,0},context.software_texturing,"Now Playing");
    auto& vm=state.descriptions[7];const auto position=vm.pos;const auto color=vm.color1;vm.pos={320,52,0};vm.color1.d3dColor=-1;renderer.draw_no_rotation(vm);vm.pos=position;vm.color1=color;return JobResult::Continue;
}
bool MusicRoom::attach(Chain& owner){
    detach();std::memset(&state,0,sizeof(state));chain=&owner;
    state.calculation=Chain::create([](void* p){return static_cast<MusicRoom*>(p)->update();});state.calculation->argument=this;state.calculation->added=added;state.calculation->deleted=deleted;
    if(owner.add(state.calculation,4)!=0){owner.cut(state.calculation);state.calculation=nullptr;chain=nullptr;return false;}
    state.drawing=Chain::create([](void* p){return static_cast<MusicRoom*>(p)->draw();});state.drawing->argument=this;owner.add(state.drawing,3,true);return true;
}
i32 MusicRoom::added(void* p){auto& self=*static_cast<MusicRoom*>(p);return self.actions.load(self)?0:-1;}
i32 MusicRoom::deleted(void* p){auto& self=*static_cast<MusicRoom*>(p);self.actions.release();self.state.tracks=nullptr;if(self.chain&&self.state.drawing)self.chain->cut(self.state.drawing);self.state.drawing=self.state.calculation=nullptr;return 0;}
void MusicRoom::detach(){if(chain){if(state.calculation)chain->cut(state.calculation);else if(state.drawing)chain->cut(state.drawing);}state.calculation=state.drawing=nullptr;chain=nullptr;}
}
