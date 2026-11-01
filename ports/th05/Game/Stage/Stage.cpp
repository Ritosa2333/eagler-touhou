#include ".\stage.h"
#include "../Game.h"
#include "../Practice/Practice.h"
#include "../Practice/KeyMonitor.h"
#include <stdexcept>
#include "Chara/Chara.h"
#include "CharaBullet/CharaBullet.h"
#include "EnemyBullet/EnemyBullet.h"
#include "GatherEffect/GatherEffect.h"
#include "SparkEffect/SparkEffect.h"
#include "PopupNumber/PopupNumber.h"
#include "Circle/Circle.h"
#include "Item/Item.h"
#include "FloatingText/FloatingText.h"
#include "Background/Background.h"
#include "Enemy/Enemy.h"
#include "MidBoss/MidBoss.h"
#include "TextOverlay/TextOverlay.h"
#include "Dialogue/Dialogue.h"
#include "boss/Boss.h"
#include "Laser/Laser.h"
#include "Summary/Summary.h"
#include "Stage2BG/Stage2BG.h"

#include "Chara/CharaReimu.h"
#include "Chara/CharaMarisa.h"
#include "Chara/CharaMima.h"
#include "Chara/CharaYuka.h"
#include "MidBoss/MidBossStage1.h"
#include "MidBoss/MidBossStage2.h"
#include "MidBoss/MidBossStage3.h"
#include "MidBoss/MidBossStage4.h"
#include "MidBoss/MidBossStage5.h"
#include "MidBoss/MidBossStageEx.h"
#include "Boss/BossStage1.h"
#include "Boss/BossStage2.h"
#include "Boss/BossStage3.h"
#include "Boss/BossStage4.h"
#include "Boss/BossStage4Mai.h"
#include "Boss/BossStage4Yuki.h"
#include "Boss/BossStage5.h"
#include "Boss/BossStage6.h"
#include "Boss/BossStageEx.h"
#include "../../gameLib/PMDPlayer/PMDPlayer.h"
#include "../../gameLib/Graphic/Th5ExtFont.h"
#include "../../gameLib/Graphic/PC98Font.h"

#define SAFE_DELETE(x) {if (x) delete x;x=NULL;}

namespace th5w{

CStage::CStage(void)
{
	m_pChara=NULL;
	m_pCharaBullet=NULL;
	m_pEnemyBullet=NULL;
	m_pGatherEffect=NULL;
	m_pSparkEffect=NULL;
	m_pPopupNumber=NULL;
	m_pCircle=NULL;
	m_pItem=NULL;
	m_pFloatingText=NULL;
	m_pBackground=NULL;
	m_pEnemy=NULL;
	m_pTextOverlay=NULL;
	m_pDialogue=NULL;
	m_pLaser=NULL;
	m_pSummary=NULL;
	m_pStage2BG=NULL;

	m_pMidBoss=NULL;
	m_pBoss=NULL;

	m_pStageRes=NULL;

	m_pauseCursorPos=0;
}

CStage::~CStage(void)
{
	SAFE_DELETE(m_pChara);
	SAFE_DELETE(m_pCharaBullet);
	SAFE_DELETE(m_pEnemyBullet);
	SAFE_DELETE(m_pGatherEffect);
	SAFE_DELETE(m_pSparkEffect);
	SAFE_DELETE(m_pPopupNumber);
	SAFE_DELETE(m_pCircle);
	SAFE_DELETE(m_pItem);
	SAFE_DELETE(m_pFloatingText);
	SAFE_DELETE(m_pBackground);
	SAFE_DELETE(m_pEnemy);
	SAFE_DELETE(m_pTextOverlay);
	SAFE_DELETE(m_pDialogue);
	SAFE_DELETE(m_pLaser);
	SAFE_DELETE(m_pSummary);
	SAFE_DELETE(m_pStage2BG);

	SAFE_DELETE(m_pMidBoss);
	SAFE_DELETE(m_pBoss);
	SAFE_DELETE(m_pStageRes);
}

void CStage::UploadVarToGlobal()
{
	CGame::GVar().m_nCurLife=m_nCurLife;
	CGame::GVar().m_nCurBomb=m_nCurBomb;
	CGame::GVar().m_powerOverflowLevel=m_powerOverflowLevel;
	CGame::GVar().m_curDream=m_curDream;
	CGame::GVar().m_curScore=m_curScore;
	CGame::GVar().m_curHighScore=m_highScore;
	CGame::GVar().m_totalNPointItem=m_totalNPointItem;
	CGame::GVar().m_playerPerformance=m_playerPerformance;
	CGame::GVar().m_nContinueUsed=m_nContinueUsed;
	CGame::GVar().m_curPower=m_pChara->GetPower();
}

void CStage::DownloadVarFromGlobal()
{
	//make sure that after creation of CStage, no original::Random() calls before this statement,
	//otherwise, replays may work incorrectly!
	srand(CGame::GVar().m_randomSeed[CGame::GVar().m_playStage]);original::ResetDemoSeed();
	//printf("------------------\n");
	//printf("%d\n",CGame::GVar().m_randomSeed[CGame::GVar().m_playStage]);
	//printf("------------------\n");
	m_nCurLife=CGame::GVar().m_nCurLife;
	m_nCurBomb=CGame::GVar().m_nCurBomb;
	m_powerOverflowLevel=CGame::GVar().m_powerOverflowLevel;
	m_curDream=CGame::GVar().m_curDream;
	m_curScore=CGame::GVar().m_curScore;
	m_highScore=CGame::GVar().m_curHighScore;
	if (m_highScore<m_curScore)			//this may occur if we play a replay file from stage 2+
		m_highScore=m_curScore;
	m_curNPointItem=CGame::GVar().m_practiceConfig.mode?CGame::GVar().m_practiceConfig.points:0;
	m_totalNPointItem=CGame::GVar().m_totalNPointItem;
	m_curNGraze=CGame::GVar().m_practiceConfig.mode?CGame::GVar().m_practiceConfig.graze:0;
	m_playerPerformance=CGame::GVar().m_playerPerformance;
	m_nContinueUsed=CGame::GVar().m_nContinueUsed;

	m_nContinueLimit=CGame::GVar().m_nContinueLimit;
	m_pointItemMaxScore=CGame::GVar().m_pointItemMaxScore;
	m_grazeBonus=CGame::GVar().m_grazeBonus;
	m_playerPerformanceLowerBound=CGame::GVar().m_playerPerformanceLowerBound;
	m_playerPerformanceUpperBound=CGame::GVar().m_playerPerformanceUpperBound;

	m_nLifeOnContinue=CGame::GVar().m_nLifeOnContinue;
	m_nBombOnMiss=CGame::GVar().m_nBombOnMiss;
	if (CGame::GVar().m_playDifficulty==4)
	{
		m_nLifeOnContinue=3;
		m_nBombOnMiss=3;
	}

	//curPower is given to m_pChara in InitChara
}

void CStage::InitChara()
{
	switch(CGame::GVar().m_playChara)
	{
	case 0:
		m_pChara=new CCharaReimu;
		break;
	case 1:
		m_pChara=new CCharaMarisa;
		break;
	case 2:
		m_pChara=new CCharaMima;
		break;
	case 3:
		m_pChara=new CCharaYuka;
		break;
	}
	m_pChara->m_pStage=this;

	m_pChara->m_moveAreaXLow=128.0f;
	m_pChara->m_moveAreaXHigh=6016.0f;
	m_pChara->m_moveAreaYLow=128.0f;
	m_pChara->m_moveAreaYHigh=5632.0f;

	m_pChara->m_curX=3072;
	m_pChara->m_curY=5120;

	m_pChara->m_bBombDisabled=false;
	m_pChara->m_bCharaCanShoot=true;
	m_pChara->m_bCharaCanMove=true;
	m_pChara->m_bMiss=false;
	m_pChara->m_invulnerableTimeLeft=64;
	m_pChara->m_cannotControlTimeLeft=0;
	m_pChara->m_nextLifeComeTimeLeft=0;
	m_pChara->m_charaShootTimer=0;
	m_pChara->m_power=CGame::GVar().m_curPower;		//assign twice to avoid floating text "FullPower"
	m_pChara->SetPower(CGame::GVar().m_curPower);
//	m_pChara->m_power=128;					//TODO: remove these two lines which are only for test use
//	m_pChara->SetPower(128);
	m_pChara->m_getPointItemPerformance=0;
}

void CStage::Initialize()
{
	if (original::DemoActive()) {}
	else if (CGame::GVar().m_bReplayMode==false)
		CGame::GVar().m_replay.OpenStage(CGame::GVar().m_playStage,false);
	else
		CGame::GVar().m_replay.OpenStage(CGame::GVar().m_playStage,true);

	m_bQuit=false;
 m_lockTimer=0;m_lockPhase=-1;m_lockKind=0;m_fastRetryCountdown=0;m_practiceDialoguePending=false;m_bossBar=practice::BossBarAnimation();m_bossRings[0]=m_bossRings[1]=practice::BossRingAnimation();

	m_bLogo=true;
	m_logoFrame=0;

	m_bPaused=false;
	m_bCanPause=true;
	m_bContinueScr=false;

	m_curFrame=-1;					//assign -1 so that when Step() is called the first time,
									//its first statement, which is m_curFrame++ will make it 0

	m_bInCharaBomb=false;
	m_bInCharaBombForItem=false;
	m_bInBossBattle=false;

	m_nMiss=0;
	m_nBomb=0;

	DownloadVarFromGlobal();
	m_drawScore=m_curScore;
	m_drawScoreDelta=0;
	m_drawHighScore=m_highScore;

	m_playAreaUpperLeftX=CGame::s_pCurGame->m_playArea.left;
	m_playAreaUpperLeftY=CGame::s_pCurGame->m_playArea.top;

	m_pStageRes=new CStageResource(this);

	InitChara();
	m_pCharaBullet=new CCharaBullet(this);
	m_pEnemyBullet=new CEnemyBullet(this);
	m_pGatherEffect=new CGatherEffect(this);
	m_pSparkEffect=new CSparkEffect(this);
	m_pPopupNumber=new CPopupNumber(this);
	m_pCircle=new CCircle(this);

	m_pItem=new CItem(this);
	m_pFloatingText=new CFloatingText(this);
	m_pBackground=new CBackground(this);
	m_pEnemy=new CEnemy(this);
	m_pTextOverlay=new CTextOverlay(this);
	m_pDialogue=new CDialogue(this);
	m_pLaser=new CLaser(this);
	m_pSummary=new CSummary(this);
	if (CGame::GVar().m_playStage==1)
		m_pStage2BG=new CStage2BG(this);

	m_curScrFade=100;

	memset(m_bPlaySound,0,sizeof(m_bPlaySound));

	//CPMDPlayer::Pause();

	CGame::s_pCurGame->m_fpsLimit=56;

	m_lastGameKeyState=0;
	m_curGameKeyState=0;
	ApplyPracticeWarp();
 if(practice::TakeQuickIntro()&&!CGame::GVar().m_bReplayMode){
  if(m_bLogo){char bgm[]="ST00.M2";bgm[3]=char('0'+CGame::GVar().m_playStage);CPMDPlayer::UnloadPMDData();CCommonFunctionMusicSE::LoadMusicToPMDFromDat(&CGame::s_pCurGame->m_th5Dat2,bgm);CPMDPlayer::SetVolume(100);CPMDPlayer::Play();}
  m_bLogo=false;
 }
 GetPhaseTimeout();
 CGame::GVar().OnStageEnter();
}

void CStage::ApplyPracticeWarp()
{
 const auto& p=CGame::GVar().m_practiceConfig;
 if(!CGame::GVar().m_bPracticeMode||!p.mode||!p.warp||(p.stage==5&&p.warp==1))return;
 const int midFrames[]={2500,2750,5750,3900,4800,300000,5800};
 const int target=p.warp==1?midFrames[CGame::GVar().m_playStage]:p.warp==3?p.frame:60000;
 // Do not run combat, collect items, change resources or record fake inputs.
 // Advance the original background (including stage 2's special renderer).
 for(int frame=1;frame<target;frame++){
  m_curFrame=frame;
  if(m_pBackground->GetCurScrollSpeed()!=0)m_pBackground->Step();
  if(p.warp==2&&m_pBackground->GetCurScrollSpeed()==0){m_curFrame=frame-1;break;}
  if(m_pStage2BG)m_pStage2BG->Step();
 }
 if(p.warp!=2)m_curFrame=target-1;
 // Skip expired enemy-table records; leaving the cursor at stage start would
 // prevent all subsequent spawns because the native reader matches exact frames.
 unsigned char*& table=m_pEnemy->m_pEnemyTableReadPtr;
 while((table[0]||table[1])&&(table[0]+table[1]*256)<=m_curFrame)table+=3+table[2]*8;
 m_bLogo=false;m_pTextOverlay->SetMode(TEXT_OVERLAY_NONE);m_curScrFade=100;
 char bgm[]="ST00.M2";bgm[3]=char('0'+CGame::GVar().m_playStage);
 CPMDPlayer::UnloadPMDData();CCommonFunctionMusicSE::LoadMusicToPMDFromDat(&CGame::s_pCurGame->m_th5Dat2,bgm);CPMDPlayer::SetVolume(100);CPMDPlayer::Play();
 if(p.warp==2){
  // Execute the original dialogue VM so boss graphics, palettes and music are
  // loaded in the original order. Holding Ctrl skips only its visible text.
  CheckBoss();
  const auto* section=practice::SelectedSection(p.stage,p.warp,p.section);
  if(p.dialogue&&(!section||!section->variant)){
   m_practiceDialoguePending=p.section!=0;
   memset(m_bPlaySound,0,sizeof(m_bPlaySound));return;
  }
  m_curGameKeyState=128;
  for(int i=0;i<10000&&m_pDialogue->IsRunning();i++)m_pDialogue->Step();
  m_curGameKeyState=0;CheckBoss();m_curScrFade=100;
 }
 ApplyPracticeSection();
 memset(m_bPlaySound,0,sizeof(m_bPlaySound));
}

void CStage::ApplyPracticeSection()
{
 const auto& p=CGame::GVar().m_practiceConfig;
 const auto* selected=practice::SelectedSection(p.stage,p.warp,p.section);
 if(!selected)return;
 // Only initialization calls this path. It never records synthetic key frames.
 struct Guard {Guard(){practice::PreparingSection(true);}~Guard(){practice::PreparingSection(false);}} guard;
 const int score=m_curScore,high=m_highScore,rank=m_playerPerformance;
 if(p.warp==1){++m_curFrame;CheckMidBoss();}
 const int stageFrame=m_curFrame;
 const auto variant=[&](){return dynamic_cast<CBossStage4Mai*>(m_pBoss)?1:dynamic_cast<CBossStage4Yuki*>(m_pBoss)?2:0;};
 const auto actualReady=[&](){return p.warp==1?m_pMidBoss&&m_pMidBoss->m_curPhase==selected->phase:m_pBoss&&variant()==selected->variant&&m_pBoss->m_curPhase==selected->phase;};
 const auto branchDialogue=[&](){return p.dialogue&&selected->variant&&variant()==0&&m_pDialogue->IsRunning();};
 const auto ready=[&](){return actualReady()||branchDialogue();};
 const auto step=[&](){
  ++m_curFrame;
  if(m_pDialogue->IsRunning()){m_curGameKeyState=128;m_pDialogue->Step();m_curGameKeyState=0;return;}
  if(p.warp==1){if(!m_pMidBoss||m_pMidBoss->Step())throw std::runtime_error("Practice midboss phase ended before destination");}
  else {if(!m_pBoss)throw std::runtime_error("Practice boss is missing");m_pBoss->Step();}
 };
 m_practiceSeekDamage=p.warp==1&&p.stage==1&&selected->phase==2&&p.trigger?2:1;
 int ticks=0;while(!ready()&&ticks++<20000)step();
 if(!ready()){m_practiceSeekDamage=0;throw std::runtime_error("Practice phase destination was not reached");}
 m_practiceDialoguePending=branchDialogue();
 m_practiceSeekDamage=2;
 // Expired attacks and rewards are discarded; native actor state, palettes,
 // background state and the RNG reached through original transitions remain.
 memset(m_pEnemyBullet->m_bullets,0,sizeof(m_pEnemyBullet->m_bullets));
 m_pEnemyBullet->m_noEnemyBullet=false;m_pEnemyBullet->m_clearEnemyBulletTimeLeft=0;
 m_pLaser->Initialize();
 if(!m_practiceDialoguePending){
 int hp=selected->startHP;
 if(p.warp==1&&p.stage==1&&selected->phase==2&&p.trigger)hp=1400;
 if(p.startHP>=0&&selected->continuous)hp=p.startHP;
 if(p.rage){
  if(p.warp==1){hp=p.stage==0?200:p.stage==1?(p.rage==1?800:600):999;}
  else if(p.stage==3)hp=p.rage==1?2750:1125;
  else if(p.stage==4)hp=selected->endHP+(selected->phase==4?599:499);
  else if(p.stage==5&&!p.trigger)hp=p.rage==1?5600:3800;
 }
 if(p.startHP>=0&&selected->continuous)hp=p.startHP;
 if(p.warp==1)m_pMidBoss->m_curHP=hp;
 else {
  m_pBoss->m_curHP=hp;m_pBoss->m_curPhaseEndHP=selected->endHP;
  if(p.stage==3&&selected->variant==0)static_cast<CBossStage4*>(m_pBoss)->m_2curHP=hp;
 }
 // Yumeko initializes both aimed-shot angles at frame 64. Do not lower HP
 // until that native initialization has run, or early rage reads stale angles.
 if(p.warp==2&&p.stage==4&&p.rage){
  m_pBoss->m_curHP=selected->startHP;
  while(m_pBoss->m_curPhaseFrame<64)step();
  m_pBoss->m_curHP=hp;
 }
 const auto rageReady=[&](){
  if(!p.rage)return true;
  if(p.warp==1)return p.stage!=6||static_cast<CMidBossStageEx*>(m_pMidBoss)->m_phase1CurSubphase==1;
  if(p.stage==3)return m_pBoss->m_nModeChange>=(p.rage==1?10:15);
  if(p.stage==4){auto* boss=static_cast<CBossStage5*>(m_pBoss);return boss->m_curPhaseFrame>=32&&(selected->phase==4?boss->m_phase4CrazyMode:boss->m_phase7CrazyMode);}
  if(p.stage==5){auto* boss=static_cast<CBossStage6*>(m_pBoss);return p.rage==1?boss->m_phase10Timer>=65:boss->m_phase10Timer>65;}
  return true;
 };
 ticks=0;while(!rageReady()&&ticks++<10000)step();
 if(!rageReady()||!ready()){m_practiceSeekDamage=0;throw std::runtime_error("Practice rage destination was not reached");}
 }
 m_practiceSeekDamage=0;m_curFrame=stageFrame;
 if(m_pBoss){m_pBoss->m_lastDrawnHP=0;m_pBoss->m_bHitThisFrame=false;m_pBoss->m_HPLostThisFrame=0;}
 if(m_pMidBoss)m_pMidBoss->m_lastDrawnHP=0;
 memset(m_pEnemyBullet->m_bullets,0,sizeof(m_pEnemyBullet->m_bullets));m_pEnemyBullet->m_clearEnemyBulletTimeLeft=0;
 if(auto* boss=dynamic_cast<CBossStage4*>(m_pBoss))boss->m_curvingBullet.Initialize();
 if(auto* boss=dynamic_cast<CBossStage4Mai*>(m_pBoss))boss->m_largeBullet.Initialize();
 if(auto* boss=dynamic_cast<CBossStage4Yuki*>(m_pBoss))boss->m_largeBullet.Initialize();
 if(auto* boss=dynamic_cast<CBossStage5*>(m_pBoss))boss->m_knifeBullet.Initialize();
 if(auto* boss=dynamic_cast<CBossStage6*>(m_pBoss)){boss->m_curvingBullet.Initialize();boss->m_shinkiBullet.Initialize();}
 if(auto* boss=dynamic_cast<CBossStageEx*>(m_pBoss))boss->m_curvingBullet.Initialize();
 m_pItem->Initialize();m_pPopupNumber->Initialize();m_pGatherEffect->Initialize();m_pSparkEffect->Initialize();m_pCircle->Initialize();
 m_curScore=score;m_highScore=high;m_drawScore=score;m_drawHighScore=high;m_playerPerformance=rank;
 m_pChara->m_bMiss=false;m_pChara->m_invulnerableTimeLeft=64;
 m_pChara->m_bBombDisabled=false;m_pChara->m_bCharaCanShoot=m_pChara->m_bCharaCanMove=true;
 m_curScrFade=100;m_pTextOverlay->SetMode(TEXT_OVERLAY_NONE);
}

void CStage::StepScore()
{
	if (m_drawScore>m_curScore)
	{
		m_drawScore=m_curScore;
		return;
	}
	if (m_drawScore==m_curScore)
		return;
	if (m_drawScoreDelta>m_curScore-m_drawScore)
		m_drawScoreDelta=m_curScore-m_drawScore;
	int curDelta=(m_curScore-m_drawScore)/32;
	if (curDelta==0) curDelta=1;
	if (curDelta>6111) curDelta=6111;
	if (m_drawScoreDelta<curDelta)
		m_drawScoreDelta=curDelta;
	m_drawScore+=m_drawScoreDelta;
	if (m_drawScore>m_drawHighScore)
		m_drawHighScore=m_drawScore;

	//unlike PC98 ver, we don't trigger "hiscore" floating text here, because this may cause incorrect "hiscore"
	//during replay.
}

void CStage::CheckMidBoss()
{
	//for stage 6(index 5), PC98 ver uses 30000. However, 30000 frame just lasts 9 minutes.
	//To be safe, it is changed to 300000 here.
	const int midBossAppearFrame[]={2500,2750,5750,3900,4800,300000,5800};
	if (m_curFrame==midBossAppearFrame[CGame::GVar().m_playStage])
	{
		switch(CGame::GVar().m_playStage)
		{
		case 0:
			m_pMidBoss=new CMidBossStage1;
			break;
		case 1:
			m_pMidBoss=new CMidBossStage2;
			break;
		case 2:
			m_pMidBoss=new CMidBossStage3;
		    break;
		case 3:
			m_pMidBoss=new CMidBossStage4;
		    break;
		case 4:
			m_pMidBoss=new CMidBossStage5;
			break;
		case 6:
			m_pMidBoss=new CMidBossStageEx;
			break;
		}
		if (m_pMidBoss!=NULL)
		{
			m_pMidBoss->m_pStage=this;
			m_pMidBoss->Initialize();
			m_pEnemy->m_noAddEnemy=true;
		}
	}
}

void CStage::CheckBoss()
{
	if (m_pBackground->GetCurScrollSpeed()==0)
	{
		if (m_bInBossBattle==false)
		{
			m_bInBossBattle=true;
			m_pDialogue->Run();
		}
		else
			if (m_pDialogue->IsRunning()==false&&m_pBoss==NULL)
			{
				switch(CGame::GVar().m_playStage)
				{
				case 0:
					m_pBoss=new CBossStage1;
					break;
				case 1:
					m_pBoss=new CBossStage2;
					break;
				case 2:
					m_pBoss=new CBossStage3;
					break;
				case 3:
					m_pBoss=new CBossStage4;
					break;
				case 4:
					m_pBoss=new CBossStage5;
					break;
				case 5:
					m_pBoss=new CBossStage6;
					break;
				case 6:
					m_pBoss=new CBossStageEx;
					break;
				}
				if (m_pBoss!=NULL)
				{
					m_pBoss->m_pStage=this;
					m_pBoss->Initialize();
				}
				m_pTextOverlay->SetMode(TEXT_OVERLAY_DRAW_STAGE_NAME_BGM_NAME,NULL,m_pStageRes->m_pStageStd->m_stageBossBGMName);
			}
	}
}

int CStage::Step()
{
	return StepFrame(CCommonFunctionInput::GetAllKeyState());
}

int CStage::StepFrame(unsigned short liveInput)
{
 original::BeginTimingFrame();
 if(practice::AdvancedOpen())return 0;
	CGame::s_pCurGame->m_fpsLimit=practice::GameFPS();
	CGame::s_pCurGame->SetVSYNC(practice::GameFPS()==56&&CGame::GVar().m_runTurboMode);
 if(practice::TakeFastRetryRequest())RequestFastRetry();
 if(m_fastRetryCountdown>0){
  if(--m_fastRetryCountdown==0){m_bPaused=false;SetQuit(STAGE_END_RESTART);m_nQuitFrameLeft=0;practice::SetQuickIntro();}
  return 0;
 }


	if (m_bQuit)
	{
		if (m_nQuitFrameLeft>0)
		{
			m_nQuitFrameLeft--;
			CPMDPlayer::SetVolume((float)(m_nQuitFrameLeft*2));
			m_curScrFade-=2;
			if (m_curScrFade<=0)
				m_curScrFade=0;
			return 0;
		}
		CPMDPlayer::Pause();
		CPMDPlayer::SetVolume(100);
		if (m_quitCode==STAGE_END_CLEAR && CGame::GVar().m_bPracticeMode && !CGame::GVar().m_bReplayMode)
            CGame::GVar().m_bCanSaveReplay=true;
		UploadVarToGlobal();
 original::EndStage(CGame::GVar().m_playStage,m_curScore,m_quitCode==STAGE_END_CLEAR);
		//make sure to upload var before close stage
		//otherwise, replay will not get the correct score
		if (original::DemoActive()) {}
		else if (m_quitCode==STAGE_END_CLEAR)
		{
			CGame::GVar().m_replay.CloseStage(true);
		}
		else
			CGame::GVar().m_replay.CloseStage(false);
		if (CGame::GVar().m_bReplayMode)
			if (m_quitCode!=STAGE_END_RESTART && (CGame::GVar().m_bPracticeMode || m_quitCode!=STAGE_END_CLEAR||CGame::GVar().m_playStage>=5))
				m_quitCode=STAGE_END_REPLAY_END;
		return m_quitCode;
	}

	if (m_bLogo)
	{
		if (CGame::GVar().m_bReplayMode)
			if (CCommonFunctionInput::LCtrlKey(liveInput))
				CGame::s_pCurGame->m_fpsLimit=1000;
		if (StepLogo()==0)
			return 0;
		m_bLogo=false;

		if(!original::DemoActive()||original::Demo().number==5){
		char bgmFileName[]="ST00.M2";
		bgmFileName[3]=CGame::GVar().m_playStage+48;
		CPMDPlayer::UnloadPMDData();
		CCommonFunctionMusicSE::LoadMusicToPMDFromDat(&CGame::s_pCurGame->m_th5Dat2,bgmFileName);
		CPMDPlayer::SetVolume(100);
		CPMDPlayer::Play();
  }

		m_bCanPause=true;
	}

	if (m_bPaused)
	{
		original::PausedFrame();StepPause();
		return 0;
	}

	if (m_bContinueScr)
	{
		StepContinue();
		return 0;
	}

 if(original::DemoActive()){
  if(m_pDialogue->IsRunning()){
   if(original::Demo().extraPart2){SetQuit(STAGE_END_PLAYER_QUIT);return 0;}
   m_curGameKeyState=128;for(int i=0;i<10000&&m_pDialogue->IsRunning();i++)m_pDialogue->Step();m_curGameKeyState=0;original::DemoDialogueEnded();return 0;
  }
  m_lastGameKeyState=m_curGameKeyState;if(!original::DemoInput(liveInput,m_curGameKeyState)){SetQuit(STAGE_END_PLAYER_QUIT);return 0;}
 }else if (CGame::GVar().m_bReplayMode==false)
 {
  m_lastGameKeyState=m_curGameKeyState;
		m_curGameKeyState=liveInput;
		m_lastMenuKeyState=m_lastGameKeyState;
		m_curMenuKeyState=m_curGameKeyState;
		if (CCommonFunctionInput::ESCPressed(m_curMenuKeyState,m_lastMenuKeyState))
		{
			if (Pause())
				return 0;
		}
	}
	else
	{
		m_lastMenuKeyState=m_curMenuKeyState;
		m_curMenuKeyState=liveInput;
		if (CCommonFunctionInput::ESCPressed(m_curMenuKeyState,m_lastMenuKeyState))
		{
			if (Pause())
				return 0;
		}

		m_lastGameKeyState=m_curGameKeyState;
		if (CGame::GVar().m_replay.GetNextData(&m_curGameKeyState)==false)
		{
			SetQuit(STAGE_END_REPLAY_END);
			return 0;
		}
	}

	if (CGame::GVar().m_bReplayMode==false&&!original::DemoActive()){
  m_curGameKeyState=practice::FilterGameplayInput(m_curGameKeyState,false);
		CGame::GVar().m_replay.PutNextdata(m_curGameKeyState);
 }

	if (CGame::GVar().m_bReplayMode)
		if (CCommonFunctionInput::LCtrlKey(m_curMenuKeyState))
		{
			CGame::s_pCurGame->SetVSYNC(false);
			CGame::s_pCurGame->m_fpsLimit=1000;
		}

	if (m_pDialogue->Step()==0){TickBossBar();return 0;}

	original::Frame(m_pBackground->GetCurScrollSpeed()!=0,m_curScore);
	m_curFrame++;

	if (m_curFrame>0)
	{
		m_pBackground->Step();

		for (int i=1;i<=15;i++)
			if (m_bPlaySound[i])
				CGame::s_pCurGame->m_soundEffect.PlaySound(i);
		memset(m_bPlaySound,0,sizeof(m_bPlaySound));

  const int performanceFrame=original::DemoActive()&&original::Demo().extraPart2?original::Demo().index-1:m_curFrame;
		if (performanceFrame>0&&performanceFrame%4096==0)
			ChangePlayerPerformance(1);

		StepScore();
	}

	CheckBoss();
	CheckMidBoss();

	m_pEnemy->StepEnemyTable();

	m_pPopupNumber->Step();
	m_pCircle->Step();
	m_pSparkEffect->Step();
	m_pChara->Step();
	m_pCharaBullet->Step();
	m_pLaser->Step();
	m_pEnemyBullet->Step();
	m_pEnemy->Step();
 GetPhaseTimeout(); // Capture a newly enabled F6 before the native attack advances.
	if (m_pMidBoss)
		if (m_pMidBoss->Step()!=0)
		{
			delete m_pMidBoss;
			m_pMidBoss=NULL;
		}
	if (m_pBoss)
		m_pBoss->Step();
 if(m_practiceDialoguePending&&!m_pDialogue->IsRunning()&&m_pBoss){
  const auto& config=CGame::GVar().m_practiceConfig;
  const auto* section=practice::SelectedSection(config.stage,config.warp,config.section);
  const int variant=dynamic_cast<CBossStage4Mai*>(m_pBoss)?1:dynamic_cast<CBossStage4Yuki*>(m_pBoss)?2:0;
  if(section&&variant==section->variant){m_practiceDialoguePending=false;ApplyPracticeSection();memset(m_bPlaySound,0,sizeof(m_bPlaySound));}
 }
	m_pSummary->Step();
	m_pItem->Step();
	m_pGatherEffect->Step();
 // Native stage_render runs after items/gather and before the bomb callback.
	if (m_pStage2BG)
		m_pStage2BG->Step();
	if (m_bInCharaBomb)
		m_pChara->StepBomb();
 m_pLaser->FinishDemoFrame();
	m_pTextOverlay->Step();
	m_pFloatingText->Step();
 if(practice::Options().showKeys)practice::RecordKey(5,m_curGameKeyState);
 TickLockTimer();TickBossBar();

	return 0;
}

void CStage::RequestFastRetry(){
 if(practice::Options().fastRetry&&CGame::GVar().m_bPracticeMode&&!CGame::GVar().m_bReplayMode&&!m_bQuit&&!m_fastRetryCountdown){m_fastRetryCountdown=15;m_bPaused=true;practice::FocusPause();}
}
void CStage::TickLockTimer(){
 const int kind=m_pBoss?2:m_pMidBoss?1:0;
 const int phase=m_pBoss?m_pBoss->m_curPhase:m_pMidBoss?m_pMidBoss->m_curPhase:-1;
 if(!kind||phase>=0xfd)m_lockTimer=0;
 else if(kind!=m_lockKind||phase!=m_lockPhase)m_lockTimer=1;
 else ++m_lockTimer;
 m_lockKind=kind;m_lockPhase=phase;
}

void CStage::Draw()
{
	if (m_bLogo)
	{
		DrawLogo();
		return;
	}

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);

	if (m_pBoss==NULL)
	{
		m_pBackground->Draw();
		if (m_pStage2BG)
			m_pStage2BG->Draw();
	}
	else
		m_pBoss->DrawBackground();

	if (m_bInCharaBomb)
		m_pChara->DrawBomb();

	if (m_pBoss)
		m_pBoss->Draw();
	if (m_pMidBoss)
		m_pMidBoss->Draw();
	m_pEnemy->Draw();

 // Keep health rings above large boss sprites (including Shinki wings),
 // while player shots, bullets and lasers remain in front of the indicator.
 if(practice::Options().ringBar)DrawBossRings();

	m_pCharaBullet->Draw();
	m_pChara->Draw();
	if (m_pBoss)
		m_pBoss->DrawSpecialBullet();
	m_pLaser->Draw();
	m_pGatherEffect->Draw();
	m_pSparkEffect->Draw();
	m_pItem->Draw();
	m_pPopupNumber->Draw();
	m_pEnemyBullet->Draw();
	m_pCircle->Draw();
	m_pChara->DrawCollisionArea();
	m_pTextOverlay->Draw();
	m_pDialogue->Draw();

	//cut pixels drawn outside play area
	CCommonFunctionGraphic::DrawRectangle(0,0,639,(float)m_playAreaUpperLeftY-1,0,0,0);
	CCommonFunctionGraphic::DrawRectangle(0,(float)m_playAreaUpperLeftY+368,639,479,0,0,0);
	CCommonFunctionGraphic::DrawRectangle(0,0,(float)m_playAreaUpperLeftX-1,479,0,0,0);
	CCommonFunctionGraphic::DrawRectangle((float)m_playAreaUpperLeftX+384,0,639,479,0,0,0);

	PlayAreaFade((float)m_curScrFade);

	m_pFloatingText->Draw();
	m_pSummary->Draw();

	DrawStatistics();

	if (m_bPaused)
		DrawPause();

	if (m_bContinueScr)
		DrawContinue();

	//CCommonFunctionGraphic::ScreenFade((float)m_curScrFade);
}

void CStage::EnableContinueScreen()
{
 if(original::DemoActive()){SetQuit(STAGE_END_PLAYER_QUIT);return;}
	if (m_bContinueScr==false)
	{
		m_bContinueScr=true;
		m_continueScrCurPart=0;
		m_continueScrTimer=32;
	}
}

void CStage::StepContinue()
{
	m_lastMenuKeyState=m_curMenuKeyState;
	m_curMenuKeyState=CCommonFunctionInput::GetAllKeyState();
	switch(m_continueScrCurPart)
	{
	case 0:
		if (m_continueScrTimer>0)
			m_continueScrTimer--;
		else
			m_continueScrCurPart++;
		break;
	case 1:
		if (m_continueScrTimer==0)
			m_curScrFade=50;
		if (m_continueScrTimer<31)
			m_continueScrTimer++;
		else
		{
			m_continueScrCurPart++;
			m_continueStrX=52;
		}
		break;
	case 2:
		if (m_continueStrX>8)
			m_continueStrX-=2;
		else
		{
			m_continueScrCurPart++;
			m_continueStrX=8;
		}
		break;
	case 3:
		if (m_continueStrX<20)
			m_continueStrX+=2;
		else
		{
			m_continueScrCurPart++;
			m_continueStrX=20;
		}
		break;
	case 4:
		if (CCommonFunctionInput::ZPressed(m_curMenuKeyState,m_lastMenuKeyState))
		{
			m_continueScrCurPart++;
			m_continueCursorPos=0;
			m_continueScrTimer=32;
			if (CGame::GVar().m_playStage==6||m_nContinueLimit-m_nContinueUsed<=0)
			{
				m_continueScrCurPart++;
				m_bContinued=false;
			}
		}
		break;
	case 5:
		if (CCommonFunctionInput::UpPressed(m_curMenuKeyState,m_lastMenuKeyState)
			||CCommonFunctionInput::DownPressed(m_curMenuKeyState,m_lastMenuKeyState))
			m_continueCursorPos=1-m_continueCursorPos;
		if (CCommonFunctionInput::ZPressed(m_curMenuKeyState,m_lastMenuKeyState))
		{
			m_bContinued=(m_continueCursorPos==0);
			m_continueScrCurPart++;
		}
		break;
	case 6:
		if (m_continueScrTimer==32)
			if (m_bContinued==false)
			{
				SetQuit(STAGE_END_ALL_MISSED);
				m_bContinueScr=false;
			}
		if (m_continueScrTimer>0)
			m_continueScrTimer--;
		else
			m_continueScrCurPart++;
		break;
	case 7:
		if (m_continueScrTimer==0)
			m_curScrFade=100;
		if (m_continueScrTimer<31)
			m_continueScrTimer++;
		else
		{
			m_bContinueScr=false;
			m_pChara->SetPower(1);
			SetDream(1);
			m_nCurLife=m_nLifeOnContinue;
			m_nBomb=m_nBombOnMiss;
			m_curScore=0;
			//unlike PC98 ver, we do not update global highscore here.
			//like PC98 ver, we do not update graze here
			m_nContinueUsed++;
			m_pChara->EnableDraw();

			CGame::GVar().m_bCanSaveReplay=false;
		}
		break;
	}
}

void CStage::DrawContinue()
{
	switch(m_continueScrCurPart)
	{
	case 0:
	case 1:
	case 6:
	case 7:
		{
			int fontIdx=0x40-1-m_continueScrTimer/4;
			unsigned char s[25];
			for (int i=0;i<24;i++)
				s[i]=fontIdx;
			s[24]=0;
			for (int i=0;i<23;i++)
				CTh5ExtFont::DrawExtString(s,24,m_playAreaUpperLeftX,i*16+m_playAreaUpperLeftY,0,0,0);
		}
		break;
	case 2:
	case 3:
		CTh5ExtFont::DrawExtChar(0xb0,m_continueStrX*8-32+m_playAreaUpperLeftX,0x0c*16-16+m_playAreaUpperLeftY,1,1,1);
		break;
	case 4:
		{
			unsigned char extGameOver[]={0xb0,0xaa,0xb6,0xae,0xb8,0xbf,0xae,0xbb,0};
			CTh5ExtFont::DrawExtString(extGameOver,100,0x14*8-32+m_playAreaUpperLeftX,0x0c*16-16+m_playAreaUpperLeftY,1,1,1);
		}
		break;
	case 5:
		{
			unsigned char extContinue[]={0xac,0xb8,0xb7,0xbd,0xb2,0xb7,0xbe,0xae,0x8,0};
			CTh5ExtFont::DrawExtString(extContinue,100,0x13*8-32+m_playAreaUpperLeftX,0x0a*16-16+m_playAreaUpperLeftY,1,1,1);
			unsigned char extYes[]={0xc2,0xae,0xbc,0};
			unsigned char extNo[]={0xb7,0xb8,0};
			if (m_continueCursorPos==0)
			{
				CTh5ExtFont::DrawExtString(extYes,100,0x18*8-32+m_playAreaUpperLeftX,0x0d*16-16+m_playAreaUpperLeftY,0,1,0,1.0,true,true);
				CTh5ExtFont::DrawExtString(extNo,100,0x19*8-32+m_playAreaUpperLeftX,0x0f*16-16+m_playAreaUpperLeftY,1,1,1);
			}
			else
			{
				CTh5ExtFont::DrawExtString(extYes,100,0x18*8-32+m_playAreaUpperLeftX,0x0d*16-16+m_playAreaUpperLeftY,1,1,1);
				CTh5ExtFont::DrawExtString(extNo,100,0x19*8-32+m_playAreaUpperLeftX,0x0f*16-16+m_playAreaUpperLeftY,0,1,0,1.0,true,true);
			}
			unsigned char extCredit[]={0xac,0xbb,0xae,0xad,0xb2,0xbd,0};
			CTh5ExtFont::DrawExtString(extCredit,100,0x13*8-32+m_playAreaUpperLeftX,0x16*16-16+m_playAreaUpperLeftY,0,1,0);
			CTh5ExtFont::DrawExtChar(0xa0+m_nContinueLimit-m_nContinueUsed,0x21*8-32+m_playAreaUpperLeftX,0x16*16-16+m_playAreaUpperLeftY,0,1,0);
		}
		break;
	}
}

int CStage::StepLogo()
{
	m_logoFrame++;
	if (m_logoFrame>=128+17)
		return 1;
	if(!original::DemoActive()||original::Demo().number==5)CPMDPlayer::SetVolume(100.0f-m_logoFrame);else CPMDPlayer::SetVolume(100);
	return 0;
}

void CStage::DrawLogo()
{
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	m_pStageRes->m_pImgLogo->Draw((float)80-32+m_playAreaUpperLeftX,(float)112-16+m_playAreaUpperLeftY);
	if (m_logoFrame<17)
        PlayAreaFade((float)100-(16-m_logoFrame)*6);
	if (m_logoFrame>=128)
		PlayAreaFade((float)100-(m_logoFrame-128)*6);
	DrawStatistics();
}

void CStage::StepPause()
{
 m_lastMenuKeyState=m_curMenuKeyState;
 m_curMenuKeyState=CCommonFunctionInput::GetAllKeyState();
 // Pause-menu input is handled by the thprac ImGui adapter.
}

void CStage::DrawPause()
{
 if (m_bPaused) practice::DrawPause(*this);
}

bool CStage::Pause()
{
	if (m_bCanPause)
	{
		m_bPaused=true;
		practice::FocusPause();
		m_pauseCursorPos=0;
		return true;
	}
	return false;
}

void CStage::PlayAreaFade(float fadeScale)
{
	if (fadeScale<0) fadeScale=0;
	if (fadeScale>200) fadeScale=200;
	float overlayColor;
	if (fadeScale<100)
		overlayColor=0;
	else
		overlayColor=1;
	float overlayAlpha=fabs(fadeScale-100)/100;
	CCommonFunctionGraphic::DrawRectangle((float)(m_playAreaUpperLeftX),(float)(m_playAreaUpperLeftY),
										  (float)(m_playAreaUpperLeftX+384),(float)(m_playAreaUpperLeftY+368),
										  overlayColor,overlayColor,overlayColor,overlayAlpha);
}


practice::PhaseTimeout CStage::GetPhaseTimeout() const {
 practice::PhaseTimeout timer;
 const int kind=m_pBoss?2:m_pMidBoss?1:0;
 const void* owner=m_pBoss?static_cast<const void*>(m_pBoss):static_cast<const void*>(m_pMidBoss);
 const int phase=m_pBoss?m_pBoss->m_curPhase:m_pMidBoss?m_pMidBoss->m_curPhase:-1;
 if(!m_bLogo&&!m_bQuit&&!m_bContinueScr){
  if(m_pBoss)timer=m_pBoss->GetPhaseTimeout();
  else if(m_pMidBoss)timer=m_pMidBoss->GetPhaseTimeout();
 }
 if(!practice::Enabled(practice::LockTimeout)||timer.unit==practice::PhaseTimeout::None){m_timeoutOwner=nullptr;return timer.ForDisplay(practice::Options().phaseTimeoutAsTime);}
 // Lock the complete display projection, not the actor's attack frame/RNG.
 // Each new stage, actor, phase or F6 activation gets its own snapshot.
 const unsigned int revision=practice::TimeoutLockRevision();
 if(owner!=m_timeoutOwner||kind!=m_timeoutKind||phase!=m_timeoutPhase||revision!=m_timeoutRevision){
  m_timeoutOwner=owner;m_timeoutKind=kind;m_timeoutPhase=phase;m_timeoutRevision=revision;m_timeoutSnapshot=timer;
 }
 return m_timeoutSnapshot.ForDisplay(practice::Options().phaseTimeoutAsTime);
}

void CStage::DrawStatistics()
{
 if(practice::Options().eosdBar||practice::Options().ringBar)DrawBossBar();
 if(practice::Options().phaseTimeout&&!practice::Options().eosdBar&&!practice::Options().ringBar){
  const practice::PhaseTimeout timer=GetPhaseTimeout();
  if(timer.unit!=practice::PhaseTimeout::None){
   const int right=m_playAreaUpperLeftX+384-8;
   const int y=m_playAreaUpperLeftY+8;
   const int suffix=timer.unit==practice::PhaseTimeout::Turns?80:0;
   const bool warning=timer.value<=(suffix?5:10);
   const float tint=warning?.65f:1.f;
   m_pStageRes->DrawNumber(right-suffix-48,y,timer.value,3,false,true,1,tint,tint);
   if(timer.estimated)CPC98Font::DrawString("~",1,right-64,y,1,tint,tint);
   if(suffix){const unsigned char turn[]={0xbd,0xbe,0xbb,0xb7,0};CTh5ExtFont::DrawExtString(turn,4,right-64,y,1,tint,tint);}
  }
 }
	//draw score and highscore
	unsigned char extHiScore[]={0xd6,0xd7,0xd8,0xd9,0};
	CTh5ExtFont::DrawExtString(extHiScore,100,0x3c*8,3*16+40,1,1,0);
	unsigned char extScore[]={0xd7,0xd8,0xd9,0};
	CTh5ExtFont::DrawExtString(extScore,100,0x3d*8,5*16+40,1,1,0);
	m_pStageRes->DrawNumber(0x38*8,4*16+40,m_drawHighScore*10,8,true,false,1,1,1);
	m_pStageRes->DrawNumber(0x38*8,6*16+40,m_drawScore*10+m_nContinueUsed,8,true,false,1,1,1);

	if (CGame::GVar().m_playChara==0)
	{
		unsigned char extReigeki[]={0xda,0xdb,0};
		CTh5ExtFont::DrawExtString(extReigeki,100,0x39*8,0xb*16+40,1,1,0);
		unsigned char extReimu[]={0xdc,0xdd,0};
		CTh5ExtFont::DrawExtString(extReimu,100,0x39*8,0xd*16+40,1,1,0);
		unsigned char extReiryoku[]={0xde,0xdf,0};
		CTh5ExtFont::DrawExtString(extReiryoku,100,(practice::Options().numericPower?0x39:0x3e)*8,0x15*16+40,1,1,0);
	}
	else
	{
		unsigned char extBomb[]={0xe0,0xe1,0};
		CTh5ExtFont::DrawExtString(extBomb,100,0x39*8,0xb*16+40,1,1,0);
		unsigned char extPlayer[]={0xe2,0xe3,0};
		CTh5ExtFont::DrawExtString(extPlayer,100,0x39*8,0xd*16+40,1,1,0);
		unsigned char extPower[]={0xe4,0xe5,0};
		CTh5ExtFont::DrawExtString(extPower,100,(practice::Options().numericPower?0x39:0x3e)*8,0x15*16+40,1,1,0);
	}

	//draw bomb row
	if (m_nCurBomb>5)
	{
		CPC98Font::DrawString((char*)m_pStageRes->m_crossRow,100,0x3e*8,0xb*16+40,1,1,1);
		m_pStageRes->DrawNumber(0x44*8,0xb*16+40,m_nCurBomb,2,false,true,1,1,1);
	}
	else
	{
		for (int i=0;i<m_nCurBomb;i++)
			CTh5ExtFont::DrawExtChar(0xd3,0x3e*8+i*16,0xb*16+40,1,1,1);
	}
	//draw life row
	if (m_nCurLife>6)
	{
		CPC98Font::DrawString((char*)m_pStageRes->m_crossRow,100,0x3e*8,0xd*16+40,1,1,1);
		m_pStageRes->DrawNumber(0x44*8,0xd*16+40,m_nCurLife-1,2,false,true,1,1,1);
	}
	else
	{
		for (int i=0;i<m_nCurLife-1;i++)
			CTh5ExtFont::DrawExtChar(0xd4,0x3e*8+i*16,0xd*16+40,1,1,1);
	}

	CTh5ExtFont::DrawExtChar(0xe6,0x3a*8,0x10*16+40,1,1,0);
	unsigned char extRuikei[]={0xed,0xee,0};
	CTh5ExtFont::DrawExtString(extRuikei,100,0x39*8,0xf*16+40,0,1,1);
	m_pStageRes->DrawNumber(0x3e*8,0x10*16+40,m_curNPointItem,5,false,true,1,1,1);
	m_pStageRes->DrawNumber(0x3e*8,0xf*16+40,m_totalNPointItem,5,false,true,0,1,1);
	CTh5ExtFont::DrawExtChar(0xe7,(practice::Options().numericDream?0x3a:0x3f)*8,0x13*16+40+(practice::Options().numericDream?8:0),1,1,0);			//Ten
	CTh5ExtFont::DrawExtChar(0xe8,0x3a*8,0x12*16+40,1,1,0);			//Dan
	m_pStageRes->DrawNumber(0x3e*8,0x12*16+40,m_curNGraze,5,false,true,1,1,1);

	//draw dream and power gauge
	float powerGaugeColor[][3]={{1,0,0},{1,0,0},{1,0,0},{1,0,1},{1,0,1},{0,0,1},{0,0,1},{0,1,1},{1,1,0},{1,1,1}};
	float dreamGaugeColor[][3]={{1,0,0},{1,0,0},{1,0,1},{1,0,1},{0,0,1},{0,0,1},{0,1,1},{1,1,0},{1,1,1}};
	if (practice::Options().numericDream)
		m_pStageRes->DrawNumber(0x3e*8,0x13*16+48,m_curDream,5,false,true,1,1,1);
	else m_pStageRes->DrawGauge(m_curDream,128,0x38*8,0x14*16+40,dreamGaugeColor[m_curDream/16][0],
			  dreamGaugeColor[m_curDream/16][1],dreamGaugeColor[m_curDream/16][2]);
	if (practice::Options().numericPower)
		m_pStageRes->DrawNumber(0x3e*8,0x15*16+40,m_pChara->GetPower(),5,false,true,1,1,1);
	else m_pStageRes->DrawGauge(m_pChara->GetPower(),128,0x38*8,0x16*16+40,powerGaugeColor[m_pChara->m_powerLevel][0],
			  powerGaugeColor[m_pChara->m_powerLevel][1],powerGaugeColor[m_pChara->m_powerLevel][2]);

	//draw difficulty
	unsigned char difficultyExtStr[][8]={{0xae,0xaa,0xbc,0xc2,2,2,2,0},				//"EASY"
										 {0xb7,0xb8,0xbb,0xb6,0xaa,0xb5,2,0},		//"NORMAL"
										 {0xb1,0xaa,0xbb,0xad,2,2,2,0},				//"HARD"
										 {0xb5,0xbe,0xb7,0xaa,0xbd,0xb2,0xac,0},	//"LUNATIC"
										 {0xae,0xc1,0xbd,0xbb,0xaa,2,2,0}};			//"EXTRA"
	float color[][3]={{0,1,0},{0,1,1},{1,0,1},{1,0,0},{1,0,0}};
	int dif=CGame::GVar().m_playDifficulty;
	if(!practice::InGameInfoOpen())CTh5ExtFont::DrawExtString(difficultyExtStr[dif],100,0x39*8,0x17*16+40,color[dif][0],color[dif][1],color[dif][2]);

	if (m_pMidBoss)
		m_pMidBoss->DrawStatistics();
	if (m_pBoss)
		m_pBoss->DrawStatistics();

	//draw fps
	char str[20];
	if (CGame::s_pCurGame->m_fps>100)
        sprintf(str,"FPS:%.1lf",CGame::s_pCurGame->m_fps);
	else
		sprintf(str,"FPS: %.1lf",CGame::s_pCurGame->m_fps);
	CPC98Font::DrawString(str,9,564,460,1,1,1);
}

int CStage::AdjustDamageFromChara(int origDmg, bool isBoss)
{
	//special adjust taken from original PC98 version
	if (isBoss)
	{
		if (CGame::GVar().m_playChara==0||CGame::GVar().m_playChara==3)
		{
			//reimu and yuka's damage to stage extra boss is increased to 5/4
			if (CGame::GVar().m_playStage==6)
				origDmg=(origDmg*5)/4;
			else
			{
				//reimu's damage to normal stage boss is increased to 8/7
				if (CGame::GVar().m_playChara==0)
					origDmg=(origDmg*8)/7;
			}
		}
		else
		{
			//marisa and mima's damage to boss in all stages is reduced to 4/5
			origDmg=(origDmg*4)/5;
		}
	}
	else
	{
		//reimu's damage to non-boss in stage extra is reduced to 4/5
		if (CGame::GVar().m_playStage==6&&CGame::GVar().m_playChara==0)
			origDmg=(origDmg*4)/5;
		//yuka's damage to non-boss in all stages is reduced to 4/5
		if (CGame::GVar().m_playChara==3)
			origDmg=(origDmg*4)/5;
	}
	return origDmg;
}

void CStage::SetQuit(int quitCode)
{
	m_bQuit=true;
	m_nQuitFrameLeft=50;
	m_quitCode=quitCode;
	m_bCanPause=false;
}

void CStage::SetDream(int newDream)
{
 if(practice::Enabled(practice::LockDream))return;
	if (newDream>128) newDream=128;
	if (newDream<1) newDream=1;
	if (newDream==128&&m_curDream<=127)
	{
		m_pEnemyBullet->SetClearBulletTimeLeft(20);
		m_pFloatingText->SetTextType(FLOATING_TEXT_DREAMMAX);
	}
	m_curDream=newDream;
}

int CStage::GetDamageFromChara(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss)
{
 if(m_practiceSeekDamage){
  if(m_practiceSeekDamage==2)return 0;
  // Seeking Alice phases must not kill puppets: their delayed death takes
  // 300 HP from Alice each, even after the destination HP has been restored.
  if(!isBoss&&dynamic_cast<CBossStage3*>(m_pBoss))return 0;
  const auto* selected=practice::SelectedSection(CGame::GVar().m_practiceConfig.stage,CGame::GVar().m_practiceConfig.warp,CGame::GVar().m_practiceConfig.section);
  if(selected&&selected->variant&&dynamic_cast<CBossStage4*>(m_pBoss)){
   const bool first=ctrX==m_pBoss->m_curX&&ctrY==m_pBoss->m_curY;
   if(first==(selected->variant==1))return 0;
  }
  return 100000;
 }
	int totalDmg=m_pCharaBullet->GetDamage(ctrX,ctrY,halfWidth,halfHeight,isBoss);
	if (m_bInCharaBomb)
		totalDmg+=m_pChara->GetBombDamage(ctrX,ctrY,halfWidth,halfHeight,isBoss);
	totalDmg=AdjustDamageFromChara(totalDmg,isBoss);
	if (practice::Enabled(practice::EnemyInvincible)) totalDmg=0;
	AddScore(totalDmg);
	return totalDmg;//*10;
}

}
















