#pragma once
#include <windows.h>
#include <vector>
namespace th5w { namespace practice {
struct AdvancedOptions {
 int fps;bool freeFPS,noContinue,disableX,disableShift,disableZ,forceShift,showKeys,lockTimer,fastRetry,showHitbox,alwaysHitbox,numericDream,numericPower,phaseTimeout,phaseTimeoutAsTime,hpPhaseMarkers,eosdBar,phaseStars,ringBar;
 AdvancedOptions():fps(56),freeFPS(false),noContinue(false),disableX(false),disableShift(false),disableZ(false),forceShift(false),showKeys(false),lockTimer(false),fastRetry(false),showHitbox(false),alwaysHitbox(false),numericDream(false),numericPower(false),phaseTimeout(false),phaseTimeoutAsTime(false),hpPhaseMarkers(false),eosdBar(false),phaseStars(false),ringBar(false){}
};
AdvancedOptions& Options();
void LoadAdvancedOptions();void SaveAdvancedOptions();
bool AdvancedOpen();void SetAdvancedOpen(bool open);
void AdvancedKeyEvent(UINT message,WPARAM key,LPARAM data);
void DrawAdvanced();void DrawRuntimeHUD();
const char* AdvancedGlyphs();
unsigned short FilterGameplayInput(unsigned short raw,bool replay);
bool ConsumeLife(int currentLife);
float GameFPS();
bool TakeFastRetryRequest();bool TakeQuickIntro();void SetQuickIntro();
struct ReactionTest {
 enum State { Idle,WaitHold,Waiting,Ready,TooEarly,Result };
 int state,type,trials,frames;bool progress;
 double deadline,delay;std::vector<float> results,frameCounts;
 ReactionTest();void Reset();void Start(double now,double wait,bool newSeries);void Tick(double now,bool arrows);
};
ReactionTest& Reaction();
} }
