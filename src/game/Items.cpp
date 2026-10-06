#include "Items.hpp"
#include <algorithm>
#include <cmath>

namespace th12::item_system {
float add(float a, float b) {
  return float(double(a) + b);
}

Vec2 polar(float angle, float speed) {
  return {float(std::cos(double(angle)) * speed), float(std::sin(double(angle)) * speed)};
}

PlayerView PlayerView::native(int character, Vec2 position, bool focus, int state) {
  constexpr float normal[3] = {30, 35, 30}, focused[3] = {50, 59, 50}, speed[3] = {5, 7, 6};
  PlayerView p;
  p.character = std::clamp(character, 0, 2);
  p.position = position;
  p.focus = focus;
  p.state = state;
  p.pickupHalfWidth = p.pickupHalfHeight = normal[p.character];
  p.nearHalfWidth = p.nearHalfHeight = (focus ? focused : normal)[p.character];
  p.attractionSpeed = speed[p.character];
  return p;
}

void Manager::reset(int side) {
  for (auto& e : entities)
    e = Entity{};
  nextId = 1;
  tokenSideCounter = side;
  smallPointCursor = smallPointSpawns = 0;
  poolMisses = unsupportedTypes = 0;
  events.clear();
}

Entity* Manager::find(uint32_t id) {
  for (auto& e : entities)
    if (e.id == id && e.active())
      return &e;
  return nullptr;
}

const Entity* Manager::find(uint32_t id) const {
  for (const auto& e : entities)
    if (e.id == id && e.active())
      return &e;
  return nullptr;
}

size_t Manager::activeCount() const {
  size_t n = 0;
  for (const auto& e : entities)
    n += e.active();
  return n;
}

Entity* Manager::spawn(const SpawnRequest& r) {
  if (r.type < 1 || r.type > 18) {
    if (r.type)
      ++unsupportedTypes;
    return nullptr;
  }
  // Native increments before searching the 16-slot pool, including failed token births.
  if (r.type >= 10 && r.type <= 15)
    ++tokenSideCounter;
  Entity* e = nullptr;
  if (r.type == 9) {
    e = &entities[ordinaryCapacity + tokenCapacity + smallPointCursor];
    ++smallPointSpawns;
    const int ordinal = smallPointCursor;
    smallPointCursor = (smallPointCursor + 1) & 2047;
    if (e->active()) {
      ++poolMisses;
      return nullptr;
    }
    *e = {};
    e->state = State::DelayedPoint;
    e->delay = smallPointSpawns >= 1024  ? (ordinal & 31) + 16
               : smallPointSpawns >= 512 ? (ordinal & 15) + 8
               : smallPointSpawns >= 256 ? (ordinal & 7) + 4
                                         : ordinal & 3;
  } else {
    const size_t first = r.type >= 10 ? ordinaryCapacity : 0,
                 last = r.type >= 10 ? ordinaryCapacity + tokenCapacity : ordinaryCapacity;
    for (size_t i = first; i < last; i++)
      if (!entities[i].active()) {
        e = &entities[i];
        break;
      }
    if (!e) {
      ++poolMisses;
      return nullptr;
    }
    *e = {};
    e->state = r.type >= 16 ? State::Aggregate : r.type >= 10 ? State::Token : State::Falling;
  }
  e->id = nextId++;
  e->type = r.type;
  e->position = r.position;
  if (r.type != 9)
    e->position.x = std::clamp(e->position.x, -192.f, 192.f);
  if (r.type >= 16)
    e->position.y = std::max(0.f, e->position.y);
  if (r.type >= 10 && r.type <= 15)
    e->velocity = polar((tokenSideCounter & 1) ? float(pi * .75) : float(pi * .25), 1.5f);
  else if (r.type < 16)
    e->velocity = polar(r.angle, r.speed);
  e->visualScript = r.type + 165;
  e->aggregateKind = r.aggregateKind;
  e->bucketA = r.bucketA;
  e->bucketB = r.bucketB;
  e->multiplier = r.multiplier;
  return e;
}

Entity* Manager::spawn(int type, Vec2 position, float angle, float speed) {
  return spawn(SpawnRequest{type, position, angle, speed});
}

void Manager::tick(const FrameView& frame, const EventSink& sink) {
  events.clear();
  smallPointSpawns = 0; // Native425c37 resets the batch counter before pool traversal.
  const auto emit = [&](EventKind kind, const Entity& entity, int value = 0) {
    Event event{kind, entity, value};
    events.push_back(event);
    if (sink)
      sink(event);
  };
  const float rate = frame.rate;
  const auto& p = frame.player;
  const auto move = [&](Entity& e) {
    e.position.x = add(e.position.x, float(double(e.velocity.x) * rate));
    e.position.y = add(e.position.y, float(double(e.velocity.y) * rate));
  };
  const auto home = [&](Entity& e, float speed, Vec2 target) {
    const float dx = float(double(target.x) - e.position.x),
                dy = float(double(target.y) - e.position.y);
    const float angle = dx == 0 && dy == 0 ? pi / 2 : float(std::atan2(double(dy), double(dx)));
    e.velocity = polar(angle, speed);
    move(e);
  };
  const auto interrupt = [&](Entity& e, int value) {
    e.visualInterrupt = value;
    e.visualInterruptAge = 0;
    ++e.visualInterruptVersion;
    emit(EventKind::AnimationInterrupt, e, value);
  };
  const auto hint = [&](Entity& e, bool active, int value = 0) {
    if (e.hintActive == active)
      return;
    e.hintActive = active;
    if (active) {
      e.hintInterrupt = value;
      ++e.hintGeneration;
      emit(EventKind::HintCreated, e, value);
    } else
      emit(EventKind::HintRemoved, e);
  };
  const auto setAge = [](Entity& e, float value) {
    e.elapsed = value;
    e.age = int(value);
  };
  const auto setColor = [](Entity& e, float value) {
    e.colorElapsed = value;
    e.colorTimer = int(value);
  };
  for (auto& e : entities) {
    if (!e.active())
      continue;
    if (e.state == State::DelayedPoint) {
      if (--e.delay < 0) {
        e.state = State::Automatic;
        e.visualAge = 0;
        emit(EventKind::AnimationRebind, e, e.visualScript);
      }
      continue;
    }
    if (e.state == State::Falling &&
        ((p.state != 2 && p.state != 4 && p.position.y < 128) || frame.dialogue)) {
      e.state = State::CollectedAbove;
      e.attractionSpeed = p.attractionSpeed;
    }
    if ((e.state == State::Falling || e.state == State::Automatic ||
         e.state == State::CollectedAbove || e.state == State::CollectedNear ||
         e.state == State::Token) &&
        frame.ufo.active && frame.ufo.age >= 60 && frame.ufo.kind != 0 &&
        (e.type == 1 || e.type == 2)) {
      e.state = State::Absorbing;
      e.velocity = {};
      e.attractionSpeed = 0;
    }
    if (e.state == State::Falling || e.state == State::Automatic) {
      move(e);
      e.velocity.y = add(e.velocity.y, float(double(.03f) * rate));
      if (e.state == State::Falling && e.velocity.y >= 0)
        e.velocity.x = 0;
      e.velocity.y = std::min(e.velocity.y, 2.f);
      if (e.state == State::Automatic && e.velocity.y >= 0) {
        e.state = State::CollectedAbove;
        e.attractionSpeed = p.attractionSpeed;
        home(e, e.attractionSpeed, p.position);
        if (e.attractionSpeed < 12)
          e.attractionSpeed = add(e.attractionSpeed, .2f);
        if (p.state == 4) {
          e.velocity = {};
          e.state = State::Falling;
        }
      }
      if (e.position.y >= 472) {
        const bool automatic = e.state == State::Automatic;
        e.state = State::Inactive;
        if (automatic)
          emit(EventKind::RankDelta, e, -4);
        continue;
      }
    } else if (e.state == State::CollectedAbove || e.state == State::CollectedNear) {
      home(e, e.attractionSpeed, p.position);
      if (e.attractionSpeed < 12)
        e.attractionSpeed = add(e.attractionSpeed, .2f);
      if (p.state == 4) {
        e.velocity = {};
        e.state = State::Falling;
      }
    } else if (e.state == State::Aggregate) {
      home(e, e.aggregateSpeed, p.position);
      if (e.age >= 60 && e.aggregateSpeed < 12)
        e.aggregateSpeed = add(e.aggregateSpeed, .2f);
      if (p.state == 4) {
        e.velocity = {};
        e.state = State::Falling;
      }
    } else if (e.state == State::Token) {
      if (frame.dialogue)
        setAge(e, 10000);
      if (e.type >= 13 && e.type <= 15) {
        const double dx = double(e.position.x) - p.position.x,
                     dy = double(e.position.y) - p.position.y;
        const float distance = float(dx * dx + dy * dy);
        if (distance <= 3600) {
          setColor(e, add(e.colorElapsed, -rate));
          setAge(e, add(e.elapsed, -rate));
          if (e.colorTimer == 90)
            setColor(e, add(e.colorElapsed, -rate));
          if (!e.near) {
            e.near = true;
            interrupt(e, 7);
          }
        } else if (e.near) {
          interrupt(e, e.colorTimer >= 90 ? 2 : 3);
          e.near = false;
        }
        if (e.colorTimer == 90)
          interrupt(e, 2);
        else if (e.colorTimer >= 150) {
          hint(e, false);
          e.type = 13 + (e.type - 12) % 3;
          e.visualScript = e.type + 165;
          e.visualAge = 0;
          e.visualInterrupt = 0;
          emit(EventKind::AnimationRebind, e, e.visualScript);
          setColor(e, 0);
        }
        setColor(e, add(e.colorElapsed, rate));
      }
      move(e);
      if (e.age < 1200) {
        if ((e.position.x <= -176 && e.velocity.x < 0) || (e.position.x >= 176 && e.velocity.x > 0))
          e.velocity.x = -e.velocity.x;
        if ((e.position.y <= 160 && e.velocity.y < 0) || (e.position.y >= 370 && e.velocity.y > 0))
          e.velocity.y = -e.velocity.y;
      } else if (e.position.x <= -208 || e.position.x >= 208 || e.position.y <= -16 ||
                 e.position.y >= 464) {
        e.state = State::Inactive;
        emit(EventKind::RankDelta, e, -4);
        hint(e, false);
        continue;
      }
      bool completion = false;
      int completionInterrupt = 0;
      if (frame.inventoryColors && (!frame.inventoryCount || *frame.inventoryCount < 3)) {
        const auto& colors = *frame.inventoryColors;
        const int color = (e.type - 10) % 3 + 1;
        const bool same = colors[0] == color && colors[1] == color;
        const bool rainbow = colors[0] != colors[1] && colors[0] != color && colors[1] != color;
        completion = colors[0] && colors[1] && (same || rainbow);
        completionInterrupt =
            rainbow ? 10 : color + 6; // Native426832: all rainbow hints use211 IRQ10.
      }
      hint(e, completion, completionInterrupt);
    } else if (e.state == State::Absorbing) {
      if (!frame.ufo.active) {
        e.velocity = {};
        e.state = State::Falling;
      } else {
        home(e, e.attractionSpeed, frame.ufo.position);
        if (e.attractionSpeed < 6)
          e.attractionSpeed =
              add(e.attractionSpeed, float(double(frame.ufo.age) / 600 * .05f + .02f));
        const float dx = float(double(frame.ufo.position.x) - e.position.x),
                    dy = float(double(frame.ufo.position.y) - e.position.y);
        if (float(double(dx) * dx + double(dy) * dy) < 324) {
          const Entity old = e;
          e.state = State::Inactive;
          emit(EventKind::Absorbed, old, e.type);
          continue;
        }
      }
    }
    const float dx = std::abs(e.position.x - p.position.x),
                dy = std::abs(e.position.y - p.position.y);
    if (p.state != 2 && dx <= p.pickupHalfWidth && dy <= p.pickupHalfHeight) {
      const Entity old = e;
      e.state = State::Inactive;
      e.hintActive = false;
      emit(old.type >= 16   ? EventKind::CollectAggregate
           : old.type >= 10 ? EventKind::CollectToken
                            : EventKind::CollectOrdinary,
           old, old.type >= 10 && old.type <= 15 ? (old.type - 10) % 3 : old.type);
      // Native426dce queuesIRQ1 after pickup;211's4tick exit can outlive the slot.
      if (old.hintActive)
        emit(EventKind::HintCollected, old, 1);
      continue;
    }
    if (e.state != State::CollectedAbove && e.state != State::CollectedNear &&
        e.state != State::Token && e.state != State::Absorbing && e.state != State::Aggregate &&
        dx < p.nearHalfWidth && dy < p.nearHalfHeight) {
      e.state = State::CollectedNear;
      e.attractionSpeed = float(double(p.attractionSpeed) / 3);
    }
    if (frame.animate)
      frame.animate(e);
    setAge(e, add(e.elapsed, rate));
    ++e.visualAge;
    ++e.visualInterruptAge;
  }
}

ResourceState ResourceState::initial(int difficulty) {
  constexpr int base[5] = {5000, 10000, 15000, 20000, 20000},
                cap[5] = {100000, 200000, 300000, 400000, 500000};
  ResourceState r;
  r.difficulty = std::clamp(difficulty, 0, 4);
  r.pointValueStored = base[r.difficulty] * 100;
  r.maxPointValueStored = cap[r.difficulty] * 100;
  return r;
}

int ResourceState::pointValue() const {
  return (pointValueStored / 100) / 10 * 10;
}

void ResourceState::score(int displayed) {
  scoreStored = int(std::min<int64_t>(999999999, int64_t(scoreStored) + displayed / 10));
}

void ResourceState::addRank(int n) {
  rank = std::clamp(rank + n, -1024, 1024);
}

void ResourceState::addPointValue(int stored) {
  pointValueStored = std::min(maxPointValueStored, pointValueStored + stored);
}

RewardDelta difference(const ResourceState& before, const ResourceState& after) {
  return {after.scoreStored - before.scoreStored,
          after.power - before.power,
          after.pointValueStored - before.pointValueStored,
          after.lives - before.lives,
          after.bombs - before.bombs,
          after.lifeFragments - before.lifeFragments,
          after.bombFragments - before.bombFragments,
          after.rank - before.rank};
}

void smallPower(ResourceState& r) {
  if (r.power >= r.maxPower) {
    r.score(10);
    return;
  }
  const int previous = r.power;
  r.power = std::min(r.maxPower, r.power + 1);
  if (previous / r.powerStep != r.power / r.powerStep)
    r.addRank(12);
}

RewardDelta collectOrdinary(ResourceState& r, const Entity& item, const PlayerView& player) {
  const ResourceState before = r;
  const auto increasePower = [&](int amount, int rank) {
    const int previous = r.power;
    r.power = std::min(r.maxPower, r.power + amount);
    if (previous / r.powerStep != r.power / r.powerStep)
      r.addRank(rank);
  };
  switch (item.type) {
  case 1:
    smallPower(r);
    break;
  case 2: {
    int value = r.pointValue();
    if (player.position.y > 128 && item.state != State::CollectedAbove) {
      value = value * 3 / 4;
      value -= ((int(player.position.y) - 128) * value) / 450;
    }
    r.score(std::max(10, value / 10 * 10));
    break;
  }
  case 3:
    if (r.power >= r.maxPower)
      r.addPointValue(20000);
    else
      increasePower(r.powerStep, 24);
    break;
  case 4:
    if (r.lives >= 9)
      r.lifeFragments = 0;
    else {
      r.lifeFragments += r.lifeFragments ? 1 : 2;
      if (r.lifeFragments >= 5) {
        r.lifeFragments = 0;
        r.lives = std::min(8, r.lives + 1);
      }
    }
    r.addRank(256);
    break;
  case 5:
    if (r.bombs >= 9)
      r.bombFragments = 0;
    else {
      r.bombFragments += 2;
      if (r.bombFragments >= 5) {
        r.bombFragments = 0;
        r.bombs = std::min(9, r.bombs + 1);
      }
    }
    r.addRank(256);
    break;
  case 6:
    r.lives = std::min(8, r.lives + 1);
    r.addRank(256);
    break;
  case 7:
    r.bombs = std::min(9, r.bombs + 1);
    r.addRank(256);
    break;
  case 8:
    if (r.power >= r.maxPower)
      r.addPointValue(10000);
    else
      increasePower(r.maxPower, 12);
    break;
  case 9:
    r.addPointValue(10);
    r.score(10);
    break;
  default:
    break;
  }
  return difference(before, r);
}

RewardDelta collectAggregate(ResourceState& r, const Entity& item) {
  const ResourceState before = r;
  for (int i = 0; i < item.bucketA; i++)
    smallPower(r);
  const int value = r.pointValue();
  int displayed;
  if (item.aggregateKind == 1)
    displayed = int(double(value * item.bucketA) * item.multiplier) + value * item.bucketB;
  else
    displayed = int(double(value * item.bucketB) * item.multiplier);
  r.score(std::max(10, displayed / 10 * 10));
  return difference(before, r);
}

DeathDropPlan deathPowerDrops(ResourceState& resources, Vec2 position) {
  // Original player state 2, timer==3: 436de6 / 439440 / 436e3d.
  const ResourceState before = resources;
  resources.power = std::max(resources.powerStep, resources.power - resources.powerStep);
  DeathDropPlan plan;
  plan.immediate = difference(before, resources);
  const float base = float(std::atan2(-224.0, -double(position.x)));
  for (int i = 0; i < 7; i++)
    plan.spawns.push_back(
        SpawnRequest{1, position, float(double(base) + double(i) * pi / 28 - double(pi / 8)), 3.f});
  return plan;
}

} // namespace th12::item_system
