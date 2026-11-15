#include "game/StageWorld.hpp"
#include "game/BulletVisuals.hpp"
#include "game/AppearanceBounds.hpp"
#include "game/BulletUpdateHost.hpp"
#include "game/GrazeRules.hpp"
#include "game/EnemyVisuals.hpp"
#include "game/PlayerHitHost.hpp"
#include "game/PlayerShotHost.hpp"
#include "game/PlayerShotGroups.hpp"
#include "game/FreezeRules.hpp"
#include "game/ItemHost.hpp"
#include "game/PlayerDeathHost.hpp"
#include "game/DamageFieldPool.hpp"
#include "game/PlayerRespawnRules.hpp"
#include "game/EnemyDropEmission.hpp"
#include "game/PerfectHud.hpp"
#include "game/BossHud.hpp"
#include "game/RingGeometry.hpp"
#include "game/HeadPower.hpp"
#include "game/PercentHud.hpp"
#include "game/FreezeRewardPanel.hpp"
#include "game/FocusIndicator.hpp"
#include "game/PlayerFireHost.hpp"
#include "game/PerfectFreezeHost.hpp"
#include "game/PauseGameOverHost.hpp"
#include "game/OriginalMenuInputFilter.hpp"
#include "game/NativeRunReplay.hpp"
#include "game/OwnedReplayPlaybackInput.hpp"
#include "game/NativeReplayStageBinding.hpp"
#include "game/NativeReplaySaveMenu.hpp"
#include "game/OriginalReplayNameGlyphs.hpp"
#include "game/OriginalReplayNameContext.hpp"
#include "game/NativeReplaySlotPreview.hpp"
#include "game/OriginalMenuFont.hpp"
#include "game/IceHost.hpp"
#include "game/FreezeAccounting.hpp"
#include "game/Geometry526Host.hpp"
#include "game/CircleFan.hpp"
#include "game/CancellationEffects.hpp"
#include "game/MessageHost.hpp"
#include "game/MessageTimeControl.hpp"
#include "game/MessageVisuals.hpp"
#include "game/RouteChoiceHost.hpp"
#include "game/StageSettlementHost.hpp"
#include "game/StageTransitionHost.hpp"
#include "game/StageControl.hpp"
#include "game/NativeRecordBootstrap.hpp"
#include "game/NativeRecordTiming.hpp"
#include "game/NativeRecordAccounting.hpp"
#include "game/NativeFreshReplayRecord.hpp"
#include "game/NativeExtraReturnOwner.hpp"
#include "game/NativeRecordSessionPolicy.hpp"
#include "game/NativePostrunRecord.hpp"
#include "game/NativeNormalRecord.hpp"
#include "game/NativeOrdinaryTerminalOwner.hpp"
#include "game/OrdinaryTransition.hpp"
#include "game/NativeBattleResources.hpp"
#include "game/NativePostrunMenu.hpp"
#include "game/NativeNormalMenuReceipt.hpp"
#include "game/NativePostrunMedia.hpp"
#include "game/NativeNormalMainOwner.hpp"
#include <cstring>
#include "game/StageSessionIdentity.hpp"
#include "game/SpellVisuals.hpp"
#include "game/StageIntroLogos.hpp"
#include "Memory.hpp"
#include "DrawJsonWriter.hpp"
#include "game/ending/EndingRuntime.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace {
th128::postrun::BattleResources ownedBattle;
auto& world=ownedBattle.world;
auto& laserLines=ownedBattle.laserLines;
auto& stageIntro=ownedBattle.stageIntro;
std::string stageIntroDrawError;
auto& bulletVisuals=ownedBattle.bulletVisuals;
auto& bulletAnimations=ownedBattle.bulletAnimations;
auto& bulletProfiles=ownedBattle.bulletProfiles;
auto& enemyAppearances=ownedBattle.enemyAppearances;
std::string operationError,snapshot,bulletDraws,presentationError;
float playerProfileRadius=0;
float playerAttractionSpeed=0;
th128::graze::Globals grazeGlobals{0,0};
bool grazeConfigured=false;
std::vector<th128::bullet::SoundRequest> grazeSounds;
std::uint64_t soundJournalBase=0;
unsigned soundJournalEpoch=1;
unsigned soundJournalWindowSerial=0;
unsigned soundJournalWindowCount=0;
unsigned soundJournalWindowEpoch=0;
std::uint64_t soundJournalWindowBase=0;
bool soundJournalSealed=false;
extern "C" unsigned th128_host_capsule_next_serial();
static unsigned nextDeliverySerial(){return th128_host_capsule_next_serial();}
void advanceSoundJournalReset(){
    soundJournalBase=0;
    soundJournalWindowSerial=0;
    soundJournalWindowCount=0;
    soundJournalWindowEpoch=0;
    soundJournalWindowBase=0;
    ++soundJournalEpoch;
    if(soundJournalEpoch==0){soundJournalEpoch=1;soundJournalSealed=true;}
}
auto& enemyVisuals=ownedBattle.enemyVisuals;
auto& cancellationEffects=ownedBattle.cancellationEffects;
auto& playerHit=ownedBattle.playerHit;
auto& playerShots=ownedBattle.playerShots;
auto& playerShotGroups=ownedBattle.playerShotGroups;
bool playerShotGroupsConfigured=false;
auto& itemHost=ownedBattle.itemHost;
auto& perfectHud=ownedBattle.perfectHud;
bool perfectHudConfigured=false;
auto& bossHud=ownedBattle.bossHud;
auto& geometryEffects=ownedBattle.geometryEffects;
bool geometryConfigured=false;
auto& damageFields=ownedBattle.damageFields;
unsigned enemiesDefeated=0;
extern std::string worldStartedName;
extern int worldStartDifficulty;
unsigned standaloneEffectOwner=1000000;
std::unordered_set<std::uint32_t> standaloneEffectOwners;
bool standaloneEclEffect(int bank,int script,const std::array<float,3>& position,int layer,bool head,std::string& error){
    if(bank!=0){error="ECL effect bank is not registered in the standalone effect owner";return false;}
    if((script==125||script==127||script==133||script==138||script==160||script==161
        ||((script==140||script==148)&&worldStartedName=="st_ex.ecl"&&worldStartDifficulty==4))&&layer==9&&head){
        while(standaloneEffectOwner<1065536){
            const auto owner=standaloneEffectOwner;
            bool realOwner=false;
            for(const auto& entity:world->entities())if(entity.id==owner){realOwner=true;break;}
            if(realOwner||standaloneEffectOwners.contains(owner)){++standaloneEffectOwner;continue;}
            if(!enemyVisuals->create(owner,0,0,script,9,error))return false;
            if(!enemyVisuals->position(owner,320+position[0],16+position[1],position[2])){
                if(error.empty())error="original standalone effect pose rejected";
                enemyVisuals->removeOwner(owner);return false;
            }
            standaloneEffectOwners.insert(owner);++standaloneEffectOwner;return true;
        }
        error="standalone ECL effect identity budget";return false;
    }
    return bool(cancellationEffects->spawn(script,320+position[0],16+position[1],position[2],layer,error,head));
}
int scoreRaw=0,pointValueRaw=100000,itemScalar=0;
int debugInfiniteRespawnSession=0;
bool debugInfiniteRespawnRun=false,debugRunTainted=false,debugSetterExecuting=false,worldTickExecuting=false;
unsigned debugReviveRefills=0;
int debugLastRefillBefore=0,debugLastRefillAfter=0;
auto& scorePopups=ownedBattle.scorePopups;
auto& scoreFont=ownedBattle.scoreFont;
bool scoreFontLoaded=false;
bool headPowerConfigured=false;
auto& focusIndicator=ownedBattle.focusIndicator;
bool focusIndicatorConfigured=false,playerFocused=false;
bool playerConfigured=false;
th128::player::hit::HitControl playerHitControl{false,0};
th128::player::hit::WindowControl playerWindowControl{false,0,0,0};
bool playerMotion=false;
th128::player::RawPosition playerRaw{0,51200};
th128::player::Input playerInput;
bool playerShotsConfigured=false;
bool shotHitsConfigured=false;
bool rapidShotConfigured=false;
int rapidShotCount=-1;
auto& playerFire=ownedBattle.playerFire;
auto& iceHost=ownedBattle.iceHost;
auto& perfectHost=ownedBattle.perfectHost;
bool perfectConfigured=false;
std::string perfectConfigError;
std::unique_ptr<th128::pause::Host> originalPauseMenu;
std::vector<std::uint8_t> originalPauseBackdropBytes;
th128::pause::OriginalMenuInputFilter originalMenuInput;
unsigned originalPauseSoundCursor=0;
int originalContinueCount=0;
std::string originalPauseStateText;
std::shared_ptr<th128::record::NativeRunReplay> normalReplay;
std::shared_ptr<th128::record::NativeNormalRecord> normalRecord;
struct NormalTerminalRuntime {
    bool resourcesConfigured=false,begun=false,messageDone=false,requested=false,endingStarted=false,sessionRetired=false;
    unsigned generation=0,lastTickFrame=0,musicSerial=0;
    int stage=0,progress=0,bonus=0,selector=0,counter=0,runtimeScene=0,fadeElapsed=-1,fadeAlpha=0;
    std::unique_ptr<th128::stage::StageSettlementHost> settlement;
    std::unique_ptr<th128::record::NativeOrdinaryTerminalOwner> owner;
    th128::stage::OrdinaryTransition transition;
};
NormalTerminalRuntime normalTerminal;
// Native fixed60 scheduler clock; global tasks retain this owner across BattleResources retirement.
th128::anm::Runtime ordinaryGlobalTaskRuntime;
struct NormalPostrunRuntime {
    unsigned generation=0,serial=0;
    bool awaitingPresent=false,failed=false,extra=false;
    std::unique_ptr<th128::postrun::Menu> menu;
    std::unique_ptr<th128::postrun::Media> media;
    std::array<std::vector<std::uint8_t>,4> assets;
    th128::pause::OriginalMenuInputFilter input;
    std::vector<std::uint8_t> profile;
    th128::postrun::NativeNormalMenuReceipt receipt;
    th128::postrun::NativeNormalMainOwner mainOwner;
    th128::postrun::NativeNormalMainRandom retainedRandom{};
    std::string text;
};
NormalPostrunRuntime normalPostrun;
unsigned normalMainAdoptedGeneration=0;
std::unique_ptr<th128::record::NativeReplaySaveMenu> normalReplaySave;
th128::record::OriginalReplayNameContext normalReplayNameContext;
std::vector<std::uint8_t> normalReplayBytes;
std::array<bool,25> normalReplayOccupied{};
std::array<th128::record::NativeReplaySlotPreview,25> normalReplayPreviewRows{};
double normalReplayOrigin=0,normalReplayClock=0;
float normalReplayFPS=0;
bool normalReplayClockPending=false;
th128::record::OwnedReplayPlaybackInput replayPlayback;
std::string replayPlaybackStateText;
unsigned normalReplayClockFrame=0,normalReplaySaveSoundCursor=0;
bool perfectBatchOwned=false;
bool perfectModeOwned=false;
std::string perfectZDenied;
bool playerFireConfigured=false;
th128::Random worldDisplayRandom;
float lastFrozenArea=0;
int freezeBonus=0;
th128::hud::FreezeRewardPanel freezeRewardPanel;
bool playerDeathConfigured=false;
int deathAwardSelector=-1;
unsigned deathsCommitted=0;
bool playerRespawnConfigured=false;
unsigned respawnsCommitted=0;
th128::player::respawn::ControlState respawnControl;
th128::player::death::Position respawnSavedPosition{0,0,0};
std::array<float,3> emptyLaserCancelCenter{};
std::unique_ptr<th128::player::DeathOperation> deathOperation;
int motivationRaw=0,motivationFlag=0,shotHitBusy=0,shotDamageBypass=0;
unsigned shotSpellFlags=0;
float cumulativeFrozenArea=0;
int shotDifficulty=1;
std::array<std::uint32_t,th128::shotenemy::DamageSourceCount> playerDamageSourceFlags{};
auto& messages=ownedBattle.messages;
auto& messageVisuals=ownedBattle.messageVisuals;
bool messagesConfigured=false,messageFrontConfigured=false;
unsigned messageMusicSerial=0;
auto& routeChoice=ownedBattle.routeChoice;
bool routeChoiceConfigured=false,routeChoiceStarted=false;
int routeChoiceStage=0,routeChoiceMessage78=0,routeChoiceMessage80=0;
bool routeChoiceInitialContextExplicit=false,routeChoiceInitialContextDefault=true;
bool routeChoiceInitialContextKnown=false;
std::string routeChoiceError;
std::size_t routeChoiceSoundConsumed=0;
auto& stageTransition=ownedBattle.stageTransition;
bool stageTransitionConfigured=false;
int stageTransitionCurrentId=0,stageTransitionProgress=0,stageTransitionGate74=0,stageTransitionMessage78=0;
int stageSessionPresent=0,stageSessionField74=0;
th128::stage::StageSettlementManager stageManager{};
struct DebugTerminalRecord {
    bool configured=false,begun=false,messageDone=false,requested=false;
    int stage=0,index=-1,selector=0,runtimeScene=0,counter=0,bonus=0;
    int fadeElapsed=-1,fadeAlpha=0;
    unsigned generation=0;
};
DebugTerminalRecord debugTerminal;
auto& battleEndingRuntime15=ownedBattle.endingRuntime15;
std::unique_ptr<th128::ending::Runtime15> ordinaryDetachedEnding;
th128::ending::Runtime15* activeEndingRuntime(){
    return ordinaryDetachedEnding?ordinaryDetachedEnding.get():battleEndingRuntime15.get();
}
std::string endingRuntimeSnapshot;
auto& debugTerminalSettlement=ownedBattle.debugTerminalSettlement;
std::string stageTransitionError;
th128::stage::Control stageControl;
struct MusicControlRecord {
    unsigned serial=0;
    int operation=5;
    int durationFrames=0;
    int durationSeconds=0;
    bool immediate=false;
    bool acknowledged=false;
    bool active=false;
};
MusicControlRecord musicControl;
unsigned musicControlSerial=0;
struct StageCarryRecord {
    std::shared_ptr<th128::record::NativeRunReplay> normalReplay;
    std::shared_ptr<th128::record::NativeNormalRecord> normalRecord;
    int originalContinueCount=0;
    double normalReplayOrigin=0,normalReplayClock=0;
    unsigned token=0;
    bool valid=false,consumed=false,gameRandomRestored=false,scalarRestored=false;
    unsigned sourceGeneration=0,restoredGeneration=0,scalarRestoredGeneration=0;
    unsigned sourceTransitionSerial=0;
    int sourceCurrentId=0,nextId=0;
    std::int32_t sourceProgress=0,nextProgress=0;
    std::string sourceWorldName;
    int sourceDifficulty=0,difficulty=0;
    int sourceControlCurrent=0,sourceControlPending=0;
    bool sourceControlActionRecreate=false,sourceDestroyExisting=false;
    std::int32_t sourceControlSessionArgument=0;
    int sourceGate74=0;
    std::int32_t sourceMessage78=0;
    int scoreRaw=0,pointValueRaw=0,motivationRaw=0,motivationFlag=0;
    int grazeCount=0,powerRaw=0;
    float cumulativeFrozenArea=0;
    int perfectCharge=0,shotDifficulty=0;
    int debugInfiniteRespawn=0;
    bool debugRunTainted=false;
    unsigned debugReviveRefills=0;
    int debugLastRefillBefore=0,debugLastRefillAfter=0;
    th128::Random gameRandom{},displayRandom{};
    bool preInitBound=false;
    th128::Random preInitGameRandom{},preInitDisplayRandom{};
    bool targetBound=false;
    std::string targetWorldName;
    int targetDifficulty=0;
    bool acknowledged=false;
};
StageCarryRecord stageCarry;
unsigned stageCarryNextToken=1;
unsigned stageTransitionSerialNext=1;
void captureDebugCarry(StageCarryRecord& carry){
    carry.debugInfiniteRespawn=debugInfiniteRespawnRun?1:0;
    carry.debugRunTainted=debugRunTainted;
    carry.debugReviveRefills=debugReviveRefills;
    carry.debugLastRefillBefore=debugLastRefillBefore;
    carry.debugLastRefillAfter=debugLastRefillAfter;
}
void restoreDebugCarry(const StageCarryRecord& carry){
    debugInfiniteRespawnRun=carry.debugInfiniteRespawn!=0;
    debugRunTainted=carry.debugRunTainted;
    debugReviveRefills=carry.debugReviveRefills;
    debugLastRefillBefore=carry.debugLastRefillBefore;
    debugLastRefillAfter=carry.debugLastRefillAfter;
}
std::string worldStartedName;
int worldStartDifficulty=0;
int currentStageProgress=0;
unsigned worldGeneration=0;

struct DebugRecordPrefixState {
    bool prepared=false;
    bool clockBound=false;
    bool captureApplied=false;
    bool worldBound=false;
    bool failed=false;
    bool freshRecordFieldsApplied=false;
    std::int32_t highScoreRaw=0;
    std::int32_t highScoreContinue=0;
    std::int32_t playCount=0;
    bool sessionPresent=false;
    std::uint32_t sessionFlags60=0;
    std::uint32_t sessionField74=0;
    std::uint32_t replayMode=0;
    unsigned sessionGeneration=0;
    std::uint64_t activeCallbacks=0;
    unsigned clearRecordEvents=0;
    unsigned epoch=0;
    unsigned generation=0;
    unsigned initialSeed=0;
    unsigned initialCalls=0;
    unsigned bootstrapSeed=0;
    unsigned bootstrapCalls=0;
    unsigned capturedSeed=0;
    unsigned capturedPriorCalls=0;
    std::uint64_t clockTime64=0;
    double clockFirstSeconds=0;
    th128::Random random;
    th128::record::NativeRecordTiming timing;
    std::vector<std::uint8_t> profile;
};
DebugRecordPrefixState debugRecordPrefix;
std::string debugRecordPrefixSnapshot;
std::shared_ptr<th128::record::NativeFreshReplayRecord> debugFreshReplay;
std::shared_ptr<th128::record::NativeRunReplay> debugExtraReplay;
th128::record::NativeExtraReturnOwner extraReturnOwner;
std::vector<std::uint8_t> extraPostrunNameFont;
unsigned debugFreshReplayGeneration=0,debugFreshReplayEpoch=0;
struct OrdinaryRecordOwner {
    bool configured=false,endingStarted=false,returned=false,endingPresented=false,sessionRetired=false;
    unsigned stage=0,route=0,rank=0,epoch=0,generation=0;
    std::uint64_t endingPresentedTick=0;
};
OrdinaryRecordOwner ordinaryRecordOwner;
unsigned actualRecordRoute(){return ordinaryRecordOwner.configured?ordinaryRecordOwner.route:6u;}
bool actualRecordTerminalReady(){
    if(!debugTerminal.requested||!debugTerminal.messageDone)return false;
    if(ordinaryRecordOwner.configured)
        return ordinaryRecordOwner.returned&&ordinaryRecordOwner.endingStarted
            &&ordinaryRecordOwner.generation==worldGeneration&&debugTerminal.runtimeScene==15
            &&debugTerminal.stage==int(ordinaryRecordOwner.stage)
            &&debugTerminal.selector==int(ordinaryRecordOwner.route+17);
    return debugTerminal.runtimeScene==16&&debugTerminal.selector==23;
}

bool actualRecordScoreRetirementReady(){
    if(ordinaryRecordOwner.configured)
        return !ordinaryRecordOwner.endingStarted&&!ordinaryRecordOwner.returned
            &&!ordinaryRecordOwner.sessionRetired&&ordinaryRecordOwner.generation==worldGeneration
            &&stageControl.current()==7&&stageControl.pending()==15&&debugTerminal.requested
            &&debugTerminal.runtimeScene==15&&debugTerminal.messageDone
            &&debugTerminal.stage==int(ordinaryRecordOwner.stage)
            &&debugTerminal.selector==int(ordinaryRecordOwner.route+17);
    return stageControl.current()==7&&stageControl.pending()==16&&actualRecordTerminalReady();
}

struct DebugRecordDriverState {
    bool bound=false;
    unsigned epoch=0,generation=0,phase=0,lastClockSerial=1;
    unsigned completedFrames=0,controllerCalls=0,presentationFrame=0;
    std::uint8_t configurationCounterByte=0;
    double lastSeconds=0,beginSeconds=0,gameplayAnchorSeconds=0;
    bool gameplayAnchorKnown=false;
    std::uint64_t lastTime64=0;
};
DebugRecordDriverState debugRecordDriver;
std::string debugRecordDriverSnapshot,debugRecordPresentationSnapshot;
std::shared_ptr<th128::record::NativePostrunRecord> debugPostrunRecord=std::make_shared<th128::record::NativePostrunRecord>();
std::string debugPostrunSnapshot;
std::unique_ptr<th128::postrun::Menu> actualPostrunMenu;
std::shared_ptr<th128::record::NativeFreshReplayRecord> retainedPostrunReplay;
std::unique_ptr<th128::postrun::Media> actualPostrunMedia;
std::array<std::vector<std::uint8_t>,3> postrunAssets;
std::string retiredWorldSnapshot,postrunStateText,postrunMediaText,postrunPresentationText;
unsigned postrunPresented=0,postrunMediaACKs=0;




auto& spellVisuals=ownedBattle.spellVisuals;bool spellsConfigured=false;
th128::spell::Result spellResult;
int spellResultFrames=0;

bool syncPerfectCharge(std::string& error) {
    if(!perfectConfigured||!perfectHost)return true;
    const auto status=perfectHost->status();
    if(status.active||status.charge==playerWindowControl.perfectCharge4b4d70)return true;
    return perfectHost->setCharge(playerWindowControl.perfectCharge4b4d70,error);
}

bool settlePerfectFreezeArea(std::uint32_t pixels,std::string& error) {
    const auto accounting=th128::freeze::account(pixels,scoreRaw);
    const auto oldLevel=th128::playershot::selectShotGroup(cumulativeFrozenArea,shotDifficulty);
    if(!accounting||!oldLevel){error="unsupported perfect freeze completion accounting";return false;}
    const float nextArea=cumulativeFrozenArea+accounting->percent;
    const auto nextLevel=th128::playershot::selectShotGroup(nextArea,shotDifficulty);
    if(!nextLevel){error="unsupported perfect cumulative freeze shot level";return false;}
    cumulativeFrozenArea=nextArea;lastFrozenArea=accounting->percent;
    if(*nextLevel>*oldLevel){
        if(!spellsConfigured){error="original shot level-up visual owner unavailable";return false;}
        const auto position=world->playerAimPosition();
        if(!spellVisuals->levelUp(int(*nextLevel),{position.x,position.y,position.z},error))return false;
        if(playerShotGroupsConfigured&&!playerShotGroups->updateSelection(
            cumulativeFrozenArea,shotDifficulty,0,{position.x,position.y,position.z},error))return false;
        if(grazeSounds.size()>=65536){error="perfect shot level sound journal budget";return false;}
        grazeSounds.push_back({13,0});
    }
    scoreRaw=accounting->score;freezeBonus=accounting->bonus;
    if(!freezeRewardPanel.complete(accounting->percent,accounting->bonus,world->playerAimPosition().y)){
        error="original freeze reward panel completion rejected";return false;
    }
    return true;
}

const char* routeChoicePhaseName(th128::routechoice::Phase phase) {
    switch(phase) {
    case th128::routechoice::Phase::Idle: return "idle";
    case th128::routechoice::Phase::Intro: return "intro";
    case th128::routechoice::Phase::Select: return "select";
    case th128::routechoice::Phase::Confirm: return "confirm";
    case th128::routechoice::Phase::Releasing: return "releasing";
    case th128::routechoice::Phase::Done: return "done";
    case th128::routechoice::Phase::Failed: return "failed";
    }
    return "unknown";
}

void writeRouteChoiceContextState(std::ostringstream& text) {
    const auto& status=routeChoice->status();
    text<<"{\"configured\":"<<(routeChoiceConfigured?"true":"false")
        <<",\"started\":"<<(routeChoiceStarted?"true":"false")
        <<",\"stage\":"<<routeChoiceStage
        <<",\"initialMessage78\":"<<routeChoiceMessage78
        <<",\"initialMessage80\":"<<routeChoiceMessage80
        <<",\"initialContextExplicit\":"<<(routeChoiceInitialContextExplicit?"true":"false")
        <<",\"initialContextDefault\":"<<(routeChoiceInitialContextDefault?"true":"false")
        <<",\"initialKnown\":"<<(routeChoiceInitialContextKnown?"true":"false")
        <<",\"currentMessage78\":"<<status.message78
        <<",\"currentMessage80\":"<<status.message80<<'}';
}

void writeRouteChoiceState(std::ostringstream& text) {
    const auto& status=routeChoice->status();
    text<<"{\"configured\":"<<(routeChoiceConfigured?"true":"false")<<",\"started\":"<<(routeChoiceStarted?"true":"false")
        <<",\"stage\":"<<routeChoiceStage<<",\"phase\":\""<<routeChoicePhaseName(status.phase)<<"\""
        <<",\"message70\":"<<status.message70<<",\"message74\":"<<status.message74
        <<",\"message78\":"<<status.message78<<",\"message7c\":"<<status.message7c
        <<",\"message80\":"<<status.message80<<",\"handle\":"<<status.handle
        <<",\"confirmed\":"<<(status.confirmed?"true":"false")
        <<",\"interruptCount\":"<<status.interruptCount<<",\"soundCount\":"<<status.soundCount<<",\"result\":";
    if(const auto result=routeChoice->result())text<<*result;else text<<"null";
    text<<'}';
}

void writeJsonString(std::ostringstream& text,const std::string& value) {
    text<<'"';
    for(const auto character:value) {
        if(character=='"'||character=='\\')text<<'\\';
        text<<character;
    }
    text<<'"';
}

void writeMusicControlState(std::ostringstream& text) {
    text<<"{\"serial\":"<<musicControl.serial<<",\"operation\":5"
        <<",\"durationFrames\":"<<musicControl.durationFrames
        <<",\"durationSeconds\":"<<musicControl.durationSeconds
        <<",\"immediate\":"<<(musicControl.immediate?"true":"false")
        <<",\"acknowledged\":"<<(musicControl.acknowledged?"true":"false")<<'}';
}

int stageIdForEcl(const std::string& name);
void writeRandomEvidence(std::ostringstream& text,const th128::Random& random) {    text<<"{\"seed\":"<<random.seed<<",\"calls\":"<<random.calls<<'}';
}
void writeFlowEvidence(std::ostringstream& text) {
    const auto game=world->gameRandomState();
    const auto current=stageIdForEcl(worldStartedName);
    const auto role=th128::stage::roleForStage(current);
    const bool canReceive=bool(role)&&th128::stage::canAcceptCarry(*role);
    const bool canHandoff=bool(role)&&th128::stage::canRequestOrdinaryHandoff(*role);
    const bool scalar=stageCarry.scalarRestored
        &&stageCarry.scalarRestoredGeneration==worldGeneration;
    const bool committed=stageCarry.acknowledged&&stageCarry.consumed;
    text<<"{\"protocol\":1,\"currentId\":"<<current
        <<",\"difficulty\":"<<worldStartDifficulty
        <<",\"shotDifficulty\":"<<shotDifficulty
        <<",\"playerShotsConfigured\":"<<(playerShotsConfigured?"true":"false")
        <<",\"playerShotGroupOwner\":";
    if(playerShotGroupsConfigured&&playerShotGroups){
        const auto groupOwner=playerShotGroups->state();
        text<<"{\"loaded\":"<<(groupOwner.loaded?"true":"false")
            <<",\"configured\":"<<(groupOwner.configured?"true":"false")
            <<",\"retired\":"<<(groupOwner.retired?"true":"false")
            <<",\"failed\":"<<(groupOwner.failed?"true":"false")
            <<",\"group\":"<<groupOwner.group<<",\"selector\":"<<groupOwner.selector
            <<",\"activeGroups\":"<<groupOwner.activeGroups<<",\"activeHandles\":"<<groupOwner.activeHandles
            <<",\"retiredHandles\":"<<groupOwner.retiredHandles
            <<",\"nativeDefault\":"<<(groupOwner.nativeDefault?"true":"false")
            <<",\"callbackResult\":"<<groupOwner.callbackResult<<'}';
    }else text<<"null";
    text<<",\"progress\":"<<currentStageProgress
        <<",\"worldGeneration\":"<<worldGeneration
        <<",\"canReceiveCarry\":"<<(canReceive?"true":"false")
        <<",\"canRequestOrdinaryHandoff\":"<<(canHandoff?"true":"false")
        <<",\"gameRandom\":";
    writeRandomEvidence(text,game);
    text<<",\"displayRandom\":";
    writeRandomEvidence(text,worldDisplayRandom);
    text<<",\"carry\":{\"tokenAvailable\":"
        <<(stageCarry.valid&&!stageCarry.consumed?"true":"false")
        <<",\"consumed\":"<<(stageCarry.consumed?"true":"false")
        <<",\"committed\":"<<(committed?"true":"false")
        <<",\"sourceId\":"<<(stageCarry.valid?stageCarry.sourceCurrentId:0)
        <<",\"targetId\":"<<(stageCarry.valid?stageCarry.nextId:0)
        <<",\"sourceSerial\":"<<(stageCarry.valid?stageCarry.sourceTransitionSerial:0u)
        <<",\"boundGeneration\":"<<(stageCarry.preInitBound?stageCarry.restoredGeneration:0u)
        <<",\"preInitGameRandom\":";
    writeRandomEvidence(text,stageCarry.preInitBound?stageCarry.preInitGameRandom:th128::Random{});
    text<<",\"preInitDisplayRandom\":";
    writeRandomEvidence(text,stageCarry.preInitBound?stageCarry.preInitDisplayRandom:th128::Random{});
    text<<",\"scalarRestored\":"<<(scalar?"true":"false")
        <<",\"ACKed\":"<<(stageCarry.acknowledged?"true":"false")<<"}}";
}

void writeStageControlState(std::ostringstream& text) {
    const auto status=stageTransition->state();
    text<<"{\"initialized\":"<<(stageControl.initialized()?"true":"false")
        <<",\"flags\":"<<stageControl.flags()<<",\"gate\":"<<(stageControl.gateSet()?"true":"false")
        <<",\"current\":"<<stageControl.current()<<",\"pending\":"<<stageControl.pending()
        <<",\"requested\":"<<((stageTransitionConfigured&&status.requested)||stageControl.sessionStartupAttempted()?"true":"false")
        <<",\"committed\":"<<((stageTransitionConfigured&&status.committed)||stageControl.sessionConstructed()?"true":"false")
        <<",\"action\":\""<<(status.controlActionRecreate||stageControl.sessionConstructed()?"recreate-session":"none")<<"\""
        <<",\"sessionFlag\":"<<stageControl.sessionFlag()
        <<",\"startupAttempted\":"<<(stageControl.sessionStartupAttempted()?"true":"false")
        <<",\"startupConstructing\":"<<(stageControl.sessionConstructing()?"true":"false")
        <<",\"startupConstructed\":"<<(stageControl.sessionConstructed()?"true":"false")
        <<",\"destroyExisting\":"<<(status.controlDestroyExisting?"true":"false")
        <<",\"sessionArgument\":"<<status.controlSessionArgument<<'}';
}

void writeStageTransitionState(std::ostringstream& text) {
    const auto status=stageTransition->state();
    text<<"{\"configured\":"<<(stageTransitionConfigured?"true":"false")
        <<",\"currentId\":"<<stageTransitionCurrentId<<",\"progress\":"<<stageTransitionProgress
        <<",\"requested\":"<<(status.requested?"true":"false")
        <<",\"committed\":"<<(status.committed?"true":"false")
        <<",\"nextId\":"<<status.nextId<<",\"nextProgress\":"<<status.nextProgress
        <<",\"settlementDone\":"<<(status.settlementDone?"true":"false")
        <<",\"messageDone\":"<<(status.messageDone?"true":"false")<<",\"error\":";
    writeJsonString(text,status.error.empty()?stageTransitionError:status.error);
    text<<'}';
}

void writeDebugTerminalState(std::ostringstream& text){
    const auto settlement=debugTerminalSettlement->state();
    text<<"{\"configured\":"<<(debugTerminal.configured?"true":"false")
        <<",\"begun\":"<<(debugTerminal.begun?"true":"false")
        <<",\"stage\":"<<debugTerminal.stage<<",\"index\":"<<debugTerminal.index
        <<",\"selector\":"<<debugTerminal.selector<<",\"counter\":"<<debugTerminal.counter
        <<",\"messageDone\":"<<(debugTerminal.messageDone?"true":"false")
        <<",\"requested\":"<<(debugTerminal.requested?"true":"false")
        <<",\"runtimeScene\":"<<debugTerminal.runtimeScene<<",\"bonus\":"<<debugTerminal.bonus
        <<",\"fadeElapsed\":"<<debugTerminal.fadeElapsed<<",\"fadeAlpha\":"<<debugTerminal.fadeAlpha
        <<",\"baseScore\":"<<stageManager.baseScore5a38<<",\"frontHandle\":"<<stageManager.handle58cc
        <<",\"ownerSlotsVisited\":"<<settlement.ownerSlotsVisited<<",\"retiredEntities\":"<<settlement.retiredEntities
        <<",\"settlementNodes\":"<<debugTerminalSettlement->snapshots().size()
        <<",\"normalClear\":false,\"debugTainted\":"<<(debugRunTainted?"true":"false")<<'}';
}
void writeNormalRecordState(std::ostringstream& text){
    text<<"{\"bound\":"<<(normalRecord?"true":"false")<<",\"started\":"<<((normalRecord&&normalRecord->started())?"true":"false")
        <<",\"prepared\":"<<((normalRecord&&normalRecord->prepared())?"true":"false")<<",\"acknowledged\":"<<((normalRecord&&normalRecord->acknowledged())?"true":"false")
        <<",\"generation\":"<<worldGeneration<<",\"route\":"<<(normalRecord?normalRecord->route():0)<<",\"rank\":"<<(normalRecord?normalRecord->rank():0)
        <<",\"frames\":"<<(normalRecord?normalRecord->frames():0)<<",\"serial\":"<<((normalRecord&&normalRecord->prepared())?1:0)
        <<",\"bytes\":"<<((normalRecord&&normalRecord->prepared())?normalRecord->pending().size():0)<<",\"digest\":"<<(normalRecord?normalRecord->digest():0)
        <<",\"terminalStatsPrepared\":"<<((normalRecord&&normalRecord->terminalStatsPrepared())?"true":"false")
        <<",\"terminalScore\":"<<(normalRecord?normalRecord->terminalScore():0)<<",\"terminalContinues\":"<<(normalRecord?normalRecord->terminalContinues():0)
        <<",\"tainted\":"<<(debugRunTainted?"true":"false")<<",\"normalClear\":false,\"provenance\":\"ordinary-native-postrun\"}";
}
void writeNormalTerminalState(std::ostringstream& text){
    const bool bound=normalTerminal.owner&&normalTerminal.owner->bound();
    const bool matches=bound&&normalTerminal.generation==worldGeneration&&!debugRunTainted&&!replayPlayback.active()
        &&normalTerminal.owner->matchesContinuation(worldGeneration,unsigned(originalContinueCount),normalRecord,normalReplay);
    const int index=bound?int(normalTerminal.owner->identity().route):-1;
    text<<"{\"configured\":"<<(normalTerminal.resourcesConfigured?"true":"false")<<",\"begun\":"<<(normalTerminal.begun?"true":"false")
        <<",\"stage\":"<<normalTerminal.stage<<",\"index\":"<<index<<",\"selector\":"<<normalTerminal.selector
        <<",\"generation\":"<<normalTerminal.generation<<",\"counter\":"<<normalTerminal.counter<<",\"messageDone\":"<<(normalTerminal.messageDone?"true":"false")
        <<",\"requested\":"<<(normalTerminal.requested?"true":"false")<<",\"runtimeScene\":"<<normalTerminal.runtimeScene<<",\"bonus\":"<<normalTerminal.bonus
        <<",\"fadeElapsed\":"<<normalTerminal.fadeElapsed<<",\"fadeAlpha\":"<<normalTerminal.fadeAlpha
        <<",\"fadeTask\":"<<normalTerminal.transition.json()
        <<",\"ownerBound\":"<<(bound?"true":"false")<<",\"ownerClaimed\":"<<((bound&&normalTerminal.owner->claimed())?"true":"false")
        <<",\"ownerRecorded\":"<<((bound&&normalTerminal.owner->recorded())?"true":"false")<<",\"ownerMatches\":"<<(matches?"true":"false")
        <<",\"score\":"<<scoreRaw<<",\"continues\":"<<originalContinueCount<<",\"musicSerial\":"<<normalTerminal.musicSerial
        <<",\"musicAcknowledged\":"<<((musicControl.active&&musicControl.acknowledged&&musicControl.operation==5&&musicControl.serial==normalTerminal.musicSerial&&normalTerminal.musicSerial)?"true":"false")
        <<",\"settlementNodes\":"<<(normalTerminal.settlement?normalTerminal.settlement->snapshots().size():0)
        <<",\"sessionRetired\":"<<(normalTerminal.sessionRetired?"true":"false")<<",\"endingStarted\":"<<(normalTerminal.endingStarted?"true":"false")
        <<",\"replayPlayback\":"<<(replayPlayback.active()?"true":"false")<<",\"debugTainted\":"<<(debugRunTainted?"true":"false")
        <<",\"normalClear\":false,\"provenance\":\"ordinary-native-terminal-owner\"}";
}
std::string retiredOrdinaryLifecycle(){
    std::ostringstream text;
    text<<"{\"frame\":"<<world->frame()<<",\"scoreRaw\":"<<scoreRaw<<",\"worldLifecycle\":\"ordinary-battle-retirement\",\"battleRetirement\":{\"attempted\":"<<(ownedBattle.attempted()?"true":"false")
        <<",\"retired\":"<<(ownedBattle.retired()?"true":"false")<<",\"completedOwners\":"<<ownedBattle.completedOwners()<<",\"error\":";
    writeJsonString(text,ownedBattle.error());text<<"},\"normalRecord\":";writeNormalRecordState(text);
    text<<",\"normalTerminal\":";writeNormalTerminalState(text);
    text<<",\"stageControl\":{\"initialized\":"<<(stageControl.initialized()?"true":"false")<<",\"current\":"<<stageControl.current()<<",\"pending\":"<<stageControl.pending()
        <<",\"flags\":"<<stageControl.flags()<<",\"gate\":"<<(stageControl.gateSet()?"true":"false")<<",\"menuMode\":"<<stageControl.menuMode()<<",\"sessionFlag\":"<<stageControl.sessionFlag()<<'}';
    text<<",\"flow\":";writeFlowEvidence(text);text<<",\"musicControl\":";writeMusicControlState(text);
    text<<",\"soundRequests\":"<<grazeSounds.size()<<",\"debug\":{\"runTainted\":"<<(debugRunTainted?"true":"false")<<",\"infiniteRespawn\":"<<(debugInfiniteRespawnRun?"true":"false")<<"}}";
    return text.str();
}

void writeMessageState(std::ostringstream& text){
    const auto& state=messages->state();
    text<<"{\"configured\":"<<(messagesConfigured?"true":"false")<<",\"active\":"<<(state.active?"true":"false")
        <<",\"eclReleased\":"<<(state.eclReleased?"true":"false")<<",\"waiting\":"<<(state.waiting?"true":"false")
        <<",\"entry\":"<<state.entry<<",\"clock\":"<<state.clock<<",\"remaining\":"<<state.waitRemaining<<",\"speaker\":"<<state.speaker
        <<",\"font\":"<<state.font<<",\"x\":"<<state.x<<",\"y\":"<<state.y<<",\"musicSerial\":"<<messageMusicSerial<<",\"lines\":[";
    bool comma=false;for(const auto& line:state.textLines){if(comma)text<<',';comma=true;text<<'[';for(std::size_t index=0;index<line.size();++index){if(index)text<<',';text<<unsigned(line[index]);}text<<']';}
    text<<"],\"textPoses\":[";
    bool poseComma=false;for(const auto& pose:messageVisuals->textPresentation()){if(poseComma)text<<',';poseComma=true;text<<"{\"visible\":"<<(pose.visible?"true":"false")<<",\"alpha\":"<<pose.alpha<<",\"sx\":"<<pose.sx<<",\"sy\":"<<pose.sy<<",\"offsetY\":"<<pose.offsetY<<'}';}
    text<<"],\"routeChoice\":";
    writeRouteChoiceState(text);
    text<<'}';
}

void resetRouteChoice() {
    routeChoice=std::make_unique<th128::routechoice::Host>();
    routeChoiceConfigured=false;routeChoiceStarted=false;
    routeChoiceStage=0;routeChoiceMessage78=0;routeChoiceMessage80=0;routeChoiceError.clear();
    routeChoiceInitialContextExplicit=false;routeChoiceInitialContextDefault=true;routeChoiceInitialContextKnown=false;
    routeChoiceSoundConsumed=0;
}
bool consumeRouteChoiceSounds(std::string& error) {
    if(!routeChoiceConfigured)return true;
    const auto count=routeChoice->status().soundCount;
    if(count==routeChoiceSoundConsumed)return true;
    if(count<routeChoiceSoundConsumed){error="route choice sound counter regressed";return false;}
    const auto sound=routeChoice->status().lastSound;
    const auto delta=count-routeChoiceSoundConsumed;
    if(delta==1){
        if(sound!=10u&&sound!=7u){error="route choice emitted an unproven sound operation";return false;}
        if(grazeSounds.size()>=65536){error="route choice sound journal budget";return false;}
        grazeSounds.push_back({int(sound),0});
    }else if(delta==2&&sound==7u){
        if(grazeSounds.size()>65534){error="route choice sound journal budget";return false;}
        grazeSounds.push_back({10,0});
        grazeSounds.push_back({7,0});
    }else{
        error="route choice sound operation sequence is not the proven 10/7 pattern";return false;
    }
    routeChoiceSoundConsumed=count;return true;
}
void resetStageTransition() {
    stageTransition=std::make_unique<th128::stage::StageTransitionHost>();
    stageTransitionConfigured=false;
    stageTransitionCurrentId=0;stageTransitionProgress=0;stageTransitionGate74=0;stageTransitionMessage78=0;
    stageSessionPresent=0;stageSessionField74=0;
    stageManager={};
    debugTerminal={};normalTerminal={};normalPostrun={};normalMainAdoptedGeneration=0;
    debugTerminalSettlement=std::make_unique<th128::stage::StageSettlementHost>();
    stageTransitionError.clear();
}
void resetMusicControl() {
    musicControl=MusicControlRecord{};
    musicControl.operation=5;
    musicControlSerial=0;
}
bool validStageCarryValues(const StageCarryRecord& carry,std::string& error) {
    if(carry.sourceCurrentId<1||carry.sourceCurrentId>16||carry.nextId<1||carry.nextId>16){
        error="stage carry route id is outside the playable range";return false;
    }
    if(carry.scoreRaw<0||carry.scoreRaw>999999999||carry.pointValueRaw<0||carry.pointValueRaw>1000000000){
        error="stage carry score is outside the supported range";return false;
    }
    if(carry.motivationRaw< -1000000000||carry.motivationRaw>1000000000
        ||carry.motivationFlag< -1000000000||carry.motivationFlag>1000000000){
        error="stage carry motivation is outside the supported range";return false;
    }
    if(carry.grazeCount<0||carry.grazeCount>1000000000||carry.powerRaw<0||carry.powerRaw>1000000000){
        error="stage carry graze/power is outside the supported range";return false;
    }
    if(!std::isfinite(carry.cumulativeFrozenArea)||carry.cumulativeFrozenArea<0||carry.cumulativeFrozenArea>1000000000){
        error="stage carry frozen area is outside the supported range";return false;
    }
    if(carry.perfectCharge<0||carry.perfectCharge>30000||carry.shotDifficulty<0||carry.shotDifficulty>3
        ||carry.difficulty<0||carry.difficulty>4||carry.sourceDifficulty<0||carry.sourceDifficulty>4
        ||carry.debugInfiniteRespawn<0||carry.debugInfiniteRespawn>1||carry.debugReviveRefills>1000000000
        ||carry.debugLastRefillBefore< -1000000000||carry.debugLastRefillBefore>1000000000
        ||carry.debugLastRefillAfter< -1000000000||carry.debugLastRefillAfter>1000000000
        ||carry.sourceWorldName.empty()){
        error="stage carry player difficulty/world state is outside the supported range";return false;
    }
    return true;
}
void beginNewWorldGeneration() {
    ++worldGeneration;
    if(worldGeneration==0)++worldGeneration;
    stageIntro=std::make_unique<th128::stageintro::Host>();
    stageIntroDrawError.clear();
    worldStartedName.clear();
    worldStartDifficulty=0;
    currentStageProgress=0;
    if(stageCarry.valid&&!stageCarry.consumed){
        stageCarry.gameRandomRestored=false;
        stageCarry.restoredGeneration=0;
        stageCarry.scalarRestored=false;
        stageCarry.scalarRestoredGeneration=0;
        stageCarry.preInitBound=false;
        stageCarry.preInitGameRandom={};
        stageCarry.preInitDisplayRandom={};
        stageCarry.targetBound=false;
        stageCarry.targetWorldName.clear();
        stageCarry.targetDifficulty=0;
    }
}
constexpr std::array<const char*,17> kStageEclNames{
    "","st_a1_1.ecl","st_a1_2.ecl","st_a1_3.ecl","st_a2_2.ecl","st_a2_3.ecl",
    "st_b1_1.ecl","st_b1_2.ecl","st_b1_3.ecl","st_b2_2.ecl","st_b2_3.ecl",
    "st_c1_1.ecl","st_c1_2.ecl","st_c1_3.ecl","st_c2_2.ecl","st_c2_3.ecl",
    "st_ex.ecl",
};
const std::array<th128::stage::StageResourceRecord,16> kStageResources{{
    {1,"stage_a1.std","st_a1_1.ecl","st_a1.msg","logo_a1_1.anm",{"th128_00.wav","th128_02.wav"}},
    {2,"stage_b2.std","st_a1_2.ecl","st_a1_2.msg","logo_a1_2.anm",{"th128_01.wav","th128_02.wav"}},
    {3,"stage_c3.std","st_a1_3.ecl","st_a1_3.msg","logo_a1_3.anm",{"th128_05.wav","th128_06.wav"}},
    {4,"stage_c2.std","st_a2_2.ecl","st_a2_2.msg","logo_a2_2.anm",{"th128_01.wav","th128_02.wav"}},
    {5,"stage_b3.std","st_a2_3.ecl","st_a2_3.msg","logo_a2_3.anm",{"th128_05.wav","th128_06.wav"}},
    {6,"stage_b1.std","st_b1_1.ecl","st_b1.msg","logo_b1_1.anm",{"th128_00.wav","th128_02.wav"}},
    {7,"stage_a2.std","st_b1_2.ecl","st_b1_2.msg","logo_b1_2.anm",{"th128_01.wav","th128_02.wav"}},
    {8,"stage_c3.std","st_b1_3.ecl","st_b1_3.msg","logo_b1_3.anm",{"th128_05.wav","th128_06.wav"}},
    {9,"stage_c2.std","st_b2_2.ecl","st_b2_2.msg","logo_b2_2.anm",{"th128_01.wav","th128_02.wav"}},
    {10,"stage_a3.std","st_b2_3.ecl","st_b2_3.msg","logo_b2_3.anm",{"th128_05.wav","th128_06.wav"}},
    {11,"stage_c1.std","st_c1_1.ecl","st_c1.msg","logo_c1_1.anm",{"th128_00.wav","th128_02.wav"}},
    {12,"stage_a2.std","st_c1_2.ecl","st_c1_2.msg","logo_c1_2.anm",{"th128_01.wav","th128_02.wav"}},
    {13,"stage_b3.std","st_c1_3.ecl","st_c1_3.msg","logo_c1_3.anm",{"th128_05.wav","th128_06.wav"}},
    {14,"stage_b2.std","st_c2_2.ecl","st_c2_2.msg","logo_c2_2.anm",{"th128_01.wav","th128_02.wav"}},
    {15,"stage_a3.std","st_c2_3.ecl","st_c2_3.msg","logo_c2_3.anm",{"th128_05.wav","th128_06.wav"}},
    {16,"stage_ex.std","st_ex.ecl","st_ex.msg","logo_ex.anm",{"th128_10.wav","th128_11.wav"}},
}};
const th128::stage::StageResourceRecord* stageRecordForId(int id) {
    return id>=1&&id<=16?&kStageResources[std::size_t(id-1)]:nullptr;
}
bool validateCarryIdentity(const StageCarryRecord& carry,int targetId,std::int32_t targetProgress,
                           std::string& error) {
    const auto* sourceRecord=stageRecordForId(carry.sourceCurrentId);
    const auto* targetRecord=stageRecordForId(targetId);
    if(!sourceRecord||!targetRecord){
        error="stage carry identity record is unavailable";return false;
    }
    const th128::stage::SessionIdentity source{carry.sourceCurrentId,carry.sourceWorldName,
        carry.sourceDifficulty,carry.sourceProgress};
    const th128::stage::SessionIdentity target{targetId,worldStartedName,worldStartDifficulty,targetProgress};
    return th128::stage::validateCarryTransition(source,*sourceRecord,target,*targetRecord,
        carry.sourceGate74,carry.sourceMessage78,error);
}
bool stageEclMatches(int id,const std::string& name) {
    return id>=1&&id<=16&&name==kStageEclNames[std::size_t(id)];
}
int stageIdForEcl(const std::string& name) {
    for(int id=1;id<=16;++id)if(name==kStageEclNames[std::size_t(id)])return id;
    return 0;
}

bool ordinaryCarrySessionPresent(bool restored){
    static constexpr std::array<int,6> terminals{3,5,8,10,13,15},previous{2,4,7,9,12,14};
    if(!world||world->faulted()||!stageControl.initialized()||!stageControl.sessionConstructed()
        ||stageControl.current()!=7||stageControl.gateSet()
        ||(stageControl.pending()!=7&&(!restored||stageControl.pending()!=15)))return false;
    const int stage=stageIdForEcl(worldStartedName);const auto selected=std::find(terminals.begin(),terminals.end(),stage);
    if(selected==terminals.end())return false;
    const auto& c=stageCarry;const auto route=std::size_t(selected-terminals.begin());
    if(!c.valid||!c.token||!c.sourceGeneration||c.sourceGeneration>=worldGeneration
        ||!c.targetBound||!c.preInitBound||!c.gameRandomRestored||c.restoredGeneration!=worldGeneration
        ||c.sourceCurrentId!=previous[route]||stageIdForEcl(c.sourceWorldName)!=c.sourceCurrentId
        ||c.nextId!=stage||c.targetWorldName!=worldStartedName
        ||c.sourceDifficulty!=worldStartDifficulty||c.difficulty!=worldStartDifficulty||c.targetDifficulty!=worldStartDifficulty
        ||!c.sourceTransitionSerial||!c.sourceControlActionRecreate||!c.sourceDestroyExisting
        ||c.sourceControlCurrent!=7||c.sourceControlPending!=7||c.sourceControlSessionArgument!=0
        ||!c.normalRecord||!c.normalReplay||c.debugRunTainted||c.debugInfiniteRespawn)return false;
    // Original state12 reconstructs the actual session with sessionFlag=0; the archived carry proves presence.
    if(!restored)return !c.consumed&&!c.acknowledged&&world->frame()==0;
    return c.scalarRestored&&c.scalarRestoredGeneration==worldGeneration&&c.acknowledged&&c.consumed
        &&c.nextProgress==currentStageProgress&&c.originalContinueCount==originalContinueCount
        &&normalRecord==c.normalRecord&&normalReplay==c.normalReplay;
}

bool ordinaryTerminalDrainReady(unsigned generation,unsigned stage,int progress,std::string& error);
void bindOrdinaryTerminalAfterReplayBegin(){
    static constexpr std::array<int,6> terminals{3,5,8,10,13,15};
    const int stage=stageIdForEcl(worldStartedName);
    if(std::find(terminals.begin(),terminals.end(),stage)==terminals.end()){
        if(normalTerminal.resourcesConfigured||normalTerminal.owner)
            throw std::runtime_error("ordinary terminal resources differ from the actual current stage");
        return;
    }
    if(!world||world->faulted()||world->frame()!=0||worldTickExecuting||!playerHit
        ||!normalTerminal.resourcesConfigured||normalTerminal.owner||normalTerminal.generation!=worldGeneration
        ||normalTerminal.stage!=stage||normalTerminal.progress!=currentStageProgress
        ||!ordinaryCarrySessionPresent(true)
        ||stageControl.current()!=7||stageControl.pending()!=7||stageControl.gateSet()
        ||!messagesConfigured||!playerConfigured||!playerShotGroupsConfigured
        ||debugRunTainted||debugInfiniteRespawnRun||debugRecordPrefix.worldBound||replayPlayback.active()
        ||!normalRecord||!normalReplay||worldStartDifficulty<0||worldStartDifficulty>3
        ||originalContinueCount<0||originalContinueCount>9)
        throw std::runtime_error("ordinary terminal late binding lacks its actual initialized native session");
    if(!stageCarry.preInitBound||stageCarry.restoredGeneration!=worldGeneration
        ||stageCarry.scalarRestoredGeneration!=worldGeneration||stageCarry.nextId!=stage
        ||stageCarry.nextProgress!=currentStageProgress||stageCarry.targetWorldName!=worldStartedName
        ||stageIdForEcl(stageCarry.sourceWorldName)!=stageCarry.sourceCurrentId
        ||stageCarry.sourceDifficulty!=worldStartDifficulty||stageCarry.difficulty!=worldStartDifficulty
        ||stageCarry.targetDifficulty!=worldStartDifficulty||stageCarry.originalContinueCount!=originalContinueCount
        ||stageCarry.sourceControlSessionArgument!=0)
        throw std::runtime_error("ordinary terminal late binding differs from its closed restored carry");
    th128::record::OrdinaryTerminalBinding binding;
    binding.generation=worldGeneration;binding.worldFrame=world->frame();binding.stage=unsigned(stage);
    binding.rank=unsigned(worldStartDifficulty);binding.continues=unsigned(originalContinueCount);
    binding.runFlags=playerHit->snapshot().soundControl.flags60;
    binding.controllerCurrent=stageControl.current();binding.controllerPending=stageControl.pending();
    binding.stageGate=stageControl.gateSet();
    binding.sessionPresent=ordinaryCarrySessionPresent(true);
    binding.sessionField74=stageCarry.sourceControlSessionArgument;
    binding.messagesConfigured=messagesConfigured;binding.playerConfigured=playerConfigured;
    binding.debugTainted=debugRunTainted;binding.replayPlayback=replayPlayback.active();
    binding.directDiagnostic=debugRecordPrefix.worldBound||debugInfiniteRespawnRun;
    binding.record=normalRecord;binding.replay=normalReplay;
    auto& carry=binding.carry;carry.token=stageCarry.token;carry.sourceGeneration=stageCarry.sourceGeneration;
    carry.targetGeneration=stageCarry.scalarRestoredGeneration;carry.sourceStage=unsigned(stageCarry.sourceCurrentId);
    carry.targetStage=unsigned(stageCarry.nextId);carry.rank=unsigned(stageCarry.targetDifficulty);
    carry.continues=unsigned(stageCarry.originalContinueCount);carry.valid=stageCarry.valid;
    carry.targetBound=stageCarry.targetBound;carry.gameRandomRestored=stageCarry.gameRandomRestored;
    carry.scalarRestored=stageCarry.scalarRestored;carry.acknowledged=stageCarry.acknowledged;carry.consumed=stageCarry.consumed;
    carry.record=stageCarry.normalRecord;carry.replay=stageCarry.normalReplay;
    auto owner=std::make_unique<th128::record::NativeOrdinaryTerminalOwner>();
    owner->bind(binding);
    if(!world->bindOrdinaryTerminalDrain(*owner,stage,currentStageProgress,
        [](unsigned generation,unsigned stage,int progress,std::string& error){
            return ordinaryTerminalDrainReady(generation,stage,progress,error);
        },operationError))throw std::runtime_error(operationError);
    normalTerminal.owner=std::move(owner);
}

bool beginOrdinaryTerminalMsg21(const th128::message::Event& event,const th128::stage::StageSettlementCallbacks& callbacks,std::string& error){
    error.clear();
    try{
        if(!world||world->faulted()||!worldTickExecuting||!playerHit||!normalRecord||!normalReplay
            ||!normalTerminal.resourcesConfigured||!normalTerminal.owner||!normalTerminal.settlement||normalTerminal.begun
            ||normalTerminal.generation!=worldGeneration||normalTerminal.stage!=stageIdForEcl(worldStartedName)
            ||worldStartDifficulty<0||worldStartDifficulty>3||originalContinueCount<0||originalContinueCount>9
            ||!messagesConfigured||!playerConfigured||!playerShotGroupsConfigured||debugRunTainted||debugInfiniteRespawnRun
            ||debugRecordPrefix.worldBound||replayPlayback.active()||pointValueRaw<0||motivationRaw<0||motivationRaw>100000)
            throw std::runtime_error("ordinary MSG21 lacks its bound actual terminal session");
        th128::record::NativeOrdinaryTerminalOwner::Event claim;
        claim.generation=worldGeneration;claim.worldFrame=world->frame();claim.stage=unsigned(stageIdForEcl(worldStartedName));
        claim.rank=unsigned(worldStartDifficulty);claim.continues=unsigned(originalContinueCount);
        claim.runFlags=playerHit->snapshot().soundControl.flags60;claim.messageEntry=unsigned(event.entry);claim.opcode=unsigned(event.opcode);
        claim.worldTickExecuting=worldTickExecuting;claim.controllerCurrent=stageControl.current();claim.controllerPending=stageControl.pending();
        claim.stageGate=stageControl.gateSet();claim.sessionPresent=ordinaryCarrySessionPresent(true);
        claim.sessionField74=stageCarry.sourceControlSessionArgument;claim.messagesConfigured=messagesConfigured;claim.playerConfigured=playerConfigured;
        claim.debugTainted=debugRunTainted;claim.replayPlayback=replayPlayback.active();claim.directDiagnostic=debugRecordPrefix.worldBound||debugInfiniteRespawnRun;
        claim.record=normalRecord;claim.replay=normalReplay;
        normalTerminal.owner->claim(claim);
        th128::stage::StageSettlementContext context;
        context.stage=normalTerminal.stage;context.score=scoreRaw;context.manager=stageManager;
        if(!normalTerminal.settlement->beginTerminal(context,callbacks,error))return false;
        auto committed=normalTerminal.settlement->state();
        if(!committed.begun||!committed.scalarApplied||!committed.cleanupCalled
            ||worldGeneration!=claim.generation||world->frame()!=claim.worldFrame||world->faulted()||!worldTickExecuting
            ||normalRecord!=claim.record||normalReplay!=claim.replay||worldStartDifficulty!=int(claim.rank)
            ||originalContinueCount!=int(claim.continues)||stageIdForEcl(worldStartedName)!=int(claim.stage)
            ||stageControl.current()!=claim.controllerCurrent||stageControl.pending()!=claim.controllerPending||stageControl.gateSet()
            ||debugRunTainted||debugInfiniteRespawnRun||debugRecordPrefix.worldBound||replayPlayback.active()
            ||!messagesConfigured||!playerConfigured||!playerShotGroupsConfigured)
            throw std::runtime_error("ordinary MSG21 settlement changed its native terminal identity");
        const std::array<int,4> multipliers{5000,10000,20000,40000};
        const int quotient=pointValueRaw/100;
        const auto bonusBits=std::uint32_t(std::int64_t(quotient-quotient%10)*100
            +std::int64_t(motivationRaw/100)*multipliers[std::size_t(worldStartDifficulty)]);
        const int bonus=std::bit_cast<std::int32_t>(bonusBits);
        int paid=std::bit_cast<std::int32_t>(std::uint32_t(committed.context.score)+std::uint32_t(bonus/10));
        if(paid>=1000000000)paid=999999999;
        auto manager=committed.context.manager;manager.flags5a04|=0x18u;
        manager.baseScore5a38=std::bit_cast<std::int32_t>(std::uint32_t(manager.baseScore5a38)+bonusBits);
        normalTerminal.owner->commitRecord(claim);
        scoreRaw=paid;stageManager=manager;normalTerminal.bonus=bonus;normalTerminal.counter=0;
        normalTerminal.selector=int(normalTerminal.owner->identity().settledSelector);normalTerminal.begun=true;return true;
    }catch(const std::exception& failure){error=failure.what();return false;}
}

bool ordinaryTerminalContinuationReady(std::string& error){
    if(!world||world->faulted()||!worldTickExecuting||!normalTerminal.resourcesConfigured
        ||!normalTerminal.begun||!normalTerminal.owner||!normalTerminal.owner->bound()
        ||!normalTerminal.owner->recorded()||!normalTerminal.settlement||!normalRecord||!normalReplay
        ||!playerHit||!messagesConfigured||!playerConfigured||!playerShotGroupsConfigured
        ||debugRunTainted||debugInfiniteRespawnRun||debugRecordPrefix.worldBound||replayPlayback.active()
        ||debugTerminal.begun||stageTransitionConfigured||normalTerminal.requested
        ||!ordinaryCarrySessionPresent(true)
        ||stageControl.current()!=7||stageControl.pending()!=7||stageControl.gateSet()
        ||stageCarry.sourceControlSessionArgument!=0||!world->frame()
        ||!normalTerminal.owner->matchesContinuation(worldGeneration,unsigned(originalContinueCount),normalRecord,normalReplay)){
        error="ordinary terminal continuation lacks its claimed actual session";return false;
    }
    const auto& identity=normalTerminal.owner->identity();
    if(normalTerminal.generation!=worldGeneration||identity.generation!=worldGeneration
        ||normalTerminal.stage!=stageIdForEcl(worldStartedName)||normalTerminal.stage!=int(identity.stage)
        ||normalTerminal.selector!=int(identity.settledSelector)||worldStartDifficulty!=int(identity.rank)
        ||!(playerHit->snapshot().gate.flags438ec&th128::player::kPlayerStageClearFlag)||!normalRecord->started()
        ||normalRecord->prepared()||normalRecord->acknowledged()||normalRecord->stage()!=identity.stage
        ||normalRecord->route()!=identity.route||normalRecord->rank()!=identity.rank
        ||!normalReplay->configuredFor(identity.stage)||normalReplay->stages()!=3
        ||normalReplay->route()!=identity.route||normalReplay->rank()!=identity.rank
        ||normalRecord->frames()!=normalReplay->frames()||normalRecord->frames()<identity.nativeFrames){
        error="ordinary terminal continuation changed its recorded identity";return false;
    }
    return true;
}

bool ordinaryTerminalMessageDone(const th128::message::Event& event,std::string& error){
    if(event.opcode!=0||event.entry!=1||normalTerminal.messageDone){
        error="ordinary terminal MSG0 completion is out of order";return false;
    }
    if(!ordinaryTerminalContinuationReady(error))return false;
    normalTerminal.messageDone=true;return true;
}

bool ordinaryTerminalDrainReady(unsigned generation,unsigned stage,int progress,std::string& error){
    error.clear();
    if(!world||world->faulted()||!worldTickExecuting||!normalTerminal.resourcesConfigured
        ||!normalTerminal.begun||!normalTerminal.owner||!normalTerminal.settlement
        ||generation!=worldGeneration||normalTerminal.generation!=generation
        ||normalTerminal.stage!=int(stage)||stageIdForEcl(worldStartedName)!=int(stage)
        ||normalTerminal.progress!=progress||currentStageProgress!=progress
        ||!normalRecord||!normalReplay||!playerHit||!messagesConfigured||!playerConfigured||!playerShotGroupsConfigured
        ||debugRunTainted||debugInfiniteRespawnRun||debugRecordPrefix.worldBound||replayPlayback.active()
        ||!normalTerminal.owner->matchesContinuation(generation,unsigned(originalContinueCount),normalRecord,normalReplay)
        ||!normalTerminal.messageDone||messages->state().active||messages->state().entry!=1
        ||!messages->state().eclReleased||!stageControl.initialized()||!stageControl.sessionConstructed()
        ||!ordinaryCarrySessionPresent(true)||stageControl.current()!=7||stageControl.gateSet()
        ||stageCarry.sourceControlSessionArgument!=0||normalRecord->prepared()||normalRecord->acknowledged()
        ||normalRecord->frames()!=normalReplay->frames()){
        error="ordinary terminal World drain differs from its actual completed native session";return false;
    }
    const auto settlement=normalTerminal.settlement->state();
    if(!settlement.begun||settlement.failed||!settlement.scalarApplied||!settlement.cleanupCalled){
        error="ordinary terminal World drain has no successful native settlement";return false;
    }
    const auto pending=stageControl.pending();
    if(!normalTerminal.requested){
        if(pending==7)return true;
    }else if(pending==15&&normalTerminal.counter==380&&normalTerminal.runtimeScene==15
        &&normalTerminal.lastTickFrame==world->frame()+1)return true;
    error="ordinary terminal World drain pending controller changed";return false;
}

th128::ending::FadeControls ordinaryGlobalTaskControls(){
    const auto* ending=activeEndingRuntime();
    const unsigned pause=originalPauseMenu&&originalPauseMenu->state().active?1u:ending?ending->pauseWord():0u;
    if(playerConfigured&&playerHit){const auto actual=playerHit->snapshot().soundControl;
        return {pause,actual.present,actual.flags60,ordinaryGlobalTaskRuntime.delta};}
    return {pause,false,0,ordinaryGlobalTaskRuntime.delta};
}
void projectOrdinaryGlobalFade(){
    normalTerminal.fadeElapsed=normalTerminal.transition.elapsed();normalTerminal.fadeAlpha=normalTerminal.transition.alpha();
}
bool tickOrdinaryTerminal(std::string& error){
    if(!ordinaryTerminalContinuationReady(error))return false;
    const auto completedFrame=unsigned(world->frame()+1);
    if(completedFrame<=normalTerminal.lastTickFrame||normalTerminal.counter<0||normalTerminal.counter>=380){
        error="ordinary terminal counter requires one actual manager callback per frame";return false;
    }
    if(!normalTerminal.settlement->tick(error))return false;
    normalTerminal.lastTickFrame=completedFrame;
    stageManager.field5a08=stageManager.field5a0c;
    ++stageManager.field5a0c;stageManager.field5a10=float(stageManager.field5a0c);
    ++normalTerminal.counter;
    if(normalTerminal.counter==180&&!normalTerminal.transition.begin(*normalTerminal.owner,worldGeneration,
        unsigned(originalContinueCount),normalRecord,normalReplay,completedFrame,error))return false;
    projectOrdinaryGlobalFade();
    if(normalTerminal.counter==380){
        if(!normalTerminal.messageDone||!normalTerminal.musicSerial||!musicControl.active||!musicControl.acknowledged
            ||musicControl.operation!=5||musicControl.serial!=normalTerminal.musicSerial){
            error="ordinary ending requires actual MSG completion and its consumed music fade";return false;
        }
        const auto requested=stageControl.request(15);
        if(requested.status!=th128::stage::RequestStatus::Written||requested.pending!=15){
            error="ordinary ending request was not accepted by the actual controller";return false;
        }
        if(!normalTerminal.transition.requestEnding(worldGeneration,completedFrame,error))return false;
        normalTerminal.runtimeScene=15;normalTerminal.requested=true;
    }
    return true;
}

template<class Writer>
bool writeVisualRecord(Writer& text,bool& comma,std::uint32_t identifier,int bank,
                       const th128::anm::State& state,const th128::anm::SpriteInfo& sprite,float pixelX,float pixelY,float pixelZ,
                       bool unifiedEffect=false,const char* surface=nullptr,const th128::geometry526::MeshSnapshot* mesh=nullptr,
                       const std::vector<th128::geometry::ColorVertex>* fan=nullptr,const char* drawPass=nullptr,
                       const std::vector<th128::geometry::RingVertex>* ring=nullptr) {
    const auto width=state.spriteWidth<0?sprite.width:state.spriteWidth;
    const auto height=state.spriteHeight<0?sprite.height:state.spriteHeight;
    const std::array scalars{width,height,pixelX,pixelY,pixelZ,state.rotation,state.sx,state.sy,state.u,state.v,state.uvScaleX,state.uvScaleY,
        sprite.u0,sprite.v0,sprite.u1,sprite.v1,state.rx,state.ry};
    if(!std::all_of(scalars.begin(),scalars.end(),[](float value){return std::isfinite(value);})) {
        presentationError="Non-finite world presentation value";return false;
    }
    if(mesh){
        if(mesh->vertices.size()!=34){presentationError="Invalid original526 row extent";return false;}
        for(const auto& vertex:mesh->vertices){
            const std::array vertexScalars{vertex.x,vertex.y,vertex.z,vertex.rhw,vertex.u,vertex.v};
            if(!std::all_of(vertexScalars.begin(),vertexScalars.end(),[](float value){return std::isfinite(value);})){ 
                presentationError="Non-finite original526 vertex";return false;
            }
        }
    }
    if(comma)text<<',';comma=true;
    text<<"{\"id\":"<<identifier<<",\"parent\":0,\"bank\":"<<bank<<",\"sprite\":"<<state.sprite
        <<",\"effectSystem\":\""<<(unifiedEffect?"unified-primary":"other")<<"\""
        <<",\"entry\":"<<sprite.textureEntry<<",\"layer\":"<<state.layer<<",\"mode\":"<<state.mode
        <<",\"anchorX\":"<<state.anchorX<<",\"anchorY\":"<<state.anchorY<<",\"blend\":"<<state.blend
        <<",\"flags\":"<<state.flags<<",\"flags2\":"<<state.flags2<<",\"color\":"<<state.primary<<",\"secondary\":"<<state.secondary
        <<",\"script\":"<<state.script<<",\"clock\":"<<state.clock<<",\"visualTicks\":"<<state.ticks
        <<",\"width\":"<<width<<",\"height\":"<<height<<",\"x\":"<<pixelX<<",\"y\":"<<pixelY<<",\"z\":"<<pixelZ
        <<",\"rotation\":"<<state.rotation<<",\"sx\":"<<state.sx<<",\"sy\":"<<state.sy<<",\"u\":"<<state.u<<",\"v\":"<<state.v
        <<",\"uvScaleX\":"<<state.uvScaleX<<",\"uvScaleY\":"<<state.uvScaleY<<",\"u0\":"<<sprite.u0<<",\"v0\":"<<sprite.v0
        <<",\"u1\":"<<sprite.u1<<",\"v1\":"<<sprite.v1<<",\"rx\":"<<state.rx<<",\"ry\":"<<state.ry;
    if(surface)text<<",\"stageSurface\":\""<<surface<<'"';
    if(drawPass)text<<",\"drawPass\":\""<<drawPass<<'"';
    if(mesh){
        text<<",\"geometry\":{\"kind\":\"triangle-strip\",\"owner\":"<<mesh->owner<<",\"column\":"<<mesh->column<<",\"vertices\":[";
        bool vertexComma=false;
        for(const auto& vertex:mesh->vertices){
            if(vertexComma)text<<',';vertexComma=true;
            text<<"{\"x\":"<<vertex.x<<",\"y\":"<<vertex.y<<",\"z\":"<<vertex.z<<",\"rhw\":"<<vertex.rhw
                <<",\"color\":"<<vertex.color<<",\"u\":"<<vertex.u<<",\"v\":"<<vertex.v<<'}';
        }
        text<<"]}";
    }
    if(fan){
        text<<",\"geometry\":{\"kind\":\""<<(state.mode==17?"circle-line":"circle-fan")<<"\",\"vertices\":[";bool vertexComma=false;
        for(const auto& vertex:*fan){if(vertexComma)text<<',';vertexComma=true;text<<"{\"x\":"<<vertex.x<<",\"y\":"<<vertex.y<<",\"color\":"<<vertex.color<<'}';}
        text<<"]}";
    }
    if(ring){
        text<<",\"geometry\":{\"kind\":\"anm-ring\",\"vertices\":[";bool vertexComma=false;
        for(const auto& vertex:*ring){if(vertexComma)text<<',';vertexComma=true;
            text<<"{\"x\":"<<vertex.x<<",\"y\":"<<vertex.y<<",\"z\":"<<vertex.z<<",\"rhw\":"<<vertex.rhw
                <<",\"color\":"<<vertex.color<<",\"u\":"<<vertex.u<<",\"v\":"<<vertex.v<<'}';}
        text<<"]}";
    }
    text<<'}';
    return true;
}

bool applyGrazeAt(th128::graze::Position position,std::string& error) {
    if(!grazeConfigured){error="graze globals/effect host are not configured";return false;}
    const auto plan=th128::graze::planGraze(grazeGlobals,position);
    if(!plan||grazeSounds.size()>65533){error="unsupported graze state or sound-request budget";return false;}
    grazeGlobals=plan->updated;
    for(const auto& request:plan->requests()) {
        if(const auto* sound=std::get_if<th128::graze::ThresholdSoundRequest>(&request.payload))
            grazeSounds.push_back({sound->soundId,0});
        else if(const auto* effect=std::get_if<th128::graze::EffectRequest>(&request.payload)) {
            if(!cancellationEffects->spawn(effect->scriptId,320+effect->position.horizontal,16+effect->position.vertical,effect->position.depth,effect->layer,error))return false;
        } else {
            const auto& sound=std::get<th128::graze::PositionalSoundRequest>(request.payload);
            grazeSounds.push_back({sound.soundId,sound.positionX});
        }
    }
    return true;
}
bool applyGraze(const th128::bullet::Birth& bullet,std::string& error) {
    return applyGrazeAt({bullet.state.position.x,bullet.state.position.y,bullet.state.position.z},error);
}

bool nameArgument(const char* bytes,std::uint32_t length,std::string& name) {
    if(length==0||length>200||!th128::runtime::inputRange(bytes,length))return false;
    name.assign(bytes,bytes+length);
    for(const auto character:name)
        if(!((character>='a'&&character<='z')||(character>='A'&&character<='Z')||(character>='0'&&character<='9')
            ||character=='_'||character=='.'||character=='-'))return false;
    return name.find("..") == std::string::npos;
}

template<class Writer>
bool writeStageIntroDraws(Writer& text,bool& comma) {
    if(!stageIntro||!stageIntro->loaded())return true;
    const auto& draws=stageIntro->draw();
    for(const auto& record:draws) {
        const std::array scalars{record.width,record.height,record.x,record.y,record.z,record.rotation,
            record.sx,record.sy,record.u,record.v,record.uvScaleX,record.uvScaleY,record.u0,record.v0,
            record.u1,record.v1,record.rx,record.ry,record.radius,record.geometryWidth};
        if(record.bank!=th128::stageintro::Host::LogoBank||record.sprite<0||record.entry<0
            ||record.geometryCount!=0||record.id>0x00ffffffu
            ||!std::all_of(scalars.begin(),scalars.end(),[](float value){return std::isfinite(value);})) {
            if(stageIntroDrawError.empty())stageIntroDrawError="Invalid original stage logo draw record";
            return false;
        }
        if(comma)text<<',';comma=true;
        text<<"{\"id\":"<<(50000000u+record.id)<<",\"parent\":"<<(record.parent?50000000u+record.parent:0u)
            <<",\"bank\":"<<record.bank<<",\"sprite\":"<<record.sprite<<",\"entry\":"<<record.entry
            <<",\"layer\":"<<record.layer<<",\"mode\":"<<record.mode<<",\"anchorX\":"<<record.anchorX
            <<",\"anchorY\":"<<record.anchorY<<",\"blend\":"<<record.blend
            <<",\"flags\":"<<record.flags<<",\"flags2\":"<<record.flags2
            <<",\"color\":"<<record.primary<<",\"secondary\":"<<record.secondary
            <<",\"width\":"<<record.width<<",\"height\":"<<record.height
            <<",\"x\":"<<record.x<<",\"y\":"<<record.y<<",\"z\":"<<record.z
            <<",\"rotation\":"<<record.rotation<<",\"sx\":"<<record.sx<<",\"sy\":"<<record.sy
            <<",\"u\":"<<record.u<<",\"v\":"<<record.v
            <<",\"uvScaleX\":"<<record.uvScaleX<<",\"uvScaleY\":"<<record.uvScaleY
            <<",\"u0\":"<<record.u0<<",\"v0\":"<<record.v0<<",\"u1\":"<<record.u1<<",\"v1\":"<<record.v1
            <<",\"rx\":"<<record.rx<<",\"ry\":"<<record.ry<<",\"geometryRadius\":"<<record.radius
            <<",\"geometryWidth\":"<<record.geometryWidth<<",\"geometryCount\":"<<record.geometryCount
            <<",\"script\":"<<record.script<<",\"clock\":"<<record.clock<<",\"order\":"<<record.order<<'}';
    }
    return true;
}
}

extern "C" {
int th128_world_load_stage_intro(int stageId,const char* logo,std::uint32_t logoLength,const void* data,std::uint32_t size) {
    operationError.clear();
    std::string logoName;
    if(!world||world->faulted()||world->frame()!=0||world->spawnCount()!=0
        ||worldStartedName.empty()||stageIdForEcl(worldStartedName)!=stageId||stageId<1||stageId>16
        ||!nameArgument(logo,logoLength,logoName)||!th128::runtime::inputRange(data,size)||size==0
        ||size>32u*1024u*1024u||stageIntro->loaded()){
        operationError="stage intro requires the healthy frame0 stage identity, exact logo and bounded ANM bytes";
        stageIntroDrawError=operationError;
        return 0;
    }
    std::string error;
    const auto stageName=std::to_string(stageId);
    if(!stageIntro->load(stageName,logoName,static_cast<const std::uint8_t*>(data),size,error)){
        operationError=error.empty()?"stage intro load rejected":error;
        stageIntroDrawError=operationError;
        return 0;
    }
    stageIntroDrawError.clear();
    return 1;
}
int th128_world_track_caption(int trackFile,int role) {
    operationError.clear();
    if(!world||world->faulted()||worldTickExecuting||stageIntro->loaded()==false
        ||worldStartedName.empty()||stageIntro->stage()!=std::to_string(stageIdForEcl(worldStartedName))) {
        operationError=!stageIntro->loaded()?"stage intro bank is not loaded":
            "stage intro caption requires a healthy loaded current stage";
        stageIntroDrawError=operationError;
        return 0;
    }
    if(!stageIntro->error().empty()){
        operationError=stageIntro->error();
        stageIntroDrawError=operationError;
        return 0;
    }
    const auto caption=stageIntro->captionInfo();
    if(caption.active&&caption.trackFile==trackFile&&caption.role==role)return 1;
    std::string error;
    if(!stageIntro->beginTrackCaption(trackFile,role,error)){
        operationError=error.empty()?"stage track caption rejected":error;
        stageIntroDrawError=operationError;
        return 0;
    }
    return 1;
}
const char* th128_world_stage_intro_state() {
    std::ostringstream text;
    text<<std::setprecision(std::numeric_limits<float>::max_digits10);
    const auto caption=stageIntro->captionInfo();
    text<<"{\"loaded\":"<<(stageIntro->loaded()?"true":"false")
        <<",\"introActive\":"<<(stageIntro->introActive()?"true":"false")
        <<",\"captionActive\":"<<(stageIntro->captionActive()?"true":"false")
        <<",\"frame\":"<<stageIntro->frame()
        <<",\"stageId\":"<<(stageIntro->stage().empty()?0:std::stoi(stageIntro->stage()))
        <<",\"logo\":"<<std::quoted(stageIntro->logo())
        <<",\"firstStage\":"<<(stageIntro->firstStage()?"true":"false")
        <<",\"error\":"<<std::quoted(stageIntro->error())
        <<",\"caption\":{\"trackFile\":"<<caption.trackFile<<",\"role\":"<<caption.role
        <<",\"atlasRow\":"<<caption.atlasRow<<",\"script\":"<<caption.script
        <<",\"sprite\":"<<caption.sprite<<",\"entry\":"<<caption.entry<<",\"clock\":"<<caption.clock
        <<",\"x\":"<<caption.x<<",\"y\":"<<caption.y
        <<",\"active\":"<<(caption.active?"true":"false")<<"},";
    std::ostringstream draws;
    bool comma=false;
    const bool drawsValid=writeStageIntroDraws(draws,comma);
    text<<"\"drawCount\":"<<(drawsValid&&stageIntro->loaded()?stageIntro->draw().size():0)
        <<",\"draws\":"<<(drawsValid?"["+draws.str()+"]":"[]")<<'}';
    snapshot=text.str();
    return snapshot.c_str();
}
const char* th128_world_stage_intro_draw() {
    std::ostringstream text;text<<'[';bool comma=false;
    if(!writeStageIntroDraws(text,comma)){snapshot="[]";return snapshot.c_str();}
    text<<']';snapshot=text.str();return snapshot.c_str();
}
const char* th128_world_stage_intro_draw_error() {
    return stageIntroDrawError.empty()?stageIntro->error().c_str():stageIntroDrawError.c_str();
}
void th128_world_reset() {
    replayPlayback.cancel();
    if(ownedBattle.attempted()){operationError="retired battle manager cannot be reset through the old gameplay API";return;}
    battleEndingRuntime15=std::make_unique<th128::ending::Runtime15>();ordinaryDetachedEnding.reset();endingRuntimeSnapshot.clear();
    beginNewWorldGeneration();
    spellVisuals=std::make_unique<th128::spell::Visuals>();spellsConfigured=false;spellResult={};spellResultFrames=0;
    messages=std::make_unique<th128::message::Host>();messageVisuals=std::make_unique<th128::message::Visuals>();messagesConfigured=false;messageFrontConfigured=false;messageMusicSerial=0;resetRouteChoice();resetStageTransition();resetMusicControl();
    world=std::make_unique<th128::StageWorld>();laserLines=std::make_unique<th128::laser::LineHost>();operationError.clear();snapshot.clear();
    bulletAnimations.clear();bulletVisuals=std::make_unique<th128::bullet::BulletVisuals>();bulletDraws.clear();
    enemyAppearances.clear();bulletProfiles.clear();
    presentationError.clear();
    playerProfileRadius=0;
    playerAttractionSpeed=0;
    grazeConfigured=false;grazeSounds.clear();grazeGlobals={0,0};advanceSoundJournalReset();
    enemyVisuals=std::make_unique<th128::enemy::EnemyVisuals>();
    cancellationEffects=std::make_unique<th128::effects::CancellationEffects>();
    playerHit=std::make_unique<th128::player::PlayerHitHost>();playerConfigured=false;
    playerShots=std::make_unique<th128::playershot::PlayerShotHost>();
    playerShotGroups.reset();playerShotGroupsConfigured=false;
    itemHost=std::make_unique<th128::item::ItemHost>();enemiesDefeated=0;
    scoreRaw=0;pointValueRaw=100000;itemScalar=0;scorePopups.clear();
    playerShotsConfigured=false;cumulativeFrozenArea=0;shotDifficulty=1;playerDamageSourceFlags.fill(0);
    damageFields=std::make_unique<th128::damagefield::DamageFieldPool>();
    perfectHud=std::make_unique<th128::hud::PerfectHud>();perfectHudConfigured=false;bossHud=std::make_unique<th128::hud::BossHud>();standaloneEffectOwner=1000000;standaloneEffectOwners.clear();
    geometryEffects=std::make_unique<th128::geometry526::Host>();geometryConfigured=false;
    playerRespawnConfigured=false;respawnsCommitted=0;respawnControl={};respawnSavedPosition={0,0,0};emptyLaserCancelCenter={};
    shotHitsConfigured=false;motivationRaw=motivationFlag=shotHitBusy=shotDamageBypass=0;shotSpellFlags=0;
    rapidShotConfigured=false;rapidShotCount=-1;
    playerFire=std::make_unique<th128::player::fire::Host>();iceHost=std::make_unique<th128::ice::Host>();
    perfectHost=std::make_unique<th128::perfect::Host>();perfectConfigured=false;perfectConfigError.clear();
    originalPauseMenu.reset();originalPauseSoundCursor=0;originalContinueCount=0;originalPauseBackdropBytes.clear();
    normalReplay.reset();normalReplaySave.reset();normalReplayNameContext={};normalReplayBytes.clear();normalReplayClockPending=false;normalReplayOrigin=normalReplayClock=0;normalRecord.reset();
    perfectBatchOwned=false;perfectZDenied.clear();
    perfectModeOwned=false;
    debugInfiniteRespawnRun=false;debugRunTainted=false;debugReviveRefills=0;debugLastRefillBefore=0;debugLastRefillAfter=0;
    playerFireConfigured=false;worldDisplayRandom={};lastFrozenArea=0;freezeBonus=0;freezeRewardPanel={};
    playerDeathConfigured=false;deathsCommitted=0;deathOperation.reset();
    playerMotion=false;playerRaw={0,51200};playerInput={};
    headPowerConfigured=false;
    focusIndicator=std::make_unique<th128::player::FocusIndicator>();focusIndicatorConfigured=false;playerFocused=false;
}
const char* th128_world_error() {return world->error().empty()?operationError.c_str():world->error().c_str();}
int th128_world_load(const char* name,std::uint32_t nameLength,const void* data,std::uint32_t size) {
    operationError.clear();std::string identifier;
    if(!nameArgument(name,nameLength,identifier)||!th128::runtime::inputRange(data,size)) {
        operationError="Invalid world resource input span";return 0;
    }
    return world->load(identifier,data,size,operationError)?1:0;
}
int th128_world_initialize_stage_control(unsigned existingFlags) {
    operationError.clear();
    if(stageControl.initialized()){operationError="stage control is already initialized";return 0;}
    stageControl.initialize(existingFlags);
    return 1;
}
int th128_world_prepare_debug_record_prefix(const void* data,unsigned bytes,unsigned seed,unsigned calls,unsigned epoch) {
    operationError.clear();
    if(debugRecordPrefix.prepared||worldGeneration!=0||!worldStartedName.empty()
        ||worldTickExecuting||debugSetterExecuting||!epoch||seed>65535||bytes!=268
        ||!th128::runtime::inputRange(data,bytes)){
        operationError="debug record prefix requires a fresh owned pre-World session";return 0;
    }
    try{
        DebugRecordPrefixState candidate;
        candidate.epoch=epoch;candidate.initialSeed=seed;candidate.initialCalls=calls;
        candidate.random.seed=std::uint16_t(seed);candidate.random.calls=calls;
        const auto native=std::span<const std::uint8_t>(static_cast<const std::uint8_t*>(data),bytes);
        candidate.profile=th128::record::NativeRecordBootstrap::create(
            candidate.random,native.first(250),native.subspan(250,9),native.subspan(259,9));
        candidate.bootstrapSeed=candidate.random.seed;candidate.bootstrapCalls=candidate.random.calls;
        candidate.prepared=true;debugRecordPrefix=std::move(candidate);return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
int th128_world_prepare_extra_record_profile(const void* data,unsigned bytes,unsigned seed,unsigned calls,unsigned epoch){
    operationError.clear();if(debugRecordPrefix.prepared||worldGeneration!=0||!worldStartedName.empty()||worldTickExecuting||debugSetterExecuting
        ||!epoch||seed>65535||bytes!=52232||!th128::runtime::inputRange(data,bytes)){operationError="EX complete profile requires a fresh actual pre-World owner";return 0;}
    try{const auto source=std::span<const std::uint8_t>(static_cast<const std::uint8_t*>(data),bytes);th128::record::NativeScoreProfile::assemble(source);
        if(source[13172]!=0)throw std::runtime_error("EX complete profile name is not terminated");
        DebugRecordPrefixState candidate;candidate.profile.assign(source.begin(),source.end());candidate.epoch=epoch;candidate.initialSeed=seed;candidate.initialCalls=calls;
        candidate.random.seed=std::uint16_t(seed);candidate.random.calls=calls;candidate.bootstrapSeed=seed;candidate.bootstrapCalls=calls;candidate.prepared=true;
        debugRecordPrefix=std::move(candidate);return 1;
    }catch(const std::exception& e){operationError=e.what();return 0;}
}
int th128_world_prepare_ordinary_record_prefix(const void* data,unsigned bytes,
        unsigned seed,unsigned calls,unsigned epoch,unsigned stage,unsigned rank) {
    operationError.clear();
    static constexpr std::array<unsigned,6> stages{3,5,8,10,13,15};
    const auto selected=std::find(stages.begin(),stages.end(),stage);
    if(ordinaryRecordOwner.configured||debugRecordPrefix.prepared||worldGeneration!=0
        ||selected==stages.end()||rank>=4||!epoch){
        operationError="ordinary pre-World route/stage/rank owner rejected";return 0;
    }
    if(!th128_world_prepare_debug_record_prefix(data,bytes,seed,calls,epoch))return 0;
    ordinaryRecordOwner.configured=true;ordinaryRecordOwner.stage=stage;
    ordinaryRecordOwner.route=unsigned(selected-stages.begin());ordinaryRecordOwner.rank=rank;
    ordinaryRecordOwner.epoch=epoch;return 1;
}
int th128_world_bind_debug_record_clock(unsigned epoch,unsigned filetimeLow,unsigned filetimeHigh,double seconds) {
    operationError.clear();
    if(!debugRecordPrefix.prepared||debugRecordPrefix.failed||debugRecordPrefix.captureApplied
        ||debugRecordPrefix.worldBound||debugRecordPrefix.clockBound||debugRecordPrefix.epoch!=epoch
        ||worldGeneration!=0||!worldStartedName.empty()){
        operationError="debug record clock epoch or lifecycle rejected";return 0;
    }
    const auto filetime=(std::uint64_t(filetimeHigh)<<32)|std::uint64_t(filetimeLow);
    const auto timestamp=th128::record::NativeRecordTiming::time64FromFileTime(filetime);
    if(timestamp==UINT64_MAX||!std::isfinite(seconds)||seconds<0){
        operationError="debug record clock provider sample is invalid";return 0;
    }
    debugRecordPrefix.clockTime64=timestamp;debugRecordPrefix.clockFirstSeconds=seconds;
    debugRecordPrefix.clockBound=true;return 1;
}
const char* th128_world_debug_record_state() {
    std::ostringstream output;
    output<<std::setprecision(17)<<std::boolalpha
        <<R"({"present":)"<<debugRecordPrefix.prepared
        <<R"(,"prepared":)"<<debugRecordPrefix.prepared
        <<R"(,"clockBound":)"<<debugRecordPrefix.clockBound
        <<R"(,"captureApplied":)"<<debugRecordPrefix.captureApplied
        <<R"(,"worldBound":)"<<debugRecordPrefix.worldBound
        <<R"(,"failed":)"<<debugRecordPrefix.failed
        <<R"(,"epoch":)"<<debugRecordPrefix.epoch
        <<R"(,"freshRecordFieldsApplied":)"<<debugRecordPrefix.freshRecordFieldsApplied
        <<R"(,"highScoreRaw":)"<<debugRecordPrefix.highScoreRaw
        <<R"(,"highScoreContinue":)"<<debugRecordPrefix.highScoreContinue
        <<R"(,"playCount":)"<<debugRecordPrefix.playCount
        <<R"(,"sessionPresent":)"<<debugRecordPrefix.sessionPresent
        <<R"(,"sessionFlags60":)"<<debugRecordPrefix.sessionFlags60
        <<R"(,"sessionField74":)"<<debugRecordPrefix.sessionField74
        <<R"(,"replayMode":)"<<debugRecordPrefix.replayMode
        <<R"(,"sessionGeneration":)"<<debugRecordPrefix.sessionGeneration
        <<R"(,"activeCallbacks":)"<<debugRecordPrefix.activeCallbacks
        <<R"(,"clearRecordEvents":)"<<debugRecordPrefix.clearRecordEvents
        <<R"(,"generation":)"<<debugRecordPrefix.generation
        <<R"(,"initialSeed":)"<<debugRecordPrefix.initialSeed
        <<R"(,"initialCalls":)"<<debugRecordPrefix.initialCalls
        <<R"(,"bootstrapSeed":)"<<debugRecordPrefix.bootstrapSeed
        <<R"(,"bootstrapCalls":)"<<debugRecordPrefix.bootstrapCalls
        <<R"(,"capturedSeed":)"<<debugRecordPrefix.capturedSeed
        <<R"(,"capturedPriorCalls":)"<<debugRecordPrefix.capturedPriorCalls
        <<R"(,"remainingSeed":)"<<debugRecordPrefix.random.seed
        <<R"(,"remainingCalls":)"<<debugRecordPrefix.random.calls
        <<R"(,"clockFirstSeconds":)"<<debugRecordPrefix.clockFirstSeconds
        <<R"(,"clockTime64":")"<<debugRecordPrefix.clockTime64
        <<R"(","percentageKnown":)"<<debugRecordPrefix.timing.slowdownPercentage().has_value()
        <<R"(,"physicalRoute":)"<<actualRecordRoute()<<R"(,"ordinaryRecorded":)"<<ordinaryRecordOwner.configured
        <<R"(,"debugTainted":true,"personalReadOnly":true,"ownedWriterBound":)"
        <<debugPostrunRecord->bound()<<R"(,"replay":{"present":)"<<bool(debugFreshReplay);
    if(debugFreshReplay){
        const auto& input=debugFreshReplay->input();const auto& first=input.chunks().front();
        unsigned packets=0,samples=0;for(const auto& chunk:input.chunks()){packets+=chunk.packets;samples+=chunk.samples;}
        output<<R"(,"mode":)"<<debugFreshReplay->mode()<<R"(,"stage":)"<<debugFreshReplay->stage()
            <<R"(,"generation":)"<<debugFreshReplayGeneration<<R"(,"epoch":)"<<debugFreshReplayEpoch
            <<R"(,"inputActive":)"<<debugFreshReplay->active()<<R"(,"inputFrame":)"<<input.frame()
            <<R"(,"packets":)"<<packets<<R"(,"chunks":)"<<input.chunks().size()<<R"(,"samples":)"<<samples
            <<R"(,"finished":false,"configurationOrigin":"owned-original-default-not-personal","firstInputBytes":[)";
        if(first.packets)for(unsigned byte=0;byte<6;++byte){if(byte)output<<',';output<<unsigned(first.input[byte]);}
        output<<R"(],"firstFPSByte":)";if(first.samples)output<<unsigned(first.fps[0]);else output<<"null";
        const auto auxiliary=messagesConfigured?messages->recordAuxiliaryLive():std::nullopt;
        output<<R"(,"messageAuxiliaryKnown":)"<<auxiliary.has_value()<<R"(,"messageAuxiliaryLive":)";
        if(auxiliary)output<<*auxiliary;else output<<"null";
    }
    output<<"}}";
    debugRecordPrefixSnapshot=output.str();return debugRecordPrefixSnapshot.c_str();
}
const void* th128_world_debug_record_data() {return actualPostrunMenu?actualPostrunMenu->profile().data():debugRecordPrefix.profile.data();}
unsigned th128_world_debug_record_size() {return unsigned(actualPostrunMenu?actualPostrunMenu->profile().size():debugRecordPrefix.profile.size());}

int th128_world_bind_debug_replay_record(unsigned epoch) {
    operationError.clear();
    if(debugFreshReplay||!debugRecordPrefix.worldBound||debugRecordPrefix.failed
        ||!debugRecordPrefix.captureApplied||!debugRecordPrefix.clockBound||epoch!=debugRecordPrefix.epoch
        ||debugRecordPrefix.generation!=worldGeneration||debugRecordPrefix.sessionGeneration!=worldGeneration
        ||!debugRecordPrefix.sessionPresent||debugRecordPrefix.sessionFlags60!=0||debugRecordPrefix.sessionField74!=0
        ||world->faulted()||world->frame()!=0
        ||(ordinaryRecordOwner.configured
            ?stageIdForEcl(worldStartedName)!=int(ordinaryRecordOwner.stage)
                ||worldStartDifficulty!=int(ordinaryRecordOwner.rank)
            :worldStartedName!="st_ex.ecl"||worldStartDifficulty!=4)
        ||!playerConfigured||!playerMotion||!playerShotsConfigured||!playerShotGroupsConfigured
        ||!shotHitsConfigured||!messagesConfigured||messages->state().active||!debugTerminal.configured
        ||debugTerminal.stage!=(ordinaryRecordOwner.configured?int(ordinaryRecordOwner.stage):16)
        ||debugTerminal.generation!=worldGeneration||!debugRunTainted
        ||!stageControl.sessionConstructed()||stageControl.sessionFlag()!=1){
        operationError="fresh replay constructor lacks the actual configured EX session owner";return 0;
    }
    const auto actual=world->gameRandomState();
    if(actual.seed!=debugRecordPrefix.capturedSeed||actual.calls!=0){
        operationError="fresh replay construction would lose the original captured RNG boundary";return 0;
    }
    const auto auxiliary=messages->recordAuxiliaryLive();
    if(!auxiliary||*auxiliary){
        operationError="fresh EX replay requires parsed MSG6c absence or its actual route-choice owner";return 0;
    }
    const auto group=th128::playershot::selectShotGroup(cumulativeFrozenArea,shotDifficulty);
    if(!group){operationError="fresh replay shot group is unavailable";return 0;}
    try{
        const th128::record::NativeFreshReplayContext context{debugRecordPrefix.sessionField74,
            unsigned(debugTerminal.stage),unsigned(worldStartDifficulty),debugRecordPrefix.capturedSeed,
            unsigned(stageControl.sessionFlag()),*group,scoreRaw,pointValueRaw,motivationRaw,
            playerWindowControl.perfectCharge4b4d70,cumulativeFrozenArea};
        if(ordinaryRecordOwner.configured){
            const th128::record::NativeOrdinaryFreshReplayContext ordinary{context,ordinaryRecordOwner.route};
            debugFreshReplay=std::make_unique<th128::record::NativeFreshReplayRecord>(ordinary);
        }else{
            debugFreshReplay=std::make_shared<th128::record::NativeFreshReplayRecord>(context);
            debugExtraReplay=std::make_shared<th128::record::NativeRunReplay>();
            debugExtraReplay->begin(16,4,context.seed,context.shotGroup,0,context.score,context.pointValue,context.motivation,context.perfectCharge,context.frozenArea,playerRaw.horizontal,playerRaw.vertical);
        }
        debugFreshReplayGeneration=worldGeneration;debugFreshReplayEpoch=epoch;
        debugRecordPrefix.replayMode=debugFreshReplay->mode();return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
const void* th128_world_debug_replay_data() {return debugFreshReplay?debugFreshReplay->bytes().data():nullptr;}
unsigned th128_world_debug_replay_size() {return debugFreshReplay?unsigned(debugFreshReplay->bytes().size()):0;}

int th128_world_bind_debug_record_driver(unsigned epoch) {
    operationError.clear();
    if(debugRecordDriver.bound||!debugFreshReplay||!debugRecordPrefix.worldBound
        ||debugRecordPrefix.failed||!debugRecordPrefix.sessionPresent
        ||debugFreshReplayGeneration!=worldGeneration||debugFreshReplayEpoch!=epoch
        ||epoch!=debugRecordPrefix.epoch||debugRecordPrefix.sessionFlags60!=0
        ||world->faulted()||world->frame()!=0||stageControl.current()!=7||stageControl.pending()!=7
        ||debugFreshReplay->active()||debugFreshReplay->input().frame()!=-1){
        operationError="record driver binding lacks the actual fresh owned EX session";return 0;
    }
    const auto counter=debugFreshReplay->bytes()[36+0x18+0x1e];
    if(counter!=0){operationError="record driver requires its original owned one-update presentation batch";return 0;}
    debugRecordDriver.bound=true;debugRecordDriver.epoch=epoch;debugRecordDriver.generation=worldGeneration;
    debugRecordDriver.configurationCounterByte=counter;
    debugRecordDriver.lastSeconds=debugRecordPrefix.clockFirstSeconds;
    debugRecordDriver.lastTime64=debugRecordPrefix.clockTime64;return 1;
}
int th128_world_debug_record_begin_update(unsigned epoch,unsigned clockSerial,
        unsigned filetimeLow,unsigned filetimeHigh,double seconds) {
    operationError.clear();
    if(!debugRecordDriver.bound||debugRecordDriver.phase!=0||epoch!=debugRecordDriver.epoch
        ||debugRecordDriver.generation!=worldGeneration||debugRecordPrefix.failed||world->faulted()
        ||clockSerial!=debugRecordDriver.lastClockSerial+1||!std::isfinite(seconds)
        ||seconds<debugRecordDriver.lastSeconds||world->frame()!=debugRecordDriver.completedFrames
        ||!grazeSounds.empty()||soundJournalWindowSerial!=0){
        operationError="record pre-update controller boundary or clock sequence rejected";return 0;
    }
    const auto timestamp=th128::record::NativeRecordTiming::time64FromFileTime(
        (std::uint64_t(filetimeHigh)<<32)|filetimeLow);
    if(timestamp==UINT64_MAX){operationError="record pre-update UTC sample is invalid";return 0;}
    if(stageControl.current()!=7||stageControl.pending()!=7){
        operationError="record pre-update scene transition requires its ordered retirement owner";return 0;
    }
    const auto consumed=stageControl.consume({debugRecordPrefix.sessionPresent,
        std::int32_t(debugRecordPrefix.sessionField74)});
    if(consumed.status!=th128::stage::CommitStatus::NoChange
        ||consumed.action.kind!=th128::stage::ActionKind::None){
        operationError="record pre-update scene consumer changed its unsupported lifecycle";return 0;
    }
    ++debugRecordDriver.controllerCalls;debugRecordDriver.lastClockSerial=clockSerial;
    debugRecordDriver.lastSeconds=seconds;debugRecordDriver.beginSeconds=seconds;
    debugRecordDriver.lastTime64=timestamp;debugRecordDriver.phase=1;return 1;
}
int th128_world_debug_record_post_present(unsigned epoch,unsigned clockSerial,unsigned frame,
        unsigned filetimeLow,unsigned filetimeHigh,double seconds) {
    operationError.clear();
    if(!debugRecordDriver.bound||debugRecordDriver.phase!=3||epoch!=debugRecordDriver.epoch
        ||debugRecordDriver.generation!=worldGeneration||debugRecordPrefix.failed||world->faulted()
        ||clockSerial!=debugRecordDriver.lastClockSerial+1||!std::isfinite(seconds)
        ||seconds<debugRecordDriver.lastSeconds||frame!=world->frame()
        ||frame!=debugRecordDriver.presentationFrame||frame!=debugRecordDriver.completedFrames+1){
        operationError="record post-presentation identity or clock sequence rejected";return 0;
    }
    const auto timestamp=th128::record::NativeRecordTiming::time64FromFileTime(
        (std::uint64_t(filetimeHigh)<<32)|filetimeLow);
    if(timestamp==UINT64_MAX){operationError="record post-presentation UTC sample is invalid";return 0;}
    try{
        debugRecordPrefix.timing.sample(seconds,debugRecordPrefix.sessionPresent,
            debugRecordPrefix.sessionFlags60,debugRecordDriver.configurationCounterByte);
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
    debugRecordDriver.lastClockSerial=clockSerial;debugRecordDriver.lastSeconds=seconds;
    debugRecordDriver.lastTime64=timestamp;++debugRecordDriver.completedFrames;
    debugRecordDriver.phase=0;return 1;
}
int th128_world_ordinary_ending_post_present(unsigned epoch,unsigned serial,unsigned endingTick,
        unsigned filetimeLow,unsigned filetimeHigh,double seconds) {
    operationError.clear();
    if(!ordinaryRecordOwner.configured||!ordinaryRecordOwner.endingStarted||ordinaryRecordOwner.returned
        ||ordinaryRecordOwner.epoch!=epoch||ordinaryRecordOwner.generation!=worldGeneration
        ||!debugRecordDriver.bound||debugRecordDriver.phase!=9||debugRecordPrefix.failed
        ||!ordinaryRecordOwner.sessionRetired||!ownedBattle.retired()||debugRecordPrefix.sessionPresent
        ||!ordinaryDetachedEnding||!actualPostrunMedia||!actualPostrunMedia->empty()
        ||world->faulted()||world->frame()!=debugRecordDriver.completedFrames
        ||stageControl.current()!=15||stageControl.pending()!=15
        ||serial!=debugRecordDriver.lastClockSerial+1||!std::isfinite(seconds)
        ||seconds<debugRecordDriver.lastSeconds||endingTick!=activeEndingRuntime()->ticks()
        ||(ordinaryRecordOwner.endingPresented
            ?endingTick!=ordinaryRecordOwner.endingPresentedTick+1:endingTick!=0)
        ||!grazeSounds.empty()||soundJournalWindowSerial!=0){
        operationError="ordinary actual ending presentation/clock identity rejected";return 0;
    }
    const auto timestamp=th128::record::NativeRecordTiming::time64FromFileTime(
        (std::uint64_t(filetimeHigh)<<32)|filetimeLow);
    if(timestamp==UINT64_MAX){operationError="ordinary ending UTC provider sample rejected";return 0;}
    try{
        debugRecordPrefix.timing.sample(seconds,debugRecordPrefix.sessionPresent,
            debugRecordPrefix.sessionFlags60,debugRecordDriver.configurationCounterByte);
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
    ordinaryRecordOwner.endingPresented=true;ordinaryRecordOwner.endingPresentedTick=endingTick;
    debugRecordDriver.lastClockSerial=serial;debugRecordDriver.lastSeconds=seconds;
    debugRecordDriver.lastTime64=timestamp;return 1;
}
int th128_world_commit_ordinary_ending_return(unsigned epoch,unsigned stage,unsigned endingTick) {
    operationError.clear();
    if(!ordinaryRecordOwner.configured||!ordinaryRecordOwner.endingStarted||ordinaryRecordOwner.returned
        ||epoch!=ordinaryRecordOwner.epoch||stage!=ordinaryRecordOwner.stage
        ||ordinaryRecordOwner.generation!=worldGeneration||!debugRecordDriver.bound
        ||debugRecordDriver.phase!=9||debugRecordPrefix.failed||world->faulted()
        ||!ordinaryRecordOwner.sessionRetired||!ownedBattle.retired()||debugRecordPrefix.sessionPresent
        ||!ordinaryDetachedEnding||!actualPostrunMedia||!actualPostrunMedia->empty()
        ||!ordinaryRecordOwner.endingPresented||endingTick!=ordinaryRecordOwner.endingPresentedTick
        ||endingTick!=activeEndingRuntime()->ticks()||!activeEndingRuntime()->returned()
        ||!activeEndingRuntime()->cleaned()||activeEndingRuntime()->returnScene()!=16
        ||activeEndingRuntime()->visualFadeActive()||debugRecordPrefix.clearRecordEvents!=1
        ||stageControl.current()!=15||stageControl.pending()!=15
        ||!debugTerminal.requested||debugTerminal.runtimeScene!=15||!debugTerminal.messageDone
        ||debugTerminal.stage!=int(stage)||debugTerminal.selector!=int(ordinaryRecordOwner.route+17)
        ||!grazeSounds.empty()||soundJournalWindowSerial!=0){
        operationError="ordinary original ending return lifecycle incomplete";return 0;
    }
    const auto requested=stageControl.request(16);
    if(requested.status!=th128::stage::RequestStatus::Written||requested.pending!=16){
        operationError="ordinary controller original return16 request rejected";return 0;
    }
    ordinaryRecordOwner.returned=true;return 1;
}
const char* th128_world_debug_record_driver_state() {
    std::ostringstream output;output<<std::setprecision(17)<<std::boolalpha;
    const auto& timing=debugRecordPrefix.timing.state();
    const auto percentage=debugRecordPrefix.timing.slowdownPercentage();
    output<<R"({"bound":)"<<debugRecordDriver.bound<<R"(,"epoch":)"<<debugRecordDriver.epoch
        <<R"(,"generation":)"<<debugRecordDriver.generation<<R"(,"phase":)"<<debugRecordDriver.phase
        <<R"(,"lastClockSerial":)"<<debugRecordDriver.lastClockSerial
        <<R"(,"completedFrames":)"<<debugRecordDriver.completedFrames
        <<R"(,"controllerCalls":)"<<debugRecordDriver.controllerCalls
        <<R"(,"presentationFrame":)"<<debugRecordDriver.presentationFrame
        <<R"(,"configurationCounterByte":)"<<unsigned(debugRecordDriver.configurationCounterByte)
        <<R"(,"lastSeconds":)"<<debugRecordDriver.lastSeconds
        <<R"(,"lastTime64":")"<<debugRecordDriver.lastTime64
        <<R"(","gameplayAnchorKnown":)"<<debugRecordDriver.gameplayAnchorKnown
        <<R"(,"gameplayAnchorSeconds":)"<<debugRecordDriver.gameplayAnchorSeconds
        <<R"(,"percentageKnown":)"<<percentage.has_value()<<R"(,"slowdownPercentage":)";
    if(percentage)output<<*percentage;else output<<"null";
    output<<R"(,"statistics":{"lastWindowSeconds":)"<<timing.lastWindowSeconds
        <<R"(,"counter":)"<<timing.counter<<R"(,"fps":)"<<timing.fps
        <<R"(,"overspeedWindows":)"<<timing.overspeedWindows
        <<R"(,"numerator":)"<<timing.numerator<<R"(,"denominator":)"<<timing.denominator<<"}}";
    debugRecordDriverSnapshot=output.str();return debugRecordDriverSnapshot.c_str();
}

const char* th128_world_debug_postrun_record(unsigned operation,unsigned epoch,
        const void* data,unsigned bytes) {
    operationError.clear();bool accepted=false;
    const auto* input=static_cast<const std::uint8_t*>(data);
    const auto read32=[&](unsigned offset){
        return std::uint32_t(input[offset])|(std::uint32_t(input[offset+1])<<8)
            |(std::uint32_t(input[offset+2])<<16)|(std::uint32_t(input[offset+3])<<24);
    };
    const auto read64=[&](unsigned offset){return std::uint64_t(read32(offset))|(std::uint64_t(read32(offset+4))<<32);};
    const auto readDouble=[&](unsigned offset){double value;std::memcpy(&value,input+offset,8);return value;};
    try{
        if(!debugRecordDriver.bound||!debugFreshReplay||debugRecordPrefix.failed||world->faulted()
            ||epoch!=debugRecordDriver.epoch||epoch!=debugRecordPrefix.epoch
            ||debugRecordDriver.generation!=worldGeneration||debugFreshReplayGeneration!=worldGeneration)
            throw std::runtime_error("actual postrun epoch or session owner rejected");
        if(operation!=3&&!th128::runtime::inputRange(data,bytes))
            throw std::runtime_error("actual postrun input packet is outside owned memory");
        if(operation==0){
            if(bytes!=8||world->frame()!=0||debugRecordDriver.phase!=0||!debugRunTainted)
                throw std::runtime_error("actual score writer binding requires the fresh debug session");
            debugPostrunRecord->bind(read64(0),true,false,false);
        }else if(operation==1){
            if(bytes!=48||debugRecordDriver.phase!=0||!debugRecordDriver.gameplayAnchorKnown
                ||!actualRecordScoreRetirementReady()
                ||debugRecordPrefix.clearRecordEvents!=1||debugFreshReplay->mode()!=0
                ||!grazeSounds.empty()||soundJournalWindowSerial!=0)
                throw std::runtime_error("actual ordered retirement record gate is incomplete");
            if(read32(8)!=debugRecordDriver.lastClockSerial+1
                ||read32(20)!=debugRecordDriver.lastClockSerial+2)
                throw std::runtime_error("actual retirement clock serial rejected");
            const auto sampled=readDouble(32),refreshed=readDouble(40);
            const auto sampledTime=th128::record::NativeRecordTiming::time64FromFileTime(
                (std::uint64_t(read32(16))<<32)|read32(12));
            const auto refreshedTime=th128::record::NativeRecordTiming::time64FromFileTime(
                (std::uint64_t(read32(28))<<32)|read32(24));
            if(sampledTime==UINT64_MAX||refreshedTime==UINT64_MAX
                ||sampled<debugRecordDriver.lastSeconds)
                throw std::runtime_error("actual retirement clock provider sample rejected");
            debugPostrunRecord->prepare(read64(0),debugRecordPrefix.profile,
                debugRecordDriver.gameplayAnchorSeconds,sampled,refreshed,
                debugRecordPrefix.sessionField74,stageControl.pending());
            debugRecordPrefix.profile=debugPostrunRecord->profile();
            debugRecordDriver.gameplayAnchorSeconds=debugPostrunRecord->refreshedAnchor();
            debugRecordDriver.lastSeconds=refreshed;debugRecordDriver.lastTime64=refreshedTime;
            debugRecordDriver.lastClockSerial=read32(20);debugRecordDriver.phase=4;
        }else if(operation==2){
            if(bytes!=20||debugRecordDriver.phase!=4
                ||stageControl.pending()!=(ordinaryRecordOwner.configured?15:16))
                throw std::runtime_error("actual retirement score write acknowledgement order rejected");
            debugPostrunRecord->acknowledge(read64(0),read32(8),read32(12),read32(16));
            debugRecordDriver.phase=5;
        }else if(operation!=3)throw std::runtime_error("actual postrun operation rejected");
        accepted=true;
    }catch(const std::exception& failure){operationError=failure.what();}
    std::ostringstream output;output<<std::boolalpha;
    output<<R"({"accepted":)"<<accepted<<R"(,"epoch":)"<<epoch<<R"(,"generation":)"<<worldGeneration
        <<R"(,"bound":)"<<debugPostrunRecord->bound()<<R"(,"prepared":)"<<debugPostrunRecord->prepared()
        <<R"(,"acknowledged":)"<<debugPostrunRecord->acknowledged()
        <<R"(,"serial":)"<<debugPostrunRecord->serial()<<R"(,"bytes":)"<<debugPostrunRecord->container().size()
        <<R"(,"digest32":)"<<debugPostrunRecord->digest()<<R"(,"pointer":)"
        <<reinterpret_cast<std::uintptr_t>(debugPostrunRecord->container().data())<<'}';
    debugPostrunSnapshot=output.str();return debugPostrunSnapshot.c_str();
}

const char* th128_world_state();

int th128_world_bind_debug_postrun_asset(unsigned epoch,int bank,const void* data,unsigned size){
    operationError.clear();
    if(!debugRecordDriver.bound||debugRecordDriver.phase!=0||epoch!=debugRecordDriver.epoch
        ||!debugRunTainted||world->frame()!=0||ownedBattle.attempted()
        ||bank!=0&&bank!=24&&bank!=25&&bank!=11||bank==11&&ordinaryRecordOwner.configured||size==0||size>16u*1024u*1024u
        ||!th128::runtime::inputRange(data,size)){
        operationError="postrun resource binding requires the actual fresh owned run";return 0;
    }
    if(bank==11){if(!extraPostrunNameFont.empty()){operationError="EX ASCII replacement rejected";return 0;}const auto* source=static_cast<const std::uint8_t*>(data);extraPostrunNameFont.assign(source,source+size);return 1;}
    const unsigned index=bank==0?0:bank==24?1:2;
    if(!postrunAssets[index].empty()){operationError="postrun bank already bound";return 0;}
    const auto* bytes=static_cast<const std::uint8_t*>(data);
    postrunAssets[index].assign(bytes,bytes+size);return 1;
}
int th128_world_debug_postrun_retire(unsigned epoch){
    operationError.clear();
    if(ordinaryRecordOwner.configured){
        if(!debugRecordDriver.bound||debugRecordDriver.phase!=9||epoch!=debugRecordDriver.epoch
            ||debugRecordPrefix.failed||debugRecordPrefix.sessionPresent||!debugPostrunRecord->acknowledged()
            ||!ordinaryRecordOwner.sessionRetired||!ownedBattle.retired()||actualPostrunMenu
            ||stageControl.current()!=15||stageControl.pending()!=16||!actualRecordTerminalReady()
            ||debugRecordPrefix.clearRecordEvents!=1||!retainedPostrunReplay
            ||debugFreshReplayGeneration!=worldGeneration||debugFreshReplayEpoch!=epoch
            ||!ordinaryDetachedEnding||!ordinaryDetachedEnding->cleaned()
            ||ordinaryDetachedEnding->visualFadeActive()||!actualPostrunMedia||!actualPostrunMedia->empty()
            ||!grazeSounds.empty()||soundJournalWindowSerial){
            operationError="ordinary ending16/menu construction ownership incomplete";return 0;
        }
        const auto& statistics=debugRecordPrefix.timing.state();
        if(!std::isfinite(statistics.numerator)||!std::isfinite(statistics.denominator)||statistics.denominator<=0){
            operationError="ordinary retained pre-ending statistics unavailable";return 0;
        }
        try{
            const auto committed=stageControl.consumeEnding16([&]{
                endingRuntimeSnapshot=ordinaryDetachedEnding->json();
                ordinaryDetachedEnding.reset();return true;
            },[&](unsigned mode){
                if(mode!=3)throw std::runtime_error("ordinary fresh menu constructor mode rejected");
                th128::postrun::Context context;context.profile=debugRecordPrefix.profile;
                context.replay=retainedPostrunReplay;context.score=scoreRaw;context.route=ordinaryRecordOwner.route;
                context.stage=context.route+17;context.rank=ordinaryRecordOwner.rank;
                context.ordinaryDiagnostic=true;context.epoch=epoch;
                context.clockSerial=debugRecordDriver.lastClockSerial;
                context.numerator=statistics.numerator;context.denominator=statistics.denominator;
                context.musicSink=[](int operation,int track,int argument){actualPostrunMedia->music(operation,track,argument);};
                context.soundSink=[](int identifier){actualPostrunMedia->sound(identifier,0,0);};
                actualPostrunMenu=std::make_unique<th128::postrun::Menu>(std::move(context));
                actualPostrunMenu->load(0,postrunAssets[0]);actualPostrunMenu->load(24,postrunAssets[1]);
                actualPostrunMenu->load(25,postrunAssets[2]);return true;
            });
            if(committed.status!=th128::stage::CommitStatus::Applied
                ||stageControl.current()!=4||stageControl.pending()!=4)
                throw std::runtime_error("ordinary actual ending owner/menu commit failed");
            ++debugRecordDriver.controllerCalls;debugRecordDriver.phase=6;return 1;
        }catch(const std::exception& failure){operationError=failure.what();debugRecordPrefix.failed=true;return 0;}
    }
    if(!debugRecordDriver.bound||debugRecordDriver.phase!=5||epoch!=debugRecordDriver.epoch||debugRecordPrefix.failed
        ||!debugRecordPrefix.sessionPresent||!debugPostrunRecord->acknowledged()||stageControl.current()!=7||stageControl.pending()!=16
        ||!actualRecordTerminalReady()||debugRecordPrefix.clearRecordEvents!=1||!debugFreshReplay||debugFreshReplay->mode()!=0
        ||debugFreshReplayGeneration!=worldGeneration||debugFreshReplayEpoch!=epoch||ownedBattle.attempted()||actualPostrunMenu||normalPostrun.menu
        ||!debugExtraReplay||debugExtraReplay->frames()!=unsigned(world->frame())||debugExtraReplay->frames()!=unsigned(debugFreshReplay->input().frame())
        ||extraPostrunNameFont.empty()||!grazeSounds.empty()||soundJournalWindowSerial
        ||!std::all_of(postrunAssets.begin(),postrunAssets.end(),[](const auto& bytes){return !bytes.empty();})){
        operationError="actual EX terminal resources/record/full-history/epoch gate is incomplete";return 0;
    }
    const auto& statistics=debugRecordPrefix.timing.state();
    if(!std::isfinite(statistics.numerator)||!std::isfinite(statistics.denominator)||statistics.denominator<=0){operationError="actual EX statistics are unavailable";return 0;}
    retiredWorldSnapshot=th128_world_state();const auto score=scoreRaw;
    const auto configurationMode=debugFreshReplay->bytes()[36+0x18+0x38];
    const auto retainedGame=world->gameRandomState();const auto retainedFrame=unsigned(world->frame());const auto retainedDisplay=worldDisplayRandom;
    th128::record::ExtraTerminalBinding binding;binding.identity={worldGeneration,epoch,16,6,4,0,23,debugExtraReplay->frames(),retainedFrame,debugPostrunRecord->digest()};
    binding.record=debugPostrunRecord;binding.replay=debugExtraReplay;binding.envelope=debugFreshReplay;binding.score=score;binding.numerator=statistics.numerator;binding.denominator=statistics.denominator;
    if(!extraReturnOwner.bind(binding,stageControl,operationError))return 0;
    try{
        normalPostrun.media=std::make_unique<th128::postrun::Media>(worldGeneration);
        const auto committed=stageControl.consumeTerminal16([&]{
            return extraReturnOwner.retire(stageControl,[&]{
                if(!ownedBattle.retire(operationError))return false;
                const auto after=world->gameRandomState();if(world->frame()!=int(retainedFrame)||after.seed!=retainedGame.seed||after.calls!=retainedGame.calls)return false;
                debugRecordPrefix.sessionPresent=false;stageSessionPresent=0;
                playerConfigured=playerMotion=playerShotsConfigured=playerShotGroupsConfigured=false;
                messagesConfigured=messageFrontConfigured=routeChoiceConfigured=stageTransitionConfigured=false;
                normalPostrun.media->music(configurationMode&0x10?4:3,-1,0);for(int identifier=0;identifier<76;++identifier)normalPostrun.media->sound(identifier,0,1);
                return true;
            },operationError);
        },[&](unsigned mode){
            if(mode!=3)throw std::runtime_error("EX original terminal menu mode must be3");
            auto permit=extraReturnOwner.mint(stageControl,operationError);if(!permit)throw std::runtime_error(operationError);
            th128::postrun::Context context;context.profile=permit->profile();context.extraReplay=debugExtraReplay;context.extraRecord=debugPostrunRecord;context.replay=debugFreshReplay;
            context.score=score;context.route=6;context.rank=4;context.stage=23;context.epoch=worldGeneration;context.clockSerial=0;
            context.numerator=statistics.numerator;context.denominator=statistics.denominator;
            context.musicSink=[](int operation,int track,int argument){normalPostrun.media->music(operation,track,argument);};
            context.soundSink=[](int identifier){normalPostrun.media->sound(identifier,0,0);};
            normalPostrun.menu=std::make_unique<th128::postrun::Menu>(std::move(context),std::move(permit));
            for(unsigned i=0;i<3;++i)normalPostrun.menu->load(i==0?0:i==1?24:25,postrunAssets[i]);normalPostrun.menu->loadNameFont(extraPostrunNameFont);return true;
        });
        if(!extraReturnOwner.commit(stageControl,committed,operationError))throw std::runtime_error(operationError);
        normalPostrun.generation=worldGeneration;normalPostrun.extra=true;normalPostrun.profile=normalPostrun.menu->profile();
        normalPostrun.retainedRandom={worldGeneration,retainedFrame,debugExtraReplay->frames(),retainedGame,retainedDisplay};
        if(!normalPostrun.receipt.bind(*normalPostrun.menu,operationError)||!normalPostrun.mainOwner.bind(*normalPostrun.menu,normalPostrun.retainedRandom,operationError))throw std::runtime_error(operationError);
        ++debugRecordDriver.controllerCalls;debugRecordDriver.phase=6;return 1;
    }catch(const std::exception& e){normalPostrun.failed=true;operationError=e.what();return 0;}
}

int th128_world_debug_postrun_bind_previews(unsigned epoch,const void* data,unsigned bytes){
    operationError.clear();
    try{
        if(!actualPostrunMenu||epoch!=debugRecordDriver.epoch||bytes!=25||!th128::runtime::inputRange(data,bytes))
            throw std::runtime_error("actual owned preview observation input rejected");
        actualPostrunMenu->bindObservedMissingPreviews({static_cast<const std::uint8_t*>(data),bytes});return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
int th128_world_debug_postrun_input(unsigned epoch,unsigned serial,unsigned holding,unsigned pressed,
        unsigned filetimeLow,unsigned filetimeHigh,double seconds){
    operationError.clear();
    try{
        if(!actualPostrunMenu||!actualPostrunMedia||debugRecordDriver.phase!=6||epoch!=debugRecordDriver.epoch
            ||debugRecordPrefix.failed||stageControl.current()!=4||stageControl.pending()!=4
            ||serial!=debugRecordDriver.lastClockSerial+1||!actualPostrunMedia->empty()
            ||!std::isfinite(seconds)||seconds<debugRecordDriver.lastSeconds)
            throw std::runtime_error("actual postrun pre-update/clock/media boundary rejected");
        const auto filetime=(std::uint64_t(filetimeHigh)<<32)|filetimeLow;
        actualPostrunMenu->tick({holding,pressed},serial,filetime,epoch);
        debugRecordPrefix.profile=actualPostrunMenu->profile();
        if(!actualPostrunMenu->replayPresent())retainedPostrunReplay.reset();
        debugRecordDriver.lastClockSerial=serial;debugRecordDriver.lastSeconds=seconds;debugRecordDriver.phase=7;
        return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
const char* th128_world_debug_postrun_media_window(){
    if(!actualPostrunMedia)return "{\"epoch\":0,\"serial\":0,\"digest\":0,\"commands\":[]}";
    postrunMediaText=actualPostrunMedia->window();return postrunMediaText.c_str();
}
int th128_world_debug_postrun_media_ack(unsigned epoch,unsigned serial,unsigned count,unsigned digest){
    operationError.clear();
    try{
        if(!actualPostrunMedia||epoch!=debugRecordDriver.epoch)throw std::runtime_error("actual media owner absent");
        actualPostrunMedia->acknowledge(epoch,serial,count,digest);++postrunMediaACKs;return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
const char* th128_world_debug_postrun_present(unsigned epoch){
    operationError.clear();postrunPresentationText.clear();
    if(!actualPostrunMenu||!actualPostrunMedia||!actualPostrunMedia->empty()
        ||epoch!=debugRecordDriver.epoch||debugRecordDriver.phase!=7){
        operationError="actual menu presentation boundary rejected";return postrunPresentationText.c_str();
    }
    std::ostringstream output;
    output<<"{\"epoch\":"<<epoch<<",\"generation\":"<<worldGeneration<<",\"serial\":"<<actualPostrunMenu->ticks()
        <<",\"frame\":"<<actualPostrunMenu->ticks()<<",\"draws\":"<<actualPostrunMenu->presentation()<<'}';
    postrunPresentationText=output.str();postrunPresented=actualPostrunMenu->ticks();debugRecordDriver.phase=8;
    return postrunPresentationText.c_str();
}
int th128_world_debug_postrun_post_present(unsigned epoch,unsigned serial,unsigned frame,
        unsigned filetimeLow,unsigned filetimeHigh,double seconds){
    operationError.clear();
    if(!actualPostrunMenu||debugRecordDriver.phase!=8||epoch!=debugRecordDriver.epoch
        ||serial!=debugRecordDriver.lastClockSerial+1||frame!=postrunPresented
        ||!std::isfinite(seconds)||seconds<debugRecordDriver.lastSeconds){
        operationError="actual postrun post-present clock boundary rejected";return 0;
    }
    const auto timestamp=th128::record::NativeRecordTiming::time64FromFileTime((std::uint64_t(filetimeHigh)<<32)|filetimeLow);
    if(timestamp==UINT64_MAX){operationError="actual postrun UTC sample rejected";return 0;}
    try{debugRecordPrefix.timing.sample(seconds,false,debugRecordPrefix.sessionFlags60,debugRecordDriver.configurationCounterByte);}
    catch(const std::exception& failure){operationError=failure.what();return 0;}
    try{actualPostrunMenu->observePresentationClock(epoch,serial);}catch(const std::exception& failure){operationError=failure.what();return 0;}
    debugRecordDriver.lastClockSerial=serial;debugRecordDriver.lastSeconds=seconds;debugRecordDriver.lastTime64=timestamp;
    debugRecordDriver.phase=6;return 1;
}
const char* th128_world_debug_postrun_state(){
    std::ostringstream output;output<<std::boolalpha;
    output<<"{\"present\":"<<bool(actualPostrunMenu)<<",\"current\":"<<stageControl.current()
        <<",\"pending\":"<<stageControl.pending()<<",\"menuMode\":"<<stageControl.menuMode()
        <<",\"retired\":"<<ownedBattle.retired()<<",\"retirementAttempted\":"<<ownedBattle.attempted()
        <<",\"retiredOwnerPhases\":"<<ownedBattle.completedOwners()<<",\"gameFrame\":"<<world->frame()
        <<",\"phase\":"<<debugRecordDriver.phase<<",\"mediaACKs\":"<<postrunMediaACKs
        <<",\"mainReady\":"<<(actualPostrunMenu&&actualPostrunMenu->mainReady())
        <<",\"nativeX86Execution\":false,\"nativeSTDContainerInstantiatedByHeadlessCore\":false";
    if(actualPostrunMenu)output<<",\"menu\":"<<actualPostrunMenu->snapshot();
    output<<'}';postrunStateText=output.str();return postrunStateText.c_str();
}

int th128_world_start(const char* name,std::uint32_t nameLength,int difficulty,unsigned seed) {
    operationError.clear();std::string identifier;
    if(!nameArgument(name,nameLength,identifier)||seed>65535){operationError="Invalid world start arguments";return 0;}
    if(debugRecordPrefix.prepared&&(!debugRecordPrefix.clockBound||debugRecordPrefix.failed
        ||debugRecordPrefix.captureApplied||debugRecordPrefix.worldBound||worldGeneration!=0
        ||(ordinaryRecordOwner.configured
            ?stageIdForEcl(identifier)!=int(ordinaryRecordOwner.stage)||difficulty!=int(ordinaryRecordOwner.rank)
            :identifier!="st_ex.ecl"||difficulty!=4)
        ||seed!=debugRecordPrefix.random.seed)){
        operationError="prepared debug record prefix does not own this fresh EX start";return 0;
    }
    const int ordinaryStage=stageIdForEcl(identifier);
    const bool freshOrdinarySession=ordinaryStage>=1&&ordinaryStage<=15&&difficulty>=0&&difficulty<4
        &&stageControl.current()==0&&stageControl.pending()==0&&!stageControl.sessionStartupAttempted();
    if(identifier=="st_ex.ecl"&&difficulty==4||ordinaryRecordOwner.configured||freshOrdinarySession){
        if(!stageControl.initialized())stageControl.initialize(0);
        const auto startup=stageControl.constructFreshSession7([&](std::int32_t argument){
            if(argument!=0)return false;
            if(!debugRecordPrefix.prepared)return world->start(identifier,difficulty,std::uint16_t(seed));
            debugRecordPrefix.sessionPresent=true;
            debugRecordPrefix.sessionField74=unsigned(argument);
            debugRecordPrefix.sessionFlags60|=4u;
            debugRecordPrefix.replayMode=unsigned(argument);
            const auto fields=th128::record::NativeRecordAccounting::beginFresh(debugRecordPrefix.profile,actualRecordRoute(),unsigned(difficulty),unsigned(argument));
            debugRecordPrefix.highScoreRaw=fields.highScore;debugRecordPrefix.highScoreContinue=fields.highScoreContinue;
            debugRecordPrefix.playCount=fields.playCount;debugRecordPrefix.freshRecordFieldsApplied=true;
            const auto captured=th128::record::NativeRecordBootstrap::captureRecorderSeed(debugRecordPrefix.random);
            debugRecordPrefix.capturedSeed=captured.seed;debugRecordPrefix.capturedPriorCalls=captured.previousCalls;
            debugRecordPrefix.captureApplied=true;
            if(!world->start(identifier,difficulty,captured.seed)){
                debugRecordPrefix.sessionFlags60|=8u;debugRecordPrefix.failed=true;return false;
            }
            const auto actual=world->gameRandomState();
            if(actual.seed!=captured.seed||actual.calls!=debugRecordPrefix.random.calls){
                debugRecordPrefix.failed=true;return false;
            }
            return true;
        });
        if(startup.status!=th128::stage::CommitStatus::Applied
            ||startup.action.kind!=th128::stage::ActionKind::RecreateSession){
            if(debugRecordPrefix.prepared)debugRecordPrefix.failed=true;
            operationError="original fresh session7 actual construction or owner commit failed";return 0;
        }
    }else if(!world->start(identifier,difficulty,std::uint16_t(seed)))return 0;
    spellVisuals=std::make_unique<th128::spell::Visuals>();spellsConfigured=false;spellResult={};spellResultFrames=0;
    laserLines=std::make_unique<th128::laser::LineHost>();
    beginNewWorldGeneration();
    worldStartedName=identifier;worldStartDifficulty=difficulty;currentStageProgress=0;
    if(debugRecordPrefix.prepared){
        debugRecordPrefix.worldBound=true;debugRecordPrefix.generation=worldGeneration;
        debugRecordPrefix.sessionGeneration=worldGeneration;
        if(ordinaryRecordOwner.configured)ordinaryRecordOwner.generation=worldGeneration;
        debugRecordPrefix.timing.setRecordedCounters(0,0);
    }
    if(!stageControl.initialized())stageControl.initialize(0);
    messages=std::make_unique<th128::message::Host>();messageVisuals=std::make_unique<th128::message::Visuals>();messagesConfigured=false;messageFrontConfigured=false;messageMusicSerial=0;resetRouteChoice();resetStageTransition();resetMusicControl();
    bulletAnimations.clear();bulletVisuals=std::make_unique<th128::bullet::BulletVisuals>();bulletDraws.clear();
    enemyAppearances.clear();bulletProfiles.clear();
    presentationError.clear();
    playerProfileRadius=0;
    playerAttractionSpeed=0;
    grazeConfigured=false;grazeSounds.clear();grazeGlobals={0,0};advanceSoundJournalReset();
    enemyVisuals=std::make_unique<th128::enemy::EnemyVisuals>(
        identifier=="st_ex.ecl"&&difficulty==4?th128::enemy::EnemyVisuals::BankDomain::ExtraStage16Rank4
            :th128::enemy::EnemyVisuals::BankDomain::Ordinary);
    cancellationEffects=std::make_unique<th128::effects::CancellationEffects>();
    playerHit=std::make_unique<th128::player::PlayerHitHost>();playerConfigured=false;
    playerShots=std::make_unique<th128::playershot::PlayerShotHost>();
    playerShotGroups.reset();playerShotGroupsConfigured=false;
    itemHost=std::make_unique<th128::item::ItemHost>();enemiesDefeated=0;
    scoreRaw=0;pointValueRaw=100000;itemScalar=0;scorePopups.clear();
    playerShotsConfigured=false;cumulativeFrozenArea=0;shotDifficulty=identifier=="st_ex.ecl"&&difficulty==4?4:1;playerDamageSourceFlags.fill(0);
    damageFields=std::make_unique<th128::damagefield::DamageFieldPool>();
    perfectHud=std::make_unique<th128::hud::PerfectHud>();perfectHudConfigured=false;bossHud=std::make_unique<th128::hud::BossHud>();standaloneEffectOwner=1000000;standaloneEffectOwners.clear();
    geometryEffects=std::make_unique<th128::geometry526::Host>();geometryConfigured=false;
    playerRespawnConfigured=false;respawnsCommitted=0;respawnControl={};respawnSavedPosition={0,0,0};emptyLaserCancelCenter={};
    shotHitsConfigured=false;motivationRaw=motivationFlag=shotHitBusy=shotDamageBypass=0;shotSpellFlags=0;
    rapidShotConfigured=false;rapidShotCount=-1;
    playerFire=std::make_unique<th128::player::fire::Host>();iceHost=std::make_unique<th128::ice::Host>();
    perfectHost=std::make_unique<th128::perfect::Host>();perfectConfigured=false;perfectConfigError.clear();
    originalPauseMenu.reset();originalPauseSoundCursor=0;originalContinueCount=0;originalPauseBackdropBytes.clear();
    normalReplay.reset();normalReplaySave.reset();normalReplayNameContext={};normalReplayBytes.clear();normalReplayClockPending=false;normalReplayOrigin=normalReplayClock=0;normalRecord.reset();
    perfectBatchOwned=false;perfectZDenied.clear();
    perfectModeOwned=false;
    debugInfiniteRespawnRun=false;debugRunTainted=false;debugReviveRefills=0;debugLastRefillBefore=0;debugLastRefillAfter=0;
    playerFireConfigured=false;worldDisplayRandom={};lastFrozenArea=0;freezeBonus=0;freezeRewardPanel={};
    playerDeathConfigured=false;deathsCommitted=0;deathOperation.reset();
    playerMotion=false;playerRaw={0,51200};playerInput={};
    headPowerConfigured=false;
    focusIndicator=std::make_unique<th128::player::FocusIndicator>();focusIndicatorConfigured=false;playerFocused=false;
    if(debugRecordPrefix.worldBound){
        debugRunTainted=true;debugRecordPrefix.sessionFlags60&=~4u;
    }
    return 1;
}
int th128_world_enable_geometry526(const void* data,std::uint32_t size){
    operationError.clear();
    if(!world->configurationOpen()||geometryConfigured||!th128::runtime::inputRange(data,size)){
        operationError="geometry526 requires actual text.anm and a fresh unexecuted world";return 0;
    }
    auto candidate=std::make_unique<th128::geometry526::Host>();
    if(!candidate->load({static_cast<const std::uint8_t*>(data),size},operationError))return 0;
    auto* runtime=candidate.get();
    if(!world->setGeometryHost(
        [runtime](std::uint32_t owner,float radius,std::uint32_t color,std::string& error){return runtime->create(owner,radius,color,error);},
        [runtime](std::uint32_t owner,const std::array<float,3>& position,std::string& error){return runtime->update(owner,position,1,error);},
        [runtime](std::uint32_t owner,std::string& error){return runtime->removeOwner(owner,error);},operationError))return 0;
    geometryEffects=std::move(candidate);geometryConfigured=true;
    originalPauseBackdropBytes.assign(static_cast<const std::uint8_t*>(data),static_cast<const std::uint8_t*>(data)+size);return 1;
}
int th128_world_enable_head_power(){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!playerConfigured||!scoreFontLoaded||headPowerConfigured){
        operationError="head power requires fresh actual player and ascii font";return 0;
    }
    const auto bank=scoreFont.bank(11);
    if(!bank||bank->sprites.size()<=288){operationError="head power requires original style7 glyphs";return 0;}
    headPowerConfigured=true;return 1;
}
int th128_world_enable_focus_indicator(){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!playerConfigured||!playerMotion||focusIndicatorConfigured){
        operationError="focus indicator requires fresh original moving player and bullet ANM";return 0;
    }
    auto candidate=std::make_unique<th128::player::FocusIndicator>();
    if(!candidate->configure(bulletVisuals->registry(),operationError))return 0;
    focusIndicator=std::move(candidate);focusIndicatorConfigured=true;return 1;
}
int th128_world_enable_bullet_updates(const void* shot,std::uint32_t size) {
    operationError.clear();
    if(!world->bulletUpdateConfigurationOpen()||!th128::runtime::inputRange(shot,size)||size<0x268||size>4u*1024u*1024u) {
        operationError="Bullet updates require an unadvanced configured world and actual bounded shot profile";return 0;
    }
    const auto* bytes=static_cast<const std::uint8_t*>(shot);
    if(bytes[0]!=3||bytes[1]!=0||bytes[2]!=16||bytes[3]!=0) {
        operationError="Unsupported player shot-profile header";return 0;
    }
    const auto raw=std::uint32_t(bytes[4])|(std::uint32_t(bytes[5])<<8)|(std::uint32_t(bytes[6])<<16)|(std::uint32_t(bytes[7])<<24);
    const auto radius=std::bit_cast<float>(raw);
    if(!std::isfinite(radius)||radius<0||radius>65536||(radius!=0&&radius<0x1p-16f)) {
        operationError="Unsupported player profile hit-radius domain";return 0;
    }
    if(!world->configureBulletUpdater([](th128::bullet::Birth& bullet,bool& removed,std::string& error,const th128::bullet::Manager::NestedEmitter& nested) {
        const auto found=bulletAnimations.find(bullet.id);
        if(found==bulletAnimations.end()){error="Missing bullet animation before manager update";return false;}
        const auto profile=bulletProfiles.find(bullet.type);
        if(profile==bulletProfiles.end()){error="Missing typed bullet profile before manager update";return false;}
        const auto success=th128::bullet::updateBulletWithKnownMisses(bullet,*bulletVisuals,found->second,
            world->playerAimPosition(),playerProfileRadius,removed,error,grazeConfigured?th128::bullet::GrazeCallback(applyGraze):th128::bullet::GrazeCallback{},
            playerConfigured?th128::bullet::HitCallback([](const th128::bullet::Birth&,std::string& reason)->std::optional<int>{
                const auto position=world->playerAimPosition();
                return playerHit->confirmedOverlap({position.x,position.y,position.z},playerHitControl,reason);
            }):th128::bullet::HitCallback{},
            playerConfigured?th128::bullet::CancellationCallback([](const th128::bullet::Birth& bullet,bool fullStep,std::string& reason){
                const auto handle=cancellationEffects->spawn(bullet.cancellationScript,320+bullet.state.position.x,
                    16+bullet.state.position.y,bullet.state.position.z,23,reason);
                if(!handle){reason="bullet cancellation script="+std::to_string(bullet.cancellationScript)+" type="+std::to_string(bullet.type)
                    +" color="+std::to_string(bullet.color)+": "+reason;return false;}
                if(fullStep&&!cancellationEffects->initializeNativeDrift(*handle,
                    {bullet.state.velocity.x,bullet.state.velocity.y,bullet.state.velocity.z},1,{0,0,0},reason))return false;
                return true;
            }):th128::bullet::CancellationCallback{},[](int identifier,std::string& reason){
                if(grazeSounds.size()>=65536){reason="extension sound journal budget";return false;}
                grazeSounds.push_back({identifier,0});return true;
            },nested,[](int type)->const th128::bullet::Profile*{
                const auto child=bulletProfiles.find(type);
                return child==bulletProfiles.end()?nullptr:&child->second;
            },[](th128::bullet::Birth& bullet,th128::bullet::BulletVisuals& visuals,th128::bullet::BulletVisuals::Visual& visual,
                const th128::bullet::ExtensionRaw& record,int index,std::string& reason){
                if(record.type!=1024||index<0||index>=18){reason="type1024 restart is not the original record domain";return false;}
                if(bullet.state.extensionIndex!=index){reason="type1024 restart is not the live extension index";return false;}
                if(bullet.state.motionState!=1&&bullet.state.motionState!=2){reason="type1024 restart state word is not 1 or 2";return false;}
                const auto owned=bulletAnimations.find(bullet.id);
                if(owned==bulletAnimations.end()||&owned->second!=&visual){reason="type1024 restart visual is not the owned bullet";return false;}
                const auto profile=bulletProfiles.find(bullet.type);
                if(profile==bulletProfiles.end()){reason="type1024 restart typed profile is missing";return false;}
                const auto cache=profile->second.cancellationScript(bullet.color);
                if(!cache||*cache<0||*cache!=bullet.cancellationScript)
                    {reason="type1024 restart cache is not the actual profile script for this color";return false;}
                if(!visuals.animationState(visual)){reason="type1024 restart animation state is stale";return false;}
                std::string error;
                const auto effect=cancellationEffects->spawn(*cache,320+bullet.state.position.x,
                    16+bullet.state.position.y,bullet.state.position.z,23,error);
                if(!effect){reason="type1024 restart effect script="+std::to_string(*cache)
                    +" type="+std::to_string(bullet.type)+" color="+std::to_string(bullet.color)+": "+error;return false;}
                if(!cancellationEffects->initializeNativeDrift(*effect,{bullet.state.velocity.x,
                    bullet.state.velocity.y,bullet.state.velocity.z},1,{0,0,0},error)){
                    cancellationEffects->remove(*effect);reason=error;return false;}
                if(!visuals.interrupt(visual,1,error)){cancellationEffects->remove(*effect);reason=error;return false;}
                if(visual.hasSecondary)visual.secondary.interrupt(1);
                bullet.state.motionState=3;
                return true;
            },[](const th128::bullet::Birth& birth,const th128::bullet::ExtensionRaw& record,int index,std::string& reason){
                return world->spawnBulletExtensionEnemy(birth,record,index,reason);
            });
        if(removed)bulletAnimations.erase(bullet.id);
        return success;
    },operationError))return 0;
    playerProfileRadius=radius;
    const auto attraction=std::uint32_t(bytes[8])|(std::uint32_t(bytes[9])<<8)|(std::uint32_t(bytes[10])<<16)|(std::uint32_t(bytes[11])<<24);
    playerAttractionSpeed=std::bit_cast<float>(attraction);
    return 1;
}
int th128_world_enable_graze(int count,int power,unsigned displaySeed){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!world->positionIntegrated()||grazeConfigured||displaySeed>65535
        ||count<0||count>99999999||power<0||power>10000){
        operationError="Graze runtime requires explicit bounded initial globals before the first pass";return 0;
    }
    grazeGlobals={count,power};grazeConfigured=true;return 1;
}
int th128_world_enable_player_context(const void* animation,std::uint32_t size,int state,unsigned flags,int counter,
    unsigned soundControlFlags,unsigned systemFlags,int systemCounter,int systemMode,int hitControlCounter,int perfectCharge){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!world->positionIntegrated()||!grazeConfigured||playerConfigured
        ||!th128::runtime::inputRange(animation,size)||size>8u*1024u*1024u||state<0||state>4||counter<0||counter>65535
        ||systemCounter<0||systemCounter>65535||perfectCharge<0||perfectCharge>30000){
        operationError="Player-contact diagnostics require a fresh world and explicit bounded context";return 0;
    }
    if(debugRecordPrefix.worldBound&&(!debugRecordPrefix.sessionPresent
        ||debugRecordPrefix.sessionGeneration!=worldGeneration
        ||soundControlFlags!=debugRecordPrefix.sessionFlags60)){
        operationError="player sound control disagrees with the actual owned native record session";return 0;
    }
    const auto actualSoundControlFlags=debugRecordPrefix.worldBound
        ?debugRecordPrefix.sessionFlags60:soundControlFlags;
    const th128::player::hit::Timer zero{-1,0,0,0x4b6f58,1};
    const th128::player::hit::Timer protection{counter-1,counter,float(counter),0x4b6f58,1};
    const th128::player::HitHostSnapshot snapshot{{state,flags,counter},zero,zero,protection,
        {true,actualSoundControlFlags},{systemFlags,systemCounter,0},{true,systemMode}};
    auto candidate=std::make_unique<th128::player::PlayerHitHost>();
    if(!candidate->configure(animation,size,snapshot,
        [](int script,th128::player::hit::Position position,int layer,std::string& error){
            return bool(cancellationEffects->spawn(script,position.horizontal,position.vertical,position.depth,layer,error));
        },[](int identifier,std::string& error){
            if(grazeSounds.size()>=65536){error="Player sound request budget exceeded";return false;}
            grazeSounds.push_back({identifier,0});return true;
        },operationError))return 0;
    if(!candidate->configureLaserEffects([]{return world->effectGameRandom32();},operationError))return 0;
    playerHit=std::move(candidate);playerHitControl={true,hitControlCounter};
    playerWindowControl={true,systemMode,perfectCharge,0};playerConfigured=true;return 1;
}
int th128_world_enable_player_motion(int horizontal,int vertical){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!playerConfigured||playerMotion||playerHit->snapshot().gate.stateF78!=1
        ||horizontal<th128::player::MinimumHorizontalRaw||horizontal>th128::player::MaximumHorizontalRaw
        ||vertical<th128::player::MinimumVerticalRaw||vertical>th128::player::MaximumVerticalRaw){
        operationError="Normal player movement requires an explicit valid initial raw position before the first pass";return 0;
    }
    const auto pixels=th128::player::toPixels({horizontal,vertical});
    if(!world->setPlayerAimPosition({pixels.horizontal,pixels.vertical,0},operationError))return 0;
    playerRaw={horizontal,vertical};playerInput={};playerMotion=true;return 1;
}
int th128_world_input(unsigned holding,unsigned pressed,unsigned released){
    operationError.clear();
    if(ownedBattle.attempted()){operationError="retired gameplay input owner is unavailable";return 0;}
    if(world->faulted()||!playerConfigured||!playerMotion||(holding|pressed|released)>65535){operationError="Invalid player input state";return 0;}
    const auto perfectInput=perfectConfigured?th128::perfect::XKeyBit:0U;
    const auto unsupported=(holding|pressed|released)&~(16U|32U|64U|128U|(rapidShotConfigured?512U|8U:0U)|(playerFireConfigured?1U:0U)|perfectInput);
    if(unsupported){operationError="Shooting, focus and freeze input lifecycles are not yet integrated";return 0;}
    playerInput.holding=std::uint16_t(holding);playerInput.pressed|=std::uint16_t(pressed);playerInput.released|=std::uint16_t(released);return 1;
}
int th128_world_set_debug_infinite_respawn(int enabled){
    operationError.clear();
    if(world->faulted()||enabled<0||enabled>1){operationError="debug infinite respawn requires a legal unfaulted world and 0/1";return 0;}
    if(debugSetterExecuting){operationError="debug infinite respawn setter is non-reentrant";return 0;}
    if(worldTickExecuting){operationError="debug infinite respawn setter is not available during world tick";return 0;}
    struct SetterGuard {bool& flag;explicit SetterGuard(bool& value):flag(value){flag=true;}~SetterGuard(){flag=false;}} guard(debugSetterExecuting);
    debugInfiniteRespawnSession=enabled;
    debugInfiniteRespawnRun=enabled!=0;
    if(enabled)debugRunTainted=true;
    return 1;
}
int th128_world_enable_rapid_shot(int initialCount){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!shotHitsConfigured||rapidShotConfigured||initialCount< -1||initialCount>14){
        operationError="rapid shot cycle requires a fresh supported shot/hit context";return 0;
    }
    rapidShotCount=initialCount;rapidShotConfigured=true;return 1;
}
int th128_world_enable_z_fire(const void* animation,unsigned animationSize,const void* masks,unsigned masksSize){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!rapidShotConfigured||!shotHitsConfigured||!perfectHudConfigured
        ||!playerRespawnConfigured||playerFireConfigured||!th128::runtime::inputRange(animation,animationSize)
        ||!th128::runtime::inputRange(masks,masksSize)||animationSize>8u*1024u*1024u||masksSize!=1228){
        operationError="Z fire requires fresh owning player/ice/sound/HUD/damage/item services and original masks";return 0;
    }
    auto ice=std::make_unique<th128::ice::Host>();auto fire=std::make_unique<th128::player::fire::Host>();
    th128::ice::Services services;
    services.random=[]{return world->effectGameRandom32();};services.displayRandom=[]{return worldDisplayRandom.next32();};
    services.visual=[](std::uint32_t identifier)->const th128::bullet::BulletVisuals::Visual*{
        const auto found=bulletAnimations.find(identifier);return found==bulletAnimations.end()?nullptr:&found->second;
    };
    services.consume=[](const th128::bullet::Birth& bullet,std::string& error){
        if(!world->consumeFrozenBullet(bullet.slot,bullet.id,error))return false;
        if(bulletAnimations.erase(bullet.id)!=1){error="frozen bullet source VM missing after transfer";return false;}return true;
    };
    services.sound=[](int identifier,float position,std::string& error){
        if(grazeSounds.size()>=65536){error="ice sound journal budget";return false;}
        grazeSounds.push_back({identifier,position});return true;
    };
    services.damage=[](th128::ice::Position position,float radius,std::string& error){
        return bool(damageFields->spawn({position.x,position.y,position.z},radius,0,1,70,1,error));
    };
    services.expire=[](th128::ice::Position position,std::string& error){
        return cancellationEffects->spawn(375,320+position.x,16+position.y,position.z,23,error)
            &&itemHost->spawn(9,{position.x,position.y,position.z},0,0,error);
    };
    services.complete=[](th128::ice::Completion completion,std::string& error){
        const auto accounting=th128::freeze::account(completion.pixels,scoreRaw);
        const auto reward=accounting?th128::freeze::areaRewardRaw(accounting->percent,completion.power):std::nullopt;
        const auto oldLevel=th128::playershot::selectShotGroup(cumulativeFrozenArea,shotDifficulty);
        if(!accounting||!reward||!oldLevel){error="unsupported original freeze completion accounting";return false;}
        const float nextArea=cumulativeFrozenArea+accounting->percent;
        const auto nextLevel=th128::playershot::selectShotGroup(nextArea,shotDifficulty);
        if(!nextLevel){error="unsupported cumulative freeze shot level";return false;}
        cumulativeFrozenArea=nextArea;lastFrozenArea=accounting->percent;
        if(*nextLevel>*oldLevel){
            if(!spellsConfigured){error="original shot level-up visual owner unavailable";return false;}
            const auto position=world->playerAimPosition();if(!spellVisuals->levelUp(int(*nextLevel),{position.x,position.y,position.z},error))return false;
            if(playerShotGroupsConfigured&&!playerShotGroups->updateSelection(
                cumulativeFrozenArea,shotDifficulty,0,{position.x,position.y,position.z},error))return false;
            grazeSounds.push_back({13,0});
        }
        const int old=playerWindowControl.perfectCharge4b4d70;
        const int charge=th128::freeze::addPerfectFreezeRaw(old,*reward);
        if(!perfectHud->charge(charge,error))return false;
        if(old/10000<charge/10000){
            if(grazeSounds.size()>=65536){error="freeze reward sound journal budget";return false;}
            grazeSounds.push_back({72,0});if(!perfectHud->notice(error))return false;
        }
        playerWindowControl.perfectCharge4b4d70=charge;scoreRaw=accounting->score;freezeBonus=accounting->bonus;
    if(!freezeRewardPanel.complete(accounting->percent,accounting->bonus,world->playerAimPosition().y)){
        error="original freeze reward panel completion rejected";return false;
    }
        return true;
    };
    services.perfectComplete=[](th128::ice::Completion completion,std::string& error){
        if(completion.power!=12000){error="perfect callback must retain native power12000";return false;}
        if(!settlePerfectFreezeArea(completion.pixels,error))return false;
        perfectBatchOwned=true;return true;
    };
    auto* icePointer=ice.get();
    if(!ice->configure({static_cast<const std::uint8_t*>(animation),animationSize},
        {static_cast<const std::uint8_t*>(masks),masksSize},*bulletVisuals,std::move(services),operationError)
        ||!fire->configure(rapidShotCount,playerHit->snapshot().heldTimer,operationError))return 0;
    if(!itemHost->enableIceItems(playerAttractionSpeed,operationError))return 0;
    if(!playerHit->setSoundOperationHost([](int operation,int identifier,std::string& error){
        if(grazeSounds.size()>=65536){error="hit charge sound operation journal budget";return false;}
        grazeSounds.push_back({identifier,world->playerAimPosition().x,operation});return true;
    },operationError))return 0;
    auto perfect=std::make_unique<th128::perfect::Host>();
    perfectConfigError.clear();
    if(!perfect->loadNativeEffect({static_cast<const std::uint8_t*>(animation),animationSize},perfectConfigError)){
        perfectHost=std::make_unique<th128::perfect::Host>();perfectConfigured=false;
    }else{
        th128::perfect::Services perfectServices;
        perfectServices.protect=[](int ticks,std::string& error){
            if(ticks==th128::perfect::ProtectionTicks)return playerHit->assignPerfectProtection200(error);
            if(ticks==th128::perfect::PhaseFreezeProtectionTicks)return playerHit->assignPerfectProtection10(error);
            error="unsupported perfect protection ticks="+std::to_string(ticks);return false;
        };
        perfectServices.freezeBullets=[icePointer](const th128::perfect::BulletRequest&,
            th128::perfect::BulletResult& result,std::string& error){
            const auto liveLasers=world->laserObjectCount();
            if(!liveLasers||*liveLasers!=0){error="Perfect phase120 requires actual laser freeze owner, not static Empty";return false;}
            th128::ice::PerfectFreezeResult frozen;
            if(grazeSounds.size()>65533){error="perfect phase120 sound journal budget";return false;}
            if(!icePointer->freezePerfect(world->bullets().snapshots(),frozen,error))return false;
            if(!frozen.completed){error="perfect ice batch did not complete";return false;}
            const float perfectSoundX=world->playerAimPosition().x;
            grazeSounds.push_back({47,perfectSoundX,0});
            grazeSounds.push_back({69,0,0});
            grazeSounds.push_back({51,0,1});
            if(frozen.frozen>0)perfectBatchOwned=true;
            result={frozen.frozen,0,frozen.skipped,true};return true;
        };
        perfectServices.shatterIce=[icePointer](std::string& error){
            return icePointer->shatterPerfect(error);
        };
        th128::perfect::Context context;
        context.perfectCharge=playerWindowControl.perfectCharge4b4d70;
        context.x=world->playerAimPosition().x;context.y=world->playerAimPosition().y;
        context.radius=playerProfileRadius;
        context.systemPresent=playerWindowControl.present;
        context.systemMode3c=playerWindowControl.mode3c;
        context.busy=0;context.playerState=playerHit->snapshot().gate.stateF78;
        context.mode=th128::perfect::Mode::Normal;
        const auto initialLaserCount=world->laserObjectCount();
        if(!initialLaserCount||*initialLaserCount!=0){operationError="Perfect configuration requires actual owned empty laser manager";return 0;}
        context.lasers=th128::perfect::LaserContext::Empty;context.laserCount=int(*initialLaserCount);
        if(context.radius<=0||!perfect->configure(context,std::move(perfectServices),perfectConfigError)){
            if(perfectConfigError.empty())perfectConfigError="perfect player radius is unavailable";
            perfectHost=std::make_unique<th128::perfect::Host>();perfectConfigured=false;
        }else{
            perfectHost=std::move(perfect);perfectConfigured=true;
        }
    }
    iceHost=std::move(ice);playerFire=std::move(fire);playerFireConfigured=true;return 1;
}
int th128_world_enable_player_respawn(int mode04,int value08,const void* targets,unsigned targetCount){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!playerDeathConfigured||playerRespawnConfigured||targetCount!=8
        ||!th128::runtime::inputRange(targets,32)){operationError="respawn requires a fresh death host and explicit control callback snapshot";return 0;}
    th128::player::respawn::ControlState candidate;candidate.mode04=mode04;candidate.value08=value08;
    const auto* bytes=static_cast<const std::uint8_t*>(targets);
    for(std::size_t index=0;index<8;++index){
        const auto offset=index*4;candidate.targets[index]=std::uint32_t(bytes[offset])|std::uint32_t(bytes[offset+1])<<8|
            std::uint32_t(bytes[offset+2])<<16|std::uint32_t(bytes[offset+3])<<24;
    }
    respawnControl=candidate;playerRespawnConfigured=true;return 1;
}
int th128_world_enable_perfect_hud(const void* bytes,unsigned size){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!playerDeathConfigured||perfectHudConfigured||!th128::runtime::inputRange(bytes,size)||size>8u*1024u*1024u){
        operationError="Perfect Freeze HUD requires a fresh death context and actual front resource";return 0;
    }
    auto candidate=std::make_unique<th128::hud::PerfectHud>();
    if(!candidate->load({static_cast<const std::uint8_t*>(bytes),size},operationError)||!candidate->boot(playerWindowControl.perfectCharge4b4d70,operationError)
        ||!candidate->rank(worldStartDifficulty,operationError))return 0;
    auto boss=std::make_unique<th128::hud::BossHud>();
    if(!boss->load({static_cast<const std::uint8_t*>(bytes),size},operationError)||!boss->boot(operationError))return 0;
    auto pauseOwner=std::make_unique<th128::pause::Host>();
    try{pauseOwner->load({static_cast<const std::uint8_t*>(bytes),size});if(!originalPauseBackdropBytes.empty())pauseOwner->loadBackdrop(originalPauseBackdropBytes);}
    catch(const std::exception& failure){operationError=failure.what();return 0;}
    originalPauseMenu=std::move(pauseOwner);originalPauseSoundCursor=0;originalContinueCount=0;
    perfectHud=std::move(candidate);bossHud=std::move(boss);perfectHudConfigured=true;return 1;
}
int th128_world_load_score_font(const void* bytes,unsigned size){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!th128::runtime::inputRange(bytes,size)||size>8u*1024u*1024u){operationError="score font requires fresh original ascii bank";return 0;}
    if(!enemyVisuals->load(11,bytes,size,operationError))return 0;
    if(!scoreFont.loadBank(11,static_cast<const std::uint8_t*>(bytes),size,operationError))return 0;
    scoreFontLoaded=true;return 1;
}
int th128_world_enable_player_death(int awardSelector){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!shotHitsConfigured||playerDeathConfigured||awardSelector< -1||awardSelector>8){
        operationError="player death requires a fresh explicit awards/shot context";return 0;
    }
    deathAwardSelector=awardSelector;playerDeathConfigured=true;return 1;
}
int th128_world_enable_player_shots(const void* shot,std::uint32_t shotSize,const void* animation,std::uint32_t animationSize,
    const void* damageFlags,std::uint32_t flagCount,float cumulativeArea,int difficulty,unsigned birthCallback,unsigned tickCallback,
    unsigned unknownCallback,unsigned collisionCallback){
    operationError.clear();
    const bool extra=worldStartedName=="st_ex.ecl"&&worldStartDifficulty==4;
    if(world->faulted()||world->frame()!=0||!playerConfigured||!playerMotion||playerShotsConfigured
        ||!th128::runtime::inputRange(shot,shotSize)||!th128::runtime::inputRange(animation,animationSize)
        ||flagCount!=playerDamageSourceFlags.size()||!th128::runtime::inputRange(damageFlags,flagCount*4u)
        ||!std::isfinite(cumulativeArea)||cumulativeArea<0||cumulativeArea>1048576||difficulty<0||difficulty>4
        ||(difficulty==4&&!extra)||(extra&&(difficulty!=worldStartDifficulty||cumulativeArea!=cumulativeFrozenArea))
        ||birthCallback||tickCallback||unknownCallback||collisionCallback){
        operationError="Player-shot diagnostic requires an explicit fresh context and supported live callback bindings";return 0;
    }
    const auto* flags=static_cast<const std::uint8_t*>(damageFlags);
    for(std::size_t index=0;index<playerDamageSourceFlags.size();++index){
        const auto offset=index*4;const auto value=std::uint32_t(flags[offset])|std::uint32_t(flags[offset+1])<<8|
            std::uint32_t(flags[offset+2])<<16|std::uint32_t(flags[offset+3])<<24;
        if(value&1u){operationError="Active secondary damage-source lifecycle is unimplemented";return 0;}
    }
    auto candidate=std::make_unique<th128::playershot::PlayerShotHost>();
    if(!candidate->load({static_cast<const std::uint8_t*>(shot),shotSize},{static_cast<const std::uint8_t*>(animation),animationSize},operationError))return 0;
    auto groups=std::make_unique<th128::playershot::PlayerShotGroups>();
    if(!groups->load({static_cast<const std::uint8_t*>(shot),shotSize},
                     {static_cast<const std::uint8_t*>(animation),animationSize},operationError))return 0;
    const auto playerPose=world->playerAimPosition();
    if(!groups->configureNativeDefaultSelection(cumulativeArea,difficulty,0,
                                               {playerPose.x,playerPose.y,playerPose.z},operationError))return 0;
    candidate->setCallbackBindings({birthCallback,tickCallback,unknownCallback,collisionCallback});
    candidate->setSoundHost([](int identifier,float position,std::string& error){
        if(grazeSounds.size()>=65536){error="player shot sound journal budget";return false;}
        grazeSounds.push_back({identifier,position});return true;
    });
    if(!world->setEnemyDamageHost([](const th128::EnemySnapshot& enemy,th128::enemy::Bounds bounds,std::string& error)
        ->std::optional<th128::EnemyDamageSample>{
        if(!playerShotsConfigured){error="player shot context unavailable";return std::nullopt;}
        const auto timer=playerHit->snapshot().stateTimer;
        const auto plan=playerShots->queryEnemy({{enemy.x,enemy.y,enemy.z},{bounds.width,bounds.height}},
            std::uint32_t(timer.previous),std::uint32_t(timer.current),playerDamageSourceFlags,error);
        if(!plan||plan->continuation!=th128::shotenemy::QueryContinuation::Complete)return std::nullopt;
        const auto spell=world->phaseSpellContext();
        return th128::EnemyDamageSample{*plan,playerHit->snapshot().gate.stateF78,shotDamageBypass,
            spell?std::optional<std::uint32_t>(spell->flags):std::nullopt};
    },operationError))return 0;
    playerShots=std::move(candidate);cumulativeFrozenArea=cumulativeArea;shotDifficulty=difficulty;
    playerShotGroups=std::move(groups);playerShotGroupsConfigured=true;
    for(std::size_t index=0;index<playerDamageSourceFlags.size();++index){
        const auto offset=index*4;playerDamageSourceFlags[index]=std::uint32_t(flags[offset])|std::uint32_t(flags[offset+1])<<8|
            std::uint32_t(flags[offset+2])<<16|std::uint32_t(flags[offset+3])<<24;
    }
    playerShotsConfigured=true;return 1;
}
int th128_world_enable_shot_hits(int motivation,int flag,int managerBusy,int damageBypass,unsigned spellFlags){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!playerShotsConfigured||shotHitsConfigured||motivation<0||motivation>100000){
        operationError="shot hit context requires fresh real shots and explicit bounded motivation/system fields";return 0;
    }
    const std::vector<th128::damagefield::Image> emptyFields(th128::damagefield::Capacity);
    if(!damageFields->configure(emptyFields,0,operationError))return 0;
    if(!playerShots->setDamageSourceHost([](const th128::shotenemy::EnemyContactShape& enemy,std::uint32_t flag,std::string& error){
        return damageFields->query({enemy.combinedPosition[0],enemy.combinedPosition[1],enemy.combinedPosition[2]},flag,error);
    })||!playerShots->setScoreHost([](int damage,std::string& error){
        const auto scored=th128::damagefield::finishDamageQuery(0,damage,scoreRaw);
        if(!scored){error="unsupported native shot score snapshot";return false;}
        scoreRaw=scored->score;return true;
    })||!playerShots->setImpactHost([](std::string& error){
        const auto prefix=th128::shotenemy::planShotQueryImpact(motivationRaw,motivationFlag);
        if(!prefix){error="unsupported shot motivation snapshot";return false;}
        motivationRaw=prefix->value;motivationFlag=prefix->flag;
        if(prefix->soundRequired){
            if(grazeSounds.size()>=65536){error="shot impact sound journal budget";return false;}
            if(!perfectHudConfigured){error="motivation notification requires the actual front HUD owner";return false;}
            grazeSounds.push_back({18,0});
            if(!perfectHud->motivationNotice(error))return false;
        }
        const auto old=grazeGlobals.powerRaw;
        const bool busy=playerFireConfigured?iceHost->statistics().busy!=0:shotHitBusy!=0;
        grazeGlobals.powerRaw=th128::freeze::addPowerRaw(old,busy?10:100);
        for(const auto [threshold,identifier]:{std::pair{10000,74},std::pair{3000,73}}){
            if(old<threshold&&grazeGlobals.powerRaw>=threshold){
                if(grazeSounds.size()>=65536){error="shot power sound journal budget";return false;}
                grazeSounds.push_back({identifier,0});
            }
        }
        return true;
    })||!playerShots->enableContactAnimations()){
        operationError="shot hit callbacks cannot be installed";return 0;
    }
    motivationRaw=motivation;motivationFlag=flag;shotHitBusy=managerBusy;shotDamageBypass=damageBypass;shotSpellFlags=spellFlags;
    if(!world->setPhaseSpellContext({spellFlags,0,0,0},operationError))return 0;
    const auto emitDrops=[](const th128::EnemySnapshot& enemy,std::string& error){
        if(enemy.drop&&!itemHost->spawn(enemy.drop,{enemy.x,enemy.y,enemy.z},-1.5707963705062866f,2.200000047683716f,error))return false;
        th128::enemy::AdditionalDropOperation drops(enemy.drops,{enemy.x,enemy.y,enemy.z});
        if(!drops.run([]{return world->effectGameRandom32();},[](const th128::enemy::DropEmission& birth,std::string& reason){
            return itemHost->spawn(birth.kind,{birth.position.x,birth.position.y,birth.position.z},birth.angle,birth.speed,reason);
        },error))return false;
        return true;
    };
    if(!world->setEclDropHost(emitDrops,operationError))return 0;
    if(!world->setEnemyDeathHost([emitDrops](const th128::EnemySnapshot& enemy,std::string& error){
        if(grazeSounds.size()>=65534){error="enemy death sound journal budget";return false;}
        grazeSounds.push_back({enemy.deathSound,enemy.x});
        if(!cancellationEffects->spawn(enemy.deathScript,320+enemy.x,16+enemy.y,enemy.z,4,error)||!emitDrops(enemy,error))return false;
        const auto old=grazeGlobals.powerRaw;grazeGlobals.powerRaw=th128::freeze::addPowerRaw(old,300);
        for(const auto [threshold,identifier]:{std::pair{10000,74},std::pair{3000,73}})
            if(old<threshold&&grazeGlobals.powerRaw>=threshold)grazeSounds.push_back({identifier,0});
        ++enemiesDefeated;return true;
    },operationError,true))return 0;
    shotHitsConfigured=true;return 1;
}
int th128_world_emit_player_shots(int shotCount){
    operationError.clear();
    if(world->faulted()||!playerShotsConfigured||playerHit->snapshot().gate.stateF78!=1||shotCount<0||shotCount>14){
        operationError="Explicit player-shot diagnostic is not available in this state";return 0;
    }
    const auto position=world->playerAimPosition();
    if(!playerShots->emitCycle(cumulativeFrozenArea,shotDifficulty,shotCount,{position.x,position.y,.1f},operationError)){
        world->reportRuntimeFault(operationError);return 0;
    }
    return 1;
}
int th128_world_load_enemy_anm(int bank,const void* data,std::uint32_t size) {
    operationError.clear();
    const bool extra=worldStartedName=="st_ex.ecl"&&worldStartDifficulty==4;
    const int maximum=extra?4:3;
    if(!world->configurationOpen()||bank<1||bank>maximum||!th128::runtime::inputRange(data,size)) {
        operationError="Enemy appearance loading requires native ANIM bank1..3 and an unexecuted world";return 0;
    }
    th128::anm::Registry candidate;
    if(!candidate.loadBank(bank,static_cast<const std::uint8_t*>(data),size,operationError))return 0;
    const auto resource=candidate.bank(bank);
    std::unordered_map<int,th128::enemy::Bounds> proven;
    for(std::size_t script=0;script<resource->scripts.size();++script) {
        const auto bounds=th128::enemy::constantSpriteBounds(*resource,int(script));
        if(bounds)proven.emplace(int(script),*bounds);
    }
    if(bank==1&&!proven.contains(0)){operationError="Enemy ANM script0 is not a proven constant-size appearance";return 0;}
    if(!enemyVisuals->load(bank,data,size,operationError))return 0;
    enemyVisuals->setRandomSources([]{return world->effectGameRandom32();},{});
    enemyAppearances[bank]=std::move(proven);
    return 1;
}
int th128_world_add_bullet_profile(int type,const void* bytes,unsigned size){
    operationError.clear();
    if(!th128::runtime::inputRange(bytes,size)||world->faulted()||world->frame()!=0||world->spawnCount()!=0||bulletProfiles.empty()){
        operationError="typed profile requires valid bytes before stage entities execute";return 0;
    }
    th128::bullet::Profile candidate;
    if(!candidate.loadTyped(type,bytes,size,operationError)||!world->addBulletProfile(type,bytes,size,operationError))return 0;
    bulletProfiles[type]=std::move(candidate);return 1;
}
int th128_world_enable_bullets(const void* profile,std::uint32_t profileSize,const void* animation,
                              std::uint32_t animationSize,float playerX,float playerY) {
    operationError.clear();
    if(!th128::runtime::inputRange(profile,profileSize)||!th128::runtime::inputRange(animation,animationSize)) {
        operationError="Invalid bullet resource input span";return 0;
    }
    if(!world->configurationOpen()||!enemyAppearances.contains(1)) {
        operationError="Bullet runtime requires a fresh world and original constant-size enemy appearance data";return 0;
    }
    th128::bullet::Profile candidateProfile;
    if(!candidateProfile.load(profile,profileSize,operationError))return 0;
    auto candidate=std::make_unique<th128::bullet::BulletVisuals>();
    if(!candidate->load(animation,animationSize,operationError))return 0;
    auto candidateCancellations=std::make_unique<th128::effects::CancellationEffects>();
    if(!candidateCancellations->load(animation,animationSize,operationError))return 0;
    auto laserCandidate=std::make_unique<th128::laser::LineHost>();
    if(!laserCandidate->configure(animation,animationSize,
        [](th128::laser::LineSnapshot& line,std::string& error){
            if(!playerConfigured||!grazeConfigured||playerProfileRadius<=0){
                error="native laser collision 0x43c660 requires actual player hitbox/hit/graze owners";return false;
            }
            const auto player=world->playerAimPosition();
            const auto hit=playerHit->snapshot();
            const auto decision=th128::laser::classifyOverlap(line,
                {player,float(double(playerProfileRadius)*.5),float(double(playerProfileRadius)*.5),hit.gate,playerHitControl});
            if(!decision){error="native laser collision 0x43c660 numeric/reference domain is unsupported";return false;}
            if(*decision==th128::laser::CollisionKind::Miss)return true;
            if(*decision==th128::laser::CollisionKind::Graze){
                if(line.grazeClock%3==0&&!applyGrazeAt({player.x,player.y,player.z},error))return false;
                if(line.grazeClock==std::numeric_limits<int>::max()){error="laser graze clock budget";return false;}
                ++line.grazeClock;return true;
            }
            const auto returned=playerHit->confirmedOverlap({player.x,player.y,player.z},playerHitControl,error);
            if(!returned||*returned!=1){if(error.empty())error="native laser43d0c0 hit owner failed";return false;}
            return laserLines->cancelRegion(line,player,32,32,0,true,error);
        },[](int identifier,float position,std::string& error){
            if(grazeSounds.size()>=65536){error="laser sound journal budget";return false;}
            grazeSounds.push_back({identifier,position});return true;
        },[]{return world->effectGameRandom32();},operationError))return 0;
    if(!laserCandidate->setSelector3Services(
        [](std::string& error){return !playerConfigured||playerHit->beginLaserContactPhase(error);},
        [](std::string& error)->std::optional<th128::laser::selector3::FrameInput>{
            if(!playerConfigured||!playerFireConfigured){error="selector3 requires actual player and ice manager owners";return std::nullopt;}
            auto input=world->selector3FrameInput();if(!input){error="selector3 current world fields are unavailable";return std::nullopt;}
            for(const auto& ice:iceHost->objects())if(ice.delay.current<=0&&ice.lifetime.current>5)
                input->ice.push_back({{ice.position.x,ice.position.y,ice.position.z},float(double(ice.value)*.5)});
            return input;
        },
        [](unsigned columns,std::string& error){
            if(!playerConfigured||!shotHitsConfigured){error="selector3 contact requires actual motivation/player owners";return false;}
            if(!playerHit->laserColumnContactRequired(columns))return true;
            if(grazeSounds.size()>=65536){error="selector3 sound75 journal budget";return false;}
            grazeSounds.push_back({75,world->playerAimPosition().x});
            return playerHit->applyLaserColumnContact(columns,motivationRaw,motivationFlag,error)&&world->applyLaserSpellPenalty(columns,error);
        },
        [](std::string& error){if(!playerConfigured)return true;const auto player=world->playerAimPosition();return playerHit->endLaserContactPhase({player.x,player.y,player.z},error);},
        operationError))return 0;
    if(!laserCandidate->setRegionEffectOwner([](th128::bullet::Position position,std::string& error){
        if(!cancellationEffects->spawn(381,320+position.x,16+position.y,position.z,23,error))return false;
        return true;
    },operationError))return 0;
    auto candidateItems=std::make_unique<th128::item::ItemHost>();
    if(!candidateItems->load({static_cast<const std::uint8_t*>(animation),animationSize},operationError))return 0;
    candidateItems->setCollectHost([](int kind,th128::item::Position position,int state,std::string& error){
        if(kind==9){
            if(!playerFireConfigured){error="ice-item reward requires owning Z/ice context";return false;}
            if(!motivationFlag){
                const int next=motivationRaw+1;
                const bool notice=next/10000>motivationRaw/10000;
                if(notice){
                    if(!perfectHudConfigured){error="ice-item notification requires the actual front HUD owner";return false;}
                    if(grazeSounds.size()>=65535){error="ice-item notice and collection sound journal budget";return false;}
                }
                // Original kind9 calls 0x427260(+1): value, sound18, front49, cap; then collect sound38.
                motivationRaw=next;
                if(notice){
                    grazeSounds.push_back({18,0});
                    if(!perfectHud->motivationNotice(error))return false;
                }
                motivationRaw=std::min(motivationRaw,100000);
            }
            scoreRaw=std::min(scoreRaw+1,999999999);
            if(grazeSounds.size()>=65536){error="ice-item collection sound journal budget";return false;}
            grazeSounds.push_back({38,position.x});return true;
        }
        const auto reward=th128::item::pointReward(pointValueRaw,world->playerAimPosition().y,state,kind);
        if(!reward){error="unimplemented item reward kind at 0x428b52";return false;}
        scoreRaw=std::min(scoreRaw+*reward/10,999999999);
        if(scorePopups.size()>=10)scorePopups.erase(scorePopups.begin());
        scorePopups.push_back({position.x,position.y,position.z,float(*reward),0});
        if(grazeSounds.size()>=65536){error="item collection sound journal budget";return false;}
        grazeSounds.push_back({38,position.x});return true;
    });
    candidateItems->setMissHost([](std::string&){itemScalar=std::clamp(itemScalar-4,-1024,1024);return true;});
    candidateCancellations->setGameRandom([]{return world->effectGameRandom32();});
    candidate->setRandomSources([]{return world->effectGameRandom32();},{});
    auto* visualRuntime=candidate.get();
    const bool configured=world->configureBullets(profile,profileSize,{playerX,playerY,0},
        [visualRuntime](th128::bullet::Birth& birth,std::string& error) {
            const auto profile=bulletProfiles.find(birth.type);
            if(profile==bulletProfiles.end()){error="required typed bullet visual profile is missing";return false;}
            th128::bullet::BulletVisuals::CreateInfo info;
            info.type=birth.type;info.color=birth.color;info.script=birth.script;
            info.birthInterrupt=birth.state.animationInterrupt;
            const auto secondary=profile->second.secondaryScript;
            if(secondary!=0){
                if(birth.type==25){
                    if(profile->second.script!=102||secondary!=103){
                        error="type25 secondary bullet VM requires its exact 102/103 profile";return false;
                    }
                }else if(birth.type==30){
                    if(profile->second.script!=285||secondary!=286
                        ||profile->second.tail.hitboxSize!=4.f||profile->second.tail.auxiliarySize!=22.f
                        ||profile->second.tail.freezePolicy!=1||!profile->second.identitySpriteRemap()
                        ||birth.cancellationScript!=381){
                        error="type30 secondary bullet VM requires its exact 285/286 profile";return false;
                    }
                }else if(birth.type==24){
                    if(profile->second.script!=283||secondary!=284
                        ||profile->second.tail.hitboxSize!=6.f||profile->second.tail.auxiliarySize!=22.f
                        ||profile->second.tail.freezePolicy!=1||!profile->second.identitySpriteRemap()
                        ||birth.cancellationScript!=375){
                        error="type24 secondary bullet VM requires its exact 283/284 profile";return false;
                    }
                }else{
                    error="only exact type24/25/30 profiles may own a secondary bullet VM";return false;
                }
                info.secondaryScript=secondary;
            }else if(birth.type==25||birth.type==30){
                error="type25/type30 requires its original secondary bullet VM";return false;
            }
            info.identitySpriteRemap=profile->second.identitySpriteRemap();
            for(std::size_t index=0;index<info.spriteRemap.size();++index) {
                const auto mapped=profile->second.remap(birth.color,std::int32_t(index));
                if(!mapped){error="Unsupported original bullet color mapping";return false;}
                info.spriteRemap[index]=*mapped;
            }
            auto visual=birth.state.extensionIndex==0&&birth.state.extension0.type==0x100000
                ?visualRuntime->createInitialExtension1048576(info,[&birth](const th128::bullet::BulletVisuals::Visual&,std::string& reason){
                    return th128::bullet::initializeBirthExtension1048576(birth,world->playerAimPosition(),
                        [](int identifier,std::string& failure){
                            if(grazeSounds.size()>=65536){failure="extension sound journal budget";return false;}
                            grazeSounds.push_back({identifier,0});return true;
                        },reason);
                },error):visualRuntime->create(info,error);
            if(!visual)return false;
            if(!bulletAnimations.emplace(birth.id,std::move(*visual)).second) {
                error="Duplicate bullet visual identity";return false;
            }
            return true;
        },operationError);
    if(!configured)return 0;
    if(!world->setBulletType25Conversion([visualRuntime](const th128::bullet::Birth& birth,std::string& error){
        const auto owned=bulletAnimations.find(birth.id);
        const auto profile=bulletProfiles.find(25);
        if(owned==bulletAnimations.end()||profile==bulletProfiles.end()||birth.type!=25||birth.script!=102
            ||profile->second.script!=102||profile->second.secondaryScript!=103||!profile->second.identitySpriteRemap()
            ||birth.state.hitboxSize!=6.f||birth.state.auxiliarySize!=22.f||birth.state.profileValue!=2||birth.state.freezePolicy!=1){
            error="type25 conversion requires same live bullet visual and exact102/103 profile";return false;
        }
        return visualRuntime->convertType25(owned->second,birth.color,error);
    },operationError))return 0;
    if(!world->setEclEffectHost([](int bank,int script,std::array<float,3> position,int layer,bool head,std::string& error){
        return standaloneEclEffect(bank,script,position,layer,head,error);
    },operationError))return 0;
    if(!world->setEclRadiusCancellation([](th128::bullet::Birth& bullet,std::string& error){
        if(bullet.type==30){
            const auto owned=bulletAnimations.find(bullet.id);
            const auto profile=bulletProfiles.find(30);
            if(owned==bulletAnimations.end()||profile==bulletProfiles.end()
                ||profile->second.script!=285||profile->second.secondaryScript!=286
                ||profile->second.tail.hitboxSize!=4.f||profile->second.tail.auxiliarySize!=22.f
                ||profile->second.tail.freezePolicy!=1||!profile->second.identitySpriteRemap()
                ||profile->second.cancellationScript(0)!=381||profile->second.cancellationScript(6)!=381
                ||bullet.script!=285||bullet.cancellationScript!=381){
                error="type30 radius cancellation requires its owned profile/pair/cache";return false;
            }
            if(!owned->second.hasSecondary||owned->second.primary.state().script!=285
                ||owned->second.secondary.state().script!=286){
                error="type30 radius cancellation requires the owned 285/286 visual pair";return false;
            }
            if(!bulletVisuals->animationState(owned->second,false)||!bulletVisuals->animationState(owned->second,true)){
                error="type30 radius cancellation requires current visual ownership";return false;
            }
            const auto effect=cancellationEffects->spawn(bullet.cancellationScript,320+bullet.state.position.x,
                16+bullet.state.position.y,bullet.state.position.z,23,error);
            if(!effect){error="type30 radius cancellation effect: "+error;return false;}
            if(!cancellationEffects->initializeNativeDrift(*effect,{bullet.state.velocity.x,
                bullet.state.velocity.y,bullet.state.velocity.z},1,{0,0,0},error)){
                cancellationEffects->remove(*effect);return false;
            }
            if(!bulletVisuals->interrupt(owned->second,1,error)){cancellationEffects->remove(*effect);return false;}
            owned->second.secondary.interrupt(1);
            return true;
        }
        if(bullet.type==24){
            const auto owned=bulletAnimations.find(bullet.id);
            const auto profile=bulletProfiles.find(24);
            if(owned==bulletAnimations.end()||profile==bulletProfiles.end()
                ||profile->second.script!=283||profile->second.secondaryScript!=284
                ||profile->second.tail.hitboxSize!=6.f||profile->second.tail.auxiliarySize!=22.f
                ||profile->second.tail.freezePolicy!=1||!profile->second.identitySpriteRemap()
                ||profile->second.cancellationScript(0)!=375
                ||bullet.script!=283||bullet.cancellationScript!=375){
                error="type24 radius cancellation requires its owned profile/pair/cache";return false;
            }
            if(!owned->second.hasSecondary||owned->second.primary.state().script!=283
                ||owned->second.secondary.state().script!=284){
                error="type24 radius cancellation requires the owned 283/284 visual pair";return false;
            }
            if(!bulletVisuals->animationState(owned->second,false)||!bulletVisuals->animationState(owned->second,true)){
                error="type24 radius cancellation requires current visual ownership";return false;
            }
            const auto effect=cancellationEffects->spawn(bullet.cancellationScript,320+bullet.state.position.x,
                16+bullet.state.position.y,bullet.state.position.z,23,error);
            if(!effect){error="type24 radius cancellation effect: "+error;return false;}
            if(!cancellationEffects->initializeNativeDrift(*effect,{bullet.state.velocity.x,
                bullet.state.velocity.y,bullet.state.velocity.z},1,{0,0,0},error)){
                cancellationEffects->remove(*effect);return false;
            }
            if(!bulletVisuals->interrupt(owned->second,1,error)){cancellationEffects->remove(*effect);return false;}
            owned->second.secondary.interrupt(1);
            return true;
        }
        const auto found=bulletAnimations.find(bullet.id);
        if(found==bulletAnimations.end()||(found->second.hasSecondary&&bullet.type!=25)){error="ECL radius cancel requires an owned single-VM bullet";return false;}
        if(!bulletVisuals->interrupt(found->second,1,error))return false;
        if(bullet.cancellationScript>=0){
            if(!cancellationEffects->spawn(bullet.cancellationScript,320+bullet.state.position.x,16+bullet.state.position.y,bullet.state.position.z,23,error))return false;
        }
        return true;
    },laserCandidate->actualCount(),operationError))return 0;
    if(!world->setBulletSoundSink([](const th128::bullet::SoundRequest& request,std::string& error){
        if(grazeSounds.size()>=65536){error="game sound journal budget";return false;}
        grazeSounds.push_back(request);return true;
    },operationError))return 0;
    if(!world->setEclSoundSink([](int identifier,float position,std::string& error){
        if(grazeSounds.size()>=65536){error="ECL sound journal budget";return false;}
        grazeSounds.push_back({identifier,position});return true;
    },operationError))return 0;
    if(!world->setAnimationHost([](const th128::AnimationBirth& birth,std::string& error){
        const auto& request=birth.request;
        if(request.script<0)return enemyVisuals->retireSlot(birth.entity,request.slot,error);
        return enemyVisuals->create(birth.entity,request.slot,request.bank,request.script,request.layer,request.visibility,error);
    },[](std::uint32_t entity){if(!enemyVisuals->removeOwner(entity))throw std::runtime_error("actual enemy visual retirement rejected");},operationError))return 0;
    if(!world->setEnemyHitFeedbackHost([](const th128::EnemySnapshot& enemy,const th128::enemy::HitFeedbackInput& input,std::string& error){
        return enemyVisuals->applyHitFeedback(enemy.id,input,error);
    },operationError))return 0;
    if(!world->setAnimationVisibilityHost([](std::uint32_t entity,th128::enemy::VisibilityEffect effect,std::string& error){
        const auto visuals=enemyVisuals->snapshots();
        if(std::none_of(visuals.begin(),visuals.end(),[entity](const auto& visual){return visual.entity==entity;}))return true;
        if(enemyVisuals->visibility(entity,effect))return true;
        error="enemy visibility owner unavailable";return false;
    },operationError))return 0;
    if(!world->setPrimaryAppearanceBoundsProvider([](const th128::EnemySnapshot& entity,std::string& error){
        return enemyVisuals->primaryAppearanceBounds(entity.id,entity.animationBanks[0],entity.animation[0],error);
    },operationError))return 0;
    bulletProfiles[8]=std::move(candidateProfile);
    bulletAnimations.clear();bulletVisuals=std::move(candidate);
    cancellationEffects=std::move(candidateCancellations);
    itemHost=std::move(candidateItems);
    if(!enemyVisuals->load(0,animation,animationSize,operationError))return 0;
    if(!world->setLaserHost(laserCandidate.get(),operationError))return 0;
    laserLines=std::move(laserCandidate);
    if(!world->enableLocalEnemyPasses(operationError))return 0;
    return 1;
}
int th128_world_enable_messages(const void* data,unsigned size,const void* text,unsigned textSize,const void* player,unsigned playerSize,const void* boss,unsigned bossSize){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||messagesConfigured||!playerConfigured||!th128::runtime::inputRange(data,size)||!th128::runtime::inputRange(text,textSize)
        ||!th128::runtime::inputRange(player,playerSize)||!th128::runtime::inputRange(boss,bossSize)){operationError="message host requires fresh world and actual MSG/text/player/Boss resources";return 0;}
    auto candidate=std::make_unique<th128::message::Host>();auto visuals=std::make_unique<th128::message::Visuals>();
    if(!candidate->load({static_cast<const std::uint8_t*>(data),size},operationError)
        ||!visuals->load(6,{static_cast<const std::uint8_t*>(text),textSize},operationError)
        ||!visuals->load(7,{static_cast<const std::uint8_t*>(player),playerSize},operationError)
        ||!visuals->load(3,{static_cast<const std::uint8_t*>(boss),bossSize},operationError))return 0;
    if(!candidate->setEventHost([](const th128::message::Event& event,std::string& error){
        if(event.opcode==30){
            if(!routeChoiceConfigured)return messageVisuals->event(event,error);
            if(routeChoiceStarted){error="original MSG30 route choice was already started";return false;}
            if(!routeChoice->start({routeChoiceStage,routeChoiceMessage78,routeChoiceMessage80},error)){
                routeChoiceError=error;return false;
            }
            routeChoiceStarted=true;return true;
        }
        if(event.opcode==21){
            if(normalTerminal.resourcesConfigured){
                th128::stage::StageSettlementCallbacks callbacks;
                callbacks.setClearFlag=[](std::string& failure){return playerHit->applyStageClearStart(failure);};
                callbacks.slot=[](unsigned group,unsigned slot){return playerShotGroups->slot(group,slot);};
                callbacks.retireTree=[](unsigned group,unsigned slot,int code,unsigned& retired,std::string& failure){
                    const auto handle=playerShotGroups->slot(group,slot);retired=0;if(!handle)return true;
                    if(!playerShotGroups->walk(handle,[&](unsigned,th128::anm::VM&,std::string&){++retired;return true;},failure))return false;
                    return playerShotGroups->retireWithChildren(handle,code,failure);
                };
                callbacks.finish=[](std::string& failure){return playerHit->applyStageClearStart(failure);};
                return beginOrdinaryTerminalMsg21(event,callbacks,error);
            }
            if(debugTerminal.configured){
                if(event.entry!=1||debugTerminal.begun||debugTerminal.generation!=worldGeneration
                    ||stageIdForEcl(worldStartedName)!=debugTerminal.stage||!debugRunTainted){
                    error="debug terminal MSG21 owner identity or order mismatch";return false;
                }
                if(debugRecordPrefix.worldBound&&(!debugFreshReplay||debugRecordPrefix.failed
                    ||debugFreshReplayGeneration!=worldGeneration||debugFreshReplayEpoch!=debugRecordPrefix.epoch
                    ||debugFreshReplay->mode()!=0
                    ||(ordinaryRecordOwner.configured
                        ?debugTerminal.stage!=int(ordinaryRecordOwner.stage)
                            ||worldStartDifficulty!=int(ordinaryRecordOwner.rank)
                        :debugTerminal.stage!=16||worldStartDifficulty!=4)
                    ||debugRecordPrefix.clearRecordEvents!=0)){
                    error="original EX clear record lacks its actual recording-mode replay owner";return false;
                }
                th128::stage::StageSettlementContext terminalContext;
                terminalContext.stage=debugTerminal.stage;terminalContext.score=scoreRaw;terminalContext.manager=stageManager;
                th128::stage::StageSettlementCallbacks callbacks;
                callbacks.setClearFlag=[](std::string& failure){return playerHit->applyStageClearStart(failure);};
                callbacks.slot=[](unsigned group,unsigned slot){return playerShotGroups->slot(group,slot);};
                callbacks.retireTree=[](unsigned group,unsigned slot,int code,unsigned& retired,std::string& failure){
                    const auto handle=playerShotGroups->slot(group,slot);retired=0;if(!handle)return true;
                    if(!playerShotGroups->walk(handle,[&](unsigned,th128::anm::VM&,std::string&){++retired;return true;},failure))return false;
                    return playerShotGroups->retireWithChildren(handle,code,failure);
                };
                callbacks.finish=[](std::string& failure){return playerHit->applyStageClearStart(failure);};
                const bool extraTerminal=debugTerminal.stage==16;
                if(extraTerminal&&(worldStartDifficulty!=4||shotDifficulty!=4)){
                    error="extra terminal requires current original rank4 and shot4 owners";return false;
                }
                const bool settled=extraTerminal
                    ?debugTerminalSettlement->beginExtraTerminal(terminalContext,callbacks,error)
                    :debugTerminalSettlement->beginTerminal(terminalContext,callbacks,error);
                if(!settled)return false;
                const auto committed=debugTerminalSettlement->state();
                scoreRaw=committed.context.score;stageManager=committed.context.manager;
                const std::array<int,4> multipliers{5000,10000,20000,40000};
                const int quotient=pointValueRaw/100;
                const auto bonusBits=extraTerminal
                    ?std::uint32_t((std::int64_t(motivationRaw/100)+250)*40000)
                    :std::uint32_t(std::int64_t(quotient-quotient%10)*100
                        +std::int64_t(motivationRaw/100)*multipliers[std::size_t(worldStartDifficulty)]);
                const int bonus=std::bit_cast<std::int32_t>(bonusBits);
                scoreRaw=std::bit_cast<std::int32_t>(std::uint32_t(scoreRaw)+std::uint32_t(bonus/10));
                if(scoreRaw>=1000000000)scoreRaw=999999999;
                if(debugRecordPrefix.worldBound&&(extraTerminal||ordinaryRecordOwner.configured)){
                    th128::record::NativeRecordAccounting::recordClear(debugRecordPrefix.profile,actualRecordRoute(),
                        unsigned(worldStartDifficulty),debugFreshReplay->mode());
                    ++debugRecordPrefix.clearRecordEvents;
                }
                stageManager.flags5a04|=0x18u;
                stageManager.baseScore5a38=std::bit_cast<std::int32_t>(std::uint32_t(stageManager.baseScore5a38)+bonusBits);
                debugTerminal.bonus=bonus;debugTerminal.counter=0;debugTerminal.selector=extraTerminal?23:debugTerminal.index+17;
                debugTerminal.begun=true;return true;
            }
            if(!stageTransitionConfigured){error="original ordinary MSG21 payment has no stage-transition owner";return false;}
            th128::stage::StageTransitionContext context;
            context.currentId=stageTransitionCurrentId;
            context.progress=stageTransitionProgress;
            context.gate74=stageTransitionGate74;
            context.message78=stageTransitionMessage78;
            context.session={stageSessionPresent!=0,stageSessionField74};
            std::optional<std::int32_t> confirmedRouteMessage78;
            if(routeChoiceConfigured){
                const auto result=routeChoice->result();
                if(!result){error="stage transition requires a confirmed route-choice result";return false;}
                if(stageTransitionMessage78!=0&&stageTransitionMessage78!=*result){
                    error="stage transition message78 disagrees with the confirmed route-choice result";return false;
                }
                confirmedRouteMessage78=*result;
                context.message78=*result;
            }
            context.settlement.stage=stageTransitionCurrentId;
            context.settlement.score=scoreRaw;
            context.settlement.manager=stageManager;
            if(!playerConfigured||!playerShotGroupsConfigured){
                error="stage-clear owner requires the real pl00a shot groups";return false;
            }
            th128::stage::StageSettlementCallbacks settlementCallbacks;
            settlementCallbacks.setClearFlag=[](std::string& serviceError){
                return playerHit->applyStageClearStart(serviceError);
            };
            settlementCallbacks.slot=[](std::uint32_t group,std::uint32_t slot){
                return playerShotGroups->slot(group,slot);
            };
            settlementCallbacks.retireTree=[](std::uint32_t group,std::uint32_t slot,int code,
                                               std::uint32_t& retired,std::string& serviceError){
                const auto handle=playerShotGroups->slot(group,slot);
                if(handle==0){retired=0;return true;}
                std::uint32_t count=0;
                if(!playerShotGroups->walk(handle,[&](std::uint32_t,th128::anm::VM&,std::string&){
                    ++count;return true;
                },serviceError))return false;
                if(!playerShotGroups->retireWithChildren(handle,code,serviceError))return false;
                retired=count;return true;
            };
            settlementCallbacks.finish=[](std::string& serviceError){
                return playerHit->applyStageClearStart(serviceError);
            };
            th128::stage::StageTransitionCallbacks transitionCallbacks;
            transitionCallbacks.settlement=settlementCallbacks;
            transitionCallbacks.settlementCommitted=[](const th128::stage::StageSettlementState& settlement,
                                                        std::string&){
                scoreRaw=settlement.context.score;
                stageManager=settlement.context.manager;
                return true;
            };
            if(!stageTransition->begin(context,transitionCallbacks,error)){stageTransitionError=error;return false;}
            if(confirmedRouteMessage78)stageTransitionMessage78=*confirmedRouteMessage78;
            const auto settlement=stageTransition->settlementState();
            scoreRaw=settlement.context.score;
            stageManager=settlement.context.manager;
            return true;
        }
        if(event.opcode==22){
            if(event.entry!=1){error="original MSG22 is only accepted in the post-Boss entry";return false;}
            if(normalTerminal.resourcesConfigured&&!ordinaryTerminalContinuationReady(error))return false;
            if(!stageTransitionConfigured&&!debugTerminal.begun&&!normalTerminal.begun){error="original MSG22 requires an actual ordinary or debug terminal owner";return false;}
            if(musicControl.active){error="original MSG22 was already queued";return false;}
            const auto plan=th128::message::planMsg22({normalTerminal.begun?normalTerminal.selector:debugTerminal.begun?debugTerminal.selector:stageTransitionCurrentId,1.0f});
            if(plan.status!=th128::message::Msg22Status::BgmFadeOut){
                error="original MSG22 fade request is unsupported for the current stage";return false;
            }
            if(musicControlSerial==0xffffffffu)musicControlSerial=0;
            musicControl.serial=++musicControlSerial;
            musicControl.operation=5;
            musicControl.durationFrames=plan.request.durationFrames;
            musicControl.durationSeconds=plan.request.durationSeconds;
            musicControl.immediate=plan.request.immediate;
            musicControl.acknowledged=false;
            musicControl.active=true;
            if(normalTerminal.begun)normalTerminal.musicSerial=musicControl.serial;
            return true;
        }
        if(!messageVisuals->event(event,error))return false;
        if(event.opcode==0&&normalTerminal.begun&&!ordinaryTerminalMessageDone(event,error))return false;
        if(event.opcode==0&&debugTerminal.begun){
            if(event.entry!=1){error="debug terminal MSG0 entry mismatch";return false;}
            debugTerminal.messageDone=true;
        }
        if(event.opcode==0
            &&th128::stage::stageTransitionMessageCompletionApplies(
                stageTransitionConfigured,stageTransition->state().begun,event.entry)
            &&!stageTransition->messageDoneForEntry(event.entry,error)){
            stageTransitionError=error;return false;
        }
        if(event.opcode==19)++messageMusicSerial;
        if(event.opcode==11){if(grazeSounds.size()>=65536){error="message confirm sound journal budget";return false;}grazeSounds.push_back({0,0});}
        return true;
    },operationError))return 0;
    if(!world->setMessageHost([](int entry,std::string& error){return messages->start(entry,error);},[](std::string&)->std::optional<bool>{return messages->state().active&&!messages->state().eclReleased;},
        [](th128::bullet::Birth& bullet,std::string& error){
            const auto found=bulletAnimations.find(bullet.id);
            if(found==bulletAnimations.end()){error="MSG clear requires its actual owned bullet visual";return false;}
            const auto* primary=bulletVisuals->animationState(found->second);
            if(!primary||primary->script!=bullet.script){error="MSG clear primary bullet VM identity mismatch";return false;}
            if(found->second.hasSecondary){
                const auto* secondary=bulletVisuals->animationState(found->second,true);
                const bool type30=bullet.type==30&&bullet.script==285&&secondary&&secondary->script==286
                    &&bullet.state.hitboxSize==4.f&&bullet.state.auxiliarySize==22.f
                    &&bullet.state.profileValue==2&&bullet.state.freezePolicy==1&&bullet.cancellationScript==381;
                const bool type25=bullet.type==25&&bullet.script==102&&secondary&&secondary->script==103
                    &&bullet.state.hitboxSize==6.f&&bullet.state.auxiliarySize==22.f
                    &&bullet.state.profileValue==2&&bullet.state.freezePolicy==1&&bullet.cancellationScript==371;
                if(!type30&&!type25){error="MSG clear requires an exact original secondary bullet VM pair";return false;}
            }
            if(!bulletVisuals->interrupt(found->second,1,error))return false;
            if(found->second.hasSecondary)found->second.secondary.interrupt(1);
            if(bullet.cancellationScript>=0){
                const auto handle=cancellationEffects->spawn(bullet.cancellationScript,320+bullet.state.position.x,
                    16+bullet.state.position.y,bullet.state.position.z,23,error);
                if(!handle)return false;
                if(!cancellationEffects->initializeNativeDrift(*handle,{bullet.state.velocity.x,
                    bullet.state.velocity.y,bullet.state.velocity.z},1,{0,0,0},error))return false;
            }
            return true;
        },operationError))return 0;
    messages=std::move(candidate);messageVisuals=std::move(visuals);messagesConfigured=true;return 1;
}
int th128_world_enable_message_front(const void* front,unsigned size){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!messagesConfigured||messageFrontConfigured||!messages->configurationOpen()
        ||!th128::runtime::inputRange(front,size)||size<64||size>8u*1024u*1024u){
        operationError="message front requires original bank5 after MSG configuration and before any message or World tick";return 0;
    }
    if(!messageVisuals->load(5,{static_cast<const std::uint8_t*>(front),size},operationError))return 0;
    messageFrontConfigured=true;return 1;
}
int th128_world_enable_route_choice(const void* front,unsigned size,int stage,int message78,int message80){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||routeChoiceConfigured||!messagesConfigured
        ||!th128::runtime::inputRange(front,size)||size>8u*1024u*1024u
        ||(stage!=1&&stage!=6&&stage!=11)||message78!=0||message80!=0){
        operationError="route choice requires fresh world, configured MSG host, actual front bank, explicit stage 1/6/11 and a proven initial context";return 0;
    }
    auto candidate=std::make_unique<th128::routechoice::Host>();
    if(!candidate->load({static_cast<const std::uint8_t*>(front),size},operationError))return 0;
    if(!messages->setExternalWait({
        [](const th128::message::Event& event,std::string& error){
            if(event.opcode!=30)return false;
            if(!routeChoiceConfigured||!routeChoiceStarted){error="route choice external wait owner is not started";return false;}
            return true;
        },
        [](std::string& error)->std::optional<bool>{
            if(!routeChoiceConfigured||!routeChoiceStarted){error="route choice external wait owner is not started";return std::nullopt;}
            const auto phase=routeChoice->status().phase;
            if(phase==th128::routechoice::Phase::Failed){error=routeChoiceError.empty()?"route choice owner failed":routeChoiceError;return std::nullopt;}
            if(routeChoice->result())return false;
            if(phase==th128::routechoice::Phase::Done){error="route choice completed without a confirmed result";return std::nullopt;}
            return true;
        }
    },operationError))return 0;
    routeChoice=std::move(candidate);routeChoiceConfigured=true;routeChoiceStarted=false;
    routeChoiceStage=stage;routeChoiceMessage78=message78;routeChoiceMessage80=message80;
    routeChoiceInitialContextExplicit=true;routeChoiceInitialContextDefault=true;
    routeChoiceInitialContextKnown=false;
    routeChoiceError.clear();routeChoiceSoundConsumed=0;return 1;
}
int th128_world_enable_stage_transition(const void* front,unsigned size,int currentId,int progress,int gate74,int message78,int sessionPresent,int sessionField74){
    operationError.clear();
    const std::array<int,7> terminalStages{3,5,8,10,13,15,16};
    const bool extraTerminal=currentId==16;
    const auto terminalIndex=std::find(terminalStages.begin(),terminalStages.end(),currentId);
    if(terminalIndex!=terminalStages.end()){
        const bool incomingTerminalCarry=stageCarry.valid&&!stageCarry.consumed&&stageCarry.targetBound
            &&stageCarry.gameRandomRestored&&stageCarry.preInitBound&&stageCarry.restoredGeneration==worldGeneration
            &&stageCarry.targetWorldName==worldStartedName&&stageCarry.targetDifficulty==worldStartDifficulty
            &&stageCarry.difficulty==worldStartDifficulty&&stageCarry.nextId==currentId&&stageCarry.nextProgress==progress;
        if(!extraTerminal&&!debugRunTainted&&!debugInfiniteRespawnRun&&!debugRecordPrefix.worldBound&&!replayPlayback.active()){
            if(world->faulted()||world->frame()!=0||worldTickExecuting||normalTerminal.resourcesConfigured
                ||debugTerminal.configured||stageTransitionConfigured||!messagesConfigured||!playerConfigured||!playerShotGroupsConfigured
                ||!incomingTerminalCarry||!stageCarry.normalRecord||!stageCarry.normalReplay
                ||!ordinaryCarrySessionPresent(false)
                ||stageControl.current()!=7||stageControl.pending()!=7||stageControl.gateSet()
                ||stageIdForEcl(worldStartedName)!=currentId||worldStartDifficulty<0||worldStartDifficulty>3
                ||gate74!=0||message78!=0||sessionPresent!=1||sessionField74!=0
                ||!th128::runtime::inputRange(front,size)||size>8u*1024u*1024u){
                operationError="ordinary terminal resources require the actual fresh incoming normal carry and constructed session";return 0;
            }
            auto settlement=std::make_unique<th128::stage::StageSettlementHost>();
            if(!settlement->load({static_cast<const std::uint8_t*>(front),size},operationError))return 0;
            normalTerminal.resourcesConfigured=true;normalTerminal.generation=worldGeneration;
            normalTerminal.stage=currentId;normalTerminal.progress=progress;normalTerminal.settlement=std::move(settlement);
            return 1;
        }
        if(world->faulted()||world->frame()!=0||debugTerminal.configured||stageTransitionConfigured
            ||!messagesConfigured||!playerConfigured||!playerShotGroupsConfigured
            ||!debugInfiniteRespawnRun||!debugRunTainted||!stageControl.initialized()
            ||stageIdForEcl(worldStartedName)!=currentId
            ||(extraTerminal?(worldStartedName!="st_ex.ecl"||worldStartDifficulty!=4||shotDifficulty!=4
                ||progress!=0||incomingTerminalCarry):(worldStartDifficulty<0||worldStartDifficulty>3))
            ||(progress!=0&&!incomingTerminalCarry)||gate74!=0||message78!=0||sessionPresent!=1||sessionField74!=0
            ||!th128::runtime::inputRange(front,size)||size>8u*1024u*1024u){
            operationError="debug terminal requires exact fresh actual route/rank/player/front/session owners";return 0;
        }
        auto terminal=std::make_unique<th128::stage::StageSettlementHost>();
        if(!terminal->load({static_cast<const std::uint8_t*>(front),size},operationError))return 0;
        debugTerminalSettlement=std::move(terminal);debugTerminal.configured=true;
        debugTerminal.stage=currentId;debugTerminal.index=extraTerminal?-1:int(terminalIndex-terminalStages.begin());
        debugTerminal.generation=worldGeneration;return 1;
    }
    if(world->faulted()||world->frame()!=0||stageTransitionConfigured||!messagesConfigured
        ||!th128::runtime::inputRange(front,size)||size>8u*1024u*1024u
        ||(sessionPresent!=0&&sessionPresent!=1)||!stageControl.initialized()
        ||!th128::stage::StageSettlementHost::supportedStage(currentId)
        ||stageIdForEcl(worldStartedName)!=currentId||worldStartDifficulty<0||worldStartDifficulty>3){
        operationError="stage transition requires fresh world, configured MSG host, actual front, initialized stage control and ordinary stage id";return 0;
    }
    auto candidate=std::make_unique<th128::stage::StageTransitionHost>();
    if(!candidate->load({static_cast<const std::uint8_t*>(front),size},operationError)
        ||!candidate->attachControl(stageControl,operationError))return 0;
    if(!world->bindStageTransitionOwner(*candidate,currentId,progress,operationError))return 0;
    stageTransition=std::move(candidate);stageTransitionConfigured=true;
    stageTransitionCurrentId=currentId;stageTransitionProgress=progress;
    currentStageProgress=progress;
    stageTransitionGate74=gate74;stageTransitionMessage78=message78;
    stageSessionPresent=sessionPresent;stageSessionField74=sessionField74;
    stageTransitionError.clear();return 1;
}
unsigned th128_world_capture_stage_carry() {
    operationError.clear();
    if(!stageTransitionConfigured||!stageTransition->state().requested){
        operationError="stage carry capture requires a requested stage transition";return 0;
    }
    if(stageCarry.valid&&!stageCarry.consumed){
        operationError="an unconsumed stage carry token already exists";return 0;
    }
    if(perfectConfigured&&perfectHost->status().active){
        operationError="active Perfect Freeze cannot be represented in the opaque stage carry";return 0;
    }
    StageCarryRecord candidate;
    const auto transition=stageTransition->state();
    if(!transition.controlActionRecreate){
        operationError="stage carry capture requires the old source RecreateSession request";return 0;
    }
    const auto sourceRole=th128::stage::roleForStage(transition.currentId);
    if(!sourceRole||!th128::stage::canRequestOrdinaryHandoff(*sourceRole)){
        operationError="terminal/extra source cannot capture an ordinary stage carry";return 0;
    }
    if(stageTransitionSerialNext==0)stageTransitionSerialNext=1;
    candidate.sourceTransitionSerial=stageTransitionSerialNext++;
    if(stageTransitionSerialNext==0)stageTransitionSerialNext=1;
    candidate.sourceGeneration=worldGeneration;
    candidate.sourceCurrentId=transition.currentId;
    candidate.sourceProgress=transition.progress;
    candidate.nextId=transition.nextId;
    candidate.nextProgress=transition.nextProgress;
    candidate.sourceControlCurrent=transition.controlCurrent;
    candidate.sourceControlPending=transition.controlPending;
    candidate.sourceControlActionRecreate=transition.controlActionRecreate;
    candidate.sourceDestroyExisting=transition.controlDestroyExisting;
    candidate.sourceControlSessionArgument=transition.controlSessionArgument;
    candidate.sourceGate74=stageTransitionGate74;
    candidate.sourceMessage78=stageTransitionMessage78;
    candidate.sourceWorldName=worldStartedName;
    candidate.sourceDifficulty=worldStartDifficulty;
    candidate.difficulty=worldStartDifficulty;
    candidate.normalReplay=normalReplay;candidate.normalRecord=normalRecord;candidate.originalContinueCount=originalContinueCount;candidate.normalReplayOrigin=normalReplayOrigin;candidate.normalReplayClock=normalReplayClock;
    candidate.scoreRaw=scoreRaw;
    candidate.pointValueRaw=pointValueRaw;
    candidate.motivationRaw=motivationRaw;
    candidate.motivationFlag=motivationFlag;
    candidate.grazeCount=grazeGlobals.grazeCount;
    candidate.powerRaw=grazeGlobals.powerRaw;
    candidate.cumulativeFrozenArea=cumulativeFrozenArea;
    candidate.perfectCharge=playerWindowControl.perfectCharge4b4d70;
    candidate.shotDifficulty=shotDifficulty;
    captureDebugCarry(candidate);
    candidate.gameRandom=world->gameRandomState();
    candidate.displayRandom=worldDisplayRandom;
    if(!validStageCarryValues(candidate,operationError))return 0;
    if(stageCarryNextToken==0)stageCarryNextToken=1;
    candidate.token=stageCarryNextToken++;
    candidate.valid=true;
    stageCarry=candidate;
    return candidate.token;
}
int th128_world_bind_stage_carry_target(unsigned token,const char* name,unsigned nameLength,int difficulty) {
    operationError.clear();
    if(!stageCarry.valid||stageCarry.consumed||token==0||token!=stageCarry.token){
        operationError="stage carry token is unavailable or already consumed";return 0;
    }
    if(stageCarry.gameRandomRestored){
        operationError="stage carry target must be bound before restore_game_random";return 0;
    }
    std::string target;
    if(!nameArgument(name,nameLength,target)||difficulty<0||difficulty>4){
        operationError="stage carry target requires a valid ECL name and difficulty";return 0;
    }
    if(!stageEclMatches(stageCarry.nextId,target)){
        operationError="stage carry target ECL does not match nextId in the original stage table";return 0;
    }
    stageCarry.targetWorldName=std::move(target);
    stageCarry.targetDifficulty=difficulty;
    stageCarry.targetBound=true;
    stageIntro=std::make_unique<th128::stageintro::Host>();
    stageIntroDrawError.clear();
    return 1;
}
int th128_world_restore_stage_carry_random(unsigned token,int nextId,int nextProgress,int difficulty) {
    operationError.clear();
    if(!stageCarry.valid||stageCarry.consumed||token==0||token!=stageCarry.token){
        operationError="stage carry token is unavailable or already consumed";return 0;
    }
    if(stageCarry.gameRandomRestored&&stageCarry.restoredGeneration==worldGeneration){
        operationError="stage carry game random was already restored in this world";return 0;
    }
    if(!stageCarry.targetBound||stageCarry.targetWorldName.empty()){
        operationError="stage carry target ECL binding is missing";return 0;
    }
    if(worldStartedName.empty()||worldStartedName!=stageCarry.targetWorldName
        ||worldStartDifficulty!=stageCarry.targetDifficulty
        ||stageCarry.difficulty!=stageCarry.targetDifficulty){
        operationError="stage carry target does not match the actual started world/difficulty";return 0;
    }
    const int targetId=stageIdForEcl(worldStartedName);
    if(targetId==0||targetId!=stageCarry.nextId
        ||!validateCarryIdentity(stageCarry,targetId,stageCarry.nextProgress,operationError)){
        operationError=operationError.empty()?
            "stage carry target is not a valid receive-carry identity":operationError;
        return 0;
    }
    if(world->frame()!=0||world->spawnCount()!=0||world->faulted()||!world->configurationOpen()){
        operationError="stage carry random restore must precede local enemy passes in a fresh world";return 0;
    }
    if(nextId!=stageCarry.nextId||nextProgress!=stageCarry.nextProgress||difficulty!=stageCarry.targetDifficulty){
        operationError="stage carry token does not match the requested next stage/progress/difficulty";return 0;
    }
    if(!validStageCarryValues(stageCarry,operationError))return 0;
    std::string restoreError;
    if(!world->restoreGameRandomState(stageCarry.gameRandom,restoreError)){
        operationError=restoreError.empty()?"StageWorld game random restore failed":restoreError;return 0;
    }
    worldDisplayRandom=stageCarry.displayRandom;
    stageCarry.preInitGameRandom=stageCarry.gameRandom;
    stageCarry.preInitDisplayRandom=stageCarry.displayRandom;
    stageCarry.preInitBound=true;
    stageCarry.gameRandomRestored=true;
    stageCarry.restoredGeneration=worldGeneration;
    return 1;
}
int th128_world_restore_stage_carry(unsigned token) {
    operationError.clear();
    if(!stageCarry.valid||stageCarry.consumed||token==0||token!=stageCarry.token){
        operationError="stage carry token is unavailable or already consumed";return 0;
    }
    if(!stageCarry.gameRandomRestored||stageCarry.restoredGeneration!=worldGeneration){
        operationError="stage carry game random must be restored in this world before scalar restore";return 0;
    }
    if(stageCarry.scalarRestored&&stageCarry.scalarRestoredGeneration==worldGeneration){
        operationError="stage carry scalars were already restored in this world";return 0;
    }
    if(!stageCarry.targetBound||stageCarry.targetWorldName.empty()
        ||stageCarry.targetWorldName!=worldStartedName
        ||stageCarry.targetDifficulty!=worldStartDifficulty){
        operationError="stage carry target does not match the actual started world/difficulty";return 0;
    }
    const int targetId=stageIdForEcl(worldStartedName);
    if(targetId==0||targetId!=stageCarry.nextId){
        operationError="stage carry target is not the actual next identity";return 0;
    }
    if(world->frame()!=0||world->faulted()||world->configurationOpen()||!messagesConfigured
        ||!perfectHudConfigured||!playerShotGroupsConfigured){
        operationError="stage carry scalar restore requires fresh messages/HUD/groups context";return 0;
    }
    const std::int32_t targetProgress=stageTransitionConfigured
        ?stageTransitionProgress:stageCarry.nextProgress;
    if(stageTransitionConfigured
        &&(stageTransitionCurrentId!=targetId||stageTransitionProgress!=stageCarry.nextProgress)){
        operationError="ordinary stage transition context does not match the carry target";return 0;
    }
    if(!validateCarryIdentity(stageCarry,targetId,targetProgress,operationError))return 0;
    if(!validStageCarryValues(stageCarry,operationError))return 0;
    const auto playerPose=world->playerAimPosition();
    if(!perfectHud->charge(stageCarry.perfectCharge,operationError)){
        world->reportRuntimeFault(operationError);return 0;
    }
    if(!playerShotGroups->updateSelection(stageCarry.cumulativeFrozenArea,stageCarry.shotDifficulty,0,
                                          {playerPose.x,playerPose.y,playerPose.z},operationError)){
        world->reportRuntimeFault(operationError);return 0;
    }
    if(playerWindowControl.perfectCharge4b4d70/10000<stageCarry.perfectCharge/10000
        &&!perfectHud->notice(operationError)){
        world->reportRuntimeFault(operationError);return 0;
    }
    scoreRaw=stageCarry.scoreRaw;
    pointValueRaw=stageCarry.pointValueRaw;
    motivationRaw=stageCarry.motivationRaw;
    motivationFlag=stageCarry.motivationFlag;
    grazeGlobals.grazeCount=stageCarry.grazeCount;
    grazeGlobals.powerRaw=stageCarry.powerRaw;
    cumulativeFrozenArea=stageCarry.cumulativeFrozenArea;
    playerWindowControl.perfectCharge4b4d70=stageCarry.perfectCharge;
    shotDifficulty=stageCarry.shotDifficulty;
    restoreDebugCarry(stageCarry);
    normalReplay=stageCarry.normalReplay;normalRecord=stageCarry.normalRecord;originalContinueCount=stageCarry.originalContinueCount;normalReplayOrigin=stageCarry.normalReplayOrigin;normalReplayClock=stageCarry.normalReplayClock;normalReplayClockPending=false;
    currentStageProgress=targetProgress;
    stageCarry.scalarRestored=true;
    stageCarry.scalarRestoredGeneration=worldGeneration;
    return 1;
}
int th128_world_discard_stage_carry(unsigned token) {
    operationError.clear();
    if(!stageCarry.valid||stageCarry.consumed||token==0||token!=stageCarry.token){
        operationError="stage carry token is unavailable or already consumed";return 0;
    }
    stageCarry.consumed=true;
    return 1;
}
int th128_world_acknowledge_stage_transition(unsigned token) {
    operationError.clear();
    if(!stageCarry.valid||stageCarry.consumed||token==0||token!=stageCarry.token){
        operationError="stage transition carry token is unavailable or already committed";return 0;
    }
    if(!stageCarry.gameRandomRestored||stageCarry.restoredGeneration!=worldGeneration
        ||!stageCarry.scalarRestored||stageCarry.scalarRestoredGeneration!=worldGeneration){
        operationError="stage transition ACK requires this world's completed carry restore";return 0;
    }
    if(!stageCarry.targetBound||stageCarry.targetWorldName.empty()
        ||stageCarry.targetWorldName!=worldStartedName
        ||stageCarry.targetDifficulty!=worldStartDifficulty
        ||stageCarry.difficulty!=stageCarry.targetDifficulty){
        operationError="stage transition ACK target no longer matches the actual world";return 0;
    }
    const int targetId=stageIdForEcl(worldStartedName);
    if(targetId==0||targetId!=stageCarry.nextId){
        operationError="stage transition ACK target is not the actual next identity";return 0;
    }
    const std::int32_t targetProgress=stageTransitionConfigured
        ?stageTransitionProgress:stageCarry.nextProgress;
    if(stageTransitionConfigured
        &&(stageTransitionCurrentId!=targetId||stageTransitionProgress!=stageCarry.nextProgress)){
        operationError="stage transition ACK ordinary context does not match the target";return 0;
    }
    if(!validateCarryIdentity(stageCarry,targetId,targetProgress,operationError))return 0;
    if(stageCarry.sourceTransitionSerial==0||!stageCarry.sourceControlActionRecreate){
        operationError="stage transition ACK has no archived source request";return 0;
    }
    if(world->frame()!=0||world->faulted()){
        operationError="stage transition ACK must precede the first target-world tick";return 0;
    }
    stageCarry.acknowledged=true;
    stageCarry.consumed=true;
    currentStageProgress=targetProgress;
    return 1;
}
int th128_world_acknowledge_music_control(unsigned serial) {
    operationError.clear();
    if(!musicControl.active||serial==0||serial!=musicControl.serial){
        operationError="music control request is unavailable or stale";return 0;
    }
    musicControl.acknowledged=true;
    return 1;
}
const char* th128_world_music_control_state() {
    static std::string result;std::ostringstream text;
    writeMusicControlState(text);result=text.str();return result.c_str();
}
const char* th128_world_route_choice_context() {
    static std::string result;std::ostringstream text;
    writeRouteChoiceContextState(text);result=text.str();return result.c_str();
}
const char* th128_world_flow_capabilities() {
    return "{\"protocol\":1,\"opaqueCarry\":true,\"tokenAck\":true,\"ordinaryTransitions\":true,\"terminalCarry\":true,\"terminalTransitions\":false,\"initialContext\":\"explicit-diagnostic\",\"supportedBulletTypes\":[4,6,8,9,10,13,16,18,20,21,23,24,25,29,30,32],\"perfectFreezeNormalEmptyLasers\":true}";
}
const char* th128_world_message_state(){static std::string result;std::ostringstream text;writeMessageState(text);result=text.str();return result.c_str();}
int th128_world_enable_spells(const void* bullet,unsigned bulletSize,const void* boss,unsigned bossSize,const void* text,unsigned textSize,const void* font,unsigned fontSize,const void* card,unsigned cardSize,const void* player,unsigned playerSize){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||spellsConfigured||!scoreFontLoaded){operationError="spell graphics require a fresh original font/world context";return 0;}
    auto candidate=std::make_unique<th128::spell::Visuals>();
    struct Input{int bank;const void* bytes;unsigned size;};
    for(const auto input:std::array<Input,6>{{{0,bullet,bulletSize},{3,boss,bossSize},{6,text,textSize},{11,font,fontSize},{12,card,cardSize},{7,player,playerSize}}}){
        const unsigned sizeLimit=input.bank==3?16u*1024u*1024u:8u*1024u*1024u;
        const auto context="spell ANM bank="+std::to_string(input.bank)+" bytes="+std::to_string(input.size);
        if(!th128::runtime::inputRange(input.bytes,input.size)){operationError=context+" input span is invalid";return 0;}
        if(input.size>sizeLimit){operationError=context+" exceeds bounded byte limit "+std::to_string(sizeLimit);return 0;}
        if(!candidate->load(input.bank,{static_cast<const std::uint8_t*>(input.bytes),input.size},operationError)){
            operationError=context+": "+(operationError.empty()?"ANM loader rejected without a diagnostic":operationError);return 0;
        }
    }
    if(!world->setSpellHost([](const th128::SpellEvent& event,std::string& error){
        if(event.retireFollowEffect)return spellVisuals->retireFollowEffect(error);
        if(event.starting){
            spellResultFrames=0;if(!spellVisuals->start(event.position,error))return false;
            if(normalRecord&&normalRecord->started()&&!debugRunTainted&&!replayPlayback.active()){
                try{normalRecord->beginSpell(event.state.id,event.title);}
                catch(const std::exception& failure){error=failure.what();return false;}
            }
            return true;
        }
        if(!spellVisuals->finish(error))return false;
        if(normalRecord&&normalRecord->started()&&!debugRunTainted&&!replayPlayback.active()){
            try{normalRecord->endSpell(event.state.id,event.state.flags);}
            catch(const std::exception& failure){error=failure.what();return false;}
        }
        spellResult=event.result;spellResultFrames=120;
        scoreRaw=int(std::min<std::int64_t>(999999999,std::int64_t(scoreRaw)+event.result.rawScore));
        if(event.result.captured)grazeSounds.push_back({48,0});return true;
    },operationError))return 0;
    spellVisuals=std::move(candidate);spellsConfigured=true;return 1;
}
int th128_world_pause_load_manual(const void* bytes,unsigned size){
    operationError.clear();
    if(!originalPauseMenu||world->faulted()||world->frame()!=0||!th128::runtime::inputRange(bytes,size)||size>8u*1024u*1024u){operationError="original manual requires fresh game and help ANM";return 0;}
    try{originalPauseMenu->loadManual({static_cast<const std::uint8_t*>(bytes),size});return 1;}
    catch(const std::exception& failure){operationError=failure.what();return 0;}
}
int th128_world_pause_active(){return originalPauseMenu&&originalPauseMenu->state().active?1:0;}
int th128_world_pause_action(){if(normalReplaySave&&normalReplaySave->finished()&&!normalReplaySave->gameOver())return int(th128::pause::Action::Title);return originalPauseMenu?int(originalPauseMenu->state().action):0;}
int th128_world_pause_open(int gameOver){
    operationError.clear();
    if(!originalPauseMenu||world->faulted()||worldTickExecuting||normalTerminal.requested||th128_world_pause_active()||gameOver<0||gameOver>1){
        operationError="original pause entry has no exclusive active World owner";return 0;
    }
    if(gameOver&&(motivationRaw>=0||playerHit->snapshot().gate.stateF78!=2)){
        operationError="Game Over requires the actual negative-motivation death gate";return 0;
    }
    try{
        originalPauseMenu->open(gameOver!=0,originalContinueCount,normalReplay&&normalReplay->frames()>0&&!debugRunTainted,world->frame(),replayPlayback.active());
        originalMenuInput.reset();
        originalPauseSoundCursor=0;
        for(const auto identifier:originalPauseMenu->sounds())grazeSounds.push_back({identifier,0});
        originalPauseSoundCursor=unsigned(originalPauseMenu->sounds().size());
        playerInput={};
        return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
int th128_world_pause_tick(unsigned holding,unsigned pressed){
    operationError.clear();
    if(!th128_world_pause_active()||worldTickExecuting||soundJournalWindowSerial||!grazeSounds.empty()){
        operationError="pause ANM tick requires active owner and completed real sound boundary";return 0;
    }
    try{
        if(normalTerminal.transition.bound()){
            if(!normalTerminal.transition.tickPaused(worldGeneration,ordinaryGlobalTaskControls(),operationError))return 0;
            projectOrdinaryGlobalFade();
        }
        const auto repeated=originalMenuInput.advance(holding)&0xf0u;
        if(normalReplaySave){
            if(normalReplaySave->finished())return 1;
            originalPauseMenu->replayPresentationTick();
            normalReplaySave->tick(pressed|repeated);
            const auto& sounds=normalReplaySave->sounds();
            while(normalReplaySaveSoundCursor<sounds.size())grazeSounds.push_back({sounds[normalReplaySaveSoundCursor++],0});
            if(normalReplaySave->finished()){
                const bool backToGameOver=normalReplaySave->gameOver();normalReplayBytes.clear();
                if(backToGameOver){normalReplaySave.reset();originalPauseMenu->returnFromReplay();}
            }
            return 1;
        }
        originalPauseMenu->tick({repeated,pressed});
        const auto& sounds=originalPauseMenu->sounds();
        if(originalPauseSoundCursor>sounds.size())throw std::runtime_error("pause sound journal cursor changed");
        while(originalPauseSoundCursor<sounds.size())grazeSounds.push_back({sounds[originalPauseSoundCursor++],0});
        return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
const char* th128_world_pause_state(){
    std::ostringstream output;
    if(!originalPauseMenu){originalPauseStateText="{\"configured\":false,\"active\":false}";return originalPauseStateText.c_str();}
    const auto& menu=originalPauseMenu->state();
    output<<"{\"configured\":true,\"active\":"<<(menu.active?"true":"false")
        <<",\"kind\":"<<int(menu.kind)<<",\"ready\":"<<(menu.ready?"true":"false")
        <<",\"selection\":"<<menu.selection<<",\"rows\":"<<menu.rows
        <<",\"confirm\":"<<(menu.confirm?"true":"false")<<",\"confirmSelection\":"<<menu.confirmSelection
        <<",\"closing\":"<<(menu.closing?"true":"false")<<",\"action\":"<<th128_world_pause_action()
        <<",\"manual\":"<<(menu.manual?"true":"false")<<",\"helpSelection\":"<<menu.helpSelection<<",\"helpPage\":"<<menu.helpPage
        <<",\"ticks\":"<<menu.ticks<<",\"serial\":"<<menu.serial<<",\"continues\":"<<originalContinueCount<<",\"disabled\":[";
    for(unsigned index=0;index<menu.disabled.size();++index){if(index)output<<',';output<<(menu.disabled[index]?"true":"false");}
    output<<"]";
    if(originalPauseMenu->backdropLoaded()){
        const auto& backdrop=originalPauseMenu->backdrop();const auto& capture=backdrop.capture();
        output<<",\"backdrop\":{\"active\":"<<(backdrop.active()?"true":"false")<<",\"epoch\":"<<capture.epoch<<",\"sourceFrame\":"<<capture.sourceFrame<<",\"ticks\":"<<backdrop.ticks()<<",\"closing\":"<<(backdrop.closing()?"true":"false")<<"}";
    }
    output<<",\"replayPhase\":"<<(normalReplaySave?normalReplaySave->phase():0)<<",\"replaySlot\":"<<(normalReplaySave?normalReplaySave->slot():0)<<",\"replaySelected\":"<<(normalReplaySave?normalReplaySave->selected():0)<<",\"replaySerial\":"<<(normalReplaySave?normalReplaySave->serial():0)<<",\"replayName\":[";
    if(normalReplaySave)for(unsigned index=0;index<8;++index){if(index)output<<',';output<<unsigned(normalReplaySave->name()[index]);}
    output<<"],\"replayProfileBound\":"<<((normalReplaySave&&normalReplaySave->profileBound())?"true":"false")<<",\"replayProfileRevision\":"<<(normalReplaySave?normalReplaySave->profileRevision():0)<<",\"replayNameEpoch\":"<<(normalReplaySave?normalReplaySave->nameEpoch():0)<<",\"replayNameContextReady\":"<<((normalReplaySave&&normalReplayNameContext.matches(normalReplaySave->nameEpoch()))?"true":"false")<<"}";originalPauseStateText=output.str();return originalPauseStateText.c_str();
}
int th128_world_pause_commit(int action){
    operationError.clear();
    if(!th128_world_pause_active()||worldTickExecuting||soundJournalWindowSerial||!grazeSounds.empty()){
        operationError="pause action requires active owner and durable sound acknowledgement";return 0;
    }
    if(action!=int(th128::pause::Action::Resume)&&action!=int(th128::pause::Action::Continue)
        &&action!=int(th128::pause::Action::Title)&&action!=int(th128::pause::Action::Restart)){
        operationError="replay action requires its complete durable recorder consumer";return 0;
    }
    try{
        const bool savedExit=action==int(th128::pause::Action::Title)&&normalReplaySave&&normalReplaySave->finished()&&!normalReplaySave->gameOver();
        if(!savedExit&&int(originalPauseMenu->state().action)!=action)throw std::runtime_error("pause action serial/identity mismatch");
        if(action==int(th128::pause::Action::Continue)){
            if(originalPauseMenu->state().kind!=th128::pause::Kind::GameOver||motivationRaw>=0)
                throw std::runtime_error("continue requires actual Game Over resources");
            const auto resources=th128::pause::continueResources({scoreRaw,motivationRaw,motivationFlag,playerWindowControl.perfectCharge4b4d70,originalContinueCount});
            scoreRaw=resources.score;motivationRaw=resources.motivation;motivationFlag=resources.motivationFlag;
            playerWindowControl.perfectCharge4b4d70=resources.perfect;originalContinueCount=resources.continues;
        }
        originalPauseMenu->commit(savedExit?th128::pause::Action::ReplaySave:th128::pause::Action(action));
        if(savedExit){normalReplaySave.reset();normalReplayBytes.clear();}
        playerInput={};return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
int th128_world_records_restore(const void* data,unsigned size){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||worldTickExecuting||normalReplay||normalRecord||debugRunTainted||debugRecordPrefix.worldBound||replayPlayback.active()
        ||(size!=0&&(size!=52232||!th128::runtime::inputRange(data,size)))||worldStartDifficulty<0||worldStartDifficulty>=4){operationError="ordinary profile restore requires a fresh natural session";return 0;}
    const int stage=stageIdForEcl(worldStartedName);if(stage!=1&&stage!=6&&stage!=11){operationError="ordinary records require original route origin";return 0;}
    try{auto next=std::make_shared<th128::record::NativeNormalRecord>(std::span<const std::uint8_t>(static_cast<const std::uint8_t*>(data),size));normalRecord=std::move(next);return 1;}
    catch(const std::exception& failure){operationError=failure.what();return 0;}
}
int th128_world_records_present(double seconds){
    operationError.clear();
    if(!normalRecord||!normalReplay||debugRunTainted||replayPlayback.active()||worldTickExecuting){operationError="ordinary presentation lacks its unassisted run owner";return 0;}
    try{normalRecord->present(seconds,normalReplay->frames(),th128_world_pause_active()!=0);return 1;}catch(const std::exception& failure){operationError=failure.what();return 0;}
}
unsigned th128_world_records_prepare(unsigned timeLow,unsigned timeHigh,double seconds){
    operationError.clear();
    if(!normalRecord||!normalReplay||debugRunTainted||replayPlayback.active()||worldTickExecuting||!normalRecord->started()||normalRecord->prepared()
        ||world->faulted()){operationError="ordinary postrun lacks its genuine native identity";return 0;}
    if(normalTerminal.requested||(normalTerminal.resourcesConfigured&&normalTerminal.begun&&stageControl.pending()==15)){
        if(!normalTerminal.resourcesConfigured||!normalTerminal.begun||!normalTerminal.requested
            ||!normalTerminal.owner||normalTerminal.generation!=worldGeneration||normalTerminal.counter!=380
            ||normalTerminal.runtimeScene!=15||!normalTerminal.messageDone||!normalTerminal.musicSerial
            ||!musicControl.active||!musicControl.acknowledged||musicControl.operation!=5
            ||musicControl.serial!=normalTerminal.musicSerial||stageControl.current()!=7||stageControl.pending()!=15){
            operationError="ordinary terminal record prepare lacks its completed actual pending ending";return 0;
        }
        if(!normalTerminal.owner->prepareStatsForEnding(worldGeneration,unsigned(originalContinueCount),
            normalRecord,normalReplay,stageControl,scoreRaw,(std::uint64_t(timeHigh)<<32)|timeLow,seconds,operationError))return 0;
        return unsigned(normalRecord->pending().size());
    }
    try{normalRecord->prepare(scoreRaw,unsigned(originalContinueCount),(std::uint64_t(timeHigh)<<32)|timeLow,seconds,normalReplay->frames());return unsigned(normalRecord->pending().size());}
    catch(const std::exception& failure){operationError=failure.what();return 0;}
}
const void* th128_world_records_data(){return normalRecord&&normalRecord->prepared()?normalRecord->pending().data():nullptr;}
unsigned th128_world_records_size(){return normalRecord&&normalRecord->prepared()?unsigned(normalRecord->pending().size()):0;}
int th128_world_records_ack(unsigned serial,unsigned bytes,unsigned digest){
    operationError.clear();if(!normalRecord||debugRunTainted){operationError="ordinary durable record owner absent";return 0;}
    try{normalRecord->acknowledge(serial,bytes,digest);return 1;}catch(const std::exception& failure){operationError=failure.what();return 0;}
}
int th128_world_replay_begin(unsigned seed,double seconds){
    operationError.clear();
    if(world->faulted()||world->frame()!=0||!playerFireConfigured||!playerMotion||seed>65536||debugRunTainted||!std::isfinite(seconds)||seconds<0){
        operationError="normal replay requires actual fresh gameplay resources and a finite host clock";return 0;
    }
    const unsigned stage=unsigned(stageIdForEcl(worldStartedName));const auto random=world->gameRandomState();
    const auto group=th128::playershot::selectShotGroup(cumulativeFrozenArea,shotDifficulty);
    if(!group){operationError="normal replay initial shot group is unavailable";return 0;}
    try{
        const bool receivedCarry=normalReplay&&stageCarry.normalReplay==normalReplay&&stageCarry.valid&&stageCarry.acknowledged&&stageCarry.consumed
            &&stageCarry.gameRandomRestored&&stageCarry.restoredGeneration==worldGeneration&&stageCarry.scalarRestored
            &&stageCarry.scalarRestoredGeneration==worldGeneration&&stageCarry.nextId==int(stage)
            &&stageCarry.targetWorldName==worldStartedName&&stageCarry.targetDifficulty==worldStartDifficulty;
        const auto boundSeed=th128::record::replayStageSeed(seed,random.seed,random.calls,receivedCarry);
        if(!normalReplay){normalReplay=std::make_shared<th128::record::NativeRunReplay>();normalReplayOrigin=seconds;normalReplayClock=seconds;}
        if(normalReplay->configuredFor(stage))throw std::runtime_error("normal replay stage already bound");
        normalReplay->begin(stage,unsigned(worldStartDifficulty),boundSeed,*group,unsigned(originalContinueCount),scoreRaw,pointValueRaw,motivationRaw,playerWindowControl.perfectCharge4b4d70,cumulativeFrozenArea,playerRaw.horizontal,playerRaw.vertical);
        if(normalRecord)normalRecord->beginStage(stage,unsigned(worldStartDifficulty),seconds);
        bindOrdinaryTerminalAfterReplayBegin();
        normalReplayClockPending=false;return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
int th128_world_replay_clock(double seconds,float fps){
    operationError.clear();
    if(!normalReplay||world->faulted()||worldTickExecuting||th128_world_pause_active()||!std::isfinite(seconds)||seconds<normalReplayClock||!std::isfinite(fps)||fps<0||normalReplayClockPending){
        operationError="normal replay pre-tick clock is stale or lacks its actual owner";return 0;
    }
    normalReplayClock=seconds;normalReplayFPS=fps;normalReplayClockFrame=world->frame();normalReplayClockPending=true;return 1;
}
int th128_world_replay_previews(const void* bytes,unsigned size){
    operationError.clear();
    if(!th128_world_pause_active()||worldTickExecuting||!th128::runtime::inputRange(bytes,size)){
        operationError="original replay preview requires the active paused owner and actual bytes";return 0;
    }
    try{
        const auto packet=std::span<const std::uint8_t>(static_cast<const std::uint8_t*>(bytes),size);
        if(size==32){
            if(!normalReplaySave||soundJournalWindowSerial||!grazeSounds.empty())
                throw std::runtime_error("last-name profile packet requires the actual replay menu and consumed sound");
            normalReplaySave->bindNameProfile(packet);return 1;
        }
        if(size==36){
            if(!normalReplay||!normalReplaySave||normalReplaySave->phase()!=2||soundJournalWindowSerial||!grazeSounds.empty()
               ||normalReplayNameContext.matches(normalReplaySave->nameEpoch()))
                throw std::runtime_error("original name context requires a fresh name entry and consumed sound boundary");
            const auto timestamp=(std::uint64_t(th128::record::NativeReplaySlotPreview::word(packet,12))<<32)
                                  |th128::record::NativeReplaySlotPreview::word(packet,8);
            const auto actual=normalReplay->snapshot(timestamp,scoreRaw,unsigned(stageIdForEcl(worldStartedName)),
                                                      unsigned(originalContinueCount),normalReplaySave->name());
            normalReplayNameContext=th128::record::OriginalReplayNameContext::decode(packet,normalReplaySave->nameEpoch(),
                                                                                   {actual.data()+36,112});
            return 1;
        }
        if((normalReplaySave&&normalReplaySave->phase()!=1)||(size!=25&&size!=3412))
            throw std::runtime_error("original replay preview observation needs25actual metadata records");
        const auto observed=th128::record::NativeReplaySlotPreview::decode(packet);
        if(normalReplaySave)for(unsigned index=0;index<25;++index)
            if(observed[index].occupied!=normalReplayOccupied[index])throw std::runtime_error("active replay slot occupancy changed outside saved acknowledgement");
        normalReplayPreviewRows=observed;
        for(unsigned index=0;index<25;++index)normalReplayOccupied[index]=observed[index].occupied;
        return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
int th128_world_replay_save_open(){
    operationError.clear();
    if(!normalReplay||!normalReplay->frames()||!th128_world_pause_active()||normalReplaySave||originalContinueCount||debugRunTainted
       ||originalPauseMenu->state().action!=th128::pause::Action::ReplaySave||soundJournalWindowSerial||!grazeSounds.empty()){
        operationError="original replay save requires completed menu choice and actual uncontinued normal recording";return 0;
    }
    for(const auto& row:normalReplayPreviewRows)if(row.occupied&&!row.known){operationError="occupied replay slot has no actual full metadata";return 0;}
    normalReplaySave=std::make_unique<th128::record::NativeReplaySaveMenu>();normalReplaySave->open(originalPauseMenu->state().kind==th128::pause::Kind::GameOver,normalReplayOccupied);
    normalReplayNameContext={};normalReplaySaveSoundCursor=0;return 1;
}
unsigned th128_world_replay_snapshot(unsigned timestampLow,unsigned timestampHigh){
    operationError.clear();
    if(!normalReplay||!normalReplaySave||!normalReplaySave->pending()||soundJournalWindowSerial||!grazeSounds.empty()){operationError="replay snapshot requires confirmed name and consumed real menu sound";return 0;}
    try{normalReplayBytes=normalReplay->snapshot(normalReplayNameContext.matches(normalReplaySave->nameEpoch())?normalReplayNameContext.timestamp():(std::uint64_t(timestampHigh)<<32)|timestampLow,scoreRaw,unsigned(stageIdForEcl(worldStartedName)),unsigned(originalContinueCount),normalReplaySave->name());return unsigned(normalReplayBytes.size());}
    catch(const std::exception& failure){operationError=failure.what();return 0;}
}
const void* th128_world_replay_data(){return normalReplayBytes.empty()?nullptr:normalReplayBytes.data();}
unsigned th128_world_replay_checksum(){
    std::uint32_t value=2166136261u;for(const auto byte:normalReplayBytes){value^=byte;value*=16777619u;}return value;
}
int th128_world_replay_saved(unsigned serial,unsigned slot,unsigned byteCount,unsigned checksum){
    operationError.clear();
    if(!normalReplaySave||normalReplayBytes.empty()||byteCount!=normalReplayBytes.size()||checksum!=th128_world_replay_checksum()){operationError="native replay durable writeback checksum/length differs";return 0;}
    try{normalReplaySave->acceptSaved(serial,slot);normalReplayOccupied[slot]=true;normalReplayPreviewRows[slot]={};normalReplayPreviewRows[slot].occupied=true;normalReplayBytes.clear();return 1;}
    catch(const std::exception& failure){operationError=failure.what();return 0;}
}
int th128_world_replay_playback_restore(const std::uint8_t* data,unsigned length,int rank){
    operationError.clear();
    if(world->faulted()||worldTickExecuting||world->frame()!=0||!playerMotion||!playerFireConfigured||normalReplay
        ||debugRunTainted||rank!=worldStartDifficulty||length!=144||!th128::runtime::inputRange(data,length)){
        operationError="recorded stage restoration requires fresh replay-only ownership";return 0;
    }
    const auto bytes=std::span<const std::uint8_t>(data,length);
    const auto word=[&](unsigned offset){return th128::record::NativeReplaySlotPreview::word(bytes,offset);};
    const auto half=[&](unsigned offset){return unsigned(bytes[offset])|(unsigned(bytes[offset+1])<<8);};
    const int score=std::bit_cast<int>(word(12)),point=std::bit_cast<int>(word(20)),motivation=std::bit_cast<int>(word(128)),perfect=std::bit_cast<int>(word(132));
    const float area=std::bit_cast<float>(word(136));
    const auto group=th128::playershot::selectShotGroup(area,rank);
    const auto random=world->gameRandomState();
    if(half(0)!=unsigned(stageIdForEcl(worldStartedName))||half(2)!=random.seed||!group||half(16)!=*group
        ||score<0||score>999999999||point<0||point>1000000000||motivation<0||motivation>100000||perfect<0||perfect>30000
        ||word(32)>9||!std::isfinite(area)||area<0||area>1048576||cumulativeFrozenArea!=area
        ||std::bit_cast<int>(word(24))!=playerRaw.horizontal||std::bit_cast<int>(word(28))!=playerRaw.vertical){
        operationError="recorded stage header resources or original initialized owners differ";return 0;
    }
    if(!perfectHud->charge(perfect,operationError))return 0;
    scoreRaw=score;pointValueRaw=point;motivationRaw=motivation;motivationFlag=0;
    playerWindowControl.perfectCharge4b4d70=perfect;originalContinueCount=int(word(32));return 1;
}

int th128_world_replay_playback_load(const void* data,unsigned length,int rank){
    operationError.clear();
    if(world->faulted()||worldTickExecuting||world->frame()!=0||!playerConfigured||!playerMotion||!playerFireConfigured
        ||normalReplay||debugRunTainted||rank!=worldStartDifficulty||!th128::runtime::inputRange(data,length)){
        operationError="replay playback requires actual fresh non-recording World with matching rank";return 0;
    }
    const auto group=th128::playershot::selectShotGroup(cumulativeFrozenArea,shotDifficulty);
    if(!group){operationError="replay playback original shot group unavailable";return 0;}
    const auto random=world->gameRandomState();
    try{
        const th128::record::ReplayPlaybackInitial actual{
            unsigned(stageIdForEcl(worldStartedName)),unsigned(worldStartDifficulty),random.seed,*group,
            unsigned(originalContinueCount),worldGeneration,unsigned(world->frame()),scoreRaw,pointValueRaw,
            playerRaw.horizontal,playerRaw.vertical,motivationRaw,playerWindowControl.perfectCharge4b4d70,cumulativeFrozenArea};
        replayPlayback.bind({static_cast<const std::uint8_t*>(data),length},actual);return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
const char* th128_world_replay_playback_state(){
    auto auxiliary=messagesConfigured?messages->recordAuxiliaryLive():std::optional<bool>{false};
    if(!auxiliary&&routeChoiceConfigured){
        const auto& choice=routeChoice->status();
        if(choice.phase!=th128::routechoice::Phase::Failed)auxiliary=choice.handle!=0;
    }
    std::ostringstream output;
    output<<"{\"active\":"<<(replayPlayback.active()?"true":"false")<<",\"stage\":"<<replayPlayback.stage()
        <<",\"rank\":"<<replayPlayback.rank()<<",\"generation\":"<<replayPlayback.generation()
        <<",\"consumed\":"<<replayPlayback.consumed()<<",\"frames\":"<<replayPlayback.frames()
        <<",\"complete\":"<<(replayPlayback.active()&&replayPlayback.consumed()==replayPlayback.frames()?"true":"false")
        <<",\"auxiliaryKnown\":"<<(auxiliary.has_value()?"true":"false")<<",\"auxiliaryLive\":";
    if(auxiliary)output<<(*auxiliary?"true":"false");else output<<"null";
    output<<"}";replayPlaybackStateText=output.str();return replayPlaybackStateText.c_str();
}
int th128_world_replay_playback_cancel(){
    operationError.clear();
    if(worldTickExecuting){operationError="cannot release replay input during actual World tick";return 0;}
    replayPlayback.cancel();return 1;
}

int th128_world_tick(){
    operationError.clear();
    if(normalTerminal.requested){operationError="pending ordinary ending requires audio-first controller consumption before another World update";return 0;}
    if(world->faulted())return 0;
    if(th128_world_pause_active()){operationError="actual World tick is suspended by original pause/GameOver owner";return 0;}
    if(playerDeathConfigured&&playerHit->snapshot().gate.stateF78==2&&playerHit->snapshot().stateTimer.current>=30&&motivationRaw<0&&!debugInfiniteRespawnRun){
        normalReplayClockPending=false;return th128_world_pause_open(1)?2:0;
    }
    if(worldTickExecuting){operationError="world tick is not reentrant";return 0;}
    if(debugRecordDriver.bound&&(debugRecordDriver.phase!=1
        ||debugRecordDriver.generation!=worldGeneration||debugRecordDriver.epoch!=debugRecordPrefix.epoch)){
        operationError="actual record driver pre-update boundary must precede World tick";return 0;
    }
    if(replayPlayback.active()){
        auto auxiliary=messagesConfigured?messages->recordAuxiliaryLive():std::optional<bool>{false};
        if(!auxiliary&&routeChoiceConfigured){
            const auto& choice=routeChoice->status();
            if(choice.phase!=th128::routechoice::Phase::Failed)auxiliary=choice.handle!=0;
        }
        try{
            std::array<std::uint16_t,3> packet{};
            if(replayPlayback.prepare(worldGeneration,auxiliary.has_value(),auxiliary.value_or(false),packet))
                playerInput={packet[0],packet[1],packet[2]};
        }catch(const std::exception& failure){operationError=failure.what();return 0;}
    }
    const auto normalReplayInput=std::array<std::uint16_t,3>{playerInput.holding,playerInput.pressed,playerInput.released};
    bool normalReplayAuxiliary=false;
    if(normalReplay){
        if(!normalReplayClockPending||normalReplayClockFrame!=world->frame()){operationError="actual normal replay pre-tick clock missing";return 0;}
        auto auxiliary=messagesConfigured?messages->recordAuxiliaryLive():std::optional<bool>{false};
        if(!auxiliary&&routeChoiceConfigured){
            const auto& actual=routeChoice->status();
            if(actual.phase!=th128::routechoice::Phase::Failed)auxiliary=actual.handle!=0;
        }
        if(!auxiliary){operationError="normal replay MSG auxiliary ownership unknown";return 0;}
        normalReplayAuxiliary=*auxiliary;
    }
    struct TickGuard {bool& flag;explicit TickGuard(bool& value):flag(value){flag=true;}~TickGuard(){flag=false;}} tickGuard(worldTickExecuting);
    if(normalTerminal.transition.bound()){
        if(!normalTerminal.transition.tickBattle(worldGeneration,world->frame()+1,ordinaryGlobalTaskControls(),operationError)){world->reportRuntimeFault(operationError);return 0;}
        projectOrdinaryGlobalFade();
    }
    if(debugRecordPrefix.worldBound){
        if(debugRecordPrefix.failed||!debugRecordPrefix.sessionPresent||!playerConfigured||!debugFreshReplay
            ||debugFreshReplayGeneration!=worldGeneration||debugFreshReplayEpoch!=debugRecordPrefix.epoch
            ||debugRecordPrefix.sessionGeneration!=worldGeneration
            ||(debugRecordPrefix.sessionFlags60&0x7cu)
            ||playerHit->snapshot().soundControl.flags60!=debugRecordPrefix.sessionFlags60){
            debugRecordPrefix.failed=true;
            world->reportRuntimeFault("native record session active callback lacks its actual ready control owner");return 0;
        }
        const auto messageAuxiliary=messagesConfigured?messages->recordAuxiliaryLive():std::nullopt;
        if(!messageAuxiliary){
            debugRecordPrefix.failed=true;
            world->reportRuntimeFault("native replay input requires parsed MSG6c absence or the actual route-choice handle");return 0;
        }
        const bool stageClearHeld=th128::record::NativeRecordSessionPolicy::stageClearHeld(
            stageManager.flags5a04,stageManager.field5a0c);
        if(debugRecordDriver.bound&&!stageClearHeld&&!debugRecordDriver.gameplayAnchorKnown
            &&debugRecordPrefix.sessionField74==0){
            debugRecordDriver.gameplayAnchorSeconds=debugRecordDriver.beginSeconds;
            debugRecordDriver.gameplayAnchorKnown=true;
        }
        th128::record::NativeRecordAccounting::recordActiveFrame(debugRecordPrefix.profile,actualRecordRoute(),
            debugFreshReplay->mode(),debugRecordPrefix.sessionFlags60,stageClearHeld);
        if(!stageClearHeld)++debugRecordPrefix.activeCallbacks;
        try{
            if(!debugFreshReplay->active()){
                if(world->frame()!=0)throw std::runtime_error("native replay stage activation missed its first session callback");
                debugFreshReplay->activate(playerRaw.horizontal,playerRaw.vertical,playerFocused);
            }
            const th128::record::NativeReplayInputContext input{debugRecordPrefix.sessionPresent,
                debugTerminal.configured,*messageAuxiliary,debugRecordPrefix.timing.state().fps,
                {playerInput.holding,playerInput.pressed,playerInput.released}};
            if(!debugFreshReplay->record(input))throw std::runtime_error("native replay initial input callback did not record");
            if(debugExtraReplay&&(!debugExtraReplay->append(input.input,input.messageAuxiliaryLive,input.fps,
                debugRecordDriver.beginSeconds-debugRecordPrefix.clockFirstSeconds)||debugExtraReplay->frames()!=unsigned(debugFreshReplay->input().frame())))
                throw std::runtime_error("EX full native history lost its actual fresh input frame");
        }catch(const std::exception& failure){
            debugRecordPrefix.failed=true;world->reportRuntimeFault(failure.what());return 0;
        }
    }
    playerFocused=false;
    bool perfectActivatedThisTick=false;
    if(perfectConfigured&&!syncPerfectCharge(operationError)){
        world->reportRuntimeFault(operationError);return 0;
    }
    if(playerConfigured){
        if(playerHit->snapshot().gate.stateF78==4){
            const auto snapshot=playerHit->snapshot();
            if(playerDeathConfigured&&snapshot.stateTimer.current>=8){
                const auto liveSpell=world->phaseSpellContext();
                th128::player::DeathContext context{{motivationRaw,motivationFlag},playerWindowControl.perfectCharge4b4d70,
                    {deathAwardSelector,0},{liveSpell?liveSpell->flags:snapshot.tail.flags7c,liveSpell?liveSpell->age:snapshot.tail.counter28,liveSpell?std::bit_cast<std::int32_t>(liveSpell->value80):snapshot.tail.value80,itemScalar},
                    {snapshot.tailControl.present,snapshot.tailControl.mode3c},{}};
                deathOperation=std::make_unique<th128::player::DeathOperation>(context,4,snapshot.stateTimer,snapshot.protection);
                const bool completed=deathOperation->run([](const th128::player::death::CallRequest& call,std::string& error){
                    const auto& progress=deathOperation->progress();
                    motivationRaw=progress.context.motivation.value4b4d64;motivationFlag=progress.context.motivation.flag4b4d68;
                    playerWindowControl.perfectCharge4b4d70=progress.context.perfectCharge;
                    if(progress.powerRaw)grazeGlobals.powerRaw=*progress.powerRaw;
                    if(call.kind==th128::player::death::CallKind::PlayerAnimation)
                        return playerHit->commitDeathState(progress.state,progress.stateTimer,progress.protection,error);
                    if(perfectHudConfigured&&call.kind==th128::player::death::CallKind::ChargeNotify424b80)return perfectHud->notice(error);
                    if(perfectHudConfigured&&call.kind==th128::player::death::CallKind::ChargeUi425820)return perfectHud->charge(progress.context.perfectCharge,error);
                    if(call.kind==th128::player::death::CallKind::Sound){
                        if(grazeSounds.size()>=65536){error="death sound journal budget";return false;}
                        grazeSounds.push_back({int(call.operands[0]),0});return true;
                    }
                    error="unimplemented required player death control/UI continuation";return false;
                },operationError);
                const auto& progress=deathOperation->progress();motivationRaw=progress.context.motivation.value4b4d64;
                motivationFlag=progress.context.motivation.flag4b4d68;playerWindowControl.perfectCharge4b4d70=progress.context.perfectCharge;
                if(progress.powerRaw)grazeGlobals.powerRaw=*progress.powerRaw;
                if(!completed){world->reportRuntimeFault(operationError);return 0;}
                if(liveSpell&&!world->applySpellTail(progress.context.tail.flags7c,std::uint32_t(progress.context.tail.value80),operationError)){world->reportRuntimeFault(operationError);return 0;}
                itemScalar=progress.context.tail.scalar4b4d30;++deathsCommitted;
                if(!playerHit->advancePostDeathUnit(operationError)){world->reportRuntimeFault(operationError);return 0;}
            }else if(!playerHit->advanceWindowUnit(playerWindowControl,operationError)){world->reportRuntimeFault(operationError);return 0;}
        }else if(playerDeathConfigured&&playerHit->snapshot().gate.stateF78==2){
            if(playerHit->snapshot().stateTimer.current>=30){
                if(!playerRespawnConfigured){world->reportRuntimeFault("unimplemented respawn damage field at 0x43df90: explicit respawn context is missing");return 0;}
                const auto old=world->playerAimPosition();
                if(debugInfiniteRespawnRun&&motivationRaw<0){
                    debugLastRefillBefore=motivationRaw;
                    motivationRaw=10000;
                    debugLastRefillAfter=motivationRaw;
                    ++debugReviveRefills;debugRunTainted=true;
                }
                const auto respawn=playerHit->beginRespawn({old.x,old.y,old.z},motivationRaw,respawnControl.mode04,
                    [old](const th128::player::death::CallRequest& call,std::string& error){
                        if(call.kind==th128::player::death::CallKind::RespawnField43df90)
                            return bool(damageFields->spawn({old.x,old.y,old.z},32,16,30,150,0,error));
                        if(call.kind==th128::player::death::CallKind::PostRespawn427190){
                            const auto control=th128::player::respawn::postFieldControl(respawnControl);
                            if(!control){error="invalid post-respawn control state";return false;}
                            respawnControl=control->next;
                            if(control->requiredSlot){error="unimplemented live post-respawn HUD callback at 0x41fd81/0x41fdc9/0x41fe02";return false;}
                            return true;
                        }
                        error="unimplemented respawn call continuation";return false;
                    },operationError);
                if(!respawn){world->reportRuntimeFault(operationError);return 0;}
                respawnSavedPosition=respawn->savedPosition;playerRaw={respawn->rawHorizontal,respawn->rawVertical};
                if(!world->setPlayerAimPosition({respawn->position.horizontal,respawn->position.vertical,respawn->position.depth},operationError)
                    ||!playerHit->stepProtection(1,operationError)||!playerHit->advanceAnimationUnit(operationError)
                    ||!playerHit->advanceEntryTimersUnit(operationError)){world->reportRuntimeFault(operationError);return 0;}
                ++respawnsCommitted;
            }else if(!playerHit->advancePostDeathUnit(operationError)){world->reportRuntimeFault(operationError);return 0;}
        }else if(playerRespawnConfigured&&playerHit->snapshot().gate.stateF78==0){
            const auto entry=playerHit->entryUnit([](int elapsed,float radius,std::string& error){
                if(elapsed<30){
                    if(!std::isfinite(radius)||radius<0){error="invalid entry laser cancellation radius";return false;}
                    const auto liveLasers=world->laserObjectCount();
                    if(!liveLasers){error="entry laser cancellation manager count is unknown";return false;}
                    if(*liveLasers!=0){
                        const th128::bullet::Position center{respawnSavedPosition.horizontal,respawnSavedPosition.vertical,respawnSavedPosition.depth};
                        if(!laserLines->cancelEntryRadius(center,radius,true,error))return false;
                        if(!laserLines->cancelEntryRadius(center,float(double(radius)*.25),false,error))return false;
                    }
                    emptyLaserCancelCenter={respawnSavedPosition.horizontal,respawnSavedPosition.vertical,respawnSavedPosition.depth};
                    return true;
                }
                return world->cancelAllBullets([](th128::bullet::Birth& bullet,std::string& reason){
                    if(bullet.type==30){
                        const auto owned=bulletAnimations.find(bullet.id);
                        const auto profile=bulletProfiles.find(30);
                        if(owned==bulletAnimations.end()||profile==bulletProfiles.end()
                            ||profile->second.script!=285||profile->second.secondaryScript!=286
                            ||profile->second.tail.hitboxSize!=4.f||profile->second.tail.auxiliarySize!=22.f
                            ||profile->second.tail.freezePolicy!=1||!profile->second.identitySpriteRemap()
                            ||profile->second.cancellationScript(0)!=381||profile->second.cancellationScript(6)!=381
                            ||bullet.script!=285||bullet.cancellationScript!=381){
                            reason="type30 respawn cancellation requires its owned profile/pair/cache";return false;
                        }
                        if(!owned->second.hasSecondary||owned->second.primary.state().script!=285
                            ||owned->second.secondary.state().script!=286){
                            reason="type30 respawn cancellation requires the owned 285/286 visual pair";return false;
                        }
                        const auto effect=cancellationEffects->spawn(bullet.cancellationScript,320+bullet.state.position.x,
                            16+bullet.state.position.y,bullet.state.position.z,23,reason);
                        if(!effect){reason="type30 respawn cancellation effect: "+reason;return false;}
                        if(!cancellationEffects->initializeNativeDrift(*effect,{bullet.state.velocity.x,
                            bullet.state.velocity.y,bullet.state.velocity.z},1,{0,0,0},reason)){
                            cancellationEffects->remove(*effect);return false;
                        }
                        if(!bulletVisuals->interrupt(owned->second,1,reason)){cancellationEffects->remove(*effect);return false;}
                        owned->second.secondary.interrupt(1);
                        return true;
                    }
                    if(bullet.type==24){
                        const auto owned=bulletAnimations.find(bullet.id);
                        const auto profile=bulletProfiles.find(24);
                        if(owned==bulletAnimations.end()||profile==bulletProfiles.end()
                            ||profile->second.script!=283||profile->second.secondaryScript!=284
                            ||profile->second.tail.hitboxSize!=6.f||profile->second.tail.auxiliarySize!=22.f
                            ||profile->second.tail.freezePolicy!=1||!profile->second.identitySpriteRemap()
                            ||profile->second.cancellationScript(0)!=375
                            ||bullet.script!=283||bullet.cancellationScript!=375){
                            reason="respawn type24 cancellation requires its owned profile/pair/cache";return false;
                        }
                        if(!owned->second.hasSecondary||owned->second.primary.state().script!=283
                            ||owned->second.secondary.state().script!=284){
                            reason="respawn type24 cancellation requires the owned 283/284 visual pair";return false;
                        }
                        if(!bulletVisuals->animationState(owned->second,false)||!bulletVisuals->animationState(owned->second,true)){
                            reason="respawn type24 cancellation requires current visual ownership";return false;
                        }
                        const auto effect=cancellationEffects->spawn(bullet.cancellationScript,
                            320+bullet.state.position.x,16+bullet.state.position.y,bullet.state.position.z,23,reason);
                        if(!effect){reason="respawn type24 cancellation effect: "+reason;return false;}
                        if(!cancellationEffects->initializeNativeDrift(*effect,{bullet.state.velocity.x,
                            bullet.state.velocity.y,bullet.state.velocity.z},1,{0,0,0},reason)){
                            cancellationEffects->remove(*effect);return false;
                        }
                        if(!bulletVisuals->interrupt(owned->second,1,reason)){cancellationEffects->remove(*effect);return false;}
                        owned->second.secondary.interrupt(1);
                        return true;
                    }
                    const auto found=bulletAnimations.find(bullet.id);
                    if(found==bulletAnimations.end()){reason="respawn bullet ANM lookup failed";return false;}
                    if(found->second.hasSecondary&&bullet.type!=25){reason="unimplemented secondary bullet cancellation";return false;}
                    if(!bulletVisuals->interrupt(found->second,1,reason))return false;
                    if(bullet.cancellationScript>=0){
                        const auto effect=cancellationEffects->spawn(bullet.cancellationScript,320+bullet.state.position.x,16+bullet.state.position.y,bullet.state.position.z,23,reason);
                        if(!effect){reason="respawn cancellation script="+std::to_string(bullet.cancellationScript)+" type="+std::to_string(bullet.type)
                            +" color="+std::to_string(bullet.color)+": "+reason;return false;}
                        if(!cancellationEffects->initializeNativeDrift(*effect,{bullet.state.velocity.x,bullet.state.velocity.y,bullet.state.velocity.z},1,{0,0,0},reason))return false;
                    }
                    return true;
                },error);
            },operationError);
            if(!entry){world->reportRuntimeFault(operationError);return 0;}
            playerRaw=*entry;
            const auto pixels=th128::player::toPixels(playerRaw);
            if(!world->setPlayerAimPosition({pixels.horizontal,pixels.vertical,respawnSavedPosition.depth},operationError)
                ||!playerHit->stepProtection(1,operationError)){world->reportRuntimeFault(operationError);return 0;}
            if(playerHit->snapshot().gate.stateF78==1&&playerMotion){
                const auto movement=playerHit->moveUnit(playerRaw,playerInput,{rapidShotConfigured,rapidShotConfigured,playerHit->focusCounter(),playerFireConfigured?playerHit->snapshot().heldTimer.current:0},operationError);
                if(!movement){world->reportRuntimeFault(operationError);return 0;}
                playerFocused=movement->focused;playerRaw=movement->position;const auto moved=th128::player::toPixels(playerRaw);
                if(!world->setPlayerAimPosition({moved.horizontal,moved.vertical,respawnSavedPosition.depth},operationError)){world->reportRuntimeFault(operationError);return 0;}
            }else if(!playerHit->advanceAnimationUnit(operationError)){world->reportRuntimeFault(operationError);return 0;}
            if(playerHit->snapshot().gate.stateF78==0&&!playerHit->advanceEntryTimersUnit(operationError)){world->reportRuntimeFault(operationError);return 0;}
        } else {
            if(!playerHit->stepProtection(1,operationError)){world->reportRuntimeFault(operationError);return 0;}
            if(playerMotion){
                const auto movement=playerHit->moveUnit(playerRaw,playerInput,
                    {rapidShotConfigured,rapidShotConfigured,playerHit->focusCounter(),playerFireConfigured?playerHit->snapshot().heldTimer.current:0},operationError);
                if(!movement){world->reportRuntimeFault(operationError);return 0;}
                playerFocused=movement->focused;playerRaw=movement->position;
                const auto pixel=th128::player::toPixels(playerRaw);
                if(!world->setPlayerAimPosition({pixel.horizontal,pixel.vertical,0},operationError)){world->reportRuntimeFault(operationError);return 0;}
            } else if(!playerHit->advanceAnimationUnit(operationError)){world->reportRuntimeFault(operationError);return 0;}
        }
    }
    if(focusIndicatorConfigured){
        const auto position=world->playerAimPosition();
        if(!focusIndicator->update(playerHit->snapshot().gate.stateF78==1&&playerFocused,position.x,position.y,position.z,operationError)){
            world->reportRuntimeFault(operationError);return 0;
        }
    }
    if(perfectConfigured){
        auto perfectStatus=perfectHost->status();
        if(perfectStatus.failed){world->reportRuntimeFault("perfect freeze host failed earlier");return 0;}
        const auto makePerfectContext=[&](th128::perfect::LaserContext lasers,int laserCount){
            th128::perfect::Context context;
            context.perfectCharge=playerWindowControl.perfectCharge4b4d70;
            const auto position=world->playerAimPosition();
            context.x=position.x;context.y=position.y;context.radius=playerProfileRadius;
            context.keys=playerInput.holding|playerInput.pressed;
            context.xPressed=(playerInput.pressed&th128::perfect::XKeyBit)!=0;
            context.systemPresent=playerWindowControl.present;
            context.systemMode3c=playerWindowControl.mode3c;
            context.busy=iceHost?int(iceHost->statistics().busy):0;
            context.playerState=playerHit->snapshot().gate.stateF78;
            context.mode=th128::perfect::Mode::Normal;
            context.lasers=lasers;context.laserCount=laserCount;
            return context;
        };
        if(!perfectStatus.active&&!(playerInput.holding&th128::perfect::XKeyBit)){
            std::string ignored;
            const auto liveLasers=world->laserObjectCount();
            if(liveLasers&&*liveLasers==0)perfectHost->activate(makePerfectContext(th128::perfect::LaserContext::Empty,0),ignored);
            else perfectHost->reject("Perfect requires the actual laser owner; populated/unknown is not Empty",ignored);
        }
        if(!perfectStatus.active&&(playerInput.pressed&th128::perfect::XKeyBit)){
            std::string denied;
            const auto laserCount=world->laserObjectCount();
            if(!laserCount){
                perfectHost->reject("laser object count is unknown",denied);
            }else if(*laserCount!=0){
                perfectHost->reject("populated lasers are outside the supported Perfect domain",denied);
            }else if(iceHost->statistics().busy!=0||iceHost->statistics().objects!=0){
                perfectHost->reject("ordinary ice batch or live Perfect objects are nonempty",denied);
            }else{
                if(perfectHost->activate(makePerfectContext(th128::perfect::LaserContext::Empty,0),denied)){
                    const auto activated=perfectHost->status();
                    playerWindowControl.perfectCharge4b4d70=activated.charge;
                    playerWindowControl.mode3c=1;perfectModeOwned=true;
                    if(!perfectHud->charge(activated.charge,operationError)){
                        world->reportRuntimeFault(operationError);return 0;
                    }
                    perfectActivatedThisTick=true;
                }else if(perfectHost->status().failed){
                    world->reportRuntimeFault(denied);return 0;
                }
            }
        }
    }
    if(playerShotsConfigured){
        if(shotHitsConfigured&&!damageFields->tickUnit(operationError)){world->reportRuntimeFault(operationError);return 0;}
        if(playerHit->snapshot().gate.stateF78==1&&!playerHit->advanceNormalTimersUnit(operationError)){
            world->reportRuntimeFault(operationError);return 0;
        }
        if(playerFireConfigured){
            if(perfectBatchOwned&&iceHost->statistics().objects==0){
                perfectBatchOwned=false;perfectZDenied.clear();
            }
            if(messagesConfigured&&messages->state().active){
                if(!playerFire->suspend([](int operation,int identifier,std::string& error){
                    if(grazeSounds.size()>=65536){error="dialogue fire sound journal budget";return false;}
                    grazeSounds.push_back({identifier,world->playerAimPosition().x,operation});return true;
                },operationError)||!playerHit->setHeldTimer(playerFire->state().held,operationError)){world->reportRuntimeFault(operationError);return 0;}
            }else{
            if(!playerFire->heldTimer(playerHit->snapshot().heldTimer,operationError)
                ||!playerFire->tick(playerHit->snapshot().gate.stateF78,playerInput,
                    iceHost->statistics().busy,
                    playerWindowControl.mode3c,
                    grazeGlobals.powerRaw,[](int count,std::string& error){const auto position=world->playerAimPosition();
                        return playerShots->emitCycle(cumulativeFrozenArea,shotDifficulty,count,{position.x,position.y,.1f},error);},
                    [](int operation,int identifier,std::string& error){if(grazeSounds.size()>=65536){error="fire sound journal budget";return false;}
                        grazeSounds.push_back({identifier,world->playerAimPosition().x,operation});return true;},
                    [](int power,std::string& error){const auto position=world->playerAimPosition();return iceHost->activate(power,{position.x,position.y,position.z},error);},
                    operationError,
                    [](int,std::string& reason){
                        if(perfectConfigured&&perfectBatchOwned&&iceHost->statistics().objects>0){
                            perfectZDenied="ordinary Z freeze during a live Perfect ice batch is unsupported";
                            reason.clear();
                            return th128::player::fire::FreezePreflightDecision::Deny;
                        }
                        return th128::player::fire::FreezePreflightDecision::Allow;
                    })
                ||!playerHit->setHeldTimer(playerFire->state().held,operationError)){world->reportRuntimeFault(operationError);return 0;}
            }
            rapidShotCount=playerFire->state().shot;
            const auto chargePosition=world->playerAimPosition();
            if(!iceHost->charge(playerHit->snapshot().heldTimer.current,{chargePosition.x,chargePosition.y,chargePosition.z},operationError)){
                world->reportRuntimeFault(operationError);return 0;
            }
        }else if(rapidShotConfigured&&playerHit->snapshot().gate.stateF78==1&&!(messagesConfigured&&messages->state().active)){
            if(rapidShotCount<0&&th128::player::startsShotCycle(playerInput,rapidShotCount))rapidShotCount=0;
            if(rapidShotCount>=0){
                const auto position=world->playerAimPosition();
                if(!playerShots->emitCycle(cumulativeFrozenArea,shotDifficulty,rapidShotCount,{position.x,position.y,.1f},operationError)){
                    world->reportRuntimeFault(operationError);return 0;
                }
                const auto cycle=th128::player::advanceShotCycle(playerInput,rapidShotCount);
                if(!cycle){world->reportRuntimeFault("unsupported rapid-shot cycle");return 0;}
                rapidShotCount=cycle->current;
            }
            const auto old=grazeGlobals.powerRaw;grazeGlobals.powerRaw=th128::freeze::addPowerRaw(old,5);
            for(const auto [threshold,identifier]:{std::pair{10000,74},std::pair{3000,73}})
                if(old<threshold&&grazeGlobals.powerRaw>=threshold){
                    if(grazeSounds.size()>=65536){world->reportRuntimeFault("rapid-shot sound journal budget");return 0;}
                    grazeSounds.push_back({identifier,0});
                }
        }
        if(!playerShots->tickUnit(operationError)){world->reportRuntimeFault(operationError);return 0;}
        if(playerShotGroupsConfigured){
            const auto position=world->playerAimPosition();
            if(!playerShotGroups->tick({position.x,position.y,position.z},operationError)){
                world->reportRuntimeFault(operationError);return 0;
            }
        }
    }
    if(normalTerminal.begun&&!tickOrdinaryTerminal(operationError)){world->reportRuntimeFault(operationError);return 0;}
    if(debugTerminal.begun){
        if(debugTerminal.generation!=worldGeneration){world->reportRuntimeFault("debug terminal generation changed");return 0;}
        if(!debugTerminalSettlement->tick(operationError)){world->reportRuntimeFault(operationError);return 0;}
        stageManager.field5a08=stageManager.field5a0c;
        ++stageManager.field5a0c;stageManager.field5a10=float(stageManager.field5a0c);
        ++debugTerminal.counter;
        if(debugTerminal.counter==180)debugTerminal.fadeElapsed=0;
        if(debugTerminal.counter>180&&debugTerminal.fadeElapsed>=0){
            debugTerminal.fadeAlpha=debugTerminal.fadeElapsed>=200?255:int(double(debugTerminal.fadeElapsed)*255./200.);
            ++debugTerminal.fadeElapsed;
        }
        if(debugTerminal.counter>=380&&!debugTerminal.requested){
            if(!debugTerminal.messageDone||!musicControl.active||!musicControl.acknowledged){
                world->reportRuntimeFault("debug ending request requires actual MSG completion and owned music fade");return 0;
            }
            if(stageControl.gateSet()){world->reportRuntimeFault("debug ending scene transition gate remains set");return 0;}
            const auto runtimeScene=debugTerminal.stage==16?16:15;
            if(debugRecordDriver.bound){
                const auto requested=stageControl.request(runtimeScene);
                if(requested.status!=th128::stage::RequestStatus::Written||requested.pending!=runtimeScene){
                    world->reportRuntimeFault("record terminal manager request was not accepted by the actual controller");return 0;
                }
            }
            debugTerminal.runtimeScene=runtimeScene;debugTerminal.requested=true;
        }
    }
    if(stageTransitionConfigured&&stageTransition->state().begun&&!stageTransition->tick(operationError)){
        stageTransitionError=operationError;world->reportRuntimeFault(operationError);return 0;
    }
    if(routeChoiceConfigured&&routeChoiceStarted){
        if(!routeChoice->tick({playerInput.pressed},operationError)){
            routeChoiceError=operationError;world->reportRuntimeFault(operationError);return 0;
        }
        if(!consumeRouteChoiceSounds(operationError)){
            routeChoiceError=operationError;world->reportRuntimeFault(operationError);return 0;
        }
    }
    if(messagesConfigured&&!messages->tick({playerInput.holding,playerInput.pressed,playerInput.released},operationError)){world->reportRuntimeFault(operationError);return 0;}
    playerInput.pressed=0;playerInput.released=0;
    if(perfectConfigured&&!perfectActivatedThisTick&&!perfectHost->tick(false,operationError)){world->reportRuntimeFault(operationError);return 0;}
    if(perfectModeOwned&&perfectHost->status().phase>=180){
        playerWindowControl.mode3c=0;perfectModeOwned=false;
    }
    if(playerFireConfigured&&!iceHost->tick(world->bullets().snapshots(),operationError)){world->reportRuntimeFault(operationError);return 0;}
    if(playerFireConfigured&&!freezeRewardPanel.tick(world->playerAimPosition().y)){
        world->reportRuntimeFault("original freeze reward panel tick rejected");return 0;
    }
    const bool success=world->tick();
    if(success&&perfectHudConfigured){
        if(!world->prepareBossHudPass(operationError)){world->reportRuntimeFault(operationError);return 0;}
        th128::hud::BossInput input;const auto countdown=world->phaseCountdown();
        input.countdown={std::bit_cast<std::int32_t>(countdown[0]),std::bit_cast<std::int32_t>(countdown[1])};
        input.message=messagesConfigured&&messages->state().active;input.hidden=world->bossSlots().flags&1u;
        const auto position=world->playerAimPosition();input.playerX=position.x;input.playerY=position.y;
        for(std::size_t slot=0;slot<input.slots.size();++slot){
            if(const auto enemy=world->bossEntity(slot)){
                input.slots[slot]={enemy->id,enemy->life,std::bit_cast<std::int32_t>(enemy->lifeWords[1]),enemy->flags,enemy->protection,enemy->x,enemy->y,enemy->z};
                input.markers[slot]=world->bossGauges().values[slot];
            }
        }
        if(!bossHud->tick(input,operationError)){world->reportRuntimeFault(operationError);return 0;}
        if(bossHud->state().sound>=0)grazeSounds.push_back({bossHud->state().sound,0});
    }
    if(success){
        for(auto& popup:scorePopups){popup[1]=float(double(popup[1])-.5);popup[4]+=1;}
        std::erase_if(scorePopups,[](const auto& popup){return popup[4]>60;});
    }
    if(success&&shotHitsConfigured){
        const auto player=world->playerAimPosition();
        if(!itemHost->tick({player.x,player.y,player.z},playerHit->snapshot().gate.stateF78,false,operationError)){
            world->reportRuntimeFault(operationError);return 0;
        }
    }
    if(success&&playerConfigured){
        const auto control=playerHit->snapshot().soundControl;
        if(!enemyVisuals->setUpdateControl({control.present,control.flags60})){
            world->reportRuntimeFault("enemy ANM control snapshot was rejected");return 0;
        }
    }
    if(success&&world->positionIntegrated()&&!enemyVisuals->tickAll(operationError)){world->reportRuntimeFault(operationError);return 0;}
    if(success&&world->positionIntegrated()&&!cancellationEffects->tickPrimary(operationError)){world->reportRuntimeFault(operationError);return 0;}
    if(success&&world->positionIntegrated()&&!laserLines->tickEffects(operationError)){world->reportRuntimeFault(operationError);return 0;}
    if(success&&playerConfigured&&!playerHit->tickLaserEffects(operationError)){world->reportRuntimeFault(operationError);return 0;}
    if(success&&playerShotsConfigured&&!playerShots->tickAnimationsUnit(operationError)){world->reportRuntimeFault(operationError);return 0;}
    if(success&&perfectHudConfigured){
        perfectHud->score(scoreRaw);
        if(!perfectHud->tick(operationError)){world->reportRuntimeFault(operationError);return 0;}
    }
    if(success&&geometryConfigured&&!geometryEffects->tickPrimary(operationError)){world->reportRuntimeFault(operationError);return 0;}
    if(success&&playerFireConfigured&&!iceHost->tickAnimations(operationError)){world->reportRuntimeFault(operationError);return 0;}
    if(success&&focusIndicatorConfigured&&!focusIndicator->tick(operationError)){world->reportRuntimeFault(operationError);return 0;}
    if(success&&messagesConfigured&&!messageVisuals->tick(operationError)){world->reportRuntimeFault(operationError);return 0;}
    if(success&&spellsConfigured){
        const auto enemy=world->bossEntity(0);const auto spell=world->phaseSpellContext();
        const std::array<float,3> position=enemy?std::array{enemy->x,enemy->y,enemy->z}:std::array{0.f,0.f,0.f};
        if(!spellVisuals->tick(position,spell&&(spell->flags&1u),operationError)){world->reportRuntimeFault(operationError);return 0;}
        if(spellResultFrames>0)--spellResultFrames;
    }
    if(world->positionIntegrated()) {
        const auto entities=world->entities();
        std::unordered_map<std::uint32_t,const th128::EnemySnapshot*> states;
        for(const auto& entity:entities)states.emplace(entity.id,&entity);
        std::unordered_set<std::uint32_t> positioned;
        std::unordered_set<std::uint32_t> liveStandalone;
        for(const auto& visual:enemyVisuals->snapshots()) {
            if(standaloneEffectOwners.contains(visual.entity)) {
                if(visual.parent==0)liveStandalone.insert(visual.entity);
                continue;
            }
            if(!positioned.insert(visual.entity).second)continue;
            const auto found=states.find(visual.entity);
            if(found==states.end()||!found->second->active){enemyVisuals->removeOwner(visual.entity);continue;}
            const auto& entity=*found->second;
            if(!enemyVisuals->position(entity.id,320+entity.x,16+entity.y,entity.z)) {
                world->reportRuntimeFault("Enemy animation position owner is unavailable");return 0;
            }
        }
        for(auto it=standaloneEffectOwners.begin();it!=standaloneEffectOwners.end();)
            if(!liveStandalone.contains(*it))it=standaloneEffectOwners.erase(it);else ++it;
    }
    if(success&&stageIntro&&stageIntro->loaded()){
        stageIntro->tick();
        if(!stageIntro->error().empty()){
            stageIntroDrawError=stageIntro->error();
            world->reportRuntimeFault(stageIntroDrawError);
            return 0;
        }
    }
    if(replayPlayback.active()){
        try{replayPlayback.commit(worldGeneration,success);}
        catch(const std::exception& failure){operationError=failure.what();return 0;}
    }
    if(success&&normalReplay){
        try{const bool recorded=normalReplay->append(normalReplayInput,normalReplayAuxiliary,normalReplayFPS,normalReplayClock-normalReplayOrigin);
            if(recorded&&normalRecord)normalRecord->activeFrame(playerHit->snapshot().soundControl.flags60,th128::record::NativeRecordSessionPolicy::stageClearHeld(stageManager.flags5a04,stageManager.field5a0c));
            normalReplayClockPending=false;}
        catch(const std::exception& failure){world->reportRuntimeFault(failure.what());return 0;}
    }
    if(success&&debugRecordDriver.bound)debugRecordDriver.phase=2;
    return success?1:0;
}
const char* th128_world_state() {
    if(ownedBattle.attempted()){
        if(normalTerminal.resourcesConfigured){snapshot=retiredOrdinaryLifecycle();return snapshot.c_str();}
        return retiredWorldSnapshot.c_str();
    }
    std::ostringstream text;
    const auto damageFlags=damageFields->flags();
    const auto perfectStatus=perfectHost?perfectHost->status():th128::perfect::Status{};
    const auto iceStatistics=iceHost->statistics();
    text<<std::setprecision(std::numeric_limits<float>::max_digits10);
    text<<"{\"frame\":"<<world->frame()<<",\"spawns\":"<<world->spawnCount()<<",\"rngCalls\":"<<world->randomCalls()
        <<",\"lastCreatedOriginalId\":"<<world->lastCreatedOriginalId()
        <<",\"linkedSources\":"<<world->linkedSources()<<",\"positionIntegrated\":"<<(world->positionIntegrated()?"true":"false")
        <<",\"grazeCount\":"<<grazeGlobals.grazeCount<<",\"powerRaw\":"<<grazeGlobals.powerRaw
        <<",\"motivationRaw\":"<<motivationRaw<<",\"damageStats\":["<<world->damageStats()[0]<<','<<world->damageStats()[1]<<']'
        <<",\"debug\":{\"infiniteRespawn\":"<<(debugInfiniteRespawnRun?"true":"false")
        <<",\"runTainted\":"<<(debugRunTainted?"true":"false")
        <<",\"reviveRefills\":"<<debugReviveRefills
        <<",\"lastRefillBefore\":"<<debugLastRefillBefore
        <<",\"lastRefillAfter\":"<<debugLastRefillAfter
        <<",\"sessionEnabled\":"<<(debugInfiniteRespawnSession?"true":"false")<<"}"
        <<",\"enemiesDefeated\":"<<enemiesDefeated
        <<",\"perfectHudNodes\":"<<perfectHud->snapshots().size()
        <<",\"geometry526Configured\":"<<(geometryConfigured?"true":"false")<<",\"geometry526Rows\":"<<geometryEffects->nodeCount()
        <<",\"deathsCommitted\":"<<deathsCommitted<<",\"perfectCharge\":"<<playerWindowControl.perfectCharge4b4d70
        <<",\"perfectFreeze\":{\"configured\":"<<(perfectConfigured?"true":"false")
        <<",\"active\":"<<(perfectStatus.active?"true":"false")
        <<",\"phase\":"<<perfectStatus.phase
        <<",\"charge\":"<<perfectStatus.charge
        <<",\"stock\":"<<playerWindowControl.perfectCharge4b4d70
        <<",\"systemMode3c\":"<<playerWindowControl.mode3c
        <<",\"protectionTicks\":"<<perfectStatus.protectionTicks
        <<",\"frozen\":"<<perfectStatus.frozen<<",\"cleared\":"<<perfectStatus.cleared<<",\"immune\":"<<perfectStatus.immune
        <<",\"lasers\":"<<perfectStatus.lasers<<",\"effect\":"<<(perfectStatus.effect?"true":"false")
        <<",\"failed\":"<<(perfectStatus.failed?"true":"false")<<",\"lastDenied\":";
    writeJsonString(text,perfectStatus.lastDenied);
    text<<",\"zDenied\":";
    writeJsonString(text,perfectZDenied);
    text<<",\"configurationError\":";
    writeJsonString(text,perfectConfigError);
    text<<"}"
        <<",\"respawnsCommitted\":"<<respawnsCommitted
        <<",\"damageFieldCount\":"<<std::count_if(damageFlags.begin(),damageFlags.end(),[](auto flags){return flags&1u;})
        <<",\"playerRespawnControl\":{\"mode04\":"<<respawnControl.mode04<<",\"pending\":[";
    for(std::size_t index=0;index<respawnControl.pending.size();++index){if(index)text<<',';text<<respawnControl.pending[index];}
    text<<"]}"
        <<",\"normalRecord\":";
    writeNormalRecordState(text);text
        <<",\"scoreRaw\":"<<scoreRaw<<",\"pointValueRaw\":"<<pointValueRaw
        <<",\"shotCycle\":"<<rapidShotCount
        <<",\"effectCount\":"<<cancellationEffects->nodeCount()<<",\"displayRandomCalls\":"<<worldDisplayRandom.calls
        <<",\"zFireConfigured\":"<<(playerFireConfigured?"true":"false")
        <<",\"heldZ\":"<<(playerConfigured?playerHit->snapshot().heldTimer.current:0)
        <<",\"cumulativeFrozenArea\":"<<cumulativeFrozenArea<<",\"lastFrozenArea\":"<<lastFrozenArea<<",\"freezeBonus\":"<<freezeBonus
        <<",\"freezeAreaPresentation\":\"original-native-area-bonus-panel\""
        <<",\"freezeRewardPanel\":{\"serial\":"<<freezeRewardPanel.serial()<<",\"active\":"<<(freezeRewardPanel.active()?"true":"false")
        <<",\"age\":"<<freezeRewardPanel.age()<<",\"fade\":"<<freezeRewardPanel.fade()<<",\"high\":"<<(freezeRewardPanel.high()?"true":"false")
        <<",\"near\":"<<(freezeRewardPanel.near()?"true":"false")<<",\"percent\":"<<freezeRewardPanel.percent()<<",\"bonus\":"<<freezeRewardPanel.bonus()<<"}"
        <<",\"ice\":{\"busy\":"<<iceStatistics.busy<<",\"power\":"<<iceStatistics.power
        <<",\"elapsed\":"<<iceStatistics.elapsed<<",\"objects\":"<<iceStatistics.objects
        <<",\"converted\":"<<iceStatistics.converted<<",\"shattered\":"<<iceStatistics.shattered
        <<",\"pixels\":"<<iceStatistics.pixels<<",\"completions\":"<<iceStatistics.completions<<'}'
        <<",\"enemyAnimationCount\":"<<enemyVisuals->snapshots().size()
        <<",\"cancellationCount\":"<<cancellationEffects->nodeCount()
        <<",\"playerState\":"<<(playerConfigured?playerHit->snapshot().gate.stateF78:-1)
        <<",\"playerWindow\":"<<(playerConfigured?playerHit->snapshot().stateTimer.current:-1)
        <<",\"playerLaserContactCounter43a90\":"<<(playerConfigured?playerHit->snapshot().laserContactCounter43a90:-1)
        <<",\"playerProtection\":"<<(playerConfigured?playerHit->snapshot().protection.current:-1)
        <<",\"playerX\":"<<world->playerAimPosition().x<<",\"playerY\":"<<world->playerAimPosition().y
        <<",\"playerAnimation\":"<<(playerConfigured&&playerHit->animationState()?playerHit->animationState()->script:-1)
        <<",\"screenShake\":["<<world->screenShake()[0]<<','<<world->screenShake()[1]<<']'
        <<",\"sceneControl\":["<<world->sceneControl()[0]<<','<<world->sceneControl()[1]<<']'
        <<",\"playerFocused\":"<<(playerFocused?"true":"false")<<",\"focusIndicatorConfigured\":"<<(focusIndicatorConfigured?"true":"false")
        <<",\"soundRequests\":"<<grazeSounds.size()<<",\"message\":";
    writeMessageState(text);text<<",\"stageTransition\":";
    writeStageTransitionState(text);
    text<<",\"debugTerminal\":";
    writeDebugTerminalState(text);
    text<<",\"normalTerminal\":";writeNormalTerminalState(text);
    text<<",\"stageControl\":";
    writeStageControlState(text);
    text<<",\"musicControl\":";
    writeMusicControlState(text);
    text<<",\"flow\":";
    writeFlowEvidence(text);
    text<<",\"spell\":{";
    const auto spell=world->phaseSpellContext();text<<"\"active\":"<<(spell&&(spell->flags&1u)?"true":"false")<<",\"id\":"<<(spell?spell->id:-1)<<",\"age\":"<<(spell?spell->age:0)<<",\"bonus\":"<<(spell?spell->value80:0)<<",\"flags\":"<<(spell?spell->flags:0)<<",\"title\":[";
    for(std::size_t index=0;index<world->spellTitle().size();++index){if(index)text<<',';text<<unsigned(world->spellTitle()[index]);}text<<"],\"pose\":";
    const auto pose=spellsConfigured?spellVisuals->titlePose():std::nullopt;
    if(pose)text<<"{\"x\":"<<pose->x<<",\"y\":"<<pose->y<<",\"sx\":"<<pose->state.sx<<",\"sy\":"<<pose->state.sy<<",\"color\":"<<pose->state.primary<<'}';else text<<"null";
    text<<",\"resultFrames\":"<<spellResultFrames<<",\"captured\":"<<(spellResult.captured?"true":"false")<<",\"resultBonus\":"<<spellResult.bonus<<"},\"phaseTransitions\":[";
    for(std::size_t index=0;index<world->phaseTransitions().size();++index){if(index)text<<',';const auto& phase=world->phaseTransitions()[index];text<<"{\"frame\":"<<phase.frame<<",\"entity\":"<<phase.entity<<",\"target\":\""<<phase.target<<"\",\"life\":"<<phase.life<<",\"timedOut\":"<<(phase.timedOut?"true":"false")<<'}';}text<<"],\"playerShots\":[";
    bool shotComma=false;
    for(const auto& shot:playerShots->snapshots()){
        if(shotComma)text<<',';shotComma=true;
        text<<"{\"id\":"<<shot.id<<",\"slot\":"<<shot.slot<<",\"script\":"<<shot.script<<",\"age\":"<<shot.age
            <<",\"damage\":"<<shot.damage<<",\"hitMarker\":"<<shot.hitMarker<<",\"x\":"<<shot.position.x<<",\"y\":"<<shot.position.y
            <<",\"active\":"<<shot.active<<",\"speed\":"<<shot.speed
            <<",\"visualTicks\":"<<shot.state.ticks<<"}";
    }
    text<<"],\"items\":[";
    bool itemComma=false;
    for(const auto& item:itemHost->snapshots()){
        if(itemComma)text<<',';itemComma=true;
        text<<"{\"id\":"<<item.id<<",\"slot\":"<<item.slot<<",\"kind\":"<<item.kind<<",\"age\":"<<item.age
            <<",\"x\":"<<item.position.x<<",\"y\":"<<item.position.y<<"}";
    }
    text<<"],\"bossGauges\":[";
    bool gaugeComma=false;
    for(std::size_t boss=0;boss<world->bossGauges().values.size();++boss)
        for(std::size_t slot=0;slot<world->bossGauges().values[boss].size();++slot){
            const auto& segment=world->bossGauges().values[boss][slot];if(!segment.configured)continue;
            if(gaugeComma)text<<',';gaugeComma=true;
            text<<"{\"boss\":"<<boss<<",\"slot\":"<<slot<<",\"ratio\":"<<segment.ratio<<",\"color\":"<<segment.color<<'}';
        }
    const auto& hud=bossHud->state();
    text<<"],\"bossHud\":{\"timerVisible\":"<<(hud.timerVisible?"true":"false")<<",\"seconds\":"<<hud.seconds<<",\"hundredths\":"<<hud.hundredths
        <<",\"color\":"<<hud.timerColor<<",\"scale\":"<<hud.timerScale<<",\"fills\":["<<hud.fills[0]<<','<<hud.fills[1]<<','<<hud.fills[2]
        <<"],\"visible\":["<<(hud.visible[0]?"true":"false")<<','<<(hud.visible[1]?"true":"false")<<','<<(hud.visible[2]?"true":"false")<<"]},\"geometry526Effects\":[";
    bool geometryComma=false;
    for(const auto& entity:world->entities())if(const auto effect=geometryEffects->effectState(entity.id)){
        if(geometryComma)text<<',';geometryComma=true;
        text<<"{\"owner\":"<<entity.id<<",\"radius\":"<<effect->radius<<",\"scale\":"<<effect->scale<<",\"color\":"<<effect->color
            <<",\"angles\":["<<effect->angles[0]<<','<<effect->angles[1]<<"]}";
    }
    text<<"],\"entities\":[";
    bool comma=false;
    for(const auto& entity:world->entities()) {
        if(comma)text<<',';comma=true;
        text<<"{\"id\":"<<entity.id<<",\"originalIdentifier\":"<<entity.originalIdentifier<<",\"flags\":"<<entity.flags<<",\"root\":"<<(entity.root?"true":"false")
            <<",\"x\":"<<entity.x<<",\"y\":"<<entity.y<<",\"angle\":"<<entity.angle<<",\"speed\":"<<entity.speed
            <<",\"appearanceWidth\":"<<entity.appearanceWidth<<",\"appearanceHeight\":"<<entity.appearanceHeight
            <<",\"life\":"<<entity.life<<",\"animation0\":"<<entity.animation[0]<<",\"bank0\":"<<entity.animationBanks[0]
            <<",\"hitSoundOverride\":"<<entity.hitSoundOverride<<",\"hitFeedbackCounter\":"<<entity.hitFeedbackCounter
            <<",\"hitFeedbackAuxiliaryFlags\":"<<entity.hitFeedbackAuxiliaryFlags
            <<",\"active\":"<<(entity.active?"true":"false")
            <<",\"phaseTime\":"<<entity.phaseTimer.current<<",\"phaseRemaining\":"<<entity.phaseRemaining
            <<",\"phaseThreshold\":"<<entity.phaseThreshold
            <<",\"phase0\":{\"threshold\":"<<entity.phases[0].threshold<<",\"limit\":"<<entity.phases[0].limit<<'}'
            <<",\"emitter0\":{\"count\":"<<entity.emitter0.count<<",\"layers\":"<<entity.emitter0.layers
            <<",\"type\":"<<entity.emitter0.type<<",\"color\":"<<entity.emitter0.color<<",\"speed\":"<<entity.emitter0.speed
            <<",\"extensionType\":"<<entity.emitter0.extension0.type<<"}}";
    }
    text<<"],\"bullets\":[";comma=false;
    for(const auto& bullet:world->bullets().snapshots()) {
        if(comma)text<<',';comma=true;
        const auto found=bulletAnimations.find(bullet.id);
        const auto* visual=found==bulletAnimations.end()?nullptr:bulletVisuals->animationState(found->second);
        text<<"{\"id\":"<<bullet.id<<",\"slot\":"<<bullet.slot<<",\"type\":"<<bullet.type<<",\"color\":"<<bullet.color
            <<",\"script\":"<<bullet.script<<",\"motionState\":"<<bullet.state.motionState
            <<",\"age\":"<<bullet.age<<",\"lifeClock\":"<<bullet.lifeClock<<",\"freezePolicy\":"<<bullet.state.freezePolicy
            <<",\"flags\":"<<bullet.state.flags
            <<",\"x\":"<<bullet.state.position.x<<",\"y\":"<<bullet.state.position.y<<",\"z\":"<<bullet.state.position.z
            <<",\"velocityX\":"<<bullet.state.velocity.x<<",\"velocityY\":"<<bullet.state.velocity.y
            <<",\"sprite\":"<<(visual?visual->sprite:-1)<<",\"integer0\":"<<(visual?visual->integers[0]:-1)
            <<",\"visualTicks\":"<<(visual?visual->ticks:0)<<"}";
    }
    text<<']';
    const auto actualLaserCount=world->laserObjectCount();
    text<<",\"laserCount\":";
    if(actualLaserCount)text<<*actualLaserCount;else text<<"null";
    if(actualLaserCount&&*actualLaserCount!=0){
        text<<",\"lasers\":[";comma=false;
        for(const auto& line:laserLines->snapshots()){
            if(comma)text<<',';comma=true;
            text<<"{\"serial\":"<<line.serial<<",\"epoch\":"<<line.epoch<<",\"owner\":"<<line.ownerEntity
                <<",\"age\":"<<line.age<<",\"type\":"<<line.type<<",\"color\":"<<line.color
                <<",\"x\":"<<line.source.x<<",\"y\":"<<line.source.y<<",\"z\":"<<line.source.z
                <<",\"angle\":"<<line.angle<<",\"length\":"<<line.length<<",\"width\":"<<line.width
                <<",\"speed\":"<<line.speed<<",\"travel\":"<<line.travel
                <<",\"maximumLength\":"<<line.maximumLength<<",\"travelLimit\":"<<line.travelLimit
                <<",\"velocityX\":"<<line.velocity.x<<",\"velocityY\":"<<line.velocity.y<<",\"velocityZ\":"<<line.velocity.z
                <<",\"grazeClock\":"<<line.grazeClock<<",\"boundsGrace\":"<<line.boundsGrace
                <<",\"nativeState\":"<<line.nativeState<<",\"guard5b4\":"<<line.guard5b4<<",\"retirement7c\":"<<unsigned(line.retirement7c)
                <<",\"shotSound\":"<<line.shotSound<<",\"transformSound\":"<<line.transformSound<<'}';
        }
        text<<']';
    }
    const auto laserEffects=laserLines->effectSnapshots();
    if(!laserEffects.empty()){
        text<<",\"laserEffects\":[";comma=false;
        for(const auto& effect:laserEffects){
            if(comma)text<<',';comma=true;const auto& state=effect.state;
            text<<"{\"id\":"<<effect.id<<",\"owner\":"<<effect.owner<<",\"parent\":"<<effect.parent
                <<",\"script\":"<<state.script<<",\"ticks\":"<<state.ticks<<",\"sprite\":"<<state.sprite
                <<",\"flags\":"<<state.flags<<",\"ended\":"<<(state.ended?"true":"false")
                <<",\"x\":"<<state.x<<",\"y\":"<<state.y<<",\"z\":"<<state.z
                <<",\"engineX\":"<<state.engineX<<",\"engineY\":"<<state.engineY<<",\"engineZ\":"<<state.engineZ<<'}';
        }
        text<<']';
    }
    text<<'}';snapshot=text.str();return snapshot.c_str();
}
const char* th128_world_bullet_draws() {
    presentationError.clear();
    th128::runtime::DrawJsonWriter text;text<<'[';bool comma=false;
    if(geometryConfigured){
        const auto main=geometryEffects->mainSnapshot(presentationError);
        const auto meshes=geometryEffects->snapshots(presentationError);
        if(!main||!meshes){bulletDraws="[]";return bulletDraws.c_str();}
        const auto& state=main->state;
        if(!writeVisualRecord(text,comma,10000000u,6,state,main->sprite,
            state.x+state.engineX+state.offsetX,state.y+state.engineY+state.offsetY,state.z+state.engineZ+state.offsetZ,false,"main")){
            bulletDraws="[]";return bulletDraws.c_str();
        }
        for(const auto& mesh:*meshes){
            if(!mesh.state.visible||!(mesh.state.flags&2u)||mesh.state.deleteRequested)continue;
            if(!writeVisualRecord(text,comma,11000000u+mesh.id,6,mesh.state,mesh.sprite,0,0,0,false,nullptr,&mesh)){
                bulletDraws="[]";return bulletDraws.c_str();
            }
        }
    }
    for(const auto& bullet:world->bullets().snapshots()) {
        const auto found=bulletAnimations.find(bullet.id);
        if(found==bulletAnimations.end()){presentationError="Missing committed bullet animation";bulletDraws="[]";return bulletDraws.c_str();}
        const auto* state=bulletVisuals->animationState(found->second);
        if(!state){presentationError="Stale bullet animation generation";bulletDraws="[]";return bulletDraws.c_str();}
        if(found->second.hasSecondary){
            const auto* secondary=bulletVisuals->animationState(found->second,true);
            if(secondary&&secondary->visible&&(secondary->flags&2U)&&!secondary->deleteRequested){
                const auto* secondarySprite=bulletVisuals->spriteInfo(secondary->sprite);
                if(!secondarySprite){presentationError="Unavailable secondary bullet sprite";bulletDraws="[]";return bulletDraws.c_str();}
                const auto secondaryRotation=th128::bullet::directionalDrawAngle(
                    bullet.angle,secondary->rotation,secondary->flags);
                if(!secondaryRotation){presentationError="unsupported secondary bullet draw angle";bulletDraws="[]";return bulletDraws.c_str();}
                auto secondaryState=*secondary;
                secondaryState.rotation=*secondaryRotation;
                if(!writeVisualRecord(text,comma,0x80000000u+bullet.id,th128::bullet::BulletVisuals::BankId,
                    secondaryState,*secondarySprite,
                    320+bullet.state.position.x+secondary->x+secondary->engineX+secondary->offsetX,
                    16+bullet.state.position.y+secondary->y+secondary->engineY+secondary->offsetY,
                    bullet.state.position.z+secondary->z+secondary->engineZ+secondary->offsetZ,
                    false,nullptr,nullptr,nullptr,"hostile-bullets")){
                    bulletDraws="[]";return bulletDraws.c_str();
                }
            }
        }
        if(!state->visible||!(state->flags&2U)||state->deleteRequested)continue;
        const auto* sprite=bulletVisuals->spriteInfo(state->sprite);
        if(!sprite){presentationError="Unavailable bullet sprite";bulletDraws="[]";return bulletDraws.c_str();}
        const auto width=state->spriteWidth<0?sprite->width:state->spriteWidth;
        const auto height=state->spriteHeight<0?sprite->height:state->spriteHeight;
        const auto rotation=th128::bullet::directionalDrawAngle(bullet.angle,state->rotation,state->flags);
        if(!rotation){presentationError="unsupported directional bullet draw angle";bulletDraws="[]";return bulletDraws.c_str();}
        const auto pixelX=320+bullet.state.position.x+state->x+state->engineX+state->offsetX;
        const auto pixelY=16+bullet.state.position.y+state->y+state->engineY+state->offsetY;
        const auto pixelZ=bullet.state.position.z+state->z+state->engineZ+state->offsetZ;
        const std::array scalars{width,height,pixelX,pixelY,pixelZ,state->rotation,state->sx,state->sy,
            state->u,state->v,state->uvScaleX,state->uvScaleY,sprite->u0,sprite->v0,sprite->u1,sprite->v1,state->rx,state->ry};
        if(!std::all_of(scalars.begin(),scalars.end(),[](float value){return std::isfinite(value);})) {
            presentationError="Non-finite bullet presentation value";bulletDraws="[]";return bulletDraws.c_str();
        }
        if(comma)text<<',';comma=true;
        text<<"{\"id\":"<<bullet.id<<",\"drawPass\":\"hostile-bullets\",\"parent\":0,\"bank\":"<<th128::bullet::BulletVisuals::BankId
            <<",\"sprite\":"<<state->sprite<<",\"entry\":"<<sprite->textureEntry<<",\"layer\":"<<state->layer
            <<",\"mode\":"<<state->mode<<",\"anchorX\":"<<state->anchorX<<",\"anchorY\":"<<state->anchorY
            <<",\"blend\":"<<state->blend<<",\"flags\":"<<state->flags<<",\"flags2\":"<<state->flags2
            <<",\"color\":"<<state->primary<<",\"secondary\":"<<state->secondary
            <<",\"width\":"<<width<<",\"height\":"<<height
            <<",\"x\":"<<pixelX<<",\"y\":"<<pixelY<<",\"z\":"<<pixelZ
            <<",\"rotation\":"<<*rotation<<",\"sx\":"<<state->sx<<",\"sy\":"<<state->sy
            <<",\"u\":"<<state->u<<",\"v\":"<<state->v<<",\"uvScaleX\":"<<state->uvScaleX
            <<",\"uvScaleY\":"<<state->uvScaleY<<",\"u0\":"<<sprite->u0<<",\"v0\":"<<sprite->v0
            <<",\"u1\":"<<sprite->u1<<",\"v1\":"<<sprite->v1<<",\"rx\":"<<state->rx<<",\"ry\":"<<state->ry<<"}";
    }
    if(laserLines->healthy()||laserLines->actualCount()!=0){
        const auto records=laserLines->draws(presentationError);
        if(!records){bulletDraws="[]";return bulletDraws.c_str();}
        for(const auto& record:*records){
            const auto& pose=record.pose;
            if(!writeVisualRecord(text,comma,record.id,0,pose,record.sprite,
                320+pose.x+pose.engineX+pose.offsetX,16+pose.y+pose.engineY+pose.offsetY,
                pose.z+pose.engineZ+pose.offsetZ,false,nullptr,nullptr,nullptr,"hostile-lasers")){
                bulletDraws="[]";return bulletDraws.c_str();
            }
        }
    }
    const auto selectorQuads=laserLines->selector3Draws(presentationError);
    if(!selectorQuads){bulletDraws="[]";return bulletDraws.c_str();}
    for(const auto& record:*selectorQuads){
        if(comma)text<<',';comma=true;
        text<<"{\"id\":"<<(0x70000000u+record.serial*512u+record.quad.column)<<",\"drawPass\":\"hostile-lasers\",\"layer\":0,\"mode\":0,\"blend\":1,\"color\":"<<record.quad.vertices[0].color
            <<",\"geometry\":{\"kind\":\"laser-selector3-quad\",\"owner\":"<<record.ownerEntity<<",\"column\":"<<record.quad.column<<",\"vertices\":[";
        bool vertexComma=false;
        for(const auto& vertex:record.quad.vertices){if(vertexComma)text<<',';vertexComma=true;
            text<<"{\"x\":"<<vertex.x<<",\"y\":"<<vertex.y<<",\"z\":"<<vertex.z<<",\"rhw\":"<<vertex.rhw<<",\"color\":"<<vertex.color<<'}';}
        text<<"]}}";
    }
    for(const auto& enemy:enemyVisuals->snapshots()) {
        const auto& state=enemy.state;
        if(!state.visible||!(state.flags&2U)||state.deleteRequested)continue;
        if(state.mode==16){
            const float centerX=enemy.world.x,centerY=enemy.world.y;
            const auto raw=th128::geometry::circleFan(centerX,centerY,enemy.world.radius,enemy.world.rotation,state.geometryCount,state.primary,state.secondary);
            if(!raw){presentationError="Invalid original mode16 circle fan state";bulletDraws="[]";return bulletDraws.c_str();}
            const auto& fan=*raw;
            const auto sprite=enemy.sprite?enemy.sprite:enemyVisuals->spriteInfo(11,313);
            if(!sprite){presentationError="Original mode16 fan resource metadata is unavailable";bulletDraws="[]";return bulletDraws.c_str();}
            const auto identity=enemy.parent?8000000u+enemy.node:2000000U+enemy.entity*16U+unsigned(enemy.slot);
            if(!writeVisualRecord(text,comma,identity,0,state,*sprite,centerX,centerY,enemy.world.z,false,nullptr,nullptr,&fan)){
                bulletDraws="[]";return bulletDraws.c_str();
            }
            continue;
        }
        const auto sprite=enemy.sprite?enemy.sprite:(state.sprite<0?enemyVisuals->spriteInfo(11,313):std::nullopt);
        if(!sprite){presentationError="Unavailable enemy animation sprite/font fallback";bulletDraws="[]";return bulletDraws.c_str();}
        auto composed=state;composed.sx=enemy.world.sx;composed.sy=enemy.world.sy;composed.rotation=enemy.world.rotation;
        const auto identity=enemy.parent?8000000u+enemy.node:2000000U+enemy.entity*16U+unsigned(enemy.slot);
        if(!writeVisualRecord(text,comma,identity,state.spriteBank,
            composed,*sprite,enemy.world.x,enemy.world.y,enemy.world.z)) {
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(perfectHudConfigured)for(const auto& node:perfectHud->snapshots()){
        const auto& state=node.state;
        if(!state.visible||!(state.flags&2u)||state.deleteRequested||!node.sprite)continue;
        if(!writeVisualRecord(text,comma,7000000u+node.id,5,state,*node.sprite,node.x,node.y,node.z)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(perfectHudConfigured){
        for(const auto& node:bossHud->snapshots()){
            const auto& state=node.state;if(!state.visible||!(state.flags&2u)||state.deleteRequested)continue;
            std::optional<std::vector<th128::geometry::RingVertex>> ring;
            if(state.mode==9||state.mode==14){ring=th128::geometry::ring(state,node.sprite,node.x,node.y,node.z);if(!ring){presentationError="invalid original Boss HP ring geometry";bulletDraws="[]";return bulletDraws.c_str();}}
            if(!writeVisualRecord(text,comma,16000000u+node.id,5,state,node.sprite,node.x,node.y,node.z,false,nullptr,nullptr,nullptr,nullptr,ring?&*ring:nullptr)){
                bulletDraws="[]";return bulletDraws.c_str();
            }
        }
        if(scoreFontLoaded)for(const auto& glyph:bossHud->glyphs()){
            const auto bank=scoreFont.bank(11);if(!bank||glyph.sprite<0||std::size_t(glyph.sprite)>=bank->sprites.size()){presentationError="original countdown font unavailable";bulletDraws="[]";return bulletDraws.c_str();}
            th128::anm::State state;state.sprite=glyph.sprite;state.layer=20;state.mode=0;state.anchorX=1;state.anchorY=1;state.flags2|=4u;
            state.primary=glyph.color;state.sx=state.sy=glyph.scale;
            if(!writeVisualRecord(text,comma,17000000u+unsigned(glyph.sprite),11,state,bank->sprites[std::size_t(glyph.sprite)],glyph.x,glyph.y,0)){
                bulletDraws="[]";return bulletDraws.c_str();
            }
        }
    }
    for(const auto& effect:cancellationEffects->snapshots()) {
        const auto& state=effect.state;
        if(!state.visible||!(state.flags&2U)||state.deleteRequested)continue;
        if(!effect.sprite){presentationError="Unavailable cancellation sprite";bulletDraws="[]";return bulletDraws.c_str();}
        auto composed=state;
        composed.sx=effect.world.sx;composed.sy=effect.world.sy;composed.rotation=effect.world.rotation;
        if(!writeVisualRecord(text,comma,3000000U+effect.node,0,composed,*effect.sprite,effect.world.x,effect.world.y,effect.world.z,true)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    const auto laserEffects=laserLines->effectDraws(presentationError);
    if(!laserEffects){bulletDraws="[]";return bulletDraws.c_str();}
    for(const auto& effect:*laserEffects){
        if(!writeVisualRecord(text,comma,effect.id,0,effect.pose,effect.sprite,
            effect.pose.x,effect.pose.y,effect.pose.z,true)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(playerConfigured){
        const auto* state=playerHit->animationState();const auto* sprite=playerHit->spriteInfo();
        if(state&&sprite&&state->visible&&(state->flags&2U)&&!state->deleteRequested){
            const auto position=world->playerAimPosition();
            const auto outline=playerHit->powerOutline(grazeGlobals.powerRaw);
            if(!outline){presentationError="Original player outline sprite metadata is unavailable";bulletDraws="[]";return bulletDraws.c_str();}
            if((outline->state.primary>>24)&&!writeVisualRecord(text,comma,4000001U,7,outline->state,outline->sprite,
                320+position.x+state->x+state->engineX+state->offsetX,16+position.y+state->y+state->engineY+state->offsetY,
                position.z+state->z+state->engineZ+state->offsetZ,false,nullptr,nullptr,nullptr,"player")){
                bulletDraws="[]";return bulletDraws.c_str();
            }
            if(!writeVisualRecord(text,comma,4000000U,7,*state,*sprite,320+position.x+state->x+state->engineX+state->offsetX,
                16+position.y+state->y+state->engineY+state->offsetY,position.z+state->z+state->engineZ+state->offsetZ,false,nullptr,nullptr,nullptr,"player")){
                bulletDraws="[]";return bulletDraws.c_str();
            }
        }
    }
    if(playerConfigured){
        const auto effects=playerHit->laserEffectSnapshots(presentationError);
        if(!presentationError.empty()){bulletDraws="[]";return bulletDraws.c_str();}
        for(const auto& effect:effects){
            if(!effect.state.visible||!(effect.state.flags&2u)||effect.state.deleteRequested)continue;
            if(!writeVisualRecord(text,comma,0x18000000u+effect.id,7,effect.state,effect.sprite,effect.x,effect.y,effect.z,true)){
                bulletDraws="[]";return bulletDraws.c_str();
            }
        }
    }
    for(const auto& shot:playerShots->snapshots()){
        const auto& state=shot.state;
        if(!state.visible||!(state.flags&2u)||state.deleteRequested)continue;
        if(!writeVisualRecord(text,comma,5000000u+shot.id,7,state,shot.sprite,
            state.engineX+state.x+state.offsetX,state.engineY+state.y+state.offsetY,state.engineZ+state.z+state.offsetZ)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(playerShotGroupsConfigured){
        for(const auto& shot:playerShotGroups->snapshots()){
            const auto& state=shot.state;
            if(!state.visible||!(state.flags&2u)||state.deleteRequested||!shot.sprite)continue;
            if(!writeVisualRecord(text,comma,5200000u+shot.handle,7,state,*shot.sprite,
                320+shot.x,16+shot.y,shot.z,false,nullptr,nullptr,nullptr,"playerShotGroup")){
                bulletDraws="[]";return bulletDraws.c_str();
            }
        }
    }
    for(const auto& item:itemHost->snapshots()){
        const auto& state=item.state;
        if(!writeVisualRecord(text,comma,6000000u+item.id,0,state,item.sprite,320+item.position.x,16+item.position.y,item.position.z,false,nullptr,nullptr,nullptr,"items")){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(scoreFontLoaded){
        const auto bank=scoreFont.bank(11);
        for(std::size_t popupIndex=0;popupIndex<scorePopups.size();++popupIndex){
            const auto& popup=scorePopups[popupIndex];const auto digits=std::to_string(int(popup[3]));
            const float spacing=popup[4]<8&&popup[4]>0?8.f/popup[4]:8.f;
            const auto player=world->playerAimPosition();const double dx=double(player.x)-popup[0],dy=double(player.y)-popup[1];
            const int distance=int(dx*dx+dy*dy);const int alpha=distance>16384?208:distance<=4096?48:48+(distance*5-20480)*32/49152;
            for(std::size_t digit=0;digit<digits.size();++digit){
                const int sprite=(popup[4]>=56?217:popup[4]>=52?207:196)+(digits[digit]-'0');
                if(!bank||std::size_t(sprite)>=bank->sprites.size()){presentationError="score digit sprite unavailable";bulletDraws="[]";return bulletDraws.c_str();}
                th128::anm::State state;state.sprite=sprite;state.layer=30;state.mode=0;state.anchorX=1;state.anchorY=1;
                state.primary=(std::uint32_t(alpha)<<24)|(popup[4]>=56?0xffffffu:0xffff00u);
                const auto& glyph=bank->sprites[std::size_t(sprite)];
                state.spriteWidth=spacing;
                if(!writeVisualRecord(text,comma,9000000u+unsigned(popupIndex*16+digit),11,state,glyph,
                    320+popup[0]-float(digits.size())*spacing*.5f+float(digit)*spacing,16+popup[1],popup[2])){
                    bulletDraws="[]";return bulletDraws.c_str();
                }
            }
        }
    }
    if(focusIndicatorConfigured)for(const auto& focus:focusIndicator->draws()){
        if(!focus.state.visible||!(focus.state.flags&2u)||focus.state.deleteRequested)continue;
        if(!writeVisualRecord(text,comma,14000000u+focus.id,0,focus.state,focus.sprite,focus.x,focus.y,focus.z)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(playerFireConfigured)for(const auto& ice:iceHost->draws()){
        if(!ice.state.visible||!(ice.state.flags&2u)||ice.state.deleteRequested)continue;
        if(!writeVisualRecord(text,comma,13000000u+ice.id,ice.bank,ice.state,ice.sprite,ice.x,ice.y,ice.z,true)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(perfectConfigured)for(const auto& effect:perfectHost->effectSnapshots()){
        const auto& state=effect.state;
        if(!effect.sprite||!state.visible||!(state.flags&2u)||state.deleteRequested)continue;
        if(!writeVisualRecord(text,comma,13500000u+effect.id,effect.bank,state,*effect.sprite,
            effect.x,effect.y,effect.z,true)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(stageTransitionConfigured)for(const auto& item:stageTransition->snapshots()){
        const auto& state=item.state;
        if(!item.sprite||!state.visible||state.deleteRequested)continue;
        if(!writeVisualRecord(text,comma,21000000u+item.id,5,state,*item.sprite,item.x,item.y,item.z)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(debugTerminal.configured)for(const auto& item:debugTerminalSettlement->snapshots()){
        const auto& state=item.state;
        if(!item.sprite||!state.visible||state.deleteRequested)continue;
        if(!writeVisualRecord(text,comma,21500000u+item.id,5,state,*item.sprite,item.x,item.y,item.z)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(normalTerminal.resourcesConfigured&&normalTerminal.settlement)for(const auto& item:normalTerminal.settlement->snapshots()){
        const auto& state=item.state;
        if(!item.sprite||!state.visible||state.deleteRequested)continue;
        if(!writeVisualRecord(text,comma,21600000u+item.id,5,state,*item.sprite,item.x,item.y,item.z)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(routeChoiceConfigured)for(const auto& item:routeChoice->snapshots()){
        const auto& state=item.state;
        if(!item.sprite||!state.visible||state.deleteRequested)continue;
        if(!writeVisualRecord(text,comma,20000000u+item.id,5,state,*item.sprite,item.x,item.y,item.z)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(messagesConfigured)for(const auto& item:messageVisuals->snapshots()){
        const auto& state=item.state;
        if(!item.sprite||!state.visible||!(state.flags&2u)||state.deleteRequested||!(state.primary>>24)||state.spriteBank==6)continue;
        if(!writeVisualRecord(text,comma,15000000u+item.id,state.spriteBank,state,*item.sprite,item.x,item.y,item.z)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    if(spellsConfigured)for(const auto& item:spellVisuals->snapshots()){
        const auto& state=item.state;if(!item.sprite||state.spriteBank==6||!state.visible||!(state.flags&2u)||state.deleteRequested)continue;
        std::optional<std::vector<th128::geometry::RingVertex>> ring;
        std::optional<std::vector<th128::geometry::ColorVertex>> fan;
        if(state.mode==9){ring=th128::geometry::ring(state,*item.sprite,item.x,item.y,item.z);if(!ring){presentationError="original spell ring geometry rejected";bulletDraws="[]";return bulletDraws.c_str();}}
        if(state.mode==16||state.mode==17){const float radius=float(state.geometryRadius*state.sx);fan=state.mode==16?th128::geometry::circleFan(item.x,item.y,radius,state.rotation,state.geometryCount,state.primary,state.secondary):th128::geometry::circleLine(item.x,item.y,radius,state.rotation,state.geometryCount,state.primary);if(!fan){presentationError="original level-up diffuse geometry rejected";bulletDraws="[]";return bulletDraws.c_str();}}
        if(!writeVisualRecord(text,comma,30000000u+item.id,state.spriteBank,state,*item.sprite,item.x,item.y,item.z,false,nullptr,nullptr,fan?&*fan:nullptr,nullptr,ring?&*ring:nullptr)){bulletDraws="[]";return bulletDraws.c_str();}
    }
    if(headPowerConfigured){
        const auto player=world->playerAimPosition();
        const auto head=th128::hud::headPower(grazeGlobals.powerRaw,playerHit->snapshot().heldTimer.current,player.x,player.y);
        const auto bank=scoreFont.bank(11);
        if(!head||!bank){presentationError="head power state/font unavailable";bulletDraws="[]";return bulletDraws.c_str();}
        unsigned index=0;
        for(const auto& glyph:head->glyphs){
            th128::anm::State state;state.sprite=glyph.sprite;state.layer=23;state.mode=0;state.anchorX=1;state.anchorY=1;
            state.flags2|=4u;
            state.primary=head->color;state.sx=state.sy=head->scale;
            if(!writeVisualRecord(text,comma,12000000u+index++,11,state,bank->sprites[std::size_t(glyph.sprite)],glyph.x,glyph.y,0)){
                bulletDraws="[]";return bulletDraws.c_str();
            }
        }
    }
    if(playerFireConfigured&&scoreFontLoaded){
        const auto bank=scoreFont.bank(11);unsigned index=0;
        for(const auto& glyph:freezeRewardPanel.glyphs()){
            if(!bank||glyph.sprite<0||std::size_t(glyph.sprite)>=bank->sprites.size()){
                presentationError="original freeze reward panel glyph unavailable";bulletDraws="[]";return bulletDraws.c_str();
            }
            th128::anm::State state;state.sprite=glyph.sprite;state.layer=0;state.mode=0;state.anchorX=state.anchorY=1;
            state.flags2|=4u;state.primary=glyph.color;state.sx=state.sy=glyph.scale;state.blend=0;
            if(!writeVisualRecord(text,comma,42000000u+index++,11,state,bank->sprites[std::size_t(glyph.sprite)],
                glyph.x,glyph.y,0,false,nullptr,nullptr,nullptr,"original-freeze-reward-panel")){
                bulletDraws="[]";return bulletDraws.c_str();
            }
        }
    }
    if(perfectHudConfigured&&scoreFontLoaded){
        const auto percentages=th128::hud::originalPercentHUD(motivationRaw,grazeGlobals.powerRaw,
            playerWindowControl.perfectCharge4b4d70,cumulativeFrozenArea);
        const auto bank=scoreFont.bank(11);
        if(!percentages||!bank){presentationError="original percentage HUD owner/font unavailable";bulletDraws="[]";return bulletDraws.c_str();}
        unsigned index=0;
        for(const auto& line:*percentages)for(const auto& glyph:line.glyphs){
            if(glyph.sprite<0||std::size_t(glyph.sprite)>=bank->sprites.size()){
                presentationError="original style4 percentage glyph unavailable";bulletDraws="[]";return bulletDraws.c_str();
            }
            th128::anm::State state;state.sprite=glyph.sprite;state.layer=0;state.mode=0;state.anchorX=state.anchorY=1;
            state.primary=0xffffffffu;state.blend=0;
            if(!writeVisualRecord(text,comma,41000000u+index++,11,state,bank->sprites[std::size_t(glyph.sprite)],
                glyph.x,glyph.y,0,false,nullptr,nullptr,nullptr,"original-percent-hud")){
                bulletDraws="[]";return bulletDraws.c_str();
            }
        }
    }
    if(perfectHudConfigured&&scoreFontLoaded){
        const int shown=perfectHud->displayedScore();
        const int savedHigh=normalRecord&&normalRecord->started()?int(normalRecord->highScoreRaw()):debugRecordPrefix.worldBound?debugRecordPrefix.highScoreRaw:100000;
        const int savedContinues=normalRecord&&normalRecord->started()?int(normalRecord->highScoreContinue()):debugRecordPrefix.worldBound?debugRecordPrefix.highScoreContinue:0;
        const int high=std::max(shown,std::max(0,savedHigh));
        const int highContinues=high==shown?originalContinueCount:std::clamp(savedContinues,0,9);
        const auto lines=th128::hud::originalScoreHUD(shown,high,originalContinueCount,highContinues);
        const auto bank=scoreFont.bank(11);
        if(!lines||!bank){presentationError="original score HUD state/font unavailable";bulletDraws="[]";return bulletDraws.c_str();}
        unsigned identity=0;
        for(const auto& line:*lines)for(const auto& glyph:line.glyphs){
            if(glyph.sprite<0||std::size_t(glyph.sprite)>=bank->sprites.size()){presentationError="original score HUD glyph unavailable";bulletDraws="[]";return bulletDraws.c_str();}
            th128::anm::State state;state.sprite=glyph.sprite;state.layer=0;state.mode=0;state.anchorX=state.anchorY=1;
            state.primary=line.color;state.blend=0;
            if(!writeVisualRecord(text,comma,41200000u+identity++,11,state,bank->sprites[std::size_t(glyph.sprite)],
                glyph.x,glyph.y,0,false,nullptr,nullptr,nullptr,"original-percent-hud")){bulletDraws="[]";return bulletDraws.c_str();}
        }
    }
    if(!writeStageIntroDraws(text,comma)){
        bulletDraws="[]";return bulletDraws.c_str();
    }
    if(normalReplaySave){
        const auto bank=scoreFont.bank(11);if(!bank){presentationError="original replay font bank unavailable";bulletDraws="[]";return bulletDraws.c_str();}
        unsigned glyphIdentity=0;
        const auto paint=[&](const std::vector<th128::pause::OriginalMenuGlyph>& glyphs){
            for(const auto& glyph:glyphs){
                if(glyph.sprite>=bank->sprites.size()){presentationError="original replay ASCII glyph unavailable";return false;}
                th128::anm::State state;state.sprite=int(glyph.sprite);state.layer=31;state.mode=0;state.anchorX=state.anchorY=1;state.primary=glyph.color;state.blend=0;
                if(!writeVisualRecord(text,comma,47500000u+glyphIdentity++,11,state,bank->sprites[glyph.sprite],glyph.x,glyph.y,0,false,nullptr,nullptr,nullptr,"original-menu-font0"))return false;
            }
            return true;
        };
        const auto& save=*normalReplaySave;
        try{
            if(save.phase()==1){
                for(unsigned index=0;index<25;++index){
                    const auto label=normalReplayPreviewRows[index].line(index);
                    if(!paint(th128::pause::originalMenuGlyphs(label,144,64+15*float(index),index==save.slot()?0xffffff00u:0xff808080u))){bulletDraws="[]";return bulletDraws.c_str();}
                }
            }else if(save.phase()==2){
                if(!paint(th128::record::originalReplayNameGlyphs(save))){bulletDraws="[]";return bulletDraws.c_str();}
                if(normalReplayNameContext.matches(save.nameEpoch())
                   &&!paint(th128::pause::originalMenuGlyphs(normalReplayNameContext.line(save.slot()),144,64,0xffffffffu))){
                    bulletDraws="[]";return bulletDraws.c_str();
                }
            }
        }catch(const std::exception& failure){presentationError=failure.what();bulletDraws="[]";return bulletDraws.c_str();}
    }
    if(originalPauseMenu&&originalPauseMenu->state().active&&originalPauseMenu->backdropLoaded()){
        const auto& backdrop=originalPauseMenu->backdrop();const auto shade=backdrop.shade();
        if(!writeVisualRecord(text,comma,47100000u+shade.id,shade.bank,shade.state,shade.sprite,shade.x,shade.y,shade.z,false,nullptr,nullptr,nullptr,"original-pause-shade")){
            bulletDraws="[]";return bulletDraws.c_str();
        }
        for(const auto& row:backdrop.draws()){
            if(!writeVisualRecord(text,comma,47200000u+row.id,row.bank,row.state,row.sprite,row.x,row.y,row.z,false,nullptr,nullptr,nullptr,"original-pause-backdrop")){
                bulletDraws="[]";return bulletDraws.c_str();
            }
        }
    }
    if(!normalReplaySave&&originalPauseMenu&&originalPauseMenu->state().active)for(const auto& draw:originalPauseMenu->draws()){
        if(!writeVisualRecord(text,comma,47000000u+draw.id,draw.bank,draw.state,draw.sprite,draw.x,draw.y,draw.z)){
            bulletDraws="[]";return bulletDraws.c_str();
        }
    }
    text<<']';bulletDraws=text.str();return bulletDraws.c_str();
}
int th128_world_debug_record_accept_cpu_presentation(unsigned epoch){
    operationError.clear();if(!debugRecordDriver.bound||debugRecordDriver.phase!=2||epoch!=debugRecordDriver.epoch
        ||debugRecordDriver.generation!=worldGeneration||world->faulted()||world->frame()!=debugRecordDriver.completedFrames+1
        ||!grazeSounds.empty()||soundJournalWindowSerial!=0){operationError="CPU presentation requires its actual completed update and drained sound owner";return 0;}
    debugRecordDriver.presentationFrame=unsigned(world->frame());debugRecordDriver.phase=3;return 1;
}
const char* th128_world_debug_record_presentation(unsigned epoch) {
    operationError.clear();debugRecordPresentationSnapshot.clear();
    if(!debugRecordDriver.bound||debugRecordDriver.phase!=2||epoch!=debugRecordDriver.epoch
        ||debugRecordDriver.generation!=worldGeneration||world->faulted()
        ||world->frame()!=debugRecordDriver.completedFrames+1||!grazeSounds.empty()
        ||soundJournalWindowSerial!=0){
        operationError="record presentation requires the actual completed update and sound boundary";
        return debugRecordPresentationSnapshot.c_str();
    }
    const std::string draws=th128_world_bullet_draws();
    if(!presentationError.empty())return debugRecordPresentationSnapshot.c_str();
    std::ostringstream output;
    output<<R"({"epoch":)"<<epoch<<R"(,"generation":)"<<worldGeneration
        <<R"(,"frame":)"<<world->frame()<<R"(,"serial":)"<<debugRecordDriver.completedFrames+1
        <<R"(,"draws":)"<<draws<<'}';
    debugRecordPresentationSnapshot=output.str();
    debugRecordDriver.presentationFrame=unsigned(world->frame());debugRecordDriver.phase=3;
    return debugRecordPresentationSnapshot.c_str();
}

const char* th128_world_bullet_draw_error(){return presentationError.c_str();}
unsigned th128_world_sound_epoch(){return soundJournalSealed?0:soundJournalEpoch;}
unsigned th128_world_sound_base_low(){return unsigned(soundJournalBase&0xffffffffu);}
unsigned th128_world_sound_base_high(){return unsigned(soundJournalBase>>32);}
int th128_world_sound_begin_window(){
    operationError.clear();
    if(soundJournalSealed||soundJournalEpoch==0){operationError="Sound journal epoch sequence is unavailable";return 0;}
    if(grazeSounds.empty())return 0;
    const unsigned serial=nextDeliverySerial();
    if(serial==0){soundJournalSealed=true;operationError="Sound journal window sequence is exhausted";return 0;}
    soundJournalWindowSerial=serial;
    soundJournalWindowEpoch=soundJournalEpoch;
    soundJournalWindowBase=soundJournalBase;
    soundJournalWindowCount=unsigned(grazeSounds.size());
    return int(soundJournalWindowSerial);
}
unsigned th128_world_sound_window_serial(){return soundJournalWindowSerial;}
int th128_world_sound_window_count(){return soundJournalWindowSerial==0?-1:int(soundJournalWindowCount);}
const char* th128_world_sound_window_state(){
    static std::string result;
    std::ostringstream text;
    text<<'{'<<"\"serial\":"<<soundJournalWindowSerial<<','<<"\"epoch\":"<<soundJournalWindowEpoch<<','
        <<"\"baseLow\":"<<unsigned(soundJournalWindowBase&0xffffffffu)<<','
        <<"\"baseHigh\":"<<unsigned(soundJournalWindowBase>>32)<<','
        <<"\"count\":"<<soundJournalWindowCount<<'}';
    result=text.str();return result.c_str();
}
int th128_world_sound_ack_window(unsigned serial,unsigned count,unsigned baseLow,unsigned baseHigh,unsigned epoch){
    operationError.clear();
    if(soundJournalSealed||epoch==0||epoch!=soundJournalEpoch){operationError="Sound journal acknowledgement epoch is stale";return -1;}
    if(serial==0||serial!=soundJournalWindowSerial){operationError="Sound journal acknowledgement window is not owned";return -1;}
    if(epoch!=soundJournalWindowEpoch){operationError="Sound journal acknowledgement window epoch changed";return -1;}
    const std::uint64_t base=(std::uint64_t(baseHigh)<<32)|std::uint64_t(baseLow);
    if(base!=soundJournalBase||base!=soundJournalWindowBase){operationError="Sound journal acknowledgement base is stale";return -1;}
    if(count>soundJournalWindowCount){operationError="Sound journal acknowledgement exceeds the read window";return -1;}
    if(count==0){
        if(soundJournalWindowCount==0){soundJournalWindowSerial=0;soundJournalWindowEpoch=0;}
        return int(grazeSounds.size());
    }
    grazeSounds.erase(grazeSounds.begin(),grazeSounds.begin()+static_cast<std::ptrdiff_t>(count));
    soundJournalBase+=count;
    soundJournalWindowBase=soundJournalBase;
    soundJournalWindowCount-=count;
    if(soundJournalWindowCount==0){soundJournalWindowSerial=0;soundJournalWindowEpoch=0;}
    return int(grazeSounds.size());
}
int th128_world_sound_restore_boundary(){
    operationError.clear();
    soundJournalWindowSerial=0;
    soundJournalWindowCount=0;
    soundJournalWindowEpoch=0;
    soundJournalWindowBase=soundJournalBase;
    return int(grazeSounds.size());
}
unsigned th128_world_sound_count(){return unsigned(grazeSounds.size());}
int th128_world_sound_id(unsigned index){return index<grazeSounds.size()?grazeSounds[index].identifier:-1;}
float th128_world_sound_x(unsigned index){return index<grazeSounds.size()?grazeSounds[index].positionX:0;}
int th128_world_sound_operation(unsigned index){return index<grazeSounds.size()?grazeSounds[index].operation:-1;}
}

namespace {
bool startOrdinaryTerminalEnding(const void* message,unsigned messageBytes,const void* art,unsigned artBytes,
        const void* base,unsigned baseBytes,const void* text,unsigned textBytes,std::string& error){
    error.clear();
    if(!world||world->faulted()||worldTickExecuting||!normalTerminal.resourcesConfigured||!normalTerminal.begun
        ||!normalTerminal.requested||!normalTerminal.messageDone||normalTerminal.counter!=380||normalTerminal.runtimeScene!=15
        ||normalTerminal.endingStarted||normalTerminal.sessionRetired||!normalTerminal.owner
        ||!normalTerminal.transition.priority1Ready(worldGeneration,world->frame())
        ||normalTerminal.generation!=worldGeneration||normalTerminal.stage!=stageIdForEcl(worldStartedName)
        ||!normalRecord||!normalReplay||!normalRecord->prepared()||!normalRecord->acknowledged()
        ||!normalRecord->terminalStatsPrepared()||normalRecord->terminalScore()!=scoreRaw
        ||normalRecord->terminalContinues()!=unsigned(originalContinueCount)
        ||!normalTerminal.owner->matchesContinuation(worldGeneration,unsigned(originalContinueCount),normalRecord,normalReplay)
        ||!musicControl.active||!musicControl.acknowledged||musicControl.operation!=5||!normalTerminal.musicSerial
        ||musicControl.serial!=normalTerminal.musicSerial||stageControl.current()!=7||stageControl.pending()!=15||stageControl.gateSet()
        ||!ordinaryCarrySessionPresent(true)||!playerConfigured||!playerHit||debugRunTainted||debugInfiniteRespawnRun||debugRecordPrefix.worldBound||replayPlayback.active()
        ||ownedBattle.attempted()||ordinaryDetachedEnding||!battleEndingRuntime15||battleEndingRuntime15->loaded()
        ||battleEndingRuntime15->cleaned()||!grazeSounds.empty()||soundJournalWindowSerial
        ||!th128::runtime::inputRange(message,messageBytes)||messageBytes>1024u*1024u
        ||!th128::runtime::inputRange(art,artBytes)||artBytes>16u*1024u*1024u
        ||!th128::runtime::inputRange(base,baseBytes)||baseBytes>16u*1024u*1024u
        ||!th128::runtime::inputRange(text,textBytes)||textBytes>16u*1024u*1024u){
        error="ordinary ending export lacks its acknowledged actual terminal owner";return false;
    }
    const auto generation=worldGeneration;
    const auto record=normalRecord;const auto replay=normalReplay;
    const auto bytes=[](const void* pointer,unsigned length){return std::span<const std::uint8_t>(static_cast<const std::uint8_t*>(pointer),length);};
    auto sound=[](std::string& reason){
        if(grazeSounds.size()>=65536){reason="ordinary ending SE0 queue full";return false;}
        grazeSounds.push_back({0,0});return true;
    };
    auto controls=[]()->std::optional<th128::ending::FadeControls>{
        if(!normalTerminal.sessionRetired||!ownedBattle.retired()||playerConfigured||playerHit)return std::nullopt;
        return ordinaryGlobalTaskControls();
    };
    const auto committed=stageControl.constructEnding15([&]{
        retiredWorldSnapshot=th128_world_state();
        ordinaryDetachedEnding=std::move(battleEndingRuntime15);
        auto permit=normalTerminal.owner->retireForEnding(generation,unsigned(originalContinueCount),record,replay,stageControl,[&]{
            if(!ownedBattle.retire(error))return false;
            stageSessionPresent=0;
            playerConfigured=playerMotion=playerShotsConfigured=playerShotGroupsConfigured=false;
            messagesConfigured=messageFrontConfigured=routeChoiceConfigured=stageTransitionConfigured=false;
            normalTerminal.settlement.reset();
            normalTerminal.sessionRetired=true;
            for(int identifier=0;identifier<76;++identifier)grazeSounds.push_back({identifier,0,1});
            return worldGeneration==generation&&normalRecord==record&&normalReplay==replay;
        },error);
        if(!permit)return false;
        const auto* retainedPermit=permit.get();
        if(!ordinaryDetachedEnding->startOrdinary(std::move(permit),stageControl.flags(),
            bytes(message,messageBytes),bytes(art,artBytes),bytes(base,baseBytes),bytes(text,textBytes),sound,controls,error))return false;
        if(!normalTerminal.transition.enterEnding(*retainedPermit,ordinaryGlobalTaskControls(),error))return false;
        projectOrdinaryGlobalFade();return true;
    });
    if(committed.status!=th128::stage::CommitStatus::Applied){
        if(error.empty())error="ordinary ending actual controller/retirement construction failed";
        return false;
    }
    normalTerminal.endingStarted=true;return true;
}
}

extern "C" {
int th128_world_start_ending(const void* message,unsigned messageBytes,const void* art,unsigned artBytes,const void* base,unsigned baseBytes,const void* text,unsigned textBytes){
    operationError.clear();
    if(normalTerminal.resourcesConfigured)return startOrdinaryTerminalEnding(message,messageBytes,art,artBytes,base,baseBytes,text,textBytes,operationError)?1:0;
    if(!world||world->faulted()||!debugTerminal.requested||debugTerminal.runtimeScene!=15||!debugTerminal.messageDone||debugTerminal.counter!=380||!playerConfigured||!debugRunTainted||!message||!art||!base||!text||activeEndingRuntime()->loaded()||activeEndingRuntime()->cleaned()){operationError="actualsameWorldBossruntime15requestrequired";return 0;}
    const th128::ending::RuntimeGate gate{debugTerminal.requested,debugTerminal.runtimeScene,debugTerminal.index,debugTerminal.stage,worldGeneration,debugRunTainted,stageControl.flags()};
    auto sound=[](std::string& error){if(grazeSounds.size()>=65536){error="actualendingSE0queuefull";return false;}grazeSounds.push_back({0,0});return true;};
    auto controls=[]()->std::optional<th128::ending::FadeControls>{
        const th128::anm::Runtime unitScheduler;
        if(ordinaryRecordOwner.configured&&ordinaryRecordOwner.sessionRetired){
            if(playerConfigured||playerHit)return std::nullopt;
            return th128::ending::FadeControls{0,false,0,unitScheduler.delta};
        }
        if(!playerConfigured||!playerHit)return std::nullopt;
        const auto actual=playerHit->snapshot().soundControl;
        return th128::ending::FadeControls{0,actual.present,actual.flags60,unitScheduler.delta};
    };
    const auto bytes=[](const void* pointer,unsigned length){return std::span<const std::uint8_t>(static_cast<const std::uint8_t*>(pointer),length);};
    const auto construct=[&]{
        return activeEndingRuntime()->start(gate,bytes(message,messageBytes),bytes(art,artBytes),
            bytes(base,baseBytes),bytes(text,textBytes),sound,controls,operationError)==true;
    };
    if(!ordinaryRecordOwner.configured)return construct()?1:0;
    if(ordinaryRecordOwner.endingStarted||ordinaryRecordOwner.returned
        ||ordinaryRecordOwner.generation!=worldGeneration||!debugRecordDriver.bound
        ||debugRecordDriver.phase!=5||debugRecordPrefix.failed
        ||!debugPostrunRecord->acknowledged()||!debugRecordPrefix.sessionPresent
        ||ordinaryRecordOwner.sessionRetired||ownedBattle.attempted()||!debugFreshReplay
        ||!std::all_of(postrunAssets.begin(),postrunAssets.end(),[](const auto& bytes){return !bytes.empty();})
        ||!grazeSounds.empty()||soundJournalWindowSerial
        ||debugRecordPrefix.clearRecordEvents!=1||stageControl.current()!=7||stageControl.pending()!=15
        ||debugTerminal.stage!=int(ordinaryRecordOwner.stage)
        ||debugTerminal.index!=int(ordinaryRecordOwner.route)
        ||worldStartDifficulty!=int(ordinaryRecordOwner.rank)){
        operationError="ordinary actual ending constructor ownership rejected";return 0;
    }
    const auto committed=stageControl.constructEnding15([&]{
        retiredWorldSnapshot=th128_world_state();
        const auto configurationMode=debugFreshReplay->bytes()[36+0x18+0x38];
        ordinaryDetachedEnding=std::move(battleEndingRuntime15);
        retainedPostrunReplay=std::shared_ptr<th128::record::NativeFreshReplayRecord>(std::move(debugFreshReplay));
        if(!ownedBattle.retire(operationError))return false;
        debugRecordPrefix.sessionPresent=false;stageSessionPresent=0;
        playerConfigured=playerMotion=playerShotsConfigured=playerShotGroupsConfigured=false;
        messagesConfigured=messageFrontConfigured=routeChoiceConfigured=stageTransitionConfigured=false;
        ordinaryRecordOwner.sessionRetired=true;
        actualPostrunMedia=std::make_unique<th128::postrun::Media>(ordinaryRecordOwner.epoch);
        actualPostrunMedia->music(configurationMode&0x10?4:3,-1,0);
        for(int identifier=0;identifier<76;++identifier)actualPostrunMedia->sound(identifier,0,1);
        return construct();
    });
    if(committed.status!=th128::stage::CommitStatus::Applied){
        if(operationError.empty())operationError="ordinary actual ending controller construction failed";
        debugRecordPrefix.failed=true;return 0;
    }
    ordinaryRecordOwner.endingStarted=true;debugRecordDriver.phase=9;
    ++debugRecordDriver.controllerCalls;return 1;
}
int th128_world_terminal_preupdate(){
    operationError.clear();
    if(worldTickExecuting||!world||world->faulted()||normalTerminal.endingStarted||normalTerminal.sessionRetired
        ||!normalTerminal.requested||normalTerminal.counter!=380||stageControl.current()!=7||stageControl.pending()!=15
        ||stageControl.gateSet()||!normalTerminal.owner||debugRunTainted||replayPlayback.active()
        ||!normalTerminal.owner->matchesContinuation(worldGeneration,unsigned(originalContinueCount),normalRecord,normalReplay)
        ||!grazeSounds.empty()||soundJournalWindowSerial||!musicControl.active||!musicControl.acknowledged
        ||musicControl.serial!=normalTerminal.musicSerial||musicControl.operation!=5){
        operationError="ordinary priority1 requires the completed exact native audio-first World boundary";return 0;
    }
    return normalTerminal.transition.preupdate(worldGeneration,world->frame(),operationError)?1:0;
}
int th128_world_ending_tick(unsigned holding,unsigned pressed,unsigned released){
    operationError.clear();auto* ending=activeEndingRuntime();
    if((!debugTerminal.requested&&!normalTerminal.endingStarted)||!ending||!ending->loaded()||ending->pauseWord()){operationError="actualendingruntimeownerrequired";return 0;}
    if(normalTerminal.endingStarted){
        if(!normalTerminal.transition.tickEnding(worldGeneration,ending->ticks()+1,ordinaryGlobalTaskControls(),operationError))return 0;
        projectOrdinaryGlobalFade();
    }
    return ending->tick({holding,pressed,released},worldGeneration,operationError)?1:0;
}
int th128_world_ending_set_pause(unsigned paused){
    operationError.clear();auto* ending=activeEndingRuntime();
    if(!ending||paused>1||!ending->setPause(paused,worldGeneration,operationError))return 0;
    if(normalTerminal.endingStarted&&paused){
        if(!normalTerminal.transition.retirePausedEnding(worldGeneration,ordinaryGlobalTaskControls(),operationError))return 0;
        projectOrdinaryGlobalFade();
    }
    return 1;
}
int th128_world_ending_return_prepare(){
    operationError.clear();
    if(!normalTerminal.endingStarted||!normalTerminal.sessionRetired||!ordinaryDetachedEnding||!ownedBattle.retired()
        ||worldTickExecuting||debugRunTainted||replayPlayback.active()||!grazeSounds.empty()||soundJournalWindowSerial){
        operationError="ordinary return requires the actual retired Ending owner and drained sound";return 0;}
    return ordinaryDetachedEnding->prepareOrdinaryReturn(stageControl,normalTerminal.transition,operationError)?1:0;
}
unsigned th128_world_ending_return_packet_size(){return ordinaryDetachedEnding?unsigned(ordinaryDetachedEnding->ordinaryReturnPacket().size()):0;}
const void* th128_world_ending_return_packet_data(){return ordinaryDetachedEnding?ordinaryDetachedEnding->ordinaryReturnPacket().data():nullptr;}
int th128_world_ending_return_ack(const void* data,unsigned bytes){
    operationError.clear();if(worldTickExecuting||!ordinaryDetachedEnding||!th128::runtime::inputRange(data,bytes)
        ||!grazeSounds.empty()||soundJournalWindowSerial){operationError="ordinary exact stopped receipt range/lifecycle rejected";return 0;}
    return ordinaryDetachedEnding->acknowledgeOrdinaryStop({static_cast<const std::uint8_t*>(data),bytes},stageControl,normalTerminal.transition,operationError)?1:0;
}
int th128_world_normal_postrun_asset(unsigned generation,int bank,const void* data,unsigned bytes){
    operationError.clear();
    if(worldTickExecuting||generation!=worldGeneration||normalPostrun.generation||normalPostrun.menu
        ||!normalTerminal.endingStarted||!ordinaryDetachedEnding||debugRunTainted||replayPlayback.active()
        ||bank!=0&&bank!=24&&bank!=25&&bank!=11||!bytes||bytes>16u*1024u*1024u||!th128::runtime::inputRange(data,bytes)){
        operationError="ordinary postrun assets require the same actual ending owner";return 0;}
    const unsigned slot=bank==0?0:bank==24?1:bank==25?2:3;if(!normalPostrun.assets[slot].empty()){
        operationError="ordinary postrun asset replacement rejected";return 0;}
    const auto* source=static_cast<const std::uint8_t*>(data);normalPostrun.assets[slot].assign(source,source+bytes);return 1;
}
int th128_world_normal_postrun_begin(unsigned generation){
    operationError.clear();
    if(worldTickExecuting||generation!=worldGeneration||normalPostrun.generation||normalPostrun.menu
        ||!normalTerminal.endingStarted||!normalTerminal.sessionRetired||!ordinaryDetachedEnding||!ownedBattle.retired()
        ||!normalRecord||!normalReplay||!normalTerminal.owner||debugRunTainted||replayPlayback.active()
        ||!normalTerminal.owner->matchesContinuation(generation,unsigned(originalContinueCount),normalRecord,normalReplay)
        ||!grazeSounds.empty()||soundJournalWindowSerial||!std::all_of(normalPostrun.assets.begin(),normalPostrun.assets.end(),[](const auto& bytes){return !bytes.empty();})){
        operationError="ordinary sealed postrun construction lifecycle rejected";return 0;}
    auto permit=ordinaryDetachedEnding->releaseOrdinaryReturn(stageControl,normalTerminal.transition,operationError);if(!permit)return 0;
    const auto identity=permit->identity();const auto record=normalRecord;const auto replay=normalReplay;
    th128::postrun::Context context;context.profile=record->pending();context.normalRecord=record;context.normalReplay=replay;
    context.score=record->terminalScore();context.continues=record->terminalContinues();context.route=identity.route;context.rank=identity.rank;
    context.stage=identity.settledSelector;context.epoch=generation;context.clockSerial=0;
    context.numerator=record->statistics().numerator;context.denominator=record->statistics().denominator;
    try{
        normalPostrun.media=std::make_unique<th128::postrun::Media>(generation);
        context.musicSink=[](int operation,int track,int argument){normalPostrun.media->music(operation,track,argument);};
        context.soundSink=[](int identifier){normalPostrun.media->sound(identifier,0,0);};
        const auto request=stageControl.request(16);if(request.status!=th128::stage::RequestStatus::Written||request.pending!=16)
            throw std::runtime_error("ordinary original request16 rejected");
        const auto committed=stageControl.consumeEnding16([&]{
            endingRuntimeSnapshot=ordinaryDetachedEnding->json();ordinaryDetachedEnding.reset();
            normalTerminal.transition={};normalTerminal.owner.reset();normalReplay.reset();normalRecord.reset();
            stageCarry={};return true;
        },[&](unsigned mode){
            if(mode!=3)throw std::runtime_error("ordinary postrun fresh menu mode must be3");
            normalPostrun.menu=std::make_unique<th128::postrun::Menu>(std::move(context),std::move(permit));
            for(unsigned index=0;index<3;++index)normalPostrun.menu->load(index==0?0:index==1?24:25,normalPostrun.assets[index]);
            normalPostrun.menu->loadNameFont(normalPostrun.assets[3]);
            return true;
        });
        if(committed.status!=th128::stage::CommitStatus::Applied||stageControl.current()!=4||stageControl.pending()!=4)
            throw std::runtime_error("ordinary sealed Ending16 -> fresh4/mode3 commit failed");
        normalPostrun.generation=generation;normalPostrun.profile=normalPostrun.menu->profile();
        if(!world||!ownedBattle.retired())throw std::runtime_error("ordinary Main lost retained retired World RNG provenance");
        normalPostrun.retainedRandom={generation,unsigned(world->frame()),record->frames(),world->gameRandomState(),worldDisplayRandom};
        if(!normalPostrun.receipt.bind(*normalPostrun.menu,operationError)
            ||!normalPostrun.mainOwner.bind(*normalPostrun.menu,normalPostrun.retainedRandom,operationError))throw std::runtime_error(operationError);return 1;
    }catch(const std::exception& failure){normalPostrun.failed=true;operationError=failure.what();return 0;}
}
int th128_world_normal_postrun_input(unsigned generation,unsigned serial,unsigned holding,unsigned pressed,unsigned filetimeLow,unsigned filetimeHigh){
    operationError.clear();
    if(!normalPostrun.menu||!normalPostrun.media||normalPostrun.failed||generation!=worldGeneration||generation!=normalPostrun.generation
        ||serial!=normalPostrun.serial+1||!normalPostrun.media->empty()||stageControl.current()!=4||stageControl.pending()!=4
        ||normalPostrun.awaitingPresent||normalPostrun.receipt.pending()||normalPostrun.mainOwner.pending()||normalPostrun.menu->mainHandoffReady()||normalPostrun.menu->ordinaryReplaySavePending()||normalPostrun.profile!=normalPostrun.menu->profile()){
        operationError="ordinary postrun input requires its completed media/presentation/profile boundary";return 0;}
    try{
        // Name/replay phases must be connected to their complete persistent writer before leaving award phase14.
        if(normalPostrun.menu->needsNormalNameOwner())throw std::runtime_error("ordinary native name font owner is not yet bound");
        if(normalPostrun.menu->needsNormalReplayOwner())throw std::runtime_error("ordinary full variable replay preview/save writer owner is not yet bound");
        const auto repeated=normalPostrun.input.advance(holding)&0xf0u;
        normalPostrun.menu->tick({repeated,pressed},serial,(std::uint64_t(filetimeHigh)<<32)|filetimeLow,generation);
        normalPostrun.serial=serial;normalPostrun.awaitingPresent=true;
        if(normalPostrun.extra&&!normalPostrun.menu->replayPresent()){debugExtraReplay.reset();debugFreshReplay.reset();retainedPostrunReplay.reset();extraReturnOwner={};}
        if(normalPostrun.receipt.prepare(*normalPostrun.menu,operationError)<0)throw std::runtime_error(operationError);return 1;
    }catch(const std::exception& failure){normalPostrun.failed=true;operationError=failure.what();return 0;}
}
unsigned th128_world_normal_postrun_profile_size(){return unsigned(normalPostrun.receipt.packet().size());}
const void* th128_world_normal_postrun_profile_data(){return normalPostrun.receipt.packet().data();}
int th128_world_normal_postrun_profile_ack(unsigned generation,const void* data,unsigned bytes){
    operationError.clear();if(!normalPostrun.menu||normalPostrun.failed||generation!=worldGeneration||generation!=normalPostrun.generation
        ||worldTickExecuting||!th128::runtime::inputRange(data,bytes)){operationError="ordinary menu profile ACK lifecycle rejected";return 0;}
    if(!normalPostrun.receipt.acknowledge(*normalPostrun.menu,{static_cast<const std::uint8_t*>(data),bytes},operationError))return 0;
    normalPostrun.profile=normalPostrun.menu->profile();return 1;
}
int th128_world_normal_postrun_replay_observe(unsigned generation,const void* data,unsigned bytes){
    operationError.clear();if(!normalPostrun.menu||!normalPostrun.media||normalPostrun.failed||generation!=worldGeneration||generation!=normalPostrun.generation||worldTickExecuting
        ||stageControl.current()!=4||stageControl.pending()!=4||normalPostrun.awaitingPresent||!normalPostrun.media->empty()||normalPostrun.receipt.pending()
        ||normalPostrun.profile!=normalPostrun.menu->profile()||bytes!=4236||!th128::runtime::inputRange(data,bytes)){
        operationError="ordinary full25-slot observation boundary rejected";return 0;}
    try{normalPostrun.menu->bindOrdinaryReplayObservations({static_cast<const std::uint8_t*>(data),bytes});return 1;}
    catch(const std::exception& failure){operationError=failure.what();return 0;}
}
unsigned th128_world_normal_postrun_replay_size(){return normalPostrun.menu?unsigned(normalPostrun.menu->ordinaryReplaySavePacket().size()):0;}
const void* th128_world_normal_postrun_replay_data(){return normalPostrun.menu?normalPostrun.menu->ordinaryReplaySavePacket().data():nullptr;}
int th128_world_normal_postrun_replay_ack(unsigned generation,const void* data,unsigned bytes){
    operationError.clear();if(!normalPostrun.menu||!normalPostrun.media||normalPostrun.failed||generation!=worldGeneration||generation!=normalPostrun.generation||worldTickExecuting
        ||stageControl.current()!=4||stageControl.pending()!=4||!normalPostrun.awaitingPresent||!normalPostrun.media->empty()||normalPostrun.receipt.pending()
        ||normalPostrun.profile!=normalPostrun.menu->profile()||bytes>32u*1024u*1024u+128+32+3412||!th128::runtime::inputRange(data,bytes)){
        operationError="ordinary full variable replay ACK requires completed same-owner profile/media receipts";return 0;}
    try{normalPostrun.menu->acknowledgeOrdinaryReplaySave({static_cast<const std::uint8_t*>(data),bytes});return 1;}
    catch(const std::exception& failure){operationError=failure.what();return 0;}
}
const char* th128_world_normal_postrun_media(){
    normalPostrun.text=normalPostrun.media?normalPostrun.media->window():"{}";return normalPostrun.text.c_str();
}
int th128_world_normal_postrun_media_ack(unsigned generation,unsigned serial,unsigned count,unsigned digest){
    operationError.clear();try{
        if(!normalPostrun.media||normalPostrun.failed||generation!=normalPostrun.generation||generation!=worldGeneration)
            throw std::runtime_error("ordinary native menu media owner changed");
        normalPostrun.media->acknowledge(generation,serial,count,digest);return 1;
    }catch(const std::exception& failure){operationError=failure.what();return 0;}
}
const char* th128_world_normal_postrun_present(unsigned generation){
    operationError.clear();normalPostrun.text.clear();
    if(!normalPostrun.menu||normalPostrun.failed||generation!=normalPostrun.generation||generation!=worldGeneration
        ||!normalPostrun.media->empty()||normalPostrun.receipt.pending()||normalPostrun.menu->ordinaryReplaySavePending()||!normalPostrun.awaitingPresent){operationError="ordinary postrun presentation owner rejected";return normalPostrun.text.c_str();}
    std::ostringstream out;out<<"{\"generation\":"<<generation<<",\"serial\":"<<normalPostrun.serial<<",\"draws\":[";bool comma=false;
    if(!normalPostrun.menu->drawNative([&](unsigned id,int bank,const th128::anm::State& state,const th128::anm::SpriteInfo& sprite,float x,float y,float z){
        return writeVisualRecord(out,comma,0x61000000u+id,bank,state,sprite,x,y,z);
    })){operationError="ordinary postrun actual draw owner rejected";normalPostrun.text.clear();return normalPostrun.text.c_str();}
    out<<"],\"menu\":"<<normalPostrun.menu->snapshot()<<'}';
    normalPostrun.text=out.str();normalPostrun.awaitingPresent=false;return normalPostrun.text.c_str();
}
int th128_world_normal_postrun_main_prepare(unsigned generation){
    operationError.clear();if(worldTickExecuting||!world||!ownedBattle.retired()||!normalPostrun.menu||!normalPostrun.media||normalPostrun.failed
        ||generation!=worldGeneration||generation!=normalPostrun.generation||normalPostrun.profile!=normalPostrun.menu->profile()){
        operationError="ordinary Main preparation has no actual retained current4 owner";return 0;}
    const th128::postrun::NativeNormalMainRandom actual{generation,unsigned(world->frame()),normalPostrun.menu->normalFrames(),world->gameRandomState(),worldDisplayRandom};
    return normalPostrun.mainOwner.prepare(*normalPostrun.menu,*normalPostrun.media,normalPostrun.receipt,stageControl,normalPostrun.serial,normalPostrun.awaitingPresent,actual,operationError)?1:0;
}
unsigned th128_world_normal_postrun_main_size(){return unsigned(normalPostrun.mainOwner.packet().size());}
const void* th128_world_normal_postrun_main_data(){return normalPostrun.mainOwner.packet().data();}
int th128_world_normal_postrun_main_ack(unsigned generation,const void* data,unsigned bytes){
    operationError.clear();if(worldTickExecuting||!world||!normalPostrun.menu||!normalPostrun.media||normalPostrun.failed
        ||generation!=worldGeneration||generation!=normalPostrun.generation||!th128::runtime::inputRange(data,bytes)){
        operationError="ordinary Main full receipt ACK lifecycle rejected";return 0;}
    const th128::postrun::NativeNormalMainRandom actual{generation,unsigned(world->frame()),normalPostrun.menu->normalFrames(),world->gameRandomState(),worldDisplayRandom};
    return normalPostrun.mainOwner.acknowledge(*normalPostrun.menu,*normalPostrun.media,normalPostrun.receipt,stageControl,normalPostrun.serial,normalPostrun.awaitingPresent,actual,{static_cast<const std::uint8_t*>(data),bytes},operationError)?1:0;
}
int th128_world_normal_postrun_main_commit(unsigned generation){
    operationError.clear();if(worldTickExecuting||!world||!ownedBattle.retired()||!normalPostrun.menu||!normalPostrun.media||normalPostrun.failed
        ||generation!=worldGeneration||generation!=normalPostrun.generation||normalMainAdoptedGeneration||normalRecord||normalReplay||normalTerminal.owner
        ||ordinaryDetachedEnding||normalTerminal.transition.fade().active||normalPostrun.profile!=normalPostrun.menu->profile()
        ||normalPostrun.extra&&(debugExtraReplay||debugFreshReplay||retainedPostrunReplay)){
        operationError="ordinary Main commit cannot discard any retained Ending/replay owner";return 0;}
    const th128::postrun::NativeNormalMainRandom actual{generation,unsigned(world->frame()),normalPostrun.menu->normalFrames(),world->gameRandomState(),worldDisplayRandom};
    auto permit=normalPostrun.mainOwner.release(*normalPostrun.menu,*normalPostrun.media,normalPostrun.receipt,stageControl,normalPostrun.serial,normalPostrun.awaitingPresent,actual,operationError);
    if(!permit)return 0;if(!th128::postrun::adoptOrdinaryMain(permit,operationError)){normalPostrun.failed=true;return 0;}
    const bool extra=normalPostrun.extra;normalPostrun={};normalTerminal={};stageCarry={};normalReplaySave.reset();normalReplayBytes.clear();normalReplayPreviewRows={};normalReplayOccupied={};
    if(extra){debugExtraReplay.reset();debugFreshReplay.reset();retainedPostrunReplay.reset();extraReturnOwner={};debugRecordPrefix={};debugRecordDriver={};debugPostrunRecord=std::make_shared<th128::record::NativePostrunRecord>();postrunAssets={};extraPostrunNameFont.clear();}
    normalMainAdoptedGeneration=generation;return 1;
}
const char* th128_world_normal_postrun_state(){
    std::ostringstream out;out<<"{\"generation\":"<<normalPostrun.generation<<",\"serial\":"<<normalPostrun.serial
        <<",\"present\":"<<(normalPostrun.menu?"true":"false")<<",\"failed\":"<<(normalPostrun.failed?"true":"false")
        <<",\"current\":"<<stageControl.current()<<",\"pending\":"<<stageControl.pending()<<",\"mode\":"<<stageControl.menuMode()
        <<",\"mainAdoptedGeneration\":"<<normalMainAdoptedGeneration<<",\"normalClear\":false,\"provenance\":\""<<(normalPostrun.extra?"sealed-extra-terminal23-return":"sealed-ordinary-ending16-return")<<"\"";
    out<<",\"extraSealed\":"<<(normalPostrun.extra?"true":"false")<<",\"ownerKind\":\""<<(normalPostrun.extra?"extra":"ordinary")<<"\",\"postrunProvenance\":\""<<(normalPostrun.extra?"sealed-extra-terminal23-return":"sealed-ordinary-ending16-return")<<"\"";
    if(normalPostrun.menu)out<<",\"menu\":"<<normalPostrun.menu->snapshot();out<<'}';normalPostrun.text=out.str();return normalPostrun.text.c_str();
}
const char* th128_world_ending_state(){if(activeEndingRuntime())endingRuntimeSnapshot=activeEndingRuntime()->json();return endingRuntimeSnapshot.c_str();}
int th128_world_ending_ack_media(unsigned serial,int operation){return activeEndingRuntime()&&activeEndingRuntime()->acknowledgeMedia(serial,operation,operationError)?1:0;}
int th128_world_ending_cleanup(){return activeEndingRuntime()&&activeEndingRuntime()->cleanup(operationError)?1:0;}
int th128_world_ending_return_tail(){
    auto* ending=activeEndingRuntime();if(!ending||ending->pauseWord())return 0;
    if(normalTerminal.endingStarted){
        if(!normalTerminal.transition.tickEnding(worldGeneration,ending->ticks()+1,ordinaryGlobalTaskControls(),operationError))return 0;
        projectOrdinaryGlobalFade();
    }
    return ending->tickReturnTail(worldGeneration,operationError)?1:0;
}
}
