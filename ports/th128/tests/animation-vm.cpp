#include "AnmScene.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>

using namespace th128::anm;
using Bytes = std::vector<uint8_t>;

void check(bool condition, const std::string& message) {
  if (!condition)
    throw std::runtime_error(message);
}

void near(float actual, float expected) {
  check(std::isfinite(actual) && std::abs(actual - expected) < 0.0001f,
        "float mismatch: " + std::to_string(actual) + " != " + std::to_string(expected));
}

void put16(Bytes& bytes, size_t offset, uint16_t value) {
  bytes.at(offset) = uint8_t(value);
  bytes.at(offset + 1) = uint8_t(value >> 8);
}

void put32(Bytes& bytes, size_t offset, uint32_t value) {
  for (size_t byte = 0; byte < 4; ++byte)
    bytes.at(offset + byte) = uint8_t(value >> (byte * 8));
}

uint32_t get32(const Bytes& bytes, size_t offset) {
  uint32_t value = 0;
  for (size_t byte = 0; byte < 4; ++byte)
    value |= uint32_t(bytes.at(offset + byte)) << (byte * 8);
  return value;
}

Bytes instruction(uint16_t opcode, int16_t time = 0, std::vector<uint32_t> words = {},
                  uint16_t mask = 0) {
  Bytes bytes(8 + words.size() * 4);
  put16(bytes, 0, opcode);
  put16(bytes, 2, opcode == 65535 ? 0 : uint16_t(bytes.size()));
  put16(bytes, 4, uint16_t(time));
  put16(bytes, 6, mask);
  for (size_t word = 0; word < words.size(); ++word)
    put32(bytes, 8 + word * 4, words[word]);
  return bytes;
}

Bytes code(std::initializer_list<Bytes> commands) {
  Bytes bytes;
  for (const auto& command : commands)
    bytes.insert(bytes.end(), command.begin(), command.end());
  const auto sentinel = instruction(65535);
  bytes.insert(bytes.end(), sentinel.begin(), sentinel.end());
  return bytes;
}

Bytes bankBytes(const std::vector<Bytes>& scripts, bool sprite = true) {
  const size_t tableEnd = 64 + (sprite ? 4 : 0) + scripts.size() * 8;
  Bytes bytes(tableEnd + 8 + (sprite ? 20 : 0));
  put32(bytes, 0, 7);
  put16(bytes, 4, sprite ? 1 : 0);
  put16(bytes, 6, uint16_t(scripts.size()));
  put16(bytes, 10, 128);
  put16(bytes, 12, 64);
  put32(bytes, 16, uint32_t(tableEnd));
  bytes[tableEnd] = 'x';
  if (sprite) {
    const size_t offset = tableEnd + 8;
    put32(bytes, 64, uint32_t(offset));
    put32(bytes, offset, 91);
    put32(bytes, offset + 4, asWord(16.f));
    put32(bytes, offset + 8, asWord(8.f));
    put32(bytes, offset + 12, asWord(32.f));
    put32(bytes, offset + 16, asWord(16.f));
  }
  for (size_t script = 0; script < scripts.size(); ++script) {
    const size_t table = 64 + (sprite ? 4 : 0) + script * 8;
    put32(bytes, table, uint32_t(700 + script));
    put32(bytes, table + 4, uint32_t(bytes.size()));
    bytes.insert(bytes.end(), scripts[script].begin(), scripts[script].end());
  }
  return bytes;
}

struct Rig {
  Registry registry;
  Runtime runtime;
  VM vm;
  std::vector<std::pair<int, std::string>> errors;
  Rig() {
    runtime.unsupported = [this](int, int, int opcode, const char* reason) {
      errors.emplace_back(opcode, reason);
    };
  }
  void load(const Bytes& bytes) {
    std::string error;
    check(registry.loadBank(24, bytes.data(), bytes.size(), error), error);
  }
  void bind(const Bytes& script) {
    load(bankBytes({script}));
    check(vm.bind(registry, 24, 0, runtime), "bind failed");
  }
};

void rejects(const Bytes& bytes) {
  Registry registry;
  std::string error;
  check(!registry.loadBank(24, bytes.data(), bytes.size(), error), "malformed bank accepted");
  check(!error.empty(), "rejection has no diagnostic");
  check(!registry.bank(24), "rejected bank published");
}

void loaderValid() {
  Rig rig;
  rig.load(bankBytes({code({instruction(63)}), code({instruction(2)})}));
  const auto bank = rig.registry.bank(24);
  check(bank->scripts.size() == 2 && bank->sprites.size() == 1, "counts");
  check(bank->scripts[0].sourceId == 700, "source id must not replace global ordinal");
  near(bank->sprites[0].u0, .125f);
  near(bank->sprites[0].v1, .375f);
  Bytes second = bankBytes({code({instruction(63)})});
  Bytes first = bankBytes({code({instruction(63)})});
  put32(first, 36, uint32_t(first.size()));
  first.insert(first.end(), second.begin(), second.end());
  rig.load(first);
  check(rig.registry.bank(24)->scripts.size() == 2, "entry-relative offsets");
  check(rig.registry.bank(24)->sprites[1].textureEntry == 1, "texture entry ordinal");
  auto reversed = bankBytes({code({instruction(63)}), code({instruction(2)})});
  const uint32_t firstOffset = get32(reversed, 72), secondOffset = get32(reversed, 80);
  put32(reversed, 72, secondOffset);
  put32(reversed, 80, firstOffset);
  rig.load(reversed);
  check(rig.registry.bank(24)->scripts[0].commands[0].op == 2, "table order is API order");
}

void loaderAliases() {
  auto bytes = bankBytes({code({instruction(63)}), code({instruction(63)})});
  put32(bytes, 80, get32(bytes, 72));
  rejects(bytes);
  bytes = bankBytes({code({instruction(0), instruction(63)}), code({instruction(63)})});
  put32(bytes, 80, get32(bytes, 72) + 8);
  rejects(bytes);
  bytes = bankBytes({code({instruction(63)})});
  put32(bytes, 64, 64);
  rejects(bytes);
  bytes = bankBytes({code({instruction(63)})});
  put32(bytes, 72, get32(bytes, 64));
  rejects(bytes);
}

void loaderMalformed() {
  rejects(Bytes(63));
  auto bytes = bankBytes({code({instruction(63)})});
  auto broken = bytes;
  put32(broken, 36, 63);
  rejects(broken);
  broken = bytes;
  put32(broken, 72, 0xfffffffcu);
  rejects(broken);
  broken = bytes;
  put16(broken, get32(broken, 72) + 2, 9);
  rejects(broken);
  broken = bytes;
  broken.resize(broken.size() - 1);
  rejects(broken);
  broken = bytes;
  put32(broken, get32(broken, 64) + 12, 0x7fc00000u);
  rejects(broken);
  broken = bankBytes({code({instruction(48, 0, {asWord(1.f)})})});
  rejects(broken);
  broken = bankBytes({code({instruction(3, 0, {0}, 2)})});
  rejects(broken);
  broken = bytes;
  put16(broken, 32, 1);
  put32(broken, 28, uint32_t(broken.size() - 8));
  rejects(broken);
  Rig rig;
  rig.load(bytes);
  const auto previous = rig.registry.bank(24);
  std::string error;
  check(!rig.registry.loadBank(24, broken.data(), broken.size(), error), "bad replacement");
  check(rig.registry.bank(24) == previous, "failed load must be transactional");
}

void loaderBudgets() {
  Bytes commands;
  const auto nop = instruction(0);
  for (size_t count = 0; count < 70000; ++count)
    commands.insert(commands.end(), nop.begin(), nop.end());
  const auto sentinel = instruction(65535);
  commands.insert(commands.end(), sentinel.begin(), sentinel.end());
  rejects(bankBytes({commands, commands, commands}, false));
  Bytes entry = bankBytes({}, false), many;
  put32(entry, 36, uint32_t(entry.size()));
  for (size_t count = 0; count < 4097; ++count)
    many.insert(many.end(), entry.begin(), entry.end());
  put32(many, many.size() - entry.size() + 36, 0);
  rejects(many);
  const auto largeOperand = instruction(300, 0, std::vector<uint32_t>(1024));
  Bytes wordHeavy;
  for (size_t count = 0; count < 1000; ++count)
    wordHeavy.insert(wordHeavy.end(), largeOperand.begin(), largeOperand.end());
  wordHeavy.insert(wordHeavy.end(), sentinel.begin(), sentinel.end());
  rejects(bankBytes({wordHeavy}, false));
  entry = bankBytes(std::vector<Bytes>(4096, code({})), false);
  put32(entry, 36, uint32_t(entry.size()));
  many.clear();
  for (size_t count = 0; count < 5; ++count)
    many.insert(many.end(), entry.begin(), entry.end());
  put32(many, many.size() - entry.size() + 36, 0);
  rejects(many);
}

void timingAndJump() {
  Rig rig;
  rig.bind(code({instruction(48, -1, {asWord(2.f), 0, 0}),
                 instruction(48, 2, {asWord(8.f), 0, 0}), instruction(63, 2)}));
  near(rig.vm.state().x, 2);
  check(rig.vm.state().clock == 1 && rig.vm.state().ticks == 1, "synchronous birth tick");
  rig.vm.tick(rig.runtime);
  near(rig.vm.state().x, 2);
  rig.vm.tick(rig.runtime);
  near(rig.vm.state().x, 8);
  check(rig.vm.state().held && rig.vm.state().clock == 2, "hold pins time");
  Rig looping;
  looping.bind(code({instruction(6, 0, {10000, 3}, 1),
                    instruction(8, 0, {10001, 1}, 1),
                    instruction(5, 0, {10000, 16, 0}, 1), instruction(63)}));
  check(looping.vm.state().integers[0] == 0 && looping.vm.state().integers[1] == 3,
        "decrement branch uses command byte offset");
  Rig bad;
  bad.bind(code({instruction(4, 0, {1, 0}), instruction(48, 0, {asWord(99.f), 0, 0})}));
  check(bad.vm.state().ended && !bad.errors.empty(), "unaligned jump must stop");
  near(bad.vm.state().x, 0);
}

void masksAndSharedWords() {
  Rig rig;
  rig.bind(code({instruction(6, 0, {10000, 5}, 1),
                 instruction(7, 0, {asWord(10004.f), asWord(2.5f)}, 1),
                 instruction(23, 0, {asWord(10005.f), asWord(10004.f), asWord(10000.f)}, 7),
                 instruction(6, 0, {10001, 10004}, 3),
                 instruction(48, 0, {asWord(10005.f), 0, 0}, 1), instruction(63)}));
  check(rig.vm.state().integers[0] == 5 && rig.vm.state().integers[1] == 2, "integer mask");
  near(rig.vm.state().floats[0], 2.5f);
  near(rig.vm.state().floats[1], 12.5f);
  near(rig.vm.state().x, 12.5f);
  Rig shared;
  shared.load(bankBytes({code({instruction(8, 0, {0, 1}), instruction(63)})}));
  VM first, second;
  check(first.bind(shared.registry, 24, 0, shared.runtime), "first bind");
  check(second.bind(shared.registry, 24, 0, shared.runtime), "second bind");
  check(shared.registry.bank(24)->scripts[0].commands[0].words[0] == 2, "shared literals");
}

void interpolationAndDelta() {
  Rig rig;
  rig.bind(code({instruction(56, 0, {4, 0, asWord(8.f), asWord(4.f), 0}), instruction(63)}));
  near(rig.vm.state().x, 2);
  rig.vm.tick(rig.runtime);
  near(rig.vm.state().x, 4);
  rig.vm.tick(rig.runtime);
  rig.vm.tick(rig.runtime);
  near(rig.vm.state().x, 8);
  check(!rig.vm.diagnostics().interpolations[0].active, "track finishes");
  Rig quadratic;
  quadratic.bind(code({instruction(56, 0, {4, 1, asWord(16.f), 0, 0}), instruction(63)}));
  near(quadratic.vm.state().x, 1);
  Rig zero;
  zero.bind(code({instruction(56, 0, {0, 0, asWord(99.f), 0, 0}), instruction(63)}));
  near(zero.vm.state().x, 0);
  Rig fractional;
  fractional.runtime.delta = .5f;
  fractional.bind(code({instruction(54, 0, {asWord(2.f), 0}), instruction(63)}));
  near(fractional.vm.state().sx, 2);
  check(fractional.vm.state().clock == 0, "fractional clock");
}

void interrupts() {
  Rig rig;
  rig.bind(code({instruction(63), instruction(64, 5, {7}),
                 instruction(48, 5, {asWord(9.f), 0, 0}), instruction(81, 5),
                 instruction(64, 6, {uint32_t(-1)}),
                 instruction(48, 6, {asWord(11.f), 0, 0}), instruction(63, 6)}));
  rig.vm.interrupt(7);
  rig.vm.tick(rig.runtime);
  near(rig.vm.state().x, 9);
  check(rig.vm.state().held && rig.vm.diagnostics().hasSavedTimer, "interrupt return");
  rig.vm.interrupt(8);
  rig.vm.tick(rig.runtime);
  near(rig.vm.state().x, 11);
  Rig unmatched;
  unmatched.bind(code({instruction(63)}));
  unmatched.vm.interrupt(99);
  unmatched.vm.tick(unmatched.runtime);
  check(unmatched.errors.empty() && unmatched.vm.state().clock == 0, "unmatched interrupt");
  Rig invalidReturn;
  invalidReturn.bind(code({instruction(81), instruction(48, 0, {asWord(4.f), 0, 0})}));
  check(invalidReturn.vm.state().ended && !invalidReturn.errors.empty(), "unmatched return");
  near(invalidReturn.vm.state().x, 0);
}

void strictStops() {
  Rig unconditionallyUpdated;
  unconditionallyUpdated.load(bankBytes({code({instruction(111,0,{0}),instruction(48,0,{asWord(4.f),0,0}),instruction(63)})}));
  unconditionallyUpdated.vm.control().flags2=0xff;
  check(unconditionallyUpdated.vm.rebindWithoutReset(unconditionallyUpdated.registry,24,0,unconditionallyUpdated.runtime),
        "literal-zero TH128 update mode must bind");
  check(!unconditionallyUpdated.vm.faulted()&&unconditionallyUpdated.vm.state().flags2==0x3f,
        "literal-zero mode clears native two-bit gate without erasing unrelated flags");
  near(unconditionallyUpdated.vm.state().x,4);
  for (uint16_t opcode : {uint16_t(44), uint16_t(98), uint16_t(109),
                          uint16_t(113), uint16_t(300), uint16_t(0xfffe)}) {
    Rig rig;
    std::vector<uint32_t> words;
    if (opcode == 44 || opcode == 109) words = {0, 0};
    if (opcode == 113) words = {10, 9, 0};
    rig.bind(code({instruction(opcode, 0, words), instruction(48, 0, {asWord(99.f), 0, 0})}));
    check(rig.vm.state().ended && rig.errors.size() == 1, "unsupported must stop once");
    near(rig.vm.state().x, 0);
    check(rig.vm.state().ticks == 0, "fault must not advance time");
    const auto diagnostic = rig.vm.diagnostics();
    check(diagnostic.faulted && diagnostic.faultOpcode == opcode &&
          diagnostic.byteOffset == 0 && !diagnostic.reason.empty(), "persistent fault diagnostic");
    rig.vm.tick(rig.runtime);
    check(rig.errors.size() == 1, "stopped VM cannot re-report");
  }
  Rig looping;
  looping.runtime.instructionBudget = 5;
  looping.bind(code({instruction(4, 0, {0, 0})}));
  check(looping.vm.state().ended && looping.errors.size() == 1, "instruction cap");
  Rig rng;
  rng.bind(code({instruction(40, 0, {10000, 7}, 1),
                 instruction(48, 0, {asWord(99.f), 0, 0})}));
  check(rng.vm.state().ended && rng.errors.size() == 1, "RNG callback required");
  near(rng.vm.state().x, 0);
  Rig child;
  child.bind(code({instruction(88, 0, {0}), instruction(48, 0, {asWord(99.f), 0, 0})}));
  check(child.vm.state().ended && child.errors.size() == 1, "child callback required");
  near(child.vm.state().x, 0);
  Rig integerFault;
  integerFault.bind(code({instruction(6, 0, {10000, 7}, 1),
                         instruction(14, 0, {10000, 0}, 1),
                         instruction(48, 0, {asWord(99.f), 0, 0})}));
  check(integerFault.vm.faulted() && integerFault.errors.size() == 1, "integer divide fault");
  check(integerFault.vm.state().integers[0] == 7, "fault preserves preceding state");
  near(integerFault.vm.state().x, 0);
  Rig unknownMode;
  unknownMode.bind(code({instruction(56, 0, {4, 99, asWord(8.f), 0, 0}), instruction(63)}));
  check(unknownMode.vm.faulted(), "unsupported interpolation mode must stop");
}

void sceneScheduling() {
  Scene scene;
  std::string error;
  const auto bytes = bankBytes({
      code({instruction(88, 0, {1}), instruction(63)}),
      code({instruction(88, 0, {2}), instruction(63)}), code({instruction(63)}),
      code({instruction(63)})});
  check(scene.registry.loadBank(24, bytes.data(), bytes.size(), error), error);
  const Node* root = scene.bind(1, 7, 24, 0);
  check(root && scene.nodes().size() == 3, "synchronous child and grandchild bind");
  auto chain = scene.chain(Membership::Primary);
  check(chain.size() == 3 && chain[0]->vm.state().script == 2 &&
        chain[1]->vm.state().script == 1 && chain[2]->vm.state().script == 0,
        "birth-before-registration order");
  scene.tickPrimary();
  check(root->vm.state().ticks == 2, "root one scheduled tick");
  check(scene.bind(2, 7, 24, 3, Membership::Secondary), "secondary bind");
  scene.tickSecondary();
  check(scene.find(2)->vm.state().ticks == 2, "secondary independent chain");
  scene.position(1, 10, 20);
  near(scene.find(1)->vm.state().engineX, 10);
  scene.removeOwner(7);
  scene.collect();
  check(scene.nodes().empty(), "owner collection includes attached descendants");
}

void sceneBirthBudget() {
  Scene scene;
  std::vector<Bytes> scripts;
  for (uint32_t script = 0; script < 70; ++script)
    scripts.push_back(script == 69 ? code({instruction(63)})
                                 : code({instruction(88, 0, {script + 1}), instruction(63)}));
  const auto bytes = bankBytes(scripts);
  std::string error;
  check(scene.registry.loadBank(24, bytes.data(), bytes.size(), error), error);
  int errors = 0;
  scene.runtime.unsupported = [&](int, int, int, const char*) { ++errors; };
  scene.bind(1, 0, 24, 0);
  check(errors > 0 && scene.nodes().size() <= 64, "synchronous recursion must be capped");
}

void badRuntime() {
  Rig delta;
  delta.runtime.delta = std::numeric_limits<float>::quiet_NaN();
  delta.bind(code({instruction(63)}));
  check(delta.vm.state().ended && delta.errors.size() == 1, "non-finite time step must stop");
  Rig missing;
  check(!missing.vm.bind(missing.registry, 24, 0, missing.runtime), "absent bank cannot bind");
  check(missing.vm.state().ended && missing.errors.size() == 1, "invalid bind diagnostic");
  Rig malformed;
  malformed.load(bankBytes({code({instruction(48, 0, {0, 0, 0})})}));
  malformed.registry.bank(24)->scripts[0].commands[0].words.clear();
  check(malformed.vm.bind(malformed.registry, 24, 0, malformed.runtime), "structural bind");
  check(malformed.vm.state().ended && malformed.errors.size() == 1, "mutated operand arity stops once");
}

Bytes readFile(const std::string& filename) {
  std::ifstream stream(filename, std::ios::binary);
  check(bool(stream), "cannot open " + filename);
  return Bytes(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}

void realBank(const std::string& filename) {
  Registry registry;
  const auto bytes = readFile(filename);
  std::string error;
  check(registry.loadBank(24, bytes.data(), bytes.size(), error), filename + ": " + error);
  const auto bank = registry.bank(24);
  std::cout << "BANK " << filename << " scripts=" << bank->scripts.size()
            << " sprites=" << bank->sprites.size() << '\n';
}

void realTitle(const std::string& filename) {
  Scene scene;
  const auto bytes = readFile(filename);
  std::string error;
  check(scene.registry.loadBank(24, bytes.data(), bytes.size(), error), error);
  std::vector<int> unsupported;
  scene.runtime.unsupported = [&](int, int, int opcode, const char*) {
    unsupported.push_back(opcode);
  };
  check(scene.bind(97, 0, 24, 97) && scene.bind(101, 0, 24, 101), "real title boot bind");
  for (int frame = 0; frame < 60; ++frame) {
    scene.tickPrimary();
    scene.tickSecondary();
    scene.collect();
  }
  check(unsupported.empty(), "title boot reached unsupported operation");
  for (int script : {97, 98, 99, 100, 101, 103, 104}) {
    bool found = false;
    for (const auto& record : scene.nodes())
      found = found || record.second->vm.state().script == script;
    check(found, "title child missing: " + std::to_string(script));
  }
  check(scene.bind(0, 0, 24, 0), "real title menu-root bind");
  check(unsupported.empty(), "title menu root reached unsupported operation");
  const std::string directory = filename.substr(0, filename.find_last_of("/\\") + 1);
  const auto titleV = readFile(directory + "title_v.anm");
  check(scene.registry.loadBank(25, titleV.data(), titleV.size(), error), error);
  check(scene.bind(25, 0, 25, 0), "real title overlay bank25/script0");
  for (int frame = 60; frame < 71; ++frame) {
    scene.tickPrimary();
    scene.tickSecondary();
    scene.collect();
  }
  scene.interrupt(0, 3, true);
  scene.interrupt(0, 17);
  for (const auto& record : scene.nodes()) {
    const auto& node = *record.second;
    if (node.vm.state().bank == 24 && node.vm.state().script >= 4 && node.vm.state().script <= 9)
      scene.interruptNode(node.id, 31, true);
  }
  for (int frame = 71; frame < 120; ++frame) {
    scene.tickPrimary();
    scene.tickSecondary();
    scene.collect();
  }
  check(unsupported.empty(), "title clock71 IRQ3/17/nonselected31 reached unsupported operation");
  std::cout << "TITLE boot97/101+120ticks+menu0+overlay25+IRQ3/17/31 nodes="
            << scene.nodes().size() << '\n';
}

int main(int argc, char** argv) {
  if (argc == 2 && std::string(argv[1]) == "--normalization-probe") {
    volatile float large = std::numeric_limits<float>::max();
    const float value = large;
    near(normalized(value), value);
    near(normalized(-value), -value);
    near(normalized(3 * Pi), Pi);
    std::cout << "PASS bounded-angle-normalization\n";
    return 0;
  }
  int failed = 0;
  const std::vector<std::pair<std::string, std::function<void()>>> tests = {
      {"loader-valid", loaderValid}, {"loader-aliases", loaderAliases},
      {"loader-malformed", loaderMalformed}, {"loader-aggregate-budgets", loaderBudgets},
      {"time-jump", timingAndJump}, {"variable-masks-shared-words", masksAndSharedWords},
      {"interpolations-delta", interpolationAndDelta}, {"interrupts", interrupts},
      {"unsupported-stop", strictStops}, {"scene-scheduling", sceneScheduling},
      {"scene-birth-budget", sceneBirthBudget}, {"invalid-runtime", badRuntime}};
  for (const auto& test : tests) {
    try {
      test.second();
      std::cout << "PASS " << test.first << '\n';
    } catch (const std::exception& error) {
      ++failed;
      std::cerr << "FAIL " << test.first << ": " << error.what() << '\n';
    }
  }
  try {
    for (int argument = 1; argument + 1 < argc; argument += 2) {
      const std::string option = argv[argument];
      if (option == "--bank") realBank(argv[argument + 1]);
      else if (option == "--title") realTitle(argv[argument + 1]);
      else throw std::runtime_error("unknown fixture option");
    }
  } catch (const std::exception& error) {
    ++failed;
    std::cerr << "FAIL real-data: " << error.what() << '\n';
  }
  std::cout << "animation fixtures failures=" << failed << '\n';
  return failed ? 1 : 0;
}
