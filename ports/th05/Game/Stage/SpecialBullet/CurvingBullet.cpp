#include ".\curvingbullet.h"
#include "../Stage.h"
#include "../Chara/Chara.h"

namespace th5w{

CCurvingBullet::CCurvingBullet(void)
{
}

CCurvingBullet::~CCurvingBullet(void)
{
}

void CCurvingBullet::Initialize()
{
	memset(m_bullet,0,sizeof(m_bullet));
}

void CCurvingBullet::AddBullet(float origX,float origY,float angle, float speed, unsigned char colorIdx)
{
	for (int i=0;i<(original::DemoActive()?7:CURVING_BULLET_N_RECORD);i++)
		if (m_bullet[i].flag==0)
		{
			m_bullet[i].flag=1;
			m_bullet[i].angle=angle;
			m_bullet[i].speed=speed;
			m_bullet[i].velX=(float)cos(m_bullet[i].angle/256.0f*2*PI)*m_bullet[i].speed;
			m_bullet[i].velY=(float)sin(m_bullet[i].angle/256.0f*2*PI)*m_bullet[i].speed;
			m_bullet[i].color[0]=m_pStage->m_pStageRes->m_palette[colorIdx*3]/255.0f;
			m_bullet[i].color[1]=m_pStage->m_pStageRes->m_palette[colorIdx*3+1]/255.0f;
			m_bullet[i].color[2]=m_pStage->m_pStageRes->m_palette[colorIdx*3+2]/255.0f;
			for (int j=0;j<16;j++)
			{
				m_bullet[i].nodeX[j]=origX;
				m_bullet[i].nodeY[j]=origY;
			}
   if(original::DemoActive()){
    auto& b=m_bullet[i];b.angle=float(uint8_t(int(angle)));b.speed=float(uint8_t(int(speed)));
    b.demoHeadX=int(origX);b.demoHeadY=int(origY);
    b.velX=float(original::math::X(int(b.angle),int(b.speed)));
    b.velY=float(original::math::Y(int(b.angle),int(b.speed)));
   }
			m_bullet[i].mode=1;
			break;
		}
}

void CCurvingBullet::Step()
{
 if(original::DemoActive()){
  for(int i=0;i<7;i++){
   auto& b=m_bullet[i];if(!b.flag)continue;b.age=uint16_t(b.age+1);
   const int px=int(m_pStage->m_pChara->GetCurX()),py=int(m_pStage->m_pChara->GetCurY());
   for(int j=15;j>0;j--){
    b.nodeX[j]=b.nodeX[j-1];b.nodeY[j]=b.nodeY[j-1];
    if(abs(int(b.nodeX[j])-px)<=96&&abs(int(b.nodeY[j])-py)<=96)m_pStage->m_pChara->m_bMiss=true;
   }
   b.nodeX[0]=float(b.demoHeadX);b.nodeY[0]=float(b.demoHeadY);
   if(b.nodeX[15]<=-16*16||b.nodeX[15]>=400*16||b.nodeY[15]<=-16*16||b.nodeY[15]>=384*16){b.flag=0;continue;}
   b.demoHeadX=int16_t(b.demoHeadX+int(b.velX));b.demoHeadY=int16_t(b.demoHeadY+int(b.velY));
   if(abs(b.demoHeadX-px)<=128&&abs(b.demoHeadY-py)<=128)m_pStage->m_pChara->m_bMiss=true;
   if(b.mode==1){b.speed=float(uint8_t(int(b.speed)-1));if(b.speed<=4)b.mode=2;}
   else b.speed=float(uint8_t(int(b.speed)+(m_pStage->m_curFrame&1)+1));
   const int aim=original::math::Angle(py-b.demoHeadY,px-b.demoHeadX);
   int delta=uint8_t(int(b.angle)-aim);if(delta>2&&delta<254)delta=delta>=128?-1:1;
   b.angle=float(uint8_t(int(b.angle)-delta));
   b.velX=float(original::math::X(int(b.angle),int(b.speed)));
   b.velY=float(original::math::Y(int(b.angle),int(b.speed)));
  }
  return;
 }
	for (int i=0;i<CURVING_BULLET_N_RECORD;i++)
	{
		if (m_bullet[i].flag==0)
			continue;
		m_bullet[i].age++;
		for (int j=15;j>=1;j--)
		{
			m_bullet[i].nodeX[j]=m_bullet[i].nodeX[j-1];
			m_bullet[i].nodeY[j]=m_bullet[i].nodeY[j-1];
			if (m_pStage->m_pChara->TestCrash(m_bullet[i].nodeX[j],m_bullet[i].nodeY[j],6.0f*16.0f,6.0f*16.0f))
				m_pStage->m_pChara->m_bMiss=true;
		}
		if (m_bullet[i].nodeX[15]<=-16.0f*16.0f||m_bullet[i].nodeX[15]>=400.0f*16.0f
			||m_bullet[i].nodeY[15]<=-16.0f*16.0f||m_bullet[i].nodeY[15]>=384.0f*16.0f)
		{
			m_bullet[i].flag=0;
			continue;
		}
		m_bullet[i].nodeX[0]+=m_bullet[i].velX;
		m_bullet[i].nodeY[0]+=m_bullet[i].velY;
		if (m_pStage->m_pChara->TestCrash(m_bullet[i].nodeX[0],m_bullet[i].nodeY[0],8.0f*16.0f,8.0f*16.0f))
			m_pStage->m_pChara->m_bMiss=true;

		if (m_bullet[i].mode==1)
		{
			m_bullet[i].speed-=1;
			if (m_bullet[i].speed<=4)
				m_bullet[i].mode=2;
		}
		else
			m_bullet[i].speed+=(float)(m_pStage->m_curFrame%2+1);
		int deltaAngle=(int)(m_bullet[i].angle-
							 m_pStage->m_pChara->AngleToChara(m_bullet[i].nodeX[0],m_bullet[i].nodeY[0]));
		while(deltaAngle<-128)
			deltaAngle+=256;
		while(deltaAngle>=128)
			deltaAngle-=256;
		if (abs(deltaAngle)>2)
		{
			if (deltaAngle>0)
				m_bullet[i].angle-=1;
			else
				m_bullet[i].angle+=1;
		}
		else
			m_bullet[i].angle-=deltaAngle;
		m_bullet[i].velX=(float)cos(m_bullet[i].angle/256.0f*2*PI)*m_bullet[i].speed;
		m_bullet[i].velY=(float)sin(m_bullet[i].angle/256.0f*2*PI)*m_bullet[i].speed;
	}
}

void CCurvingBullet::Draw()
{
	for (int i=0;i<CURVING_BULLET_N_RECORD;i++)
	{
		if (m_bullet[i].flag==0)
			continue;
		for (int j=0;j<16;j++)
			m_pStage->m_pStageRes->DrawCircle(m_bullet[i].nodeX[j],m_bullet[i].nodeY[j],10.0f*16.0f,
								 m_bullet[i].color[0],m_bullet[i].color[1],m_bullet[i].color[2],true);
	}
}

}























