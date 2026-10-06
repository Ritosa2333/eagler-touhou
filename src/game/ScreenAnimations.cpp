// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"

namespace th12 {
uint64_t enemyAnimationKey(u32 id, int slot) {
  return (uint64_t(id) << 8) | uint32_t(slot);
}
void configureAnimations() {
  animationScene.runtime.gameRandom32 = []() { return random.next32(); };
  animationScene.runtime.displayRandom32 = []() { return displayRandom.next32(); };
}
void bindEnemyAnimation(Enemy& e, int slot, int script) {
  if (slot < 0 || slot >= 16) {
    ++unsupported[259];
    return;
  }
  e.animationSlots[slot] = script;
  e.animationBanks[slot] = e.bank;
  e.animationBound[slot] = true;
  animationScene.bind(enemyAnimationKey(e.id, slot), e.id, e.bank, script,
                      anm_logic::Membership::Primary, e.x, e.y);
}
void detachedAnimation(int bank, int script, const Enemy& e, bool head) {
  animationScene.bind(0x100000000ull + nextDetachedAnimation++, 0, bank, script,
                      anm_logic::Membership::Primary, e.x, e.y, {}, head);
}
int spellAnimationBank(spell_system::VisualBank bank) {
  switch (bank) {
  case spell_system::VisualBank::Font:
    return 11;
  case spell_system::VisualBank::SpellText:
    return 8;
  case spell_system::VisualBank::Bullet:
    return 0;
  case spell_system::VisualBank::Stage:
    return 2;
  case spell_system::VisualBank::StageAlternate:
    return 3;
  case spell_system::VisualBank::Front:
    return 7;
  }
  return -1;
}
uint64_t bindScreenAnimation(int bank, int script, int layer) {
  const u32 serial = nextDetachedAnimation++;
  const uint64_t key = 0x300000000ull + serial;
  animationScene.bind(key, 0xe0000000u + serial, bank, script, anm_logic::Membership::Primary, 0, 0,
                      {}, false, nullptr, 0, 0, layer);
  return key;
}
void startMessageAnimations() {
  messageAnimationKeys = {};
  for (int slot = 0; slot < 4; slot++)
    messageAnimationKeys[slot + 3] = bindScreenAnimation(8, slot);
}
void messagePortraitSprites(uint64_t key, const int* childScripts, const int* spriteBases,
                            int expression) {
  const auto* root = animationScene.find(key);
  if (!root)
    return;
  for (int i = 0; i < 3; i++)
    if (spriteBases[i] >= 0)
      for (const auto* child : animationScene.chain(anm_logic::Membership::Primary)) {
        if (child->vm.state().script != childScripts[i])
          continue;
        u32 ancestor = child->parent;
        bool attached = false;
        for (int depth = 0; ancestor && depth < 16; depth++) {
          if (ancestor == root->id) {
            attached = true;
            break;
          }
          auto at = animationScene.nodes().find(ancestor);
          if (at == animationScene.nodes().end())
            break;
          ancestor = at->second->parent;
        }
        if (attached) {
          auto& pose = animationScene.nodes().at(child->id)->vm.control();
          const auto bank = animationScene.registry.bank(pose.bank);
          const int sprite = spriteBases[i] + expression;
          if (bank && sprite >= 0 && sprite < int(bank->sprites.size()))
            pose.sprite = sprite;
          else
            ++unsupported[983];
          break;
        }
      }
}
void messageCommand(const message_system::Command& command) {
  const auto interrupt = [](int slot, int code) {
    animationScene.interrupt(messageAnimationKeys[slot], code);
  };
  switch (command.op) {
  case 1: {
    static constexpr int scripts[] = {31, 30, 28};
    messageAnimationKeys[0] = bindScreenAnimation(4 + character, scripts[character]);
    break;
  }
  case 2: {
    static constexpr int scripts[] = {13, 16, 10, 19, 10, 12, 11};
    const bool alternate = (stageNumber == 5 || stageNumber == 7) && messageVM.entry >= 2;
    messageAnimationKeys[1] = bindScreenAnimation(
        alternate ? 3 : 2, alternate ? (stageNumber == 5 ? 10 : 13) : scripts[stageNumber - 1]);
    break;
  }
  case 3:
    messageAnimationKeys[2] = bindScreenAnimation(7, 52);
    break;
  case 4:
    interrupt(0, 1);
    messageAnimationKeys[0] = 0;
    break;
  case 5:
    interrupt(1, 1);
    interrupt(7, 1);
    messageAnimationKeys[1] = 0;
    break;
  case 6:
    for (int slot = 2; slot < 7; slot++)
      interrupt(slot, 1);
    break;
  case 7:
  case 8:
  case 9: {
    const int code = command.op == 9 ? 8 : 2;
    interrupt(0, command.op == 8 ? 3 : code);
    interrupt(1, command.op == 7 ? 3 : code);
    interrupt(2, command.op == 9 ? 8 : command.op - 5);
    for (int slot = 3; slot < 7; slot++) {
      animationScene.position(messageAnimationKeys[slot], 8, 0);
      if (auto* node = animationScene.find(messageAnimationKeys[slot]))
        node->vm.control().offsetY = 0;
    }
    break;
  }
  case 13: {
    static constexpr int scripts[3][3] = {{28, 29, 30}, {27, 28, 29}, {25, 26, 27}},
                         sprites[3][3] = {{43, 51, 59}, {42, 50, 58}, {43, 51, 59}};
    messagePortraitSprites(messageAnimationKeys[0], scripts[character], sprites[character],
                           command.integer());
    break;
  }
  case 14: {
    static constexpr int scripts[7][3] = {{10, 11, 12}, {13, 14, 15}, {7, 8, 9}, {16, 17, 18},
                                          {7, 8, 9},    {9, 10, 11},  {8, 9, 10}},
                         sprites[7][3] = {{14, 22, -1}, {16, 25, 34}, {12, 21, 30}, {28, 37, 46},
                                          {15, 24, 33}, {16, 25, 34}, {17, 26, 35}};
    static constexpr int alternateScripts[2][3] = {{7, 8, 9}, {10, 11, 12}},
                         alternateSprites[2][3] = {{12, 20, 29}, {14, 23, 32}};
    const bool alternate = (stageNumber == 5 || stageNumber == 7) && messageVM.entry >= 2;
    const int index = stageNumber == 7;
    messagePortraitSprites(
        messageAnimationKeys[1], alternate ? alternateScripts[index] : scripts[stageNumber - 1],
        alternate ? alternateSprites[index] : sprites[stageNumber - 1], command.integer());
    break;
  }
  case 18:
    for (int slot = 3; slot < 7; slot++)
      interrupt(slot, 3);
    break;
  case 19:
    bindScreenAnimation(9, 2);
    break;
  case 20: {
    static constexpr int scripts[] = {17, 21, 14, 23, 14, 16, 16};
    messageAnimationKeys[7] = bindScreenAnimation(2, scripts[stageNumber - 1]);
    break;
  }
  case 23:
    interrupt(0, 7);
    break;
  case 24:
    interrupt(1, 7);
    break;
  case 25:
    for (int slot = 3; slot < 7; slot++)
      if (auto* node = animationScene.find(messageAnimationKeys[slot]))
        node->vm.control().offsetY = float(command.integer());
    break;
  default:
    break;
  }
  for (const auto& action : messageVM.textActions)
    if (action.slot >= 0 && action.slot < 4)
      interrupt(action.slot + 3, action.interrupt);
}
void startSpellAnimations() {
  spellAnimationKeys = {};
  const auto bindings = spell_system::startBindings(stageNumber, frame);
  for (size_t i = 0; i < bindings.size() && i < spellAnimationKeys.size(); i++)
    spellAnimationKeys[i] = bindScreenAnimation(spellAnimationBank(bindings[i].bank),
                                                bindings[i].script, bindings[i].layer);
  animationScene.position(spellAnimationKeys[3], spellState.ringX + 224, spellState.ringY + 16);
  if (auto* root = animationScene.find(spellAnimationKeys[3]))
    for (auto& [id, node] : animationScene.nodes())
      if (node->parent == root->id &&
          (node->vm.state().script == 125 || node->vm.state().script == 126))
        node->vm.control().integers[2] = spellState.timeLimit;
}
void endSpellAnimations(const spell_system::Result& result) {
  if (result.kind == spell_system::ResultKind::None)
    return;
  for (int i = 0; i < 3; i++)
    animationScene.interrupt(spellAnimationKeys[i], 1);
  animationScene.remove(spellAnimationKeys[5]);
  animationScene.remove(spellAnimationKeys[3]);
  spellAnimationKeys[3] = spellAnimationKeys[5] = 0;
  const bool captured = result.kind == spell_system::ResultKind::Captured;
  const auto digits = spell_system::resultDigits(result.bonusDisplayed);
  for (const auto& binding : spell_system::resultBindings(captured, result.bonusDisplayed)) {
    const auto key =
        bindScreenAnimation(spellAnimationBank(binding.bank), binding.script, binding.layer);
    if (binding.bank == spell_system::VisualBank::Font && binding.script >= 5 &&
        binding.script <= 12)
      if (auto* node = animationScene.find(key)) {
        const auto& digit = digits[binding.script - 5];
        auto& state = node->vm.control();
        state.sprite = digit.sprite;
        state.visible = digit.visible;
        if (digit.visible)
          state.flags |= 1;
        else
          state.flags &= ~1u;
      }
  }
}

} // namespace th12
