#include "Rollback.h"
#include "../Stage/Stage.h"
#include "../Game.h"
#include "../Stage/Chara/CharaReimu.h"
#include "../Stage/Chara/CharaMarisa.h"
#include "../Stage/Chara/CharaMima.h"
#include "../Stage/Chara/CharaYuka.h"
#include "../Stage/CharaBullet/CharaBullet.h"
#include "../Stage/EnemyBullet/EnemyBullet.h"
#include "../Stage/GatherEffect/GatherEffect.h"
#include "../Stage/SparkEffect/SparkEffect.h"
#include "../Stage/PopupNumber/PopupNumber.h"
#include "../Stage/Circle/Circle.h"
#include "../Stage/Item/Item.h"
#include "../Stage/FloatingText/FloatingText.h"
#include "../Stage/Background/Background.h"
#include "../Stage/Enemy/Enemy.h"
#include "../Stage/TextOverlay/TextOverlay.h"
#include "../Stage/Dialogue/Dialogue.h"
#include "../Stage/Laser/Laser.h"
#include "../Stage/Summary/Summary.h"
#include "../Stage/Stage2BG/Stage2BG.h"
#include "../Stage/MidBoss/MidBossStage1.h"
#include "../Stage/MidBoss/MidBossStage2.h"
#include "../Stage/MidBoss/MidBossStage3.h"
#include "../Stage/MidBoss/MidBossStage4.h"
#include "../Stage/MidBoss/MidBossStage5.h"
#include "../Stage/MidBoss/MidBossStageEx.h"
#include "../Stage/Boss/BossStage1.h"
#include "../Stage/Boss/BossStage2.h"
#include "../Stage/Boss/BossStage3.h"
#include "../Stage/Boss/BossStage4.h"
#include "../Stage/Boss/BossStage4Mai.h"
#include "../Stage/Boss/BossStage4Yuki.h"
#include "../Stage/Boss/BossStage5.h"
#include "../Stage/Boss/BossStage6.h"
#include "../Stage/Boss/BossStageEx.h"
#include <stdexcept>

namespace th5w {
bool CStage::RollbackPredictable() const {
 if(!Cooperative()||m_bPaused||m_bContinueScr)return false;
 // These animation ticks only change journaled scalar state. Keep exact-input
 // barriers at their resource/scene transitions, rather than once per frame.
 if(m_bQuit)return m_nQuitFrameLeft>0;
 if(m_bLogo)return m_logoFrame+1<145;
 if(BrowserDialogue())return m_pDialogue->RollbackPredictable();
 // Resource-changing dialogue VMs, actor factories and scene destruction run
 // after the complete preceding input prefix has been reconciled.
 static const int mid[]={2500,2750,5750,3900,4800,300000,5800};
 if(m_curFrame+1==mid[CGame::GVar().m_playStage])return false;
 if(!m_pBoss){
  if(m_bInBossBattle||m_pBackground->GetCurScrollSpeed()==0)return false;
  const int pos=m_pBackground->m_curPos,speed=m_pBackground->m_curSpeed,next=pos+speed;
  if((next+1279)/1280>(pos+1279)/1280&&m_pStageRes->m_pStageStd->m_pBGScrollSpeedArray[(next+1279)/1280+4]==0)return false;
 }
 // Mai/Yuki's branch replaces the combined actor and runs another resource VM.
 if(dynamic_cast<CBossStage4*>(m_pBoss)&&m_pBoss->GetCurPhase()>=0xfd)return false;
 return true;
}

void CStage::CaptureRollback(coop::CaptureSink& sink){
 // Every object below has fixed inline simulation storage. Its referenced DAT,
 // scripts and GPU assets are immutable for the lifetime of a battle interval.
 sink.Object(this);
 for(int seat=0;seat<m_seatCount;seat++){
  auto* player=m_seats[seat].player;
  switch(coop::Run().characters[seat]){
   case 0:sink.Object(static_cast<CCharaReimu*>(player));break;
   case 1:sink.Object(static_cast<CCharaMarisa*>(player));break;
   case 2:sink.Object(static_cast<CCharaMima*>(player));break;
   case 3:sink.Object(static_cast<CCharaYuka*>(player));break;
  }
  sink.Object(m_seats[seat].shots);
 }
 sink.Object(m_pEnemyBullet);sink.Object(m_pGatherEffect);sink.Object(m_pSparkEffect);
 sink.Object(m_pPopupNumber);sink.Object(m_pCircle);sink.Object(m_pItem);
 sink.Object(m_pFloatingText);sink.Object(m_pBackground);sink.Object(m_pEnemy);
 sink.Object(m_pTextOverlay);sink.Object(m_pDialogue);sink.Object(m_pLaser);
 sink.Object(m_pSummary);sink.Object(m_pStage2BG);
#define ACTOR(Type,field) if(auto* actor=dynamic_cast<Type*>(field)){sink.Object(actor);found=true;}
 if(m_pMidBoss){bool found=false;
  ACTOR(CMidBossStage1,m_pMidBoss) else ACTOR(CMidBossStage2,m_pMidBoss) else ACTOR(CMidBossStage3,m_pMidBoss)
  else ACTOR(CMidBossStage4,m_pMidBoss) else ACTOR(CMidBossStage5,m_pMidBoss) else ACTOR(CMidBossStageEx,m_pMidBoss)
  if(!found)throw std::runtime_error("Unknown TH05 rollback midboss");
 }
 if(m_pBoss){bool found=false;
  ACTOR(CBossStage1,m_pBoss) else ACTOR(CBossStage2,m_pBoss) else ACTOR(CBossStage3,m_pBoss)
  else ACTOR(CBossStage4,m_pBoss) else ACTOR(CBossStage4Mai,m_pBoss) else ACTOR(CBossStage4Yuki,m_pBoss)
  else ACTOR(CBossStage5,m_pBoss) else ACTOR(CBossStage6,m_pBoss) else ACTOR(CBossStageEx,m_pBoss)
  if(!found)throw std::runtime_error("Unknown TH05 rollback boss");
 }
#undef ACTOR
 sink.Object(&coop::Run());sink.Object(&original::Stats());
 auto& global=CGame::GVar();
 // CReplay owns vectors; cooperative recording is handled by confirmed inputs,
 // and never included in a raw byte checkpoint.
 sink.Bytes(&global.m_bPracticeMode,unsigned(reinterpret_cast<char*>(&global.m_bCanSaveReplay)+sizeof(global.m_bCanSaveReplay)-reinterpret_cast<char*>(&global.m_bPracticeMode)));
 coop::CaptureRuntime(sink);
}
}
