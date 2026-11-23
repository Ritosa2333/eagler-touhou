// There is no ordinary recorder or practice owner in a co-op run. Bind the terminal
// to the shared battle and consume the original post-boss MSG21 / MSG22 / MSG0 sequence.
namespace {
struct CoopClearOwner {
    unsigned generation=0,fadeSerial=0,deliveredSerial=0,counter=0;int stage=0,selector=0,scene=0;
    bool begun=false,messageDone=false,done=false,endingStarted=false;
    th128::stage::StageSettlementHost settlement;
};
std::unique_ptr<CoopClearOwner> coopClear;
bool hasCoopClear(){return coopActive()&&coopClear&&coopClear->generation==worldGeneration;}
bool coopIsSettling(){return hasCoopClear()&&coopClear->begun;}
bool coopClearDone(){return hasCoopClear()&&coopClear->done;}
int coopClearStage(){return coopClearDone()?coopClear->stage:0;}
bool beginCoopClear(const th128::message::Event& event,std::string& error){
    if(!hasCoopClear()||event.entry!=1||event.opcode!=21||coopClear->begun||!worldTickExecuting||!playerShotGroupsConfigured||normalRecord||normalReplay||replayPlayback.active()||world->thpracChapter()){
        error="Invalid co-op post-boss terminal source";return false;}
    auto& c=*coopClear;const bool extra=c.stage==16;
    th128::stage::StageSettlementContext context;context.stage=c.stage;context.score=scoreRaw;context.manager=stageManager;
    th128::stage::StageSettlementCallbacks callbacks;
    callbacks.setClearFlag=callbacks.finish=[](std::string& err){if(!playerHit->applyStageClearStart(err))return false;
        for(unsigned i=1;i<coop->seats;i++)if(!coop->extra[i-1]->body.applyStageClearStart(err))return false;return true;};
    callbacks.slot=[](unsigned group,unsigned slot){return playerShotGroups->slot(group,slot);};
    callbacks.retireTree=[](unsigned group,unsigned slot,int code,unsigned& count,std::string& err){const auto h=playerShotGroups->slot(group,slot);count=0;
        if(!h)return true;return playerShotGroups->walk(h,[&](unsigned,th128::anm::VM&,std::string&){++count;return true;},err)&&playerShotGroups->retireWithChildren(h,code,err);};
    if(!(extra?c.settlement.beginExtraTerminal(context,callbacks,error):c.settlement.beginTerminal(context,callbacks,error)))return false;
    const auto result=c.settlement.state();scoreRaw=result.context.score;stageManager=result.context.manager;
    // One original stage-clear payment, not one payment per ship. Use the best surviving motivation for its life term.
    int motivation=std::max(0,motivationRaw);for(unsigned i=1;i<coop->seats;i++)motivation=std::max(motivation,coop->extra[i-1]->motivation);
    const std::array<int,4> scale{5000,10000,20000,40000};const int q=pointValueRaw/100;
    const std::int64_t bonus=extra?(std::int64_t(motivation/100)+250)*40000:std::int64_t(q-q%10)*100+std::int64_t(motivation/100)*scale.at(worldStartDifficulty);
    scoreRaw=int(std::min<std::int64_t>(999999999,scoreRaw+bonus/10));stageManager.flags5a04|=0x18u;
    stageManager.baseScore5a38=std::bit_cast<int>(std::uint32_t(stageManager.baseScore5a38)+std::uint32_t(bonus));c.begun=true;return true;
}
bool tickCoopClear(std::string& error){
    if(!hasCoopClear()||!coopClear->begun)return true;auto& c=*coopClear;
    if(c.done||c.counter>=380||!c.settlement.tick(error)){if(error.empty())error="Invalid terminal stage clock";return false;}
    stageManager.field5a08=stageManager.field5a0c;++stageManager.field5a0c;stageManager.field5a10=float(stageManager.field5a0c);
    if(++c.counter!=380)return true;
    if(!c.messageDone||!c.fadeSerial||c.fadeSerial!=musicControl.serial||c.deliveredSerial!=c.fadeSerial||!musicControl.acknowledged||!musicControl.active||musicControl.operation!=5){error="Co-op clear requires completed original text and an owned BGM fade";return false;}
    c.scene=c.stage==16?16:15;const auto action=stageControl.request(c.scene);if(action.status!=th128::stage::RequestStatus::Written||action.pending!=c.scene){error="Co-op clear controller refused the original scene";return false;}
    c.done=true;return true;
}
}
extern "C" int th128_coop_enable_clear(const void* front,unsigned size){
    operationError.clear();const int stage=stageIdForEcl(worldStartedName);const std::array<int,7> terminal{3,5,8,10,13,15,16};const auto pos=std::find(terminal.begin(),terminal.end(),stage);
    if(!coopActive()||pos==terminal.end()||hasCoopClear()||world->frame()!=0||world->faulted()||worldTickExecuting||!messagesConfigured||!playerShotGroupsConfigured||world->thpracChapter()||normalRecord||normalReplay||
       !th128::runtime::inputRange(front,size)||size>8u*1024u*1024u||(stage==16)!=(worldStartDifficulty==4)){operationError="Co-op clear requires an actual fresh final native stage";return 0;}
    auto c=std::make_unique<CoopClearOwner>();c->stage=stage;c->generation=worldGeneration;c->selector=17+int(pos-terminal.begin());
    if(!c->settlement.load({static_cast<const std::uint8_t*>(front),size},operationError))return 0;coopClear=std::move(c);return 1;
}
extern "C" int th128_coop_take_music_fade(){
    if(!hasCoopClear()||!coopClear->fadeSerial||coopClear->deliveredSerial==coopClear->fadeSerial)return -1;
    if(!musicControl.active||musicControl.serial!=coopClear->fadeSerial||musicControl.operation!=5)return -2;
    coopClear->deliveredSerial=musicControl.serial;musicControl.acknowledged=true;return musicControl.durationFrames;
}
namespace th128::coopphysics {
bool terminalDrain(const StageWorld* current){
    if(!hasCoopClear()||current!=world.get()||!worldTickExecuting||normalRecord||normalReplay||!coopClear->begun||!coopClear->messageDone||
       messages->state().active||!messages->state().eclReleased||messages->state().entry!=1||!stageControl.initialized()||!stageControl.sessionConstructed()||stageControl.current()!=7||stageControl.gateSet())return false;
    const auto settled=coopClear->settlement.state();return settled.begun&&settled.scalarApplied&&settled.cleanupCalled&&!settled.failed&&stageControl.pending()==(coopClear->done?coopClear->scene:7);
}
}

// The same sealed cooperative World owns runtime15. No normal-record permit,
// fabricated debug terminal, profile write, or ordinary replay slot is involved.
extern "C" int th128_coop_start_ending(const void* message,unsigned messageBytes,const void* art,unsigned artBytes,const void* base,unsigned baseBytes,const void* text,unsigned textBytes){
    operationError.clear();
    if(!coopClearDone()||coopClear->scene!=15||coopClear->endingStarted||worldTickExecuting||world->faulted()||normalRecord||normalReplay||world->thpracChapter()||stageControl.current()!=7||stageControl.pending()!=15||
       !th128::runtime::inputRange(message,messageBytes)||!th128::runtime::inputRange(art,artBytes)||!th128::runtime::inputRange(base,baseBytes)||!th128::runtime::inputRange(text,textBytes)){
        operationError="Ending requires the sealed cooperative runtime15 owner";return 0;
    }
    using namespace th128::ending;
    const RuntimeGate gate{true,15,coopClear->selector-17,coopClear->stage,worldGeneration,false,stageControl.flags()};
    const auto bytes=[](const void* p,unsigned n){return std::span<const std::uint8_t>(static_cast<const std::uint8_t*>(p),n);};
    const auto sound=[](std::string& error){return coopSound(0,0,error);};
    const auto controls=[]()->std::optional<FadeControls>{return FadeControls{0,false,0,1.f};};
    const auto commit=stageControl.constructEnding15([&]{return activeEndingRuntime()->startCooperative(gate,bytes(message,messageBytes),bytes(art,artBytes),bytes(base,baseBytes),bytes(text,textBytes),sound,controls,operationError);});
    if(commit.status!=th128::stage::CommitStatus::Applied){if(operationError.empty())operationError="Cooperative runtime15 construction failed";return 0;}
    coopClear->endingStarted=true;return 1;
}
extern "C" int th128_coop_tick_ending(unsigned holding,unsigned pressed,unsigned released){
    operationError.clear();if(!coopClearDone()||!coopClear->endingStarted||worldTickExecuting||stageControl.current()!=15){operationError="No live cooperative ending";return 0;}
    grazeSounds.clear();const auto advanced=activeEndingRuntime()->tick({holding,pressed,released},worldGeneration,operationError);
    if(advanced)coop->frameSounds.swap(grazeSounds);return advanced?1:0;
}