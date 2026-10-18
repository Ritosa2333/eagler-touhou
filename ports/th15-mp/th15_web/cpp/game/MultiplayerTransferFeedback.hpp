#pragma once
namespace th15 {
struct MultiplayerTransferFeedback {
 enum Kind {none,life,power} kind=none;unsigned value=0;
};
// TH11 MultiplayerPresentation: valid life hold first, otherwise P taps >=3.
constexpr MultiplayerTransferFeedback multiplayer_transfer_feedback(bool alive,unsigned hold,bool receiver,unsigned taps)noexcept{
 if(!alive)return {};
 if(hold&&receiver)return {MultiplayerTransferFeedback::life,hold>=90?100:hold*100/90};
 if(taps>=3)return {MultiplayerTransferFeedback::power,taps};
 return {};
}
}
