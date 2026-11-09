#pragma once
#include <cstdint>
#include <vector>
namespace th5w { class CStage; namespace original {
// Resident counters and integer arithmetic from th05_maine.asm sub_CA9B.
struct RunStats {
 uint32_t frames,slowFrames;uint16_t stdFrames,itemsSpawned,itemsCollected,points,maxPoints,enemiesGone,enemiesKilled,graze;
 uint8_t misses,bombs,legacyUnknown;int highestScore,finalScore,stageScores[6],initialLives,initialBombs,endSequence,stage,difficulty;
 bool turbo,pendingFrame,complete;
 RunStats():frames(0),slowFrames(0),stdFrames(0),itemsSpawned(0),itemsCollected(0),points(0),maxPoints(0),enemiesGone(0),enemiesKilled(0),graze(0),misses(0),bombs(0),legacyUnknown(0),highestScore(0),finalScore(0),stageScores{},initialLives(3),initialBombs(3),endSequence(0),stage(0),difficulty(1),turbo(true),pendingFrame(false),complete(false){}
};
struct Verdict {
 int skill,maximum,row,comment;bool valid;uint32_t fractions[6];
 Verdict():skill(0),maximum(0),row(25),comment(0),valid(true),fractions{}{}
};
RunStats& Stats();Verdict Evaluate(const RunStats&);
// One simulation tick may consume two native VSync intervals in Slow mode.
void BeginTimingFrame();void BulletSlowdown(int bullets,int frame,int difficulty);int FrameDelay();
void BeginRun();void Frame(bool stageScrolling,int score);void PausedFrame();void RenderTiming(bool slow);void EndStage(int stage,int score,bool clear);
void ItemSpawned();void ItemCollected(bool point,bool maxPoint);void EnemyGone(bool killed);void Miss();void Bomb();
struct DemoState {
 bool active,extraPart2;int number,index,frames;uint32_t seed;unsigned char ring[256],ringIndex,itemDropIndex,bulletTemplateAngle;std::vector<unsigned char> data;
 DemoState():active(false),extraPart2(false),number(0),index(0),frames(5000),seed(318),ring{},ringIndex(0),itemDropIndex(0),bulletTemplateAngle(0){}
};
DemoState& Demo();bool DemoActive();bool StartDemo(bool extra=false);void StopDemo(bool resetSequence=false);bool DemoInput(unsigned short live,unsigned short& input);void DemoDialogueEnded();void ResetDemoSeed();int Random();int IRandom();
} }
