#include "CoopPhysics.hpp"
#include "CoopReplaySave.inc"
// Included only by the co-op translation unit after the ordinary native owners are defined.
// P1 is the original character owner. Additional seats have independent original ANM and SHT
// hosts. All seats query the SAME StageWorld enemy; only StageWorld may write its life or kill it.
namespace {
struct CoopSeat {
    th128::player::PlayerHitHost body;
    th128::playershot::PlayerShotHost shots;
    th128::player::RawPosition raw{};
    th128::player::Input input{};
    std::unique_ptr<th128::player::FocusIndicator> focus;
    th128::damagefield::DamageFieldPool fields;
    th128::player::death::Position deathPosition{};
    th128::player::respawn::ControlState respawn{};
    unsigned deaths=0,respawns=0;int grazes=0;
    th128::perfect::Host perfect;
    th128::ice::Host ice;
    th128::player::fire::Host fire;
    int motivation=0,motivationNotice=0,icePower=10000,charge=0;
};
struct CoopIceBatch {unsigned owner=0,pendingStars=0,pendingMotivation=0;bool finished=false,shared=false;};
struct TransferChain{int target=-1;unsigned presses=0,last=0;};
enum class GiftKind {life, skill};
struct LifeGift {unsigned id=0,recipient=0,age=0;th128::player::RawPosition pos{};GiftKind kind=GiftKind::life;};
struct GhostDrift {unsigned age=0;int dx=0,dy=0;};
struct CoopBattle {
    std::array<TransferChain,3> chains{},skillChains{};std::array<GhostDrift,3> ghosts{};std::array<unsigned,3> transfers{},skillTransfers{},rescued{};std::vector<LifeGift> gifts;unsigned giftSerial=0;
    unsigned seats=0, generation=0, commonTarget=0, prev[3]{}, freezeMotivationMultiplierQuarters=4;
    std::array<std::unique_ptr<CoopSeat>,2> extra;
    std::array<unsigned,3> lastTarget{},freezeCount{},lastPixels{},lastChargeReward{},starCredits{};
    std::array<unsigned,3> starResidue{},chargeResidue{};
    // Fatal retirement is a lifecycle latch, not an encoded negative resource.
    std::array<bool,3> retired{};
    std::array<std::shared_ptr<CoopIceBatch>,3> freezing,iceDamageBatches;
    std::array<std::uint32_t,3> iceDamageThisTick{};
    std::unordered_map<unsigned,std::shared_ptr<CoopIceBatch>> drops;
    unsigned expiringSeat=0,collectingItem=0,recipient=0,contactSeat=0,wipeFrames=0;bool challenge=false;
    int perfectCaster=-1;
    std::vector<th128::bullet::SoundRequest> frameSounds;
    std::uint64_t teamPixels=0;
    std::array<std::uint64_t,3> laserColumns{};
    std::array<std::uint64_t,3> damage{};
};
std::unique_ptr<CoopBattle> coop;
th128::ice::Services coopIceServices;
th128::perfect::Services coopPerfectServices;
constexpr unsigned CoopBasicKeys=th128::player::BombKey|th128::player::ShootFreezeKey|th128::player::RapidKey|th128::player::FocusKey|th128::player::LeftKey|
    th128::player::RightKey|th128::player::UpKey|th128::player::DownKey;
bool coopActive(){return coop&&coop->generation==worldGeneration;}
th128::player::Input coopMenuInput(){
    auto keys=playerInput;if(coopActive())for(unsigned i=1;i<coop->seats;i++){const auto in=coop->extra[i-1]->input;
        keys.holding|=in.holding;keys.pressed|=in.pressed;keys.released|=in.released;}
    return keys;
}
bool tickLifeTransfers(std::string&);
th128::player::PlayerHitHost& seatBody(unsigned);
bool coopIsSettling();
bool isGhost(unsigned seat);
bool coopClearDone();
int coopClearStage();
bool coopSound(int id,float x,std::string& error){
    if(grazeSounds.size()>=65536){error="Co-op sound journal is full";return false;}
    grazeSounds.push_back({id,x});return true;
}
constexpr int MotivationCap=100000;
constexpr unsigned FreezeMaskPixels=172032, RewardDenominator=20;
constexpr unsigned coopRateUnits(unsigned seats){return seats==2?16:15;}
constexpr bool sharesFreeze(unsigned pixels){return std::uint64_t(pixels)*20>=FreezeMaskPixels;}
static_assert(!sharesFreeze(8601)&&sharesFreeze(8602)); // exactly 5%, before rewards are scaled
// Quarter units represent the five exact room multipliers without floating-point rounding.
constexpr bool validFreezeMotivationMultiplier(unsigned quarters){return quarters==10||quarters==8||quarters==4||quarters==3||quarters==2;}
constexpr unsigned iceDamageMotivationRaw(int iceDamage,unsigned quarters){
    // Original enemy-manager pass (0x415954..0x415973), then apply the room rule.
    return iceDamage>0?unsigned(std::min(iceDamage,1700)/2)*quarters/4:0;
}
static_assert(iceDamageMotivationRaw(5354,4)==850&&iceDamageMotivationRaw(5354,10)==2125);
static_assert(iceDamageMotivationRaw(1700,3)==637&&iceDamageMotivationRaw(3,10)==2);
float coopArea(std::uint64_t pixels,unsigned seats){return float(double(pixels)*(5*coopRateUnits(seats))/FreezeMaskPixels);}
unsigned scaledReward(unsigned raw,unsigned& residue){const auto amount=raw*coopRateUnits(coop->seats)+residue;residue=amount%RewardDenominator;return amount/RewardDenominator;}
int& seatMotivation(unsigned seat){return seat?coop->extra[seat-1]->motivation:motivationRaw;}
int& seatNotice(unsigned seat){return seat?coop->extra[seat-1]->motivationNotice:motivationFlag;}
int& seatCharge(unsigned seat){return seat?coop->extra[seat-1]->charge:playerWindowControl.perfectCharge4b4d70;}
void retireIfExhausted(unsigned seat){
    if(!coopActive()||coop->challenge||seatMotivation(seat)>=0)return;
    coop->retired[seat]=true;seatMotivation(seat)=0;seatNotice(seat)=0;
    coop->starResidue[seat]=coop->chargeResidue[seat]=0;
}
// Each actual enemy hit advances every eligible seat via the original +2 raw rule, once.
// The per-character Z charge is handled separately by the shooter's impact callback.
bool coopHitMotivation(std::string& error){
    bool anyNotice=false;
    for(unsigned i=0;i<coop->seats;i++){
        auto& raw=seatMotivation(i);auto& flag=seatNotice(i);
        const auto gain=th128::shotenemy::planShotQueryImpact(raw,flag);
        if(!gain){error="Co-op shot impact reward rejected";return false;}
        raw=gain->value;flag=gain->flag;
        if(raw>=MotivationCap)coop->starResidue[i]=0;
        if(gain->soundRequired){anyNotice=true;if(i==0&&!perfectHud->motivationNotice(error))return false;}
    }
    return !anyNotice||coopSound(18,0,error);
}
bool coopMotivationCredit(unsigned seat,unsigned points,std::string& error){
    if(!points||seatNotice(seat))return true;
    auto& raw=seatMotivation(seat);const int after=std::min(MotivationCap,raw+int(points));
    if(raw/10000<after/10000&&(!coopSound(18,0,error)||(!seat&&!perfectHud->motivationNotice(error))))return false;
    raw=after;return true;
}
bool shareIceMotivation(unsigned owner,unsigned amount,std::string& error){
    for(unsigned i=0;i<coop->seats;i++)if(i!=owner&&!coopMotivationCredit(i,amount,error))return false;
    return true;
}
std::shared_ptr<CoopIceBatch>& activeIceBatch(unsigned seat){
    auto& batch=coop->freezing[seat];
    if(!batch)batch=std::make_shared<CoopIceBatch>(CoopIceBatch{seat});
    return batch;
}
bool spawnCoopIceDamage(unsigned seat,th128::damagefield::DamageFieldPool& pool,th128::ice::Position p,float radius,std::string& error){
    // Ice shatter fields last one enemy update. Keep their batch even if the
    // mask finishes earlier that tick. Respawn fields are not created here.
    const auto field=pool.spawn({p.x,p.y,p.z},radius,0,1,70,1,error);
    if(field&&field->created&&coopActive())coop->iceDamageBatches[seat]=activeIceBatch(seat);
    return bool(field);
}
bool finishCoopIceMotivation(std::string& error){
    if(!coopActive())return true;
    for(unsigned seat=0;seat<coop->seats;seat++)if(const auto& batch=coop->iceDamageBatches[seat]){
        const auto damage=std::bit_cast<std::int32_t>(coop->iceDamageThisTick[seat]);
        const auto amount=iceDamageMotivationRaw(damage,coop->freezeMotivationMultiplierQuarters);
        if(!amount)continue;
        if(!coopMotivationCredit(seat,amount,error))return false;
        // The caster always receives the original (scaled) damage award.
        // The final unscaled mask determines eligibility for the same award
        // on other seats, even if shattering damaged enemies a few ticks ago.
        if(!batch->finished)batch->pendingMotivation+=amount;
        else if(batch->shared&&!shareIceMotivation(seat,amount,error))return false;
    }
    return true;
}
// Collection keeps the separate, existing small ice-star bonus.
bool coopStarCredit(unsigned seat,unsigned n,std::string& error){
    if(!n)return true;const int before=seatMotivation(seat);auto& residue=coop->starResidue[seat];
    if(seatNotice(seat)||before>=MotivationCap){residue=0;return true;}
    if(!coopMotivationCredit(seat,scaledReward(n,residue),error))return false;
    const int after=seatMotivation(seat);coop->starCredits[seat]+=after-before;
    if(after>=MotivationCap)residue=0;return true;
}
bool coopCollectStar(float x,std::string& error){
    const auto it=coop->drops.find(coop->collectingItem);if(it==coop->drops.end()){error="Co-op star lost its freeze owner";return false;}
    auto& batch=*it->second;
    if(!coopStarCredit(batch.owner,1,error))return false;
    if(!batch.finished)++batch.pendingStars;
    else if(batch.shared)for(unsigned i=0;i<coop->seats;i++)if(i!=batch.owner&&!coopStarCredit(i,1,error))return false;
    scoreRaw=std::min(scoreRaw+1,999999999);return coopSound(38,x,error);
}
bool settleCoopArea(unsigned seat,th128::ice::Completion c,bool perfect,std::string& error){
    const auto original=th128::freeze::account(c.pixels,scoreRaw);if(!original){error="Invalid shared freeze mask";return false;}
    const auto award=perfect||coop->challenge?std::optional<int>(0):th128::freeze::areaRewardRaw(original->percent,c.power);
    if(!award){error="Invalid original freeze reward";return false;}
    const bool shared=sharesFreeze(c.pixels);
    const auto prev=th128::playershot::selectShotGroup(cumulativeFrozenArea,shotDifficulty);
    coop->teamPixels+=c.pixels;
    cumulativeFrozenArea=coopArea(coop->teamPixels,coop->seats);
    const auto next=th128::playershot::selectShotGroup(cumulativeFrozenArea,shotDifficulty);
    if(!prev||!next){error="Shared freeze shot level unavailable";return false;}
    const auto p=th128::player::toPixels(seat?coop->extra[seat-1]->raw:playerRaw);
    if(*next>*prev&&(!spellVisuals->levelUp(int(*next),{p.horizontal,p.vertical,0},error)||
        !playerShotGroups->updateSelection(cumulativeFrozenArea,shotDifficulty,0,{float(playerRaw.horizontal)/128,float(playerRaw.vertical)/128,0},error)||!coopSound(13,0,error)))return false;
    bool fullStock=false;
    if(*award)for(unsigned i=0;i<coop->seats;i++)if(i==seat||shared){
        auto& charge=seatCharge(i);auto& residue=coop->chargeResidue[i];const int old=charge;
        if(old>=th128::freeze::PerfectFreezeCapRaw){residue=0;continue;}
        charge=th128::freeze::addPerfectFreezeRaw(charge,scaledReward(*award,residue));
        if(charge>=th128::freeze::PerfectFreezeCapRaw)residue=0;
        if(!i&&!perfectHud->charge(charge,error))return false;
        if(old/10000<charge/10000){fullStock=true;if(!i&&!perfectHud->notice(error))return false;}
    }
    if(fullStock&&!coopSound(72,0,error))return false;
    if(auto batch=std::move(coop->freezing[seat])){
        batch->finished=true;batch->shared=shared;
        if(shared){
            if(!shareIceMotivation(seat,batch->pendingMotivation,error))return false;
            for(unsigned i=0;i<coop->seats;i++)if(i!=seat&&!coopStarCredit(i,batch->pendingStars,error))return false;
        }
        batch->pendingMotivation=0;
    }
    coop->freezeCount[seat]++;coop->lastPixels[seat]=c.pixels;coop->lastChargeReward[seat]=*award*coopRateUnits(coop->seats)/RewardDenominator;
    scoreRaw=original->score;freezeBonus=original->bonus;lastFrozenArea=original->percent;
    if(!freezeRewardPanel.complete(original->percent,original->bonus,p.vertical)){error="Co-op freeze reward panel rejected";return false;}return true;
}
bool coopEntryCancel(const th128::player::death::Position&,int,float,std::string&);
bool canRequestPerfect(const th128::player::Input& input){
    constexpr auto key=th128::perfect::XKeyBit;
    if(!coopActive())return (input.holding&key)!=0;
    return coop->perfectCaster<0&&(input.pressed&key);
}
bool tryCoopPerfect(unsigned index,CoopSeat& seat,bool& activated,std::string& error){
    if(coop->challenge)return true;const auto status=seat.perfect.status();if(status.failed){error="Faulted co-op Perfect owner";return false;}
    if(status.active)return true;if(status.charge!=seat.charge&&!seat.perfect.setCharge(seat.charge,error))return false;
    if(!canRequestPerfect(seat.input)||seat.charge<th128::perfect::CostRaw)return true;
    const auto snap=seat.body.snapshot();const bool saving=snap.gate.stateF78==4&&snap.stateTimer.current<16&&snap.heldTimer.current<=0;
    if(snap.gate.stateF78!=1&&!saving)return true;const auto amount=world->laserObjectCount();const auto frozen=seat.ice.statistics();
    if(!amount||frozen.busy||frozen.objects||(*amount&&(saving||stageIdForEcl(worldStartedName)!=16||!laserLines->healthy()||laserLines->selector3Snapshots().size()!=*amount)))return true;
    th128::perfect::Context c;c.perfectCharge=seat.charge;const auto p=th128::player::toPixels(seat.raw);c.x=p.horizontal;c.y=p.vertical;c.radius=playerProfileRadius;
    c.keys=seat.input.holding;c.xPressed=bool(seat.input.pressed&2u);c.systemPresent=true;c.playerState=snap.gate.stateF78;
    c.mode=saving?th128::perfect::Mode::DeathbombWindow:th128::perfect::Mode::Normal;c.lasers=*amount?th128::perfect::LaserContext::Populated:th128::perfect::LaserContext::Empty;c.laserCount=int(*amount);
    std::string why;activated=saving?seat.perfect.activatePracticeDeathbomb(c,why):seat.perfect.activate(c,why);
    if(!activated){if(seat.perfect.status().failed){error=why;return false;}return true;}
    if(saving&&!seat.body.commitPracticeDeathbomb(error))return false;
    coop->perfectCaster=int(index);
    seat.charge=seat.perfect.status().charge;return coopSound(46,p.horizontal,error)&&coopSound(51,p.horizontal,error);
}
unsigned livingSeats(){unsigned live=0;for(unsigned i=0;i<coop->seats;i++)if(!coop->retired[i])++live;return live;}
bool advanceCoopBody(unsigned index,CoopSeat& seat,bool& focused,std::string& error){
    using namespace th128::player;auto& body=seat.body;auto snapshot=body.snapshot();const auto gate=snapshot.gate.stateF78;
    const auto p=toPixels(seat.raw);const death::Position old{p.horizontal,p.vertical,0};
    if(gate==4){
        if(snapshot.stateTimer.current<16)return body.advanceWindowUnit({true,0,seat.charge,0},error);
        const auto spell=world->phaseSpellContext();DeathContext ctx{{seat.motivation,seat.motivationNotice},seat.charge,{deathAwardSelector,0},
            {spell?spell->flags:snapshot.tail.flags7c,spell?spell->age:snapshot.tail.counter28,spell?std::bit_cast<int>(spell->value80):snapshot.tail.value80,itemScalar},
            {snapshot.tailControl.present,snapshot.tailControl.mode3c},{}};
        DeathOperation op(ctx,4,snapshot.stateTimer,snapshot.protection);
        if(!op.run([&](const death::CallRequest& req,std::string& why){const auto& result=op.progress();
            switch(req.kind){case death::CallKind::PlayerAnimation:return body.commitDeathState(result.state,result.stateTimer,result.protection,why);
            case death::CallKind::Sound:return coopSound(int(req.operands[0]),p.horizontal,why);
            case death::CallKind::ChargeUi425820:case death::CallKind::ChargeNotify424b80:return true; // per-seat presentation reads the authoritative value
            default:why="Unsupported co-op death callback";return false;}},error,coop->challenge))return false;
        const auto& result=op.progress();seat.motivation=result.context.motivation.value4b4d64;seat.motivationNotice=result.context.motivation.flag4b4d68;retireIfExhausted(index);
        seat.charge=coop->challenge?0:result.context.perfectCharge;if(result.powerRaw)seat.icePower=*result.powerRaw;
        if(spell&&!world->applySpellTail(result.context.tail.flags7c,std::uint32_t(result.context.tail.value80),error))return false;
        itemScalar=result.context.tail.scalar4b4d30;++seat.deaths;return body.advancePostDeathUnit(error);
    }
    if(gate==2){
        if(snapshot.stateTimer.current<30)return body.advancePostDeathUnit(error);
        if(coop->retired[index])return true; // team, not this character, decides game-over
        const auto born=body.beginRespawn(old,seat.motivation,seat.respawn.mode04,[&](const death::CallRequest& req,std::string& why){
            if(req.kind==death::CallKind::RespawnField43df90)return bool(seat.fields.spawn({old.horizontal,old.vertical,old.depth},32,16,30,150,0,why));
            if(req.kind==death::CallKind::PostRespawn427190){const auto update=respawn::postFieldControl(seat.respawn);if(update&&!update->requiredSlot){seat.respawn=update->next;return true;}}
            why="Unsupported co-op respawn callback";return false;},error);
        if(!born)return false;seat.deathPosition=born->savedPosition;seat.raw={born->rawHorizontal,born->rawVertical};++seat.respawns;
        return body.stepProtection(1,error)&&body.advanceAnimationUnit(error)&&body.advanceEntryTimersUnit(error);
    }
    if(gate==0){const auto entry=body.entryUnit([&](int t,float r,std::string& why){return coopEntryCancel(seat.deathPosition,t,r,why);},error);
        if(!entry)return false;seat.raw=*entry;if(body.snapshot().gate.stateF78!=1)return body.stepProtection(1,error)&&body.advanceAnimationUnit(error)&&body.advanceEntryTimersUnit(error);}
    if(!body.stepProtection(1,error))return false;
    const auto move=body.moveUnit(seat.raw,seat.input,{true,true,body.focusCounter(),body.snapshot().heldTimer.current},error);
    if(!move||!body.advanceNormalTimersUnit(error))return false;seat.raw=move->position;focused=move->focused;return true;
}
bool advanceCoopSeats(std::string& error){
    if(!coopActive())return true;
    // P1's native action has already run. Guests try in ascending seat order.
    if(coop->perfectCaster<0&&perfectHost->status().active)coop->perfectCaster=0;
    for(unsigned i=1;i<coop->seats;i++){
        auto& seat=*coop->extra[i-1];
        bool activated=false,focused=false;
        if(seat.body.snapshot().gate.stateF78==4&&!tryCoopPerfect(i,seat,activated,error))return false;
        if(!advanceCoopBody(i,seat,focused,error)||!seat.fields.tickUnit(error)||(!activated&&!tryCoopPerfect(i,seat,activated,error)))return false;
        if(!activated&&!seat.perfect.tick(false,error))return false;
        const auto spell=seat.perfect.status();const int mode=spell.active&&spell.phase<th128::perfect::NativeLaserPhase?1:0;
        const auto pos=th128::player::toPixels(seat.raw);
        if(seat.focus&&!seat.focus->update(focused&&seat.body.snapshot().gate.stateF78==1,pos.horizontal,pos.vertical,0,error))return false;
        const auto sound=[&](int op,int id,std::string& why){if(grazeSounds.size()>=65536){why="Co-op sound journal full";return false;}grazeSounds.push_back({id,pos.horizontal,op});return true;};
        if(messagesConfigured&&messages->state().active){if(!seat.fire.suspend(sound,error))return false;}
        else if(!seat.fire.heldTimer(seat.body.snapshot().heldTimer,error)||!seat.fire.tick(seat.body.snapshot().gate.stateF78,seat.input,seat.ice.statistics().busy,mode,seat.icePower,
            [&](int count,std::string& why){return seat.shots.emitCycle(cumulativeFrozenArea,shotDifficulty,count,{pos.horizontal,pos.vertical,.1f},why);},sound,
            [&](int power,std::string& why){return seat.ice.activate(power,{pos.horizontal,pos.vertical,0},why);},error))return false;
        if(!seat.body.setHeldTimer(seat.fire.state().held,error)||!seat.ice.charge(seat.fire.state().held.current,{pos.horizontal,pos.vertical,0},error))return false;
        coop->expiringSeat=i;
        // Refresh the *one* shared bullet inventory after every seat's conversions.
        const bool iced=seat.ice.tick(world->bullets().snapshots(),error);coop->expiringSeat=0;if(!iced)return false;
        if(!seat.shots.tickUnit(error))return false;
    }
    int spellMode=playerWindowControl.mode3c;for(unsigned i=1;i<coop->seats;i++){const auto st=coop->extra[i-1]->perfect.status();if(st.active&&st.phase<th128::perfect::NativeLaserPhase)spellMode=1;}
    world->setPhaseSpellPlayerMode(spellMode);return true;
}
bool finishCoopAnimations(std::string& error){
    if(coopActive())for(unsigned i=1;i<coop->seats;i++){
        auto& seat=*coop->extra[i-1];
        if(!seat.shots.tickAnimationsUnit(error)||!seat.ice.tickAnimations(error)||!seat.body.tickLaserEffects(error)||
           (seat.focus&&!seat.focus->tick(error)))return false;
    }
    return true;
}
bool protectCoopTeam(std::string& error){
    if(!coopActive()||coop->perfectCaster<0)return true;
    const auto caster=coop->perfectCaster;
    const auto skill=(caster?coop->extra[caster-1]->perfect:*perfectHost).status();
    if(!skill.active){coop->perfectCaster=-1;return true;}
    const int remaining=th128::perfect::ProtectionTicks-skill.phase;
    // Contacts run after every seat has advanced. Refresh protection before
    // the shared bullet, laser and enemy managers can affect any teammate.
    for(unsigned i=0;i<coop->seats;i++){
        auto& body=seatBody(i);
        if(body.snapshot().gate.stateF78==1&&!body.extendCoopPerfectProtection(remaining,error))return false;
    }
    return true;
}
void accountCoopIceDamage(unsigned seat,int playerState,const th128::EnemySnapshot& enemy,const th128::shotenemy::ShotEnemyQueryPlan& query){
    if(!coop->iceDamageBatches[seat]||!(query.outFlag&2u))return;
    // Read the original ice-tagged manager increment before mixing seats or
    // applying co-op boss HP scaling. No enemy life or score is written here.
    const auto original=th128::shotenemy::planEnemyDamageAfterCompletedQuery(query,
        {playerState,shotDamageBypass,enemy.flags,enemy.protection,enemy.lifeWords,{},std::nullopt});
    coop->iceDamageThisTick[seat]+=original.nextManagerStats[0];
}
// Return a single completed query. Player damage fields belong to the shared stage and are
// queried by the original host exactly once, never re-applied per participating seat.
bool collectCoopDamage(const th128::EnemySnapshot& enemy,th128::enemy::Bounds bounds,th128::shotenemy::ShotEnemyQueryPlan& query,std::string& error){
    if(!coopActive())return true;
    const auto record=[&](unsigned i,int amount){if(amount<=0)return;coop->damage[i]+=unsigned(amount);coop->lastTarget[i]=enemy.id;
        for(unsigned j=0;j<coop->seats;j++)if(j!=i&&coop->lastTarget[j]==enemy.id)coop->commonTarget=enemy.id;};
    const auto scale=[](auto state,int damage){return state==0||state==2?damage/5:damage;};
    const auto hostState=playerHit->snapshot().gate.stateF78;
    accountCoopIceDamage(0,hostState,enemy,query);
    record(0,query.completedDamage);query.completedDamage=scale(hostState,query.completedDamage);
    for(unsigned i=1;i<coop->seats;i++){
        auto& seat=*coop->extra[i-1];const auto timer=seat.body.snapshot().stateTimer;
        const auto hit=seat.shots.queryEnemy({{enemy.x,enemy.y,enemy.z},{bounds.width,bounds.height}},timer.previous,timer.current,playerDamageSourceFlags,error);
        if(!hit||hit->continuation!=th128::shotenemy::QueryContinuation::Complete)return false;
        const auto playerState=seat.body.snapshot().gate.stateF78;
        accountCoopIceDamage(i,playerState,enemy,*hit);
        record(i,hit->completedDamage);query.completedDamage+=scale(playerState,hit->completedDamage);query.outFlag|=hit->outFlag;
    }
    const auto live=livingSeats();const auto& bosses=world->bossSlots().owners;
    if(std::find(bosses.begin(),bosses.end(),enemy.id)!=bosses.end())query.completedDamage=live==2?query.completedDamage*3/4:live==3?query.completedDamage*2/3:query.completedDamage;
    return true;
}
template<class Writer> bool drawFocusIndicators(Writer& out,bool& comma){
    if(!focusIndicatorConfigured)return true;
    const bool active=coopActive();
    constexpr unsigned focusStart=0x3c000000u,seatStride=0x01000000u;
    for(unsigned i=0;i<(active?coop->seats:1u);i++){
        if(active&&isGhost(i))continue;
        const auto* owner=i?coop->extra[i-1]->focus.get():focusIndicator.get();
        if(!owner)continue;
        for(const auto& focus:owner->draws()){
            if(!focus.state.visible||!(focus.state.flags&2u)||focus.state.deleteRequested)continue;
            // Reserved seat ranges shared with coop-hud.mjs. IDs survive
            // redraws and rollback even when all ships share a position.
            if(active&&focus.id>=seatStride){presentationError="Co-op focus draw identity exhausted";return false;}
            const unsigned id=active?focusStart+i*seatStride+focus.id:14000000u+focus.id;
            if(!writeVisualRecord(out,comma,id,0,focus.state,focus.sprite,focus.x,focus.y,focus.z))return false;
        }
    }
    return true;
}
template<class Writer> bool drawCoopSeats(Writer& out,bool& comma){
    if(!coopActive())return true;
    for(unsigned i=1;i<coop->seats;i++){
        const auto& seat=*coop->extra[i-1];
        const auto laserEffects=seat.body.laserEffectSnapshots(presentationError);if(!presentationError.empty())return false;
        for(const auto& e:laserEffects)if(e.state.visible&&(e.state.flags&2u)&&!e.state.deleteRequested)if(!writeVisualRecord(out,comma,0x34000000u+i*0x01000000u+e.id,7,e.state,e.sprite,e.x,e.y,e.z,true))return false;
        for(const auto& e:seat.perfect.effectSnapshots())if(e.sprite&&e.state.visible&&(e.state.flags&2u)&&!e.state.deleteRequested)
            if(!writeVisualRecord(out,comma,0x2c000000u+i*0x01000000u+e.id,e.bank,e.state,*e.sprite,e.x,e.y,e.z,true))return false;
        for(const auto& ice:seat.ice.draws())if(ice.state.visible&&(ice.state.flags&2u)&&!ice.state.deleteRequested&&!writeVisualRecord(out,comma,0x24000000u+i*0x01000000u+ice.id,ice.bank,ice.state,ice.sprite,ice.x,ice.y,ice.z,true))return false;
        for(const auto& shot:seat.shots.snapshots()){
            const auto& v=shot.state;if(!v.visible||!(v.flags&2u)||v.deleteRequested)continue;
            // Distinct from native shot, laser, enemy and per-seat player identities.
            if(!writeVisualRecord(out,comma,0x20000000u+i*0x01000000u+shot.id,7,v,shot.sprite,
                v.engineX+v.x+v.offsetX,v.engineY+v.y+v.offsetY,v.engineZ+v.z+v.offsetZ))return false;
        }
    }
    return true;
}
}
extern "C" int th128_world_tick();
extern "C" int th128_coop_bind(unsigned count,const void* shot,unsigned shotSize,const void* art,unsigned artSize,const void* iceArt,unsigned iceSize,const void* masks,unsigned maskSize,unsigned challenge,unsigned freezeMotivationMultiplierQuarters){
    operationError.clear();
    if(!validFreezeMotivationMultiplier(freezeMotivationMultiplierQuarters)){operationError="Invalid co-op freeze motivation rule";return 0;}
    if(challenge>1||!world||coopActive()||world->frame()!=0||world->faulted()||world->thpracChapter()||normalRecord||debugRunTainted||
       !playerShotsConfigured||!shotHitsConfigured||!playerMotion||!playerFireConfigured||count<2||count>3||
       !th128::runtime::inputRange(iceArt,iceSize)||iceSize>8*1024*1024||maskSize!=1228||!th128::runtime::inputRange(masks,maskSize)||!th128::runtime::inputRange(shot,shotSize)||!th128::runtime::inputRange(art,artSize)||artSize>8*1024*1024||shotSize>4*1024*1024){operationError="Co-op bind requires a fresh unassisted stage and two or three seats";return 0;}
    auto owner=std::make_unique<CoopBattle>();owner->seats=count;owner->generation=worldGeneration;owner->challenge=challenge!=0;owner->freezeMotivationMultiplierQuarters=freezeMotivationMultiplierQuarters;
    const auto spawnX=[&](unsigned i){return (2*int(i)-int(count-1))*16*th128::player::RawUnitsPerPixel;};
    const auto start=playerHit->snapshot();
    for(unsigned i=1;i<count;i++){
        owner->extra[i-1]=std::make_unique<CoopSeat>();auto& s=*owner->extra[i-1];s.raw={spawnX(i),playerRaw.vertical};s.motivation=motivationRaw;
        s.respawn=respawnControl;
        if(focusIndicatorConfigured){
            s.focus=std::make_unique<th128::player::FocusIndicator>();
            if(!s.focus->configure(bulletVisuals->registry(),operationError))return 0;
        }
        if(!s.fields.configure(std::vector<th128::damagefield::Image>(th128::damagefield::Capacity),0,operationError))return 0;
        if(!s.body.configure(art,artSize,start,[](int script,th128::player::hit::Position p,int layer,std::string& error){return bool(cancellationEffects->spawn(script,p.horizontal,p.vertical,p.depth,layer,error));},
                [](int id,std::string& err){return coopSound(id,0,err);},operationError)||
           !s.shots.load({static_cast<const std::uint8_t*>(shot),shotSize},{static_cast<const std::uint8_t*>(art),artSize},operationError)||
           !s.shots.setCallbackBindings({0,0,0,0})||!s.shots.setSoundHost(coopSound)||!s.shots.enableContactAnimations())return 0;
        if(!s.body.configureLaserEffects([]{return world->effectGameRandom32();},operationError))return 0;
        if(!s.body.setSoundOperationHost([&s](int op,int id,std::string& err){if(grazeSounds.size()>=65536){err="Co-op charge sound budget";return false;}grazeSounds.push_back({id,th128::player::toPixels(s.raw).horizontal,op});return true;},operationError))return 0;
        s.shots.setDamageSourceHost([&s](const th128::shotenemy::EnemyContactShape& e,unsigned flag,std::string& err){return s.fields.query({e.combinedPosition[0],e.combinedPosition[1],e.combinedPosition[2]},flag,err);});
        s.shots.setScoreHost([](int damage,std::string& err){const auto scored=th128::damagefield::finishDamageQuery(0,damage,scoreRaw);if(!scored){err="Invalid co-op damage score";return false;}scoreRaw=scored->score;return true;});
        auto services=coopIceServices;services.damage=[i,&s](th128::ice::Position pos,float radius,std::string& err){return spawnCoopIceDamage(i,s.fields,pos,radius,err);};
        services.complete=[i](auto c,std::string& err){return settleCoopArea(i,c,false,err);};
        services.perfectComplete=[i](auto c,std::string& err){return settleCoopArea(i,c,true,err);};
        if(!s.ice.configure({static_cast<const std::uint8_t*>(iceArt),iceSize},{static_cast<const std::uint8_t*>(masks),maskSize},*bulletVisuals,std::move(services),operationError)||
           !s.fire.configure(-1,start.heldTimer,operationError))return 0;
        auto perfect=coopPerfectServices;
        perfect.protect=[&s](int ticks,std::string& e){return s.body.extendCoopPerfectProtection(ticks,e);};
        perfect.freezeBullets=[&s](const th128::perfect::BulletRequest&,th128::perfect::BulletResult& out,std::string& e){th128::ice::PerfectFreezeResult result;
            if(!s.ice.freezePerfect(world->bullets().snapshots(),result,e)||!result.completed)return false;
            out={result.frozen,0,result.skipped,true};if(!coopSound(47,th128::player::toPixels(s.raw).horizontal,e)||!coopSound(69,0,e))return false;
            if(grazeSounds.size()>=65536){e="Perfect co-op sound journal full";return false;}grazeSounds.push_back({51,0,1});return true;};
        perfect.shatterIce=[&s](std::string& e){return s.ice.shatterPerfect(e);};
        th128::perfect::Context ctx;ctx.perfectCharge=s.charge;ctx.radius=playerProfileRadius;ctx.x=float(s.raw.horizontal)/128;ctx.y=float(s.raw.vertical)/128;
        ctx.systemPresent=true;ctx.playerState=1;ctx.mode=th128::perfect::Mode::Normal;ctx.lasers=th128::perfect::LaserContext::Empty;
        if(!s.perfect.loadNativeEffect({static_cast<const std::uint8_t*>(iceArt),iceSize},operationError)||!s.perfect.configure(ctx,std::move(perfect),operationError))return 0;
        s.shots.setImpactHost([&s](std::string& error){
            if(!coopHitMotivation(error))return false;
            s.icePower=th128::freeze::addPowerRaw(s.icePower,100);return true;});
    }
    const auto px=th128::player::toPixels({spawnX(0),playerRaw.vertical});
    if(!world->setPlayerAimPosition({px.horizontal,px.vertical,0},operationError))return 0;
    playerRaw.horizontal=spawnX(0);coop=std::move(owner);return 1;
}
extern "C" int th128_coop_tick(unsigned frame,unsigned count,unsigned p0,unsigned p1,unsigned p2){
    operationError.clear();const std::array<unsigned,3> keys{p0,p1,p2};
    if(!coopActive()||frame!=world->frame()||count!=coop->seats||(count==2&&p2)||((p0|p1|p2)&~CoopBasicKeys)||world->faulted()||worldTickExecuting){operationError="Invalid co-op frame, seats or input mask";return 0;}
    // Validate all seats before binding any edges. A missed, duplicate or malformed row cannot partially control P1.
    constexpr unsigned castKey=th128::perfect::XKeyBit;
    const unsigned mask=stageTransitionConfigured&&stageTransition->state().requested?CoopBasicKeys&~515u:
        coop->challenge?CoopBasicKeys&~castKey:CoopBasicKeys;
    for(unsigned i=0;i<count;i++){
        const auto held=keys[i]&mask,prev=coop->prev[i];
        auto& dest=i?coop->extra[i-1]->input:playerInput;
        dest={std::uint16_t(held),std::uint16_t(held&~prev),std::uint16_t(prev&~held&(mask|~castKey))};
        // Retain the physical X hold through a stage transition so a blocked
        // press cannot turn into a new cast when the next stage accepts input.
        coop->prev[i]=held|(keys[i]&castKey);
    }
    if(!tickLifeTransfers(operationError))return 0;
    coop->iceDamageBatches.fill(nullptr);coop->iceDamageThisTick.fill(0);
    coop->frameSounds.clear();const int result=th128_world_tick();if(result==1)coop->frameSounds.swap(grazeSounds);if(result==1&&coop->challenge)playerWindowControl.perfectCharge4b4d70=0;
    if(result==1)for(unsigned i=0;i<coop->seats;i++){
        const auto motivation=seatMotivation(i);
        if(motivation>=MotivationCap)coop->starResidue[i]=0;
        if(seatCharge(i)>=th128::freeze::PerfectFreezeCapRaw)coop->chargeResidue[i]=0;
    }
    if(result==1){coop->wipeFrames=livingSeats()?0:coop->wipeFrames+1;if(coopClearDone())return 3;if(coop->wipeFrames==180)return 2;}return result;
}
static_assert(sizeof(th128::bullet::SoundRequest)==12&&offsetof(th128::bullet::SoundRequest,positionX)==4&&offsetof(th128::bullet::SoundRequest,operation)==8);
extern "C" unsigned th128_coop_sound_count(){return coopActive()?unsigned(coop->frameSounds.size()):0;}
extern "C" const void* th128_coop_sound_data(){return coopActive()?coop->frameSounds.data():nullptr;}
extern "C" const char* th128_coop_state(){
    static std::string value;if(!coopActive())return "null";
    std::ostringstream s;s<<"{\"frame\":"<<world->frame()<<",\"challengeMode\":"<<(coop->challenge?"true":"false")<<",\"freezeMotivationMultiplier\":"<<double(coop->freezeMotivationMultiplierQuarters)/4<<",\"wipeFrames\":"<<coop->wipeFrames<<",\"lifeGifts\":"<<coop->gifts.size()<<",\"clearStage\":"<<coopClearStage()<<",\"ecl\":\""<<worldStartedName<<"\",\"rank\":"<<worldStartDifficulty<<",\"spawns\":"<<world->spawnCount()<<",\"rngCalls\":"<<world->randomCalls()<<",\"commonTarget\":"<<coop->commonTarget<<",\"teamPixels\":"<<coop->teamPixels<<",\"firepower\":"<<std::setprecision(9)<<cumulativeFrozenArea<<",\"seats\":[";
    for(unsigned i=0;i<coop->seats;i++){const auto raw=i?coop->extra[i-1]->raw:playerRaw;if(i)s<<',';
        s<<"{\"seat\":"<<i<<",\"rawX\":"<<raw.horizontal<<",\"rawY\":"<<raw.vertical<<",\"damage\":"<<coop->damage[i]<<",\"laserColumns\":"<<coop->laserColumns[i]<<",\"lastTarget\":"<<coop->lastTarget[i]
         <<",\"freezeCount\":"<<coop->freezeCount[i]<<",\"lastPixels\":"<<coop->lastPixels[i]<<",\"lastChargeReward\":"<<coop->lastChargeReward[i]
         <<",\"charge\":"<<(i?coop->extra[i-1]->charge:playerWindowControl.perfectCharge4b4d70)<<",\"icePower\":"<<(i?coop->extra[i-1]->icePower:grazeGlobals.powerRaw)<<",\"iceObjects\":"<<(i?coop->extra[i-1]->ice: *iceHost).statistics().objects
         <<",\"retired\":"<<(coop->retired[i]?"true":"false")<<",\"ghost\":"<<(isGhost(i)?"true":"false")<<",\"motivation\":"<<(i?coop->extra[i-1]->motivation:motivationRaw)<<",\"starCredits\":"<<coop->starCredits[i]
         <<",\"state\":"<<(i?coop->extra[i-1]->body:*playerHit).snapshot().gate.stateF78<<",\"deaths\":"<<(i?coop->extra[i-1]->deaths:deathsCommitted)<<",\"respawns\":"<<(i?coop->extra[i-1]->respawns:respawnsCommitted)
         <<",\"perfectActive\":"<<((i?coop->extra[i-1]->perfect:*perfectHost).status().active?"true":"false")<<",\"perfectPhase\":"<<(i?coop->extra[i-1]->perfect:*perfectHost).status().phase
         <<",\"protection\":"<<(i?coop->extra[i-1]->body:*playerHit).snapshot().protection.current<<",\"transferTaps\":"<<coop->chains[i].presses<<",\"skillTaps\":"<<coop->skillChains[i].presses<<",\"skillTransfers\":"<<coop->skillTransfers[i]<<",\"transfers\":"<<coop->transfers[i]<<",\"rescued\":"<<coop->rescued[i]<<",\"lifeDisplay\":"<<(coop->challenge?int(i?coop->extra[i-1]->deaths:deathsCommitted):(i?coop->extra[i-1]->motivation:motivationRaw))<<'}'  ;}
    s<<"]}";value=s.str();return value.c_str();
}

namespace th128::coopitems {
void born(unsigned id){if(!coopActive())return;const auto i=coop->expiringSeat;coop->drops.emplace(id,activeIceBatch(i));}
void removed(unsigned id){if(coopActive())coop->drops.erase(id);}
void target(unsigned id,item::Position at,item::Position& pos,int& state,bool& focus){
    if(!coopActive())return;
    double best=std::numeric_limits<double>::infinity();coop->collectingItem=id;
    for(unsigned i=0;i<coop->seats;i++){
        const auto& body=i?coop->extra[i-1]->body:*playerHit;const auto gate=body.snapshot().gate.stateF78;if(gate!=1)continue;
        const auto raw=player::toPixels(i?coop->extra[i-1]->raw:playerRaw);
        const double dx=at.x-raw.horizontal,dy=at.y-raw.vertical,dist=dx*dx+dy*dy;
        if(dist<best){best=dist;coop->recipient=i;pos={raw.horizontal,raw.vertical,0};state=gate;focus=(coop->prev[i]&player::FocusKey)!=0;}
    }
}
}

namespace th128::coopphysics {
bullet::Position aim(bullet::Position origin,bullet::Position fallback){
    if(!coopActive())return fallback;
    // During a wipe or a death animation there may be no eligible character.
    // Do not steer original enemy scripts at the drifting P1 ghost in that case.
    auto target=bullet::Position{0,400,0};double nearest=std::numeric_limits<double>::infinity();
    for(unsigned i=0;i<coop->seats;i++){const auto& body=i?coop->extra[i-1]->body:*playerHit;if(body.snapshot().gate.stateF78!=1)continue;
        const auto p=player::toPixels(i?coop->extra[i-1]->raw:playerRaw);const double dx=p.horizontal-origin.x,dy=p.vertical-origin.y,d=dx*dx+dy*dy;
        if(d<nearest){nearest=d;target={p.horizontal,p.vertical,0};}}return target;
}
void contact(bullet::collision::Vec2 center,float bulletRadius,float playerRadius,std::optional<bullet::collision::Geometry>& geometry){
    namespace col=bullet::collision;if(!coopActive())return;
    int priority=0;double nearest=std::numeric_limits<double>::infinity();geometry=col::Geometry::Miss;coop->contactSeat=0;
    for(unsigned i=0;i<coop->seats;i++){const auto& body=i?coop->extra[i-1]->body:*playerHit;const auto gate=body.snapshot().gate;
        if(gate.stateF78!=1)continue;const auto p=player::toPixels(i?coop->extra[i-1]->raw:playerRaw);
        const auto hit=col::circleGeometry(center,bulletRadius,{p.horizontal,p.vertical},playerRadius);if(!hit){geometry.reset();return;}
        const int rank=*hit==col::Geometry::PotentialHit?(gate.counter438dc>0?2:3):*hit==col::Geometry::PotentialGraze?1:0;
        const double dx=p.horizontal-center.horizontal,dy=p.vertical-center.vertical,d=dx*dx+dy*dy;
        if(rank>priority||(rank==priority&&d<nearest)){priority=rank;nearest=d;coop->contactSeat=i;geometry=hit;}
    }
}
}

// An original transition token binds P1, the stage, RNGs and score. The co-op companion
// binds the SAME token to the other seats and integer team area in this Wasm instance.
extern "C" unsigned th128_world_capture_stage_carry();
extern "C" int th128_world_acknowledge_stage_transition(unsigned);
namespace {
struct CoopCampaignCarry {
    struct Resources {int motivation=0,notice=0,power=0,charge=0,grazes=0;unsigned deaths=0,respawns=0,held=0;};
    unsigned token=0,seats=0,sourceGeneration=0,freezeMotivationMultiplierQuarters=4;int target=0,progress=0;bool challenge=false;
    std::array<Resources,3> actor{};
    std::uint64_t teamPixels=0;std::array<std::uint64_t,3> damage{};
    std::array<unsigned,3> count{},pixels{},reward{},credits{},transfers{},skillTransfers{},rescued{},giftsOwed{},starResidue{},chargeResidue{};std::array<std::uint64_t,3> columns{};
};
std::optional<CoopCampaignCarry> coopCarry;
}
extern "C" int th128_coop_next_stage(){
    if(!coopActive()||!stageTransitionConfigured||world->faulted()||!stageTransition->state().requested)return 0;
    // Let an in-flight mask/Perfect event settle in the source heap before retiring its bullets.
    if(iceHost->statistics().busy||perfectHost->status().active)return 0;
    for(unsigned i=1;i<coop->seats;i++)if(coop->extra[i-1]->ice.statistics().busy||coop->extra[i-1]->perfect.status().active)return 0;
    return stageTransition->state().nextId;
}
extern "C" unsigned th128_coop_capture_stage(int target){
    if(target<=0||target!=th128_coop_next_stage()||worldTickExecuting||coopCarry){operationError="Co-op carry requires its own requested stage";return 0;}
    const unsigned token=th128_world_capture_stage_carry();if(!token)return 0;
    CoopCampaignCarry c;c.token=token;c.seats=coop->seats;c.challenge=coop->challenge;c.freezeMotivationMultiplierQuarters=coop->freezeMotivationMultiplierQuarters;c.sourceGeneration=worldGeneration;c.target=target;c.progress=stageTransition->state().nextProgress;
    c.teamPixels=coop->teamPixels;c.damage=coop->damage;c.count=coop->freezeCount;c.pixels=coop->lastPixels;c.reward=coop->lastChargeReward;c.credits=coop->starCredits;
    c.actor[0]={motivationRaw,motivationFlag,grazeGlobals.powerRaw,playerWindowControl.perfectCharge4b4d70,grazeGlobals.grazeCount,deathsCommitted,respawnsCommitted,coop->prev[0]};
    for(unsigned i=1;i<c.seats;i++){const auto& a=*coop->extra[i-1];c.actor[i]={a.motivation,a.motivationNotice,a.icePower,a.charge,a.grazes,a.deaths,a.respawns,coop->prev[i]};}
    c.transfers=coop->transfers;c.skillTransfers=coop->skillTransfers;c.rescued=coop->rescued;c.columns=coop->laserColumns;
    c.starResidue=coop->starResidue;c.chargeResidue=coop->chargeResidue;
    for(const auto& gift:coop->gifts)if(gift.kind==GiftKind::life)++c.giftsOwed[gift.recipient];
    coopCarry=c;return token;
}
extern "C" int th128_coop_next_progress(unsigned token){return coopCarry&&coopCarry->token==token?coopCarry->progress:-1;}
extern "C" int th128_coop_finish_stage(unsigned token){
    operationError.clear();if(!coopActive()||!coopCarry||!token||coopCarry->token!=token||world->frame()!=0||worldTickExecuting||
        coopCarry->seats!=coop->seats||coopCarry->challenge!=coop->challenge||coopCarry->freezeMotivationMultiplierQuarters!=coop->freezeMotivationMultiplierQuarters||coopCarry->sourceGeneration!=stageCarry.sourceGeneration||
        coopCarry->sourceGeneration==worldGeneration||coopCarry->target!=stageIdForEcl(worldStartedName)||coopCarry->progress!=stageCarry.nextProgress||!stageCarry.scalarRestored){operationError="Co-op carry target does not match its original stage transition";return 0;}
    const auto& c=*coopCarry;const auto& p1=c.actor[0];const auto area=coopArea(c.teamPixels,c.seats);
    if(area!=cumulativeFrozenArea||p1.motivation!=motivationRaw||p1.notice!=motivationFlag||p1.power!=grazeGlobals.powerRaw||
       p1.charge!=playerWindowControl.perfectCharge4b4d70||p1.grazes!=grazeGlobals.grazeCount||normalRecord||normalReplay){operationError="Original carry changed team area or personal resources";return 0;}
    if(th128_world_acknowledge_stage_transition(token)!=1)return 0;
    coop->teamPixels=c.teamPixels;coop->damage=c.damage;coop->freezeCount=c.count;coop->lastPixels=c.pixels;coop->lastChargeReward=c.reward;coop->starCredits=c.credits;coop->prev[0]=p1.held;
    deathsCommitted=p1.deaths;respawnsCommitted=p1.respawns;
    // The fresh World clears the retirement latch. Its normal respawn animation
    // returns every seat with exactly the resources earned before handoff.
    for(unsigned i=1;i<c.seats;i++){auto& a=*coop->extra[i-1];const auto& saved=c.actor[i];a.motivation=saved.motivation;a.motivationNotice=saved.notice;
        a.icePower=saved.power;a.charge=saved.charge;a.grazes=saved.grazes;a.deaths=saved.deaths;a.respawns=saved.respawns;coop->prev[i]=saved.held;}
    coop->transfers=c.transfers;coop->skillTransfers=c.skillTransfers;coop->rescued=c.rescued;coop->laserColumns=c.columns;
    coop->starResidue=c.starResidue;coop->chargeResidue=c.chargeResidue;for(unsigned i=0;i<c.seats;i++)if(c.giftsOwed[i]){auto& v=i?coop->extra[i-1]->motivation:motivationRaw;v=std::min(v+int(c.giftsOwed[i])*10000,MotivationCap);if(v>=MotivationCap)coop->starResidue[i]=0;}
    coopCarry.reset();return 1;
}

extern "C" unsigned th128_coop_message_music(){return coopActive()?messageMusicSerial:0;}

namespace {
bool coopBeamContact(th128::laser::LineSnapshot& line,std::string& error){
    using namespace th128;std::optional<unsigned> cut;float along=std::numeric_limits<float>::infinity();bullet::Position at{};
    for(unsigned i=0;i<coop->seats;i++){
        auto& body=i?coop->extra[i-1]->body:*playerHit;if(body.snapshot().gate.stateF78!=1)continue;
        const auto p=player::toPixels(i?coop->extra[i-1]->raw:playerRaw);const bullet::Position pos{p.horizontal,p.vertical,0};
        const auto decision=laser::classifyOverlap(line,{pos,playerProfileRadius*.5f,playerProfileRadius*.5f,body.snapshot().gate,playerHitControl});
        if(!decision){error="Invalid cooperative laser collision geometry";return false;}
        if(*decision==laser::CollisionKind::Graze){if(!i){if(line.grazeClock%3==0&&!applyGrazeAt({pos.x,pos.y,pos.z},error))return false;++line.grazeClock;}
            else{auto& a=*coop->extra[i-1];if(line.age%3==0){graze::Globals g{a.grazes,a.icePower};if(!applyOwnedGraze(g,{pos.x,pos.y,pos.z},error))return false;a.grazes=g.grazeCount;a.icePower=g.powerRaw;}}}
        else if(*decision==laser::CollisionKind::HitCallRequired){const auto hit=body.confirmedOverlap({pos.x,pos.y,pos.z},playerHitControl,error);if(!hit||*hit!=1)return false;
            const float d=float((double(pos.x)-line.source.x)*std::cos(line.angle)+(double(pos.y)-line.source.y)*std::sin(line.angle));
            if(d<along){along=d;cut=i;at=pos;}}
    }
    // CUT mutates the shared owner. All contacts above saw the same original beam; split it only once.
    return !cut||laserLines->cancelRegion(line,at,32,32,0,true,error);
}
}
namespace th128::coopphysics {
bool columnContact(laser::selector3::Position start,float angle,float& length,std::array<unsigned,3>& hits){
    if(!coopActive())return false;float nearest=length;std::optional<unsigned> winner;
    for(unsigned i=0;i<coop->seats;i++){const auto& body=i?coop->extra[i-1]->body:*playerHit;if(body.snapshot().gate.stateF78!=1)continue;
        const auto pos=player::toPixels(i?coop->extra[i-1]->raw:playerRaw);const auto hit=laser::selector3::rayCircle(start,angle,{{pos.horizontal,pos.vertical,0},6});
        if(hit&&(*hit)[0]<nearest&&(*hit)[1]>0){nearest=(*hit)[0];winner=i;}}
    if(winner){++hits[*winner];length=std::max(0.f,nearest);}return true;
}
bool dispatchColumns(const std::array<unsigned,3>& hits,std::string& error){
    if(!coopActive())return true;for(unsigned i=0;i<coop->seats;i++){const unsigned n=hits[i];if(!n)continue;
        coop->laserColumns[i]+=n;auto& body=i?coop->extra[i-1]->body:*playerHit;if(!body.laserColumnContactRequired(n))continue;
        const auto px=player::toPixels(i?coop->extra[i-1]->raw:playerRaw);if(!coopSound(75,px.horizontal,error))return false;
        auto& motivation=i?coop->extra[i-1]->motivation:motivationRaw;auto& flag=i?coop->extra[i-1]->motivationNotice:motivationFlag;
        if(!body.applyLaserColumnContact(n,motivation,flag,error)||!world->applyLaserSpellPenalty(n,error))return false;}
    return true;
}
}
