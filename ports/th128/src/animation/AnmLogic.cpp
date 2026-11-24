// Reconstructed TH12 1.00b ANM logical interpreter.
#include "AnmLogic.hpp"

namespace th128::anm {

std::shared_ptr<Bank> Registry::bank(int id) const {
  auto i = banks_.find(id);
  return i == banks_.end() ? nullptr : i->second;
}

namespace {
int operandCount(int opcode) {
  static constexpr int counts[] = {
      0, 0, 0, 1, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
      2, 2, 2, 2, 2, 2, 2, 1, 3, 3, 2, 1, 3, 3, 2, 2, 5, 5, 3, 5,
      4, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 3, 1, 5, 3,
      1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 3, 3, 0, 1,
      10, 1, 2, 2, 2, 2, 2, 4, 2, 2, 2, 1, 1, 3, 1};
  return opcode == -1 ? 0 : opcode >= 0 && opcode <= 114 ? counts[opcode] : -1;
}
}

bool Registry::loadBank(int id, const uint8_t* bytes, size_t length, std::string& error) {
  constexpr size_t MaxEntries = 4096, MaxSprites = 65536, MaxScripts = 16384;
  constexpr size_t MaxCommands = 200000, MaxWords = 1000000;
  auto fail = [&](const char* why) {
    error = why;
    return false;
  };
  if (!bytes || length < 64 || length > 128u * 1024u * 1024u)
    return fail("ANM size");
  if (banks_.size() >= 256 && !bank(id))
    return fail("ANM registry bank budget");
  auto u16 = [&](size_t offset) {
    return uint16_t(uint16_t(bytes[offset]) | uint16_t(bytes[offset + 1]) << 8);
  };
  auto u32 = [&](size_t offset) {
    return uint32_t(bytes[offset]) | uint32_t(bytes[offset + 1]) << 8 |
           uint32_t(bytes[offset + 2]) << 16 | uint32_t(bytes[offset + 3]) << 24;
  };
  struct Region {
    size_t begin, end;
  };
  auto result = std::make_shared<Bank>();
  size_t base = 0, entries = 0, commandCount = 0, wordCount = 0;
  for (;;) {
    if (++entries > MaxEntries)
      return fail("ANM entry budget");
    if (base > length - 64 || u32(base) != 7)
      return fail("ANM v7 entry");
    const size_t sprites = u16(base + 4), scripts = u16(base + 6);
    const size_t next = u32(base + 36), texture = u32(base + 28);
    const bool hasTexture = u16(base + 32) != 0;
    const size_t extent = next ? next : length - base;
    const size_t tableEnd = 64 + sprites * 4 + scripts * 8;
    if (extent > length - base || extent < 64 || tableEnd > extent || (next & 3))
      return fail("ANM entry extent/tables");
    if (scripts > 4096 || scripts > MaxScripts - result->scripts.size() ||
        sprites > MaxSprites - result->sprites.size())
      return fail("ANM aggregate object budget");
    auto fits = [&](size_t offset, size_t size) {
      return offset >= tableEnd && offset <= extent && size <= extent - offset;
    };
    std::vector<Region> regions;
    regions.reserve(sprites + scripts + 2);
    const size_t name = u32(base + 16);
    if (!fits(name, 1))
      return fail("ANM name offset");
    size_t nameEnd = name;
    while (nameEnd < extent && nameEnd - name < 4096 && bytes[base + nameEnd])
      ++nameEnd;
    if (nameEnd == extent || nameEnd - name == 4096)
      return fail("ANM unterminated/oversized name");
    regions.push_back({name, nameEnd + 1});
    if (hasTexture) {
      if ((texture & 3) || !fits(texture, 16) || u32(base + texture) != 0x58544854u ||
          u16(base + texture + 4) != 0)
        return fail("ANM THTX header");
      const auto format = u16(base + texture + 6);
      const size_t width = u16(base + texture + 8), height = u16(base + texture + 10);
      const size_t size = u32(base + texture + 12);
      const size_t depth = format == 1 || format == 65535 ? 4 :
                           format == 3 || format == 5 ? 2 : format == 7 ? 1 : 0;
      if (!depth || !width || !height || width > 8192 || height > 8192 ||
          width * height * depth > size || !fits(texture + 16, size))
        return fail("ANM THTX format/dimensions/payload");
      regions.push_back({texture, texture + 16 + size});
    } else if (texture) {
      return fail("ANM texture offset without texture");
    }
    for (size_t sprite = 0; sprite < sprites; ++sprite) {
      const size_t offset = u32(base + 64 + sprite * 4);
      if ((offset & 3) || !fits(offset, 20))
        return fail("ANM sprite offset");
      regions.push_back({offset, offset + 20});
      const float width = asFloat(u32(base + offset + 12));
      const float height = asFloat(u32(base + offset + 16));
      const float x = asFloat(u32(base + offset + 4)), y = asFloat(u32(base + offset + 8));
      const float textureWidth = float(std::max(1, int(u16(base + 10))));
      const float textureHeight = float(std::max(1, int(u16(base + 12))));
      SpriteInfo info{width, height, x / textureWidth, y / textureHeight,
                      float((double(x) + width) / textureWidth),
                      float((double(y) + height) / textureHeight), int(entries - 1)};
      if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(width) ||
          !std::isfinite(height) || !std::isfinite(info.u0) || !std::isfinite(info.v0) ||
          !std::isfinite(info.u1) || !std::isfinite(info.v1))
        return fail("ANM non-finite sprite geometry");
      result->sprites.push_back(info);
    }
    for (size_t script = 0; script < scripts; ++script) {
      const size_t offset = u32(base + 64 + sprites * 4 + script * 8 + 4);
      if ((offset & 3) || !fits(offset, 8))
        return fail("ANM script offset");
      regions.push_back({offset, offset});
    }
    std::sort(regions.begin(), regions.end(), [](const Region& first, const Region& second) {
      return first.begin < second.begin;
    });
    for (size_t region = 1; region < regions.size(); ++region)
      if (regions[region - 1].begin == regions[region].begin ||
          regions[region - 1].end > regions[region].begin)
        return fail("ANM aliased/overlapping resource offsets");
    for (size_t scriptIndex = 0; scriptIndex < scripts; ++scriptIndex) {
      const size_t table = base + 64 + sprites * 4 + scriptIndex * 8;
      const size_t offset = u32(table + 4), start = base + offset;
      const auto region = std::lower_bound(regions.begin(), regions.end(), offset,
          [](const Region& candidate, size_t value) { return candidate.begin < value; });
      const auto successor = std::next(region);
      const size_t limit = base + (successor == regions.end() ? extent : successor->begin);
      Script script;
      script.sourceId = int32_t(u32(table));
      size_t position = start;
      for (;;) {
        if (position > limit || limit - position < 8)
          return fail("ANM unterminated/overlapping script");
        if (++commandCount > MaxCommands)
          return fail("ANM aggregate command budget");
        const uint16_t opcode = u16(position), size = u16(position + 2);
        Command command;
        command.op = int16_t(opcode);
        command.time = int16_t(u16(position + 4));
        command.mask = u16(position + 6);
        command.offset = uint32_t(position - start);
        if (opcode == 65535) {
          if ((size != 0 && size != 8) || command.mask)
            return fail("ANM sentinel header");
        } else {
          if (size < 8 || (size - 8) % 4 || size > limit - position)
            return fail("ANM command length/boundary");
          const size_t words = (size - 8) / 4;
          const int expected = operandCount(command.op);
          if (expected >= 0 && words != size_t(expected))
            return fail("ANM opcode operand count");
          if (words < 16 && (command.mask >> words))
            return fail("ANM operand mask outside payload");
          if (words > MaxWords - wordCount)
            return fail("ANM aggregate operand budget");
          wordCount += words;
          command.words.reserve(words);
          for (size_t word = 0; word < words; ++word)
            command.words.push_back(u32(position + 8 + word * 4));
        }
        script.offsets.emplace(command.offset, script.commands.size());
        script.commands.push_back(std::move(command));
        if (opcode == 65535)
          break;
        position += size;
      }
      result->scripts.push_back(std::move(script));
    }
    if (!next)
      break;
    base += next;
  }
  size_t registryCommands = commandCount, registryWords = wordCount;
  size_t registryScripts = result->scripts.size(), registrySprites = result->sprites.size();
  for (const auto& entry : banks_) {
    if (entry.first == id)
      continue;
    registryScripts += entry.second->scripts.size();
    registrySprites += entry.second->sprites.size();
    for (const auto& script : entry.second->scripts) {
      registryCommands += script.commands.size();
      for (const auto& command : script.commands)
        registryWords += command.words.size();
    }
  }
  if (registryCommands > 1000000 || registryWords > 4000000 ||
      registryScripts > 65536 || registrySprites > 262144)
    return fail("ANM aggregate registry budget");
  banks_[id] = std::move(result);
  error.clear();
  return true;
}

void VM::report(Runtime& r, int op, const char* why) {
  if (faulted_)
    return;
  faulted_ = true;
  faultOpcode_ = op < -1 ? int(uint16_t(op)) : op;
  const auto offset = script_ && pc_ < script_->commands.size() ? script_->commands[pc_].offset : 0;
  faultReason_ = std::string(why) + "; byte offset=" + std::to_string(offset);
  state_.ended = true;
  state_.held = false;
  if (r.unsupported)
    r.unsupported(state_.bank, state_.script, faultOpcode_, faultReason_.c_str());
}

uint32_t VM::random32(Runtime& r, bool forceGame) {
  const bool display = r.nativeDisplayRandomSelector
      ? (state_.flags2 & 2u) != 0 : state_.displayDomain;
  auto& f = display && !forceGame ? r.displayRandom32 : r.gameRandom32;
  if (!f) {
    report(r, -1, "missing RNG callback");
    return 0;
  }
  return f();
}

float VM::unit(Runtime& r) {
  return float(double(random32(r)) * 0x1p-32);
}

float VM::signedUnit(Runtime& r) {
  return float(double(random32(r)) * 0x1p-31 - 1.);
}

void VM::selectSprite(int sprite, Runtime& runtime) {
  if (faulted_)
    return;
  if (runtime.remapSprite)
    sprite = runtime.remapSprite(sprite);
  // 455e19..455e38 and the corresponding random-sprite path select a
  // different sprite resource for every negative result. Script execution
  // stays in its original bank; replacing state.bank would break children.
  state_.spriteBank = sprite < 0 ? runtime.whiteSpriteBank : state_.bank;
  state_.sprite = sprite < 0 ? runtime.whiteSprite : sprite;
  if (auto bank = registry_->bank(state_.spriteBank);
      bank && state_.sprite >= 0 && state_.sprite < int(bank->sprites.size())) {
    state_.spriteWidth = bank->sprites[state_.sprite].width;
    state_.spriteHeight = bank->sprites[state_.sprite].height;
  }
  state_.visible = true;
  state_.flags |= 1;
}

int VM::integerIndex(int id) {
  return id >= 10000 && id <= 10003 ? id - 10000 : id == 10008 ? 4 : id == 10009 ? 5 : -1;
}

float* VM::floatVariable(int id) {
  if (id >= 10004 && id <= 10007)
    return &state_.floats[id - 10004];
  switch (id) {
  case 10013:
    return &state_.x;
  case 10014:
    return &state_.y;
  case 10015:
    return &state_.z;
  case 10023:
    return &state_.rx;
  case 10024:
    return &state_.ry;
  case 10025:
    return &state_.rotation;
  default:
    return nullptr;
  }
}

int32_t VM::readInt(Command& c, size_t i, Runtime& r) {
  if (faulted_)
    return 0;
  if (i >= c.words.size()) {
    report(r, c.op, "missing argument");
    return 0;
  }
  int32_t n = int32_t(c.words[i]);
  if (!(c.mask & (1u << i)))
    return n;
  int local = integerIndex(n);
  if (local >= 0)
    return state_.integers[local];
  if (n >= 10004 && n <= 10007)
    return nativeInt(state_.floats[n - 10004]);
  // Native int remap table 0x4554ac maps 10010..10021 to raw literals;
  // only int builtin 10022 consumes RNG. Float builtins differ.
  return n == 10022 ? int32_t(random32(r)) : n;
}

float VM::readFloat(Command& c, size_t i, Runtime& r) {
  if (faulted_)
    return 0;
  if (i >= c.words.size()) {
    report(r, c.op, "missing argument");
    return 0;
  }
  float n = asFloat(c.words[i]);
  if (!(c.mask & (1u << i)))
    return n;
  const int id = nativeInt(n), local = integerIndex(id);
  if (local >= 0)
    return float(state_.integers[local]);
  if (auto* p = floatVariable(id))
    return *p;
  if (id == 10010)
    return float(signedUnit(r) * Pi);
  if (id == 10011)
    return unit(r);
  if (id == 10012)
    return signedUnit(r);
  if (id >= 10016 && id <= 10021) {
    const auto value=id<=10018?r.cameraPosition[id-10016]:r.cameraEyeOffset[id-10019];
    if(!std::isfinite(value)){report(r,-1,"camera operand context is unavailable");return 0;}
    return value;
  }
  if (id == 10022)
    return float(double(random32(r)));
  return n;
}

uint32_t* VM::integerDestination(Command& c, size_t i) {
  if (i >= c.words.size())
    return nullptr;
  int id = int32_t(c.words[i]);
  const int local = (c.mask & (1u << i)) ? integerIndex(id) : -1;
  // memcpy at the call sites avoids aliasing an int through uint32_t*.
  return local >= 0 ? reinterpret_cast<uint32_t*>(&state_.integers[local]) : &c.words[i];
}

void VM::writeInt(Command& c, size_t i, int32_t n) {
  if (faulted_)
    return;
  if (auto* p = integerDestination(c, i))
    std::memcpy(p, &n, 4);
}

int32_t VM::destinationInt(Command& c, size_t i) {
  int32_t n = 0;
  if (auto* p = integerDestination(c, i))
    std::memcpy(&n, p, 4);
  return n;
}

void VM::writeFloat(Command& c, size_t i, float n) {
  if (faulted_)
    return;
  if (i >= c.words.size())
    return;
  if (c.mask & (1u << i)) {
    if (auto* p = floatVariable(nativeInt(asFloat(c.words[i])))) {
      if (p == &state_.x || p == &state_.y || p == &state_.z ||
          p == &state_.rx || p == &state_.ry || p == &state_.rotation)
        ++presentationRevision_;
      *p = n;
      return;
    }
  }
  c.words[i] = asWord(n);
}

float VM::destinationFloat(Command& c, size_t i) {
  if (i >= c.words.size())
    return 0;
  if (c.mask & (1u << i)) {
    if (auto* p = floatVariable(nativeInt(asFloat(c.words[i]))))
      return *p;
  }
  return asFloat(c.words[i]);
}

float VM::field(Field f) const {
  switch (f) {
  case X:
    return state_.flags & 0x200 ? state_.offsetX : state_.x;
  case Y:
    return state_.flags & 0x200 ? state_.offsetY : state_.y;
  case Z:
    return state_.flags & 0x200 ? state_.offsetZ : state_.z;
  case RX:
    return state_.rx;
  case RY:
    return state_.ry;
  case RZ:
    return state_.rotation;
  case SX:
    return state_.sx;
  case SY:
    return state_.sy;
  case UVEL:
    return scrollVelocity_[0];
  case VVEL:
    return scrollVelocity_[1];
  case USCALE:
    return state_.uvScaleX;
  case VSCALE:
    return state_.uvScaleY;
  default:
    break;
  }
  uint32_t color = f >= ALPHA2 ? state_.secondary : state_.primary;
  int k = (int(f) - int(f >= ALPHA2 ? ALPHA2 : ALPHA));
  return float((color >> (k == 0 ? 24 : k == 1 ? 16 : k == 2 ? 8 : 0)) & 255);
}

void VM::setField(Field f, float n) {
  switch (f) {
  case X:
    (state_.flags & 0x200 ? state_.offsetX : state_.x) = n;
    return;
  case Y:
    (state_.flags & 0x200 ? state_.offsetY : state_.y) = n;
    return;
  case Z:
    (state_.flags & 0x200 ? state_.offsetZ : state_.z) = n;
    return;
  case RX:
    state_.rx = n;
    return;
  case RY:
    state_.ry = n;
    return;
  case RZ:
    state_.rotation = n;
    return;
  case SX:
    state_.sx = n;
    return;
  case SY:
    state_.sy = n;
    return;
  case UVEL:
    scrollVelocity_[0] = n;
    return;
  case VVEL:
    scrollVelocity_[1] = n;
    return;
  case USCALE:
    state_.uvScaleX = n;
    return;
  case VSCALE:
    state_.uvScaleY = n;
    return;
  default:
    break;
  }
  uint32_t& color = f >= ALPHA2 ? state_.secondary : state_.primary;
  int k = (int(f) - int(f >= ALPHA2 ? ALPHA2 : ALPHA));
  int shift = k == 0 ? 24 : k == 1 ? 16 : k == 2 ? 8 : 0;
  color = (color & ~(255u << shift)) | ((uint32_t(nativeInt(n)) & 255) << shift);
}

void VM::interpolate(std::initializer_list<Field> fields, std::vector<float> to, int duration,
                     int mode, std::vector<float> a, std::vector<float> b) {
  if (faulted_)
    return;
  std::vector<Field> keys(fields);
  auto it = std::find_if(interpolations_.begin(), interpolations_.end(),
                         [&](auto& t) { return t.fields == keys; });
  if (it == interpolations_.end()) {
    interpolations_.push_back({});
    it = std::prev(interpolations_.end());
  }
  *it = {};
  it->fields = keys;
  it->to = std::move(to);
  it->duration = float(duration);
  it->mode = mode;
  it->a = std::move(a);
  it->b = std::move(b);
  for (auto f : keys)
    it->from.push_back(field(f));
  // Native455630 tests the stored duration itself (scale +1e0): zero
  // disables the interpolator without writing its target on later updates.
  it->active = duration != 0;
}

void VM::jump(uint32_t offset, int clock, Runtime& r) {
  auto i = script_->offsets.find(offset);
  if (i == script_->offsets.end()) {
    report(r, pc_ < script_->commands.size() ? script_->commands[pc_].op : 4, "invalid jump");
    state_.ended = true;
    return;
  }
  pc_ = i->second;
  timer_ = float(clock);
  state_.clock = clock;
}

bool VM::handleInterrupt() {
  if (!pendingInterrupt_)
    return false;
  size_t at = script_->commands.size(), fallback = at;
  for (size_t i = 0; i < script_->commands.size(); i++) {
    const auto& c = script_->commands[i];
    if (c.op == 64 && !c.words.empty()) {
      if (int32_t(c.words[0]) == pendingInterrupt_) {
        at = i;
        break;
      }
      if (int32_t(c.words[0]) == -1)
        fallback = i;
    }
  }
  pendingInterrupt_ = 0;
  // 45576d clears the held flag before checking either a matching label or
  // the -1 fallback. An unmatched request still releases this flag.
  state_.flags &= ~0x2000u;
  if (at == script_->commands.size())
    at = fallback;
  if (at == script_->commands.size())
    return false;
  savedPc_ = pc_;
  savedTimer_ = timer_;
  saved_ = true;
  pc_ = at + 1;
  timer_ = float(script_->commands[at].time);
  state_.clock = nativeInt(timer_);
  state_.held = false;
  state_.visible = true;
  state_.flags |= 1;
  return true;
}

bool VM::command(Command& c, Runtime& r) {
  const int expected = operandCount(c.op);
  if ((expected >= 0 && c.words.size() != size_t(expected)) ||
      (c.words.size() < 16 && (c.mask >> c.words.size()))) {
    report(r, c.op, "malformed opcode operands/mask");
    return true;
  }
  auto I = [&](size_t i) { return readInt(c, i, r); };
  auto F = [&](size_t i) { return readFloat(c, i, r); };
  const int op = c.op;
  if ((op >= 56 && op <= 60) || op == 78 || op == 79 || op == 93 || op == 94 || op == 107) {
    const int mode = op == 107 ? int(c.words[1] & 255) : int32_t(c.words[1]);
    if (mode < 0 || mode > 17) {
      report(r, op, "unimplemented interpolation mode");
      return true;
    }
  }
  // Explicit authored writes are boundaries; continuous VM tracks/velocities
  // are advanced elsewhere. Do not read operands again for this sidecar.
  switch (op) {
  case 3: case 48: case 49: case 50: case 51: case 52: case 61: case 62:
  case 65: case 66: case 67: case 68: case 72: case 73: case 76: case 77:
  case 80: case 83: case 84: case 85: case 89: case 101: case 102:
  case 103: case 104: case 105: case 106: case 108: case 110:
    ++presentationRevision_;
    break;
  default: break;
  }
  if (op >= 6 && op <= 27) {
    const bool fp = op & 1, three = op >= 18;
    const int action = three ? (op - 18) / 2 + 1 : (op - 6) / 2;
    if (fp) {
      float a = action == 0 ? 0 : three ? F(1) : destinationFloat(c, 0);
      float b = F(three ? 2 : 1), n = b;
      if (action == 1)
        n = float(double(a) + b);
      else if (action == 2)
        n = float(double(a) - b);
      else if (action == 3)
        n = float(double(a) * b);
      else if (action == 4)
        n = float(double(a) / b);
      else if (action == 5)
        n = float(std::fmod(double(a), double(b)));
      writeFloat(c, 0, n);
    } else {
      int32_t a = action == 0 ? 0
                  : three     ? I(1)
                              : destinationInt(c, 0),
              b = I(three ? 2 : 1), n = b;
      if (action == 1)
        n = int32_t(uint32_t(a) + uint32_t(b));
      else if (action == 2)
        n = int32_t(uint32_t(a) - uint32_t(b));
      else if (action == 3)
        n = int32_t(uint32_t(a) * uint32_t(b));
      else if (action == 4 || action == 5) {
        if (!b || (a == INT32_MIN && b == -1)) {
          report(r, op, "native integer divide exception");
          state_.ended = true;
          return true;
        }
        n = action == 4 ? a / b : a % b;
      }
      writeInt(c, 0, n);
    }
    return false;
  }
  if (op >= 28 && op <= 39) {
    double a, b;
    if (op & 1) {
      a = F(0);
      b = F(1);
    } else {
      a = I(0);
      b = I(1);
    }
    const int kind = (op - 28) / 2;
    bool yes = kind == 0   ? a == b
               : kind == 1 ? a != b
               : kind == 2 ? a < b
               : kind == 3 ? a <= b
               : kind == 4 ? a > b
                           : a >= b;
    if (yes) {
      uint32_t offset = c.words.size() > 2 ? c.words[2] : 0;
      int clock = c.words.size() > 3 ? int32_t(c.words[3]) : 0;
      jump(offset, clock, r);
      return true;
    }
    return false;
  }
  switch (op) {
  case -1:
  case 2:
    state_.ended = true;
    return true;
  case 0:
    break;
  case 1:
    state_.visible = false;
    state_.flags &= ~1u;
    state_.deleteRequested = true;
    state_.ended = true;
    return true;
  case 3:
    selectSprite(I(0), r);
    break;
  case 4: {
    uint32_t off = c.words.size() > 0 ? c.words[0] : 0;
    int clock = c.words.size() > 1 ? int32_t(c.words[1]) : 0;
    jump(off, clock, r);
    return true;
  }
  case 5: {
    int32_t n = int32_t(uint32_t(destinationInt(c, 0)) - 1);
    writeInt(c, 0, n);
    n = I(0);
    if (n > 0) {
      jump(c.words[1], int32_t(c.words[2]), r);
      return true;
    }
    break;
  }
  case 40: {
    uint32_t bound = uint32_t(I(1)), n = bound ? random32(r) % bound : 0;
    writeInt(c, 0, int32_t(n));
    break;
  }
  case 41: {
    float bound = F(1);
    float n = float(double(unit(r)) * bound);
    writeFloat(c, 0, n);
    break;
  }
  case 42:
  case 43: {
    float n = F(1);
    writeFloat(c, 0, float(op == 42 ? std::sin(double(n)) : std::cos(double(n))));
    break;
  }
  case 47: {
    float n = F(0);
    writeFloat(c, 0, normalized(n));
    break;
  }
  case 48: {
    float z = F(2), y = F(1), x = F(0);
    if (state_.flags & 0x200) {
      state_.offsetX = x;
      state_.offsetY = y;
      state_.offsetZ = z;
    } else {
      state_.x = x;
      state_.y = y;
      state_.z = z;
    }
    break;
  }
  case 49:
    state_.rx = F(0);
    state_.ry = F(1);
    state_.rotation = F(2);
    break;
  case 50:
    state_.sx = F(0);
    state_.sy = F(1);
    break;
  case 51:
    setField(ALPHA, float(I(0)));
    break;
  case 52:
  case 76: {
    Field start = op == 52 ? RED : RED2;
    setField(start, float(I(0)));
    setField(Field(start + 1), float(I(1)));
    setField(Field(start + 2), float(I(2)));
    break;
  }
  case 53:
    rotationVelocity_[0] = F(0);
    rotationVelocity_[1] = F(1);
    rotationVelocity_[2] = F(2);
    break;
  case 54:
    scaleVelocity_[0] = F(0);
    scaleVelocity_[1] = F(1);
    break;
  case 55: {
    int duration = I(1), alpha = c.words[0] & 255;
    interpolate({ALPHA}, {float(alpha)}, duration, 0);
    break;
  }
  case 56: {
    int duration = I(0), mode = int32_t(c.words[1]);
    float z = F(4), y = F(3), x = F(2);
    interpolate({X, Y, Z}, {x, y, z}, duration, mode);
    break;
  }
  case 57:
  case 78: {
    int blue = I(4), green = I(3), red = I(2), duration = I(0), mode = int32_t(c.words[1]);
    Field start = op == 57 ? RED : RED2;
    interpolate({start, Field(start + 1), Field(start + 2)},
                {float(red & 255), float(green & 255), float(blue & 255)}, duration, mode);
    break;
  }
  case 58:
  case 79: {
    int alpha = I(2), duration = I(0), mode = int32_t(c.words[1]);
    interpolate({op == 58 ? ALPHA : ALPHA2}, {float(alpha & 255)}, duration, mode);
    break;
  }
  case 59: {
    float z = F(4), y = F(3), x = F(2);
    int duration = I(0), mode = int32_t(c.words[1]);
    interpolate({RX, RY, RZ}, {x, y, z}, duration, mode);
    break;
  }
  case 60: {
    float y = F(3), x = F(2);
    int duration = I(0), mode = int32_t(c.words[1]);
    interpolate({SX, SY}, {x, y}, duration, mode);
    break;
  }
  case 61:
    state_.sx = -state_.sx;
    state_.flags = (state_.flags ^ 0x400u) | 8;
    break;
  case 62:
    state_.sy = -state_.sy;
    state_.flags = (state_.flags ^ 0x800u) | 8;
    break;
  case 63:
    state_.held = true;
    state_.flags |= 0x2000;
    return true;
  case 64:
    break;
  case 65: {
    uint32_t a = c.words[0];
    state_.anchorX = a & 3;
    state_.anchorY = (a >> 16) & 3;
    state_.flags = (state_.flags & ~0xf00000u) | (state_.anchorX << 20) | (state_.anchorY << 22);
    break;
  }
  case 66:
    state_.blend = c.words[0] & 7;
    state_.flags = (state_.flags & ~0xe0u) | (state_.blend << 5);
    break;
  case 67:
    state_.mode = c.words[0] & 31;
    state_.flags = (state_.flags & ~0x1f000000u) | (state_.mode << 24);
    break;
  case 68:
    state_.layer = c.words[0] & 255;
    break;
  case 69:
    state_.visible = false;
    state_.flags &= ~1u;
    state_.held = true;
    state_.flags |= 0x2000;
    return true;
  case 70:
    scrollVelocity_[0] = F(0);
    break;
  case 71:
    scrollVelocity_[1] = F(0);
    break;
  case 72:
    state_.visible = c.words[0] & 1;
    state_.flags = (state_.flags & ~1u) | (state_.visible ? 1u : 0u);
    break;
  case 73:
    state_.flags = (state_.flags & ~0x1000u) | ((c.words[0] & 1) << 12);
    break;
  case 74:
    state_.flags = (state_.flags & ~0x4000u) | ((c.words[0] & 1) << 14);
    break;
  case 75:
    timer_ -= float(I(0));
    state_.clock = nativeInt(timer_);
    break;
  case 77:
    setField(ALPHA2, float(I(0)));
    break;
  case 80:
    state_.flags = (state_.flags & ~0x10000u) | ((c.words[0] & 1) << 16);
    break;
  case 81:
    if (saved_ && savedPc_ < script_->commands.size()) {
      pc_ = savedPc_;
      timer_ = savedTimer_;
      state_.clock = nativeInt(timer_);
      return true;
    } else {
      report(r, op, "return without interrupt");
      state_.ended = true;
      return true;
    }
  case 82:
    state_.flags = (state_.flags & ~0x40000000u) | ((c.words[0] & 1) << 30);
    break;
  case 83:
    state_.x = state_.engineX;
    state_.y = state_.engineY;
    state_.z = state_.engineZ;
    state_.engineX = state_.engineY = state_.engineZ = 0;
    break;
  case 84:
  case 101:
    state_.geometryCount = I(0);
    state_.mode = op == 84 ? 9 : 13;
    break;
  case 85:
    state_.flags = (state_.flags & ~0x80000000u) | ((c.words[0] & 1) << 31);
    break;
  case 86:
    state_.displayDomain = I(0) & 1;
    state_.flags2 = (state_.flags2 & ~1u) | uint32_t(state_.displayDomain);
    break;
  case 87:
    state_.flags2 = (state_.flags2 & ~2u) | ((c.words[0] & 1) << 1);
    break;
  case 88:
  case 90:
  case 91:
  case 92:
  case 95:
  case 96:
  case 97: {
    ChildRequest child;
    child.bank = state_.bank;
    child.script = I(0);
    child.opcode = op;
    child.parent = &state_;
    child.detached = op == 95 || op == 97;
    child.registrationFlags = op == 90 ? 4 : op == 91 ? 2 : op == 92 ? 6 : 0;
    if (child.detached) {
      child.x = state_.engineX;
      child.y = state_.engineY;
      child.z = state_.engineZ;
    }
    // 455c15/455c3c and 455d5f/455d85 write the child's additional
    // position after its synchronous birth tick, not engine position.
    child.hasOffset = op == 96 || op == 97;
    if (r.spawnChild)
      r.spawnChild(child);
    else
      report(r, op, "missing child scheduler");
    if (child.failure)
      report(r, op, child.failure);
    if (faulted_)
      return true;
    if (child.hasOffset) {
      child.offsetX = F(1);
      child.offsetY = F(2);
      if (child.createdState) {
        child.createdState->offsetX = child.offsetX;
        child.createdState->offsetY = child.offsetY;
      } else
        report(r, op, "missing created child state");
    }
    break;
  }
  case 89:
    state_.flags2 = (state_.flags2 & ~4u) | ((c.words[0] & 1) << 2);
    break;
  case 93:
  case 94: {
    float n = F(2);
    int duration = I(0), mode = int32_t(c.words[1]);
    interpolate({op == 93 ? UVEL : VVEL}, {n}, duration, mode);
    break;
  }
  case 100: {
    int duration = I(0);
    float a = F(1), b = F(2), c0 = F(3), x = F(4), y = F(5), z = F(6), d = F(7), e = F(8), f = F(9);
    interpolate({X, Y, Z}, {x, y, z}, duration, 8, {a, b, c0}, {d, e, f});
    break;
  }
  // Unlike the variable readers and op40/41, native 0x455ef1 loads
  // 0x4ce568 directly. Op87 cannot move random-sprite selection into the
  // display domain. Its DIV also consumes RNG before a zero-bound fault.
  case 102: {
    int base = I(0);
    uint32_t bound = uint32_t(I(1)), n = random32(r, true);
    if (!bound) {
      report(r, c.op, "native random-sprite division by zero");
      state_.ended = true;
      break;
    }
    const auto selectedRaw=uint32_t(base)+(n%bound);
    int32_t selected;
    std::memcpy(&selected,&selectedRaw,sizeof(selected));
    selectSprite(selected, r);
    break;
  }
  case 103:
  case 110:
    state_.mode = op == 103 ? 14 : 20;
    state_.geometryRadius = F(0);
    state_.geometryWidth = F(1);
    break;
  case 104:
  case 105:
    state_.mode = op == 104 ? 16 : 17;
    state_.geometryRadius = F(0);
    state_.geometryCount = I(1);
    break;
  case 106:
    state_.uvScaleX = F(0);
    state_.uvScaleY = F(1);
    state_.flags |= 0x10;
    break;
  case 107: {
    float y = F(3), x = F(2);
    int duration = I(0), mode = c.words[1] & 255;
    interpolate({USCALE, VSCALE}, {x, y}, duration, mode);
    state_.flags |= 0x10;
    break;
  }
  case 108:
    state_.mode = 18;
    state_.geometryRadius = F(0);
    state_.geometryWidth = F(1);
    break;
  case 111:
    state_.flags2=(state_.flags2&~0xc0u)|((uint32_t(I(0))<<6)&0xc0u);
    break;
  case 113: {
    const int duration = I(0);
    const int mode = int32_t(c.words[1]);
    const float target = F(2);
    if (mode != 9) {
      report(r, op, "TH128 opcode113 mode outside restored mode 9");
      return true;
    }
    if (duration < 0) {
      report(r, op, "TH128 opcode113 negative duration unsupported");
      return true;
    }
    angular_ = {};
    angular_.from = normalized(state_.rotation);
    angular_.to = normalized(target);
    angular_.duration = duration;
    angular_.active = duration > 0;
    state_.flags |= 4u;
    break;
  }
  case 114:
    state_.geometryCount = I(0);
    if(state_.geometryCount<2||state_.geometryCount>4096){report(r,op,"allocated arc vertex budget");return true;}
    state_.mode=14;
    state_.flags=(state_.flags&0xeeffffffu)|0x0e000000u;
    ++presentationRevision_;
    break;
  default:
    report(r, op, "opcode not yet restored");
    return true;
  }
  return false;
}

void VM::advance(Runtime& r) {
  float dt = (state_.flags & 0x80000000u) ? 1.f : r.delta;
  // Native leaves externally written angles untouched for a stationary axis.
  // UFO206 uses Rx as an arc span up to Tau, not a normalized orientation.
  if (rotationVelocity_[0] != 0)
    state_.rx = normalized(float(double(state_.rx) + float(rotationVelocity_[0] * dt)));
  if (rotationVelocity_[1] != 0)
    state_.ry = normalized(float(double(state_.ry) + float(rotationVelocity_[1] * dt)));
  if (rotationVelocity_[2] != 0)
    state_.rotation = normalized(float(double(state_.rotation) + float(rotationVelocity_[2] * dt)));
  state_.sx = float(double(state_.sx) + double(scaleVelocity_[0]) * dt);
  state_.sy = float(double(state_.sy) + double(scaleVelocity_[1]) * dt);
  state_.u = float(double(state_.u) + double(scrollVelocity_[0]) * dt);
  state_.v = float(double(state_.v) + double(scrollVelocity_[1]) * dt);
  if (state_.u >= 1)
    state_.u -= 1;
  else if (state_.u < 0)
    state_.u += 1;
  if (state_.v >= 1)
    state_.v -= 1;
  else if (state_.v < 0)
    state_.v += 1;
  for (auto& t : interpolations_)
    if (t.active) {
      // Native458e40 advances only positive-duration timers. A negative
      // duration remains active at time0; a zero duration never reaches here.
      if (t.duration > 0)
        t.time += dt;
      const float n = std::min(1.f, t.time / t.duration), e = curve(n, t.mode);
      for (size_t i = 0; i < t.fields.size(); i++) {
        const auto f = t.fields[i];
        double value;
        if (t.mode == 7) {
          t.from[i] += t.to[i];
          value = t.from[i];
        } else if (t.mode == 17) {
          t.from[i] += t.b.size() > i ? t.b[i] : 0;
          t.to[i] += t.b.size() > i ? t.b[i] : 0;
          value = t.from[i];
        } else if (t.mode == 8 && !t.a.empty()) {
          double x = n;
          value = (2 * x * x * x - 3 * x * x + 1) * t.from[i] +
                  (-2 * x * x * x + 3 * x * x) * t.to[i] + (x * x * x - 2 * x * x + x) * t.a[i] +
                  (x * x * x - x * x) * t.b[i];
        } else if (f == RED || f == GREEN || f == BLUE || f == RED2 || f == GREEN2 || f == BLUE2)
          value = t.from[i] - nativeInt(double(t.from[i] - t.to[i]) * e);
        else
          value = double(t.from[i]) + (double(t.to[i]) - t.from[i]) * e;
        // Native integer alpha interpolator converts the x87 result
        // before an f32 store (255 -> 0 at t=.2 produces 203, not 204).
        if (f == ALPHA || f == ALPHA2)
          value = nativeInt(value);
        setField(f, float(value));
      }
      if (t.duration > 0 && t.time >= t.duration)
        t.active = false;
    }
  if (angular_.active) {
    if (dt >= 0.99f && dt <= 1.01f) {
      angular_.time += 1.f;
      ++angular_.elapsed;
    } else {
      angular_.time += dt;
      angular_.elapsed = nativeInt(angular_.time);
    }
    if (angular_.elapsed >= angular_.duration) {
      state_.rotation = angular_.to;
      angular_.active = false;
      angular_.duration = 0;
    } else {
      const float t = angular_.time / float(angular_.duration);
      const float twice = float(2.0 * double(t));
      const float eased = twice < 1.f
                              ? float(0.5 * double(twice) * double(twice))
                              : float(1.0 - 0.5 * (2.0 - double(twice)) * (2.0 - double(twice)));
      double difference = double(angular_.to) - double(angular_.from);
      if (difference > double(Pi))
        difference -= double(Tau);
      else if (difference < -double(Pi))
        difference += double(Tau);
      const float step = normalized(float(difference * double(eased)));
      state_.rotation = normalized(float(double(angular_.from) + double(step)));
    }
    state_.flags |= 4u;
  }
  if (!state_.held)
    timer_ += dt;
  state_.clock = nativeInt(timer_);
  state_.ticks++;
}

const State& VM::state() const {
  return state_;
}

State& VM::control() {
  return state_;
}

uint32_t VM::continuousFields() const {
  uint32_t mask=0;
  if(rotationVelocity_[0]||rotationVelocity_[1]||rotationVelocity_[2])mask|=2;
  if(angular_.active)mask|=2;
  if(scaleVelocity_[0]||scaleVelocity_[1])mask|=4;
  if(scrollVelocity_[0]||scrollVelocity_[1])mask|=8;
  for(const auto& track:interpolations_)if(track.active)for(const auto field:track.fields){
    if(field<=Z)mask|=1;else if(field<=RZ)mask|=2;
    else if(field<=SY)mask|=4;else if(field<=VVEL)mask|=8;
    else if(field<=BLUE2)mask|=16;else if(field<=VSCALE)mask|=32;
  }
  return mask;
}

Diagnostic VM::diagnostics() const {
  Diagnostic result;
  result.faulted = faulted_;
  result.faultOpcode = faultOpcode_;
  result.reason = faultReason_;
  result.pc = pc_;
  result.savedPC = savedPc_;
  result.time = timer_;
  result.savedTimer = savedTimer_;
  result.pendingInterrupt = pendingInterrupt_;
  result.hasSavedTimer = saved_;
  result.rotationVelocity = rotationVelocity_;
  result.scaleVelocity = scaleVelocity_;
  result.scrollVelocity = scrollVelocity_;
  if (script_) {
    if (pc_ < script_->commands.size())
      result.byteOffset = script_->commands[pc_].offset;
    if (savedPc_ < script_->commands.size())
      result.savedByteOffset = script_->commands[savedPc_].offset;
  }
  for (const auto& t : interpolations_) {
    DiagnosticInterpolation d;
    d.active = t.active;
    d.mode = t.mode;
    d.time = t.time;
    d.duration = t.duration;
    d.from = t.from;
    d.to = t.to;
    d.a = t.a;
    d.b = t.b;
    for (auto f : t.fields)
      d.fields.push_back(int(f));
    result.interpolations.push_back(std::move(d));
  }
  return result;
}

bool VM::bind(Registry& registry, int bank, int script, Runtime& runtime,
              const std::array<float, 3>* initialScriptPosition) {
  ++presentationRevision_;
  // Reset preserves only engine position in the native VM, not domain,
  // local variables, colors or interpolation state (0x402520).
  const auto engine = std::array<float, 3>{state_.engineX, state_.engineY, state_.engineZ};
  const int layer = state_.layer;
  state_ = {};
  faulted_ = false;
  faultOpcode_ = -1;
  faultReason_.clear();
  state_.engineX = engine[0];
  state_.engineY = engine[1];
  state_.engineZ = engine[2];
  state_.layer = layer;
  registry_ = &registry;
  bank_ = registry.bank(bank);
  script_ = nullptr;
  pc_ = savedPc_ = 0;
  timer_ = savedTimer_ = 0;
  pendingInterrupt_ = 0;
  saved_ = false;
  rotationVelocity_ = {};
  scaleVelocity_ = {};
  scrollVelocity_ = {};
  interpolations_.clear();
  angular_ = {};
  state_.bank = bank;
  state_.script = script;
  if (!bank_ || script < 0 || size_t(script) >= bank_->scripts.size()) {
    report(runtime, -1, "invalid bank/script binding");
    return false;
  }
  script_ = &bank_->scripts[script];
  state_.bank = bank;
  state_.script = script;
  state_.ended = false;
  if (initialScriptPosition) {
    state_.x = (*initialScriptPosition)[0];
    state_.y = (*initialScriptPosition)[1];
    state_.z = (*initialScriptPosition)[2];
  }
  tick(runtime);
  return true;
}

bool VM::rebindWithoutReset(Registry& registry, int bank, int script, Runtime& runtime) {
  ++presentationRevision_;
  // Native 0x454ee0 keeps locals, RNG-domain flags, transforms, velocities
  // and active interpolators. It resets script timer and flip flags only.
  registry_ = &registry;
  bank_ = registry.bank(bank);
  script_ = nullptr;
  faulted_ = false;
  faultOpcode_ = -1;
  faultReason_.clear();
  if (!bank_ || script < 0 || size_t(script) >= bank_->scripts.size()) {
    state_ = {};
    state_.bank = bank;
    state_.script = script;
    report(runtime, -1, "invalid bank/script rebinding");
    return false;
  }
  script_ = &bank_->scripts[script];
  pc_ = 0;
  timer_ = 0;
  state_.clock = 0;
  state_.ticks = 0;
  state_.flags &= ~0xc01u;
  state_.visible = false;
  state_.ended = false;
  state_.deleteRequested = false;
  state_.held = false;
  state_.bank = bank;
  state_.script = script;
  state_.displayDomain = state_.flags2 & 1;
  tick(runtime);
  return true;
}

void VM::interrupt(int code) {
  ++presentationRevision_;
  pendingInterrupt_ = int16_t(code);
}

bool VM::tick(Runtime& runtime) {
  if (!script_ || state_.ended)
    return true;
  if (state_.flags & 0x80000u)
    return false;
  if (!std::isfinite(runtime.delta) || runtime.delta < 0) {
    report(runtime, -1, "invalid animation time step");
    return true;
  }
  if (state_.flags & 0x40000u)
    return false;
  if(pendingInterrupt_==0&&(state_.flags2&0xc0u)==0x40u){
    if(!runtime.updateControl){
      report(runtime,111,"TH128 opcode111 requires explicit native control snapshot (0x459e07)");
      return true;
    }
    if(runtime.updateControl->present&&(runtime.updateControl->flags60&2u))return false;
  }
  const float tickRate = (state_.flags & 0x80000000u) ? 1.f : runtime.delta;
  const bool requestedInterrupt = pendingInterrupt_ != 0;
  const bool unmatchedInterrupt = requestedInterrupt && !handleInterrupt();
  state_.held = false;
  size_t budget = 0;
  while (!unmatchedInterrupt && pc_ < script_->commands.size() &&
         script_->commands[pc_].time <= state_.clock) {
    if (budget++ >= runtime.instructionBudget) {
      report(runtime, -1, "instruction budget");
      state_.ended = true;
      break;
    }
    const size_t old = pc_;
    auto& c = script_->commands[pc_];
    const State previousState = state_;
    bool jumped = command(c, runtime);
    if (faulted_) {
      state_ = previousState;
      state_.ended = true;
      state_.held = false;
      break;
    }
    if (state_.ended || state_.held)
      break;
    if (!jumped && pc_ == old)
      ++pc_;
  }
  if (!state_.ended && pc_ >= script_->commands.size())
    report(runtime, -1, "instruction pointer outside script");
  if (!state_.ended) {
    // Unmatched IRQ455786 and held commands both reach45645f's464a20(-1)
    // before the common velocity/interpolation tail and positive timer step.
    const bool negativeStep = unmatchedInterrupt || state_.held;
    const bool fastTimer = tickRate > .99f && tickRate < 1.01f;
    const float timerStep = fastTimer ? 1.f : tickRate;
    float negativeTime = 0;
    int negativeClock = 0;
    if (negativeStep) {
      timer_ -= timerStep;
      state_.clock = nativeInt(timer_);
      negativeTime = timer_;
      negativeClock = state_.clock;
    }
    advance(runtime);
    if (negativeStep) {
      // 4586de increments the integer clock directly in the near-one path.
      // With a fractional timer the preceding negative step can truncate to
      // zero, so recomputing this clock from the final float is not equivalent.
      timer_ = float(double(negativeTime) + timerStep);
      state_.clock = fastTimer ? int32_t(uint32_t(negativeClock) + 1) : nativeInt(timer_);
    }
  }
  return state_.ended;
}
} // namespace th128::anm
