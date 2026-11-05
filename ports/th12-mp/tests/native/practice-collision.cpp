#include "game/GameState.hpp"
#include "game/Practice.hpp"
#include "wasm/Api.hpp"
#include <cstdio>
#include <cstdlib>
using namespace th12;
static void check(bool ok,const char* why){if(!ok){std::fprintf(stderr,"%s\n",why);std::exit(1);}}
int main(){
  th12_start(0,0,1,1234);practice::setEnabled(true);practice::setTools(practice::Invulnerable);
  invuln=0;playerState=1;const int life=lives,energy=power,bomb=bombs;const auto randomBefore=th12::random;
  check(spellState.start({}),"Spell fixture failed to start");spellState.age=120;
  sound_system::queue.reset();playerMissCollision();
  check(th12_sound_events()==1,"Invulnerable contact omitted miss sound");
  const auto* event=static_cast<const int32_t*>(th12_sound_events_ptr());
  check(event[0]==2,"Invulnerable contact used wrong sound ID");
  check(playerState==1&&deathWindow==0&&invuln==6,"Invulnerable contact entered death or lost six-tick guard");
  check(lives==life&&power==energy&&bombs==bomb&&!(eventBits&32)&&practice::trackerInfo().misses==0,"Invulnerable contact changed resources or counted a miss");
  check(th12::random.seed==randomBefore.seed&&th12::random.calls==randomBefore.calls,"Invulnerable contact created death effects consuming game RNG");
  check(!spellState.eligible()&&spellState.failure==spell_system::Failure::Miss&&spellState.bonusDisplayed==0,"Invulnerable collision retained spell capture eligibility");
  int32_t stored=12345;const auto result=spellState.end(stored);
  check(result.kind==spell_system::ResultKind::Failed&&stored==12345&&result.loadoutRecord.captures==0,"Invulnerable contact awarded a captured spell");
  check(spellState.start({}),"Next spell failed to start");spellState.age=120;
  sound_system::queue.reset();playerMissCollision();check(th12_sound_events()==0,"Guarded contact repeated miss sound");
  check(spellState.eligible(),"Existing six-tick invulnerability incorrectly failed the next spell");
  for(int i=0;i<6;i++)tickPlayer(0,0);
  check(invuln==0,"Six-tick contact guard did not expire");playerMissCollision();check(th12_sound_events()==1,"Next eligible invulnerable contact omitted sound");
  check(!spellState.eligible(),"Second eligible contact retained capture eligibility");
  invuln=0;sound_system::queue.reset();practice::setReplay(true);playerMissCollision();
  check(playerState==4&&(eventBits&32)&&th12_sound_events()==1,"Replay incorrectly applied live invulnerability tool");
  std::puts("PASS purple invulnerability collision sound, six-tick gating, resources and replay isolation");
}
