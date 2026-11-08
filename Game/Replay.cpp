#include ".\replay.h"
#include <stdio.h>
#include <time.h>
#include <cmath>
#include <algorithm>
#include "game.h"

namespace th5w{

CReplay::CReplay(void)
{
	m_opStage=-1;
	Reset();
}

CReplay::~CReplay(void)
{
}

bool CReplay::Reset()
{
	if (m_opStage!=-1)
		return false;

	m_nInitLife=0;
	m_nInitBomb=0;
	m_playDifficulty=0;
	m_playChara=0;
	m_bPracticeMode=false;
 m_turboMode=true;
 m_combatRevision=original::OriginalBossGeometry;
 m_practiceConfig=practice::Config();
	memset(m_stageFlag,0,sizeof(m_stageFlag));
	memset(m_nCurLife,0,sizeof(m_nCurLife));
	memset(m_nCurBomb,0,sizeof(m_nCurBomb));
	memset(m_curPower,0,sizeof(m_curPower));
	memset(m_powerOverflowLevel,0,sizeof(m_powerOverflowLevel));
	memset(m_curDream,0,sizeof(m_curDream));
	memset(m_curScore,0,sizeof(m_curScore));
	memset(m_totalNPointItem,0,sizeof(m_totalNPointItem));
	memset(m_playerPerformance,0,sizeof(m_playerPerformance));
	memset(m_stageScore,0,sizeof(m_stageScore));
	memset(m_randSeed,0,sizeof(m_randSeed));
	for (int i=0;i<7;i++)
        {m_keyState[i].clear();m_touchMotion[i].clear();}

	memset(m_playerName,0,sizeof(m_playerName));

	m_opIdx=0;
	return true;
}

bool CReplay::GetRepFileInfo(REPLAY_BRIEF_INFO* pOut, const char *fileName)
{
#define FAIL_CLOSE_SETERRORCODE_RETURN(x,y) if (x) {fclose(fp);pOut->infoRes=y;return false;}
	FILE *fp=fopen(fileName,"rb");
	if (fp==NULL)
	{
		pOut->infoRes=InfoRes_FileNotExist;
		return false;
	}

	char sig[]="RPY ";
	unsigned char buf[16];
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(buf,1,4,fp)!=4,InfoRes_InvalidReplay)
	FAIL_CLOSE_SETERRORCODE_RETURN((*(int*)buf)!=(*(int*)sig),InfoRes_InvalidReplay)
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(buf,1,4,fp)!=4,InfoRes_InvalidReplay)
	const int replayVersion=*(int*)buf;
	FAIL_CLOSE_SETERRORCODE_RETURN(replayVersion!=CGame::GVar().m_gameVersion&&replayVersion!=CGame::GVar().m_gameVersion+1,InfoRes_GameVersionMismatch)
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(buf,1,16,fp)!=16,InfoRes_InvalidReplay)
	for (int i=0;i<16;i++)
		FAIL_CLOSE_SETERRORCODE_RETURN(buf[i]!=CGame::GVar().m_modMD5[i],InfoRes_ModFileMisMatch)

	FAIL_CLOSE_SETERRORCODE_RETURN(fread(&pOut->playerName,1,8,fp)!=8,InfoRes_InvalidReplay)
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(&pOut->saveTime,4,1,fp)!=1,InfoRes_InvalidReplay)

	fseek(fp,8,SEEK_CUR);
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(&pOut->playDifficulty,4,1,fp)!=1,InfoRes_InvalidReplay)
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(&pOut->playChara,4,1,fp)!=1,InfoRes_InvalidReplay)
	fseek(fp,1,SEEK_CUR);

	FAIL_CLOSE_SETERRORCODE_RETURN(fread(&pOut->stageFlag,1,7,fp)!=7,InfoRes_InvalidReplay)

	fseek(fp,224,SEEK_CUR);
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(&pOut->stageScore,4,7,fp)!=7,InfoRes_InvalidReplay)

	fclose(fp);
 {CReplay parsed;if(!parsed.LoadFile(fileName)){pOut->infoRes=InfoRes_InvalidReplay;return false;}}
	pOut->infoRes=InfoRes_ValidReplay;
	return true;
#undef FAIL_CLOSE_SETERRORCODE_RETURN
}

bool CReplay::SaveFile(const char *fileName)
{
 if(m_practiceConfig.mode&&(!m_bPracticeMode||!m_practiceConfig.ValidReplay(m_playDifficulty)))return false;
#define FAIL_CLOSE_RETURN(x) if (x) {fclose(fp);return false;}
	FILE *fp=fopen(fileName,"wb");
	if (fp==NULL)
		return false;

	FAIL_CLOSE_RETURN(fwrite("RPY ",1,4,fp)!=4)
	const int replayVersion=CGame::GVar().m_gameVersion+(m_practiceConfig.mode?1:0);
	FAIL_CLOSE_RETURN(fwrite(&replayVersion,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fwrite(CGame::GVar().m_modMD5,1,16,fp)!=16)

	FAIL_CLOSE_RETURN(fwrite(&m_playerName,1,8,fp)!=8)
	m_saveTime=(unsigned int)time(NULL);
	FAIL_CLOSE_RETURN(fwrite(&m_saveTime,4,1,fp)!=1)

	FAIL_CLOSE_RETURN(fwrite(&m_nInitLife,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fwrite(&m_nInitBomb,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fwrite(&m_playDifficulty,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fwrite(&m_playChara,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fwrite(&m_bPracticeMode,1,1,fp)!=1)

	FAIL_CLOSE_RETURN(fwrite(m_stageFlag,1,7,fp)!=7)

	FAIL_CLOSE_RETURN(fwrite(m_nCurLife,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_nCurBomb,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_curPower,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_powerOverflowLevel,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_curDream,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_curScore,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_totalNPointItem,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_playerPerformance,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_stageScore,4,7,fp)!=7)

	FAIL_CLOSE_RETURN(fwrite(m_randSeed,4,7,fp)!=7)
	for (int i=0;i<7;i++)
		if (m_stageFlag[i]!=0)
		{
			int nkey=(int)m_keyState[i].size();
			FAIL_CLOSE_RETURN(fwrite(&nkey,4,1,fp)!=1)
			FAIL_CLOSE_RETURN(fwrite(m_keyState[i].data(),2,nkey,fp)!=nkey)
		}
 if(m_practiceConfig.mode){
  const unsigned int version=3,size=sizeof(practice::Config);
  FAIL_CLOSE_RETURN(fwrite("TPC5",1,4,fp)!=4)
  FAIL_CLOSE_RETURN(fwrite(&version,4,1,fp)!=1)
  FAIL_CLOSE_RETURN(fwrite(&size,4,1,fp)!=1)
  FAIL_CLOSE_RETURN(fwrite(&m_practiceConfig,1,size,fp)!=size)
 }
 // Record the combat rules so replay playback and seeking
 // reproduce the simulation used when the run was recorded.
 if(m_combatRevision!=original::LegacyCombat){
  const unsigned int version=m_combatRevision;
  FAIL_CLOSE_RETURN(fwrite("TSH5",1,4,fp)!=4)
  FAIL_CLOSE_RETURN(fwrite(&version,4,1,fp)!=1)
 }
 // Legacy Turbo files remain byte-compatible; Slow stores its run timing.
 if(!m_turboMode){const unsigned int version=1,mode=0;
  FAIL_CLOSE_RETURN(fwrite("TSP5",1,4,fp)!=4)
  FAIL_CLOSE_RETURN(fwrite(&version,4,1,fp)!=1)
  FAIL_CLOSE_RETURN(fwrite(&mode,4,1,fp)!=1)
 }
 bool touch=false;for(int i=0;i<7;i++)touch=touch||!m_touchMotion[i].empty();
 if(touch){const unsigned int version=1;
  FAIL_CLOSE_RETURN(fwrite("TCH5",1,4,fp)!=4)
  FAIL_CLOSE_RETURN(fwrite(&version,4,1,fp)!=1)
  for(int i=0;i<7;i++){const unsigned int count=m_touchMotion[i].size();
   FAIL_CLOSE_RETURN(fwrite(&count,4,1,fp)!=1)
   for(const auto& s:m_touchMotion[i]){FAIL_CLOSE_RETURN(fwrite(&s.frame,4,1,fp)!=1) FAIL_CLOSE_RETURN(fwrite(&s.x,4,1,fp)!=1) FAIL_CLOSE_RETURN(fwrite(&s.y,4,1,fp)!=1)}
  }
 }
	fclose(fp);
	return true;
#undef FAIL_CLOSE_RETURN
}

bool CReplay::LoadFile(const char *fileName)
{
 for(int stage=0;stage<7;stage++){m_keyState[stage].clear();m_touchMotion[stage].clear();}
 m_turboMode=true;
 m_combatRevision=original::LegacyCombat;
 m_practiceConfig=practice::Config();
#define FAIL_CLOSE_RETURN(x) if (x) {fclose(fp);return false;}
	FILE *fp=fopen(fileName,"rb");
	if (fp==NULL)
		return false;

	char sig[]="RPY ";
	unsigned char buf[16];
	FAIL_CLOSE_RETURN(fread(buf,1,4,fp)!=4)
	FAIL_CLOSE_RETURN((*(int*)buf)!=(*(int*)sig))
	FAIL_CLOSE_RETURN(fread(buf,1,4,fp)!=4)
	const int replayVersion=*(int*)buf;
	FAIL_CLOSE_RETURN(replayVersion!=CGame::GVar().m_gameVersion&&replayVersion!=CGame::GVar().m_gameVersion+1)
	FAIL_CLOSE_RETURN(fread(buf,1,16,fp)!=16)
	for (int i=0;i<16;i++)
		FAIL_CLOSE_RETURN(buf[i]!=CGame::GVar().m_modMD5[i])

	FAIL_CLOSE_RETURN(fread(&m_playerName,1,8,fp)!=8)
	FAIL_CLOSE_RETURN(fread(&m_saveTime,4,1,fp)!=1)

	FAIL_CLOSE_RETURN(fread(&m_nInitLife,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fread(&m_nInitBomb,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fread(&m_playDifficulty,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fread(&m_playChara,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fread(&m_bPracticeMode,1,1,fp)!=1)

	FAIL_CLOSE_RETURN(fread(m_stageFlag,1,7,fp)!=7)

	FAIL_CLOSE_RETURN(fread(m_nCurLife,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_nCurBomb,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_curPower,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_powerOverflowLevel,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_curDream,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_curScore,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_totalNPointItem,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_playerPerformance,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_stageScore,4,7,fp)!=7)

	FAIL_CLOSE_RETURN(fread(m_randSeed,4,7,fp)!=7)
	for (int i=0;i<7;i++)
		if (m_stageFlag[i]!=0)
		{
			int nkey;
			FAIL_CLOSE_RETURN(fread(&nkey,4,1,fp)!=1)
			FAIL_CLOSE_RETURN(nkey<0||nkey>10000000)
            const long dataStart=ftell(fp);fseek(fp,0,SEEK_END);const long bytesLeft=ftell(fp)-dataStart;fseek(fp,dataStart,SEEK_SET);
            FAIL_CLOSE_RETURN(nkey>bytesLeft/2)
			m_keyState[i].resize(nkey);
			FAIL_CLOSE_RETURN(fread(m_keyState[i].data(),2,nkey,fp)!=nkey)
		}
 if(replayVersion!=CGame::GVar().m_gameVersion){
  char sig[4];unsigned int version=0,size=0;
  FAIL_CLOSE_RETURN(fread(sig,1,4,fp)!=4||memcmp(sig,"TPC5",4)!=0)
  FAIL_CLOSE_RETURN(fread(&version,4,1,fp)!=1||(version<1||version>3))
  FAIL_CLOSE_RETURN(fread(&size,4,1,fp)!=1||size!=(version==1?14*4:version==2?18*4:sizeof(practice::Config)))
  FAIL_CLOSE_RETURN(fread(&m_practiceConfig,1,size,fp)!=size)
  FAIL_CLOSE_RETURN(m_playDifficulty<0||m_playDifficulty>4||m_playChara<0||m_playChara>3)
  FAIL_CLOSE_RETURN(!m_bPracticeMode||m_practiceConfig.mode!=1||!m_practiceConfig.ValidReplay(m_playDifficulty))
  for(int stage=0;stage<7;stage++)FAIL_CLOSE_RETURN((m_stageFlag[stage]!=0)!=(stage==m_practiceConfig.stage))
  FAIL_CLOSE_RETURN((m_practiceConfig.stage==6)!=(m_playDifficulty==4))
 }
 for(auto& motion:m_touchMotion)motion.clear();
 bool seenSpeed=false,seenTouch=false;
 while(true){const int first=fgetc(fp);if(first==EOF)break;char sig[4]={char(first),0,0,0};unsigned int version=0;
  FAIL_CLOSE_RETURN(fread(sig+1,1,3,fp)!=3||fread(&version,4,1,fp)!=1)
  if(!memcmp(sig,"TSH5",4)){
   FAIL_CLOSE_RETURN(m_combatRevision!=original::LegacyCombat||seenSpeed||seenTouch||
     version<original::OriginalPlayerShots||version>original::OriginalBossGeometry)
   m_combatRevision=version;
  }
  else if(version!=1){FAIL_CLOSE_RETURN(true)}
  else if(!memcmp(sig,"TSP5",4)){unsigned int mode=0;FAIL_CLOSE_RETURN(seenSpeed||seenTouch||fread(&mode,4,1,fp)!=1||mode>1) m_turboMode=mode!=0;seenSpeed=true;}
  else if(!memcmp(sig,"TCH5",4)){FAIL_CLOSE_RETURN(seenTouch)seenTouch=true;
   for(int stage=0;stage<7;stage++){unsigned int count=0;FAIL_CLOSE_RETURN(fread(&count,4,1,fp)!=1||count>m_keyState[stage].size())
    auto& samples=m_touchMotion[stage];samples.resize(count);unsigned int previous=0;
    for(unsigned int i=0;i<count;i++){auto& s=samples[i];FAIL_CLOSE_RETURN(fread(&s.frame,4,1,fp)!=1||fread(&s.x,4,1,fp)!=1||fread(&s.y,4,1,fp)!=1)
     FAIL_CLOSE_RETURN(s.frame>=m_keyState[stage].size()||(i&&s.frame<=previous)||!std::isfinite(s.x)||!std::isfinite(s.y)||fabs(s.x)>8192||fabs(s.y)>8192)previous=s.frame;}
   }
  }else{FAIL_CLOSE_RETURN(true)}
 }
	fclose(fp);
	return true;
#undef FAIL_CLOSE_RETURN
}

bool CReplay::OpenStage(int stageIdx,bool bRead)
{
	if (stageIdx<0||stageIdx>6)
		return false;
	if (m_opStage!=-1)
		return false;
	if (bRead&&m_stageFlag[stageIdx]==0)
		return false;
	if (bRead==false&&m_stageFlag[stageIdx]!=0)
		return false;

	if (!bRead)
	{
		m_nCurLife[stageIdx]=CGame::GVar().m_nCurLife;
		m_nCurBomb[stageIdx]=CGame::GVar().m_nCurBomb;
		m_curPower[stageIdx]=CGame::GVar().m_curPower;
		m_powerOverflowLevel[stageIdx]=CGame::GVar().m_powerOverflowLevel;
		m_curDream[stageIdx]=CGame::GVar().m_curDream;
		m_curScore[stageIdx]=CGame::GVar().m_curScore;
		m_totalNPointItem[stageIdx]=CGame::GVar().m_totalNPointItem;
		m_playerPerformance[stageIdx]=CGame::GVar().m_playerPerformance;
		m_stageScore[stageIdx]=CGame::GVar().m_curScore;
		m_randSeed[stageIdx]=CGame::GVar().m_randomSeed[stageIdx];
	}

	m_opStage=stageIdx;
	m_opIdx=0;
	m_bOpRead=bRead;
	return true;
}

bool CReplay::CloseStage(bool bCleared)
{
	if (m_opStage==-1)
		return false;
	if (!m_bOpRead)
	{
		m_stageFlag[m_opStage]=bCleared?2:1;
		m_stageScore[m_opStage]=CGame::GVar().m_curScore;
	}
	m_opStage=-1;
	return true;
}

void CReplay::TouchMotion(float& x,float& y,bool playback){
 if(m_opStage<0||m_opStage>6||m_opIdx<=0){x=y=0;return;}
 const unsigned int frame=m_opIdx-1;auto& samples=m_touchMotion[m_opStage];
 if(playback){x=y=0;auto found=std::lower_bound(samples.begin(),samples.end(),frame,[](const TouchSample& s,unsigned int f){return s.frame<f;});if(found!=samples.end()&&found->frame==frame){x=found->x;y=found->y;}}
 else if(x!=0||y!=0)samples.push_back({frame,x,y});
}

bool CReplay::GetNextData(unsigned short *pOut)
{
	if (m_opStage==-1)
		return false;
	if (!m_bOpRead)
		return false;
	if (m_opIdx>=(int)m_keyState[m_opStage].size())
		return false;
	(*pOut)=m_keyState[m_opStage][m_opIdx];
	m_opIdx++;
	return true;
}

bool CReplay::PutNextdata(unsigned short data)
{
	if (m_opStage==-1)
		return false;
	if (m_bOpRead)
		return false;
	m_keyState[m_opStage].push_back(data);
	m_opIdx++;
	return true;
}

}


















