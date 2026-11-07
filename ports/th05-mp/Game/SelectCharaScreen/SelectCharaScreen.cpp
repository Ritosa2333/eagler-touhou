#include ".\selectcharascreen.h"
#include "../Game.h"
#include "../Practice/Practice.h"
#include "../../gameLib/Graphic/Th5ExtFont.h"
#include <cstdio>
#include "../../gameLib/CommonFunction/CommonFunctionInput.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"

namespace th5w{

CSelectCharaScreen::CSelectCharaScreen(void)
{
}

CSelectCharaScreen::~CSelectCharaScreen(void)
{
	if (m_pBGImage!=NULL)
		m_pBGImage->Destroy();
	m_pBGImage=NULL;
}

void CSelectCharaScreen::Initialize()
{
	m_bStageSelect=false;
 m_bThpracSetup=false;m_setupAction=0;
	m_stageSelectClosing=false;
	m_stageCursor=0;
	m_stageSelectAge=0;
	m_bQuit=false;
	m_quitCode=0;
	m_lastKeyState=0;

	m_cursorPos=0;

	int difficulty=CGame::GVar().m_playDifficulty;
	for (int i=0;i<4;i++)
		m_bCharaClearFlag[i]=CGame::GVar().m_bClear[i][difficulty];
	if (difficulty!=4)
		for (int i=0;i<4;i++)
			m_bCharaSelectable[i]=true;
	else
		for (int i=0;i<4;i++)
			m_bCharaSelectable[i]=CGame::GVar().m_bExtraPlayable[i];
	for (int i=0;i<4;i++)
		if (m_bCharaSelectable[i])
			m_charaDisplayImg[i]=i;
		else
			m_charaDisplayImg[i]=4;

	CCommonFunctionGraphic::LoadPIFromDat(&m_pBGImage,m_palette,&CGame::s_pCurGame->m_th5Dat1,"SLB1.PI");

	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,"SL00.CDG",m_palette);
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,"SL01.CDG",m_palette);
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,"SL02.CDG",m_palette);
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,"SL03.CDG",m_palette);
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,"SL04.CDG",m_palette);
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,"SLCL.CDG",m_palette);

	m_curScrFade=0;
}

int CSelectCharaScreen::Step()
{
	if (m_bQuit)
	{
		if (m_curScrFade>0)
		{
			m_curScrFade-=6;
			if (m_curScrFade<0)
				m_curScrFade=0;
			return 0;
		}
		else
			return m_quitCode;
	}

	if (m_curScrFade<100)
	{
		m_curScrFade+=6;
		if (m_curScrFade>100)
			m_curScrFade=100;
		return 0;
	}

 if(m_bThpracSetup){
  if(m_stageSelectClosing){
   if(--m_stageSelectAge<=0){m_bThpracSetup=m_bStageSelect=m_stageSelectClosing=false;}
   m_lastKeyState=CCommonFunctionInput::GetAllKeyState();return 0;
  }
  ++m_stageSelectAge;
  if(m_setupAction==2){m_stageSelectClosing=true;m_stageSelectAge=8;m_setupAction=0;}
  else if(m_setupAction==1&&practice::StartSetup()){
   m_bThpracSetup=m_bStageSelect=false;
   m_bQuit=true;m_quitCode=SELECTCHARASCREEN_END_SELECTED_CHARA;
   CGame::s_pCurGame->m_soundEffect.PlaySound(11);
  }
  m_lastKeyState=CCommonFunctionInput::GetAllKeyState();return 0;
 }
	if (m_bStageSelect && m_stageSelectClosing) {
        if (--m_stageSelectAge<=0) { m_bStageSelect=false; m_stageSelectClosing=false; }
        m_lastKeyState=CCommonFunctionInput::GetAllKeyState();
        return 0;
    }
	if (m_bStageSelect) ++m_stageSelectAge;
	ParseKeyEvent();

	return 0;
}

void CSelectCharaScreen::Draw()
{
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	m_pBGImage->Draw(0,40);

	int charaX[]={16,272,160,400};
	int charaY[]={48+40,48+40,224+40,224+40};

	for (int i=0;i<4;i++)
	{
		if (m_cursorPos==i)
		{
			CCommonFunctionGraphic::DrawRectangle((float)charaX[i],(float)charaY[i],
												  (float)charaX[i]+m_spriteArray.GetImagePtr(m_charaDisplayImg[i])->m_width-1,
												  (float)charaY[i]+m_spriteArray.GetImagePtr(m_charaDisplayImg[i])->m_height-1,
												  0,0,0);
			m_spriteArray.GetImagePtr(m_charaDisplayImg[i])->Draw((float)charaX[i]-8,(float)charaY[i]-8);
			if (m_bCharaClearFlag[i])
				m_spriteArray.GetImagePtr(5)->Draw((float)charaX[i]-8+160,(float)charaY[i]-8+144);
		}
		else
		{
			m_spriteArray.GetImagePtr(m_charaDisplayImg[i])->DrawColorScaled((float)charaX[i],(float)charaY[i],0.5,0.5,0.5);
			if (m_bCharaClearFlag[i])
				m_spriteArray.GetImagePtr(5)->DrawColorScaled((float)charaX[i]+160,(float)charaY[i]+144,0.5,0.5,0.5);
		}
	}

	if(m_bThpracSetup)m_setupAction=practice::DrawSetup(m_stageSelectAge);
 else if (m_bStageSelect) DrawStageSelect();
	CCommonFunctionGraphic::ScreenFade((float)m_curScrFade);
}

// TH06 MainMenu::ChoosePracticeLevel: six STAGE rows, 24px spacing,
// return to character selection on cancel; confirm starts only the chosen stage.
void CSelectCharaScreen::DrawStageSelect()
{
 // Ten frames (1/6 second): expand from the center while fading in.
 const float t=m_stageSelectAge>=10 ? 1.f : float(m_stageSelectAge)/10.f;
 const float expand=1.f-(1.f-t)*(1.f-t)*(1.f-t);
 const int left=440-int(160*expand), right=440+int(160*expand);
 const int top=272-int(92*expand), bottom=272+int(92*expand);
 CCommonFunctionGraphic::DrawRectangle(0,40,639,439,0,0,0,.5f*t);
 if (right<=left || bottom<=top) return;
 CCommonFunctionGraphic::DrawRectangle(float(left),float(top),float(right),float(bottom),0,0,0,t);
 CCommonFunctionGraphic::DrawRectangle(float(left),float(top),float(right),float(top),.5f,.75f,.75f,t);
 CCommonFunctionGraphic::DrawRectangle(float(left),float(bottom),float(right),float(bottom),.5f,.75f,.75f,t);
 CCommonFunctionGraphic::DrawRectangle(float(left),float(top),float(left),float(bottom),.5f,.75f,.75f,t);
 CCommonFunctionGraphic::DrawRectangle(float(right),float(top),float(right),float(bottom),.5f,.75f,.75f,t);
 GLint viewport[4]; glGetIntegerv(GL_VIEWPORT,viewport);
 glPushAttrib(GL_SCISSOR_BIT); glEnable(GL_SCISSOR_TEST);
 glScissor(viewport[0]+left*viewport[2]/640,viewport[1]+(480-bottom)*viewport[3]/480,
  (right-left)*viewport[2]/640,(bottom-top)*viewport[3]/480);
 for (int i=0;i<6;i++) {
  const bool unlocked=CGame::GVar().PracticeStageUnlocked(i);
  char text[48]; if(unlocked) sprintf_s(text,"STAGE %d  %09d",i+1,0);
  else sprintf_s(text,"STAGE %d  LOCKED",i+1);
  const bool selected=i==m_stageCursor;
  for (int j=0;text[j];j++) {
   // The same ZUN.COM gaiji as CHighScoreScreen's name-entry grid.
   unsigned char glyph=0;
   if (text[j]>='A' && text[j]<='Z') glyph=0xaa+text[j]-'A';
   else if (text[j]>='0' && text[j]<='9') glyph=0xa0+text[j]-'0';
   if (glyph) CTh5ExtFont::DrawExtChar(glyph,296+j*16,200+i*24,
    unlocked?(selected?.75f:.5f):.25f,unlocked?(selected?.94f:.75f):.25f,unlocked?(selected?.94f:.75f):.25f,t);
  }
 }
 glPopAttrib();
}
void CSelectCharaScreen::ParseStageKey(unsigned short state)
{
 if (m_stageSelectClosing) { m_lastKeyState=state; return; }
 if (m_stageSelectAge<30) { m_lastKeyState=state; return; }
 if (CCommonFunctionInput::ESCPressed(state,m_lastKeyState)||CCommonFunctionInput::XPressed(state,m_lastKeyState)) {
  m_stageSelectClosing=true; m_stageSelectAge=10;
  CGame::s_pCurGame->m_soundEffect.PlaySound(1);
 } else if (CCommonFunctionInput::ZPressed(state,m_lastKeyState)) {
  if (!CGame::GVar().PracticeStageUnlocked(m_stageCursor)) { m_lastKeyState=state; return; }
  CGame::GVar().m_playStage=m_stageCursor;
  CGame::GVar().OnBeginGame();
  m_bQuit=true; m_quitCode=SELECTCHARASCREEN_END_SELECTED_CHARA;
  CGame::s_pCurGame->m_soundEffect.PlaySound(11);
 } else {
  int delta=CCommonFunctionInput::DownPressed(state,m_lastKeyState)?1:CCommonFunctionInput::UpPressed(state,m_lastKeyState)?-1:0;
  if (delta) { m_stageCursor=(m_stageCursor+delta+6)%6; CGame::s_pCurGame->m_soundEffect.PlaySound(1); }
 }
 m_lastKeyState=state;
}

void CSelectCharaScreen::ParseKeyEvent()
{
	unsigned short curState=CCommonFunctionInput::GetAllKeyState();
	if (m_bStageSelect) { ParseStageKey(curState); return; }
	if (CCommonFunctionInput::LeftPressed(curState,m_lastKeyState)||CCommonFunctionInput::RightPressed(curState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(1);
		m_cursorPos=m_cursorPos/2*2+(1-m_cursorPos%2);
		m_lastKeyState=curState;
		return;
	}
	if (CCommonFunctionInput::UpPressed(curState,m_lastKeyState)||CCommonFunctionInput::DownPressed(curState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(1);
		m_cursorPos=m_cursorPos%2+(2-m_cursorPos+m_cursorPos%2);
		m_lastKeyState=curState;
		return;
	}
	if (CCommonFunctionInput::ZPressed(curState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(11);
		if (m_bCharaSelectable[m_cursorPos]==true)
		{
			CGame::GVar().m_playChara=m_cursorPos;
			if (CGame::GVar().m_bPracticeMode)
			{
				m_bStageSelect=true; m_stageSelectClosing=false; m_stageSelectAge=0;
                m_stageCursor=0;
                for(int i=0;i<6;i++) if(CGame::GVar().PracticeStageUnlocked(i)) { m_stageCursor=i; break; }
                m_bThpracSetup=true;m_setupAction=0;practice::OpenSetup(m_stageCursor);
                m_lastKeyState=curState; return;
			}
			CGame::GVar().OnBeginGame();
			m_bQuit=true;
			m_quitCode=SELECTCHARASCREEN_END_SELECTED_CHARA;
		}
		m_lastKeyState=curState;
		return;
	}
	if (CCommonFunctionInput::ESCPressed(curState,m_lastKeyState))
	{
		m_bQuit=true;
		m_quitCode=SELECTCHARASCREEN_END_BACK;
		m_lastKeyState=curState;
		return;
	}

	m_lastKeyState=curState;
	return;
}

}












