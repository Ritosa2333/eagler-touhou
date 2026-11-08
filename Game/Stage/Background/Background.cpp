#include ".\background.h"
#include "../Stage.h"
#include "../../Game.h"
#include "../../Original/OriginalFeatures.h"

namespace th5w{

CBackground::CBackground(CStage *pStage)
{
	m_mapTileIdxArray=NULL;
	m_pStage=pStage;
	Initialize();
}

CBackground::~CBackground(void)
{
#ifdef __EMSCRIPTEN__
 if(m_tileAtlas)m_tileAtlas->Destroy();
#endif
	if (m_mapTileIdxArray)
		delete m_mapTileIdxArray;
	m_mapTileIdxArray=NULL;
}

void CBackground::Initialize()
{
	if (m_pStage->m_pStageRes->m_stageMap!=NULL)
	{
		int nBlock=m_pStage->m_pStageRes->m_pStageStd->m_nBGBlockIdx;
		m_mapTileIdxArray=new unsigned char[nBlock*5*24];
		for (int i=0;i<nBlock;i++)
		{
			int blockIdx=m_pStage->m_pStageRes->m_pStageStd->m_pBGBlockIdxArray[i]/2;
			unsigned char *ptr=m_pStage->m_pStageRes->m_stageMap+blockIdx*320;
			for (int j=0;j<5;j++)
			{
				for (int k=0;k<24;k++)
				{
					int v=ptr[k*2]+ptr[k*2+1]*256;
					int x=(v%80*8-576)/16;
					int y=(v/80)/16;
					int patternIdx=x*25+y;
					m_mapTileIdxArray[i*120+(4-j)*24+k]=patternIdx;
				}
				ptr+=64;
			}
		}
	}
#ifdef __EMSCRIPTEN__
 if(m_tileAtlas){m_tileAtlas->Destroy();m_tileAtlas=nullptr;}
 if(m_pStage->m_pStageRes->m_mpnArray.NImage()){
  m_tileAtlas=C2DImage::Create2DImage(256,256);auto* pixels=m_tileAtlas->GetDataPointer();
  unsigned char tile[16*16*4];
  for(int n=0;n<m_pStage->m_pStageRes->m_mpnArray.NImage();n++){
   glBindTexture(GL_TEXTURE_2D,m_pStage->m_pStageRes->m_mpnArray.GetImagePtr(n)->GetTexID());
   glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,tile);
   for(int row=0;row<16;row++)memcpy(pixels+(((n/16)*16+row)*256+(n%16)*16)*4,tile+row*16*4,16*4);
  }
  m_tileAtlas->UploadToTexture();
 }
#endif
	m_curPos=0;
	m_curSpeed=m_pStage->m_pStageRes->m_pStageStd->m_nBGScroolSpeed;
	m_curDeltaPixel=0;
	m_bDraw=true;
	m_bForceDraw=false;
}

void CBackground::Step()
{
	int nextPos=m_curPos+m_curSpeed;
	// Native tile rows advance on whole pixels, not fractional subpixels.
	const int section=(nextPos/16+79)/80;
	if(section>(m_curPos/16+79)/80)
		m_curSpeed=m_pStage->m_pStageRes->m_pStageStd->m_pBGScrollSpeedArray[section+4];
	m_curDeltaPixel=nextPos/16-m_curPos/16;
	m_curPos=nextPos;
}

void CBackground::Draw()
{
	if (m_bDraw==false)
		return;

	if (CGame::GVar().m_playStage==5)
	{
		C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(16);
		m_pStage->m_pStageRes->ClearPlayArea(0,0,0);
		pImage->Draw((float)32-32+m_pStage->m_playAreaUpperLeftX,(float)120-16+m_pStage->m_playAreaUpperLeftY);
		return;
	}
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,CGame::s_pCurGame->m_windowWidth,
			   0,CGame::s_pCurGame->m_windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	unsigned char *ptr=m_mapTileIdxArray;
	int yOff=0;
	if (m_bForceDraw==false)
	{
		ptr+=m_curPos/256*24;
		yOff=m_curPos%256/16;
	}
#ifdef __EMSCRIPTEN__
 if(m_tileAtlas){
  glBindTexture(GL_TEXTURE_2D,m_tileAtlas->GetTexID());glBegin(GL_QUADS);
  for(int i=0;i<25;i++)for(int j=0;j<24;j++){
   const int tile=*ptr++;const float u=(tile%16)/16.f,v=(tile/16)/16.f;
   const float x=float(m_pStage->m_playAreaUpperLeftX+j*16);
   const float top=float(m_pStage->m_playAreaUpperLeftY-16+(24-i)*16+yOff);
   const float y=CGame::s_pCurGame->m_gameWindow.m_height-1-top-15;
   glTexCoord2f(u,v);glVertex2f(x,y);glTexCoord2f(u+1/16.f,v);glVertex2f(x+16,y);
   glTexCoord2f(u+1/16.f,v+1/16.f);glVertex2f(x+16,y+16);glTexCoord2f(u,v+1/16.f);glVertex2f(x,y+16);
  }
  glEnd();glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();return;
 }
#endif
	for (int i=0;i<25;i++)
		for (int j=0;j<24;j++)
		{
			GLuint texID=m_pStage->m_pStageRes->m_mpnArray.GetImagePtr(*ptr)->GetTexID();
			float drawX=(float)m_pStage->m_playAreaUpperLeftX+j*16;
			float drawY=(float)m_pStage->m_playAreaUpperLeftY-16+(24-i)*16+yOff;
			drawY=CGame::s_pCurGame->m_gameWindow.m_height-1-drawY-(16-1);
			glBindTexture(GL_TEXTURE_2D,texID);
			glBegin(GL_QUADS);
				glTexCoord2d(0,0);
				glVertex2f(drawX,drawY);
				glTexCoord2d(1,0);
				glVertex2f(drawX+16,drawY);
				glTexCoord2d(1,1);
				glVertex2f(drawX+16,drawY+16);
				glTexCoord2d(0,1);
				glVertex2f(drawX,drawY+16);
			glEnd();

			ptr++;
		}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

}










