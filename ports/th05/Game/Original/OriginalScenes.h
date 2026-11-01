#pragma once
#include "../Scheme.h"
#include "OriginalFeatures.h"
#include "../../gameLib/Graphic/2DImageArray.h"
#include <array>
#include <string>
#include <map>
namespace th5w {
// TH05 MAINE: staffroll_animate, allcast_animate and verdict_animate.
// The original 640x400 image is letterboxed at y=40 in the Windows port.
class COriginalEnd : public CScheme {
 friend struct PracticeTests;
public:
 enum Mode {Staff,AllCast,VerdictOnly};
 explicit COriginalEnd(Mode mode);
 ~COriginalEnd();
 bool Initialize();
 int Step();
 void Draw();
private:
 Mode m_mode;
 original::RunStats m_stats;
 original::Verdict m_verdict;
 C2DImage* m_background;
 GLuint m_font;
 C2DImageArray m_cards,m_sprites;
 unsigned char m_palette[48];
 std::map<std::wstring,C2DImage*> m_chineseText;
 int m_age,m_measure,m_phase,m_phaseAge,m_card,m_cardAge,m_cardOut,m_auxAge,m_auxOut;
 int m_fadeOut,m_scroll,m_scrollDirection,m_cameraAge,m_rainAge,m_particleIndex,m_rainIndex;
 int m_windowX,m_windowY,m_windowW,m_windowH,m_cameraX,m_cameraY,m_decay,m_orbRadius;
 bool m_cameraSway,m_released,m_ready;
 unsigned short m_lastKey;
 unsigned int m_seed;
 struct Particle {int x,y,vx,vy,speed,angle,cel,sway;bool active;Particle():x(0),y(0),vx(0),vy(0),speed(0),angle(0),cel(0),sway(0),active(false){}};
 std::array<Particle,64> m_particles;
 std::array<Particle,48> m_stars;
 std::array<Particle,8> m_trails;
 Particle m_orb;
 int m_castScreen,m_castLine,m_castTarget,m_castAnim,m_castState;
 int m_testMeasure; // Private clock override used by the production test fixture.
 void Tick(unsigned short keys);
 void StaffTick(unsigned short keys);
 void CastTick();
 void LoadCastPicture();
 void StartMusic(const char* name);
 void Text(const char* text,int x,int y,int color,bool bold=true,bool translated=false);
 C2DImage* ChineseImage(const wchar_t* text);
 void Chinese(const wchar_t* text,int x,int y,int color);
 void Number(const char* value,int x,int y,int color);
 void Fraction(unsigned int value,int x,int y,int color,bool percent);
 void DrawVerdict(int x,int y,int color,bool comments);
 void DrawScores(int x,int y,int color);
 void DrawStaff();
 void DrawCast();
 void DrawCard(int slot,int age,int out,int cx,int cy);
 bool CreditTick(int slot,int target,int& age,int& out);
 void InitSpace();
 void Gather();
 void Burst();
 bool CameraTick();
 bool RainTick();
 void SpaceTick();
 void DrawSpace();
 int Random();
 void Velocity(Particle& p);
 void SpawnOrbParticle();
 void Circle(int x,int y,int radius,int color);
};
}
