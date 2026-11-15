#pragma once
namespace th19 {
// 0x50be30, after the enemy's first ECL update. This is the INITIAL
// main animation; later directional frames must not change its death color.
inline int nativeEnemyDeathScript(int file,int script){
 if(file!=2)return 37;
 switch(script){
 case 5:case 25:case 53:case 91:return 33;
 case 10:case 56:case 96:return 41;
 case 15:case 106:return 45;
 case 30:return 51;
 case 35:return 50;
 case 40:return 49;
 default:return 37;
 }
}
}
