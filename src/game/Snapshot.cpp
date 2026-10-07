// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"
#include "LaserSnapshot.hpp"

namespace th12 {
// Read-only, semantic candidate state. This is not original-game evidence.
// Included inside th12 after entity definitions; never used by simulation.
std::string snapshotBuffer;
std::string messageSnapshotBuffer;
std::string spellSnapshotBuffer;
std::string backgroundSnapshotBuffer;
void snapshotHex(std::ostream& out, const std::string& value) {
  static constexpr char digits[] = "0123456789abcdef";
  out << '"';
  for (uint8_t c : value)
    out << digits[c >> 4] << digits[c & 15];
  out << '"';
}
void snapshotTextRecord(std::ostream& out, const message_system::TextRecord& r) {
  out << "{\"textHex\":";
  snapshotHex(out, r.bytes);
  out << ",\"offset\":" << r.offset << ",\"spacing\":" << r.spacing << ",\"style\":" << r.style
      << ",\"color\":" << r.color << '}';
}
void snapshotMessage(std::ostream& out) {
  out << "{\"entry\":" << messageVM.entry << ",\"active\":" << messageVM.active
      << ",\"clock\":" << messageVM.clock << ",\"wait\":" << messageVM.wait
      << ",\"pc\":" << messageVM.pc << ",\"flags\":" << messageVM.flags
      << ",\"age\":" << messageVM.age << ",\"speaker\":" << messageVM.speaker
      << ",\"line\":" << messageVM.line << ",\"textInitialized\":" << messageVM.textInitialized
      << ",\"textHex\":[";
  snapshotHex(out, messageVM.text[0]);
  out << ',';
  snapshotHex(out, messageVM.text[1]);
  out << "],\"textRecords\":[";
  bool comma = false;
  for (const auto& r : messageVM.textRecords) {
    if (comma)
      out << ',';
    comma = true;
    snapshotTextRecord(out, r);
  }
  out << "],\"textActions\":[";
  comma = false;
  for (const auto& a : messageVM.textActions) {
    if (comma)
      out << ',';
    comma = true;
    out << "{\"slot\":" << a.slot << ",\"interrupt\":" << a.interrupt << ",\"record\":";
    snapshotTextRecord(out, a.record);
    out << '}';
  }
  out << "],\"animationHandles\":[";
  for (size_t i = 0; i < messageAnimationKeys.size(); i++) {
    if (i)
      out << ',';
    out << messageAnimationKeys[i];
  }
  out << "],\"events\":[";
  comma = false;
  for (const auto& e : messageEvents) {
    if (comma)
      out << ',';
    comma = true;
    out << '[' << e.op << ',' << e.frame << ',';
    snapshotHex(out, std::string(e.payload.begin(), e.payload.end()));
    out << ']';
  }
  out << "]}";
}
const std::string& captureMessage() {
  std::ostringstream out;
  snapshotMessage(out);
  messageSnapshotBuffer = out.str();
  return messageSnapshotBuffer;
}
std::pair<std::string, int> contextLocation(const u8* pc) {
  if (!pc)
    return {"", -1};
  const uintptr_t at = reinterpret_cast<uintptr_t>(pc);
  uintptr_t best = 0;
  std::string name;
  for (const auto& file : program.files) {
    const uintptr_t first = reinterpret_cast<uintptr_t>(file.data());
    if (at < first || at >= first + file.size())
      continue;
    for (const auto& [routine, start] : program.routines) {
      const uintptr_t pos = reinterpret_cast<uintptr_t>(start);
      if (pos >= first && pos < first + file.size() && pos <= at && pos >= best) {
        best = pos;
        name = routine;
      }
    }
  }
  return {name, best ? int(at - best) : -1};
}
template <class T> void snapshotArray(std::ostream& out, const T& values) {
  out << '[';
  bool comma = false;
  for (const auto& value : values) {
    if (comma)
      out << ',';
    comma = true;
    out << value;
  }
  out << ']';
}
void snapshotStageTrack(std::ostream& out, const stage_logic::Interpolation& t) {
  out << "{\"duration\":" << t.duration << ",\"mode\":" << t.mode << ",\"age\":" << t.age
      << ",\"time\":" << t.time << ",\"from\":";
  snapshotArray(out, t.from);
  out << ",\"to\":";
  snapshotArray(out, t.to);
  out << ",\"a\":";
  snapshotArray(out, t.a);
  out << ",\"b\":";
  snapshotArray(out, t.b);
  out << '}';
}
void snapshotBackground(std::ostream& out, bool full = false) {
  const auto& s = stageState;
  const auto& c = s.camera;
  out << "{\"frame\":" << backgroundFrame << ",\"visible\":" << backgroundVisible
      << ",\"loaded\":" << (!stageProgram.commands.empty()) << ",\"flags\":" << s.flags
      << ",\"clock\":" << s.clock << ",\"previousClock\":" << s.previousClock << ",\"pc\":" << s.pc
      << ",\"callbackCount\":" << s.callbackCount << ",\"camera\":{\"position\":";
  snapshotArray(out, c.position);
  out << ",\"direction\":";
  snapshotArray(out, c.direction);
  out << ",\"up\":";
  snapshotArray(out, c.up);
  out << ",\"eyeOffset\":";
  snapshotArray(out, c.eyeOffset);
  out << ",\"offset\":";
  snapshotArray(out, c.offset);
  out << ",\"velocity\":";
  snapshotArray(out, c.velocity);
  out << ",\"fov\":" << c.fov << ",\"fog\":[" << ((c.fogArgb >> 16) & 255) / 255.f << ','
      << ((c.fogArgb >> 8) & 255) / 255.f << ',' << (c.fogArgb & 255) / 255.f << ',' << c.fogStart
      << ',' << c.fogEnd << "]},\"poses\":[";
  bool comma = false;
  for (const auto& q : stageProgram.primitives)
    if (auto* node = animationScene.find(stagePrimitiveKey(q.slot))) {
      const auto& a = node->vm.state();
      if (comma)
        out << ',';
      comma = true;
      out << '[' << q.slot << ',' << a.sprite << ',' << a.x + a.engineX + a.offsetX << ','
          << a.y + a.engineY + a.offsetY << ',' << a.z + a.engineZ + a.offsetZ << ',' << a.rx << ','
          << a.ry << ',' << a.rotation << ',' << a.sx << ',' << a.sy << ',' << a.u << ',' << a.v
          << ',' << a.uvScaleX << ',' << a.uvScaleY << ',' << a.primary << ',' << a.secondary << ','
          << a.flags << ',' << a.flags2 << ',' << a.mode << ',' << a.blend << ',' << a.layer << ','
          << a.spriteWidth << ',' << a.spriteHeight << ']';
    }
  out << ']';
  if (full) {
    out << ",\"transitionAge\":" << s.transitionAge << ",\"clockTime\":" << s.clockTime
        << ",\"shake\":[" << s.shakeMode << ',' << s.shakeAge << ',' << s.shakeTime
        << "],\"renderHint\":" << s.renderHint << ",\"backgroundMode\":" << s.backgroundMode
        << ",\"fogBytes\":";
    snapshotArray(out, c.fogBytes);
    out << ",\"tracks\":[";
    snapshotStageTrack(out, s.positionTrack);
    out << ',';
    snapshotStageTrack(out, s.directionTrack);
    out << ',';
    snapshotStageTrack(out, s.upTrack);
    out << ',';
    snapshotStageTrack(out, s.fogTrack);
    out << ']';
  }
  out << '}';
}
const std::string& captureBackground() {
  std::ostringstream out;
  out << std::setprecision(9);
  snapshotBackground(out);
  backgroundSnapshotBuffer = out.str();
  return backgroundSnapshotBuffer;
}
void snapshotWords(std::ostream& out, const std::array<u32, 256>& words) {
  out << '[';
  bool comma = false;
  for (size_t i = 0; i < words.size(); i++)
    if (words[i]) {
      if (comma)
        out << ',';
      comma = true;
      out << '[' << i << ',' << words[i] << ']';
    }
  out << ']';
}
void snapshotProgram(std::ostream& out, const eb::Program& p) {
  out << "{\"first\":" << p.first << ",\"flags\":" << p.flags << ",\"sound\":" << p.transformSound
      << ",\"records\":[";
  bool comma = false;
  for (size_t i = 0; i < p.records.size(); i++) {
    const auto& r = p.records[i];
    if (!r.kind)
      continue;
    if (comma)
      out << ',';
    comma = true;
    out << '[' << i << ',' << r.a << ',' << r.b << ',' << r.c << ',' << r.d << ',' << r.kind << ','
        << r.concurrent << ']';
  }
  out << "]}";
}
void snapshotMotion(std::ostream& out, const eb::Motion& m) {
  out << '[' << m.a << ',' << m.b << ',' << m.vx << ',' << m.vy << ',' << m.timer << ','
      << m.duration << ',' << m.limit << ',' << m.count << ']';
}
void snapshotTrailPoint(std::ostream& out, const pw::TrailPoint& p) {
  out << '[' << p.x << ',' << p.y << ',' << p.color << ']';
}
void snapshotBombVisual(std::ostream& out, const pw::BombVisual& v) {
  out << '[' << v.id << ',' << v.bank << ',' << v.script << ',' << v.age << ',' << v.interrupt
      << ',' << v.x << ',' << v.y << ',' << v.angle << ',' << v.active << ']';
}
void snapshotBomb(std::ostream& out) {
  const auto& b = playerBomb;
  out << "{\"loadout\":" << b.loadout << ",\"active\":" << b.active << ",\"age\":" << b.age
      << ",\"previousAge\":" << b.previousAge << ",\"tailAge\":" << b.tailAge << ",\"position\":["
      << b.x << ',' << b.y << "],\"speedMultiplier\":" << b.speedMultiplier << ",\"visuals\":[";
  bool comma = false;
  for (const auto& v : b.visuals) {
    if (comma)
      out << ',';
    comma = true;
    snapshotBombVisual(out, v);
  }
  out << "],\"primaryVisuals\":[";
  comma = false;
  for (const auto& v : b.primaryVisuals) {
    if (comma)
      out << ',';
    comma = true;
    snapshotBombVisual(out, v);
  }
  out << "],\"retiredVisuals\":[";
  comma = false;
  for (const auto& v : b.retiredVisuals) {
    if (comma)
      out << ',';
    comma = true;
    out << '[';
    snapshotBombVisual(out, v.visual);
    out << ',' << v.remaining << ']';
  }
  out << "],\"marisaB\":[" << b.marisaB.id << ',' << b.marisaB.active << ',' << b.marisaB.visible
      << ',' << b.marisaB.age << ',' << b.marisaB.visualAge << ',' << b.marisaB.interrupt << ','
      << b.marisaB.x << ',' << b.marisaB.y << ',' << b.marisaB.clear.enabled << ','
      << b.marisaB.clear.x << ',' << b.marisaB.clear.y << ',' << b.marisaB.clear.width << ','
      << b.marisaB.clear.height << "],\"reimuAEmitter\":[" << b.reimuAEmitter.id << ','
      << b.reimuAEmitter.age << ',' << b.reimuAEmitter.first << ',' << b.reimuAEmitter.second << ','
      << b.reimuAEmitter.firstStep << ',' << b.reimuAEmitter.secondStep << ','
      << b.reimuAEmitter.active << "],\"reimuABeams\":[";
  comma = false;
  for (const auto& v : b.reimuABeams) {
    if (comma)
      out << ',';
    comma = true;
    out << "{\"state\":[" << v.id << ',' << v.sourceId << ',' << v.age << ',' << v.sourceSlot << ','
        << v.active << ',' << v.x << ',' << v.y << ',' << v.heading << ',' << v.anchorX
        << "],\"history\":[";
    bool inner = false;
    for (const auto& p : v.history) {
      if (inner)
        out << ',';
      inner = true;
      snapshotTrailPoint(out, p);
    }
    out << "]}";
  }
  out << "],\"reimuBOrbs\":[";
  comma = false;
  for (const auto& v : b.reimuBOrbs) {
    if (comma)
      out << ',';
    comma = true;
    out << '[' << v.id << ',' << v.sourceId << ',' << v.index << ',' << v.age << ',' << v.sourceSlot
        << ',' << v.tailAge << ',' << v.active << ',' << v.polar << ',' << v.x << ',' << v.y << ','
        << v.centerX << ',' << v.centerY << ',' << v.heading << ',' << v.radius << ',' << v.speed
        << ',' << v.deltaX << ',' << v.deltaY << ']';
  }
  out << "],\"leaves\":[";
  comma = false;
  for (const auto& v : b.leaves) {
    if (comma)
      out << ',';
    comma = true;
    out << "{\"state\":[" << v.id << ',' << v.age << ',' << v.heading << ',' << v.active
        << "],\"points\":[";
    bool inner = false;
    for (const auto& p : v.points) {
      if (inner)
        out << ',';
      inner = true;
      snapshotTrailPoint(out, p);
    }
    out << "]}";
  }
  out << "],\"trails\":[";
  comma = false;
  for (const auto& v : b.trails) {
    if (comma)
      out << ',';
    comma = true;
    out << "{\"state\":[" << v.id << ',' << v.kind << ',' << v.width << "],\"points\":[";
    bool inner = false;
    for (const auto& p : v.points) {
      if (inner)
        out << ',';
      inner = true;
      snapshotTrailPoint(out, p);
    }
    out << "]}";
  }
  out << "],\"rectangles\":[";
  comma = false;
  for (const auto& v : b.rectangles) {
    if (comma)
      out << ',';
    comma = true;
    out << '[' << v.enabled << ',' << v.x << ',' << v.y << ',' << v.width << ',' << v.height << ']';
  }
  out << "],\"circles\":[";
  comma = false;
  for (const auto& v : b.circles) {
    if (comma)
      out << ',';
    comma = true;
    out << '[' << v.x << ',' << v.y << ',' << v.radius << ']';
  }
  out << "]}";
}
void snapshotSpellResult(std::ostream& out, const spell_system::Result& r) {
  out << "{\"kind\":" << int(r.kind) << ",\"failure\":" << int(r.failure)
      << ",\"cardId\":" << r.cardId << ",\"bonusDisplayed\":" << r.bonusDisplayed
      << ",\"scoreDeltaStored\":" << r.scoreDeltaStored << ",\"elapsedTicks\":" << r.elapsedTicks
      << ",\"survivedTimeout\":" << r.survivedTimeout << ",\"titleHex\":";
  snapshotHex(out, r.title);
  out << ",\"records\":[" << r.loadoutRecord.attempts << ',' << r.loadoutRecord.captures << ','
      << r.allRecord.attempts << ',' << r.allRecord.captures << "]}";
}
void snapshotSpell(std::ostream& out) {
  const auto& s = spellState;
  out << "{\"flags\":" << s.flags << ",\"cardId\":" << s.cardId << ",\"age\":" << s.age
      << ",\"previousAge\":" << s.previousAge << ",\"timeLimit\":" << s.timeLimit
      << ",\"serialTicks\":" << s.serialTicks << ",\"bonusDisplayed\":" << s.bonusDisplayed
      << ",\"initialBonusDisplayed\":" << s.initialBonusDisplayed << ",\"ring\":[" << s.ringX << ','
      << s.ringY << "],\"replay\":" << s.replay << ",\"timedOut\":" << s.timedOut
      << ",\"failure\":" << int(s.failure) << ",\"titleHex\":";
  snapshotHex(out, s.title);
  out << ",\"records\":[" << s.loadoutRecord.attempts << ',' << s.loadoutRecord.captures << ','
      << s.allRecord.attempts << ',' << s.allRecord.captures << "],\"lastResult\":";
  snapshotSpellResult(out, s.lastResult);
  out << '}';
}
const std::string& captureSpell() {
  std::ostringstream out;
  snapshotSpell(out);
  spellSnapshotBuffer = out.str();
  return spellSnapshotBuffer;
}
void snapshotAnimations(std::ostream& out) {
  out << "{\"chains\":[";
  for (int i = 0; i < 3; i++) {
    if (i)
      out << ',';
    out << '[';
    bool comma = false;
    for (const auto* node : animationScene.chain(i == 0   ? anm_logic::Membership::Primary
                                                 : i == 1 ? anm_logic::Membership::Secondary
                                                          : anm_logic::Membership::Embedded)) {
      if (comma)
        out << ',';
      comma = true;
      out << node->id;
    }
    out << ']';
  }
  out << "],\"nodes\":[";
  std::vector<u32> ids;
  for (const auto& [id, node] : animationScene.nodes())
    ids.push_back(id);
  std::sort(ids.begin(), ids.end());
  bool comma = false;
  for (u32 id : ids) {
    const auto& n = *animationScene.nodes().at(id);
    const auto& s = n.vm.state();
    const auto d = n.vm.diagnostics();
    if (comma)
      out << ',';
    comma = true;
    out << "{\"id\":" << id << ",\"key\":" << n.key << ",\"owner\":" << n.owner
        << ",\"parent\":" << n.parent << ",\"membership\":" << int(n.membership)
        << ",\"alive\":" << n.alive << ",\"detached\":" << n.detached << ",\"viewport\":["
        << n.viewportX << ',' << n.viewportY << "],\"bank\":" << s.bank
        << ",\"script\":" << s.script << ",\"clock\":" << s.clock << ",\"ticks\":" << s.ticks
        << ",\"integers\":";
    snapshotArray(out, s.integers);
    out << ",\"floats\":";
    snapshotArray(out, s.floats);
    out << ",\"pose\":[" << s.x << ',' << s.y << ',' << s.z << ',' << s.engineX << ',' << s.engineY
        << ',' << s.engineZ << ',' << s.offsetX << ',' << s.offsetY << ',' << s.offsetZ << ','
        << s.rx << ',' << s.ry << ',' << s.rotation << ',' << s.sx << ',' << s.sy << ',' << s.u
        << ',' << s.v << ',' << s.uvScaleX << ',' << s.uvScaleY << ',' << s.geometryRadius << ','
        << s.geometryWidth << "],\"render\":[" << s.primary << ',' << s.secondary << ',' << s.flags
        << ',' << s.flags2 << ',' << s.sprite << ',' << s.layer << ',' << s.mode << ',' << s.anchorX
        << ',' << s.anchorY << ',' << s.blend << ',' << s.geometryCount << "],\"lifecycle\":["
        << s.visible << ',' << s.held << ',' << s.ended << ',' << s.deleteRequested << ','
        << s.displayDomain << "],\"execution\":[" << d.pc << ',' << d.byteOffset << ',' << d.time
        << ',' << d.pendingInterrupt << ',' << d.savedPC << ',' << d.savedByteOffset << ','
        << d.savedTimer << ',' << d.hasSavedTimer << "],\"rotationVelocity\":";
    snapshotArray(out, d.rotationVelocity);
    out << ",\"scaleVelocity\":";
    snapshotArray(out, d.scaleVelocity);
    out << ",\"scrollVelocity\":";
    snapshotArray(out, d.scrollVelocity);
    out << ",\"interpolations\":[";
    bool inner = false;
    for (const auto& t : d.interpolations) {
      if (inner)
        out << ',';
      inner = true;
      out << "{\"active\":" << t.active << ",\"mode\":" << t.mode << ",\"time\":" << t.time
          << ",\"duration\":" << t.duration << ",\"fields\":";
      snapshotArray(out, t.fields);
      out << ",\"from\":";
      snapshotArray(out, t.from);
      out << ",\"to\":";
      snapshotArray(out, t.to);
      out << ",\"a\":";
      snapshotArray(out, t.a);
      out << ",\"b\":";
      snapshotArray(out, t.b);
      out << '}';
    }
    out << "]}";
  }
  out << "],\"unsupported\":[";
  std::vector<u32> keys;
  for (const auto& [key, count] : animationUnsupported)
    keys.push_back(key);
  std::sort(keys.begin(), keys.end());
  comma = false;
  for (auto key : keys) {
    if (comma)
      out << ',';
    comma = true;
    out << '[' << (key >> 16) << ',' << (key & 65535) << ',' << animationUnsupported.at(key) << ']';
  }
  out << "]}";
}
const std::string& captureState() {
  std::ostringstream out;
  out << std::setprecision(9);
  out << "{\"referenceFaults\":[";
  std::vector<uint64_t> faultKeys;
  for (const auto& [key, count] : referenceFaults)
    faultKeys.push_back(key);
  std::sort(faultKeys.begin(), faultKeys.end());
  bool faultComma = false;
  for (const auto key : faultKeys) {
    if (faultComma)
      out << ',';
    faultComma = true;
    out << '[' << (key >> 48) << ',' << ((key >> 40) & 255) << ',' << ((key >> 32) & 1) << ','
        << i32(u32(key)) << ',' << referenceFaults.at(key) << ']';
  }
  out << "],\"runtimeFaults\":[";
  faultKeys.clear();
  for (const auto& [key, count] : runtimeFaults)
    faultKeys.push_back(key);
  std::sort(faultKeys.begin(), faultKeys.end());
  faultComma = false;
  for (const auto key : faultKeys) {
    if (faultComma)
      out << ',';
    faultComma = true;
    out << '[' << (key >> 32) << ',' << u32(key) << ',' << runtimeFaults.at(key) << ']';
  }
  out << "],\"bossHud\":" << captureBossHud()
      << ",\"music\":{\"track\":" << stageNumber * 2 - (bossMusic ? 0 : 1)
      << ",\"serial\":" << musicSerial << "},\"fixtureFlags\":" << oracleFixtureFlags
      << ",\"frame\":" << frame << ",\"input\":{\"held\":" << previousHeld
      << "},\"player\":{\"xFixed\":" << px << ",\"yFixed\":" << py << ",\"state\":" << playerState
      << ",\"stateAge\":" << playerStateTicks << ",\"invulnerability\":" << invuln
      << ",\"deathWindow\":" << deathWindow << ",\"graze\":" << graze
      << ",\"focusAnimation\":" << playerFocusAnimation << ",\"fireFrame\":" << shotSchedule.frame
      << ",\"character\":" << character << ",\"shot\":" << shot << ",\"body\":["
      << playerMotion.bodyAnimation << ',' << playerMotion.bodyAge << ',' << playerMotion.previousDx
      << "],\"options\":[";
  for (int i = 0; i < playerMotion.count; i++) {
    if (i)
      out << ',';
    const auto& o = playerMotion.options[i];
    out << '[' << o.x << ',' << o.y << ',' << o.age << ']';
  }
  out << "],\"bomb\":[" << playerBomb.loadout << ',' << playerBomb.active << ',' << playerBomb.age
      << ',' << playerBomb.previousAge << ',' << playerBomb.x << ',' << playerBomb.y
      << "],\"damageSources\":[";
  bool comma = false;
  for (const auto& s : playerDamageSources.sources)
    if (s.flags & 1) {
      if (comma)
        out << ',';
      comma = true;
      out << '[' << s.id << ',' << s.x << ',' << s.y << ',' << s.radius << ',' << s.growth << ','
          << s.width << ',' << s.height << ',' << s.widthGrowth << ',' << s.heightGrowth << ','
          << s.angle << ',' << s.ticks << ',' << s.previousTicks << ',' << s.damage << ','
          << s.accumulated << ',' << s.limit << ',' << s.cadence << ',' << s.flags << ']';
    }
  out << "],\"damageSourceNextId\":" << playerDamageSources.nextId << ",\"bombState\":";
  snapshotBomb(out);
  out << "},\"message\":";
  snapshotMessage(out);
  out << ",\"spellState\":";
  snapshotSpell(out);
  out << ",\"background\":";
  snapshotBackground(out, true);
  out << ",\"animations\":";
  snapshotAnimations(out);
  out << ",\"rng\":{\"seed\":" << random.seed << ",\"calls\":" << random.calls
      << ",\"displaySeed\":" << displayRandom.seed << ",\"displayCalls\":" << displayRandom.calls
      << "},\"ecl\":[";
  comma = false;
  for (const auto& p : enemies)
    for (const auto& c : p->contexts) {
      if (comma)
        out << ',';
      comma = true;
      const auto location = contextLocation(c.pc);
      out << "{\"enemy\":" << p->id << ",\"id\":" << c.id
          << ",\"routine\":" << std::quoted(location.first) << ",\"byteOffset\":" << location.second
          << ",\"time\":" << c.time << ",\"locals\":";
      snapshotWords(out, c.locals);
      out << ",\"stack\":[";
      bool inner = false;
      for (const auto& v : c.stack) {
        if (inner)
          out << ',';
        inner = true;
        out << '[' << v.integer << ',' << v.bits << ']';
      }
      out << "],\"calls\":[";
      inner = false;
      for (const auto& call : c.calls) {
        if (inner)
          out << ',';
        inner = true;
        const auto l = contextLocation(call.pc);
        out << "{\"routine\":" << std::quoted(l.first) << ",\"byteOffset\":" << l.second
            << ",\"time\":" << call.time << ",\"locals\":";
        snapshotWords(out, call.locals);
        out << '}';
      }
      out << "]}";
    }
  out << "],\"enemies\":[";
  comma = false;
  for (const auto& p : enemies) {
    const auto& e = *p;
    if (comma)
      out << ',';
    comma = true;
    out << "{\"id\":" << e.id << ",\"position\":[" << e.x << ',' << e.y << "],\"absolute\":["
        << e.ax << ',' << e.ay << ',' << e.angle << ',' << e.speed << "],\"relative\":[" << e.rx
        << ',' << e.ry << ',' << e.relativeAngle << ',' << e.relativeSpeed
        << "],\"life\":" << e.life << ",\"maxLife\":" << e.maxLife << ",\"age\":" << e.age
        << ",\"phaseAge\":" << e.phaseAge << ",\"flags\":" << e.flags
        << ",\"lifeFlags\":" << e.lifeFlags << ",\"lifeRaw\":" << e.lifeRaw
        << ",\"phaseLife\":" << e.phaseLife << ",\"lifeThreshold\":" << e.lifeThreshold
        << ",\"bossSlot\":" << e.bossSlot << ",\"active\":" << e.active << ",\"boss\":" << e.boss
        << ",\"hidden\":" << e.hidden << ",\"invincible\":" << e.invincible
        << ",\"bodyImmunity\":" << e.bodyImmunityTicks << ",\"renderExtents\":[" << e.renderWidth
        << ',' << e.renderHeight << "],\"immunity\":" << e.immunityTicks
        << ",\"mirror\":" << e.mirror << ",\"timedOut\":" << e.timedOut << ",\"ufo\":" << e.isUfo
        << ",\"hitbox\":[" << e.hitWidth << ',' << e.hitHeight << ',' << e.bodyWidth << ','
        << e.bodyHeight << "],\"variables\":";
    snapshotArray(out, e.variables);
    out << ",\"drops\":[" << e.drop << ',' << e.score << ',' << e.dropSpreadX << ','
        << e.dropSpreadY << "],\"dropCounts\":";
    snapshotArray(out, e.dropCounts);
    out << ",\"interrupts\":[";
    for (size_t i = 0; i < e.interrupts.size(); i++) {
      if (i)
        out << ',';
      const auto& s = e.interrupts[i];
      out << '[' << s.health << ',' << s.time << ',' << std::quoted(s.healthScript) << ','
          << std::quoted(s.timeoutScript) << ']';
    }
    out << "],\"movement\":[" << e.fromX << ',' << e.fromY << ',' << e.toX << ',' << e.toY << ','
        << e.fromA << ',' << e.toA << ',' << e.fromS << ',' << e.toS << ',' << e.positionTicks
        << ',' << e.positionTime << ',' << e.positionMode << ',' << e.polarTicks << ','
        << e.polarTime << ',' << e.polarMode << ',' << e.relativeFromX << ',' << e.relativeFromY
        << ',' << e.relativeToX << ',' << e.relativeToY << ',' << e.relativeFromA << ','
        << e.relativeToA << ',' << e.relativeFromS << ',' << e.relativeToS << ','
        << e.relativePositionTicks << ',' << e.relativePositionTime << ',' << e.relativePositionMode
        << ',' << e.relativePolarTicks << ',' << e.relativePolarTime << ',' << e.relativePolarMode
        << ',' << e.clampX << ',' << e.clampY << ',' << e.clampWidth << ',' << e.clampHeight
        << "],\"circles\":[";
    for (int i = 0; i < 2; i++) {
      if (i)
        out << ',';
      const auto& m = i ? e.relativeCircle : e.absoluteCircle;
      out << '[' << m.enabled << ',' << m.centerX << ',' << m.centerY << ',' << m.radius << ','
          << m.radialSpeed << ',' << m.fromOmega << ',' << m.toOmega << ',' << m.fromRadius << ','
          << m.toRadius << ',' << m.fromRadial << ',' << m.toRadial << ',' << m.ticks << ','
          << m.time << ',' << m.mode << ']';
    }
    out << "],\"shooters\":[";
    for (size_t i = 0; i < e.shooters.size(); i++) {
      if (i)
        out << ',';
      const auto& s = e.shooters[i];
      out << "{\"values\":[" << s.x << ',' << s.y << ',' << s.originX << ',' << s.originY << ','
          << s.radius << ',' << s.angle << ',' << s.spacing << ',' << s.speed << ',' << s.slow
          << ',' << s.count << ',' << s.layers << ',' << s.mode << ',' << s.type << ',' << s.color
          << ',' << s.fireSound << ',' << s.fixedOrigin << "],\"extensions\":";
      snapshotProgram(out, s.extensions);
      out << '}';
    }
    out << "]}";
  }
  out << "],\"bullets\":[";
  comma = false;
  for (const auto& b : bullets) {
    if (comma)
      out << ',';
    comma = true;
    out << "{\"id\":" << b.entityId << ",\"friendly\":" << b.friendly << ",\"position\":[" << b.x
        << ',' << b.y << "],\"motion\":[" << b.angle << ',' << b.speed << "],\"type\":" << b.type
        << ",\"color\":" << b.color << ",\"age\":" << b.age << ",\"active\":" << b.active;
    if (b.friendly)
      out << ",\"weapon\":[" << int(b.weapon.phase) << ',' << b.weapon.previousAge << ','
          << b.weapon.hit << ',' << b.weapon.auxiliaryHit << ',' << b.damage << ',' << b.hitWidth
          << ',' << b.hitHeight << ',' << b.option << ',' << b.update << ',' << b.targetId << ','
          << b.extraCallback << ',' << b.hitCallback << ',' << b.spawnCallback << ']';
    else {
      const auto& s = b.enemy;
      out << ",\"hostile\":[" << s.vx << ',' << s.vy << ',' << int(s.phase) << ',' << s.collisionAge
          << ',' << s.grazed << ',' << s.rotateToMotion << ',' << s.spawnDuration << ','
          << s.cancelAge << ',' << s.collisionDelay << ',' << s.offscreenGrace << ',' << s.active
          << ',' << s.cursor << "],\"extensions\":";
      snapshotProgram(out, s.program);
      out << ",\"transforms\":[";
      snapshotMotion(out, s.fast);
      out << ',';
      snapshotMotion(out, s.acceleration);
      out << ',';
      snapshotMotion(out, s.polar);
      out << ',';
      snapshotMotion(out, s.turn);
      out << ',';
      snapshotMotion(out, s.bounce);
      out << ',';
      snapshotMotion(out, s.wait);
      out << ',';
      snapshotMotion(out, s.protect);
      out << "],\"gate\":[" << s.gate.remaining << ',' << s.gate.value << ']';
    }
    out << '}';
  }
  out << "],\"lasers\":";
  snapshotLasers(out);
  out << ",\"items\":[";
  comma = false;
  for (size_t i = 0; i < itemManager.entities.size(); i++) {
    const auto& e = itemManager.entities[i];
    if (!e.active())
      continue;
    if (comma)
      out << ',';
    comma = true;
    out << '[' << e.id << ',' << i << ',' << e.type << ',' << int(e.state) << ',' << e.position.x
        << ',' << e.position.y << ',' << e.velocity.x << ',' << e.velocity.y << ',' << e.age << ','
        << e.colorTimer << ',' << e.delay << ',' << e.elapsed << ',' << e.colorElapsed << ','
        << e.attractionSpeed << ',' << e.aggregateSpeed << ',' << e.near << ',' << e.aggregateKind
        << ',' << e.bucketA << ',' << e.bucketB << ',' << e.multiplier << ']';
  }
  out << "],\"economy\":{\"resources\":[" << resources.power << ',' << resources.maxPower << ','
      << resources.powerStep << ',' << resources.scoreStored << ',' << resources.pointValueStored
      << ',' << resources.maxPointValueStored << ',' << resources.lives << ',' << resources.bombs
      << ',' << resources.lifeFragments << ',' << resources.bombFragments << ',' << resources.rank
      << "],\"ufoColors\":";
  snapshotArray(out, ufoManager.inventory.colors);
  out << ",\"inventory\":[" << ufoManager.inventory.count << ','
      << ufoManager.inventory.displayState << ',' << ufoManager.inventory.gateTimer << ','
      << ufoManager.inventory.combination << "],\"ufo\":[" << int(ufoManager.phase) << ','
      << ufoManager.enemyId << ',' << ufoManager.kind << ',' << ufoManager.age << ','
      << ufoManager.duration << ',' << ufoManager.elapsed << ',' << ufoManager.fill << ','
      << ufoManager.bucketA << ',' << ufoManager.bucketB << ',' << ufoManager.position.x << ','
      << ufoManager.position.y << ',' << ufoManager.rewardIssued << "],\"itemPool\":["
      << itemManager.nextId << ',' << itemManager.tokenSideCounter << ','
      << itemManager.smallPointCursor << ',' << itemManager.smallPointSpawns
      << "]},\"stage\":{\"number\":" << stageNumber << ",\"difficulty\":" << difficulty
      << ",\"dialogue\":" << dialogue << ",\"spell\":" << spell << ",\"ended\":" << ended
      << ",\"eventBits\":" << eventBits << ",\"nextIds\":[" << nextEnemyId << ','
      << nextProjectileId << "],\"globals\":[";
  comma = false;
  std::vector<int> keys;
  for (const auto& entry : globals)
    keys.push_back(entry.first);
  std::sort(keys.begin(), keys.end());
  for (int key : keys) {
    if (comma)
      out << ',';
    comma = true;
    const auto v = globals.at(key);
    out << '[' << key << ',' << v.integer << ',' << v.bits << ']';
  }
  out << "],\"unsupported\":[";
  comma = false;
  for (size_t i = 0; i < unsupported.size(); i++)
    if (unsupported[i]) {
      if (comma)
        out << ',';
      comma = true;
      out << '[' << i << ',' << unsupported[i] << ']';
    }
  out << "]}}";
  snapshotBuffer = out.str();
  return snapshotBuffer;
}

} // namespace th12
