#include "SpectatorStageScene.hpp"
#include "SpectatorStageState.hpp"
#include "../platform/World.hpp"
#include "../platform/Common.hpp"
#include "../platform/Hud.hpp"
#include "../game/StageHints.hpp"
#include "../game/CallbackNames.hpp"
#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>

namespace th10::multiplayer {
namespace {
// Read by multiplayer_spectator_scene_restore_phase() after a failed restore.
// 0=not attempted, 1=schema, 2=unpack/validation, 3=current stage,
// 4=previous stage, 5=environment, 6=registry, 7=handles,
// 8=common, 9=GUI, 10=spell, 11=hint, 12=players, 13=trailing data,
// 14=success.
std::uint32_t gSpectatorSceneRestorePhase=0;
std::uint32_t gSpectatorSceneRestoreDiag[8]={};
constexpr std::uint32_t kDiagUnknown=0xffffffffu;
enum : std::uint32_t {
    kRestorePhaseSchema=1,
    kRestorePhaseUnpack=2,
    kRestorePhaseCurrentStage=3,
    kRestorePhasePreviousStage=4,
    kRestorePhaseEnvironment=5,
    kRestorePhaseRegistry=6,
    kRestorePhaseHandles=7,
    kRestorePhaseCommon=8,
    kRestorePhaseGui=9,
    kRestorePhaseSpell=10,
    kRestorePhaseHint=11,
    kRestorePhasePlayers=12,
    kRestorePhaseDone=13,
    kRestorePhaseSuccess=14,
};
void set_gui_restore_diag(std::uint32_t phase,std::uint32_t index=kDiagUnknown){
    gSpectatorSceneRestoreDiag[0]=phase;
    gSpectatorSceneRestoreDiag[1]=index;
}
}
namespace {
using Netplay::SpectatorScalars::Reader;
using Netplay::SpectatorScalars::Writer;
constexpr std::uint32_t kSceneSchema=4;
constexpr std::uint32_t kMaxRaw=2u*1024u*1024u;
constexpr std::uint32_t kMaxWire=61440u;
constexpr std::uint32_t kMaxInstructions=1024u*1024u;
constexpr std::uint32_t kMaxVms=8192u;
template<class C> constexpr bool reading=std::is_same_v<C,Reader>;

template<class C> bool vec(C& c,Vec2& v){return c.Values(v.x,v.y);}
template<class C> bool vec(C& c,Vec3& v){return c.Values(v.x,v.y,v.z);}
template<class C> bool matrix(C& c,Matrix4& m){for(auto& row:m.elements)for(auto& f:row)if(!c.Value(f))return false;return true;}
template<class C> bool fog(C& c,AnmFog& f){return c.Values(f.near_distance,f.far_distance)&&c.Array(f.color)&&c.Value(f.packed_color);}

template<class C> bool name(C& c,std::string& value,std::size_t capacity){
    std::uint16_t length=static_cast<std::uint16_t>(value.size());
    if constexpr(!reading<C>){if(value.size()>=capacity)return false;}
    if(!c.Value(length)||length>=capacity)return false;
    if constexpr(reading<C>)value.clear();
    for(std::uint16_t i=0;i<length;++i){std::uint8_t ch=0;if constexpr(!reading<C>)ch=static_cast<std::uint8_t>(value[i]);if(!c.Value(ch))return false;if constexpr(reading<C>)value.push_back(static_cast<char>(ch));}
    if constexpr(reading<C>){if(value.find('\0')!=std::string::npos)return false;}
    return true;
}
template<class C> bool name(C& c,const char* source,std::size_t capacity){
    std::string value;
    if constexpr(!reading<C>){if(!source)return false;std::size_t n=0;while(n<capacity&&source[n])++n;if(n==capacity)return false;value.assign(source,n);}
    return name(c,value,capacity);
}

template<class C> bool timer(C& c,Timer& t,float* localRate){
    bool hadRate=t.rate!=nullptr;
    if(!c.Values(t.previous,t.current,t.fractional,hadRate))return false;
    if constexpr(reading<C>)t.rate=hadRate?localRate:nullptr;
    return true;
}
template<class C,class T,unsigned N> bool interpolation(C& c,Interpolator<T,N>& i,float* rate){
    if(!c.Array(i.start)||!c.Array(i.end)||!c.Array(i.initial_tangent)||!c.Array(i.final_tangent)||
       !timer(c,i.timer,rate)||!c.Values(i.flags,i.duration,i.mode))return false;
    return true;
}
template<class C> bool camera(C& c,Camera& v,float* rate){
    (void)rate;
    return vec(c,v.position)&&vec(c,v.target_offset)&&vec(c,v.up)&&vec(c,v.reserved_024)&&
        vec(c,v.right)&&vec(c,v.eye_offset)&&c.Value(v.field_of_view)&&matrix(c,v.view)&&
        matrix(c,v.projection)&&c.Values(v.viewport.x,v.viewport.y,v.viewport.width,v.viewport.height,
        v.viewport.near_depth,v.viewport.far_depth,v.screen_space)&&vec(c,v.draw_offset)&&
        vec(c,v.animation_delta)&&fog(c,v.fog);
}

AnmFile* file_at(browser::World&,i32,const std::string&);
i32 file_slot(const browser::World&,const AnmFile*);

template<class C> bool handle(C& c,u32& id,const browser::World& world,const SpectatorStageState& saved,
                              bool encodeDeadHandle=false){
    u32 source=id;bool dead=false;
    if constexpr(!reading<C>){
        if(encodeDeadHandle&&source)dead=world.engine.manager.registry.find(source)==nullptr;
    }
    if(!c.Value(source)||(encodeDeadHandle&&!c.Value(dead)))return false;
    if constexpr(reading<C>){
        const u32 target=ResolveSpectatorStageAnm(world,saved,source);
        if(encodeDeadHandle){
            if(dead&&(!source||target))return false;
            if(dead){gSpectatorSceneRestoreDiag[2]=source;gSpectatorSceneRestoreDiag[3]=0;id=0;return true;}
        }
        if(source&&!target){
            gSpectatorSceneRestoreDiag[2]=source;
            gSpectatorSceneRestoreDiag[3]=target;
            return false;
        }
        id=target;
    }
    return true;
}

template<class C> bool file_reference(C& c,browser::World& world,AnmFile*& file){
    i32 slot=reading<C>?-1:file_slot(world,file);std::string resource;
    if constexpr(!reading<C>){if(file){if(slot<0)return false;resource.assign(file->name);}}
    if(!c.Value(slot)||slot< -1||!name(c,resource,sizeof(AnmFile::name)))return false;
    if constexpr(reading<C>){
        gSpectatorSceneRestoreDiag[2]=static_cast<u32>(slot);
        gSpectatorSceneRestoreDiag[4]=static_cast<u32>(slot);
        if(slot<0){if(slot!= -1||!resource.empty())return false;file=nullptr;gSpectatorSceneRestoreDiag[3]=0;}
        else{file=file_at(world,slot,resource);gSpectatorSceneRestoreDiag[3]=file?1u:0u;if(!file)return false;}
    }
    return true;
}

AnmFile* file_at(browser::World& world,i32 slot,const std::string& resource){
    if(slot<0||slot>=AnmManager::file_slot_count)return nullptr;
    auto* file=world.engine.manager.files[slot];
    if(!file||!file->loaded||file->unavailable||resource.empty()||std::strncmp(file->name,resource.c_str(),sizeof(file->name))!=0)return nullptr;
    return file;
}
i32 file_slot(const browser::World& world,const AnmFile* file){
    if(!file)return -1;
    for(i32 i=0;i<AnmManager::file_slot_count;++i)if(world.engine.manager.files[i]==file)return i;
    return -1;
}
bool valid_anm_pc(const AnmFile& file,i32 script,i32 offset,AnmInstruction*& out){
    out=nullptr;if(offset<0)return offset==-1;
    if(script<0||script>=file.script_count||!file.scripts||!file.scripts[script]||static_cast<u32>(offset)>kMaxInstructions)return false;
    auto* pc=file.scripts[script];u32 at=0;
    // ANM files do not retain their input byte length. The next parsed script
    // still provides a safe local upper bound for every non-final script.
    // Last-script offsets remain bounded by the conservative instruction cap.
    const auto base=reinterpret_cast<std::uintptr_t>(pc);
    std::uintptr_t end=base+kMaxInstructions;
    for(i32 i=0;i<file.script_count;++i){
        if(!file.scripts[i])return false;
        const auto candidate=reinterpret_cast<std::uintptr_t>(file.scripts[i]);
        if(candidate>base&&candidate<end)end=candidate;
    }
    const u32 byteLimit=static_cast<u32>(end-base);
    const bool boundedByNext=end!=base+kMaxInstructions;
    if(static_cast<u32>(offset)>byteLimit||(boundedByNext&&static_cast<u32>(offset)>=byteLimit))return false;
    for(u32 steps=0;steps<100000;++steps){
        if(at==static_cast<u32>(offset)){out=pc;return true;}
        if(at>static_cast<u32>(offset)||!pc||pc->length<8||pc->length>byteLimit-at||pc->length>static_cast<u32>(offset)-at)return false;
        const auto opcode=pc->opcode;const auto length=pc->length;
        at+=length;
        if(opcode==-1||opcode==1||opcode==2)return false;
        pc=reinterpret_cast<AnmInstruction*>(reinterpret_cast<u8*>(pc)+length);
    }
    return false;
}
i32 anm_pc_offset(const AnmVm& vm,const AnmInstruction* pc){
    if(!pc)return -1;if(!vm.script_begin)return std::numeric_limits<i32>::min();
    const auto base=reinterpret_cast<std::uintptr_t>(vm.script_begin),address=reinterpret_cast<std::uintptr_t>(pc);
    if(address<base||address-base>kMaxInstructions)return std::numeric_limits<i32>::min();
    return static_cast<i32>(address-base);
}

template<class C> bool distortion(C& c,AnmDistortion& d){
    for(auto& v:d.vertices)if(!vec(c,v.position)||!c.Values(v.reciprocal_w,v.color)||!vec(c,v.uv))return false;
    return c.Array(d.radii)&&c.Array(d.radial_velocity)&&c.Values(d.u_velocity,d.v_velocity,d.reserved);
}
u8 geometry_kind(const AnmVm& vm){
    if((vm.flags&0x3c00000u)==0x2400000u)return vm.geometry?1:255;
    if(vm.update_callback==callback_id::DistortionUpdate&&vm.draw_callback==callback_id::DistortionDraw)return vm.geometry?2:255;
    if(!vm.geometry)return 0;
    return 255;
}

template<class C> bool anm_vm(C& c,AnmVm& vm,browser::World& world,u32* sourceIdOut=nullptr,
                             u32 sourceIdWrite=0,bool releaseOldGeometry=false){
    u32 sourceId=reading<C>?0:(sourceIdWrite?sourceIdWrite:vm.id);
    std::string resource;
    i32 slot=-1,script=vm.script_index;
    i32 instructionOffset=-1,savedOffset=-1;
    u8 geom=0;bool hasScript=vm.script_begin!=nullptr;
    if constexpr(!reading<C>){
        if(vm.animation_file){resource.assign(vm.animation_file->name);slot=file_slot(world,vm.animation_file);if(slot<0)return false;}
        if(hasScript&&(!vm.animation_file||script<0||script>=vm.animation_file->script_count||
           vm.script_begin!=vm.animation_file->scripts[script]))return false;
        instructionOffset=anm_pc_offset(vm,vm.instruction);savedOffset=anm_pc_offset(vm,vm.saved_instruction);
        if(instructionOffset==std::numeric_limits<i32>::min()||savedOffset==std::numeric_limits<i32>::min())return false;
        geom=geometry_kind(vm);if(geom==255)return false;
    }
    if(!c.Value(sourceId)||!name(c,resource,sizeof(AnmFile::name))||!c.Values(slot,script,hasScript,instructionOffset,savedOffset,geom)||geom>2)return false;
    AnmFile* file=nullptr;AnmInstruction* instruction=nullptr;AnmInstruction* savedInstruction=nullptr;
    if constexpr(reading<C>){
        if(slot< -1)return false;
        if(slot>=0){file=file_at(world,slot,resource);if(!file)return false;}
        else if(!resource.empty())return false;
        if(hasScript){if(!file||script<0||script>=file->script_count||!file->scripts||!file->scripts[script])return false;
            if(!valid_anm_pc(*file,script,instructionOffset,instruction))return false;
            if(savedOffset>=0&&!valid_anm_pc(*file,script,savedOffset,savedInstruction))return false;
            if(savedOffset==-1)savedInstruction=nullptr;
        }else if(instructionOffset!=-1||savedOffset!=-1)return false;
        if(!file&&geom!=0)return false;
        if(releaseOldGeometry&&vm.geometry)world.engine.release_memory(vm.geometry);
        vm.clear();vm.initialize();
    }
    if(!c.Values(vm.owner_tag))return false;
    if(!vec(c,vm.rotation)||!vec(c,vm.angular_velocity)||!vec(c,vm.scale)||!vec(c,vm.scale_velocity)||
       !vec(c,vm.sprite_size)||!vec(c,vm.uv_offset)||!timer(c,vm.script_timer,&world.engine.speed)||
       !c.Value(vm.script_timer_flags)||!interpolation(c,vm.position_interpolation,&world.engine.speed)||
       !interpolation(c,vm.color_interpolation,&world.engine.speed)||!interpolation(c,vm.alpha_interpolation,&world.engine.speed)||
       !interpolation(c,vm.rotation_interpolation,&world.engine.speed)||!interpolation(c,vm.scale_interpolation,&world.engine.speed)||
       !interpolation(c,vm.color2_interpolation,&world.engine.speed)||!interpolation(c,vm.alpha2_interpolation,&world.engine.speed)||
       !vec(c,vm.uv_velocity)||!matrix(c,vm.sprite_matrix)||!matrix(c,vm.transform_matrix)||!matrix(c,vm.uv_matrix)||
       !c.Values(vm.color,vm.secondary_color,vm.pending_interrupt,vm.reserved_306))return false;
    if constexpr(reading<C>)vm.animation_file=file;
    if(!c.Array(vm.integer_variables)||!c.Array(vm.float_variables)||!c.Array(vm.extra_integer_variables)||
       !vec(c,vm.script_position)||!vec(c,vm.position)||!vec(c,vm.child_position)||
       !c.Values(vm.flags,vm.reserved_360[0],vm.reserved_360[1])||
       !timer(c,vm.saved_timer,&world.engine.speed)||!c.Value(vm.saved_timer_flags))return false;
    // The file/script pointers are process-local; their relative PCs above are the portable identity.
    if(!c.Values(vm.sprite_frame,vm.sprite_index))return false;
    i32 encodedScript=script;
    if(!c.Values(encodedScript,vm.update_callback,vm.draw_callback))return false;
    if(!c.Array(vm.text_settings))return false;
    if constexpr(reading<C>){
        if(encodedScript!=script||encodedScript< -1||(file&&encodedScript>=file->script_count))return false;
        vm.id=0;vm.file_index=file?static_cast<u16>(file->file_index):0;
        vm.script_index=static_cast<std::int16_t>(encodedScript);
        vm.script_begin=(hasScript&&file&&encodedScript>=0)?file->scripts[encodedScript]:nullptr;
        vm.instruction=instruction;vm.saved_instruction=savedInstruction;
        vm.sprite=(file&&vm.sprite_index>=0&&vm.sprite_index<file->sprite_count)?&file->sprites[vm.sprite_index]:nullptr;
        if(vm.sprite_index< -1||(file&&vm.sprite_index>=0&&!vm.sprite))return false;
        if(((vm.flags&0x3c00000u)==0x2400000u)!=(geom==1))return false;
        if((vm.update_callback==callback_id::DistortionUpdate&&vm.draw_callback==callback_id::DistortionDraw)!=(geom==2))return false;
        if(vm.registry_node.value!=&vm)vm.registry_node.initialize(&vm);
        vm.child_node.initialize(&vm);vm.draw_next=nullptr;
        if(geom==1){
            if(!file||!vm.sprite||vm.integer_variables[0]<1||vm.integer_variables[0]>16384)return false;
            vm.geometry=world.engine.allocate_geometry(static_cast<u32>(vm.integer_variables[0])*0x38u);
            if(!vm.geometry)return false;
            vm.update_ring_geometry();
        }else if(geom==2){
            vm.geometry=world.engine.allocate(sizeof(AnmDistortion));if(!vm.geometry)return false;
            if(!distortion(c,*static_cast<AnmDistortion*>(vm.geometry)))return false;
        }else vm.geometry=nullptr;
        if(sourceIdOut)*sourceIdOut=sourceId;
    }else{
        (void)encodedScript;
        if(geom==2&&!distortion(c,*static_cast<AnmDistortion*>(vm.geometry)))return false;
    }
    return true;
}

bool stage_path(const browser::World& world,i32 stage,const char*& path){
    if(stage<1||stage>7)return false;
    const auto& config=browser::menu_data(world.state.chinese).stages[stage];
    path=reinterpret_cast<const char*>(config.resources[1]);return path&&*path;
}
bool stage_pc_valid(const Stage& stage,i32 offset,StageInstruction*& result){
    result=nullptr;if(offset<0)return offset==-1;
    if(!stage.script_begin||offset<0||static_cast<u32>(offset)>stage.source_size)return false;
    const auto scriptOffset=reinterpret_cast<std::uintptr_t>(stage.script_begin)-reinterpret_cast<std::uintptr_t>(stage.file);
    if(scriptOffset+static_cast<u32>(offset)>=stage.source_size)return false;
    auto* pc=stage.script_begin;u32 at=0;
    for(u32 steps=0;steps<100000;++steps){
        if(at==static_cast<u32>(offset)){result=pc;return true;}
        if(at>static_cast<u32>(offset)||!pc||pc->length<=0||static_cast<u32>(pc->length)>static_cast<u32>(offset)-at)return false;
        at+=static_cast<u32>(pc->length);pc=reinterpret_cast<StageInstruction*>(reinterpret_cast<u8*>(pc)+pc->length);
    }
    return false;
}

Stage* ensure_stage(browser::World& world,bool previous,i32 number,const std::string& path,const std::string& animationName,
                    i32 objectCount,i32 primitiveCount){
    const char* expected=nullptr;if(!stage_path(world,number,expected)||path!=expected)return nullptr;
    Stage*& slot=previous?world.backgrounds.previous:world.backgrounds.current;
    if(slot&&(slot->stage_number!=number||!slot->animation_file||animationName!=slot->animation_file->name)){
        world.backgrounds.destroy(slot);slot=nullptr;
    }
    if(!slot){
        const auto oldStage=world.state.game.stage;auto* oldCurrent=world.state.current_stage;
        world.state.game.stage=number;world.state.current_stage=&browser::menu_data(world.state.chinese).stages[number];
        slot=world.backgrounds.create(path.c_str(),previous?1:0);
        world.state.game.stage=oldStage;world.state.current_stage=oldCurrent;
    }
    if(!slot||!slot->file||!slot->animation_file||slot->file->object_count!=objectCount||
       slot->file->primitive_count!=primitiveCount||animationName!=slot->animation_file->name)return nullptr;
    return slot;
}

template<class C> bool animation_environment(C& c,browser::World& world){
    u8 activeCamera=0;
    if constexpr(!reading<C>){
        if(world.engine.active==&world.engine.world)activeCamera=0;
        else if(world.engine.active==&world.engine.ui)activeCamera=1;
        else return false;
    }
    if(!camera(c,world.engine.world,&world.engine.speed)||
       !camera(c,world.engine.ui,&world.engine.speed)||!vec(c,world.engine.tangent)||
       !c.Values(world.engine.screen_space,world.engine.fog_enabled)||!c.Value(activeCamera)||activeCamera>1)
        return false;
    if constexpr(reading<C>)world.engine.active=activeCamera?&world.engine.ui:&world.engine.world;
    return true;
}

template<class C> bool stage(C& c,browser::World& world,bool previous,Stage*& target,
                            i32 expectedCurrentStage,
                            std::vector<std::pair<Stage*,std::pair<unsigned,u32>>>& pendingHandles){
    bool present=target!=nullptr;if(!c.Value(present))return false;
    if constexpr(reading<C>){if(!present){if(target){world.backgrounds.destroy(target);target=nullptr;}return true;}}
    if(!present)return true;
    i32 stageNumber=target?target->stage_number:world.state.game.stage;
    std::string path,animationName;
    i32 objectCount=target&&target->file?target->file->object_count:0;
    i32 primitiveCount=target&&target->file?target->file->primitive_count:0;
    if constexpr(!reading<C>){const char* p=nullptr;if(!stage_path(world,stageNumber,p)||!target||!target->file||!target->animation_file)return false;path=p;animationName=target->animation_file->name;objectCount=target->file->object_count;primitiveCount=target->file->primitive_count;}
    if(!c.Values(stageNumber,objectCount,primitiveCount)||!name(c,path,260)||!name(c,animationName,sizeof(AnmFile::name))||
       stageNumber<1||stageNumber>7||objectCount<0||objectCount>4096||primitiveCount<0||primitiveCount>16384)return false;
    // The current background must belong to the checkpoint's gameplay stage
    // before ensure_stage creates or replaces any native resources.
    if(!previous&&stageNumber!=expectedCurrentStage)return false;
    if constexpr(reading<C>){target=ensure_stage(world,previous,stageNumber,path,animationName,objectCount,primitiveCount);if(!target)return false;}
    auto& s=*target;
    const bool wasRestarted=s.instruction!=nullptr;
    if constexpr(reading<C>){if(!wasRestarted)s.restart(world.backgrounds.script);}
    // Stage/resource pointers, update-chain links and source bytes stay local.
    i32 instructionOffset=-1;
    if constexpr(!reading<C>){if(s.instruction){const auto a=reinterpret_cast<std::uintptr_t>(s.script_begin),b=reinterpret_cast<std::uintptr_t>(s.instruction);if(b<a||b-a>s.source_size)return false;instructionOffset=static_cast<i32>(b-a);}else instructionOffset=-1;}
    if(!c.Values(s.flags,s.state,s.camera_effect)||!c.Array(s.reserved_021)||
       !timer(c,s.effect_timer,&world.engine.speed)||!c.Value(s.effect_timer_flags)||
       !timer(c,s.script_timer,&world.engine.speed)||!c.Value(s.script_timer_flags)||
       !c.Value(instructionOffset)||!interpolation(c,s.target_interpolation,&world.engine.speed)||
       !interpolation(c,s.position_interpolation,&world.engine.speed)||!fog(c,s.fog_interpolation.start)||
       !fog(c,s.fog_interpolation.end)||!fog(c,s.fog_interpolation.initial_tangent)||
       !fog(c,s.fog_interpolation.final_tangent)||!timer(c,s.fog_interpolation.timer,&world.engine.speed)||
       !c.Values(s.fog_interpolation.flags,s.fog_interpolation.duration,s.fog_interpolation.mode,s.reserved_174)||
       !c.Value(s.draw_distance_squared)||!c.Values(s.effects_enabled,s.fade_color,s.frame_effect)||
       !vec(c,s.effect_position)||!vec(c,s.effect_size)||!c.Values(s.drawn_objects,s.culled_objects,
       s.drawn_primitives,s.draw_flags)||!timer(c,s.fade_timer,&world.engine.speed)||
       !c.Value(s.fade_timer_flags)||!c.Values(s.stage_number,s.frame_count)||s.stage_number!=stageNumber)return false;
    for(auto& id:s.effect_ids){u32 source=id;if(!c.Value(source))return false;if constexpr(reading<C>){id=source;if(source)pendingHandles.push_back({&s,{static_cast<unsigned>(&id-s.effect_ids),source}});}}
    if(!camera(c,s.camera,&world.engine.speed))return false;
    if constexpr(reading<C>){StageInstruction* pc=nullptr;if(instructionOffset>=0){if(!stage_pc_valid(s,instructionOffset,pc))return false;}s.instruction=pc;}
    if(!c.Value(objectCount)||objectCount!=s.file->object_count)return false;
    for(i32 i=0;i<objectCount;++i)if(!c.Value(s.objects[i]->flags))return false;
    if(!c.Value(primitiveCount)||primitiveCount!=s.file->primitive_count||(!s.object_animations&&primitiveCount))return false;
    u32 primitive=0;
    for(i32 object=0;object<objectCount;++object)for(auto* p=s.objects[object]->primitives();p->type>=0;p=p->next()){
        if(primitive>=static_cast<u32>(primitiveCount))return false;
        if(!c.Value(p->animation))return false;
        if constexpr(reading<C>){if(p->animation<0||p->animation>=primitiveCount)return false;}
        ++primitive;
    }
    if(primitive!=static_cast<u32>(primitiveCount))return false;
    for(i32 i=0;i<primitiveCount;++i)if(!anm_vm(c,s.object_animations[i],world,nullptr,0,reading<C>))return false;
    for(auto& vm:s.script_animations)if(!anm_vm(c,vm,world,nullptr,0,reading<C>))return false;
    for(auto& vm:s.effect_animations)if(!anm_vm(c,vm,world,nullptr,0,reading<C>))return false;
    return true;
}

struct RegistryRecord {AnmVm* vm=nullptr;u8 placement=0;u32 order=0,parent=0,childOrder=0,occurrence=0,sourceId=0;};
bool collect_registry(browser::World& world,std::vector<RegistryRecord>& out){
    auto& r=world.engine.manager.registry;
    for(u8 placement=0;placement<2;++placement){auto* node=placement?r.ui_head:r.world_head;u32 order=0;
        for(;node;node=node->next){if(!node->value||!node->value->id||out.size()>=kMaxVms)return false;out.push_back({node->value,placement,order++,0,0,0,node->value->id});}}
    for(auto& rec:out){auto& root=rec.vm->child_node;if(root.previous)continue;u32 childOrder=0;
        for(auto* child=root.next;child;child=child->next){if(!child->value)return false;auto found=std::find_if(out.begin(),out.end(),[&](const RegistryRecord& item){return item.vm==child->value;});if(found==out.end()||found->parent)return false;found->parent=rec.vm->id;found->childOrder=childOrder++;}}
    for(auto& rec:out){const auto* file=rec.vm->animation_file;if(!file)return false;u32 occurrence=0;for(const auto& earlier:out){if(&earlier==&rec)break;if(earlier.placement==rec.placement&&earlier.vm->script_index==rec.vm->script_index&&earlier.vm->animation_file&&std::strcmp(earlier.vm->animation_file->name,file->name)==0)++occurrence;}rec.occurrence=occurrence;}
    return true;
}
template<class C> bool registry_vm(C& c,browser::World& world,RegistryRecord& rec,
                                   std::vector<SpectatorStageAnmHandle>* handles,
                                   std::vector<std::pair<u32,AnmVm*>>* restored,
                                   bool releaseOld){
    u32 sourceId=reading<C>?rec.sourceId:rec.sourceId;
    if(!c.Values(sourceId,rec.placement,rec.order,rec.parent,rec.childOrder,rec.occurrence)||rec.placement>1)return false;
    if constexpr(reading<C>){
        auto* vm=world.engine.manager.allocate(world.engine);if(!vm)return false;
        rec.vm=vm;u32 decodedSource=0;
        if(!anm_vm(c,*vm,world,&decodedSource,0,releaseOld)||decodedSource!=sourceId)return false;
        vm->id=0;
        const auto placement=rec.placement?AnimationPlacement::UiBack:AnimationPlacement::WorldBack;
        const u32 id=world.engine.manager.insert(*vm,placement);if(!id)return false;
        vm->id=id;if(restored)restored->push_back({sourceId,vm});if(handles)handles->push_back({sourceId,id});
    }else{
        if(!rec.vm||!anm_vm(c,*rec.vm,world,nullptr,sourceId,false))return false;
    }
    return true;
}
void clear_registry(browser::World& world){
    auto& manager=world.engine.manager;
    while(manager.registry.world_head){auto* vm=manager.registry.world_head->value;if(!vm)__builtin_trap();manager.remove(*vm,world.engine);}
    while(manager.registry.ui_head){auto* vm=manager.registry.ui_head->value;if(!vm)__builtin_trap();manager.remove(*vm,world.engine);}
}

template<class C> bool registry(C& c,browser::World& world,std::vector<SpectatorStageAnmHandle>* handles){
    u32 count=0;
    std::vector<RegistryRecord> records;
    if constexpr(!reading<C>){if(!collect_registry(world,records))return false;count=static_cast<u32>(records.size());}
    if(!c.Value(count)||count>kMaxVms)return false;
    std::vector<std::pair<u32,AnmVm*>> restored;
    if constexpr(reading<C>)clear_registry(world);
    u32 worldOrder=0,uiOrder=0;
    for(u32 i=0;i<count;++i){RegistryRecord rec{};if constexpr(!reading<C>)rec=records[i];
        if constexpr(reading<C>){
            // Input records are sorted world list then UI list. Keep list order exact.
            u32 source=0;u8 place=0;u32 order=0,parent=0,child=0,occurrence=0;
            if(!c.Values(source,place,order,parent,child,occurrence)||place>1||!source)return false;
            if(std::any_of(records.begin(),records.end(),[&](const RegistryRecord& item){return item.sourceId==source;}))return false;
            rec.placement=place;rec.order=order;rec.parent=parent;rec.childOrder=child;rec.occurrence=occurrence;rec.sourceId=source;
            const u32 expected=place?uiOrder++:worldOrder++;if(order!=expected)return false;
            auto* vm=world.engine.manager.allocate(world.engine);if(!vm)return false;rec.vm=vm;
            u32 decoded=0;if(!anm_vm(c,*vm,world,&decoded,0,false)||decoded!=source)return false;
            vm->id=0;const auto placement=place?AnimationPlacement::UiBack:AnimationPlacement::WorldBack;
            const u32 id=world.engine.manager.insert(*vm,placement);if(!id)return false;vm->id=id;
            restored.push_back({source,vm});if(handles)handles->push_back({source,id});rec.vm=vm;records.push_back(rec);
        }else{
            if(!registry_vm(c,world,rec,nullptr,nullptr,false))return false;
        }
    }
    if constexpr(reading<C>){
        for(const auto& rec:records){
            if(rec.parent&&(rec.parent==rec.sourceId||std::none_of(records.begin(),records.end(),[&](const RegistryRecord& p){return p.sourceId==rec.parent;})))return false;
            u32 ancestor=rec.parent;
            for(u32 depth=0;ancestor&&depth<=records.size();++depth){
                if(ancestor==rec.sourceId)return false;
                const auto parent=std::find_if(records.begin(),records.end(),[&](const RegistryRecord& item){return item.sourceId==ancestor;});
                if(parent==records.end())return false;ancestor=parent->parent;
                if(depth==records.size()&&ancestor)return false;
            }
        }
        for(const auto& parentRec:records){
            if(std::any_of(records.begin(),records.end(),[&](const RegistryRecord& r){return r.parent==parentRec.sourceId;})){
                std::vector<const RegistryRecord*> children;for(const auto& rec:records)if(rec.parent==parentRec.sourceId)children.push_back(&rec);
                std::sort(children.begin(),children.end(),[](auto* a,auto* b){return a->childOrder>b->childOrder;});
                for(std::size_t i=1;i<children.size();++i)if(children[i-1]->childOrder!=children[i]->childOrder+1)return false;
                if(!children.empty()&&children.back()->childOrder!=0)return false;
                for(auto* child:children)child->vm->child_node.insert_after(parentRec.vm->child_node);
            }
        }
    }
    return true;
}

template<class C> bool common_owner(C& c,browser::World& world,const SpectatorStageState& saved){
    bool present=world.common.value!=nullptr;if(!c.Value(present))return false;
    if constexpr(reading<C>){if(present!=(world.common.value!=nullptr))return false;}
    if(!present)return true;
    auto& value=*world.common.value;
    return c.Values(value.flags,value.state)&&file_reference(c,world,value.effects)&&
        file_reference(c,world,value.capture)&&file_reference(c,world,value.text_animations)&&
        handle(c,value.introduction_animation,world,saved)&&handle(c,value.loading_animation,world,saved)&&
        c.Values(value.text_count,value.early_text_count,value.color,value.scale.x,value.scale.y,
            value.camera,value.reserved_8984,value.shadow,value.character_width,value.frames)&&
        anm_vm(c,value.characters,world,nullptr,0,reading<C>)&&
        anm_vm(c,value.small_characters,world,nullptr,0,reading<C>);
}

template<class C> bool gui_owner(C& c,browser::World& world,const SpectatorStageState& saved){
    if constexpr(reading<C>)set_gui_restore_diag(901,0);
    bool present=world.actors.gui!=nullptr;if(!c.Value(present))return false;
    if constexpr(reading<C>){gSpectatorSceneRestoreDiag[2]=present?1u:0u;gSpectatorSceneRestoreDiag[3]=world.actors.gui?1u:0u;if(present!=(world.actors.gui!=nullptr))return false;}
    if(!present)return true;
    auto& value=*world.actors.gui;
    // Dialogue owns a separate message VM/resource graph and is not part of
    // the stage activation boundary. Reject it rather than silently detach it.
    if constexpr(reading<C>){set_gui_restore_diag(902,0);gSpectatorSceneRestoreDiag[5]=value.dialogue?1u:0u;}
    if(value.dialogue)return false;
    if constexpr(reading<C>)set_gui_restore_diag(903,0);
    bool hasMessage=value.message_file!=nullptr;if(!c.Value(hasMessage))return false;
    if constexpr(reading<C>){gSpectatorSceneRestoreDiag[6]=hasMessage?1u:0u;gSpectatorSceneRestoreDiag[7]=value.message_file?1u:0u;if(hasMessage!=(value.message_file!=nullptr))return false;}
    if constexpr(reading<C>)set_gui_restore_diag(910,0);
    if(!c.Values(value.flags,value.state))return false;
    {u32 i=0;for(auto& vm:value.high_score_digits){if constexpr(reading<C>)set_gui_restore_diag(911,i);if(!anm_vm(c,vm,world,nullptr,0,reading<C>))return false;++i;}}
    {u32 i=0;for(auto& vm:value.score_digits){if constexpr(reading<C>)set_gui_restore_diag(912,i);if(!anm_vm(c,vm,world,nullptr,0,reading<C>))return false;++i;}}
    {u32 i=0;for(auto& vm:value.life_icons){if constexpr(reading<C>)set_gui_restore_diag(913,i);if(!anm_vm(c,vm,world,nullptr,0,reading<C>))return false;++i;}}
    {u32 i=0;for(auto& vm:value.power_digits){if constexpr(reading<C>)set_gui_restore_diag(914,i);if(!anm_vm(c,vm,world,nullptr,0,reading<C>))return false;++i;}}
    {u32 i=0;for(auto& vm:value.countdown_digits){if constexpr(reading<C>)set_gui_restore_diag(915,i);if(!anm_vm(c,vm,world,nullptr,0,reading<C>))return false;++i;}}
    {u32 i=0;for(auto& vm:value.faith_digits){if constexpr(reading<C>)set_gui_restore_diag(916,i);if(!anm_vm(c,vm,world,nullptr,0,reading<C>))return false;++i;}}
    if constexpr(reading<C>)set_gui_restore_diag(917,0);
    if(!anm_vm(c,value.enemy_marker,world,nullptr,0,reading<C>))return false;
    {u32 i=0;for(auto& id:value.bonus_digits){if constexpr(reading<C>)set_gui_restore_diag(920,i);if(!handle(c,id,world,saved))return false;++i;}}
    u32* ids[]={&value.notification,&value.power_notification,&value.unresolved_9e1c[0],
                &value.unresolved_9e1c[1],&value.boss_name};
    constexpr u32 idPhases[]={921,922,923,924,925};
    for(std::size_t i=0;i<std::size(ids);++i){if constexpr(reading<C>)set_gui_restore_diag(idPhases[i],static_cast<u32>(i));if(!handle(c,*ids[i],world,saved))return false;}
    {u32 i=0;for(auto& id:value.boss_life_icons){if constexpr(reading<C>)set_gui_restore_diag(926,i);if(!handle(c,id,world,saved))return false;++i;}}
    u32* otherIds[]={&value.difficulty_badge,&value.difficulty_label,&value.border,&value.background};
    constexpr u32 otherPhases[]={927,928,929,930};
    for(std::size_t i=0;i<std::size(otherIds);++i){if constexpr(reading<C>)set_gui_restore_diag(otherPhases[i],static_cast<u32>(i));if(!handle(c,*otherIds[i],world,saved,i==0))return false;}
    if constexpr(reading<C>)set_gui_restore_diag(940,0);
    if(!timer(c,value.elapsed,&world.engine.speed))return false;
    if constexpr(reading<C>)set_gui_restore_diag(941,0);
    if(!c.Value(value.elapsed_flags))return false;
    if constexpr(reading<C>)set_gui_restore_diag(942,0);
    if(!c.Values(value.displayed_high_score,value.displayed_score,value.score_step))return false;
    if constexpr(reading<C>)set_gui_restore_diag(943,0);
    if(!file_reference(c,world,value.stage_animations))return false;
    if constexpr(reading<C>)set_gui_restore_diag(944,0);
    if(!c.Values(value.displayed_boss_health,value.target_boss_health,value.boss_health_points,value.boss_lives))return false;
    {u32 i=0;for(auto& bar:value.boss_health){if constexpr(reading<C>)set_gui_restore_diag(945,i);if(!c.Values(bar.amount,bar.style))return false;++i;}}
    if constexpr(reading<C>)set_gui_restore_diag(946,0);
    if(!c.Values(value.display_flags,value.countdown,value.previous_countdown,value.ending_frames))return false;
    if constexpr(reading<C>)set_gui_restore_diag(947,0);
    return file_reference(c,world,value.animations);
}

template<class C> bool spell_owner(C& c,browser::World& world,const SpectatorStageState& saved){
    bool present=world.actors.spell!=nullptr;if(!c.Value(present))return false;
    if constexpr(reading<C>){if(present!=(world.actors.spell!=nullptr))return false;}
    if(!present)return true;
    auto& value=*world.actors.spell;
    if(!c.Values(value.flags,value.state))return false;
    for(auto& vm:value.backgrounds)if(!anm_vm(c,vm,world,nullptr,0,reading<C>))return false;
    for(auto& id:value.title_animations)if(!handle(c,id,world,saved))return false;
    if(!handle(c,value.circle_animation,world,saved))return false;
    for(auto& vm:value.bonus_digits)if(!anm_vm(c,vm,world,nullptr,0,reading<C>))return false;
    for(auto& vm:value.record_digits)if(!anm_vm(c,vm,world,nullptr,0,reading<C>))return false;
    return timer(c,value.elapsed,&world.engine.speed)&&c.Value(value.elapsed_flags)&&
        c.Array(value.name)&&c.Values(value.number,value.spell_flags,value.bonus,value.initial_bonus,
            value.duration)&&vec(c,value.circle_position)&&c.Value(value.reserved_37a8);
}

template<class C> bool hint_owner(C& c,browser::World& world,const SpectatorStageState& saved){
    bool present=world.actors.hints!=nullptr;if(!c.Value(present))return false;
    if constexpr(reading<C>){if(present!=(world.actors.hints!=nullptr))return false;}
    if(!present)return true;
    auto& value=*world.actors.hints;
    if(!c.Values(value.flags,value.manager_state,value.frames,value.section_frames,value.previous_section))return false;
    for(auto& id:value.animations)if(!handle(c,id,world,saved))return false;
    for(auto& position:value.positions)if(!vec(c,position))return false;
    return c.Array(value.widths)&&c.Value(value.cursor);
}

bool raw_scene(Writer& writer,browser::World& world){
    if(!writer.Value(kSceneSchema))return false;
    auto* current=world.backgrounds.current;auto* previous=world.backgrounds.previous;
    std::vector<std::pair<Stage*,std::pair<unsigned,u32>>> pending;
    if(!stage(writer,world,false,current,world.state.game.stage,pending)||
       !stage(writer,world,true,previous,world.state.game.stage,pending)||
       !animation_environment(writer,world))return false;
    std::vector<RegistryRecord> records;if(!collect_registry(world,records))return false;
    if(!writer.Value(static_cast<u32>(records.size())))return false;
    for(auto& rec:records)if(!registry_vm(writer,world,rec,nullptr,nullptr,false))return false;
    // Registry-backed IDs have a different local allocator identity after
    // reconstruction. Visit every live game owner that keeps such IDs.
    SpectatorStageState ownerView{};
    if(!common_owner(writer,world,ownerView)||!gui_owner(writer,world,ownerView)||
       !spell_owner(writer,world,ownerView)||!hint_owner(writer,world,ownerView))return false;
    for(u32 seat=0;seat<3;++seat){auto* p=world.pilots[seat].player;bool present=p!=nullptr;if(!writer.Value(present))return false;if(present&&!anm_vm(writer,p->animation,world))return false;}
    return writer.Good();
}

bool restore_raw(Reader& reader,browser::World& world,const SpectatorStageState& saved,
                 std::vector<SpectatorStageAnmHandle>& handles){
    gSpectatorSceneRestorePhase=kRestorePhaseSchema;
    u32 schema=0;if(!reader.Value(schema)||schema!=kSceneSchema)return false;
    std::vector<std::pair<Stage*,std::pair<unsigned,u32>>> pending;
    Stage* current=world.backgrounds.current;Stage* previous=world.backgrounds.previous;
    const i32 checkpointStage=static_cast<i32>(saved.checkpoint.label&255u);
    gSpectatorSceneRestorePhase=kRestorePhaseCurrentStage;
    if(world.state.game.stage!=checkpointStage||!stage(reader,world,false,current,checkpointStage,pending))return false;
    gSpectatorSceneRestorePhase=kRestorePhasePreviousStage;
    if(!stage(reader,world,true,previous,checkpointStage,pending))return false;
    gSpectatorSceneRestorePhase=kRestorePhaseEnvironment;
    if(!animation_environment(reader,world))return false;
    world.backgrounds.current=current;world.backgrounds.previous=previous;
    for(auto& presentation:world.backgrounds.presentation)presentation={};
    gSpectatorSceneRestorePhase=kRestorePhaseRegistry;
    if(!registry(reader,world,&handles))return false;
    saved.scene.resolvedHandles=handles;
    gSpectatorSceneRestorePhase=kRestorePhaseHandles;
    for(auto& item:pending){auto found=std::find_if(handles.begin(),handles.end(),[&](const auto& h){return h.sourceId==item.second.second;});if(found==handles.end())return false;item.first->effect_ids[item.second.first]=found->targetId;}
    gSpectatorSceneRestorePhase=kRestorePhaseCommon;
    if(!common_owner(reader,world,saved))return false;
    gSpectatorSceneRestorePhase=kRestorePhaseGui;
    if(!gui_owner(reader,world,saved))return false;
    gSpectatorSceneRestorePhase=kRestorePhaseSpell;
    if(!spell_owner(reader,world,saved))return false;
    gSpectatorSceneRestorePhase=kRestorePhaseHint;
    if(!hint_owner(reader,world,saved))return false;
    gSpectatorSceneRestorePhase=kRestorePhasePlayers;
    for(u32 seat=0;seat<3;++seat){bool present=world.pilots[seat].player!=nullptr;if(!reader.Value(present))return false;
        if(present){auto* player=world.pilots[seat].player;if(!player)return false;if(!anm_vm(reader,player->animation,world,nullptr,0,true))return false;}}
    gSpectatorSceneRestorePhase=kRestorePhaseDone;
    if(!reader.Done())return false;
    gSpectatorSceneRestorePhase=kRestorePhaseSuccess;
    return true;
}
} // namespace

bool PackSpectatorStageFields(const std::vector<std::uint8_t>& raw,std::uint32_t& rawSize,
                              std::uint32_t& checksum,std::vector<std::uint8_t>& compressed){
    if(raw.empty()||raw.size()>kMaxRaw)return false;
    std::array<std::int32_t,65536> heads;heads.fill(-1);
    std::vector<std::int32_t> previous(raw.size(),-1);compressed.clear();compressed.reserve(std::min<std::size_t>(raw.size(),kMaxWire));
    auto hash=[&](std::size_t at)->u32{return (u32(raw[at])*251u+u32(raw[at+1])*31u+raw[at+2])&65535u;};
    auto add=[&](std::size_t at){if(at+2>=raw.size())return;const auto h=hash(at);previous[at]=heads[h];heads[h]=static_cast<std::int32_t>(at);};
    std::size_t at=0;
    while(at<raw.size()){
        const auto controlAt=compressed.size();compressed.push_back(0);u8 control=0;
        for(unsigned bit=0;bit<8&&at<raw.size();++bit){
            std::size_t best=0,bestDistance=0;
            if(at+2<raw.size()){
                auto candidate=heads[hash(at)];unsigned checked=0;
                while(candidate>=0&&at-static_cast<std::size_t>(candidate)<=4096&&checked++<96){
                    const auto distance=at-static_cast<std::size_t>(candidate);std::size_t length=0;
                    while(length<18&&at+length<raw.size()&&raw[static_cast<std::size_t>(candidate)+length]==raw[at+length])++length;
                    if(length>best){best=length;bestDistance=distance;if(best==18)break;}
                    candidate=previous[static_cast<std::size_t>(candidate)];
                }
            }
            if(best>=3){
                const u16 token=static_cast<u16>((bestDistance-1)|((best-3)<<12));
                compressed.push_back(static_cast<u8>(token));compressed.push_back(static_cast<u8>(token>>8));
                for(std::size_t n=0;n<best;++n)add(at+n);at+=best;
            }else{
                control|=static_cast<u8>(1u<<bit);compressed.push_back(raw[at]);add(at);++at;
            }
            if(compressed.size()>kMaxWire)return false;
        }
        compressed[controlAt]=control;
    }
    std::uint32_t hashValue=2166136261u;for(auto b:raw){hashValue^=b;hashValue*=16777619u;}
    rawSize=static_cast<u32>(raw.size());checksum=hashValue;return !compressed.empty()&&compressed.size()<=kMaxWire;
}

bool UnpackSpectatorStageFields(std::uint32_t rawSize,std::uint32_t checksum,
                                const std::vector<std::uint8_t>& compressed,std::vector<std::uint8_t>& raw){
    if(!rawSize||rawSize>kMaxRaw||compressed.empty()||compressed.size()>kMaxWire)return false;
    raw.clear();raw.reserve(rawSize);std::size_t at=0;
    while(at<compressed.size()&&raw.size()<rawSize){const u8 control=compressed[at++];unsigned bit=0;
        for(;bit<8&&raw.size()<rawSize;++bit){
            if(control&(1u<<bit)){if(at>=compressed.size())return false;raw.push_back(compressed[at++]);}
            else{if(compressed.size()-at<2)return false;const u16 token=u16(compressed[at])|u16(compressed[at+1])<<8;at+=2;
                const std::size_t distance=(token&0x0fffu)+1,length=(token>>12)+3;
                if(distance>raw.size()||length>rawSize-raw.size())return false;
                for(std::size_t n=0;n<length;++n)raw.push_back(raw[raw.size()-distance]);}
        }
        if(raw.size()==rawSize&&bit<8){const u8 unused=static_cast<u8>(0xffu<<bit);if(control&unused)return false;}
    }
    if(at!=compressed.size()||raw.size()!=rawSize)return false;
    std::uint32_t hashValue=2166136261u;for(auto b:raw){hashValue^=b;hashValue*=16777619u;}
    return hashValue==checksum;
}

bool ValidateSpectatorStageScene(const SpectatorStageSceneSnapshot& scene){
    if(!scene.present)return !scene.uncompressedBytes&&!scene.checksum&&scene.compressed.empty();
    std::vector<u8> raw;return UnpackSpectatorStageFields(scene.uncompressedBytes,scene.checksum,scene.compressed,raw);
}
bool WriteSpectatorStageScene(Writer& writer,const SpectatorStageSceneSnapshot& scene){
    if(!ValidateSpectatorStageScene(scene))return false;
    if(!writer.Values(scene.present,scene.uncompressedBytes,scene.checksum))return false;
    if(scene.compressed.size()>kMaxWire)return false;
    const u32 size=static_cast<u32>(scene.compressed.size());if(!writer.Value(size))return false;
    for(auto byte:scene.compressed)if(!writer.Value(byte))return false;return writer.Good();
}
bool ReadSpectatorStageScene(Reader& reader,SpectatorStageSceneSnapshot& scene){
    u32 size=0;if(!reader.Values(scene.present,scene.uncompressedBytes,scene.checksum,size)||size>kMaxWire)return false;
    scene.compressed.resize(size);for(auto& byte:scene.compressed)if(!reader.Value(byte))return false;
    scene.resolvedHandles.clear();return ValidateSpectatorStageScene(scene);
}
bool CaptureSpectatorStageScene(browser::World& world,SpectatorStageState& state){
    state.scene={};Writer writer(kMaxRaw);if(!raw_scene(writer,world)||!writer.Good())return false;
    state.scene.present=true;if(!PackSpectatorStageFields(writer.Bytes(),state.scene.uncompressedBytes,state.scene.checksum,state.scene.compressed)){state.scene={};return false;}
    return ValidateSpectatorStageScene(state.scene);
}
bool RestoreSpectatorStageScene(browser::World& world,const SpectatorStageState& state){
    gSpectatorSceneRestorePhase=0;
    for(auto& value:gSpectatorSceneRestoreDiag)value=kDiagUnknown;
    if(!state.scene.present){gSpectatorSceneRestorePhase=kRestorePhaseSuccess;return true;}
    gSpectatorSceneRestorePhase=kRestorePhaseUnpack;
    if(!ValidateSpectatorStageScene(state.scene))return false;
    std::vector<u8> raw;if(!UnpackSpectatorStageFields(state.scene.uncompressedBytes,state.scene.checksum,state.scene.compressed,raw))return false;
    Reader reader(raw);std::vector<SpectatorStageAnmHandle> handles;if(!restore_raw(reader,world,state,handles))return false;
    state.scene.resolvedHandles=std::move(handles);return true;
}
u32 ResolveSpectatorStageAnm(const browser::World&,const SpectatorStageState& state,u32 sourceId){
    if(!sourceId)return 0;const auto& map=state.scene.resolvedHandles;
    auto found=std::find_if(map.begin(),map.end(),[&](const SpectatorStageAnmHandle& item){return item.sourceId==sourceId;});
    return found==map.end()?0:found->targetId;
}
} // namespace th10::multiplayer

extern "C" __attribute__((used,export_name("multiplayer_spectator_scene_restore_phase")))
std::uint32_t multiplayer_spectator_scene_restore_phase(){
    return th10::multiplayer::gSpectatorSceneRestorePhase;
}
extern "C" __attribute__((used,export_name("multiplayer_spectator_scene_restore_diag")))
const std::uint32_t* multiplayer_spectator_scene_restore_diag(){
    return th10::multiplayer::gSpectatorSceneRestoreDiag;
}
