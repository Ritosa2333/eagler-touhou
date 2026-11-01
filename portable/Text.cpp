#include "Platform.hpp"
#include "Localization.hpp"
#include "Bitmap.hpp"
#include "../Game/Game.h"
#include "../gameLib/Graphic/PC98Font.h"
#include "../gameLib/Graphic/2DImage.h"
#include "../gameLib/CommonFunction/CommonFunctionGraphic.h"
#include <map>
#include <string>
EM_JS(void,th05_raster_text,(const void* ptr,int size,int codepage,int out,int width),{
 let text;
 if(codepage===12000){text='';for(let n=0;n<size;n++)text+=String.fromCodePoint(HEAPU32[(ptr>>2)+n]);}
 else text=new TextDecoder(codepage===932?'shift_jis':codepage===936?'gbk':'utf-8').decode(HEAPU8.subarray(ptr,ptr+size));
 const canvas=new OffscreenCanvas(width,16),ctx=canvas.getContext('2d',{willReadFrequently:true});ctx.font='16px TH05GameFont, monospace';ctx.textBaseline='top';ctx.fillStyle='white';let x=0;
 for(const glyph of text){const cells=glyph.codePointAt(0)<128?8:16;ctx.fillText(glyph,x,0,cells);x+=cells;}
 const data=ctx.getImageData(0,0,width,16).data;
 for(let y=0;y<16;y++)HEAPU8.set(data.subarray(y*width*4,(y+1)*width*4),out+(15-y)*width*4);
});
namespace th5w {
bool CPC98Font::s_bFontLoaded=false;GLuint CPC98Font::s_fontTex=0;
bool CPC98Font::s_bCacheInited=false;int CPC98Font::s_curCacheWriteIdx=0;int CPC98Font::s_charIdxInCache[65536]{};int CPC98Font::s_cacheList[200]{};GLuint CPC98Font::s_displayListBase=0;GLuint CPC98Font::s_CommonCharTex=0;
CPC98Font::CPC98Font(){}CPC98Font::~CPC98Font(){}
namespace {std::map<std::string,C2DImage*> textCache;
void Quad(GLuint texture,int width,int height,float x,float y,float sx,float sy,float tw,float th,float r,float g,float b,float alpha,float scale=1){
 glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();gluOrtho2D(0,640,0,480);glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,texture);glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);glDisable(GL_DEPTH_TEST);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);glColor4f(r,g,b,alpha);glBegin(GL_QUADS);glTexCoord2f(sx,sy);glVertex2f(x,y);glTexCoord2f(sx+tw,sy);glVertex2f(x+width*scale,y);glTexCoord2f(sx+tw,sy+th);glVertex2f(x+width*scale,y+height*scale);glTexCoord2f(sx,sy+th);glVertex2f(x,y+height*scale);glEnd();glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();
}
}
bool CPC98Font::CreateTextureForCommonChar(){return true;}
void CPC98Font::DestroyTextureForCommonChar(){}
GLuint CPC98Font::GetDisplayListForChar(int){return 0;}
void CPC98Font::FinalizeCache(){for(auto& pair:textCache)pair.second->Destroy();textCache.clear();}
int CPC98Font::ConvertCharToTexXY(float* x,float* y,unsigned char* str){
 unsigned a=str[0],b=str[1];if(a<128){*x=8*a/2048.f;*y=2032/2048.f;return 1;}
 if(a>=0xa1&&a<=0xdf){*x=8*(a-0xa0)/2048.f;*y=2016/2048.f;return 1;}
 if(!((a>=0x81&&a<=0x9f)||(a>=0xe0&&a<=0xfc))||b<0x40||b>0xfc||b==0x7f)return 0;
 if(b>0x7f)b--;if(a>=0xe0)a-=0x40;int cx=16+(a-0x81)*32;if(b>=0x9e){cx+=16;b-=0x5e;}*x=cx/2048.f;*y=(2032-(528+(b-0x40)*16))/2048.f;return 2;
}
GLuint CPC98Font::LoadAuxiliaryAtlas(const char* path,bool){auto bitmap=web::ReadBitmap(path);if(bitmap.width!=2048||bitmap.height!=2048)return 0;GLuint id;glGenTextures(1,&id);glBindTexture(GL_TEXTURE_2D,id);for(size_t i=0;i<bitmap.rgba.size();i+=4){unsigned char ink=255-bitmap.rgba[i];bitmap.rgba[i]=bitmap.rgba[i+1]=bitmap.rgba[i+2]=255;bitmap.rgba[i+3]=ink;}glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,2048,2048,0,GL_RGBA,GL_UNSIGNED_BYTE,bitmap.rgba.data());glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);return id;}
bool CPC98Font::LoadAnex86BMP(const char* path,bool chinese){s_fontTex=LoadAuxiliaryAtlas(path,chinese);return s_bFontLoaded=s_fontTex!=0;}
bool CPC98Font::DrawStringFromAtlas(GLuint atlas,char* text,int count,int x,int y,float r,float g,float b,float alpha){if(!atlas)return DrawString(text,count,x,y,r,g,b,alpha);for(int n=0;n<count&&*text;n++){float sx,sy;int len=ConvertCharToTexXY(&sx,&sy,reinterpret_cast<unsigned char*>(text));if(!len||len==2&&!text[1])break;Quad(atlas,len*8,16,x,480-y-16,sx,sy,len*8/2048.f,16/2048.f,r,g,b,alpha);x+=len*8;text+=len;}return true;}
bool CPC98Font::DrawString(char* text,int count,int x,int y,float r,float g,float b,float alpha,bool upper,float scale){
 if(!text)return false;std::string bytes;for(int n=0;n<count&&*text;n++){unsigned char c=*text++;bytes.push_back(c);if((c>=0x81&&c<=0x9f||c>=0xe0&&c<=0xfc)&&*text)bytes.push_back(*text++);}
 if(bytes.empty())return true;
 if(!CGame::GVar().m_bUseSystemFont&&s_bFontLoaded&&scale==1&&upper)return DrawStringFromAtlas(s_fontTex,bytes.data(),count,x,y,r,g,b,alpha);
 std::string key=std::to_string(CGame::GVar().m_textCodePage)+":"+bytes;auto it=textCache.find(key);C2DImage* image;
 if(it==textCache.end()){if(textCache.size()>256)FinalizeCache();image=C2DImage::Create2DImage(bytes.size()*8,16);th05_raster_text(bytes.data(),bytes.size(),CGame::GVar().m_textCodePage,reinterpret_cast<intptr_t>(image->GetDataPointer()),image->m_width);image->UploadToTexture();textCache[key]=image;}else image=it->second;
 GLint w,h;glBindTexture(GL_TEXTURE_2D,image->GetTexID());glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&w);glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&h);Quad(image->GetTexID(),image->m_width,16,x,upper?480-y-16*scale:y,0,0,float(image->m_width)/w,16.f/h,r,g,b,alpha,scale);return true;
}
bool CCommonFunctionGraphic::LoadBinaryImageFile(C2DImage** result,unsigned char transparent[3],const char* path){auto bitmap=web::ReadBitmap(path);if(!bitmap.width)return false;if(transparent)for(size_t i=0;i<bitmap.rgba.size();i+=4)if(!memcmp(bitmap.rgba.data()+i,transparent,3))bitmap.rgba[i+3]=0;auto* image=C2DImage::Create2DImage(bitmap.width,bitmap.height);memcpy(image->GetDataPointer(),bitmap.rgba.data(),bitmap.rgba.size());image->UploadToTexture();*result=image;return true;}
namespace web {
C2DImage* WideText(const wchar_t* text){if(!text||!*text)return nullptr;auto translated=locale::GameText(text);int size=translated.size(),width=0;for(auto c:translated)width+=c<128?8:16;auto* image=C2DImage::Create2DImage(width,16);th05_raster_text(translated.c_str(),size,12000,reinterpret_cast<intptr_t>(image->GetDataPointer()),image->m_width);image->UploadToTexture();return image;}
}
}
