#pragma once
#include <eagler/gameplay/CooperativeRules.hpp>
namespace th15 {
struct MultiplayerTransferFeedback {
 enum Kind {none,life,power} kind=none;unsigned value=0;
};
// Show the same tap count for life and Power gifts.
constexpr MultiplayerTransferFeedback multiplayer_transfer_feedback(bool alive,unsigned hold,bool receiver,unsigned taps)noexcept{
 if(!alive)return {};
 if(eagler::cooperative::TapCount(hold)>=3&&receiver)return {MultiplayerTransferFeedback::life,eagler::cooperative::TapCount(hold)};
 if(taps>=3)return {MultiplayerTransferFeedback::power,taps};
 return {};
}
}
