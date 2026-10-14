#pragma once
namespace th12 {
struct SessionCarry;
void startGame(int character, int shot, int difficulty, int seed,
               const int* replayInitial = nullptr, const SessionCarry* carry = nullptr);
void selectStage(int number);
void setInitial(int x, int y, int power, int lives, int bombs, int score);
void setReplayEconomy(int piv, int lifeFragments, int bombFragments, int red, int blue, int green,
                      int rank);
} // namespace th12
