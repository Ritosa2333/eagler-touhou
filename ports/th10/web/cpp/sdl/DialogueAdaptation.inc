// Read-only loaded-source evidence and one gated call through the actual ECL
// dialogue command. No instruction, timer, key, or resource bytes are patched.
#include "../../../portable/input/DialogueSource.hpp"
namespace {
struct LoadedDialogueSources {
    th10::adaptation::SourceBytes ecl{},message{};
    th10::adaptation::DialogueSources parsed{};
    u32 reason=1,messageAsset=0,difficulty=0;
};
LoadedDialogueSources loaded_dialogue_sources(Application* app){
    LoadedDialogueSources result;if(!app)return result;
    auto* world=app->world;result.reason=2;
    if(!world||world->loading||!world->actors.session||!world->actors.player||!world->actors.enemies||
       !world->actors.gui||!world->hud||!app->state.current_stage)return result;
    result.reason=3;
    if(app->state.game.stage!=1||app->state.game.character<0||app->state.game.character>1||
       (app->state.game.flags&0x30)||app->state.practice.enabled||!app->state.replay||app->state.replay->mode!=0)return result;
    const auto& gate=app->state.adaptation.scene;result.reason=4;
    if(!gate.active||!gate.firstBlocked||gate.executedCalls||gate.unexpectedOwners)return result;
    const auto* writes=sdl_validation_write_receipt();result.reason=5;
    if(!writes[1]||writes[2]||writes[6]||writes[7])return result;
    result.reason=6;const auto* program=world->actors.enemies->program;if(!program)return result;
    const auto* eclName=reinterpret_cast<const char*>(app->state.current_stage->resources[3]);
    if(!eclName||std::strcmp(eclName,"stage01.ecl"))return result;
    for(const auto* file:program->loaded_files){const auto found=world->validation_script_sources.find(file);
        if(file&&found!=world->validation_script_sources.end()&&found->second.name==eclName){result.ecl={file,found->second.size};break;}}
    const auto* message=world->actors.gui->message_file;const auto found=world->hud->validation_message_sources.find(message);
    const char* messageName=app->state.current_stage->messages[app->state.game.character];
    if(found==world->hud->validation_message_sources.end()||found->second.name!=messageName)return result;
    if(!std::strcmp(messageName,"st01_00.msg"))result.messageAsset=1;
    else if(!std::strcmp(messageName,"st01_01.msg"))result.messageAsset=2;else return result;
    result.message={message,found->second.size};if(!result.ecl.data||!result.ecl.size||!result.message.size)return result;
    const auto difficulty=app->state.game.difficulty;if(difficulty<0||difficulty>4){result.reason=3;return result;}
    result.difficulty=1u<<difficulty;
    result.parsed=th10::adaptation::dialogue_sources(result.ecl,result.message,result.difficulty);result.reason=result.parsed.reason;
    return result;
}
void dialogue_identity(u32* out,Application* app,u32 magic,u32 words,u32 reason){
    std::fill(out,out+words,0);out[0]=magic;out[1]=1;out[2]=words;out[3]=reason==0;out[4]=reason;
    out[5]=session_generation;out[6]=static_cast<u32>(reinterpret_cast<uintptr_t>(app));out[7]=sdl_loop_epoch();
}
}
extern "C" {
EXPORT("th10_dialogue_touch_manifest_v1") const u32* th10_dialogue_touch_manifest_v1(){
    static u32 out[344];auto* app=session?session->app:nullptr;const auto sources=loaded_dialogue_sources(app);
    dialogue_identity(out,app,0x44544d31,344,sources.reason);if(sources.reason)return out;
    out[8]=app->state.game.stage;out[9]=out[10]=app->state.game.character;out[11]=sources.difficulty;
    out[12]=sources.ecl.size;const auto eclHash=sources.ecl.fnv();out[13]=u32(eclHash);out[14]=u32(eclHash>>32);
    out[15]=sources.parsed.subroutines;out[16]=sources.parsed.entries;out[17]=sources.message.size;
    const auto messageHash=sources.message.fnv();out[18]=u32(messageHash);out[19]=u32(messageHash>>32);
    out[20]=1;out[21]=sources.messageAsset;out[22]=sources.parsed.candidates.size();out[23]=10;
    for(u32 i=0;i<out[22];++i)std::copy(sources.parsed.candidates[i].begin(),sources.parsed.candidates[i].end(),out+24+i*10);
    return out;
}
EXPORT("th10_dialogue_touch_observation_v1") const u32* th10_dialogue_touch_observation_v1(){
    static u32 out[45];auto* app=session?session->app:nullptr;const auto sources=loaded_dialogue_sources(app);
    dialogue_identity(out,app,0x44544f31,45,sources.reason);if(!app)return out;
    auto& observed=app->state.adaptation;const auto& gate=observed.scene;const auto* write=sdl_validation_write_receipt();
    const auto* world=app->world;const auto* gui=world?world->actors.gui:nullptr;const auto* dialogue=gui?gui->dialogue:nullptr;
    out[8]=observed.hostFrames;out[9]=!app->stopped;out[10]=world&&world->actors.session&&!world->loading;
    out[11]=app->state.game.stage;out[12]=app->state.game.character;out[13]=app->state.game.flags;
    out[14]=gate.status==2;out[15]=gate.active;out[16]=gate.updateStarted;out[17]=gate.blockedCalls;
    out[18]=write[1];out[19]=write[2];out[20]=write[6];out[21]=dialogue!=nullptr;
    out[22]=out[23]=out[24]=out[25]=~0u;
    if(dialogue){
        const auto base=reinterpret_cast<uintptr_t>(sources.message.data),cursor=reinterpret_cast<uintptr_t>(dialogue->instruction);
        if(!sources.message.data||cursor<base||cursor-base>sources.message.size||!sources.message.has(cursor-base,4)){out[3]=0;out[4]=7;return out;}
        const auto offset=u32(cursor-base);const auto length=sources.message.data[offset+3];
        if(!sources.message.has(offset,4+length)){out[3]=0;out[4]=7;return out;}
        out[22]=dialogue->id;out[23]=offset;out[24]=dialogue->instruction->opcode;out[25]=dialogue->instruction->time;
        out[26]=std::max(0,dialogue->script_time.current);
        if(out[24]==10&&length>=4&&dialogue->wait.current>0){const auto timeout=sources.message.u32(offset+4);out[27]=timeout>=u32(dialogue->wait.current)?timeout-u32(dialogue->wait.current):0;}
        out[28]=0; // TH10 has no separate minimum-confirm interval; opcode 10 reads fresh input immediately.
        out[29]=(dialogue->flags&1)!=0;out[30]=0;
    }
    out[31]=observed.dialogueCurrent;out[32]=observed.dialoguePrevious;
    out[33]=observed.dialogueValid&&(out[31]&1)&&!(out[32]&1);out[34]=observed.dialogueSequence;
    out[35]=observed.hostFrames;out[36]=world&&world->actors.enemies?world->actors.enemies->lifetime.current:0;
    out[37]=gestures.fire;out[38]=gestures.current_context();
    for(const auto& key:keyboard_map)if(key.vk==90&&key.hosted)out[39]=1;
    out[40]=observed.dialogueRequestSerial;out[41]=observed.dialogueRequestStatus;
    out[42]=observed.dialogueEntry;out[43]=observed.dialogueSubroutine;out[44]=observed.dialogueValid;
    return out;
}
EXPORT("th10_dialogue_touch_request_v1") u32 th10_dialogue_touch_request_v1(u32 entry){
    auto* app=session?session->app:nullptr;if(!app)return 3;auto& observed=app->state.adaptation;
    ++observed.dialogueRequestSerial;observed.dialogueEntry=entry;observed.dialogueSubroutine=~0u;
    const auto finish=[&](u32 code){observed.dialogueRequestStatus=code;return code;};
    const auto sources=loaded_dialogue_sources(app);
    if(sources.reason)return finish(sources.reason==3?4:sources.reason==4?5:sources.reason==5?6:3);
    if(app->world->actors.gui->dialogue)return finish(7);
    if(!gestures.fire)return finish(11);
    if(entry>=sources.parsed.entries)return finish(2);
    const auto found=std::find_if(sources.parsed.candidates.begin(),sources.parsed.candidates.end(),[&](const auto& row){return row[0]==entry;});
    if(found==sources.parsed.candidates.end())return finish(8);
    observed.dialogueSubroutine=(*found)[1];
    const auto* instruction=reinterpret_cast<const EclInstruction*>(sources.ecl.data+(*found)[4]);
    return finish(app->world->validation_dialogue_request(instruction)?1:10);
}
}
