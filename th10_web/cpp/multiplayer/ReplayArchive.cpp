#include "ReplayArchive.hpp"
#include "CooperativeRules.hpp"
#include <algorithm>
#include <cstring>

namespace th10::multiplayer {
namespace {
constexpr std::size_t BaseDescriptionBytes=120,DescriptionHeaderBytes=128;
constexpr u32 DescriptionVersion=4;
constexpr std::size_t LegacyCheckpointBytes=
    8+16+Netplay::MAX_PLAYERS*sizeof(GameInput)+34+
    Netplay::MAX_PLAYERS*2+4+Netplay::MAX_PLAYERS*sizeof(ReplayStage);
constexpr std::size_t ActivationCheckpointBytes=LegacyCheckpointBytes+16;
constexpr std::size_t RetainedCheckpointBytes=ActivationCheckpointBytes+12;
constexpr std::size_t CheckpointBytes=RetainedCheckpointBytes;
static_assert(sizeof(GameInput)==0x58);
static_assert(sizeof(ReplayStage)==0x1c4);
static_assert(sizeof(Rng)==8);
void put(std::vector<u8>& out,u32 value){for(unsigned n=0;n<4;++n)out.push_back(u8(value>>(n*8)));}
void put16(std::vector<u8>& out,u16 value){out.push_back(u8(value));out.push_back(u8(value>>8));}
u32 word(const u8* p){return u32(p[0])|(u32(p[1])<<8)|(u32(p[2])<<16)|(u32(p[3])<<24);}
u16 half(const u8* p){return u16(p[0])|(u16(p[1])<<8);}
void put_rng(std::vector<u8>& out,const Rng& rng){put16(out,rng.seed);put16(out,rng.reserved);put(out,rng.calls);}
Rng get_rng(const u8*& p){Rng out{half(p),half(p+2),word(p+4)};p+=8;return out;}
void put_input(std::vector<u8>& out,const GameInput& v){
    const u16 prefix[]{v.raw,v.raw_previous,v.raw_repeat,v.raw_pressed,v.raw_released};
    for(auto x:prefix)put16(out,x);for(auto x:v.raw_held_frames)put16(out,x);
    const u16 middle[]{v.focus_hold,v.current,v.previous,v.repeat,v.pressed,v.released,v.reserved_036};
    for(auto x:middle)put16(out,x);for(auto x:v.held_frames)put16(out,x);
}
GameInput get_input(const u8*& p){
    GameInput v{};
    auto take=[&](){const auto x=half(p);p+=2;return x;};
    v.raw=take();v.raw_previous=take();v.raw_repeat=take();v.raw_pressed=take();v.raw_released=take();
    for(auto& x:v.raw_held_frames)x=take();
    v.focus_hold=take();v.current=take();v.previous=take();v.repeat=take();v.pressed=take();v.released=take();v.reserved_036=take();
    for(auto& x:v.held_frames)x=take();return v;
}
void put_cooperation(std::vector<u8>& out,const ReplayCheckpointCooperation& s){
    out.push_back(s.seatCount);out.push_back(s.retryPending?1:0);put16(out,s.wipeTicks);
    for(const auto& seat:s.seats){
        put16(out,u16(seat.lives));put16(out,u16(seat.power));out.push_back(seat.character);out.push_back(seat.shot);
        out.push_back(u8(seat.lifeState));out.push_back(seat.rescueTicks);out.push_back(u8(seat.rescueTarget));
        out.push_back(seat.waitingForFocusRelease?1:0);
    }
}
bool get_cooperation(const u8*& p,u8 players,ReplayCheckpointCooperation& s){
    s={};s.seatCount=*p++;const u8 retry=*p++;s.retryPending=retry!=0;s.wipeTicks=half(p);p+=2;
    if(retry>1||s.seatCount!=players)return false;
    for(auto& seat:s.seats){
        seat.lives=std::int16_t(half(p));p+=2;seat.power=std::int16_t(half(p));p+=2;
        seat.character=*p++;seat.shot=*p++;const u8 life=*p++;seat.lifeState=life;
        seat.rescueTicks=*p++;seat.rescueTarget=std::int8_t(*p++);const u8 waiting=*p++;seat.waitingForFocusRelease=waiting!=0;
        if(waiting>1||seat.character>1||seat.shot>2||seat.lives<-1||seat.lives>kMaxLives||
           seat.power<0||seat.power>kMaxPower||life>u8(LifeState::Eliminated)||
           seat.rescueTarget<-1||seat.rescueTarget>=static_cast<std::int8_t>(players))return false;
    }
    return true;
}
bool checkpoint_valid(const Netplay::InputReplayInfo& tape,const ReplayDescription& description,
                      const ReplayCheckpoint& cp){
    const u32 stage=cp.label&255u;
    if(stage<1||stage>7||cp.firstFrame>=tape.frameCount||
       cp.cooperation.seatCount!=tape.config.playerCount)return false;
    bool chapter=false;
    for(u32 i=0;i<tape.chapterCount;++i){
        if(tape.chapters[i].label!=cp.label||tape.chapters[i].firstFrame>cp.firstFrame)continue;
        const u32 end=i+1<tape.chapterCount?tape.chapters[i+1].firstFrame:tape.frameCount;
        chapter=cp.firstFrame<end;if(chapter)break;
    }
    if(!chapter)return false;
    for(u32 seat=0;seat<tape.config.playerCount;++seat){
        const auto& snap=cp.pilots[seat];
        const auto& coop=cp.cooperation.seats[seat];
        const auto& loadout=description.setup.loadouts[seat];
        const auto extendMax=description.setup.difficulty==4?2:4;
        if(snap.stage!=static_cast<std::int16_t>(stage)||snap.power<0||snap.power>kMaxPower||
           snap.lives<-1||snap.lives>kMaxLives||cp.reservedPower[seat]>kMaxPower||
           snap.score_units<0||snap.score_units>9||snap.extend_index<0||snap.extend_index>extendMax||
           (snap.focused!=0&&snap.focused!=1)||
           coop.character!=loadout.character||coop.shot!=loadout.shot||
           coop.lives!=snap.lives||coop.power!=snap.power)return false;
    }
    if(cp.retainedStateValid&&(cp.faithCursor>=2048||
       (description.setup.difficulty==4?cp.reservedStage!=7:
        cp.reservedStage<1||cp.reservedStage>6)))return false;
    return true;
}
std::vector<u8> encode(const ReplayDescription& info,const ReplayCheckpoint* checkpoints,
                       u32 checkpointCount,u32 frameCount,u32 version){
    u32 included=0;for(u32 i=0;i<checkpointCount;++i)
        if(frameCount==Netplay::INVALID_FRAME||checkpoints[i].firstFrame<frameCount)++included;
    if(version<1||version>DescriptionVersion||(version==1&&included))return {};
    const auto recordBytes=version>=4?CheckpointBytes:
                           version>=3?ActivationCheckpointBytes:LegacyCheckpointBytes;
    std::vector<u8> out;out.reserve((version==1?BaseDescriptionBytes:DescriptionHeaderBytes)+included*recordBytes);
    put(out,version);put(out,info.setup.seed);
    put(out,info.setup.difficulty);put(out,info.setup.difficulty==4?7:1);
    for(const auto& loadout:info.setup.loadouts){put(out,loadout.character);put(out,loadout.shot);}
    out.insert(out.end(),info.name,info.name+12);put(out,u32(info.timestamp));
    put(out,u32(info.score));put(out,u32(info.scoreUnits));put(out,u32(info.lastStage));
    const auto* bytes=reinterpret_cast<const u8*>(&info.configuration);
    // Original title configuration is exactly 52 bytes of integer fields and
    // byte arrays. No pointer, native handle or generated file is included.
    static_assert(sizeof(ApplicationConfig)==52);
    out.insert(out.end(),bytes,bytes+sizeof(ApplicationConfig));
    if(version>=2){put(out,included);put(out,u32(recordBytes));}
    for(u32 i=0;i<checkpointCount;++i){
        const auto& cp=checkpoints[i];if(frameCount!=Netplay::INVALID_FRAME&&cp.firstFrame>=frameCount)continue;
        put(out,cp.label);put(out,cp.firstFrame);put_rng(out,cp.scriptRandom);put_rng(out,cp.visualRandom);
        if(version>=3){put_rng(out,cp.activationScriptRandom);put_rng(out,cp.activationVisualRandom);}
        for(const auto& input:cp.inputSeats)put_input(out,input);
        put_cooperation(out,cp.cooperation);
        for(auto value:cp.reservedPower)put16(out,value);
        put(out,cp.cheatMovementUsed?1u:0u);
        for(const auto& snap:cp.pilots){
            const auto* raw=reinterpret_cast<const u8*>(&snap);
            out.insert(out.end(),raw,raw+sizeof(snap));
        }
        if(version>=4){
            if(!cp.retainedStateValid||cp.faithCursor>=2048)return {};
            put(out,cp.faithCursor);put(out,cp.laserLastId);put(out,cp.reservedStage);
        }
    }
    return out;
}
bool decode(const Netplay::InputReplayInfo& tape,ReplayDescription& out,
            std::array<ReplayCheckpoint,7>* checkpointOut=nullptr,u8* checkpointCountOut=nullptr){
    const auto& config=tape.config;const auto& bytes=config.description;
    if(config.gameId!=10||bytes.size()<BaseDescriptionBytes)return false;
    const auto version=word(bytes.data());
    if((version==1&&bytes.size()!=BaseDescriptionBytes)||version<1||version>DescriptionVersion)return false;
    ReplayDescription next;const auto* p=bytes.data();
    const u32 words[]{1,config.playerCount,config.recordedPlayer,word(p+8),word(p+4),
        word(p+16),word(p+20),word(p+24),word(p+28),word(p+32),word(p+36)};
    if(!DecodeSessionSetup(next.setup,words,11)||
       config.gameplayAbi!=GameplayContract(next.setup)||
       word(p+12)!=(next.setup.difficulty==4?7u:1u))return false;
    if(!std::memchr(p+40,0,12))return false;
    std::memcpy(next.name,p+40,12);std::memcpy(&next.timestamp,p+52,4);
    std::memcpy(&next.score,p+56,4);std::memcpy(&next.scoreUnits,p+60,4);
    std::memcpy(&next.lastStage,p+64,4);std::memcpy(&next.configuration,p+68,52);
    if(next.scoreUnits<0||next.scoreUnits>9||next.lastStage<1||next.lastStage>8||
       !next.configuration.valid(sizeof(ApplicationConfig)))return false;
    u32 previousGeneration=0,previousStage=0;
    for(u32 i=0;i<tape.chapterCount;++i){
        const auto label=tape.chapters[i].label,stage=label&255u,generation=label>>8;
        if(stage<1||stage>8||generation>previousGeneration+1||generation<previousGeneration||
           (!i&&(generation||stage!=word(p+12)))||
           (next.setup.difficulty==4?(stage!=7&&stage!=8):stage>6)||
           (stage==8&&(!i||generation!=previousGeneration||previousStage!=7)))return false;
        previousGeneration=generation;previousStage=stage;
    }
    std::array<ReplayCheckpoint,7> checkpoints{};u8 checkpointCount=0;
    if(version>=2){
        if(bytes.size()<DescriptionHeaderBytes)return false;
        const u32 count=word(p+120),recordBytes=word(p+124);
        const std::size_t expectedRecordBytes=version>=4?CheckpointBytes:
                                              version>=3?ActivationCheckpointBytes:LegacyCheckpointBytes;
        if(count>checkpoints.size()||recordBytes!=expectedRecordBytes||
           bytes.size()!=DescriptionHeaderBytes+std::size_t(count)*expectedRecordBytes)return false;
        const u8* at=p+DescriptionHeaderBytes;
        for(u32 i=0;i<count;++i){
            ReplayCheckpoint cp{};cp.label=word(at);cp.firstFrame=word(at+4);at+=8;
            cp.scriptRandom=get_rng(at);cp.visualRandom=get_rng(at);
            if(version>=3){
                cp.activationScriptRandom=get_rng(at);cp.activationVisualRandom=get_rng(at);
                cp.activationRandomValid=true;
            }else{
                cp.activationScriptRandom=cp.scriptRandom;
                cp.activationVisualRandom=cp.visualRandom;
            }
            for(auto& input:cp.inputSeats)input=get_input(at);
            if(!get_cooperation(at,config.playerCount,cp.cooperation))return false;
            for(auto& value:cp.reservedPower){value=half(at);at+=2;}
            const u32 cheat=word(at);at+=4;if(cheat>1)return false;cp.cheatMovementUsed=cheat!=0;
            for(auto& snap:cp.pilots){std::memcpy(&snap,at,sizeof(snap));at+=sizeof(snap);}
            if(version>=4){
                cp.faithCursor=word(at);at+=4;cp.laserLastId=word(at);at+=4;
                cp.reservedStage=word(at);at+=4;
                cp.retainedStateValid=true;
            }
            if(!checkpoint_valid(tape,next,cp))return false;
            for(u32 prior=0;prior<i;++prior)if((checkpoints[prior].label&255u)==(cp.label&255u))return false;
            checkpoints[i]=cp;++checkpointCount;at=p+DescriptionHeaderBytes+std::size_t(i+1)*expectedRecordBytes;
        }
    }
    if(checkpointOut)*checkpointOut=checkpoints;if(checkpointCountOut)*checkpointCountOut=checkpointCount;
    out=next;return true;
}
}

void ReplayArchive::Clear(){tape_.Clear();description_={};stamps_={};checkpoints_={};save={};base_=next_=generation_=cursor_=0;saved_=Netplay::INVALID_FRAME;checkpoint_count_=0;selected_checkpoint_=0xff;format_version_=DescriptionVersion;}
bool ReplayArchive::Begin(const SessionSetup& setup,const ApplicationConfig& configuration){
    const u32 words[]{1,setup.playerCount,setup.localPlayer,setup.difficulty,setup.seed,
        setup.loadouts[0].character,setup.loadouts[0].shot,setup.loadouts[1].character,
        setup.loadouts[1].shot,setup.loadouts[2].character,setup.loadouts[2].shot};
    ReplayDescription description;
    if(!DecodeSessionSetup(description.setup,words,11)||!configuration.valid(sizeof(configuration)))return false;
    description.configuration=configuration;description.lastStage=setup.difficulty==4?7:1;
    Netplay::InputReplayConfig config;config.gameId=10;config.gameplayAbi=GameplayContract(setup);
    config.playerCount=u8(setup.playerCount);config.recordedPlayer=u8(setup.localPlayer);
    config.description=encode(description,nullptr,0,Netplay::INVALID_FRAME,DescriptionVersion);
    if(!tape_.Begin(config))return false;
    description_=description;stamps_={};checkpoints_={};save={};base_=next_=generation_=cursor_=0;saved_=Netplay::INVALID_FRAME;checkpoint_count_=0;selected_checkpoint_=0xff;format_version_=DescriptionVersion;return true;
}
bool ReplayArchive::Inspect(const u8* bytes,std::size_t size,Netplay::InputReplayInfo& info,ReplayDescription& description){
    Netplay::InputReplayInfo candidate;ReplayDescription metadata;
    if(!Netplay::InputReplay::Inspect(bytes,size,&candidate)||!decode(candidate,metadata))return false;
    info=std::move(candidate);description=metadata;return true;
}
bool ReplayArchive::Load(const u8* bytes,std::size_t size){
    Netplay::InputReplayInfo info;ReplayDescription description;std::array<ReplayCheckpoint,7> checkpoints{};u8 count=0;
    if(!Netplay::InputReplay::Inspect(bytes,size,&info)||!decode(info,description,&checkpoints,&count)||
       !tape_.Decode(bytes,size))return false;
    description_=description;stamps_={};checkpoints_=checkpoints;save={};base_=next_=generation_=cursor_=0;saved_=Netplay::INVALID_FRAME;checkpoint_count_=count;selected_checkpoint_=0xff;format_version_=word(info.config.description.data());return true;
}
bool ReplayArchive::Stamp(u32 frame,u32 stage){
    if(!Recording())return Playing();
    if(frame==Netplay::INVALID_FRAME||frame<next_||stage<1||stage>8||
       (description_.setup.difficulty==4?(stage!=7&&stage!=8):stage>6)||
       generation_>0x00ffffffu)return false;
    auto& entry=stamps_[frame%stamps_.size()];
    if(entry.frame!=Netplay::INVALID_FRAME&&entry.frame!=frame&&entry.frame>=next_)return false;
    entry={frame,(generation_<<8)|stage};return true;
}
bool ReplayArchive::Commit(const NetplayRuntime& runtime){
    if(!Recording())return Playing();
    const auto confirmed=runtime.ConfirmedThroughAllRemotes(),last=runtime.LastSimulatedFrame();
    if(confirmed==Netplay::INVALID_FRAME||last==Netplay::INVALID_FRAME)return true;
    const u32 through=std::min(confirmed,last);
    while(next_<=through){
        const auto& entry=stamps_[next_%stamps_.size()];Netplay::InputReplay::Frame inputs;
        if(entry.frame!=next_||!runtime.ConfirmedInputs(next_,&inputs)||
           !tape_.Append(base_+next_,entry.label,inputs.data(),runtime.Config().playerCount))return false;
        ++next_;cursor_=base_+next_;
    }
    return true;
}
bool ReplayArchive::NextGeneration(u32 generation){
    if(generation!=generation_+1||generation>0x00ffffffu)return false;
    base_=cursor_;next_=0;generation_=generation;stamps_={};selected_checkpoint_=0xff;return true;
}
bool ReplayArchive::CaptureCheckpoint(const ReplayCheckpoint& checkpoint){
    if(!Recording())return Playing();
    const u32 stage=checkpoint.label&255u;
    if(stage<1||stage>7||checkpoint.label!=((generation_<<8)|stage)||
       checkpoint.firstFrame>=cursor_||!checkpoint.activationRandomValid||
       !checkpoint.retainedStateValid||!checkpoint_valid(tape_.Info(),description_,checkpoint))return false;
    for(u32 i=0;i<checkpoint_count_;++i)
        if((checkpoints_[i].label&255u)==stage)return true;
    if(checkpoint_count_>=checkpoints_.size())return false;
    checkpoints_[checkpoint_count_++]=checkpoint;return true;
}
bool ReplayArchive::SelectCheckpoint(u32 stage){
    if(!Playing()||stage<1||stage>7)return false;
    for(u32 i=0;i<checkpoint_count_;++i){
        const auto& cp=checkpoints_[i];
        // Direct bootstrap currently starts a fresh offline Runtime generation.
        // A checkpoint whose first occurrence exists only after Retry keeps the
        // proven frame-zero reconstruction fallback until generation bootstrap
        // is made independently portable.
        if((cp.label&255u)==stage&&(cp.label>>8)==0){
            if(!cp.activationRandomValid||!cp.retainedStateValid||
               !checkpoint_valid(tape_.Info(),description_,cp))return false;
            base_=cursor_=cp.firstFrame;next_=0;generation_=0;selected_checkpoint_=u8(i);return true;
        }
    }
    return false;
}
const Netplay::InputReplay::Frame* ReplayArchive::PlaybackFrame(u32 frame,u32 stage)const{
    if(!Playing()||base_>Frames()||frame==Netplay::INVALID_FRAME||frame>=Frames()-base_||
       base_+frame!=cursor_)return nullptr;
    const auto& info=tape_.Info();u32 chapter=0;
    while(chapter+1<info.chapterCount&&info.chapters[chapter+1].firstFrame<=cursor_)++chapter;
    if(info.chapters[chapter].label!=((generation_<<8)|stage))return nullptr;
    return tape_.FrameAt(cursor_);
}
bool ReplayArchive::Played(u32 frame){
    if(!Playing()||base_+frame!=cursor_||cursor_>=Frames())return false;
    ++cursor_;return true;
}
bool ReplayArchive::Encode(std::vector<u8>& out,const ReplayDescription* description,u32 frameCount)const{
    if(format_version_<1||format_version_>DescriptionVersion||
       (format_version_==1&&checkpoint_count_))return false;
    const auto bytes=encode(description?*description:description_,checkpoints_.data(),
                            checkpoint_count_,frameCount,format_version_);
    if(bytes.empty())return false;
    return tape_.Encode(&out,&bytes,frameCount);
}
u32 ReplayArchive::SeekFrame(u32 stage)const{
    // Native Extra Results uses 8 as a terminal marker, not a playable stage.
    if(stage<1||stage>7)return Netplay::INVALID_FRAME;
    for(u32 i=0;i<checkpoint_count_;++i){
        const auto& cp=checkpoints_[i];
        if((cp.label&255u)==stage&&(cp.label>>8)==0&&cp.activationRandomValid&&
           cp.retainedStateValid&&checkpoint_valid(tape_.Info(),description_,cp))
            return cp.firstFrame;
    }
    for(u32 i=0;i<tape_.Info().chapterCount;++i)
        if((tape_.Info().chapters[i].label&255u)==stage)return tape_.Info().chapters[i].firstFrame;
    return Netplay::INVALID_FRAME;
}
const ReplayCheckpoint* ReplayArchive::Checkpoint(u32 stage)const{
    if(stage<1||stage>7)return nullptr;
    for(u32 i=0;i<checkpoint_count_;++i)
        if((checkpoints_[i].label&255u)==stage)return &checkpoints_[i];
    return nullptr;
}
bool ReplayArchive::SafePath(const char* path){
    if(!path)return false;const auto size=std::strlen(path);
    if(size<12||size>=256||std::strncmp(path,"replay/",7)||std::strcmp(path+size-4,".rpy"))return false;
    for(std::size_t i=7;i<size;++i){const char c=path[i];if(c=='/'||c=='\\'||c==':'||u8(c)<32)return false;}
    return std::strstr(path,"..")==nullptr;
}
bool ReplayArchive::RequestSave(const char* file,const char* name,u32 frame,i32 score,i32 units,i32 stage){
    if(!SafePath(file)||!name||frame==Netplay::INVALID_FRAME||units<0||units>9||stage<1||stage>8)return false;
    SaveRequest next;std::strcpy(next.file,file);std::strncpy(next.name,name,11);
    next.frame=frame;next.score=score;next.scoreUnits=units;next.lastStage=stage;next.pending=true;
    save=next;return true;
}
}
