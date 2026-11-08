#include "../Game/Game.h"
#include "../Game/Stage/Stage.h"
#include "../Game/Coop/Coop.h"
#include "../Game/Practice/Practice.h"
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <emscripten.h>
extern "C" int th05_coop_start_stock(int,int,int,int,int,int,int,int,int,int,int);
namespace {
std::vector<unsigned char> entry;unsigned serial=0;
struct Codec {
 std::vector<unsigned char>& bytes;bool loading;unsigned offset=0;
 void Value(int& value){uint32_t n=uint32_t(value);if(loading){if(offset+4>bytes.size())throw std::runtime_error("Truncated TH05 stage state");n=0;for(int i=0;i<4;i++)n|=uint32_t(bytes[offset+i])<<(8*i);value=int32_t(n);}else for(int i=0;i<4;i++)bytes.push_back((n>>(8*i))&255);offset+=4;}
 void Flag(bool& value){int n=value;Value(n);if(n<0||n>1)throw std::runtime_error("Invalid TH05 stage flag");value=n!=0;}
 template<class T> void Scalar(T& value){int n=value;Value(n);value=T(n);}
};
#define GAME_FIELDS(X) X(m_nCurLife) X(m_nCurBomb) X(m_curPower) X(m_powerOverflowLevel) X(m_curDream) X(m_curScore) X(m_totalNPointItem) X(m_curNGraze) X(m_playerPerformance) X(m_curHighScore) X(m_nContinueUsed) X(m_nContinueLimit) X(m_pointItemMaxScore) X(m_grazeBonus) X(m_playerPerformanceLowerBound) X(m_playerPerformanceUpperBound) X(m_nLifeOnContinue) X(m_nBombOnMiss)
#define STAT_FIELDS(X) X(frames) X(slowFrames) X(stdFrames) X(itemsSpawned) X(itemsCollected) X(points) X(maxPoints) X(enemiesGone) X(enemiesKilled) X(graze) X(misses) X(bombs) X(legacyUnknown) X(highestScore) X(finalScore) X(initialLives) X(initialBombs) X(endSequence) X(stage) X(difficulty)
struct State {th5w::coop::Session run;int stage=0;int itemRankProgress=0;int game[18]{};int seeds[7]{};th5w::original::RunStats stats;};
// The magic selects the checkpoint layout and combat rules for the
// whole stage, including when opening an older cooperative recording.
constexpr int LegacyStageMagic=0x35535631;
void Wire(Codec& c,State& s){
 int magic=LegacyStageMagic+int(s.run.combatRevision);
 c.Value(magic);
 if(magic<LegacyStageMagic||magic>LegacyStageMagic+int(th5w::original::OriginalMotion))throw std::runtime_error("Unsupported TH05 stage state");
 s.run.combatRevision=unsigned(magic-LegacyStageMagic);
 c.Value(s.run.count);c.Value(s.stage);c.Value(s.run.seed);c.Value(s.run.difficulty);c.Value(s.run.initialSpares);c.Value(s.run.initialBombs);for(auto& n:s.run.characters)c.Value(n);c.Flag(s.run.challenge);c.Flag(s.run.continuing);
 for(auto& stock:s.run.carry){c.Value(stock.lives);c.Value(stock.bombs);c.Value(stock.power);c.Value(stock.overflow);c.Value(stock.fraction);c.Value(stock.deaths);c.Flag(stock.ghost);c.Flag(stock.offline);}
 for(auto& n:s.game)c.Value(n);for(auto& n:s.seeds)c.Value(n);
 #define STAT(name) c.Scalar(s.stats.name);
 STAT_FIELDS(STAT)
 #undef STAT
 for(auto& n:s.stats.stageScores)c.Value(n);c.Flag(s.stats.turbo);c.Flag(s.stats.pendingFrame);c.Flag(s.stats.complete);
 if(s.run.combatRevision>=th5w::original::OriginalMotion)c.Value(s.itemRankProgress);
}
}
namespace th5w::coop {
void CaptureSpectatorStage(){if(Run().count<2)return;State s;s.run=Run();s.stage=CGame::GVar().m_playStage;s.stats=original::Stats();s.itemRankProgress=CGame::GVar().m_itemRankProgress;int at=0;
 #define SAVE(name) s.game[at++]=CGame::GVar().name;
 GAME_FIELDS(SAVE)
 #undef SAVE
 for(int i=0;i<7;i++)s.seeds[i]=CGame::GVar().m_randomSeed[i];entry.clear();Codec codec{entry,false};Wire(codec,s);++serial;
}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE unsigned th05_spectator_serial(){return serial;}
EMSCRIPTEN_KEEPALIVE const void* th05_spectator_state(){return entry.data();}
EMSCRIPTEN_KEEPALIVE unsigned th05_spectator_size(){return entry.size();}
EMSCRIPTEN_KEEPALIVE int th05_spectator_load(const unsigned char* data,unsigned size){try{
 if(!data||size>4096)return 0;std::vector<unsigned char> bytes(data,data+size);State s;Codec codec{bytes,true};Wire(codec,s);
 if(codec.offset!=size||s.itemRankProgress<0||s.itemRankProgress>=32||s.stage<0||s.stage>6||(s.run.difficulty==4)!=(s.stage==6))return 0;
 for(int i=0;i<7;i++)if(s.seeds[i]<0)return 0;
 for(const auto& stock:s.run.carry)if(stock.lives<0||stock.lives>99||stock.bombs<0||stock.bombs>99||stock.power<0||stock.power>128||stock.overflow<0||stock.overflow>42||stock.fraction<0||stock.fraction>1024||stock.deaths<0)return 0;
 // These fields index native bonus tables and affect rank/continue arithmetic.
 if(s.run.seed<0||s.run.seed>65535||s.game[0]<0||s.game[0]>99||s.game[1]<0||s.game[1]>99||
    s.game[2]<0||s.game[2]>128||s.game[3]<0||s.game[3]>42||s.game[4]<0||s.game[4]>128||
    s.game[10]<0||s.game[10]>99||s.game[11]<0||s.game[11]>9||s.game[14]<0||s.game[15]>64||s.game[14]>s.game[15])return 0;
 if(!th05_coop_start_stock(s.run.count,0,s.run.difficulty,s.run.seed,s.run.characters[0],s.run.characters[1],s.run.characters[2],s.run.challenge,0,s.run.initialSpares,s.run.initialBombs))return 0;
 using namespace th5w;auto* game=CGame::s_pCurGame;delete game->m_pCurScheme;game->m_pCurScheme=nullptr;coop::Run()=s.run;coop::Run().local=0;auto& g=CGame::GVar();g.m_playStage=s.stage;g.m_itemRankProgress=s.itemRankProgress;int at=0;
 #define LOAD(name) g.name=s.game[at++];
 GAME_FIELDS(LOAD)
 #undef LOAD
 for(int i=0;i<7;i++)g.m_randomSeed[i]=s.seeds[i];original::Stats()=s.stats;auto* stage=new CStage;game->m_pCurScheme=stage;game->m_curSchemeID=4;stage->Initialize();return 1;
 }catch(const std::exception& e){th5w::web::ReportError(e.what());return 0;}}
}
