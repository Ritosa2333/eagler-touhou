#pragma once
#include "Interpolation.hpp"
#include "PolygonContact.hpp"
#include <array>
namespace th19 {
// Circular bomb paths verified in 53c7d0/53c510,417f10/418190,
// 41b5c0/541b10 and4244f0/424690. Other shapes have separate native paths.
struct PlayerBomb {
 int character=-1,frame=-1,duration=0,initial_energy=0;
 int area_script=0,back_script=0;
 inline static constexpr int areas[19]={16,18,17,13,12,11,12,14,9,11,14,15,16,12,11,12,11,13,12};
 inline static constexpr int backs[19]={19,23,20,16,15,14,15,17,13,14,17,18,19,15,14,15,14,16,15};
 inline static constexpr int minimums[19]={120,180,120,120,120,200,120,220,200,180,140,160,90,240,180,120,180,260,120};
 inline static constexpr int coefficients[19]={250,350,320,300,300,370,300,320,370,400,280,280,220,480,330,340,320,400,180};
 struct Point{float x=0,y=0;};std::array<Point,3> centers{};int count=1;
 float angle=0,last_px=0,tilt=0,speed=1;
 float x=0,y=0,back_x=0,back_y=0,radius=0,width=0,target_radius=0,target_width=0;
 static bool supported(int ch){return ch>=0&&ch<19;}
 bool active()const{return frame>=0&&frame<duration;}
 bool occupied()const{return frame>=0&&frame<duration+10;}
 // 4ed580 initializes integer interpolation mode0 from current gauge to zero.
 // 403250 truncates the interpolated float; the ten-frame exit keeps zero.
 int gauge_energy()const{
  if(duration<=0)return 0;
  return std::max(0,int(initial_energy*(1.f-std::clamp(float(frame)/duration,0.f,1.f))));
 }
 bool start(int ch,int energy,int&stock,float px,float py,float initial_dx=0){
  if(!supported(ch)||occupied()||stock<=0)return false;
  *this={};character=ch;frame=0;initial_energy=std::clamp(energy,0,2500);
  x=back_x=px;y=back_y=py;
  if(ch==1)y=back_y=0; // 53dc50 explicitly clears BombMarisaInf+34.
  last_px=px;
  duration=std::max(minimums[ch],coefficients[ch]*initial_energy/2500);
  area_script=areas[ch];back_script=backs[ch];
  target_radius=ch==12?400+48*initial_energy/2500.f:
      (ch==5||ch==6)?80+80*initial_energy/4000.f:128+224*initial_energy/4000.f;
  target_width=ch==12?30:16+((ch==5||ch==6)?16:32)*initial_energy/4000.f;
  if(ch==1){target_radius=16+64*initial_energy/4000.f;target_width=1;}
  if(ch==3){target_radius=32+64*initial_energy/2500.f;target_width=target_radius;}
  if(ch==8){target_radius=64+40*initial_energy/2500.f;target_width=target_radius;angle=-1.57079632679f;}
  if(ch==10){target_radius=160+16*initial_energy/2500.f;target_width=target_radius;angle=-1.57079632679f;}
  const float e=initial_energy/2500.f;
  if(ch==4){target_radius=128+100*initial_energy/4000.f;target_width=128+400*initial_energy/4000.f;angle=-1.570796327f;}
  if(ch==7){target_radius=64+64*e;target_width=target_radius;angle=-1.570796327f+(initial_dx<-.1f?-.104719755f:initial_dx>.1f?.104719755f:0);} // 41bf70: +/-6 degrees from player velocity.
  if(ch==9){target_radius=48+32*e;target_width=30;count=3;}
  if(ch==11){target_radius=144+48*e;target_width=48+16*e;}
  if(ch==13){target_radius=96+48*e;target_width=30;count=3;}
  if(ch==14){target_radius=320+48*e;target_width=30;}
  if(ch==15){target_radius=128+48*e;target_width=30;count=3;}
  if(ch==16){target_radius=144+48*e;target_width=30;}
  if(ch==17){target_radius=192+256*e;target_width=30;}
  if(ch==18){target_radius=192+256*e;target_width=96+128*e;angle=-1.570796327f;}
  radius=ch>=9?24:1;width=ch>=9?30:1;
  if(ch==10)width=24;
  if(ch==11||ch==18)width=24;
  for(auto&c:centers)c={x,y};--stock;return true;
 }
 // 540aa0: target is selected by 508420; no target falls back to the player.
 void seek(float tx,float ty){if(character!=5||!active())return;
  const float dx=tx-x,dy=ty-y,d=std::hypot(dx,dy),s=d>3?3/d:1;
  x+=dx*s;y+=dy*s;
 }
 void advance(float px,float py){
  if(frame<0)return;
  if(active()){
   const int ch=character;const float dx=px-last_px;last_px=px;
   if(ch==11||ch==15||ch==16||ch==17||ch==18)tilt+=( (dx<-.1f?-.122173048f:dx>.1f?.122173048f:0)-tilt)*.1f;
   if(ch==0||ch==4||ch==12||ch>=13){x=px;y=py;}
   if(ch==11){x=px+64*std::sin(tilt);y=py-64*std::cos(tilt);}
   if(ch==15){x=px+128*std::sin(tilt);y=py-128*std::cos(tilt);}
   if(ch==17){x=px-96*std::sin(tilt);y=py+96*std::cos(tilt);}
   if(ch==11||ch==15||ch==16||ch==17)angle=tilt;
   if(ch==18)angle=tilt-1.570796327f;
   if(ch==14)angle+=.01745329252f;
   if(ch!=1&&ch!=2&&ch!=3){back_x=px;back_y=py;}
   if(ch==7){x+=std::cos(angle)*speed;y+=std::sin(angle)*speed;speed=frame<60?speed+.05f:speed>0?speed-.035f:speed;}
   if(ch==8){y-=1;angle+=.0872664626f;}
   if(ch==9||ch==10)y-=.1f;
   const float start=ch>=9?24.f:1.f;
   const float t=nativeEase(ch==4?1:4,float(frame+1)/((ch==7||ch==8)?20:duration));
   radius=start+(target_radius-start)*t;
   width=(ch==9||ch==12||ch>=13&&ch<=17)?30:start+(target_width-start)*t;
   centers[0]={x,y};
   if(ch==9){centers[0]={x,y-96};centers[1]={x-64,y-64};centers[2]={x+64,y-64};}
   if(ch==15){const float c=std::cos(tilt),s=std::sin(tilt);centers[1]={px+96*c+40*s,py+96*s-40*c};centers[2]={px-96*c+40*s,py-96*s-40*c};}
  }
  ++frame;
 }
};
}
