#include ".\charabullet.h"
#include "../Stage.h"
#include "../EnemyBullet/EnemyBullet.h"
#include "../SparkEffect/SparkEffect.h"
#include "../../Game.h"
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

#include "../../Original/OriginalShots.h"

namespace th5w{

CCharaBullet::CCharaBullet(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CCharaBullet::~CCharaBullet(void)
{
}

void CCharaBullet::Initialize()
{
	m_originalHitCounter=0;
	memset(m_bullets,0,sizeof(m_bullets));
	memset(m_hitBullets,0,sizeof(m_hitBullets));
	m_clipXLow=-128;
	m_clipXHigh=6272;
	m_clipYLow=-128;
	m_clipYHigh=6016;
}

void CCharaBullet::Step()
{
	StepBullets();
	StepHitBullets();
}

void CCharaBullet::Draw()
{
	DrawBullets();
	DrawHitBullets();
}

void CCharaBullet::AddBullet(float bulletX, float bulletY, unsigned char imgIdx,unsigned char bulletType,
							 float velX, float velY, float moveAngle,unsigned char damage)
{
	for (int i=0;i<CHARA_BULLET_N_RECORD;i++)
		if (m_bullets[i].flag==0)
		{
			m_bullets[i].flag=1;
			m_bullets[i].age=0;
			m_bullets[i].bulletType=bulletType;
			m_bullets[i].curX=bulletX;
			m_bullets[i].curY=bulletY;
			m_bullets[i].damage=damage;
			m_bullets[i].lastX=bulletX;
			m_bullets[i].lastY=bulletY;
			m_bullets[i].imgIdx=imgIdx;
			if (moveAngle==ADD_CHARA_BULLET_IGNORE_MOVEANGLE)
			{
				m_bullets[i].velX=velX;
				m_bullets[i].velY=velY;
			}
			else
			{
				m_bullets[i].velX=(float)cos(moveAngle/256.0*2*PI)*192;
				m_bullets[i].velY=(float)sin(moveAngle/256.0*2*PI)*192;
				m_bullets[i].moveAngle=moveAngle;
    if(original::DemoActive()){m_bullets[i].velX=float(original::math::X(int(moveAngle),192));m_bullets[i].velY=float(original::math::Y(int(moveAngle),192));}
			}
			break;
		}
}

void CCharaBullet::ShootOriginal(int character,int level,int timer,float x,float y)
{
 using namespace original::shots;shot_time=timer;originX=int(x);originY=int(y);
 for(int i=0;i<64;i++){auto& src=m_bullets[i];auto& dst=slots[i];dst.flag=src.flag;}
 slots[64].flag=0;
 Run(character,level);
 for(int i=0;i<64;i++){auto& src=slots[i];auto& dst=m_bullets[i];if(!src.flag||dst.flag)continue;
 dst.flag=1;dst.age=src.age;dst.curX=dst.lastX=float(src.pos.cur.x.v);dst.curY=dst.lastY=float(src.pos.cur.y.v);dst.velX=float(src.pos.velocity.x.v);dst.velY=float(src.pos.velocity.y.v);dst.imgIdx=src.patnum_base;dst.bulletType=src.type;dst.damage=src.damage;dst.moveAngle=float(src.angle);}
}

void CCharaBullet::StepBullets()
{
	for (int i=0;i<CHARA_BULLET_N_RECORD;i++)
	{
		if (m_bullets[i].flag%2==0)
		{
			m_bullets[i].flag=0;
			continue;
		}
		switch(m_bullets[i].bulletType)
		{
		case CHARA_BULLET_TYPE_HOMING:
			{
				if (m_bullets[i].age>=16)
					m_bullets[i].bulletType=CHARA_BULLET_TYPE_NORMAL;
				if (m_pStage->m_bHomingEnemyExist==false)
					break;
				if(original::DemoActive()){
      auto& b=m_bullets[i];const int desired=original::math::Angle(int(m_pStage->m_homingEnemyY-b.curY),int(m_pStage->m_homingEnemyX-b.curX));
      const int diff=(int(b.moveAngle)-desired)&255;int angle=int(b.moveAngle);
      if(diff>=128){int step=(256-diff)/4;angle=step<3?desired:angle+step;}else{int step=diff/4;angle=step<3?desired:angle-step;}
      b.moveAngle=float(angle&255);b.velX=float(original::math::X(angle,192));b.velY=float(original::math::Y(angle,192));break;
     }
     float desiredAngle=atan2(m_pStage->m_homingEnemyY-m_bullets[i].curY,m_pStage->m_homingEnemyX-m_bullets[i].curX);
				desiredAngle=(float)(desiredAngle/2.0/PI*256);
				float deltaAngle=desiredAngle-m_bullets[i].moveAngle;
				if (deltaAngle>128.0f)
					deltaAngle-=256.0f;
				if (deltaAngle<-128.0f)
					deltaAngle+=256.0f;
				deltaAngle/=4;
				if (fabs(deltaAngle)<3)
					m_bullets[i].moveAngle=desiredAngle;
				else
					m_bullets[i].moveAngle+=deltaAngle;
				if (m_bullets[i].moveAngle>256.0f)
					m_bullets[i].moveAngle-=256.0f;
				if (m_bullets[i].moveAngle<0.0f)
					m_bullets[i].moveAngle+=256.0f;
				m_bullets[i].velX=(float)(cos(m_bullets[i].moveAngle/256*PI*2)*192);
				m_bullets[i].velY=(float)(sin(m_bullets[i].moveAngle/256*PI*2)*192);
				break;
			}
		case CHARA_BULLET_TYPE_MISSILE_LEFT:
			m_bullets[i].velX+=1.0f;
			m_bullets[i].velY-=4.0f;
			break;
		case CHARA_BULLET_TYPE_MISSILE_RIGHT:
			m_bullets[i].velX-=1.0f;
			m_bullets[i].velY-=4.0f;
		    break;
		case CHARA_BULLET_TYPE_MISSILE_STRAIGHT:
			m_bullets[i].velY-=4.0f;
		    break;
		}
		m_bullets[i].lastX=m_bullets[i].curX;
		m_bullets[i].lastY=m_bullets[i].curY;
		m_bullets[i].curX+=m_bullets[i].velX;
		m_bullets[i].curY+=m_bullets[i].velY;
		if (m_bullets[i].curX<=m_clipXLow||m_bullets[i].curX>=m_clipXHigh||
			m_bullets[i].curY<=m_clipYLow||m_bullets[i].curY>=m_clipYHigh)
			m_bullets[i].flag=2;

		m_bullets[i].age++;
	}
}

void CCharaBullet::StepHitBullets()
{
	for (int i=0;i<HIT_CHARA_BULLET_N_RECORD;i++)
	{
		if (m_hitBullets[i].age>=13)
			m_hitBullets[i].age=0;
		if (m_hitBullets[i].age!=0)
		{
			m_hitBullets[i].lastX=m_hitBullets[i].curX;
			m_hitBullets[i].lastY=m_hitBullets[i].curY;
			m_hitBullets[i].curX+=m_hitBullets[i].velX;
			m_hitBullets[i].curY+=m_hitBullets[i].velY;
			if (m_hitBullets[i].curX<=m_clipXLow||m_hitBullets[i].curX>=m_clipXHigh||
				m_hitBullets[i].curY<=m_clipYLow||m_hitBullets[i].curY>=m_clipYHigh)
				m_hitBullets[i].age=18;
			else
			{
				m_hitBullets[i].age++;
				if (m_hitBullets[i].age%3==1)
					m_hitBullets[i].imgIdx++;
			}
		}
	}
}

void CCharaBullet::DrawBullets()
{
	for (int i=0;i<CHARA_BULLET_N_RECORD;i++)
		if (m_bullets[i].flag==1)
			m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_bullets[i].imgIdx+m_bullets[i].age%2),
										   m_bullets[i].curX,m_bullets[i].curY);
}

void CCharaBullet::DrawHitBullets()
{
	for (int i=0;i<HIT_CHARA_BULLET_N_RECORD;i++)
		if (m_hitBullets[i].age>0&&m_hitBullets[i].age<13)
			m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_hitBullets[i].imgIdx),
										   m_hitBullets[i].curX,m_hitBullets[i].curY);
}

void CCharaBullet::ConvertToHitRecord(int bulletRecordIdx)
{
	m_bullets[bulletRecordIdx].flag=2;
	for (int i=0;i<HIT_CHARA_BULLET_N_RECORD;i++)
		if (m_hitBullets[i].age==0)
		{
			m_hitBullets[i].age=1;
			m_hitBullets[i].imgIdx=m_bullets[bulletRecordIdx].imgIdx==20?28:32;
			m_hitBullets[i].curX=m_bullets[bulletRecordIdx].curX;
			m_hitBullets[i].curY=m_bullets[bulletRecordIdx].curY;
			m_hitBullets[i].lastX=m_bullets[bulletRecordIdx].lastX;
			m_hitBullets[i].lastY=m_bullets[bulletRecordIdx].lastY;
			m_hitBullets[i].velX=m_bullets[bulletRecordIdx].velX/6;
			m_hitBullets[i].velY=m_bullets[bulletRecordIdx].velY/6;
			break;
		}
}

int CCharaBullet::GetDamage(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss,bool bReflectAndAddScore)
{
	ENEMY_SHOOT_INFO shootInfo;
	shootInfo.bulletBornType=18;
	shootInfo.bulletImg=0;
	shootInfo.bulletSpeed=24;
	shootInfo.shootType=EST00_SINGLE;
	m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);

	int nHit=0;
	int totalDamage=0;

	for (int i=0;i<CHARA_BULLET_N_RECORD;i++)
		if (m_bullets[i].flag==1)
			if (m_bullets[i].curX>=ctrX-halfWidth&&m_bullets[i].curX<=ctrX+halfWidth)
				if (m_bullets[i].curY>=ctrY-halfHeight&&m_bullets[i].curY<=ctrY+halfHeight)
				{
					nHit++;
					unsigned char actualDamage=m_bullets[i].damage/nHit;
					if (actualDamage==0)
						actualDamage=1;
					totalDamage+=actualDamage;
					if (original::DemoActive()?(++m_originalHitCounter&1):(nHit%2==1))
						m_pStage->m_pSparkEffect->AddEffect(m_bullets[i].curX,m_bullets[i].curY,128,1,false);
					if (bReflectAndAddScore)
					{
						if (!(CGame::GVar().m_playDifficulty==0&&m_pStage->m_curFrame%4!=0))
						{
							shootInfo.shootOrigX=m_bullets[i].curX;
							shootInfo.shootOrigY=m_bullets[i].curY;
							shootInfo.shootAngle=(float)(original::Random()%256);
							m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
						}
					}
					ConvertToHitRecord(i);
				}

	//damage for boss while in bomb is reduced to 1/4
	if (isBoss&&m_pStage->m_bInCharaBomb)
		totalDamage/=4;

	if (bReflectAndAddScore)
		m_pStage->AddScore(totalDamage);

	return totalDamage;
}

}
























