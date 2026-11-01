// Windows adapter for ReC98 b6ba5b0a th02/op/m_music.cpp (GAME=5),
// th05/shiftjis/music.hpp and th05/op/piano_c.cpp. Resource files stay original.
#include "MusicRoom.h"
#include "MusicTracks.h"
#include "../Game.h"
#include "../../gameLib/CommonFunction/CommonFunctionInput.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"
#include "../../gameLib/CommonFunction/CommonFunctionMusicSE.h"
#include "../../gameLib/Graphic/PC98Font.h"
#include "../../gameLib/PMDPlayer/PMDPlayer.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
namespace th5w {
CMusicRoom::CMusicRoom():m_background(NULL),m_polygonBackground(NULL),m_font(0),m_game(4),m_selection(0),m_playing(0),m_top(0),m_age(0),m_repeat(0),m_fade(0),m_commentAge(0),m_quit(false),m_ready(false),m_playFailed(false),m_lastKey(0) {}
CMusicRoom::~CMusicRoom(){if(m_background)m_background->Destroy();if(m_polygonBackground)m_polygonBackground->Destroy();if(m_font)glDeleteTextures(1,&m_font);}
bool CMusicRoom::Available(){return GetFileAttributesA("music.dat")!=INVALID_FILE_ATTRIBUTES && GetFileAttributesA("music-font.bmp")!=INVALID_FILE_ATTRIBUTES;}
bool CMusicRoom::Initialize(){
 if(!Available()||!m_music.LoadFile("music.dat"))return false;
 for(int g=0;g<5;g++) {
  char comments[24];sprintf_s(comments,"_MUSIC%d.TXT",g);int id=m_music.GetChildFileIndex(comments);
  if(id<0||m_music.GetChildFileLength(id)<musicTrackCounts[g]*400)return false;
  for(int t=0;t<musicTrackCounts[g];t++) {
   const std::string name=musicFiles[g][t];
   if(m_music.GetChildFileIndex((name+".M2").c_str())<0 && m_music.GetChildFileIndex((name+".M").c_str())<0)return false;
  }
 }
 CCommonFunctionGraphic::LoadPIFromDat(&m_background,m_palette,&CGame::s_pCurGame->m_th5Dat1,"MUSIC.PI");
 if(!m_background)return false;
 // Original polygons set only the B plane: palette index |= 1. Preserve
 // the other three planes, including the background's stippled artwork.
 auto& dat=CGame::s_pCurGame->m_th5Dat1;int piID=dat.GetChildFileIndex("MUSIC.PI");
 CPIFile pi(dat.GetChildFileContentPointer(piID),dat.GetChildFileLength(piID));
 m_polygonBackground=C2DImage::Create2DImage(pi.GetWidth(),pi.GetHeight());
 if(!m_polygonBackground||!pi.ExtractImageAndPalette(m_polygonBackground->GetDataPointer(),NULL,NULL,255,1)||!m_polygonBackground->UploadToTexture())return false;
 m_font=CPC98Font::LoadAuxiliaryAtlas("music-font.bmp",true);
 if(!m_background||!m_font)return false;
 // Keep display-only randomness independent of the gameplay RNG.
 unsigned int seed=0x5a17;auto random=[&](){seed=seed*1664525+1013904223;return (seed>>16)&65535;};
 for(auto& p:m_polygons){p.x=float(random()%640);p.y=float(random()%400);p.vx=float(4-(int(random())&7));if(!p.vx)p.vx=1;p.vy=float(2+(random()&3));p.angle=random()&255;p.turn=4-(random()&7);if(!p.turn)p.turn=4;}
 m_ready=true;return Play(0);
}
std::string CMusicRoom::Line(int game,int track,int line) const {
 char name[24];sprintf_s(name,"_MUSIC%d.TXT",game);
 // Cth5DatFile's read accessors are non-const despite read-only storage.
 auto& dat=const_cast<Cth5DatFile&>(m_music);const int id=dat.GetChildFileIndex(name);
 if(id<0)return "";const int offset=track*400+line*40;
 if(offset+38>dat.GetChildFileLength(id))return "";
 const char* data=reinterpret_cast<const char*>(dat.GetChildFileContentPointer(id));
 std::string text(data+offset,38);while(!text.empty()&&(text.back()==' '||text.back()=='\r'||text.back()=='\n'||text.back()==0))text.pop_back();return text;
}
bool CMusicRoom::Play(int track){
 if(track<0||track>=musicTrackCounts[m_game])return false;
 std::string name=musicFiles[m_game][track];int id=m_music.GetChildFileIndex((name+".M2").c_str());
 if(id<0)id=m_music.GetChildFileIndex((name+".M").c_str());
 if(id<0)return false;
 if(!CPMDPlayer::LoadPMDData(m_music.GetChildFileContentPointer(id),m_music.GetChildFileLength(id))){m_playFailed=true;return false;}
 CPMDPlayer::SetVolume(100);CPMDPlayer::Play();m_playing=track;m_playFailed=false;
 for(int i=0;i<10;i++)m_comment[i]=Line(m_game,track,i);m_commentAge=0;return true;
}
void CMusicRoom::Input(unsigned short held){
 if(m_quit)return;
 const unsigned short pressed=held&~m_lastKey;m_lastKey=held;
 if(!held){m_repeat=0;return;}++m_repeat;
 const bool repeat=m_repeat>=24&&(m_repeat-24)%4==0;
 const auto key=pressed|(repeat?(held&15):0);
 if(key&4096){m_quit=true;return;}
 const int count=musicTrackCounts[m_game];
 if(key&3){m_selection=(m_selection+((key&1)?count:1))%(count+1);}
 else if(pressed&12){m_game=(m_game+((pressed&4)?4:1))%5;m_selection=m_top=0;Play(0);}
 if(m_selection<m_top)m_top=m_selection;
 if(m_selection>=m_top+12)m_top=m_selection-11;
 if(pressed&(32|8192)){if(m_selection==musicTrackCounts[m_game])m_quit=true;else Play(m_selection);}
}
int CMusicRoom::Step(){
 if(!m_ready)return 1;
 if(m_quit){m_fade=(std::max)(0,m_fade-8);return m_fade?0:1;}
 m_fade=(std::min)(100,m_fade+8);++m_age;++m_commentAge;Input(CCommonFunctionInput::GetAllKeyState());
 for(auto& p:m_polygons){p.x+=p.vx;p.y+=p.vy;p.angle=(p.angle+p.turn)&255;if(p.x<=0||p.x>=639)p.vx=-p.vx;if(p.y>=500){p.y=-100;p.x=float((m_age*37)%640);}}
 return 0;
}
void CMusicRoom::Text(std::string text,int x,int y,int color,float alpha){if(text.empty()||text[0]==';')return;CPC98Font::DrawStringFromAtlas(m_font,&text[0],int(text.size()),x,y+40,m_palette[color*3]/255.f,m_palette[color*3+1]/255.f,m_palette[color*3+2]/255.f,alpha);}
void CMusicRoom::DrawPolygons(){
 glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,m_polygonBackground->GetTexID());glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);
 glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,640,480,0,-1,1);glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
 glEnable(GL_SCISSOR_TEST);glScissor(0,40,640,400);
 glColor4f(1,1,1,1);int tw=1,th=1;while(tw<m_polygonBackground->m_width)tw*=2;while(th<m_polygonBackground->m_height)th*=2;
 for(int i=0;i<14;i++) {const auto& p=m_polygons[i];int n=i/4+3;float radius=float((i&3)*16+64);glBegin(GL_POLYGON);for(int j=0;j<n;j++){float a=float((j*256/n+p.angle)&255)*6.28318530718f/256;float x=p.x+std::cos(a)*radius,y=p.y+std::sin(a)*radius;glTexCoord2f(x/tw,(m_polygonBackground->m_height-y)/th);glVertex2f(x,y+40);}glEnd();}
 glDisable(GL_SCISSOR_TEST);glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);
}
void CMusicRoom::DrawPianos(){
 const auto notes=CPMDPlayer::PianoNotes();const int whiteMap[12]={0,0,1,1,2,3,3,4,4,5,5,6};
 const int blackX[12]={0,3,0,7,0,0,15,0,19,0,23,0};
 for(int row=0;row<6;row++){
  int y=64+40+row*18;
  CCommonFunctionGraphic::DrawRectangle(384.f,float(y),608.f,float(y+14),.8f,.8f,.8f);
  for(int key=0;key<=56;key++)CCommonFunctionGraphic::DrawRectangle(float(384+key*4),float(y),float(384+key*4),float(y+14),0,0,0);
  for(int octave=0;octave<8;octave++)for(int note=0;note<12;note++)if(blackX[note])CCommonFunctionGraphic::DrawRectangle(float(384+octave*28+blackX[note]-1),float(y),float(384+octave*28+blackX[note]+1),float(y+8),0,0,0);
  for(int part=0;part<8;part++)if((row<5&&part==row)||(row==5&&part>=5&&part<7)){
   int n=notes[part],octave=n>>4,note=n&15;if(n<0||n==255||note>=12||octave>=8)continue;
   int x=384+octave*28+(blackX[note]?blackX[note]-1:whiteMap[note]*4+1);
   CCommonFunctionGraphic::DrawRectangle(float(x),float(y+1),float(x+2),float(y+(blackX[note]?7:13)),0,.75f,.75f);
  }
 }
 // Original labels are 8x8 sprites, not the resident 8x16 text font.
 glDisable(GL_TEXTURE_2D);glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);
 glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,640,480,0,-1,1);glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
 glColor4f(m_palette[15]/255.f,m_palette[16]/255.f,m_palette[17]/255.f,1);glBegin(GL_QUADS);
 for(int row=0;row<6;row++)for(int col=0;col<3;col++){
  const int glyph=row==5?(col==2?8:7):(col==0?0:col==1?1:row+2);
  for(int y=0;y<8;y++)for(int x=0;x<8;x++)if(musicPianoLabels[glyph][y]&(128>>x)){
   const float px=float(352+col*8+x),py=float(64+40+4+row*18+y);
   glVertex2f(px,py);glVertex2f(px+1,py);glVertex2f(px+1,py+1);glVertex2f(px,py+1);
  }
 }
 glEnd();glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);
}
void CMusicRoom::Draw(){
 glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT);if(!m_ready)return;
 m_background->Draw(0,40);DrawPolygons();DrawPianos();
 Text(musicGameLabels[m_game],12,32,3);Text("             ------ \x81\xaa ------       ",12,80,5);Text("             ------ \x81\xab ------       ",12,288,5);
 for(int row=0;row<12;row++){
  const int track=m_top+row;if(track>musicTrackCounts[m_game])break;
  std::string title=musicChoices[m_game][track];
  Text(title,12,96+row*16,track==m_playing?3:5);
  if(track==m_selection){const float y=float(96+40+row*16);for(int edge=0;edge<2;edge++)CCommonFunctionGraphic::DrawRectangle(12,y+edge*15,300,y+edge*15,m_palette[15]/255.f,m_palette[16]/255.f,m_palette[17]/255.f);CCommonFunctionGraphic::DrawRectangle(12,y,12,y+15,m_palette[15]/255.f,m_palette[16]/255.f,m_palette[17]/255.f);CCommonFunctionGraphic::DrawRectangle(300,y,300,y+15,m_palette[15]/255.f,m_palette[16]/255.f,m_palette[17]/255.f);}
 }
 const float alpha=(std::min)(1.f,m_commentAge/12.f);Text(m_comment[0],320,32,7,alpha);for(int i=1;i<10;i++)Text(m_comment[i],320,180+(i-1)*16,7,alpha);
 if(m_playFailed)Text("Music playback failed",12,320,7);
 CCommonFunctionGraphic::ScreenFade(float(m_fade));
}
}
