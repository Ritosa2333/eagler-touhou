#pragma once
#include "NativeFreshReplayRecord.hpp"
#include "NativeOrdinaryReturn.hpp"
#include "NativeExReturn.hpp"
#include "OriginalScoreNameGlyphs.hpp"
#include "NativeOrdinaryReplaySave.hpp"
#include "OriginalOrdinaryReplayGlyphs.hpp"
#include "NativeNormalRecord.hpp"
#include "NativeRunReplay.hpp"
#include "NativeScoreProfile.hpp"
#include "NativeRecordTiming.hpp"
#include "animation/AnmScene.hpp"
#include <array>
#include <cstring>
#include <limits>
#include <memory>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace th128::postrun {

struct Input {std::uint32_t held=0,pressed=0;};
struct MusicCommand {unsigned serial=0,epoch=1;int operation=0,track=0,argument=0;};
struct Context {
    std::vector<std::uint8_t> profile;
    std::shared_ptr<record::NativeFreshReplayRecord> replay;
    std::shared_ptr<record::NativeNormalRecord> normalRecord;
    std::shared_ptr<record::NativeRunReplay> normalReplay;
    std::shared_ptr<record::NativeRunReplay> extraReplay;
    std::shared_ptr<const record::NativeRunReplay> practiceReplay;
    std::shared_ptr<record::NativePostrunRecord> extraRecord;
    std::function<void(int,int,int)> musicSink;
    std::function<void(int)> soundSink;
    std::int32_t score=0;
    unsigned stage=0,route=0,rank=0,continues=0,epoch=1,clockSerial=0;
    double numerator=0,denominator=0;
    bool ordinaryDiagnostic=false;
};

class NativeNormalMainOwner;
class Menu {
    friend class NativeNormalMainOwner;
    struct Admission {Context context;bool sealed=false,extra=false;unsigned practiceFrames=0;};
    static Admission admit(Context context,std::unique_ptr<record::NativeOrdinaryReturn> permit){
        if(!permit||!permit->ready()||context.normalRecord!=permit->record_||context.normalReplay!=permit->replay_
            ||context.replay||context.ordinaryDiagnostic||context.epoch!=permit->identity_.generation
            ||context.route!=permit->identity_.route||context.rank!=permit->identity_.rank||context.stage!=permit->identity_.settledSelector
            ||context.score!=permit->score_||context.continues!=permit->continues_||context.profile!=permit->profile_
            ||context.numerator!=permit->numerator_||context.denominator!=permit->denominator_)
            throw std::runtime_error("ordinary Menu requires its exact sealed actual Ending16 return owner");
        permit->consumed_=true;return {std::move(context),true};
    }
    static Admission admitExtra(Context context,std::unique_ptr<record::NativeExReturn> permit){
        if(!permit||!permit->ready()||context.normalRecord||context.normalReplay||context.ordinaryDiagnostic
            ||context.extraRecord!=permit->record_||context.extraReplay!=permit->replay_||context.replay!=permit->envelope_
            ||context.epoch!=permit->identity_.generation||context.route!=6||context.rank!=4||context.stage!=23||context.continues
            ||context.score!=permit->score_||context.profile!=permit->profile_||context.numerator!=permit->numerator_||context.denominator!=permit->denominator_)
            throw std::runtime_error("EX Menu requires its exact sealed actual terminal23 return owner");
        permit->consumed_=true;return{std::move(context),false,true};
    }
    Context context_;
    bool normalSealed_=false,extraSealed_=false,nameFontReady_=false;
    unsigned practiceFrames_=0;
    unsigned extraNativeFrames_=0;
    std::unique_ptr<anm::Scene> sceneOwner_=std::make_unique<anm::Scene>();
    anm::Scene& scene_=*sceneOwner_;
    std::unique_ptr<anm::Scene> releaseMainScene(){
        if(!mainHandoffReady()||!sceneOwner_||executing_)throw std::runtime_error("ordinary Main scene transfer boundary rejected");
        return std::move(sceneOwner_);
    }
    std::array<bool,3> loaded_{};
    std::array<std::uint8_t,9> name_{};
    std::array<std::uint8_t,292> finishedReplay_{};
    std::vector<std::uint8_t> finishedNormalReplay_;
    NativeOrdinaryReplaySave replaySave_;
    std::array<record::NativeReplaySlotPreview,25> replayPreviews_{};
    std::array<std::uint8_t,9> replayName_{};
    unsigned replaySlot_=0,replaySelection_=0,replayLength_=0,replaySnapshots_=0,nameConfirmationSerial_=0;
    bool replayConfirmed_=false;
    std::vector<MusicCommand> music_;
    std::vector<int> sounds_;
    std::string failure_;
    unsigned phase_=0,subphase_=0,timer_=0,ticks_=0,nameLength_=0,finishSerial_=0;
    int selected_=0,choiceCount_=999,position_=-1;
    bool previewsKnown_=false,executing_=false,mainReady_=false,title0_=false,titleV0_=false;

    static constexpr std::array<std::uint8_t,9> DefaultName{32,32,32,32,32,32,32,32,0};
    static constexpr char Characters[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*";

    void checked() {
        if(!failure_.empty())throw std::runtime_error(failure_);
        for(const auto& [identity,node]:scene_.nodes())if(node->vm.faulted()){
            failure_="original postrun ANM fault script="+std::to_string(node->vm.state().script);
            throw std::runtime_error(failure_);
        }
    }
    void bind(int bank,int script) {
        const auto key=bank==24&&script==97?1ull:(std::uint64_t(unsigned(bank))<<32)|unsigned(script);
        if(!scene_.bind(key,1,bank,script,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23)){
            checked();throw std::runtime_error("original menu ANM constructor rejected bank/script");
        }
        checked();
    }
    void interrupt(int bank,int script,int code) {
        const auto key=bank==24&&script==97?1ull:(std::uint64_t(unsigned(bank))<<32)|unsigned(script);
        scene_.interrupt(key,code);checked();
    }
    void setSubphase(unsigned value){subphase_=value;timer_=0;}
    void setPhase(unsigned value){phase_=value;subphase_=0;timer_=0;}
    void music(int operation,int track,int argument=0){
        if(operation==2&&!practiceSealed()){
            if(argument<0||argument>15)throw std::runtime_error("unbound native music-room flag index");
            context_.profile[record::NativeScoreProfile::StatsOffset+0x26+unsigned(argument)]=1;
        }
        if(context_.musicSink)context_.musicSink(operation,track,argument);
        music_.push_back({unsigned(music_.size()+1),context_.epoch,operation,track,argument});
    }
    void sound(int identifier){
        if(context_.soundSink)context_.soundSink(identifier);
        sounds_.push_back(identifier);
    }
    void move(int delta) {
        if(choiceCount_<=0)return;
        selected_+=delta;
        while(selected_>=choiceCount_)selected_-=choiceCount_;
        while(selected_<0)selected_+=choiceCount_;
    }
    void beginName(std::uint64_t timestamp){
        if(!scene_.find(24))bind(0,24);
        bind(24,111);bind(24,132+int(context_.route));bind(24,141+int(context_.rank));
        bind(24,97);interrupt(24,97,3);
        const record::NativeScoreRowContext row{context_.route,context_.rank,context_.score,
            context_.stage,context_.continues,timestamp,context_.numerator,context_.denominator,DefaultName};
        position_=context_.normalRecord&&!context_.normalRecord->scoreEligible()?-1:record::NativeScoreProfile::insert(context_.profile,row);
        if(context_.normalRecord)context_.stage=0;
        name_.fill(0);std::copy_n(context_.profile.begin()+record::NativeScoreProfile::StatsOffset+12,9,name_.begin());
        if(name_.back()!=0)throw std::runtime_error("native last-name field has no terminator");
        selected_=choiceCount_>0?0:choiceCount_-1;choiceCount_=91;
        if(name_!=DefaultName)move(-1);
        nameLength_=8;while(nameLength_&&name_[nameLength_-1]==32)--nameLength_;
        setSubphase(1);
    }
    void nameInput(Input input){
        const auto keys=input.held|input.pressed;
        if(position_>=0){
            const auto previous=selected_;
            if(keys&0x10)move(-13);
            if(keys&0x20)move(13);
            if(keys&0x40)move(selected_%13? -1:12);
            if(keys&0x80)move(selected_%13==12? -12:1);
            if(previous!=selected_)sound(10);
        }
        if(input.pressed&0x80201u){
            if(position_<0)setSubphase(3);
            else if(selected_<88){
                name_[std::min(nameLength_,7u)]=std::uint8_t(Characters[selected_]);
                if(nameLength_<8)++nameLength_;
                if(nameLength_==8)selected_=90;
            }else if(selected_==88){
                name_[std::min(nameLength_,7u)]=32;
                if(nameLength_<8)++nameLength_;
                if(nameLength_==8)selected_=90;
            }else if(selected_==89){
                if(!nameLength_)return;
                name_[--nameLength_]=32;
            }else if(selected_==90){
                if(nameConfirmationSerial_==UINT32_MAX)throw std::runtime_error("ordinary name confirmation serial exhausted");++nameConfirmationSerial_;
                name_[8]=0;
                const auto offset=std::size_t(context_.route)*1644+16+context_.rank*320+unsigned(position_)*32+6;
                std::copy(name_.begin(),name_.end(),context_.profile.begin()+offset);
                std::copy(name_.begin(),name_.end(),context_.profile.begin()+record::NativeScoreProfile::StatsOffset+12);
                setSubphase(3);
            }else throw std::runtime_error("unowned native name selection");
            sound(7);
        }
        if(input.pressed&0x102u){
            if(position_<0){setSubphase(3);sound(7);}
            else if(nameLength_){name_[--nameLength_]=32;sound(9);}
        }
    }
    void beginReplay(std::uint64_t timestamp,unsigned serial){
        if(!replayPresent()||!previewsKnown_)throw std::runtime_error("real replay or owned preview observations unavailable");
        // Practice retains its input until the player chooses a slot.
        if(context_.practiceReplay){bind(24,112);selected_=0;choiceCount_=25;setSubphase(1);return;}
        if(context_.extraReplay){
            std::array<std::uint8_t,8> name{};std::copy_n(name_.begin(),8,name.begin());
            finishedNormalReplay_=context_.extraReplay->finishExtra(timestamp,context_.score,name,context_.numerator,context_.denominator);
        }else if(context_.normalReplay){
            if(context_.continues)throw std::runtime_error("continued ordinary run cannot enter replay finish");
            std::array<std::uint8_t,8> name{};std::copy_n(name_.begin(),8,name.begin());
            finishedNormalReplay_=context_.extraReplay?context_.extraReplay->finishExtra(timestamp,context_.score,name,context_.numerator,context_.denominator):context_.normalReplay->finishOrdinary(timestamp,context_.score,name,context_.numerator,context_.denominator);
        }else{
            context_.replay->finish(timestamp,context_.score,context_.numerator,context_.denominator,true,0);
            finishedReplay_=context_.replay->bytes();
        }
        finishSerial_=serial;++replaySnapshots_;
        bind(24,112);selected_=0;choiceCount_=25;setSubphase(1);
    }
    void nameTick(Input input,std::uint64_t timestamp){
        if(subphase_==4){
            if(input.pressed&0x80303u){interrupt(24,211,1);beginName(timestamp);}
        }else if(subphase_==1){
            if(timer_>6)setSubphase(2);
        }else if(subphase_==2)nameInput(input);
        else if(subphase_==3&&timer_>=6){
            interrupt(24,111,1);interrupt(24,132+int(context_.route),1);interrupt(24,141+int(context_.rank),1);
            if(context_.normalRecord&&(context_.continues||(context_.normalReplay&&context_.normalReplay->usedPracticeTools()))){
                setPhase(1);selected_=0;choiceCount_=999;context_.normalReplay.reset();
                music(1,7);music(2,7);
            }else setPhase(15);
        }else if(subphase_!=3)throw std::runtime_error("unowned name phase");
    }
    void beginReplayName(std::uint64_t timestamp,unsigned serial){
        replaySlot_=unsigned(selected_);replaySelection_=0;replayName_.fill(0);
        std::copy_n(context_.profile.begin()+record::NativeScoreProfile::StatsOffset+12,9,replayName_.begin());
        if(replayName_.back()!=0)throw std::runtime_error("ordinary replay ST+12 name has no terminator");
        replayLength_=8;while(replayLength_&&replayName_[replayLength_-1]==32)--replayLength_;
        if(replayName_!=DefaultName)replaySelection_=90;
        std::array<std::uint8_t,8> name{};std::copy_n(replayName_.begin(),8,name.begin());
        if(context_.practiceReplay)finishedNormalReplay_=context_.practiceReplay->snapshot(timestamp,context_.score,context_.practiceReplay->stage(),0,name);
        else finishedNormalReplay_=context_.extraReplay?context_.extraReplay->finishExtra(timestamp,context_.score,name,context_.numerator,context_.denominator):context_.normalReplay->finishOrdinary(timestamp,context_.score,name,context_.numerator,context_.denominator);
        finishSerial_=serial;++replaySnapshots_;setSubphase(3);sound(7);
    }
    void replayNameInput(Input input,unsigned serial){
        const auto keys=input.held|input.pressed;const auto before=replaySelection_;auto moveCharacter=[&](int delta){replaySelection_=unsigned((int(replaySelection_)+delta+91)%91);};
        if(keys&0x10)moveCharacter(-13);if(keys&0x20)moveCharacter(13);
        if(keys&0x40)moveCharacter(replaySelection_%13?-1:12);if(keys&0x80)moveCharacter(replaySelection_%13==12?-12:1);
        if(before!=replaySelection_)sound(10);
        if(input.pressed&0x80201u){
            if(replaySelection_<88||replaySelection_==88){replayName_[std::min(replayLength_,7u)]=replaySelection_==88?32:std::uint8_t(Characters[replaySelection_]);if(replayLength_<8)++replayLength_;if(replayLength_==8)replaySelection_=90;}
            else if(replaySelection_==89){if(!replayLength_)return;replayName_[--replayLength_]=32;}
            else{
                replayName_[8]=0;
                if(!practiceSealed()){
                    if(nameConfirmationSerial_==UINT32_MAX)throw std::runtime_error("ordinary replay name confirmation serial exhausted");++nameConfirmationSerial_;
                    std::copy(replayName_.begin(),replayName_.end(),context_.profile.begin()+record::NativeScoreProfile::StatsOffset+12);
                }
                std::copy_n(replayName_.begin(),8,finishedNormalReplay_.begin()+36);
                replaySave_.prepare(context_.epoch,ticks_+1,replaySlot_,context_.route,context_.rank,unsigned(context_.score),normalFrames(),finishSerial_,finishedNormalReplay_,
                    context_.practiceReplay?context_.practiceReplay->stage():0);
                replayConfirmed_=true;sound(18);
            }
            sound(7);
        }
        if(!replaySave_.pending()&&(input.pressed&0x102u)){if(!replayLength_)setSubphase(2);else{replayName_[--replayLength_]=32;sound(9);}}
    }
    void replayTick(Input input,std::uint64_t timestamp,unsigned serial){
        if(subphase_==0)beginReplay(timestamp,serial);
        else if(subphase_==1){if(timer_>6)setSubphase(2);}
        else if(subphase_==2){
            const auto keys=input.held|input.pressed;const auto previous=selected_;
            if(keys&0x10)move(-1);if(keys&0x20)move(1);if(previous!=selected_)sound(10);
            if(input.pressed&0x102u){setSubphase(4);sound(9);}
            else if(input.pressed&0x80201u){
                if(!sealedPostrun()||!replaySave_.bound()||(!context_.normalReplay&&!context_.extraReplay&&!context_.practiceReplay))throw std::runtime_error("replay save branch requires its real durable replay writer");
                beginReplayName(timestamp,serial);
            }
        }else if(subphase_==3)replayNameInput(input,serial);
        else if(subphase_==4&&timer_>=6){
            interrupt(24,112,1);setPhase(1);selected_=0;choiceCount_=999;
            context_.replay.reset();context_.normalReplay.reset();context_.extraReplay.reset();context_.practiceReplay.reset();finishedNormalReplay_.clear();replaySave_={};replayPreviews_={};previewsKnown_=false;
            if(!practiceSealed()){music(1,7);music(2,7);}
        }else if(subphase_!=4)throw std::runtime_error("unowned replay phase");
    }
    void mainTick(Input input){
        if(subphase_==0){
            selected_=0;choiceCount_=7;setSubphase(1);
            if(!scene_.find((std::uint64_t(24)<<32)|97)){bind(24,97);interrupt(24,97,3);}
        }else if(subphase_==1){
            if(timer_==60){bind(24,0);bind(25,0);title0_=titleV0_=true;}
            if(timer_>70){setSubphase(2);mainReady_=true;}
        }else if(subphase_==2){
            if(input.held||input.pressed)throw std::runtime_error("main selection requires its original next-scene owner");
        }else throw std::runtime_error("unowned main phase");
    }

public:
    explicit Menu(Context context):Menu(Admission{std::move(context),false}){}
    Menu(Context context,std::unique_ptr<record::NativeOrdinaryReturn> permit):Menu(admit(std::move(context),std::move(permit))){}
    Menu(Context context,std::unique_ptr<record::NativeExReturn> permit):Menu(admitExtra(std::move(context),std::move(permit))){}
    static std::unique_ptr<Menu> practiceName(Context context,std::shared_ptr<const record::NativeRunReplay> replay){
        static constexpr std::array<unsigned,6> terminalStages{3,5,8,10,13,15};
        if(!replay||!replay->frames()||context.replay||context.normalReplay||context.normalRecord||context.extraRecord||context.extraReplay||context.practiceReplay
            ||context.ordinaryDiagnostic||context.route>=6||context.rank>=4||context.stage!=17+context.route
            ||context.score<0||context.score>999999999||context.continues>9||replay->route()!=context.route||replay->rank()!=context.rank||replay->stage()!=terminalStages[context.route])
            throw std::runtime_error("practice name screen requires a completed live route and its own score");
        record::NativeScoreProfile::assemble(context.profile);
        const auto frames=replay->frames();context.practiceReplay=std::move(replay);
        return std::unique_ptr<Menu>(new Menu(Admission{std::move(context),false,false,frames}));
    }
private:
    explicit Menu(Admission admission):context_(std::move(admission.context)),normalSealed_(admission.sealed),extraSealed_(admission.extra),practiceFrames_(admission.practiceFrames){
        static constexpr std::array<unsigned,6> terminalStages{3,5,8,10,13,15};
        if(context_.practiceReplay&&!admission.practiceFrames)throw std::runtime_error("practice replay has no live Ending16 owner");
        if((context_.extraReplay||context_.extraRecord)&&!admission.extra)throw std::runtime_error("EX Context has no sealed native terminal return credential");
        const bool ordinary=bool(context_.normalRecord)||bool(context_.normalReplay);
        if(admission.extra&&(!context_.extraRecord||!context_.extraRecord->acknowledged()||!context_.extraReplay
            ||context_.extraReplay->stages()!=1||context_.extraReplay->route()!=6||context_.extraReplay->rank()!=4||!context_.extraReplay->configuredFor(16)
            ||context_.replay->input().frame()!=int(context_.extraReplay->frames())||context_.profile!=context_.extraRecord->profile()))
            throw std::runtime_error("sealed EX record/history profile differs");
        if(ordinary){
            if(!admission.sealed)throw std::runtime_error("ordinary Context has no sealed original Ending16 return credential");
            const auto& r=context_.normalRecord;const auto& replay=context_.normalReplay;
            if(!r||!replay||context_.replay||context_.ordinaryDiagnostic||context_.route>=6||context_.rank>=4
                ||context_.stage!=context_.route+17||context_.continues>9||!context_.epoch
                ||!r->started()||!r->prepared()||!r->acknowledged()||!r->terminalStatsPrepared()
                ||r->route()!=context_.route||r->rank()!=context_.rank||r->stage()!=terminalStages[context_.route]
                ||r->terminalScore()!=context_.score||r->terminalContinues()!=context_.continues||context_.profile!=r->pending()
                ||!replay->configuredFor(r->stage())||replay->stages()!=3||replay->route()!=context_.route
                ||replay->rank()!=context_.rank||replay->frames()!=r->frames()
                ||context_.numerator!=r->statistics().numerator||context_.denominator!=r->statistics().denominator)
                throw std::runtime_error("ordinary postrun menu differs from its acknowledged terminal record/replay");
        }
        const bool domain=context_.ordinaryDiagnostic
            ?context_.route<6&&context_.rank<4&&context_.stage==context_.route+17&&context_.replay
                &&context_.replay->stage()==terminalStages[context_.route]
                &&context_.replay->physicalRoute()==context_.route
            :context_.route==6&&context_.rank==4&&context_.stage==23;
        if(context_.profile.size()!=52232||(!ordinary&&!practiceSealed()&&(!context_.replay||!domain||context_.continues!=0))||!context_.epoch
            ||!std::isfinite(context_.numerator)||context_.numerator<0
            ||!std::isfinite(context_.denominator)||context_.denominator<=0)
            throw std::runtime_error("original postrun menu context outside owned EX domain");
        if(admission.extra)extraNativeFrames_=context_.extraReplay->frames();
        static_assert(sizeof(Characters)-1==88);
        scene_.runtime.instructionBudget=4096;
        scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
        scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
        scene_.runtime.gameRandom32=[this]()->std::uint32_t{
            failure_="original menu requested unbound actual game RNG";throw std::runtime_error(failure_);
        };
        scene_.runtime.displayRandom32=[this]()->std::uint32_t{
            failure_="original menu requested unbound actual display RNG";throw std::runtime_error(failure_);
        };
        scene_.runtime.unsupported=[this](int bank,int script,int opcode,const char* reason){
            if(failure_.empty())failure_="menu bank="+std::to_string(bank)+" script="+std::to_string(script)
                +" opcode="+std::to_string(opcode)+": "+reason;
        };
    }
public:
    void loadNameFont(std::span<const std::uint8_t> bytes){
        if(ticks_||executing_||nameFontReady_)throw std::runtime_error("original name font requires the fresh sealed menu resource boundary");
        std::string error;if(!scene_.registry.loadBank(11,bytes.data(),bytes.size(),error))throw std::runtime_error(error);
        const auto bank=scene_.registry.bank(11);if(!bank||bank->sprites.size()<98)throw std::runtime_error("original ASCII style0 name glyph bank unavailable");nameFontReady_=true;
    }
    void load(int bank,std::span<const std::uint8_t> bytes){
        if(executing_||ticks_||bank!=0&&bank!=24&&bank!=25)throw std::runtime_error("menu resource load lifecycle rejected");
        const unsigned index=bank==0?0:bank==24?1:2;
        if(loaded_[index])throw std::runtime_error("menu bank replacement rejected");
        std::string error;
        if(!scene_.registry.loadBank(bank,bytes.data(),bytes.size(),error))throw std::runtime_error(error);
        loaded_[index]=true;
    }
    void bindObservedMissingPreviews(std::span<const std::uint8_t> observed){
        if(previewsKnown_||ticks_)throw std::runtime_error("owned replay preview observation lifecycle rejected");
        if(observed.size()==3412){replayPreviews_=record::NativeReplaySlotPreview::decode(observed);previewsKnown_=true;return;}
        if(sealedPostrun()||observed.size()!=25||!std::all_of(observed.begin(),observed.end(),[](auto value){return value==1;}))
            throw std::runtime_error("owned actual missing-preview observation rejected");
        previewsKnown_=true;
    }
    void bindOrdinaryReplayObservations(std::span<const std::uint8_t> packet){
        if(!sealedPostrun()||(!practiceSealed()&&context_.continues)||phase_!=15||subphase_!=0||replaySave_.bound()||executing_)
            throw std::runtime_error("ordinary replay observations require the exact phase15 entry owner");
        replaySave_.bind(context_.epoch,ticks_,packet);replayPreviews_=record::NativeReplaySlotPreview::decode(replaySave_.previews());previewsKnown_=true;
    }
    std::span<const std::uint8_t> ordinaryReplaySavePacket()const{return replaySave_.packet();}
    void acknowledgeOrdinaryReplaySave(std::span<const std::uint8_t> packet){
        if(!sealedPostrun()||phase_!=15||subphase_!=3||!replayConfirmed_||executing_)throw std::runtime_error("ordinary replay ACK has no active original save selection");
        replaySave_.acknowledge(context_.epoch,ticks_,packet);replayPreviews_=record::NativeReplaySlotPreview::decode(replaySave_.previews());
        replayConfirmed_=false;selected_=int(replaySlot_);choiceCount_=25;setSubphase(2);
    }
    void tick(Input input,unsigned serial,std::uint64_t filetime,unsigned epoch){
        if(executing_||replaySave_.pending()||!failure_.empty()||!std::all_of(loaded_.begin(),loaded_.end(),[](bool value){return value;})
            ||epoch!=context_.epoch||serial!=context_.clockSerial+1)
            throw std::runtime_error("original menu tick ownership/clock order rejected");
        const auto timestamp=record::NativeRecordTiming::time64FromFileTime(filetime);
        if(timestamp==UINT64_MAX)throw std::runtime_error("original menu UTC sample rejected");
        executing_=true;
        try{
            if(phase_==0){
                music(1,8);music(2,8,9);bind(24,211);setPhase(14);setSubphase(4);
                if(input.pressed&0x80303u){interrupt(24,211,1);beginName(timestamp);}
            }else if(phase_==14)nameTick(input,timestamp);
            else if(phase_==15)replayTick(input,timestamp,serial);
            else if(phase_==1)mainTick(input);
            else throw std::runtime_error("unowned original menu phase");
            scene_.tickPrimary();scene_.tickSecondary();scene_.collect();checked();
            ++timer_;++ticks_;context_.clockSerial=serial;executing_=false;
        }catch(const std::exception& error){
            executing_=false;if(failure_.empty())failure_=error.what();throw;
        }
    }
    std::string snapshot() const {
        unsigned drawable=0;
        for(const auto& [identity,node]:scene_.nodes()){
            const auto& state=node->vm.state();
            if(node->alive&&!state.ended&&!state.deleteRequested&&state.sprite>=0)++drawable;
        }
        std::ostringstream output;output<<std::boolalpha;
        output<<"{\"phase\":"<<phase_<<",\"subphase\":"<<subphase_<<",\"timer\":"<<timer_
            <<",\"ticks\":"<<ticks_<<",\"selected\":"<<selected_<<",\"choiceCount\":"<<choiceCount_
            <<",\"position\":"<<position_<<",\"award211\":"<<(phase_==14&&subphase_==4)
            <<",\"replayPresent\":"<<replayPresent()<<",\"replayFinished\":"<<bool(finishSerial_)
            <<",\"finishSerial\":"<<finishSerial_<<",\"failed\":"<<!failure_.empty()
            <<",\"mainHandoffReady\":"<<mainHandoffReady()<<",\"mainReady\":"<<mainReady_<<",\"title0\":"<<title0_<<",\"titleV0\":"<<titleV0_
            <<",\"nodeCount\":"<<scene_.nodes().size()<<",\"drawNodes\":"<<drawable<<",\"musicCommands\":[";
        for(unsigned index=0;index<music_.size();++index){
            if(index)output<<',';const auto& command=music_[index];
            output<<"{\"serial\":"<<command.serial<<",\"epoch\":"<<command.epoch<<",\"operation\":"<<command.operation
                <<",\"track\":"<<command.track<<",\"argument\":"<<command.argument<<'}';
        }
        output<<"],\"soundRequests\":[";
        for(unsigned index=0;index<sounds_.size();++index){if(index)output<<',';output<<sounds_[index];}
        output<<"],\"mediaConsumed\":false,\"nativeACK\":0,\"nameGlyphRendererBound\":"<<nameFontReady_<<",\"replayWriterBound\":"<<replaySave_.bound()<<",\"replaySavePending\":"<<replaySave_.pending()<<",\"replaySnapshots\":"<<replaySnapshots_<<"}";
        return output.str();
    }
    void observePresentationClock(unsigned epoch,unsigned serial){
        if(executing_||epoch!=context_.epoch||serial!=context_.clockSerial+1)
            throw std::runtime_error("actual menu post-presentation clock identity rejected");
        context_.clockSerial=serial;
    }
    template<class Writer>bool drawNative(Writer writer)const{
        for(const auto membership:{anm::Membership::Primary,anm::Membership::Secondary})for(const auto* node:scene_.chain(membership)){
            if(!node||!node->alive)continue;const auto& state=node->vm.state();
            if(!state.visible||state.ended||state.deleteRequested||state.sprite<0||!(state.primary>>24))continue;
            const int bank=state.spriteBank<0?state.bank:state.spriteBank;const auto resource=scene_.registry.bank(bank);
            if(!resource||std::size_t(state.sprite)>=resource->sprites.size())return false;
            const auto& sprite=resource->sprites[std::size_t(state.sprite)];
            if(!writer(node->id,bank,state,sprite,state.x+state.engineX+state.offsetX,state.y+state.engineY+state.offsetY,state.z+state.engineZ+state.offsetZ))return false;
        }
        auto glyphs=normalNameGlyphs();if(nameFontReady_&&phase_==15){const auto replayGlyphs=ordinaryReplayGlyphs(subphase_,timer_,unsigned(selected_),replaySlot_,replayPreviews_,finishedNormalReplay_,replayName_,replayLength_,replaySelection_,Characters);glyphs.insert(glyphs.end(),replayGlyphs.begin(),replayGlyphs.end());}
        unsigned glyphOrder=0;if(nameFontReady_)for(const auto& glyph:glyphs){ 
            const auto bank=scene_.registry.bank(11);if(!bank||glyph.sprite>=bank->sprites.size())return false;
            const auto& sprite=bank->sprites[glyph.sprite];th128::anm::State state{};state.bank=state.spriteBank=11;state.sprite=int(glyph.sprite);
            state.layer=31;state.mode=1;state.flags=2;state.anchorX=state.anchorY=1;state.primary=state.secondary=glyph.color;
            state.sx=state.sy=state.uvScaleX=state.uvScaleY=1;
            if(!writer(0x100000u+glyphOrder++,11,state,sprite,glyph.x,glyph.y,0))return false;
        }
        return true;
    }
    bool normalSealed()const{return normalSealed_&&bool(context_.normalRecord);}
    bool extraSealed()const{return extraSealed_&&context_.extraRecord&&context_.extraRecord->acknowledged();}
    bool practiceSealed()const{return practiceFrames_!=0;}
    bool sealedPostrun()const{return normalSealed()||extraSealed()||practiceSealed();}
    unsigned normalGeneration()const{return context_.epoch;}
    unsigned normalRoute()const{return context_.route;}unsigned normalRank()const{return context_.rank;}
    int normalScorePosition()const{return position_;}std::int32_t normalScore()const{return context_.score;}
    unsigned normalContinues()const{return context_.continues;}
    unsigned normalFrames()const{return practiceSealed()?practiceFrames_:context_.normalRecord?context_.normalRecord->frames():context_.extraReplay?context_.extraReplay->frames():extraNativeFrames_;}
    unsigned normalPhase()const{return phase_;}unsigned normalSubphase()const{return subphase_;}
    bool normalNameCommitted()const{return phase_==14&&subphase_==3;}
    bool needsNormalNameOwner()const{return sealedPostrun()&&phase_==14&&!nameFontReady_;}
    unsigned normalNameConfirmationSerial()const{return nameConfirmationSerial_;}
    bool normalReplayNameCommitted()const{return replayConfirmed_&&replaySave_.pending();}
    bool ordinaryReplaySavePending()const{return replaySave_.pending();}
    unsigned replaySnapshots()const{return replaySnapshots_;}
    bool needsNormalReplayOwner()const{return sealedPostrun()&&phase_==15&&!replaySave_.bound();}
    std::vector<th128::pause::OriginalMenuGlyph> normalNameGlyphs()const{
        if(phase_!=14||subphase_!=2)return {};if(!nameFontReady_)throw std::runtime_error("original normal name font owner unavailable");
        return originalScoreNameGlyphs(context_.profile,context_.route,context_.rank,position_,name_,nameLength_,unsigned(selected_),Characters);
    }
    bool replayPresent()const{return bool(context_.replay||context_.normalReplay||context_.extraReplay||context_.practiceReplay);}
    const std::vector<std::uint8_t>& finishedNormalReplay()const{return finishedNormalReplay_;}
    unsigned ticks()const{return ticks_;}
    bool mainReady()const{return mainReady_;}
    bool mainHandoffReady()const{return sealedPostrun()&&phase_==1&&subphase_==0&&!replayPresent()&&!ordinaryReplaySavePending();}
    unsigned sceneNodeCount()const{return unsigned(scene_.nodes().size());}
    std::string presentation()const{
        std::ostringstream output;output<<'[';bool comma=false;
        for(const auto& [identity,node]:scene_.nodes()){
            if(!node->alive)continue;const auto& state=node->vm.state();
            if(state.ended||state.deleteRequested||state.sprite<0)continue;
            if(comma)output<<',';comma=true;
            output<<"{\"id\":"<<identity<<",\"bank\":"<<state.bank<<",\"script\":"<<state.script
                <<",\"sprite\":"<<state.sprite<<",\"clock\":"<<state.clock<<",\"layer\":"<<state.layer
                <<",\"x\":"<<state.x<<",\"y\":"<<state.y<<",\"sx\":"<<state.sx<<",\"sy\":"<<state.sy<<'}';
        }
        output<<']';return output.str();
    }
    const std::vector<std::uint8_t>& profile() const{return context_.profile;}
    const std::array<std::uint8_t,292>& finishedReplay() const{return finishedReplay_;}
};

}
