#pragma once
#include "../scheme.h"
#include "../../gameLib/Graphic/2DImage.h"
#include "../../gameLib/th5DatFile/th5DatFile.h"
#include <array>
#include <string>
namespace th5w {
class CMusicRoom : public CScheme {
 friend struct PracticeTests;
 Cth5DatFile m_music;
 C2DImage* m_background;
 C2DImage* m_polygonBackground;
 GLuint m_font;
 unsigned char m_palette[48];
 int m_game,m_selection,m_playing,m_top,m_age,m_repeat,m_fade,m_commentAge;
 bool m_quit,m_ready,m_playFailed;
 unsigned short m_lastKey;
 std::array<std::string,10> m_comment;
 struct Polygon {float x,y,vx,vy;int angle,turn;};
 std::array<Polygon,14> m_polygons;
 void Input(unsigned short held);
 bool Play(int track);
 std::string Line(int game,int track,int line) const;
 void Text(std::string text,int x,int y,int color,float alpha=1);
 void DrawPianos();
 void DrawPolygons();
 public:
 CMusicRoom();
 ~CMusicRoom();
 static bool Available();
 bool Initialize();
 int Step();
 void Draw();
};
}
