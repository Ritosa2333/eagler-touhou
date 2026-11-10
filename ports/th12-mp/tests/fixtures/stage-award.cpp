#include "../../src/stage_award.hpp"
extern "C" {
int award_base(int stage,int score){th12::stage_award::add(score,th12::stage_award::base(stage));return score;}
int award_final(int stage,int difficulty,int power,int lives,int bombs,int piv,int score){th12::stage_award::add(score,th12::stage_award::final(stage,difficulty,power,lives,bombs,piv));return score;}
int award_value(int stage,int difficulty,int power,int lives,int bombs,int piv){return th12::stage_award::final(stage,difficulty,power,lives,bombs,piv);}
}
