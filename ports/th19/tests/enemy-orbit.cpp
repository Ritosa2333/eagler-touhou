#include "EnemyOrbit.hpp"
#include <cassert>
#include <iostream>
int main(){
{auto p=th19::enemyEllipseOffset(0.0f,-1.1780972450961724f,8.0f,0.0f); assert(std::abs(p[0]-6.828426837921143f)<0.0001f);assert(std::abs(p[1]-2.8284270763397217f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(0.08726646f,-1.1780972450961724f,8.0f,0.0f); assert(std::abs(p[0]-7.048956394195557f)<0.0001f);assert(std::abs(p[1]-2.9197733402252197f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(0.6f,-1.1780972450961724f,8.0f,0.0f); assert(std::abs(p[0]-7.232793807983398f)<0.0001f);assert(std::abs(p[1]-2.9959213733673096f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(1.5f,-1.1780972450961724f,8.0f,0.0f); assert(std::abs(p[0]-3.304365634918213f)<0.0001f);assert(std::abs(p[1]-1.3687130212783813f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(3.0f,-1.1780972450961724f,8.0f,0.0f); assert(std::abs(p[0]- -6.36094331741333f)<0.0001f);assert(std::abs(p[1]- -2.634788990020752f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(-2.0f,-1.1780972450961724f,8.0f,0.0f); assert(std::abs(p[0]- -5.413509845733643f)<0.0001f);assert(std::abs(p[1]- -2.242349147796631f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(0.0f,1.5707963267948966f,32.0f,0.0f); assert(std::abs(p[0]-32.0f)<0.0001f);assert(std::abs(p[1]-1.3987644251756137e-06f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(0.08726646f,1.5707963267948966f,32.0f,0.0f); assert(std::abs(p[0]-31.878231048583984f)<0.0001f);assert(std::abs(p[1]-1.3934417211203254e-06f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(0.6f,1.5707963267948966f,32.0f,0.0f); assert(std::abs(p[0]-26.41073989868164f)<0.0001f);assert(std::abs(p[1]-1.154450160356646e-06f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(1.5f,1.5707963267948966f,32.0f,0.0f); assert(std::abs(p[0]-2.263591766357422f)<0.0001f);assert(std::abs(p[1]-9.894473862459563e-08f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(3.0f,1.5707963267948966f,32.0f,0.0f); assert(std::abs(p[0]- -31.679759979248047f)<0.0001f);assert(std::abs(p[1]- -1.3847662785337889e-06f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(-2.0f,1.5707963267948966f,32.0f,0.0f); assert(std::abs(p[0]- -13.31669807434082f)<0.0001f);assert(std::abs(p[1]- -5.820913884235779e-07f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(0.0f,0.0f,32.0f,0.0f); assert(std::abs(p[0]-0.0f)<0.0001f);assert(std::abs(p[1]-0.0f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(0.08726646f,0.0f,32.0f,0.0f); assert(std::abs(p[0]-0.0f)<0.0001f);assert(std::abs(p[1]-2.7889835834503174f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(0.6f,0.0f,32.0f,0.0f); assert(std::abs(p[0]-0.0f)<0.0001f);assert(std::abs(p[1]-18.068559646606445f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(1.5f,0.0f,32.0f,0.0f); assert(std::abs(p[0]-0.0f)<0.0001f);assert(std::abs(p[1]-31.91983985900879f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(3.0f,0.0f,32.0f,0.0f); assert(std::abs(p[0]-0.0f)<0.0001f);assert(std::abs(p[1]-4.51584005355835f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(-2.0f,0.0f,32.0f,0.0f); assert(std::abs(p[0]-0.0f)<0.0001f);assert(std::abs(p[1]- -29.097517013549805f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(0.0f,0.6f,50.0f,0.5f); assert(std::abs(p[0]-32.97052764892578f)<0.0001f);assert(std::abs(p[1]- -11.65048885345459f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(0.08726646f,0.6f,50.0f,0.5f); assert(std::abs(p[0]-31.82965850830078f)<0.0001f);assert(std::abs(p[1]- -7.943046569824219f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(0.6f,0.6f,50.0f,0.5f); assert(std::abs(p[0]-20.633390426635742f)<0.0001f);assert(std::abs(p[1]-14.11606216430664f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(1.5f,0.6f,50.0f,0.5f); assert(std::abs(p[0]- -9.289060592651367f)<0.0001f);assert(std::abs(p[1]-41.10006332397461f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(3.0f,0.6f,50.0f,0.5f); assert(std::abs(p[0]- -34.28469467163086f)<0.0001f);assert(std::abs(p[1]-17.465091705322266f)<0.0001f);}
{auto p=th19::enemyEllipseOffset(-2.0f,0.6f,50.0f,0.5f); assert(std::abs(p[0]- -3.1268186569213867f)<0.0001f);assert(std::abs(p[1]- -33.368980407714844f)<0.0001f);}
std::cout<<"24 original EXE ellipse position vectors PASS\n";}

