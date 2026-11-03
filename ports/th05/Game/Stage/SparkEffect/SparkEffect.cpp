#include ".\sparkeffect.h"
#include "../Stage.h"
#include <string.h>
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

CSparkEffect::CSparkEffect(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CSparkEffect::~CSparkEffect(void)
{
}

void CSparkEffect::Initialize()
{
	memset(m_spark,0,sizeof(m_spark));m_nativeRingOffset=0;
 if(original::DemoActive())for(int i=0;i<96;i++){const int angle=original::IRandom()&255;if(i<64)m_spark[i].angle=float(angle);}
	m_clipXLow=-64;
	m_clipXHigh=6208;
	m_clipYLow=-64;
	m_clipYHigh=5952;
}

void CSparkEffect::AddEffect(float origX,float origY,float baseSpeed,int nTrail,bool isExplode)
{
	if(original::DemoActive()){
  if(origX<0||origX>384*16||origY<0||origY>368*16)return;
  for(int i=0;i<nTrail;i++){
   auto& p=m_spark[m_nativeRingOffset];m_nativeRingOffset=(m_nativeRingOffset+1)%64;if(p.flag)continue;
   p.flag=1;p.age=0;p.curX=origX;p.curY=origY;
   const int angle=isExplode?((nTrail-i)*256/nTrail)&255:int(p.angle);
   const int speed=int(baseSpeed)+(isExplode?0:(original::Random()&31));
   p.velX=float(original::math::X(angle,speed));p.velY=float(original::math::Y(angle,speed));
  }
  return;
 }
 int curAdd=0;
	for (int i=0;i<SPARK_EFFECT_N_RECORD;i++)
	{
		if (m_spark[i].flag!=0)
			continue;
		m_spark[i].flag=1;
		m_spark[i].age=0;
        m_spark[i].curX=origX;
		m_spark[i].curY=origY;

		float speed;
		if (isExplode)
		{
			m_spark[i].angle=(float)curAdd*256.0f/(float)nTrail;
			speed=baseSpeed;
		}
		else
		{
			m_spark[i].angle=(float)(original::Random()%256);
			speed=baseSpeed+(float)(original::Random()%32);
		}
		m_spark[i].velX=(float)cos((m_spark[i].angle/256.0f*2*PI))*speed;
		m_spark[i].velY=(float)sin((m_spark[i].angle/256.0f*2*PI))*speed;
		curAdd++;
		if (curAdd==nTrail)
			break;
	}
}

void CSparkEffect::Step()
{
	for (int i=0;i<SPARK_EFFECT_N_RECORD;i++)
	{
		if (m_spark[i].flag!=1)
		{
			m_spark[i].flag=0;
			continue;
		}
		m_spark[i].lastX=m_spark[i].curX;
		m_spark[i].lastY=m_spark[i].curY;
		m_spark[i].curX+=m_spark[i].velX;
		m_spark[i].curY+=m_spark[i].velY;
		if (m_spark[i].curX<=m_clipXLow||m_spark[i].curX>=m_clipXHigh
			||m_spark[i].curY<=m_clipYLow||m_spark[i].curY>=m_clipYHigh)
		{
			m_spark[i].flag=2;
			continue;
		}
		m_spark[i].velY+=1.0f;
		m_spark[i].age++;
		if (m_spark[i].age>40)
			m_spark[i].flag=2;
	}
}

void CSparkEffect::Draw()
{
	float color[3];
	color[0]=m_pStage->m_pStageRes->m_palette[12*3]/255.0f;
	color[1]=m_pStage->m_pStageRes->m_palette[12*3+1]/255.0f;
	color[2]=m_pStage->m_pStageRes->m_palette[12*3+2]/255.0f;
	for (int i=0;i<SPARK_EFFECT_N_RECORD;i++)
		if (m_spark[i].flag==1)
			m_pStage->m_pStageRes->DrawStageObjectImageColorScaled(m_pStage->m_pStageRes->m_pImgSpark[m_spark[i].age%8],
													  m_spark[i].curX,m_spark[i].curY,color[0],color[1],color[2]);
}

}
