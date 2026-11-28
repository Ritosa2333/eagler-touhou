#pragma once
#include <cmath>
namespace th19 {
// 0x4f1340 loops row(speed) outside column(angle); 0x4f3910 modes 0..10.
// Modes 6/8 use signed random spread; 7/8 add unsigned random speed2 to speed.
struct BulletPatternPoint {float angle=0,speed=0,launch_angle=0;};
// 0x404a70 and 0x4f3bd7 round each complete turn in single precision.
// The executable stops after 34 turns even for an unbounded ECL angle.
inline float nativeShooterAngle(float angle){
 constexpr float pi=3.1415927410125732f;
 for(int turn=0;turn<34;++turn){
  if(angle>pi)angle-=2*pi;
  else if(angle< -pi)angle+=2*pi;
  else break;
 }
 return angle;
}
inline bool nativeBulletPattern(int mode,int count,int rows,int column,int row,
 float angle,float spread,float speed,float speed2,float aim,BulletPatternPoint& out,
 float signedRandom=0,float unitRandom=0){
 if(mode<0||mode>10||count<=0||rows<=0||column<0||column>=count||row<0||row>=rows)return false;
 constexpr float pi=3.1415927410125732f;
 out.speed=rows>1?speed-(speed-speed2)*row/(rows-1):speed;
 if(mode<=1){float delta=count%2?((column+1)/2)*spread:(column/2+.5f)*spread;
  if(column&1)delta=-delta;out.angle=angle+delta+(mode==0?aim:0);
 }else if(mode>=6&&mode<=8){
  out.angle=mode==7?row*spread+angle+2*pi*column/count:angle+signedRandom*spread;
  if(mode==7||mode==8)out.speed=speed+unitRandom*speed2;
 }else if(mode==9||mode==10){
  float delta=2*pi*column/count;
  if(rows%2){delta+=((row+1)/2)*spread;if(rows>1)out.speed=((row+1)&~1)*(speed2-speed)/(rows-1)+speed;}
  else{delta+=(row/2+.5f)*spread;if(rows>1)out.speed=(row&~1)*(speed2-speed)/(rows-1)+speed;}
  if(row&1)delta=-delta;out.angle=angle+delta+(mode==9?aim:0);
 }else{out.angle=row*spread+angle+2*pi*column/count+(mode>=4?pi/count:0)+(mode==2||mode==4?aim:0);}
 out.launch_angle=out.angle;
 // 0x4f407c repeats the bounded reduction before placing the muzzle.
 // The launch velocity still uses the angle saved at 0x4f3ba6.
 out.angle=nativeShooterAngle(nativeShooterAngle(out.angle));
 return true;
}
}
