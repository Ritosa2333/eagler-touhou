// TH128 spends the same 100% resource unit when it is used or gifted. Z *charge*
// (ice power) is personal and is not the transferable resource.
namespace {
using Raw=th128::player::RawPosition;
constexpr int StockUnit=th128::freeze::PerfectFreezeCostRaw;
Raw& seatPose(unsigned i){return i?coop->extra[i-1]->raw:playerRaw;}
th128::player::PlayerHitHost& seatBody(unsigned i){return i?coop->extra[i-1]->body:*playerHit;}
const th128::player::Input& seatKeys(unsigned i){return i?coop->extra[i-1]->input:playerInput;}
bool isGhost(unsigned i){const auto snap=seatBody(i).snapshot();return coop->retired[i]&&snap.gate.stateF78==2&&snap.stateTimer.current>=30;}
void driftGhost(unsigned i){auto& g=coop->ghosts[i];auto& p=seatPose(i);if((g.age++%96)==0){const auto a=world->effectGameRandom32();g.dx=int((a&127u))-64;g.dy=int((a>>8)&127u)-64;if(!g.dx&&!g.dy)g.dx=32;}
    p=th128::player::advancePosition(p,{g.dx,g.dy});if(i==0){const auto px=th128::player::toPixels(p);world->setPlayerAimPosition({px.horizontal,px.vertical,0},operationError);}}
unsigned outstandingLife(unsigned recipient){return std::count_if(coop->gifts.begin(),coop->gifts.end(),[recipient](const auto& g){return g.recipient==recipient&&g.kind==GiftKind::life;});}
int recipientFor(unsigned from,GiftKind kind){
    constexpr int range=20*th128::player::RawUnitsPerPixel;
    const auto p=seatPose(from);int best=-1,stock=0;bool ghostFirst=false;
    for(unsigned i=0;i<coop->seats;i++){
        if(i==from)continue;const auto q=seatPose(i);const int dx=p.horizontal-q.horizontal,dy=p.vertical-q.vertical;
        if(std::int64_t(dx)*dx+std::int64_t(dy)*dy>range*range)continue;
        const bool life=kind==GiftKind::life,ghost=life&&isGhost(i);
        const int value=life?seatMotivation(i):seatCharge(i);
        const int capacity=life?MotivationCap:th128::freeze::PerfectFreezeCapRaw;
        if(!ghost&&(seatBody(i).snapshot().gate.stateF78!=1||coop->retired[i]||value+int(1+(life?outstandingLife(i):0))*StockUnit>capacity))continue;
        const int rank=life?std::max(value,0)/StockUnit:value;
        if(best<0||(ghost&&!ghostFirst)||(ghost==ghostFirst&&rank<stock)){best=int(i);ghostFirst=ghost;stock=rank;}
    }
    return best;
}
bool transferLife(unsigned from,unsigned to,std::string& error){
    if(isGhost(to)){
        if(!seatBody(to).reviveCoopGhost(error))return false;
        // The gift buys the active life; do not erase motivation or X earned while retired.
        coop->retired[to]=false;coop->ghosts[to]={};++coop->rescued[to];
        if(to)coop->extra[to-1]->icePower=5000;else grazeGlobals.powerRaw=5000;
    }else coop->gifts.push_back({++coop->giftSerial,to,0,seatPose(from),GiftKind::life});
    seatMotivation(from)-=StockUnit;seatNotice(from)=0;++coop->transfers[from];return true;
}
bool transferSkill(unsigned from,unsigned to,std::string& error){
    // Commit the complete transfer on the fifth input; the original-art courier
    // shows direction. An intervening teammate hit, spend or stage transition
    // cannot discard, split, or re-award an in-flight 100% stock.
    seatCharge(from)-=StockUnit;seatCharge(to)+=StockUnit;++coop->skillTransfers[from];
    if((from==0||to==0)&&!perfectHud->charge(seatCharge(0),error))return false;
    coop->gifts.push_back({++coop->giftSerial,to,0,seatPose(from),GiftKind::skill});return true;
}
bool tickLifeTransfers(std::string& error){
    if(coop->challenge)return true;
    for(auto it=coop->gifts.begin();it!=coop->gifts.end();){auto& g=*it;const auto p=seatPose(g.recipient);const int dx=p.horizontal-g.pos.horizontal,dy=p.vertical-g.pos.vertical;
        const double dist=std::hypot(double(dx),double(dy));const int speed=6*th128::player::RawUnitsPerPixel;
        if(++g.age>=180||dist<=speed){if(g.kind==GiftKind::life&&!coop->retired[g.recipient]){seatMotivation(g.recipient)=std::min(MotivationCap,seatMotivation(g.recipient)+StockUnit);seatNotice(g.recipient)=0;}
            it=coop->gifts.erase(it);continue;}
        g.pos.horizontal+=int(dx*speed/dist);g.pos.vertical+=int(dy*speed/dist);++it;
    }
    const bool locked=(messagesConfigured&&messages->state().active)||(stageTransitionConfigured&&stageTransition->state().begun)||coopIsSettling();
    using namespace th128::player;
    for(unsigned i=0;i<coop->seats;i++){
        auto& life=coop->chains[i];auto& skill=coop->skillChains[i];
        if(isGhost(i)){life=skill={};driftGhost(i);continue;}
        const auto input=seatKeys(i);
        if(locked||seatBody(i).snapshot().gate.stateF78!=1||coop->retired[i]){life=skill={};continue;}
        auto gesture=[&](GiftKind kind,TransferChain& chain,int stock,unsigned key,unsigned excluded){
            if(stock<StockUnit||(input.holding&excluded)){chain={};return true;}
            const int target=recipientFor(i,kind);if(target<0){chain={};return true;}
            if(chain.target!=target||world->frame()-chain.last>24)chain={};
            if(!(input.pressed&key))return true;
            chain.target=target;chain.last=world->frame();if(++chain.presses<5)return true;
            chain={};return kind==GiftKind::life?transferLife(i,unsigned(target),error):transferSkill(i,unsigned(target),error);
        };
        if(!gesture(GiftKind::life,life,seatMotivation(i),FocusKey,RapidKey|ShootFreezeKey|BombKey)||
           !gesture(GiftKind::skill,skill,seatCharge(i),ShootFreezeKey,RapidKey|FocusKey|BombKey))return false;
    }return true;
}
template<class Writer>bool drawLifeTransfers(Writer& out,bool& comma){
    if(!coopActive())return true;
    const auto bank=scoreFont.bank(11);if(!bank||bank->sprites.size()<=271){presentationError="Co-op gift glyph bank missing";return false;}
    auto paint=[&](unsigned id,char letter,float x,float y,unsigned color,float scale){
        th128::anm::State s;s.sprite=int(letter)-32;s.layer=23;s.mode=0;s.anchorX=s.anchorY=1;s.primary=color;s.sx=s.sy=scale;
        return writeVisualRecord(out,comma,id,11,s,bank->sprites[std::size_t(s.sprite)],x,y,0);
    };
    for(const auto& gift:coop->gifts){const auto p=th128::player::toPixels(gift.pos);const bool life=gift.kind==GiftKind::life;
        if(!paint(0x38000000u+gift.id,life?'L':'S',314+p.horizontal,10+p.vertical,life?0xffaeffbbu:0xff9ceeffu,1.25f))return false;
    }
    for(unsigned seat=0;seat<coop->seats;seat++){
        const auto& life=coop->chains[seat];const auto& skill=coop->skillChains[seat];const bool lifeVisible=life.presses>=3;
        const auto count=lifeVisible?life.presses:skill.presses;if(count<3||count>4)continue;
        const auto p=th128::player::toPixels(seatPose(seat));const auto y=std::max(20.f,p.vertical-51.f);
        const char text[]={lifeVisible?'L':'S',' ',char('0'+count),'/','5'};
        unsigned j=0;for(const char c:text){
            if(!paint(0x39000000u+seat*16+j,c,297.5f+p.horizontal+9.f*j,y,lifeVisible?0xffaeffbbu:0xff9ceeffu,1.f))return false;
            ++j;
        }
    }
    return true;
}
}
