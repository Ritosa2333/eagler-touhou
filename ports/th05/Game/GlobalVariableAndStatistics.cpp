#include ".\globalvariableandstatistics.h"
#include "Original/OriginalFeatures.h"
#include "Practice/Practice.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <windows.h>
#include "../gameLib/CommonFunction/CommonFunctionSystem.h"

namespace th5w{

CGlobalVariableAndStatistics::CGlobalVariableAndStatistics(void)
{
	getcwd(m_workingPath,999);
	if (m_workingPath[strlen(m_workingPath)-1]!='\\')
		strcat(m_workingPath,"\\");
	strcpy(m_replaySubDir,"replay");
	memset(m_modMD5,0,sizeof(m_modMD5));
	m_gameVersion=1;
 m_turboMode=m_runTurboMode=true;
	m_bReplayMode=false;
	m_bCanSaveReplay=false;
	memset(m_practiceUnlocked,0,sizeof(m_practiceUnlocked));
}

CGlobalVariableAndStatistics::~CGlobalVariableAndStatistics(void)
{
}

void CGlobalVariableAndStatistics::LoadProgramConfig()
{
	m_bUseMod=false;
	m_modFileName[0]=0;
	m_textCodePage=932;
	m_bUseSystemFont=false;
	m_fontCharset=134;
	FILE *fp=fopen("config.ini","r");
	if (fp==NULL)
		return;
	char curLine[1000];
	while(fgets(curLine,1000,fp)!=NULL)
	{
		if (strlen(curLine)>0&&curLine[strlen(curLine)-1]=='\n')		//replace trailing '\n' with 0
			curLine[strlen(curLine)-1]=0;

		char *ptr=curLine;

		while(ptr[0]==' ') ptr++;										//remove leading spaces

		if (strlen(ptr)<2)
			continue;

		if (curLine[0]=='/'&&curLine[1]=='/')							//commented line
			continue;

		if (strchr(ptr,'=')==NULL)										//'=' not found
			continue;
		
		char *value=strchr(ptr,'=');
		value[0]=0;														//cut the string to 2 parts

		value++;
		while(value[0]==' ') value++;									//remove leading spaces

		char variable[100];
		sscanf(ptr,"%s",variable);										//sscanf to remove trailing spaces

		if (strcmp(variable,"MOD_FILE")==0)
		{
			m_bUseMod=true;
			sscanf(value,"%s",m_modFileName);
			CCommonFunctionSystem::GetFileMD5(m_modMD5,m_modFileName);
			continue;
		}
		if (strcmp(variable,"CONSOLE_WINDOW")==0)
		{
			int console;
			sscanf(value,"%d",&console);
			if (console!=0)
				CCommonFunctionSystem::CreateConsoleWindow();
			continue;
		}
	}
	fclose(fp);
}

void CGlobalVariableAndStatistics::LoadConfig()
{
	m_nInitLife=3;
	m_nInitBomb=3;
	m_initDifficulty=1;
 m_turboMode=true;

	memset(m_defaultHighScoreName,0,sizeof(m_defaultHighScoreName));
	memset(m_defaultReplayName,0,sizeof(m_defaultReplayName));

	memset(m_bClear,0,sizeof(m_bClear));

	FILE *fp=fopen("game.cfg","rb");
	if (fp==NULL)
		return;
	fseek(fp,0,SEEK_END);
	if (ftell(fp)<52)
	{
		fclose(fp);
		return;
	}
	fseek(fp,0,SEEK_SET);
	fread(&m_nInitLife,sizeof(int),1,fp);
	fread(&m_nInitBomb,sizeof(int),1,fp);
	fread(&m_initDifficulty,sizeof(int),1,fp);
	fread(&m_bClear,1,20,fp);
	fread(m_defaultHighScoreName,1,10,fp);
	fread(m_defaultReplayName,1,10,fp);
 // Keep the 52-byte prefix and the 40-byte practice extension unchanged.
 unsigned char speed[9];fseek(fp,92,SEEK_SET);
 if(fread(speed,1,9,fp)==9&&!memcmp(speed,"TH5SPD1",8)&&speed[8]<=1)m_turboMode=speed[8]!=0;
	fclose(fp);
}

void CGlobalVariableAndStatistics::SaveConfig()
{
	FILE *fp=fopen("game.cfg","wb");
	if (fp==NULL)
		return;
	fwrite(&m_nInitLife,sizeof(int),1,fp);
	fwrite(&m_nInitBomb,sizeof(int),1,fp);
	fwrite(&m_initDifficulty,sizeof(int),1,fp);
	fwrite(&m_bClear,1,20,fp);
	fwrite(m_defaultHighScoreName,1,10,fp);
	fwrite(m_defaultReplayName,1,10,fp);
	fwrite("TH5PRC1",1,8,fp);
	fwrite(m_modMD5,1,16,fp);
	fwrite(m_practiceUnlocked,1,16,fp);
 fwrite("TH5SPD1",1,8,fp);const unsigned char turbo=m_turboMode?1:0;fwrite(&turbo,1,1,fp);
	fclose(fp);
}

bool CGlobalVariableAndStatistics::PracticeStageUnlocked(int stage) const
{
 return stage>=0 && stage<6 && m_playChara>=0 && m_playChara<4 &&
  m_playDifficulty>=0 && m_playDifficulty<4 &&
  (m_practiceUnlocked[m_playChara][m_playDifficulty] & (1<<stage))!=0;
}
void CGlobalVariableAndStatistics::LoadPracticeProgress()
{
 memset(m_practiceUnlocked,0,sizeof(m_practiceUnlocked));
 // Existing full-game clears are also evidence of completing all six stages.
 for(int c=0;c<4;c++) for(int d=0;d<4;d++)
  if(m_bClear[c][d]) m_practiceUnlocked[c][d]=63;
 // The extension follows the unchanged 52-byte native configuration prefix.
 FILE* f=fopen("game.cfg","rb");
 unsigned char embedded[40];
 if(f) {
  fseek(f,52,SEEK_SET);const size_t n=fread(embedded,1,40,f);fclose(f);
  if(n==40 && !memcmp(embedded,"TH5PRC1",8) && !memcmp(embedded+8,m_modMD5,16)) {
   bool valid=true;for(int i=0;i<16;i++)if(embedded[24+i]>63)valid=false;
   if(valid)for(int c=0;c<4;c++)for(int d=0;d<4;d++)m_practiceUnlocked[c][d]|=embedded[24+c*4+d];
  }
 }
 // Read the earlier sidecar only as an import; all new writes go to game.cfg.
 f=fopen("practice-progress.dat","rb"); if(!f) return;
 unsigned char data[40]; const size_t count=fread(data,1,sizeof(data),f);
 const int extra=fgetc(f); fclose(f);
 if(count!=sizeof(data)||extra!=EOF||memcmp(data,"TH5PRC1",8)||memcmp(data+8,m_modMD5,16)) return;
 for(int i=0;i<16;i++) if(data[24+i]>63) return;
 for(int c=0;c<4;c++) for(int d=0;d<4;d++) m_practiceUnlocked[c][d]|=data[24+c*4+d];
}
void CGlobalVariableAndStatistics::SavePracticeProgress()
{
 SaveConfig();
}
void CGlobalVariableAndStatistics::OnStageEnter()
{
 if(original::DemoActive() || m_bReplayMode || m_bPracticeMode || practice::Assisted() || m_playDifficulty<0 || m_playDifficulty>=4 ||
  m_playChara<0 || m_playChara>=4 || m_playStage<0 || m_playStage>=6) return;
 for(int i=0;i<practice::ToolCount;i++) if(practice::Enabled(practice::Tool(i))) return;
 m_practiceUnlocked[m_playChara][m_playDifficulty]|=1<<m_playStage;
 SavePracticeProgress();
}

void CGlobalVariableAndStatistics::OnClear()
{
	if (original::DemoActive() || practice::Assisted() || m_bPracticeMode) return;
	m_bClear[m_playChara][m_playDifficulty]=true;
	for (int i=0;i<4;i++)
		for (int j=1;j<4;j++)
			if (m_bClear[i][j])
				m_bExtraPlayable[i]=true;
}

void CGlobalVariableAndStatistics::Initialize()
{
	LoadProgramConfig();
	LoadConfig();
	LoadPracticeProgress();

	memset(m_bExtraPlayable,0,sizeof(m_bExtraPlayable));
	for (int i=0;i<4;i++)
		for (int j=1;j<4;j++)
			if (m_bClear[i][j])
				m_bExtraPlayable[i]=true;

	LoadHighScore();
}

void CGlobalVariableAndStatistics::LoadHighScore()
{
	for (int i=0;i<4;i++)
		for (int j=0;j<5;j++)
			for (int k=0;k<5;k++)
			{
				m_highScoreName[i][j][k][0]=0;
				m_highScore[i][j][k]=20000*(5-k);
				m_highScoreFlag[i][j][k]=(j==4?1:6-k);
			}

	FILE *fp=fopen("score.dat","rb");
	if (fp==NULL)
		return;
	fseek(fp,0,SEEK_END);
	if (ftell(fp)!=1500)
	{
		fclose(fp);
		return;
	}
	fseek(fp,0,SEEK_SET);
	for (int i=0;i<4;i++)
		for (int j=0;j<5;j++)
			for (int k=0;k<5;k++)
			{
				fread(m_highScoreName[i][j][k],1,10,fp);
				fread(&m_highScore[i][j][k],4,1,fp);
				fread(&m_highScoreFlag[i][j][k],1,1,fp);
			}
	fclose(fp);
}

void CGlobalVariableAndStatistics::SaveHighScore()
{
	FILE *fp=fopen("score.dat","wb");
	if (fp==NULL)
		return;
	for (int i=0;i<4;i++)
		for (int j=0;j<5;j++)
			for (int k=0;k<5;k++)
			{
				fwrite(m_highScoreName[i][j][k],1,10,fp);
				fwrite(&m_highScore[i][j][k],4,1,fp);
				fwrite(&m_highScoreFlag[i][j][k],1,1,fp);
			}
	fclose(fp);
}

void CGlobalVariableAndStatistics::OnBeginGame()
{
	practice::ResetRun();
 m_runTurboMode=m_bReplayMode?m_replay.m_turboMode:m_turboMode;
	//for replay mode
	if (m_bReplayMode)
	{
  m_practiceConfig=m_replay.m_practiceConfig;
		for (int i=0;i<7;i++)
			m_randomSeed[i]=m_replay.m_randSeed[i];
		m_curPower=m_replay.m_curPower[m_playStage];
		m_powerOverflowLevel=m_replay.m_powerOverflowLevel[m_playStage];
		m_curDream=m_replay.m_curDream[m_playStage];
		m_nCurLife=m_replay.m_nCurLife[m_playStage];
		m_nCurBomb=m_replay.m_nCurBomb[m_playStage];
		m_nLifeOnContinue=m_replay.m_nInitLife;
		m_nBombOnMiss=m_replay.m_nInitBomb;
		m_curScore=m_replay.m_curScore[m_playStage];
		m_totalNPointItem=m_replay.m_totalNPointItem[m_playStage];
		m_playerPerformance=m_replay.m_playerPerformance[m_playStage];
		m_bPracticeMode=m_replay.m_bPracticeMode;
		m_playDifficulty=m_replay.m_playDifficulty;
		m_playChara=m_replay.m_playChara;
		m_bCanSaveReplay=false;
		m_nContinueLimit=0;				//although not required, we set it here to avoid bug in replay.
										//which may cause game over in unexpected places
	}
	else
	{
		if(!m_bPracticeMode)m_practiceConfig=practice::Config();
		m_replay.Reset();
  m_replay.m_turboMode=m_runTurboMode;
		srand((unsigned int)time(NULL));
		for (int i=0;i<7;i++)
			m_randomSeed[i]=original::Random()*32768+original::Random();
		m_replay.m_nInitLife=m_nInitLife;
		m_replay.m_nInitBomb=m_nInitBomb;
		m_replay.m_playDifficulty=m_playDifficulty;
		m_replay.m_playChara=m_playChara;
		m_replay.m_bPracticeMode=m_bPracticeMode;
		m_curPower=1;
		m_powerOverflowLevel=0;
		m_curDream=1;
		m_nCurLife=m_nInitLife;
		m_nCurBomb=m_nInitBomb;
		m_nLifeOnContinue=m_nInitLife;
		m_nBombOnMiss=m_nInitBomb;
		m_nContinueLimit=3;
		if (m_playDifficulty==4)
		{
			m_nCurLife=3;
			m_nCurBomb=3;
			m_nContinueLimit=0;
		}
		m_curScore=0;
		m_totalNPointItem=0;
		int initP[]={32,32,44,48,32};
		m_playerPerformance=initP[m_playDifficulty];
		if (m_bPracticeMode)
		{
			m_curPower=128;
			m_curDream=128;
			m_nCurLife=9; // Eight spare lives; the native HUD displays life - 1.
			m_nCurBomb=8;
			m_nBombOnMiss=8;
			m_replay.m_nInitLife=9;
			m_replay.m_nInitBomb=8;
			m_nContinueLimit=0;
		}
		m_bCanSaveReplay=true;
	}

	m_curNGraze=0;
	int PL[]={16,24,44,48,32};
	int PU[]={32,40,54,58,32};
	m_playerPerformanceLowerBound=PL[m_playDifficulty];
	m_playerPerformanceUpperBound=PU[m_playDifficulty];

 if(m_bPracticeMode&&m_practiceConfig.mode){
  if(!m_bReplayMode){
   m_practiceConfig.Clamp(m_playDifficulty);
   const auto& p=m_practiceConfig;
   m_nCurLife=p.lives+1;m_nCurBomb=p.bombs;m_curScore=p.score/10;
   m_curPower=p.power;m_curDream=p.dream;m_totalNPointItem=p.totalPoints;m_playerPerformance=p.rank;
   m_replay.m_practiceConfig=p;m_replay.m_nInitLife=p.lives+1;m_replay.m_nInitBomb=p.bombs;
   practice::MarkAssisted();
  }
  m_curNGraze=m_practiceConfig.graze;
  m_nBombOnMiss=m_practiceConfig.bombs;
  if(m_practiceConfig.lockRank)m_playerPerformanceLowerBound=m_playerPerformanceUpperBound=m_practiceConfig.rank;
 }
	m_curHighScore=m_highScore[m_playChara][m_playDifficulty][0];
	m_nContinueUsed=0;
 original::BeginRun();

	int pScore[]={6000,10000,15000,20000,40000};
	m_pointItemMaxScore=pScore[m_playDifficulty];
	int grazeBonus[]={25,50,100,200,500};
	m_grazeBonus=grazeBonus[m_playDifficulty];

}


}













