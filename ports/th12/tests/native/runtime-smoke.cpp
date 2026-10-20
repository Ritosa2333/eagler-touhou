#include "game/Laser.hpp"
#include "game/GameState.hpp"
#include "game/BulletCancellation.hpp"
#include "wasm/Api.hpp"
#include <array>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

static bool check(bool condition, const char* message) {
  if (!condition)
    std::fprintf(stderr, "%s\n", message);
  return condition;
}
int main() {
  // Original41a9f0 accepts damage-immune targets and excludes targeting flags.
  // These samples include the one-tick birth immunity seen in replay01.
  for (const auto flags : {0u, 0x10u, 1u, 0x20u, 0x2000000u, 0x4000000u}) {
    th12::enemies.clear();
    auto enemy = std::make_unique<th12::Enemy>();
    enemy->id = 1;
    enemy->active = true;
    enemy->x = 20;
    enemy->y = 40;
    enemy->flags = flags;
    enemy->immunityTicks = 1;
    enemy->invincible = true;
    th12::enemies.push_back(std::move(enemy));
    const bool accepted = flags == 0 || flags == 0x10;
    if (!check(bool(th12::playerTarget(0, 0, 256)) == accepted,
               "Original homing acquisition differs for an immune/flagged enemy") ||
        !check(bool(th12::playerTarget(0, 0, 256, 1)) == accepted,
               "Original homing target retention differs for an immune/flagged enemy"))
      return 1;
  }
  th12::enemies.clear();
  // Actual43a480 two-update samples: an invalid still-linked target is
  // released without turning; the next update acquires the other enemy.
  for (const auto flags : {1u, 0x20u, 0x2000000u, 0x4000000u}) {
    th12::enemies.clear();
    auto old = std::make_unique<th12::Enemy>();
    old->id = 91; old->active = true; old->flags = flags; old->x = -80; old->y = 100;
    th12::enemies.push_back(std::move(old));
    auto other = std::make_unique<th12::Enemy>();
    other->id = 92; other->active = true; other->flags = 0; other->x = 80; other->y = 100;
    th12::enemies.push_back(std::move(other));
    th12::Projectile bullet;
    bullet.friendly = bullet.active = true;
    bullet.x = 0; bullet.y = 300; bullet.speed = 8;
    bullet.angle = -1.5707963705062866f;
    bullet.update = 1; bullet.age = 10; bullet.targetId = 91;
    bullet.weapon.phase = th12::pw::Phase::Alive;
    th12::updateFriendlyProjectile(bullet);
    if (!check(bullet.targetId == 0 && bullet.speed == 8.100000381469727f &&
                   bullet.angle == -1.5707963705062866f,
               "Invalid retained target reacquired during the same original update"))
      return 1;
    ++bullet.age;
    th12::updateFriendlyProjectile(bullet);
    if (!check(bullet.targetId == 92 && bullet.speed == 8.100000381469727f &&
                   std::abs(bullet.angle - -1.5403558015823364f) < 0.000001f,
               "Original homing acquisition did not resume on the following update"))
      return 1;
  }
  th12::enemies.clear();
  if (!check(!th12::bullet_cancellation::viewportOverlaps(-180, 422, 4, 4),
             "Retail ECL510 unexpectedly uses playfield dimensions") ||
      !check(th12::bullet_cancellation::viewportOverlaps(2, 0, 4, 4),
             "Retail zero rectangle lost inclusive boundary contact") ||
      !check(!th12::bullet_cancellation::viewportOverlaps(2.01f, 0, 4, 4),
             "Retail zero rectangle accepts an outside bullet") ||
      !check(th12::bullet_cancellation::viewportOverlaps(-180, 422, 4, 4, 0, 224, 384, 448),
             "Explicitly constructed rectangle differs from original probes"))
    return 1;
  // Actual43a810: both ends of Sanae A's vertical targeting band are
  // inclusive, stop sends IRQ2, and the horizontal launch waits until1004.
  auto target = std::make_unique<th12::Enemy>();
  target->id = 94; target->active = true; target->flags = 0;
  target->x = -42.26f; target->y = 160;
  th12::enemies.push_back(std::move(target));
  struct SanaeSample {float y;int age;bool impact,stop;};
  const SanaeSample sanaeSamples[]={{176.01f,5,false,false},{176,5,false,true},
      {175.99f,5,false,true},{128,5,false,true},{127.99f,5,false,false},
      {128.01f,5,false,true},{176,4,false,false},{176,5,true,false}};
  auto savedInterruptHook = th12::friendlyInterruptHook;
  int interruptCount = 0, interruptCode = 0;
  th12::friendlyInterruptHook = [&](th12::Projectile&,int code,bool auxiliary) {
    if (!auxiliary) {++interruptCount;interruptCode=code;}
  };
  for (const auto& sample : sanaeSamples) {
    interruptCount=interruptCode=0;
    th12::Projectile bullet;
    bullet.friendly = bullet.active = true; bullet.x=30;bullet.y=sample.y;
    bullet.speed=20;bullet.angle=-1.5707963705062866f;
    bullet.update=3;bullet.age=sample.age;
    bullet.weapon.phase=sample.impact?th12::pw::Phase::Impact:th12::pw::Phase::Alive;
    bullet.weapon.impactDuration=100;
    th12::updateFriendlyProjectile(bullet);
    if (!check(bullet.age==(sample.stop?1000:sample.age)&&
                   bullet.speed==(sample.stop?0.f:20.f)&&
                   interruptCount==(sample.stop?1:0)&&(!sample.stop||interruptCode==2),
               "Sanae A stop differs from original vertical-band/age/phase sample")||
        !check(!sample.stop||(bullet.weapon.previousAge==999&&bullet.weapon.ageTime==1000.f),
               "Sanae A stop did not reset the original complete timer"))
      return 1;
  }
  th12::friendlyInterruptHook = savedInterruptHook;
  for (int age=1000;age<=1004;++age) {
    th12::Projectile bullet;
    bullet.friendly=bullet.active=true;bullet.update=3;bullet.age=age;
    bullet.speed=0;bullet.angle=-1.5707963705062866f;bullet.targetId=0;
    bullet.weapon.phase=th12::pw::Phase::Alive;
    th12::updateFriendlyProjectile(bullet);
    if (!check(bullet.speed==(age==1004?24.f:0.f)&&
                   bullet.angle==(age==1004?3.1415927410125732f:-1.5707963705062866f),
               "Sanae A horizontal launch differs from original1000..1004 samples"))
      return 1;
  }
  th12::enemies.clear();
  // Four independently captured values from the immutable native RNG corpus.
  struct RngSample {
    int seed;
    uint32_t next16, next32;
  };
  constexpr std::array<RngSample, 4> samples = {{{0, 50036, 0x30ddeff1u},
                                                 {1, 50040, 0x30deeff5u},
                                                 {1234, 46652, 0x2d8fbab9u},
                                                 {65535, 4592, 0x047c226du}}};
  for (const auto& s : samples)
    if (!check(th12_rng16(s.seed) == s.next16 && th12_rng32(s.seed) == s.next32,
               "Native RNG ABI differs from original samples"))
      return 1;

  // Resource ownership and the same actual game core are exercised without DATs.
  std::vector<uint8_t> ecl(100);
  const auto word = [&](size_t at, uint32_t v) { std::memcpy(ecl.data() + at, &v, 4); };
  const auto half = [&](size_t at, uint16_t v) { std::memcpy(ecl.data() + at, &v, 2); };
  word(0, 0x54504353);
  half(4, 1);
  half(16, 1);
  word(36, 48);
  std::memcpy(ecl.data() + 40, "main", 5);
  word(48, 0x484c4345);
  half(68, 83);
  half(70, 20);
  ecl[74] = 63;
  ecl[75] = 1;
  word(80, 1000);
  half(88, 10);
  half(90, 16);
  ecl[94] = 63;
  if (!check(th12_load_ecl(ecl.data(), int(ecl.size())) == 1, "Native C++ ECL load failed"))
    return 1;
  ecl.clear();
  ecl.shrink_to_fit(); // Program owns its copy, not the caller's buffer.
  th12_start(0, 0, 1, 1234);
  for (int i = 0; i < 30; ++i)
    th12_tick(0, 0);
  const std::string before = th12_state();
  for (int i = 0; i < 20; ++i) {
    th12_draw();
    th12_anm_draw();
    th12_laser_draw();
    th12_mesh_draw();
    th12_score_popups();
    th12_hud();
  }
  if (!check(before == th12_state(), "Native draw queries advanced game state"))
    return 1;
  if (!check(th12_hud()[0] == 30 && th12_hud_size() == 41 && th12_anm_draw_stride() == 128 &&
                 th12_laser_draw_stride() == 64 && th12_mesh_draw_stride() == 56 &&
                 th12_mesh_vertex_stride() == 28,
             "Native ABI dimensions differ"))
    return 1;

  th12::laser::Manager lasers;
  {
    // Original42a328..42a360, precision32: this squared distance is exactly
    //58^2 after per-operation rounding; the next float outward is outside.
    struct CircleSample {float x,y,r;bool hit;};
    const CircleSample boundary[]={{43.16087341308594f,89.25546264648438f,58,true},
      {43.1608772277832f,89.25546264648438f,58,false},
      {43.16086959838867f,89.25546264648438f,58,true},
      {3,132,5,true},{0,128,0,true}};
    for(const auto& sample:boundary){
      th12::laser::Manager circle;th12::laser::World circleWorld;
      th12::laser::Parameters p;p.position={sample.x-8,sample.y,0};
      p.angle=0;p.initialLength=p.maxLength=16;p.width=14;p.speed=0;
      circle.spawn(th12::laser::Kind::Moving,p,circleWorld);
      if(!check(bool(circle.clearCircle(circleWorld,{0,128,0},sample.r))==sample.hit,
        "Laser circle edge differs from actual precision32 predicate"))return 1;
    }
  }
  {
    // Real replay21 fragment67256 inherits offset32; after one native update
    // its position is[-34.6570511,363.327087]. Timed fragments clear the offset.
    th12::laser::World fragmentWorld;
    th12::laser::Manager fragments;
    th12::laser::Parameters source;
    source.position={0,128,0};source.angle=1.994135856628418f;
    source.initialOffset=32;source.initialLength=source.maxLength=128;
    source.travelDistance=512;source.width=14;source.speed=3;
    int parent=fragments.spawn(th12::laser::Kind::Moving,source,fragmentWorld);
    auto* child=fragments.appendMovingFragment(*fragments.find(parent),{-20.278789520263672f,331.41680908203125f,0},48,fragmentWorld);
    const int childId=child->state.serialId;
    fragments.tick(fragmentWorld);child=fragments.find(childId);
    if(!check(child&&child->state.parameters.initialOffset==32&&
        std::abs(child->state.position.x- -34.65705108642578f)<.00003f&&
        std::abs(child->state.position.y-363.32708740234375f)<.00003f,
        "Moving fragment lost the actual parent's constructor offset"))return 1;
    fragments.reset();source.delay=8;source.grow=3;source.hold=30;source.shrink=5;source.lookupId=123;
    parent=fragments.spawn(th12::laser::Kind::Timed,source,fragmentWorld);
    child=fragments.appendMovingFragment(*fragments.find(123),{20,120,0},48,fragmentWorld);
    if(!check(child&&child->state.parameters.initialOffset==0&&child->state.position.x==20&&child->state.position.y==120,
        "Timed fragment incorrectly inherited a Moving-only constructor offset"))return 1;
  }
  th12::laser::World world;
  th12::laser::Parameters p;
  p.position = {0, 100, 0};
  p.width = 32;
  p.initialLength = 10;
  p.maxLength = 100;
  p.speed = 2;
  p.hold = 20;
  p.grow = 10;
  p.delay = 8;
  p.shrink = 12;
  p.nodeCount = 8;
  for (const auto kind :
       {th12::laser::Kind::Moving, th12::laser::Kind::Timed, th12::laser::Kind::Curve})
    lasers.spawn(kind, p, world);
  if (!check(lasers.size() == 3, "Native laser classes failed to construct"))
    return 1;
  for (int i = 0; i < 5; ++i)
    lasers.tick(world);
  if (!check(lasers.size() == 3, "Native laser lifecycle failed"))
    return 1;
  std::puts("Native C++ ABI, resource ownership, read-only presentation and laser classes passed.");
  return 0;
}
