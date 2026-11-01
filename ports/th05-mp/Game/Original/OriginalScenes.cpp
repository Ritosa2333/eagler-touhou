#include "OriginalScenes.h"
#include "OriginalText.h"
#include "../Game.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"
#include "../../gameLib/CommonFunction/CommonFunctionMusicSE.h"
#include "../../gameLib/CommonFunction/CommonFunctionInput.h"
#include "../../gameLib/Graphic/PC98Font.h"
#include "../../gameLib/Graphic/Th5ExtFont.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
namespace th5w {
namespace {
const double Tau=6.2831853071795864769;
const int CreditTargets[]={76,76,92,120,120,172,188,204,220,236,3996};
const int CastCount[]={1,2,6,10,9,12,7};
const int CastFiles[4][7]={{1,7,8,9,10,16,15},{1,11,11,11,3,16,15},{1,12,12,13,4,16,15},{1,14,5,14,6,16,15}};
const int CastQuarters[4][7]={{1,0,1,3,0,0,0},{1,0,1,2,1,1,1},{1,2,0,1,0,2,2},{1,0,0,2,0,3,3}};
void Clip(int x,int y,int w,int h){glEnable(GL_SCISSOR_TEST);glScissor(x,480-y-h,w,h);}
void Unclip(){glDisable(GL_SCISSOR_TEST);}
void Stipple(const unsigned short* mask,int x,int y){
 GLubyte pixels[128]={};
 for(int row=0;row<32;row++)for(int col=0;col<32;col++)
  if(mask[(479-row-y)&3]&(1<<(15-((col-x)&15))))pixels[row*4+col/8]|=128>>(col%8);
 glPolygonStipple(pixels);glEnable(GL_POLYGON_STIPPLE);
}
}
COriginalEnd::COriginalEnd(Mode mode):m_mode(mode),m_stats(original::Stats()),m_verdict(original::Evaluate(m_stats)),m_background(NULL),m_font(0),m_age(0),m_measure(0),m_phase(0),m_phaseAge(0),m_card(0),m_cardAge(0),m_cardOut(-1),m_auxAge(0),m_auxOut(-1),m_fadeOut(0),m_scroll(0),m_scrollDirection(0),m_cameraAge(0),m_rainAge(0),m_particleIndex(0),m_rainIndex(0),m_windowX(320),m_windowY(200),m_windowW(384),m_windowH(320),m_cameraX(0),m_cameraY(0),m_decay(0),m_orbRadius(0),m_cameraSway(false),m_released(false),m_ready(false),m_lastKey(0),m_seed(0),m_castScreen(0),m_castLine(0),m_castTarget(2),m_castAnim(0),m_castState(0),m_testMeasure(-1){memset(m_palette,0,48);}
COriginalEnd::~COriginalEnd(){if(m_background)m_background->Destroy();if(m_font)glDeleteTextures(1,&m_font);for(auto& entry:m_chineseText)entry.second->Destroy();}
void COriginalEnd::StartMusic(const char* name){auto* p=CGame::s_pCurGame->FindResource(name);CPMDPlayer::UnloadPMDData();if(p&&CCommonFunctionMusicSE::LoadMusicToPMDFromDat(p,const_cast<char*>(name))){CPMDPlayer::SetVolume(100);CPMDPlayer::Play();}}
bool COriginalEnd::Initialize(){
 auto* game=CGame::s_pCurGame;
 m_font=CPC98Font::LoadAuxiliaryAtlas("anex86.bmp");
 auto* p=game->FindResource("UDE.PI");
 if(m_mode==VerdictOnly){p=game->FindResource("UDE.PI");return m_ready=p&&CCommonFunctionGraphic::LoadPIFromDat(&m_background,m_palette,p,"UDE.PI");}
 if(m_mode==AllCast){LoadCastPicture();StartMusic("EXED.M2");return m_ready=m_background!=NULL;}
 p=game->FindResource("STF00.BFT");C2DImage* paletteImage=NULL;
 if(!p||!CCommonFunctionGraphic::LoadBFTFromDat(&paletteImage,m_palette,p,"STF00.BFT",0,0))return false;
 paletteImage->Destroy();m_palette[2]=48;
 // PC-98 has one hardware palette: both BFT banks use the final STF00 palette.
 p=game->FindResource("STF01.BFT");
 if(!p||!CCommonFunctionGraphic::LoadBFTAllPatternFromDat(&m_sprites,NULL,p,"STF01.BFT",0,m_palette))return false;
 p=game->FindResource("STF00.BFT");
 if(!p||!CCommonFunctionGraphic::LoadBFTAllPatternFromDat(&m_sprites,NULL,p,"STF00.BFT",0,m_palette))return false;
 for(int i=0;i<11;i++){char name[16];sprintf_s(name,"STF%02d.CDG",i);p=game->FindResource(name);if(!p||!CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_cards,p,name,m_palette))return false;}
 InitSpace();StartMusic("STAFF.M2");return m_ready=true;
}
int COriginalEnd::Step(){CGame::s_pCurGame->m_fpsLimit=56;CGame::s_pCurGame->SetVSYNC(false);if(!m_ready)return 1;Tick(CCommonFunctionInput::GetAllKeyState());if(m_fadeOut>0){const int duration=m_mode==Staff?100:m_mode==AllCast?64:32;if(++m_fadeOut>duration){if(m_mode==Staff){CPMDPlayer::Pause();CPMDPlayer::SetVolume(100);}return 1;}if(m_mode==Staff)CPMDPlayer::SetVolume(100.f*(duration-m_fadeOut)/duration);}return 0;}
void COriginalEnd::Tick(unsigned short keys){
 ++m_age;int music=CPMDPlayer::Measure();m_measure=m_testMeasure>=0?m_testMeasure:music<0?m_age/(m_mode==AllCast?44:22):music;
 if(!m_fadeOut){if(m_mode==Staff)StaffTick(keys);else if(m_mode==AllCast)CastTick();else{if(!keys)m_released=true;if(m_age>=128&&m_released&&keys&&!m_lastKey)m_fadeOut=1;}}
 m_lastKey=keys;
}
void COriginalEnd::Text(const char* text,int x,int y,int color,bool bold,bool translated){
 const float r=m_palette[color*3]/255.f,g=m_palette[color*3+1]/255.f,b=m_palette[color*3+2]/255.f;
 // The legacy Chinese mod changes the global code page to GBK. Original
 // MAINE strings remain CP932 and must keep their own PC-98 font atlas.
 if(m_font&&!translated){CPC98Font::DrawStringFromAtlas(m_font,const_cast<char*>(text),int(strlen(text)),x,y+40,r,g,b);if(bold)CPC98Font::DrawStringFromAtlas(m_font,const_cast<char*>(text),int(strlen(text)),x+1,y+40,r,g,b);return;}
 std::string value=text;
 if(!translated&&CGame::GVar().m_bUseSystemFont&&CGame::GVar().m_textCodePage!=932){
  const int n=MultiByteToWideChar(932,0,text,-1,NULL,0);std::vector<wchar_t> wide(n);MultiByteToWideChar(932,0,text,-1,wide.data(),n);
  const int bytes=WideCharToMultiByte(CGame::GVar().m_textCodePage,0,wide.data(),-1,NULL,0,NULL,NULL);value.resize(bytes);WideCharToMultiByte(CGame::GVar().m_textCodePage,0,wide.data(),-1,&value[0],bytes,NULL,NULL);
 }
 CPC98Font::DrawString(&value[0],int(value.size()),x,y+40,r,g,b);if(bold)CPC98Font::DrawString(&value[0],int(value.size()),x+1,y+40,r,g,b);
}
C2DImage* COriginalEnd::ChineseImage(const wchar_t* text)
{
 auto found=m_chineseText.find(text);if(found!=m_chineseText.end())return found->second;
 const int length=int(wcslen(text)),width=length*16;
 HDC dc=CreateCompatibleDC(NULL);if(!dc)return NULL;
 BITMAPINFO info={};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
 info.bmiHeader.biWidth=width;info.bmiHeader.biHeight=-16;
 info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;
 void* pixels=NULL;HBITMAP bitmap=CreateDIBSection(dc,&info,DIB_RGB_COLORS,&pixels,NULL,0);
 if(!bitmap){DeleteDC(dc);return NULL;}
 HGDIOBJ oldBitmap=SelectObject(dc,bitmap);
 HFONT font=CreateFontW(-16,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,GB2312_CHARSET,
  OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,FIXED_PITCH,L"SimSun");
 HGDIOBJ oldFont=font?SelectObject(dc,font):NULL;
 memset(pixels,0,width*16*4);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGB(255,255,255));
 TextOutW(dc,0,0,text,length);GdiFlush();
 C2DImage* image=C2DImage::Create2DImage(width,16);
 unsigned char* out=image->GetDataPointer();
 const unsigned char* in=static_cast<const unsigned char*>(pixels);
 for(int y=0;y<16;y++)for(int x=0;x<width;x++){
  const int dest=((15-y)*width+x)*4;out[dest]=out[dest+1]=out[dest+2]=255;out[dest+3]=in[(y*width+x)*4];
 }
 image->UploadToTexture();
 if(font){SelectObject(dc,oldFont);DeleteObject(font);}SelectObject(dc,oldBitmap);
 DeleteObject(bitmap);DeleteDC(dc);m_chineseText[text]=image;return image;
}
void COriginalEnd::Chinese(const wchar_t* text,int x,int y,int color){
 auto* image=ChineseImage(text);if(image)image->DrawColorScaled(float(x),float(y+40),m_palette[color*3]/255.f,m_palette[color*3+1]/255.f,m_palette[color*3+2]/255.f);
}
void COriginalEnd::Number(const char* text,int x,int y,int color){for(int i=0;text[i];i++){if(text[i]>='0'&&text[i]<='9')CTh5ExtFont::DrawExtChar(0xa0+text[i]-'0',x+i*16,y+40,m_palette[color*3]/255.f,m_palette[color*3+1]/255.f,m_palette[color*3+2]/255.f);}}
void COriginalEnd::Fraction(unsigned int value,int x,int y,int color,bool percent){
 // graph_fraction_of_million_put displays three digits and two decimals.
 const unsigned int unit=10000;char digits[16];sprintf_s(digits,"%3u",value/unit);Number(digits,x,y,color);sprintf_s(digits," %02u",(value%unit)/(unit/100));Number(digits,x+48,y,color);Text("\x81\x44",x+48,y,color);Chinese(percent?L"\uff05":L"\u5206",x+96,y,color);
}
void COriginalEnd::DrawVerdict(int x,int y,int color,bool comments){
 using namespace original;Chinese(VerdictLabels[0],x+120,y,color);const int offsets[]={24,48,72,96,120,144,168,192,216,240,272};for(int i=0;i<11;i++)Chinese(VerdictLabels[i+1],x,y+offsets[i],color);
 Chinese(VerdictDifficulties[m_stats.stage==6?4:m_stats.difficulty],x+224,y+24,color);
 char value[32];sprintf_s(value,"%9d",m_stats.finalScore);Number(value,x+128,y+48,color);Chinese(L"\u5206",x+272,y+48,color);
 sprintf_s(value,"%3u",m_stats.misses);Number(value,x+224,y+72,color);Chinese(L"\u6b21",x+272,y+72,color);
 sprintf_s(value,"%3u",m_stats.bombs);Number(value,x+224,y+96,color);Chinese(L"\u6b21",x+272,y+96,color);
 for(int i=0;i<6;i++)Fraction(m_verdict.fractions[i],x+176,y+120+i*24,color,i!=4);
 if(m_verdict.valid){Fraction(m_verdict.skill,x+176,y+272,color,false);if(comments){Chinese(VerdictComments[m_verdict.row],x+48,y+296,color);Chinese(VerdictComments[26+m_verdict.comment],x+48,y+312,color);}}
 else{Chinese(L"\uff1f\uff1f\uff1f\uff1f\uff1f\uff1f\u5206",x+176,y+272,color);Chinese(VerdictLabels[12],x+48,y+296,color);}
}
void COriginalEnd::DrawScores(int x,int y,int color){for(int i=0;i<7;i++){char value[32];const int offset=i==6?288:64+i*32;Chinese(i<6?original::VerdictStages[i]:original::VerdictLabels[2],x,y+offset,color);sprintf_s(value,"%9d",i==6?m_stats.finalScore:m_stats.stageScores[i]);Number(value,x+128,y+offset,color);Chinese(L"\u5206",x+272,y+offset,color);}}
int COriginalEnd::Random(){m_seed=m_seed*0x015a4e35u+1;return (m_seed>>16)&32767;}
void COriginalEnd::Velocity(Particle& p){p.vx=int(std::cos((p.angle&255)*Tau/256)*p.speed);p.vy=int(std::sin((p.angle&255)*Tau/256)*p.speed);}
void COriginalEnd::InitSpace(){
 for(auto& p:m_particles){p.active=true;p.x=(Random()%384-192)*16;p.y=(Random()%320-160)*16;p.angle=Random()&255;p.speed=10;Velocity(p);}
 for(auto& p:m_stars){p.active=true;p.x=(Random()%384-192)*16;p.y=(Random()%320-160)*16;}
}
void COriginalEnd::Gather(){for(auto& p:m_particles){p.angle=int(std::atan2(-double(p.y),-double(p.x))*256/Tau)&255;p.speed=int(std::hypot(double(p.x),double(p.y)))/32;Velocity(p);}m_orb.active=true;m_orbRadius=1;}
void COriginalEnd::Burst(){for(auto& p:m_particles){p.active=true;p.angle=Random()&127;p.speed=Random()%64+92;p.x=m_orb.x+int(std::cos(p.angle*Tau/256)*192);p.y=m_orb.y+int(std::sin(p.angle*Tau/256)*192);p.cel=0;Velocity(p);}m_orb.active=false;m_decay=1;}
bool COriginalEnd::CameraTick(){
 ++m_cameraAge;const int t=m_cameraAge;if(t<80)return false;
 if(t<176)++m_cameraY;else if(t<344)m_cameraY+=m_age&1;else if(m_age%16==0){m_cameraY+=m_cameraSway?-1:1;if(m_cameraY>182)m_cameraSway=true;if(m_cameraY<178)m_cameraSway=false;}
 if(t>=234&&t<512){int i=t-234;m_windowX=320-(std::max)(i-88,0);m_windowW=384-i/2;m_windowH=320+i/8;}
 if(t>=992&&t<=1024)--m_orb.vx;if(t>=1008&&t<=1040)--m_cameraX;return t>=512;
}
bool COriginalEnd::RainTick(){
 if(m_cameraX<0)m_cameraX+=m_age&1;const int age=++m_rainAge;
 if(age<32)return false;if(age<128)--m_cameraY;else if(age<308)m_cameraY-=m_age&1;
 else{if(m_windowX+m_windowW/2<630)m_windowX+=4;m_decay=0;
  if(age%8==0){if(m_rainIndex<63){auto& p=m_particles[m_rainIndex++];p.active=true;p.speed=Random()%24+8;p.angle=Random()%64+32;p.cel=Random()%6;p.x=(Random()%m_windowW-m_windowW/2)*16;p.y=-m_windowH*8-64;p.sway=p.angle>=64;Velocity(p);}
   for(auto& p:m_particles){p.angle+=p.sway?-1:1;if(p.angle>=96)p.sway=1;if(p.angle<=32)p.sway=0;Velocity(p);}}
  return m_windowX+m_windowW/2>=630;
 }return false;
}
void COriginalEnd::SpawnOrbParticle(){auto& p=m_particles[m_particleIndex];m_particleIndex=(m_particleIndex+1)%64;p.active=true;p.cel=0;p.angle=Random()&127;p.speed=48;p.x=m_orb.x+int(std::cos(p.angle*Tau/256)*192);p.y=m_orb.y+int(std::sin(p.angle*Tau/256)*192);Velocity(p);}
void COriginalEnd::SpaceTick(){
 for(auto& p:m_particles){if(!p.active)continue;p.x+=p.vx-m_cameraX;p.y+=p.vy-m_cameraY;
  if(!m_decay){if(p.x<=-m_windowW*8-64)p.x+=(m_windowW+8)*16;if(p.x>=m_windowW*8+64)p.x-=(m_windowW+8)*16;if(p.y<=-m_windowH*8-64)p.y+=(m_windowH+8)*16;if(p.y>=m_windowH*8+64)p.y-=(m_windowH+8)*16;}
  else{p.speed-=m_decay;if(p.speed<=0){p.active=false;continue;}if(p.speed>35)p.cel=0;else if(p.speed%7==0)++p.cel;Velocity(p);}}
 for(auto& t:m_trails){if(t.active){t.x-=m_cameraX;t.y-=m_cameraY;}}
 for(int i=0;i<48;i++){auto& s=m_stars[i];s.x-=m_cameraX/(i%2?2:1);s.y-=m_cameraY/(i%2?2:1);
  if(!m_cameraX&&(s.x<=-m_windowW*8||s.x>=m_windowW*8)){s.x=(Random()%m_windowW-m_windowW/2)*16;s.y=m_windowH*8;}else if(s.x<=-m_windowW*8)s.x=m_windowW*8;else if(s.x>=m_windowW*8)s.x=-m_windowW*8;
  if(s.y<=-m_windowH*8)s.y=m_windowH*8;else if(s.y>=m_windowH*8)s.y=-m_windowH*8;}
 if(m_orb.active){m_orb.x+=m_orb.vx-m_cameraX;m_orb.y+=m_orb.vy-m_cameraY;if(m_orbRadius>=16&&(m_age&1)){SpawnOrbParticle();if(m_orb.vy<180)++m_orb.vy;}}
 if(m_age%3==0){for(int i=7;i>0;i--)m_trails[i]=m_trails[i-1];m_trails[0]=m_orb;}
}
bool COriginalEnd::CreditTick(int slot,int target,int& age,int& out){
 ++age;const int reveal=(m_cards.GetImagePtr(slot)->m_width/16+8)*4;
 if(out<0&&age>=reveal&&m_measure>=target&&target!=3996)out=0;
 if(out>=0)return ++out>reveal+2;return false;
}
void COriginalEnd::StaffTick(unsigned short keys){
 ++m_phaseAge;
 switch(m_phase){
 case 0:if(m_measure>=30){Gather();m_phase=1;m_phaseAge=0;}break;
 case 1:for(auto& p:m_particles)if(m_phaseAge%8==0&&p.cel<5)++p.cel;if(m_orbRadius<16)m_orbRadius+=m_age&1;if(m_phaseAge>=32){for(auto& p:m_particles)p.active=false;m_orbRadius=16;m_decay=1;m_phase=2;m_phaseAge=0;}break;
 case 2:if(CameraTick()&&m_measure>=48){m_phase=3;m_phaseAge=0;}break;
 case 3:{CameraTick();if((m_card==0&&m_phaseAge>128)||(m_card==3&&m_phaseAge>256))CreditTick(m_card+1,CreditTargets[m_card+1],m_auxAge,m_auxOut);
  if(CreditTick(m_card,CreditTargets[m_card],m_cardAge,m_cardOut)){m_card=m_card==0?2:m_card==2?3:5;m_cardAge=m_auxAge=0;m_cardOut=m_auxOut=-1;m_phaseAge=0;if(m_card==5){Burst();m_phase=4;}}break;}
 case 4:if(RainTick()){m_phase=5;m_phaseAge=0;}break;
 case 5:RainTick();if(CreditTick(m_card,CreditTargets[m_card],m_cardAge,m_cardOut)){++m_card;m_cardAge=0;m_cardOut=-1;m_phaseAge=0;if(m_card==10)m_phase=6;}break;
 case 6:RainTick();CreditTick(10,3996,m_cardAge,m_cardOut);
  if(m_phaseAge>=400){if(!keys)m_released=true;const unsigned short pressed=keys&~m_lastKey;if(m_released){if(pressed&(16|4096))m_fadeOut=1;else if(!m_scrollDirection){if(pressed&(32|8192)){if(m_scroll==352)m_fadeOut=1;else m_scrollDirection=1;}else if((pressed&2)&&!m_scroll)m_scrollDirection=1;else if((pressed&1)&&m_scroll==352)m_scrollDirection=-1;}}if(m_scrollDirection){m_scroll+=m_scrollDirection*8;if(m_scroll>=352){m_scroll=352;m_scrollDirection=0;}if(m_scroll<=0){m_scroll=0;m_scrollDirection=0;}}}break;
 }
 SpaceTick();
}
void COriginalEnd::Circle(int x,int y,int radius,int color){for(int dy=-radius;dy<=radius;dy++){int dx=int(std::sqrt(double(radius*radius-dy*dy)));CCommonFunctionGraphic::DrawRectangle(float(x-dx),float(y+dy+40),float(x+dx+1),float(y+dy+41),m_palette[color*3]/255.f,m_palette[color*3+1]/255.f,m_palette[color*3+2]/255.f);}}
void COriginalEnd::DrawSpace(){
 const int left=m_windowX-m_windowW/2,top=m_windowY-m_windowH/2;
 CCommonFunctionGraphic::DrawRectangle(float(left),float(top+40),float(left+m_windowW),float(top+m_windowH+40),0,0,(std::min)(96,48+m_rainAge/4)/255.f);
 Clip(left,top+40,m_windowW,m_windowH);
 for(int i=0;i<48;i++){auto& p=m_stars[i];m_sprites.GetImagePtr(6+i%2)->Draw(float(m_windowX+p.x/16-4),float(m_windowY+p.y/16-4+40));}
 for(int i=7;i>=0;i--){auto& p=m_trails[i];if(p.active)Circle(m_windowX+p.x/16,m_windowY+p.y/16,16,13-i);}
 if(m_orb.active){if(m_orbRadius<16)Circle(m_windowX+m_orb.x/16,m_windowY+m_orb.y/16,m_orbRadius,15);else m_sprites.GetImagePtr(8+(m_age/4)%4)->Draw(float(m_windowX+m_orb.x/16-16),float(m_windowY+m_orb.y/16-16+40),1,1,false);}
 for(int i=63;i>=0;i--){auto& p=m_particles[i];if(p.active&&p.cel<6)m_sprites.GetImagePtr(p.cel)->Draw(float(m_windowX+p.x/16-4),float(m_windowY+p.y/16-4+40));}Unclip();
}
void COriginalEnd::DrawCard(int slot,int age,int out,int cx,int cy){
 if(age<=0)return;auto* img=m_cards.GetImagePtr(slot);const int x=cx-img->m_width/2,y=cy-img->m_height/2+40;
 img->Draw(float(x),float(y));
 // The original text RAM reveal uses these very same ZUN.COM gaiji.
 const int mask=out<0?img->m_width/16+7-age/4:out/4;
 for(int bx=0;bx<=img->m_width/16;bx++){
  const int level=mask-(out<0?bx:img->m_width/16-1-bx);if(level<0)continue;
  const unsigned char glyph=level>=7?152:159-level;
  for(int by=0;by<=img->m_height/16;by++)CTh5ExtFont::DrawExtChar(glyph,x+bx*16,((y-40)/16+by)*16+40,0,0,0);
 }
}
void COriginalEnd::DrawStaff(){
 CCommonFunctionGraphic::DrawRectangle(0,40,640,440,m_palette[3]/255.f,m_palette[4]/255.f,m_palette[5]/255.f);DrawSpace();
 if(m_phase==3){if(m_card==0){DrawCard(0,m_cardAge,m_cardOut,464,192);DrawCard(1,m_auxAge,m_auxOut,528,240);}else if(m_card==2)DrawCard(2,m_cardAge,m_cardOut,464,200);else{DrawCard(3,m_cardAge,m_cardOut,464,176);DrawCard(4,m_auxAge,m_auxOut,464,224);}}
 if(m_phase==5)DrawCard(m_card,m_cardAge,m_cardOut,176,200);
 if(m_phase==6){DrawCard(10,m_cardAge,m_cardOut,176,368);if(m_phaseAge>=256){Clip(0,40,320,352);DrawVerdict(32,16-m_scroll,13,m_phaseAge>=320);if(m_scroll)DrawScores(32,16+352-m_scroll,13);Unclip();}}
}
void COriginalEnd::LoadCastPicture(){if(m_background){m_background->Destroy();m_background=NULL;}char name[16];sprintf_s(name,"EXED%02d.PI",CastFiles[CGame::GVar().m_playChara][m_castScreen]);auto* p=CGame::s_pCurGame->FindResource(name);if(p)CCommonFunctionGraphic::LoadPIFromDat(&m_background,m_palette,p,name);}
void COriginalEnd::CastTick(){
 if(m_age%2)return;++m_castAnim;
 switch(m_castState){
 case 0:if(m_measure>=m_castTarget){m_castState=1;m_castAnim=0;}break;
 case 1:if(m_castAnim>=32){if(m_castLine>=CastCount[m_castScreen]){++m_castScreen;m_castLine=0;LoadCastPicture();}m_castState=2;m_castAnim=0;}break;
 case 2:if(m_castAnim>=9){m_castTarget+=2;m_castState=3;m_castAnim=0;}break;
 case 3:if(m_measure>=m_castTarget){m_castState=4;m_castAnim=0;}break;
 case 4:if(m_castAnim>=10){++m_castLine;m_castAnim=0;if(m_castLine>=CastCount[m_castScreen]){m_castState=5;m_castTarget+=30;}else{m_castTarget+=2;m_castState=3;}}break;
 case 5:if(m_measure>=m_castTarget){if(m_castScreen==6){m_castState=6;m_castTarget+=16;}else{m_castState=1;m_castAnim=0;}}break;
 case 6:if(m_measure>=m_castTarget)m_fadeOut=1;break;
 }
}
void COriginalEnd::DrawCast(){
 if(m_castState==0||(m_castState==1&&m_castScreen==0&&m_castLine==0))return;
 // Original PI_MASKS and graph_putsa_fx masks, not opacity approximations.
 static const unsigned short piMasks[2][4]={{0,0x1111,0,0x4444},{0x8888,0x1111,0x2222,0x4444}};
 static const unsigned short textMasks[4][4]={{0x8888,0,0x2222,0},{0x8888,0x4444,0x2222,0x1111},{0xaaaa,0x4444,0xaaaa,0x1111},{0xaaaa,0x5555,0xaaaa,0x5555}};
 const int q=CastQuarters[CGame::GVar().m_playChara][m_castScreen];Clip(160,140,320,200);
 if(m_castState==2&&m_castAnim<8)Stipple(piMasks[m_castAnim/4],160,140);
 if(m_background)m_background->Draw(float(160-(q%2)*320),float(140-(q/2)*200));
 glDisable(GL_POLYGON_STIPPLE);Unclip();
 int start=0;for(int i=0;i<m_castScreen;i++)start+=CastCount[i];
 const int count=m_castLine+(m_castState==4?1:0);
 for(int i=0;i<count&&i<CastCount[m_castScreen];i++){
  const int y=192-(CastCount[m_castScreen]-1)*16+i*32;
  if(i==m_castLine&&m_castState==4&&m_castAnim<8)Stipple(textMasks[3-m_castAnim/2],64,y+40);
  Text(original::CastLines[start+i],64,y,15);glDisable(GL_POLYGON_STIPPLE);
 }
 if(m_castState==1)CCommonFunctionGraphic::ScreenFade(float((32-m_castAnim)*100/32));
}
void COriginalEnd::Draw(){glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT);if(!m_ready)return;if(m_mode==Staff)DrawStaff();else if(m_mode==AllCast)DrawCast();else{m_background->Draw(0,40);if(m_age>=64)DrawVerdict(336,48,2,m_age>=128);CCommonFunctionGraphic::ScreenFade(float((std::min)(100,m_age*100/64)));}
 if(m_mode==Staff&&m_age<100)CCommonFunctionGraphic::ScreenFade(float(m_age));if(m_fadeOut){const int duration=m_mode==Staff?100:m_mode==AllCast?64:32;CCommonFunctionGraphic::ScreenFade(100.f*(duration-m_fadeOut)/duration);}
}
}
