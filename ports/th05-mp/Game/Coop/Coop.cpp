#include "Coop.h"
#include "../Stage/Stage.h"
#include "../Stage/Chara/Chara.h"
#include "../Stage/Boss/Boss.h"
#include "../Stage/MidBoss/MidBoss.h"
#include "../Stage/CharaBullet/CharaBullet.h"
#include "../Stage/EnemyBullet/EnemyBullet.h"
#include "../Stage/SparkEffect/SparkEffect.h"
#include "../Stage/FloatingText/FloatingText.h"
#include "../Stage/Item/Item.h"
#include "../Stage/Enemy/Enemy.h"
#include "../Stage/Laser/Laser.h"
#include "../Game.h"
#include "../../third_party/imgui/imgui.h"
#include "../../portable/Localization.hpp"
#include "../Practice/Practice.h"
#include "../../gameLib/Graphic/PC98Font.h"
#include "../../gameLib/Graphic/Th5ExtFont.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace th5w {
namespace coop {
Session& Run(){static Session run;return run;} void Reset(){Run()=Session();}
struct ArtFrame {unsigned offset;int width,height;};
#include "PlayerArt.inc"
int Variant(int seat){int occurrence=0;for(int i=0;i<seat;i++)if(Run().characters[i]==Run().characters[seat])++occurrence;return occurrence;}
const unsigned char* ArtColor(int character,int variant){return kArtColors[character][variant];}
C2DImage* ArtImage(int character,int variant,int frame){
 const auto& source=kArtFrames[character][variant][frame];auto* image=C2DImage::Create2DImage(source.width,source.height);auto* out=image->GetDataPointer();
 for(int n=0;n<source.width*source.height;n++)memcpy(out+n*4,kArtPalette[kArtPixels[source.offset+n]],4);
 image->UploadToTexture();return image;
}
void ApplyArt(CStageResource& resources,int character,int variant){
 for(int i=0;i<3;i++)resources.m_patternArray.ReplaceImage(i,ArtImage(character,variant,i));
 for(int i=0;i<16;i++)resources.m_patternArray.ReplaceImage(20+i,ArtImage(character,variant,3+i));
 if(resources.m_pCoopGhost)resources.m_pCoopGhost->Destroy();resources.m_pCoopGhost=ArtImage(character,variant,19);
#ifdef __EMSCRIPTEN__
 resources.RefreshSpriteAtlas();
#endif
}
// Same view-only proximity curve as TH08/TH10 PlayerPresentation.hpp.
float ProximityAlpha(float dx,float dy){
 float distance=std::sqrt(dx*dx+dy*dy);if(distance>=100.f)return 128.f/255;
 distance=std::max(50.f,distance);const float progress=(distance-50.f)/50.f;
 return std::min(128,std::max(0,int(progress*(255-51))+51))/255.f;
}
}


void CStage::SaveSeat(){
 if(!Cooperative())return;
 auto& s=m_seats[m_boundSeat];
 s.stock.lives=m_nCurLife;s.stock.bombs=m_nCurBomb;s.stock.overflow=m_powerOverflowLevel;
 s.stock.power=m_pChara->GetPower();s.current=m_curGameKeyState;s.previous=m_lastGameKeyState;
}
void CStage::BindSeat(int seat){
 if(!Cooperative()||seat==m_boundSeat)return;
 SaveSeat();m_boundSeat=seat;auto& s=m_seats[seat];
 m_pChara=s.player;m_pCharaBullet=s.shots;m_pStageRes=s.resources;
 m_nCurLife=s.stock.lives;m_nCurBomb=s.stock.bombs;m_powerOverflowLevel=s.stock.overflow;
 m_curGameKeyState=s.current;m_lastGameKeyState=s.previous;
 CGame::GVar().m_playChara=coop::Run().characters[seat];
}
void CStage::InitCoop(){
 auto& run=coop::Run();if(run.count==1)return;
 m_seatCount=run.count;m_boundSeat=0;
 m_seats[0].player=m_pChara;m_seats[0].shots=m_pCharaBullet;m_seats[0].resources=m_pStageRes;
 SaveSeat();const auto first=m_seats[0].stock;
 for(int i=1;i<m_seatCount;i++){
  CGame::GVar().m_playChara=run.characters[i];InitChara();
  m_seats[i].player=m_pChara;m_seats[i].shots=new CCharaBullet(this);
  m_seats[i].resources=new CStageResource(this);m_seats[i].stock=first;
 }
 // Restore the initial binding before loading cross-stage stock.
 m_pChara=m_seats[0].player;m_pCharaBullet=m_seats[0].shots;m_pStageRes=m_seats[0].resources;
 CGame::GVar().m_playChara=run.characters[0];
 for(int i=0;i<m_seatCount;i++){
  BindSeat(i);auto& s=m_seats[i];coop::ApplyArt(*s.resources,run.characters[i],coop::Variant(i));
  if(run.continuing){s.stock=run.carry[i];m_nCurLife=s.stock.lives;m_nCurBomb=s.stock.bombs;m_powerOverflowLevel=s.stock.overflow;m_pChara->m_power=s.stock.power;m_pChara->SetPower(s.stock.power);}
  m_pChara->m_curX=(192+(i*2-(m_seatCount-1))*32)*16.f;
  m_pChara->m_lastX=m_pChara->m_curX;m_pChara->m_lastY=m_pChara->m_curY;
  m_pChara->m_bDraw=!s.stock.ghost;
 }
 BindSeat(0);SaveSeat();
}
void CStage::SaveCoop(){
 SaveSeat();for(int i=0;i<m_seatCount;i++)coop::Run().carry[i]=m_seats[i].stock;
 coop::Run().continuing=true;
}
void CStage::StepPlayers(){
 for(int i=0;i<m_seatCount;i++){
  BindSeat(i);auto& s=m_seats[i];s.previous=s.current;s.current=coop::Run().input[i].buttons;
  m_lastGameKeyState=s.previous;m_curGameKeyState=s.current;
  if(s.stock.ghost){
   m_pChara->m_bMiss=false;
   m_pChara->m_curX=m_pChara->m_lastX=(192+(i*2-(m_seatCount-1))*32)*16.f;
   m_pChara->m_curY=m_pChara->m_lastY=320*16.f;m_pChara->m_deltaX=m_pChara->m_deltaY=0;
  }
  else m_pChara->Step();
  m_pCharaBullet->Step();
 }
 BindSeat(0);StepRescue();
}
void CStage::StepPlayerBomb(){
 if(!m_bInCharaBomb)return;const int saved=m_boundSeat;BindSeat(m_bombOwner);m_pChara->StepBomb();BindSeat(saved);
}
bool CStage::TestPlayerCrash(float x,float y,float w,float h){
 if(!Cooperative())return m_pChara->TestCrash(x,y,w,h);
 m_hitSeats=0;
 for(int i=0;i<m_seatCount;i++){
  const auto& s=m_seats[i];const auto* p=s.player;
  if(!s.stock.ghost&&!s.stock.offline&&!p->m_nextLifeComeTimeLeft&&!p->m_invulnerableTimeLeft&&
     std::abs(x-p->m_curX)<=w&&std::abs(y-p->m_curY)<=h)m_hitSeats|=1<<i;
 }
 return m_hitSeats!=0;
}
void CStage::MarkPlayerMiss(){
 if(!Cooperative()){m_pChara->m_bMiss=true;return;}
 for(int i=0;i<m_seatCount;i++)if(m_hitSeats&(1<<i))m_seats[i].player->m_bMiss=true;
 m_hitSeats=0;
}
void CStage::GrazePlayers(float x,float y,unsigned char& mask){
 for(int i=0;i<m_seatCount;i++){
  const auto& s=m_seats[i];auto* p=s.player;
  if(s.stock.ghost||s.stock.offline||p->m_nextLifeComeTimeLeft||(mask&(1<<i)))continue;
  if(std::abs(x-p->m_curX)<=352&&y>=p->m_curY-256&&y<=p->m_curY+320){
   mask|=1<<i;m_pSparkEffect->AddEffect(x,y,32,2,false);
   if(m_curNGraze<999){++m_curNGraze;AddScore(m_grazeBonus);}
  }
 }
}
void CStage::ProtectPlayers(int time){
 if(!Cooperative()){m_pChara->m_invulnerableTimeLeft=time;return;}
 for(int i=0;i<m_seatCount;i++)m_seats[i].player->m_invulnerableTimeLeft=time;
}
void CStage::DisablePlayerBombs(){
 if(!Cooperative()){m_pChara->m_bBombDisabled=true;return;}
 for(int i=0;i<m_seatCount;i++)m_seats[i].player->m_bBombDisabled=true;
}
int CStage::TargetSeat(float x,float y) const {
 int best=0;float distance=std::numeric_limits<float>::max();
 for(int i=0;i<m_seatCount;i++){
  const auto& s=m_seats[i];if(s.stock.ghost||s.stock.offline||s.player->m_nextLifeComeTimeLeft)continue;
  const float dx=x-s.player->m_curX,dy=y-s.player->m_curY,d=dx*dx+dy*dy;
  if(d<distance){best=i;distance=d;}
 }
 return best;
}
float CStage::AimAngle(float x,float y){
 if(!Cooperative())return m_pChara->AngleToChara(x,y);
 return m_seats[TargetSeat(x,y)].player->AngleToChara(x,y);
}
void CStage::ChooseTarget(float x,float y){if(Cooperative())BindSeat(TargetSeat(x,y));}
bool CStage::CollectItem(float x,float y,unsigned char type){
 const int saved=m_boundSeat;
 for(int i=0;i<m_seatCount;i++){
  const auto& s=m_seats[i];auto* p=s.player;
  if(s.stock.ghost||s.stock.offline||p->m_nextLifeComeTimeLeft)continue;
  if(x>=p->m_curX-384&&x<=p->m_curX+384&&y>=p->m_curY-224&&y<=p->m_curY+384){
   BindSeat(i);p->CaughtItem(x,y,type);BindSeat(saved);return true;
  }
 }
 return false;
}
bool CStage::SharePower(float x,float y,unsigned char type){
 if(type!=ITEM_POWER&&type!=ITEM_BIGPOWER)return false;
 int target=m_boundSeat;
 if(m_pChara->GetPower()>=128){
  int power=128;float distance=std::numeric_limits<float>::max();
  for(int i=0;i<m_seatCount;i++){
   const auto& s=m_seats[i];if(s.stock.ghost||s.stock.offline)continue;
   const int p=s.player->GetPower();const float dx=x-s.player->m_curX,dy=y-s.player->m_curY,d=dx*dx+dy*dy;
   if(p<power||(p==power&&p<128&&d<distance)){power=p;distance=d;target=i;}
  }
  if(power==128)return false; // Preserve retail overflow rewards for a full team.
 }
 const int saved=m_boundSeat;BindSeat(target);auto& s=m_seats[target];
 int gain=type==ITEM_BIGPOWER?(m_seatCount==2?15:20):(m_seatCount==2?1:2);
 if(type==ITEM_POWER&&m_seatCount==2){s.stock.fraction^=1;if(!s.stock.fraction)++gain;}
 m_pChara->SetPower(m_pChara->GetPower()+gain);AddScore(1);BindSeat(saved);return true;
}
void CStage::ReviveSeat(int seat,int bombs){
 const int saved=m_boundSeat;BindSeat(seat);auto& s=m_seats[seat];
 s.stock.ghost=false;s.rescue=0;s.rescueAge=0;s.rescueTarget=-1;
 m_nCurLife=1;m_nCurBomb=bombs;m_pChara->m_bDraw=true;m_pChara->m_bMiss=false;
 m_pChara->m_nextLifeComeTimeLeft=0;m_pChara->m_cannotControlTimeLeft=0;m_pChara->m_deltaX=m_pChara->m_deltaY=0;
 m_pChara->m_invulnerableTimeLeft=192;SaveSeat();BindSeat(saved);
}
void CStage::TeamLife(){
 if(coop::Run().challenge)return;
 const int saved=m_boundSeat;
 for(int i=0;i<m_seatCount;i++){if(m_seats[i].stock.offline)continue;BindSeat(i);
  if(m_seats[i].stock.ghost)ReviveSeat(i,1);else m_nCurLife=std::min(99,m_nCurLife+1);
 }
 BindSeat(saved);m_pEnemyBullet->SetClearBulletTimeLeft(20);m_bPlaySound[7]=true;
}
void CStage::TeamBomb(){
 if(coop::Run().challenge)return;
 const int saved=m_boundSeat;
 for(int i=0;i<m_seatCount;i++)if(!m_seats[i].stock.offline&&!m_seats[i].stock.ghost){BindSeat(i);m_nCurBomb=std::min(99,m_nCurBomb+1);}
 BindSeat(saved);
}
void CStage::MakeGhost(){
 auto& s=m_seats[m_boundSeat];s.stock.ghost=true;m_nCurLife=0;m_nCurBomb=0;
 m_pChara->m_curX=m_pChara->m_lastX=(192+(m_boundSeat*2-(m_seatCount-1))*32)*16.f;
 m_pChara->m_curY=m_pChara->m_lastY=320*16;m_pChara->m_deltaX=m_pChara->m_deltaY=0;SaveSeat();
 bool alive=false;for(int i=0;i<m_seatCount;i++)alive|=!m_seats[i].stock.ghost&&!m_seats[i].stock.offline;
 if(!alive){m_bContinueScr=true;m_pauseSelection=0;for(int i=0;i<m_seatCount;i++)m_pauseHeld[i]=coop::Run().input[i].buttons;}
}
void CStage::StepRescue(){
 SaveSeat();
 for(int i=0;i<m_seatCount;i++){
  auto& s=m_seats[i];if(s.stock.offline)continue;
  if(s.rescueAge>0)--s.rescueAge;else{s.rescue=0;s.rescueTarget=-1;}
  if(s.current&32){s.rescue=0;s.rescueTarget=-1;continue;}
  if(!(s.current&64)||(s.previous&64))continue;
  int target=-1;float distance=48*16*48*16.f;
  for(int j=0;j<m_seatCount;j++){
   if(i==j||m_seats[j].stock.offline)continue;
   // A living donor must retain its current life. Ghosts can request rescue;
   // living players can also receive a spare life from a nearby donor.
   const int recipient=s.stock.ghost?i:j;if(!m_seats[recipient].stock.ghost&&m_seats[recipient].stock.lives>=99)continue;
   const int donor=s.stock.ghost?j:i;if(m_seats[donor].stock.ghost||m_seats[donor].stock.lives<=1)continue;
   const float dx=s.player->m_curX-m_seats[j].player->m_curX,dy=s.player->m_curY-m_seats[j].player->m_curY,d=dx*dx+dy*dy;
   if(d<distance){target=j;distance=d;}
  }
  if(target<0){s.rescue=0;s.rescueTarget=-1;continue;}
  if(s.rescueTarget!=target)s.rescue=0;s.rescueTarget=target;s.rescueAge=24;
  if(++s.rescue<5)continue;
  const int donor=s.stock.ghost?target:i,recipient=s.stock.ghost?i:target;
  BindSeat(donor);--m_nCurLife;const bool revive=m_seats[recipient].stock.ghost;
  if(revive)m_nCurBomb/=2;SaveSeat();
  if(revive)ReviveSeat(recipient,1);else{BindSeat(recipient);m_nCurLife=std::min(99,m_nCurLife+1);}
  s.rescue=0;s.rescueTarget=-1;s.rescueAge=0;m_bPlaySound[7]=true;BindSeat(0);
 }
}
int CStage::ShotsDamage(float x,float y,float w,float h,bool boss,bool reflect){
 if(!Cooperative())return m_pCharaBullet->GetDamage(x,y,w,h,boss,reflect);
 int result=0,saved=m_boundSeat;for(int i=0;i<m_seatCount;i++){BindSeat(i);result+=m_pCharaBullet->GetDamage(x,y,w,h,boss,reflect);}BindSeat(saved);return result;
}
unsigned short CStage::CoopMenuInput(){
 m_coopLastMenu=m_coopMenu;m_coopMenu=0;
 for(int i=0;i<m_seatCount;i++){
  auto buttons=coop::Run().input[i].buttons;m_pauseHeld[i]&=buttons;
  m_coopMenu|=buttons&~m_pauseHeld[i];
 }
 return m_coopMenu;
}
void CStage::StepCoopMenu(){
 const unsigned short pressed=m_coopMenu&~m_coopLastMenu;
 if(pressed&1)m_pauseSelection=(m_pauseSelection+2)%3;
 if(pressed&2)m_pauseSelection=(m_pauseSelection+1)%3;
 if(pressed&4096){if(m_bContinueScr)SetQuit(STAGE_END_RESTART);else m_bPaused=false;return;}
 int action=(pressed&32768)?1:(pressed&16384)?2:((pressed&(32|8192))?m_pauseSelection:-1);
 if(action<0)return;
 if(action==0&&!m_bContinueScr)m_bPaused=false;
 else if(action==1||(action==0&&m_bContinueScr))SetQuit(STAGE_END_RESTART);
 else SetQuit(STAGE_END_PLAYER_QUIT);
}
int CStage::SeatValue(int seat,int field){
 if(seat<0||seat>=m_seatCount)return -1;SaveSeat();auto& s=m_seats[seat];
 switch(field){case 0:return s.stock.lives;case 1:return s.stock.bombs;case 2:return s.player->GetPower();case 3:return s.stock.ghost;case 4:return s.rescue;case 5:return int(s.player->m_curX);case 6:return int(s.player->m_curY);case 7:return m_curScore;case 8:return m_totalNPointItem;case 9:return s.stock.offline;case 10:return s.stock.deaths;case 11:return coop::Variant(seat);case 12:return int(PlayerOpacity(seat)*255+.5f);default:return -1;}
}
unsigned int CStage::CoopChecksum(){
 SaveSeat();unsigned int hash=2166136261u;
 auto add=[&](int value){for(int n=0;n<4;n++){hash=(hash^static_cast<unsigned char>(value>>(n*8)))*16777619u;}};
 add(int(coop::Run().random));add(int(coop::Run().random>>32));
 add(CGame::GVar().m_playStage);add(m_curFrame);add(m_curScore);add(m_curDream);add(m_totalNPointItem);add(m_curNGraze);add(m_playerPerformance);add(m_bPaused);add(m_bContinueScr);add(m_bInCharaBomb);add(m_bombOwner);
 for(int i=0;i<m_seatCount;i++){const auto& s=m_seats[i];add(s.stock.lives);add(s.stock.bombs);add(s.stock.ghost);add(s.stock.fraction);add(s.stock.overflow);add(s.stock.deaths);add(s.previous);add(s.current);add(s.rescue);add(s.rescueAge);add(s.rescueTarget);add(s.player->GetPower());add(int(s.player->m_curX*256));add(int(s.player->m_curY*256));}
 for(const auto& laser:m_pLaser->m_laser){add(laser.mode);if(laser.mode){add(int(laser.srcX*256));add(int(laser.srcY*256));add(int(laser.angle*256));add(int(laser.width*256));add(int(laser.lenLower*256));add(int(laser.lenUpper*256));add(laser.age);}}
 for(const auto& enemy:m_pEnemy->m_enemy){add(enemy.flag);if(enemy.flag){add(enemy.age);add(enemy.life);add(int(enemy.curX*256));add(int(enemy.curY*256));add(int(enemy.curSpeed*256));add(int(enemy.curAngle*256));}}
 for(int i=0;i<m_seatCount;i++)for(const auto& shot:m_seats[i].shots->m_bullets){add(shot.flag);if(shot.flag){add(shot.age);add(shot.damage);add(int(shot.curX*256));add(int(shot.curY*256));add(int(shot.velX*256));add(int(shot.velY*256));}}
 if(m_pBoss){add(m_pBoss->m_curPhase);add(m_pBoss->m_curPhaseFrame);add(m_pBoss->m_curHP);add(int(m_pBoss->m_curX*256));add(int(m_pBoss->m_curY*256));}
 if(m_pMidBoss){add(m_pMidBoss->m_curPhase);add(m_pMidBoss->m_curPhaseFrame);add(m_pMidBoss->m_curHP);add(int(m_pMidBoss->m_curX*256));add(int(m_pMidBoss->m_curY*256));}
 for(const auto& b:m_pEnemyBullet->m_bullets){add(b.flag);if(b.flag==1){add(int(b.curX*256));add(int(b.curY*256));add(int(b.curAngle*256));add(b.bornStageAndGrazeFlag);add(b.coopGrazeMask);}}
 return hash;
}
float CStage::PlayerOpacity(int seat) const {
 const auto& s=m_seats[seat];float alpha=s.stock.ghost?80.f/255:1.f;
 if(seat!=coop::Run().local){const auto* local=m_seats[coop::Run().local].player;
  alpha=std::min(alpha,coop::ProximityAlpha((s.player->m_curX-local->m_curX)/16,(s.player->m_curY-local->m_curY)/16));
 }
 return alpha;
}
void CStage::DrawPlayers(int layer){
 const int saved=m_boundSeat;
 if(layer==0){if(m_bInCharaBomb){BindSeat(m_bombOwner);m_pChara->DrawBomb();}BindSeat(saved);return;}
 if(layer==1){
  for(int i=0;i<m_seatCount;i++){BindSeat(i);m_pStageRes->m_playerOpacity=PlayerOpacity(i);m_pCharaBullet->Draw();m_pStageRes->m_playerOpacity=1;}
 }else{
  // All bodies follow bullets/effects; the local player's body is the last body.
  for(int i=0;i<m_seatCount;i++)if(m_seats[i].stock.ghost){
   BindSeat(i);m_pStageRes->m_playerOpacity=PlayerOpacity(i);
   m_pStageRes->DrawStageObjectImage(m_pStageRes->m_pCoopGhost,m_pChara->m_curX,m_pChara->m_curY);m_pStageRes->m_playerOpacity=1;
  }
  for(int n=0;n<m_seatCount;n++){
   const int i=n==m_seatCount-1?coop::Run().local:(n>=coop::Run().local?n+1:n);
   BindSeat(i);if(m_seats[i].stock.ghost)continue;
   m_pStageRes->m_playerOpacity=PlayerOpacity(i);m_pChara->Draw();m_pStageRes->m_playerOpacity=1;
   if(i==coop::Run().local)m_pChara->DrawCollisionArea();
  }
  if(coop::Run().localVisibility){auto* p=m_seats[coop::Run().local].player;m_pStageRes->DrawCircle(p->m_curX,p->m_curY,80,1,1,1,false,.9f);}
 }
 BindSeat(saved);
}
void CStage::DrawCoopStatistics(){
 // PR #143 uses the original 128-pixel sidebar, not an application window.
 // Coordinates retain the original 640x400 image with uth05win's 40-pixel inset.
 SaveSeat();const auto& display=practice::Options();
 const unsigned char high[]={0xd6,0xd7,0xd8,0xd9,0},score[]={0xd7,0xd8,0xd9,0};
 CTh5ExtFont::DrawExtString(high,4,480,88,1,1,0);
 CTh5ExtFont::DrawExtString(score,3,496,120,1,1,0);
 m_pStageRes->DrawNumber(448,104,m_drawHighScore*10,8,true,false,1,1,1);
 m_pStageRes->DrawNumber(448,136,m_drawScore*10,8,true,false,1,1,1);

 auto text=[](const char* label,int x,int y,float red,float green,float blue,float scale=1.f){
  CPC98Font::DrawString(const_cast<char*>(label),100,x,y,red,green,blue,1,true,scale);
 };
 for(int i=0;i<m_seatCount;i++){
  const auto& seat=m_seats[i];const auto* rgb=coop::ArtColor(coop::Run().characters[i],coop::Variant(i));const float color[]={rgb[0]/255.f,rgb[1]/255.f,rgb[2]/255.f};const int y=168+i*32;
  char label[20];snprintf(label,sizeof(label),"%dP",i+1);
  if(!m_bLogo)text(label,int(m_playAreaUpperLeftX+seat.player->m_curX/16)-4,int(m_playAreaUpperLeftY+seat.player->m_curY/16)-32,color[0],color[1],color[2],.5f);
  if(display.numericPower){
   // Compact two-line stocks. The type marker is the seat's actual recolored
   // shot texture, while P1/P2/P3 retain their independent seat colors.
   const float seatColors[][3]={{0,1,1},{1,1,0},{0,1,0}};
   snprintf(label,sizeof(label),"P%d",i+1);text(label,448,y,seatColors[i][0],seatColors[i][1],seatColors[i][2]);
   seat.resources->m_patternArray.GetImagePtr(20)->Draw(472,y);
   seat.resources->m_patternArray.GetImagePtr(36+ITEM_POWER)->Draw(448,y+16);
   snprintf(label,sizeof(label),"%3d/128",seat.player->GetPower());text(label,472,y+16,1,1,1);
   if(seat.stock.ghost)text(seat.stock.offline?"OFFLINE":"GHOST",496,y,.5f,.6f,.8f);
   else{
    if(coop::Run().challenge){text("MISS",504,y,1,1,1);snprintf(label,sizeof(label),"%02d",seat.stock.deaths);}
    else{seat.resources->m_patternArray.GetImagePtr(36+ITEM_1UP)->Draw(528,y);snprintf(label,sizeof(label),"%02d",std::max(0,seat.stock.lives-1));}
    text(label,544,y,1,1,1);
    seat.resources->m_patternArray.GetImagePtr(36+ITEM_BOMB)->Draw(528,y+16);
    snprintf(label,sizeof(label),"%02d",seat.stock.bombs);text(label,544,y+16,1,1,1);
   }
  }else{
  text(label,448,y,color[0],color[1],color[2]);
  seat.resources->m_patternArray.GetImagePtr(20)->Draw(464,y);
  if(seat.stock.ghost){text(seat.stock.offline?"OFFLINE":"GHOST",480,y,.5f,.6f,.8f);}
  else if(coop::Run().challenge){text("MISS",480,y,1,1,1);m_pStageRes->DrawNumber(544,y,seat.stock.deaths,2,false,true,1,1,1);}
  else{
   CTh5ExtFont::DrawExtChar(0xd4,480,y,1,1,1);
   m_pStageRes->DrawNumber(496,y,std::max(0,seat.stock.lives-1),2,false,true,1,1,1);
   CTh5ExtFont::DrawExtChar(0xd3,528,y,1,1,1);
   m_pStageRes->DrawNumber(544,y,seat.stock.bombs,2,false,true,1,1,1);
  }
  m_pStageRes->DrawGauge(seat.player->GetPower(),128,448,y+16,color[0],color[1],color[2]);
  }
  if(seat.rescue){snprintf(label,sizeof(label),"%d/5",seat.rescue);text(label,576,y,color[0],color[1],color[2],.5f);}
 }
 const unsigned char cumulative[]={0xed,0xee,0};
 CTh5ExtFont::DrawExtString(cumulative,2,456,280,0,1,1);
 CTh5ExtFont::DrawExtChar(0xe6,464,296,1,1,0);
 CTh5ExtFont::DrawExtChar(0xe8,464,328,1,1,0);
 m_pStageRes->DrawNumber(496,280,m_totalNPointItem,5,false,true,0,1,1);
 m_pStageRes->DrawNumber(496,296,m_curNPointItem,5,false,true,1,1,1);
 m_pStageRes->DrawNumber(496,328,m_curNGraze,5,false,true,1,1,1);
 if(display.numericDream){CTh5ExtFont::DrawExtChar(0xe7,464,352,1,1,0);m_pStageRes->DrawNumber(528,352,m_curDream,3,false,true,1,1,1);}
 else{
  CTh5ExtFont::DrawExtChar(0xe7,504,344,1,1,0);
  const float dreamColors[][3]={{1,0,0},{1,0,0},{1,0,1},{1,0,1},{0,0,1},{0,0,1},{0,1,1},{1,1,0},{1,1,1}};
  const auto* color=dreamColors[std::min(128,std::max(0,m_curDream))/16];
  m_pStageRes->DrawGauge(m_curDream,128,448,360,color[0],color[1],color[2]);
 }
 if(display.eosdBar||display.ringBar)DrawBossBar();
 else{
  // Move the original Enemy caption and total HP bar below the co-op stocks.
  if(m_pMidBoss)m_pMidBoss->DrawStatistics();if(m_pBoss)m_pBoss->DrawStatistics();
  if(display.phaseTimeout&&!BrowserDialogue()){
   const auto timer=GetPhaseTimeout();if(timer.unit!=practice::PhaseTimeout::None){
    const bool turns=timer.unit==practice::PhaseTimeout::Turns;
    char value[20];snprintf(value,sizeof(value),"%s%d%s",timer.estimated?"~":"",timer.value,turns?"T":"");
    text(value,m_playAreaUpperLeftX+320,m_playAreaUpperLeftY+8,1,timer.value<=(turns?5:10)?.65f:1,timer.value<=(turns?5:10)?.65f:1);
   }
  }
 }
 // Use the original ZUN.COM difficulty artwork and its native color table.
 const unsigned char difficultyGlyphs[][8]={
  {0xae,0xaa,0xbc,0xc2,2,2,2,0},
  {0xb7,0xb8,0xbb,0xb6,0xaa,0xb5,2,0},
  {0xb1,0xaa,0xbb,0xad,2,2,2,0},
  {0xb5,0xbe,0xb7,0xaa,0xbd,0xb2,0xac,0},
  {0xae,0xc1,0xbd,0xbb,0xaa,2,2,0}};
 const float difficultyColors[][3]={{0,1,0},{0,1,1},{1,0,1},{1,0,0},{1,0,0}};
 const int difficulty=CGame::GVar().m_playDifficulty;
 CTh5ExtFont::DrawExtString(difficultyGlyphs[difficulty],100,456,408,
  difficultyColors[difficulty][0],difficultyColors[difficulty][1],difficultyColors[difficulty][2]);
 char fps[24];snprintf(fps,sizeof(fps),"FPS: %.1f",CGame::s_pCurGame->m_fps);text(fps,564,460,1,1,1);
 if(!m_bPaused&&!m_bContinueScr)return;
 ImGui::SetNextWindowPos(ImVec2(115,170),ImGuiCond_Always);ImGui::SetNextWindowSize(ImVec2(235,150),ImGuiCond_Always);
 ImGui::Begin("TH05 Pause",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoNav);
 const char* labels[]={locale::Text(m_bContinueScr?u8"继续":u8"继续游戏"),locale::Text(u8"重新开始"),locale::Text(u8"返回房间")};
 for(int i=0;i<3;i++){
  if(i==m_pauseSelection)ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(.4f,.2f,.6f,1));
  if(ImGui::Button(labels[i],ImVec2(215,35))){
#ifdef __EMSCRIPTEN__
   EM_ASM({Module.eaglerCoopAction?.($0);},i);
#endif
  }
  if(i==m_pauseSelection)ImGui::PopStyleColor();
 }
 ImGui::End();
}
}
