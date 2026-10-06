// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"

namespace th12 {
void tickMessage(int held, int pressed) {
  // Native HUD/MSG callback priority 25 follows ItemManager priority 22.
  messageVM.tick(u32(held), u32(pressed), [&](const message_system::Command& command) {
    messageCommand(command);
    if (command.op != 10 && command.op != 11 && command.op != 12)
      messageEvents.push_back(
          {command.op, frame + 1, {command.payload, command.payload + command.size}});
    if (command.op == 21) {
      stage_award::add(score, stage_award::base(stageNumber));
      bindScreenAnimation(8, 85);
      stage_award::add(score, stage_award::final(stageNumber, difficulty, power, lives, bombs,
                                                 resources.pointValueStored));
      ended = 1;
    }
  });
  dialogue = messageVM.active ? messageVM.entry + 1 : 0;
}

} // namespace th12
