#pragma once
#include <optional>
namespace th128::item {
struct Position {float x=0,y=0,z=0;};
enum class MotionContinuation {Contact,AutoAttraction,Removed};
struct MotionInput {Position position,velocity;int playerState=1;float playerY=400;};
struct MotionResult {MotionContinuation continuation=MotionContinuation::Contact;Position position,velocity;};
enum class Contact {None,Collect,Attract};
std::optional<MotionResult> ordinaryMotion(const MotionInput& input) noexcept;
std::optional<Contact> contactDecision(Position item,Position player,float collectFullSize,float attractHalfSize,bool focused) noexcept;
std::optional<int> pointReward(int pointValue,float playerY,int state,int kind) noexcept;
}
