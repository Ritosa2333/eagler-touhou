#include "../../src/spell.hpp"
th12::spell_system::State state;
int32_t score=0;
extern "C" {
void reset(){state.reset();score=0;}
void start(){th12::spell_system::Start input;input.cardId=3;state.start(input);}
void clock_ms(double milliseconds){state.observeWallTime(milliseconds);}
void tick(){state.tick(false,300,40,128);}
void end(){state.end(score);}
int encoded(){return state.lastResult.realTimeEncoded;}
int display_ticks(){return state.lastResult.displayElapsedTicks;}
int flags(){return state.flags;}
void replay(int encoded){state.replayWallClock=true;state.replayResultTimes[0]=encoded;state.replayResultTimeCount=1;}
}
