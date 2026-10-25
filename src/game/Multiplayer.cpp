#include "Multiplayer.hpp"
#include "GameState.hpp"
#include "GameSession.hpp"
#include "StageFlow.hpp"
#include "EnemyCallbacks.hpp"
#include "LaserCollision.hpp"
#include "LaserWorld.hpp"
#include "FriendlyPool.hpp"
#include "../multiplayer/StageEntry.hpp"
#include <eagler/gameplay/CooperativeRules.hpp>

namespace th12::multiplayer {
std::array<Pilot, MaxPlayers> pilots{};
Options options{};
bool active = false, paused = false, finished = false;
bool restartRequested=false,returnRequested=false,saveRequested=false,menuConfirm=false;
unsigned menuSelection=0,menuAction=0,menuPreviousHeld=0,menuAge=0,menuHoldAge=0,storyPreviousHeld=0;
unsigned currentSeat = 0, tickNumber = 0, wipeFrames = 0, lifeFragments = 0, bombFragments = 0;
int teamRank = 0;
std::unordered_map<uint32_t, unsigned> damageRemainders;
std::unordered_map<uint32_t, uint8_t> bulletGrazeMasks;
Pilot& pilot() { return pilots[currentSeat]; }
Scope::Scope(unsigned seat) : previous(currentSeat) { currentSeat = seat; }
Scope::~Scope() { currentSeat = previous; }
bool Options::valid() const {
  if (count < 2 || count > 3 || local >= count || seed > 65535 || stage < 1 || stage > 7 ||
      difficulty < 0 || difficulty > 4 || (stage == 7) != (difficulty == 4)) return false;
  for (unsigned s = 0; s < count; ++s) if (selections[s] < 0 || selections[s] > 5) return false;
  return true;
}
const std::string& snapshot() {
  static std::string buffer;
  std::ostringstream out;
  out << "{\"version\":1,\"frame\":" << tickNumber << ",\"stage\":" << stageNumber
      << ",\"paused\":" << paused << ",\"menuSelection\":" << menuSelection << ",\"menuConfirm\":" << menuConfirm
      << ",\"menuAction\":" << menuAction << ",\"returnRequested\":" << returnRequested << ",\"saveRequested\":" << saveRequested
      << ",\"restart\":" << restartRequested << ",\"finished\":" << finished << ",\"score\":" << score * int64_t(10)
      << ",\"ufoColors\":[" << ufoManager.inventory.colors[0] << ',' << ufoManager.inventory.colors[1]
      << ',' << ufoManager.inventory.colors[2] << "],\"players\":[";
  for (unsigned s = 0; s < options.count; ++s) {
    const auto& p = pilots[s]; const auto& r = p.resources;
    if (s) out << ',';
    out << "{\"seat\":" << s << ",\"selection\":" << options.selections[s] << ",\"ghost\":" << p.ghost
        << ",\"x\":" << p.px / 128.f << ",\"y\":" << p.py / 128.f << ",\"state\":" << p.playerState
        << ",\"lives\":" << r.lives << ",\"bombs\":" << r.bombs << ",\"power\":" << r.power
        << ",\"maxPower\":" << r.maxPower << ",\"pointValue\":" << r.pointValueStored
        << ",\"lifeFragments\":" << r.lifeFragments << ",\"bombFragments\":" << r.bombFragments
        << ",\"lifeTaps\":" << eagler::cooperative::TapCount(p.lifeTaps) << ",\"powerTaps\":" << p.powerTaps
        << ",\"graze\":" << p.graze << ",\"misses\":" << p.misses << '}';
  }
  out << "]}"; buffer = out.str(); return buffer;
}
uint32_t canonicalHash() {
  uint32_t hash = 2166136261u;
  const auto bytes = [&](const std::string& value) { for (unsigned char b : value) hash = (hash ^ b) * 16777619u; };
  for (unsigned s = 0; s < options.count; ++s) { Scope owner(s); bytes(captureState()); }
  bytes(snapshot());
  const auto word = [&](uint32_t value) { for (unsigned i = 0; i < 4; ++i) hash = (hash ^ ((value >> (i * 8)) & 255)) * 16777619u; };
  for (const auto& p : pilots) {
    word(p.lifeTaps); word(p.lifeLatched); word(p.powerTaps); word(p.powerGap); word(p.gestureHeld);
    word(p.ghostRandom); word(p.ghostClock); word(p.ghostDx); word(p.ghostDy);
  }
  // Menu age is presentation time; idle paused peers may be one input frame
  // apart while displaying the same authoritative world and menu selection.
  word(menuPreviousHeld); word(menuHoldAge); word(storyPreviousHeld);
  word(temporaryEnemyOwner);
  for (const auto& item : itemManager.entities) if (item.active()) { word(item.id); word(item.recipient); word(item.collector); word(item.gift); }
  std::vector<uint32_t> ids;
  for (const auto& [id, unused] : damageRemainders) ids.push_back(id);
  std::sort(ids.begin(), ids.end()); for (auto id : ids) { word(id); word(damageRemainders.at(id)); }
  ids.clear(); for (const auto& [id, unused] : bulletGrazeMasks) ids.push_back(id);
  std::sort(ids.begin(), ids.end()); for (auto id : ids) { word(id); word(bulletGrazeMasks.at(id)); }
  return hash;
}
bool operable(unsigned seat) { return seat < options.count && !pilots[seat].ghost; }
unsigned operableCount() { unsigned n = 0; for (unsigned s = 0; s < options.count; ++s) n += operable(s); return n; }
namespace {
double distance2(const Pilot& a, const Pilot& b) {
  const double x = double(a.px - b.px) / 128, y = double(a.py - b.py) / 128;
  return x*x + y*y;
}
double distance2(const Pilot& a, float x, float y) {
  const double dx = double(a.px) / 128 - x, dy = double(a.py) / 128 - y;
  return dx*dx + dy*dy;
}
bool alive(unsigned s) { return operable(s) && pilots[s].playerState != 2 && pilots[s].playerState != 4; }
uint32_t ghostNext(uint32_t& state) { state ^= state<<13; state ^= state>>17; state ^= state<<5; return state; }
void resetPilot(unsigned s, bool carry) {
  auto& p = pilots[s];
  const auto resources = p.resources;
  const auto graze = p.graze;
  const auto misses = p.misses;
  const int x=p.px,y=p.py;const bool focused=p.playerFocused;
  const bool ghost = p.ghost;
  std::destroy_at(&p);
  std::construct_at(&p);
  p.seat = s; p.character = options.selections[s] / 2; p.shot = options.selections[s] % 2;
  p.resources = carry ? resources : item_system::ResourceState::initial(options.difficulty);
  p.resources.rank = teamRank;
  p.graze = carry ? graze : 0; p.misses = carry ? misses : 0;
  p.ghostRandom = options.seed ^ (0x9e3779b9u * (s + 1));
  if (!p.ghostRandom) p.ghostRandom = 1;
  p.px = carry?x:int((float(s) - float(options.count - 1) * .5f) * (options.count == 2 ? 48 : 56) * 128);
  if(carry){p.py=y;p.playerFocused=focused;}
  if (carry && ghost) { p.resources.lives = std::max(0, p.resources.lives); p.resources.power = p.resources.maxPower / 2; p.resources.bombs = std::max(2, p.resources.bombs); }
  if (options.challenge) { p.resources.bombs = 0; p.resources.lives = 2; }
  if (pendingStageEntry) {
    p.resources=pendingStageEntry->resources[s]; p.graze=pendingStageEntry->graze[s]; p.misses=pendingStageEntry->misses[s];
    if(pendingStageEntry->version>=2){p.px=pendingStageEntry->x[s];p.py=pendingStageEntry->y[s];p.playerFocused=pendingStageEntry->focused[s]!=0;}
  }
  p.playerMotion.updateOptions(loadouts[options.selections[s]], p.resources.power, p.px, p.py, p.playerFocused, true);
  Scope owner(s); initializePlayerAnimation();
}
}
int nearest(float x, float y, bool needsPower) {
  int best = -1; double d = 0;
  for (unsigned s = 0; s < options.count; ++s) {
    if (!alive(s) || (needsPower && pilots[s].resources.power >= pilots[s].resources.maxPower)) continue;
    const double next = distance2(pilots[s], x, y);
    if (best < 0 || next < d) { best = int(s); d = next; }
  }
  return best;
}
std::array<float, 2> target(float x, float y) {
  const int s = nearest(x, y);
  return s < 0 ? std::array<float,2>{0,400} : std::array<float,2>{pilots[s].px/128.f,pilots[s].py/128.f};
}
bool anyBomb() { for (unsigned s = 0; s < options.count; ++s) if (operable(s) && pilots[s].playerBomb.active) return true; return false; }
void shareInvulnerability(unsigned source, int ticks) {
  if (!active || ticks <= 0) return;
  for (unsigned s = 0; s < options.count; ++s) if (s != source && operable(s)) {
    pilots[s].invuln = std::max(pilots[s].invuln, ticks);
    pilots[s].playerInvulnerabilityTime = float(pilots[s].invuln);
  }
}
int deathbombFrames() { return active ? eagler::cooperative::DeathbombFrames(8) : 8; }
void syncRank(int before) {
  if (!active) return;
  const int delta = pilot().resources.rank - before;
  teamRank = std::clamp(before + (delta < 0 ? delta / int(options.count) : delta), -1024, 1024);
  for (unsigned s = 0; s < options.count; ++s) pilots[s].resources.rank = teamRank;
}
void syncScore(int before) {
  score = int(std::clamp(int64_t(score) + pilot().resources.scoreStored - before, int64_t(0), int64_t(999999999)));
  for (unsigned s = 0; s < options.count; ++s) pilots[s].resources.scoreStored = score;
}
void initializePilots(bool nextStage) {
  for (unsigned s = 0; s < options.count; ++s) resetPilot(s, nextStage);
  paused=menuConfirm=false;menuSelection=menuAction=menuPreviousHeld=menuAge=menuHoldAge=storyPreviousHeld=0;
  wipeFrames = 0; damageRemainders.clear(); bulletGrazeMasks.clear();
}
bool begin(const Options& setup) {
  if (!setup.valid() || stageNumber != int(setup.stage)) return false;
  options = setup; currentSeat = 0; active = true; paused = finished = false;
  restartRequested=returnRequested=saveRequested=menuConfirm=false;
  menuSelection=menuAction=menuPreviousHeld=menuAge=menuHoldAge=storyPreviousHeld=0;
  tickNumber = wipeFrames = lifeFragments = bombFragments = 0; teamRank = score = 0;
  for (unsigned s = 0; s < MaxPlayers; ++s) {
    std::destroy_at(&pilots[s]); std::construct_at(&pilots[s]); pilots[s].seat = s;
  }
  startGame(setup.selections[0]/2, setup.selections[0]%2, setup.difficulty, setup.seed);
  // Construct every player before shared ECL receives its first input tick.
  setFullGameMode(false);
  spellState.replayWallClock = true; // network frame time is the only spell clock
  return true;
}
void becameGhost() {
  auto& p = pilot(); p.ghost = true; p.ghostClock = 0; p.playerState = 3;
  p.playerMotion.count = 0; p.playerDamageSources.reset(); p.playerBomb.reset();
  p.playerFocused = false; p.lifeTaps = p.powerTaps = 0;
  if (p.playerFocusAnimation) animationScene.interrupt(p.playerFocusAnimation, 1);
  p.playerFocusAnimation = 0;
  for (auto& b : bullets) if (b.friendly && b.ownerSeat == currentSeat) b.active = false;
  initializePlayerAnimation();
}
void revive(unsigned seat, bool rescued) {
  Scope owner(seat); auto& p = pilot();
  p.ghost = false; p.ghostClock = p.lifeTaps = p.powerTaps = 0; p.lifeLatched = false;
  p.resources.lives = std::max(p.resources.lives, 0); p.resources.power = p.resources.maxPower / 2;
  p.resources.bombs = options.challenge ? 0 : std::max(2, p.resources.bombs);
  p.playerState = 1; p.playerStateTicks = 60; p.playerStateTime = 60;
  p.invuln = rescued ? 280 : 120; p.playerInvulnerabilityTime = float(p.invuln);
  p.playerDamageSources.reset(); p.playerBomb.reset(); p.playerFlags = 0;
  p.playerFocused = false; p.playerMotion = {};
  p.playerMotion.updateOptions(loadouts[p.character*2+p.shot], p.resources.power, p.px, p.py, false, true);
  initializePlayerAnimation();
}
void tickPlayers() {
  for (unsigned s = 0; s < options.count; ++s) {
    Scope owner(s); auto& p = pilot();
    if (p.ghost) {
      if (p.ghostClock++ % 90 == 0) { p.ghostDx = int(ghostNext(p.ghostRandom)%97)-48; p.ghostDy = int(ghostNext(p.ghostRandom)%81)-40; }
      p.px = std::clamp(p.px+p.ghostDx,-23552,23552); p.py = std::clamp(p.py+p.ghostDy,4096,55296);
      tickPlayerAnimation(); continue;
    }
    const int rank = teamRank; p.resources.rank = rank;
    const int beforeInvuln = p.invuln;
    p.adaptedMotion.begin(p.input.motion, p.input.x, p.input.y);
    const int held = options.challenge ? int(p.input.held & ~2u) : int(p.input.held);
    tickPlayer(held, held & ~p.previousHeld); p.adaptedMotion.finish();
    syncRank(rank);
    if (p.playerBomb.active && p.invuln > beforeInvuln) shareInvulnerability(s,p.invuln);
  }
}
void tickBombs() {
  for (unsigned s = 0; s < options.count; ++s) if (operable(s)) {
    Scope owner(s); const int before = pilot().invuln; tickBomb();
    if (pilot().playerBomb.active && pilot().invuln > before) shareInvulnerability(s,pilot().invuln);
  }
}
void tickRules() {
  ++tickNumber;
  for (unsigned s = 0; s < options.count; ++s) {
    auto& p = pilots[s]; const auto held = p.input.held;
    const auto pressed = held & ~p.gestureHeld; p.gestureHeld = held;
    if (!(held&8)) p.lifeLatched = false;
    if ((pressed&8) && !(held&1)) p.powerTaps = p.powerGap = 0;
    if (p.powerGap < 25) ++p.powerGap;
    if (p.powerGap > 24) p.powerTaps = 0;
    if (!alive(s) || dialogue || fullGameFlow.status != FullGameStatus::Playing) { p.lifeTaps = 0; continue; }
    int receiver = -1;
    if (!(held&1) && !p.lifeLatched && p.resources.lives>0) {
      for (unsigned j=0;j<options.count;++j) {
        const auto& q=pilots[j]; if(j==s||(!alive(j)&&!q.ghost)||(!q.ghost&&q.resources.lives>=8)||distance2(p,q)>400)continue;
        if(receiver<0||q.ghost>pilots[unsigned(receiver)].ghost||
           (q.ghost==pilots[unsigned(receiver)].ghost&&q.resources.lives<pilots[unsigned(receiver)].resources.lives))receiver=int(j);
      }
    }
    if (eagler::cooperative::AdvanceLifeTransfer(p.lifeTaps,(pressed&8)!=0,receiver>=0)) {
      auto& q=pilots[unsigned(receiver)]; bool delivered=false;
      if(q.ghost) { --p.resources.lives;revive(unsigned(receiver),true);delivered=true; }
      else if(auto* item=itemManager.spawn(6,{p.px/128.f,p.py/128.f})) { item->recipient=receiver;item->gift=true;item->state=item_system::State::CollectedAbove;item->attractionSpeed=8;--p.resources.lives;delivered=true; }
      if(delivered)p.lifeLatched=true;
    }
    if(pressed&1) {
      p.powerGap=0;
      if(++p.powerTaps>=5) {
        p.powerTaps=0;
        receiver=eagler::cooperative::LowestPower(options.count,
          [&](unsigned j){return j!=s&&alive(j)&&pilots[j].resources.power<pilots[j].resources.maxPower&&distance2(p,pilots[j])<=400;},
          [&](unsigned j){return pilots[j].resources.power;},[&](unsigned j){return pilots[j].resources.powerStep;});
        if(receiver>=0&&p.resources.power>=p.resources.powerStep)
          if(auto* item=itemManager.spawn(3,{p.px/128.f,p.py/128.f})) {item->recipient=receiver;item->gift=true;item->state=item_system::State::CollectedAbove;item->attractionSpeed=8;p.resources.power-=p.resources.powerStep;}
      }
    }
  }
  if(!operableCount()) { if(++wipeFrames>=180) {ended=2;fullGameFlow.status=FullGameStatus::GameOver;} }
  else wipeFrames=0;
}
int bossDamage(const Enemy& e,int amount) {
  if(!active||amount<=0||(!(e.flags&0x400000u)&&!e.boss&&!e.isUfo))return amount;
  // Match TH15: an exhausted pilot leaves the damage coefficient immediately,
  // including the death animation before the ghost transition completes.
  unsigned count=0;for(unsigned s=0;s<options.count;++s)
    count+=!pilots[s].ghost&&pilots[s].resources.lives>=0;
  const auto factor=count<2?12u:count==2?9u:8u;
  auto& remainder=damageRemainders[e.id];const uint64_t scaled=uint64_t(unsigned(amount))*factor+remainder;
  remainder=unsigned(scaled%12);return int(scaled/12);
}
bool update(const std::array<Input,MaxPlayers>& inputs) {
  if(!active||finished)return false;
  bool pauseEdge=false;
  for(unsigned s=0;s<options.count;++s) {
    const auto& i=inputs[s];if(i.motion<0||i.motion>3||(i.held&~0x32ffu)||
      (i.motion==3&&(i.x<-32767||i.x>32767||i.y<-32767||i.y>32767))||
      (i.motion>0&&i.motion<3&&(i.x<-23552||i.x>23552||i.y<4096||i.y>55296)))return false;
    pauseEdge|=i.pause&&!pilots[s].pauseHeld;
  }
  for(unsigned s=0;s<options.count;++s){pilots[s].pauseHeld=inputs[s].pause;pilots[s].input=inputs[s];}
  unsigned menuHeld=0;for(unsigned s=0;s<options.count;++s)menuHeld|=inputs[s].held;
  const auto menuPressed=menuHeld&~menuPreviousHeld;
  const auto previousMenuHeld=menuPreviousHeld;menuPreviousHeld=menuHeld;
  if(pauseEdge){
    if(paused&&menuConfirm){menuConfirm=false;menuSelection=menuAction;menuAge=0;}
    else {paused=!paused;menuSelection=menuAction=menuAge=0;menuConfirm=false;}
  }
  if(paused){
    sound_system::queue.reset();
    ++menuAge;
    if(menuPressed&0x2000u){restartRequested=true;finished=true;return true;}
    const auto directions=menuHeld&0xf0u;
    menuHoldAge=directions&&directions==(previousMenuHeld&0xf0u)?menuHoldAge+1:0;
    const auto move=menuPressed|(menuHoldAge>=20&&menuHoldAge%4==0?directions:0u);
    const unsigned count=menuConfirm?2:4;
    if(move&16)menuSelection=(menuSelection+count-1)%count;
    if(move&32)menuSelection=(menuSelection+1)%count;
    if(menuPressed&2){
      if(menuConfirm){menuConfirm=false;menuSelection=menuAction;menuAge=0;}else paused=false;
    }else if(menuPressed&0x1001u){
      if(menuConfirm){
        if(menuSelection){menuConfirm=false;menuSelection=menuAction;menuAge=0;}
        else {restartRequested=menuAction==3;returnRequested=menuAction==1;saveRequested=menuAction==2;finished=true;}
      }else if(!menuSelection)paused=false;
      else {menuAction=menuSelection;menuSelection=1;menuConfirm=true;menuAge=0;}
    }
    return true;
  }
  if(menuPressed&0x2000u){paused=true;restartRequested=true;finished=true;return true;}
  Scope owner(0);
  // Dialogues use the same combined non-movement controls on every peer.
  uint32_t story=0;for(unsigned s=0;s<options.count;++s)story|=inputs[s].held&0x201u;
  const auto storyPressed=story&~storyPreviousHeld;storyPreviousHeld=story;
  if(!tickFullGame(int(story),int(storyPressed)))tick(int(story),int(storyPressed));
  if(fullGameFlow.status==FullGameStatus::GameOver||fullGameFlow.status==FullGameStatus::Result||fullGameFlow.status==FullGameStatus::Ending)finished=true;
  return true;
}
eb::Collision bulletCollision(eb::State& state) {
  eb::Collision result=eb::Collision::None;auto& mask=bulletGrazeMasks[state.id];
  for(unsigned s=0;s<options.count;++s)if(operable(s)) {
    Scope owner(s);auto& p=pilot();const auto c=eb::collide(state,p.px/128.f,p.py/128.f,eb::playerHitboxes[p.character]);
    if(c==eb::Collision::Graze&&!(mask&(1u<<s))){mask|=uint8_t(1u<<s);awardPlayerGraze(p.px/128.f,p.py/128.f);}
    if(c==eb::Collision::Hit&&!dialogue&&p.playerState!=2&&p.playerState!=3&&p.playerState!=4&&!(p.playerFlags&2u)){playerMissCollision();result=c;}
  }
  return result;
}
laser::Collision laserCollision(const laser::Vec3& pos,float angle,float width,float length,uint8_t* grazers) {
  if(grazers)*grazers=0;
  laser::Collision out=laser::Collision::None;
  for(unsigned s=0;s<options.count;++s)if(operable(s)) {
    Scope owner(s);auto& p=pilot();laser::PlayerCollision view{{p.px/128.f,p.py/128.f,0},eb::playerHitboxes[p.character]*.5f,eb::playerHitboxes[p.character]*.5f,p.playerState,p.invuln,p.playerFlags,dialogue>0,loadouts[p.character*2+p.shot].hit};
    const auto c=laser::collidePlayerLine(pos,angle,width,length,view);if(c.beginMiss)playerMissCollision();
    if(c.code==laser::Collision::Graze&&grazers)*grazers|=uint8_t(1u<<s);
    if(c.code==laser::Collision::Hit)out=c.code;else if(c.code==laser::Collision::Graze&&out==laser::Collision::None)out=c.code;
  }
  return out;
}
int allPlayerDamage(const Enemy& e,float x,float y,float w,float h) {
  int sum=0;for(unsigned s=0;s<options.count;++s)if(operable(s)) {
    Scope owner(s);auto& p=pilot();int amount=queryPlayerDamage(e,x,y,w,h);
    if(p.playerState==0||p.playerState==2)amount/=5;
    if(spellState.active()&&spellState.cardId>=93&&spellState.cardId<=99&&(p.playerBomb.active||p.invuln||(p.playerFlags&2u))&&!(p.playerFlags&4u))amount/=5;
    sum+=amount;
  }return sum;
}
void enemyCollision(Enemy& e) {for(unsigned s=0;s<options.count;++s)if(operable(s)){Scope owner(s);customEnemyCollision(e);}}
void collectSharedPiece(int type) {
  auto& count=type==4?lifeFragments:bombFragments;
  count+=type==4?(count?1:2):2;
  if(count>=5){count=0;for(unsigned s=0;s<options.count;++s){auto& r=pilots[s].resources;if(type==4)r.lives=std::min(8,r.lives+1);else if(!options.challenge)r.bombs=std::min(9,r.bombs+1);}sound_system::queue.play(type==4?18:48,0,false);}
  for(unsigned s=0;s<options.count;++s){pilots[s].resources.lifeFragments=int(lifeFragments);pilots[s].resources.bombFragments=int(bombFragments);}
  const int before=pilot().resources.rank;pilot().resources.addRank(256);syncRank(before);
}
void itemPlayer(item_system::Entity& item, item_system::PlayerView& out) {
  using S=item_system::State;
  const bool power=item.type==1||item.type==3||item.type==8||item.type==16||item.type==18;
  const bool homing=item.state==S::DelayedPoint||item.state==S::Automatic||item.state==S::Aggregate||
    item.state==S::CollectedAbove||item.state==S::CollectedNear;
  int seat=item.recipient;
  if (seat >= int(options.count)) item.recipient = seat = -1;
  if(seat>=0&&(!alive(unsigned(seat)))) {item.recipient=seat=-1;if(!homing){item.state=S::Falling;item.velocity={};}}
  auto choosePower=[&]{return eagler::cooperative::NearestPowerRecipient(options.count,
    [&](unsigned s){return alive(s);},[&](unsigned s){return pilots[s].resources.power<pilots[s].resources.maxPower;},
    [&](unsigned s){return distance2(pilots[s],item.position.x,item.position.y);});};
  if(seat>=0&&power&&pilots[unsigned(seat)].resources.power>=pilots[unsigned(seat)].resources.maxPower) {
    const int next=choosePower();if(next>=0&&pilots[unsigned(next)].resources.power<pilots[unsigned(next)].resources.maxPower){seat=next;item.recipient=seat;if(item.state!=S::Aggregate){item.state=S::CollectedAbove;item.attractionSpeed=8;}}
  }
  // Cancellation points and UFO aggregates already home without contact or
  // crossing the collection line. Bind their target before Item22 moves them.
  if(seat<0&&homing){seat=power?choosePower():nearest(item.position.x,item.position.y);if(seat>=0)item.recipient=seat;}
  if(seat<0) {
    double best=0;
    for(unsigned s=0;s<options.count;++s)if(alive(s)) {
      const auto& p=pilots[s];const auto v=item_system::PlayerView::native(p.character,{p.px/128.f,p.py/128.f},p.playerFocused,p.playerState);
      const float x=item.position.x+item.velocity.x*gameSpeed,y=item.position.y+item.velocity.y*gameSpeed;
      const bool contact=std::abs(x-v.position.x)<=v.nearHalfWidth&&std::abs(y-v.position.y)<=v.nearHalfHeight;
      if(!contact&&!(item.type<10&&(v.position.y<128||dialogue)))continue;
      const double d=distance2(p,item.position.x,item.position.y);if(seat<0||d<best){seat=int(s);best=d;}
    }
    if(seat>=0) {
      const auto& p=pilots[unsigned(seat)];const bool attract=item.type<10&&(p.py/128.f<128||dialogue);
      if(power&&(attract||p.resources.power>=p.resources.maxPower))seat=choosePower();
      if(seat>=0&&(attract||power&&pilots[unsigned(seat)].resources.power<pilots[unsigned(seat)].resources.maxPower)) {
        item.recipient=seat;
        if(attract||power){item.state=S::CollectedAbove;item.attractionSpeed=8;}
      }
    }
  }
  item.collector=seat;
  if(seat>=0){const auto& p=pilots[unsigned(seat)];out=item_system::PlayerView::native(p.character,{p.px/128.f,p.py/128.f},p.playerFocused,p.playerState);}
  else {out={};out.state=2;out.position={10000,10000};}
}
} // namespace th12::multiplayer
