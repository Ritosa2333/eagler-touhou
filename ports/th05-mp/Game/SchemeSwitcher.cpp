#include ".\schemeswitcher.h"
#include "OP/Opening.h"
#include "TitleScreen/TitleScreen.h"
#include "SelectCharaScreen/SelectCharaScreen.h"
#include "Stage/Stage.h"
#include "Game.h"
#include "MusicRoom/MusicRoom.h"
#include "Original/OriginalScenes.h"
#include "ed/Ending.h"
#include "SelectReplayScreen/SelectReplayScreen.h"
#include "HighScoreScreen/HighScoreScreen.h"


#define SCHEME_DUMMY 0
#define SCHEME_OPENING 1
#define SCHEME_TITLESCREEN 2
#define SCHEME_SELECTCHARASCREEN 3
#define SCHEME_STAGE 4
#define SCHEME_ENDING 5
#define SCHEME_SELECTREPLAYSCREEN 6
#define SCHEME_HIGHSCORESCREEN 7

namespace th5w{

CSchemeSwitcher::CSchemeSwitcher(void)
{
}

CSchemeSwitcher::~CSchemeSwitcher(void)
{
}

bool CSchemeSwitcher::SwitchScheme(CScheme** ppOutNextScheme,int *pOutNextSchemeID, int curSchemeID, int curSchemeExitValue)
{
	if (curSchemeExitValue==1&&curSchemeID==SCHEME_OPENING)
	{
		CTitleScreen *pScheme=new CTitleScreen;
		pScheme->Initialize(true);
		*ppOutNextScheme=(CScheme*)pScheme;
		*pOutNextSchemeID=SCHEME_TITLESCREEN;
		return true;
	}

 if(curSchemeID==8) {
  CTitleScreen* next=new CTitleScreen;next->Initialize(false,5,true);*ppOutNextScheme=next;*pOutNextSchemeID=SCHEME_TITLESCREEN;return true;
 }
 if(curSchemeID==SCHEME_TITLESCREEN && curSchemeExitValue==TITLESCREEN_END_MUSIC_ROOM) {
  CMusicRoom* room=new CMusicRoom;if(!room->Initialize()){
   delete room;MessageBoxW(CGame::s_pCurGame->m_gameWindow.m_hWnd,L"Music Room resources are missing or invalid.",L"Music Room",MB_OK|MB_ICONERROR);
   CTitleScreen* next=new CTitleScreen;next->Initialize(false,5,true);*ppOutNextScheme=next;*pOutNextSchemeID=SCHEME_TITLESCREEN;return true;
  }
  *ppOutNextScheme=room;*pOutNextSchemeID=8;return true;
 }
 if(curSchemeID==SCHEME_TITLESCREEN&&(curSchemeExitValue==TITLESCREEN_END_DEMO||curSchemeExitValue==TITLESCREEN_END_EXTRA_DEMO)){
  if(original::StartDemo(curSchemeExitValue==TITLESCREEN_END_EXTRA_DEMO)){
   CStage* next=new CStage;next->Initialize();*ppOutNextScheme=next;*pOutNextSchemeID=SCHEME_STAGE;
  }else{CTitleScreen* next=new CTitleScreen;next->Initialize(false,0,true);*ppOutNextScheme=next;*pOutNextSchemeID=SCHEME_TITLESCREEN;}
  return true;
 }
 if(curSchemeID==9||curSchemeID==10||curSchemeID==11){
  if(curSchemeID==10){COriginalEnd* next=new COriginalEnd(COriginalEnd::VerdictOnly);next->Initialize();*ppOutNextScheme=next;*pOutNextSchemeID=11;}
  else if(curSchemeID==11&&!original::Stats().complete){CSelectReplayScreen* next=new CSelectReplayScreen;next->Initialize(false,true);*ppOutNextScheme=next;*pOutNextSchemeID=SCHEME_SELECTREPLAYSCREEN;}
  else{CHighScoreScreen* next=new CHighScoreScreen;next->Initialize(false,true);*ppOutNextScheme=next;*pOutNextSchemeID=SCHEME_HIGHSCORESCREEN;}
  return true;
 }
	if (curSchemeID==SCHEME_TITLESCREEN)
	{
		if (curSchemeExitValue==TITLESCREEN_END_QUIT_GAME)
		{
			*ppOutNextScheme=NULL;
			return true;
		}
		if (curSchemeExitValue==TITLESCREEN_END_START_GAME||curSchemeExitValue==TITLESCREEN_END_START_EXTRA||curSchemeExitValue==TITLESCREEN_END_START_PRACTICE)
		{
			CSelectCharaScreen *pScheme=new CSelectCharaScreen;
			pScheme->Initialize();
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_SELECTCHARASCREEN;
			return true;
		}
		if (curSchemeExitValue==TITLESCREEN_END_CHOOSE_REPLAY)
		{
			CSelectReplayScreen *pScheme=new CSelectReplayScreen;
			pScheme->Initialize(true,false);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_SELECTREPLAYSCREEN;
			return true;
		}
		if (curSchemeExitValue==TITLESCREEN_END_VIEW_HIGHSCORE)
		{
			CHighScoreScreen *pScheme=new CHighScoreScreen;
			pScheme->Initialize(true,false);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_HIGHSCORESCREEN;
			return true;
		}
	}

	if (curSchemeID==SCHEME_SELECTCHARASCREEN)
	{
		if (curSchemeExitValue==SELECTCHARASCREEN_END_SELECTED_CHARA)
		{
			CStage *pScheme=new CStage;
			pScheme->Initialize();
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_STAGE;
			return true;
		}
		if (curSchemeExitValue==SELECTCHARASCREEN_END_BACK)
		{
			CTitleScreen *pScheme=new CTitleScreen;
			pScheme->Initialize(false,CGame::GVar().m_bPracticeMode?2:(CGame::GVar().m_playDifficulty==4?1:0),false);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_TITLESCREEN;
			return true;
		}
	}
	if (curSchemeID==SCHEME_SELECTREPLAYSCREEN)
	{
		if (curSchemeExitValue==SELECTREPLAYSCREEN_END_SELECTED_REPLAY)
		{
			CStage *pScheme=new CStage;
			pScheme->Initialize();
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_STAGE;
			return true;
		}
		if (curSchemeExitValue==SELECTREPLAYSCREEN_END_BACK)
		{
			CTitleScreen *pScheme=new CTitleScreen;
			pScheme->Initialize(false,3,false);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_TITLESCREEN;
			return true;
		}
		if (curSchemeExitValue==SELECTREPLAYSCREEN_END_SAVE_REPLAY_END||curSchemeExitValue==SELECTREPLAYSCREEN_END_SAVE_REPLAY_SKIPPED)
		{
			CTitleScreen *pScheme=new CTitleScreen;
			pScheme->Initialize(false,0,true);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_TITLESCREEN;
			return true;
		}
	}

	if (curSchemeID==SCHEME_STAGE)
	{
  if(original::DemoActive()){
   original::StopDemo();CTitleScreen* next=new CTitleScreen;next->Initialize(true,0,true);
   *ppOutNextScheme=next;*pOutNextSchemeID=SCHEME_TITLESCREEN;return true;
  }
		if (curSchemeExitValue==STAGE_END_RESTART)
		{
			if (!CGame::GVar().m_bReplayMode && !CGame::GVar().m_bPracticeMode)
				CGame::GVar().m_playStage=CGame::GVar().m_playDifficulty==4 ? 6 : 0;
			CGame::GVar().OnBeginGame();
			CStage *pScheme=new CStage;
			pScheme->Initialize();
			*ppOutNextScheme=pScheme;
			*pOutNextSchemeID=SCHEME_STAGE;
			return true;
		}
		if (curSchemeExitValue==STAGE_END_SAVE_EXIT)
		{
			CSelectReplayScreen *pScheme=new CSelectReplayScreen;
			pScheme->Initialize(false,true);
			*ppOutNextScheme=pScheme;
			*pOutNextSchemeID=SCHEME_SELECTREPLAYSCREEN;
			return true;
		}
		if (CGame::GVar().m_bPracticeMode && !CGame::GVar().m_bReplayMode &&
			(curSchemeExitValue==STAGE_END_CLEAR || curSchemeExitValue==STAGE_END_ALL_MISSED))
		{
			CSelectReplayScreen *pScheme=new CSelectReplayScreen;
			pScheme->Initialize(false,true);
			*ppOutNextScheme=pScheme;
			*pOutNextSchemeID=SCHEME_SELECTREPLAYSCREEN;
			return true;
		}
		if (curSchemeExitValue==STAGE_END_CLEAR)
		{
			if (CGame::GVar().m_playStage<5)
			{
				CGame::GVar().m_playStage++;
				CStage *pScheme=new CStage;
				pScheme->Initialize();
				*ppOutNextScheme=(CScheme*)pScheme;
				*pOutNextSchemeID=SCHEME_STAGE;
				return true;
			}
			else
			{
				CEnding *pScheme=new CEnding;
				if (CGame::GVar().m_playStage==5)
					pScheme->Initialize(CGame::GVar().m_nContinueUsed==0?1:0);
				else
					pScheme->Initialize(2);
				CGame::GVar().OnClear();
				CGame::GVar().SaveConfig();
				*ppOutNextScheme=(CScheme*)pScheme;
				*pOutNextSchemeID=SCHEME_ENDING;
				return true;
			}
		}
		if (curSchemeExitValue==STAGE_END_ALL_MISSED)
		{
			CHighScoreScreen *pScheme=new CHighScoreScreen;
			pScheme->Initialize(false,false);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_HIGHSCORESCREEN;
			return true;
		}
		if (curSchemeExitValue==STAGE_END_PLAYER_QUIT)
		{
			CTitleScreen *pScheme=new CTitleScreen;
			pScheme->Initialize(true,0,true);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_TITLESCREEN;
			return true;
		}
		if (curSchemeExitValue==STAGE_END_REPLAY_END)
		{
			CSelectReplayScreen *pScheme=new CSelectReplayScreen;
			pScheme->Initialize(true,true);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_SELECTREPLAYSCREEN;
			return true;
		}
	}

	if (curSchemeID==SCHEME_ENDING)
	{
  const bool extra=CGame::GVar().m_playStage==6;
  COriginalEnd* next=new COriginalEnd(extra?COriginalEnd::AllCast:COriginalEnd::Staff);next->Initialize();
  *ppOutNextScheme=next;*pOutNextSchemeID=extra?10:9;return true;
	}
	if (curSchemeID==SCHEME_HIGHSCORESCREEN)
	{
		if (curSchemeExitValue==HIGHSCORESCREEN_END_ENTER_END||curSchemeExitValue==HIGHSCORESCREEN_END_ENTER_SKIPPED)
		{
   if(!CGame::GVar().m_bPracticeMode&&!original::Stats().complete){
    COriginalEnd* next=new COriginalEnd(COriginalEnd::VerdictOnly);next->Initialize();*ppOutNextScheme=next;*pOutNextSchemeID=11;return true;
   }
			CSelectReplayScreen *pScheme=new CSelectReplayScreen;
			if (curSchemeExitValue==HIGHSCORESCREEN_END_ENTER_END)
				pScheme->Initialize(false,false);
			else
				pScheme->Initialize(false,true);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_SELECTREPLAYSCREEN;
			return true;
		}
		if (curSchemeExitValue==HIGHSCORESCREEN_END_VIEW_END)
		{
			CTitleScreen *pScheme=new CTitleScreen;
			pScheme->Initialize(false,4,true);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_TITLESCREEN;
			return true;
		}
	}

	return false;
}

bool CSchemeSwitcher::GetFirstScheme(CScheme** ppOutNextScheme,int *pOutNextSchemeID)
{
	COpening *pOP=new COpening;
	pOP->Initialize();
	*ppOutNextScheme=pOP;
	*pOutNextSchemeID=SCHEME_OPENING;

	return true;
}

}
