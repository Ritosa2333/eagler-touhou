#pragma once
namespace th5w { namespace practice {
struct Section {int phase,variant,startHP,endHP,rageCount,continuous;const char* name;};
inline const Section* Sections(int stage,int warp,int& count){
 static const Section b0[]={
  {2,0,4650,2550,0,0,u8"\u9636\u6bb5 1"},
  {3,0,2550,450,0,0,u8"\u9636\u6bb5 2"},
  {4,0,450,0,0,0,u8"\u9636\u6bb5 3"},
 };
 static const Section b1[]={
  {2,0,4400,3000,0,0,u8"\u9636\u6bb5 1"},
  {4,0,3000,1900,0,0,u8"\u9636\u6bb5 2"},
  {5,0,1900,500,0,0,u8"\u9636\u6bb5 3"},
  {7,0,500,0,0,0,u8"\u9636\u6bb5 4"},
 };
 static const Section b2[]={
  {2,0,9600,7400,0,0,u8"\u4eba\u5076\u98de\u5165"},
  {3,0,9600,7400,0,0,u8"\u9636\u6bb5 1"},
  {4,0,7400,5200,0,0,u8"\u4eba\u5076\u5c55\u5f00 1"},
  {5,0,7400,5200,0,0,u8"\u62a4\u76fe 1"},
  {6,0,7400,5200,0,0,u8"\u9636\u6bb5 2"},
  {7,0,5200,3000,0,0,u8"\u4eba\u5076\u5c55\u5f00 2"},
  {8,0,5200,3000,0,0,u8"\u62a4\u76fe 2"},
  {9,0,5200,3000,0,0,u8"\u9636\u6bb5 3"},
  {10,0,3000,800,0,0,u8"\u4eba\u5076\u5c55\u5f00 3"},
  {11,0,3000,800,0,0,u8"\u62a4\u76fe 3"},
  {12,0,3000,800,0,0,u8"\u9636\u6bb5 4"},
  {13,0,800,0,0,0,u8"\u6700\u7ec8\u653b\u51fb\u51c6\u5907"},
  {14,0,800,0,0,0,u8"\u9636\u6bb5 5"},
 };
 static const Section b3[]={
  {2,0,4500,0,2,0,u8"\u53cc Boss"},
  {3,1,7800,5800,0,0,u8"\u821e - \u9636\u6bb5 1"},
  {5,1,5800,2800,0,0,u8"\u821e - \u9636\u6bb5 2"},
  {7,1,2800,1200,0,0,u8"\u821e - \u9636\u6bb5 3"},
  {9,1,1200,0,0,0,u8"\u821e - \u9636\u6bb5 4"},
  {3,2,7900,4600,0,0,u8"\u96ea - \u9636\u6bb5 1"},
  {5,2,4600,3200,0,0,u8"\u96ea - \u9636\u6bb5 2"},
  {7,2,3200,1200,0,0,u8"\u96ea - \u9636\u6bb5 3"},
  {9,2,1200,0,0,0,u8"\u96ea - \u9636\u6bb5 4"},
 };
 static const Section b4[]={
  {2,0,8300,7500,0,0,u8"\u9636\u6bb5 1"},
  {4,0,7500,5700,1,0,u8"\u9636\u6bb5 2"},
  {5,0,5700,4500,0,0,u8"\u9636\u6bb5 3"},
  {7,0,4500,2700,1,0,u8"\u9636\u6bb5 4"},
  {8,0,2700,1200,0,0,u8"\u9636\u6bb5 5"},
  {10,0,1200,0,0,0,u8"\u9636\u6bb5 6"},
 };
 static const Section b5[]={
  {2,0,22800,20600,0,0,u8"\u9636\u6bb5 1"},
  {3,0,20600,18400,0,0,u8"\u9636\u6bb5 2"},
  {6,0,18400,14600,0,0,u8"\u9636\u6bb5 3"},
  {7,0,14600,11600,0,0,u8"\u9636\u6bb5 4"},
  {9,0,11600,8600,0,0,u8"\u9636\u6bb5 5"},
  {10,0,8600,2800,2,0,u8"\u9636\u6bb5 6"},
  {12,0,2800,0,0,0,u8"\u9636\u6bb5 7"},
 };
 static const Section b6[]={
  {2,0,26500,23800,0,0,u8"\u9636\u6bb5 1"},
  {4,0,23800,21000,0,0,u8"\u9636\u6bb5 2"},
  {6,0,21000,18100,0,0,u8"\u9636\u6bb5 3"},
  {8,0,18100,15100,0,0,u8"\u9636\u6bb5 4"},
  {10,0,15100,12600,0,0,u8"\u9636\u6bb5 5"},
  {12,0,12600,9600,0,0,u8"\u9636\u6bb5 6"},
  {14,0,9600,6800,0,0,u8"\u9636\u6bb5 7"},
  {16,0,6800,3400,0,0,u8"\u9636\u6bb5 8"},
  {17,0,3400,0,0,1,u8"\u9636\u6bb5 9"},
 };
 static const Section m0[]={
  {3,0,1000,0,1,1,u8"\u9636\u6bb5 1"},
 };
 static const Section m1[]={
  {1,0,1400,400,2,0,u8"\u9636\u6bb5 1"},
  {2,0,400,0,0,0,u8"\u9636\u6bb5 2"},
 };
 static const Section m2[]={
  {1,0,1400,0,0,0,u8"\u9636\u6bb5 1"},
 };
 static const Section m3[]={
  {1,0,1100,0,0,0,u8"\u9636\u6bb5 1"},
 };
 static const Section m4[]={
  {1,0,1550,0,0,0,u8"\u9636\u6bb5 1"},
 };
 static const Section m6[]={
  {1,0,3000,0,1,0,u8"\u9636\u6bb5 1"},
 };
 count=0;const Section* result=0;
 if(warp==1){switch(stage){
 case 0:result=m0;count=sizeof(m0)/sizeof(*m0);break;
 case 1:result=m1;count=sizeof(m1)/sizeof(*m1);break;
 case 2:result=m2;count=sizeof(m2)/sizeof(*m2);break;
 case 3:result=m3;count=sizeof(m3)/sizeof(*m3);break;
 case 4:result=m4;count=sizeof(m4)/sizeof(*m4);break;
 case 6:result=m6;count=sizeof(m6)/sizeof(*m6);break;
 }}else if(warp==2){switch(stage){
 case 0:result=b0;count=sizeof(b0)/sizeof(*b0);break;
 case 1:result=b1;count=sizeof(b1)/sizeof(*b1);break;
 case 2:result=b2;count=sizeof(b2)/sizeof(*b2);break;
 case 3:result=b3;count=sizeof(b3)/sizeof(*b3);break;
 case 4:result=b4;count=sizeof(b4)/sizeof(*b4);break;
 case 5:result=b5;count=sizeof(b5)/sizeof(*b5);break;
 case 6:result=b6;count=sizeof(b6)/sizeof(*b6);break;
 }}return result;
}
inline const Section* SelectedSection(int stage,int warp,int section){int n=0;const Section* list=Sections(stage,warp,n);return section>0&&section<=n?list+section-1:0;}
inline const char* SectionGlyphs(){return u8"\u9636\u6bb5 1\u9636\u6bb5 2\u9636\u6bb5 3\u9636\u6bb5 1\u9636\u6bb5 2\u9636\u6bb5 3\u9636\u6bb5 4\u4eba\u5076\u98de\u5165\u9636\u6bb5 1\u4eba\u5076\u5c55\u5f00 1\u62a4\u76fe 1\u9636\u6bb5 2\u4eba\u5076\u5c55\u5f00 2\u62a4\u76fe 2\u9636\u6bb5 3\u4eba\u5076\u5c55\u5f00 3\u62a4\u76fe 3\u9636\u6bb5 4\u6700\u7ec8\u653b\u51fb\u51c6\u5907\u9636\u6bb5 5\u53cc Boss\u821e - \u9636\u6bb5 1\u821e - \u9636\u6bb5 2\u821e - \u9636\u6bb5 3\u821e - \u9636\u6bb5 4\u96ea - \u9636\u6bb5 1\u96ea - \u9636\u6bb5 2\u96ea - \u9636\u6bb5 3\u96ea - \u9636\u6bb5 4\u9636\u6bb5 1\u9636\u6bb5 2\u9636\u6bb5 3\u9636\u6bb5 4\u9636\u6bb5 5\u9636\u6bb5 6\u9636\u6bb5 1\u9636\u6bb5 2\u9636\u6bb5 3\u9636\u6bb5 4\u9636\u6bb5 5\u9636\u6bb5 6\u9636\u6bb5 7\u9636\u6bb5 1\u9636\u6bb5 2\u9636\u6bb5 3\u9636\u6bb5 4\u9636\u6bb5 5\u9636\u6bb5 6\u9636\u6bb5 7\u9636\u6bb5 8\u9636\u6bb5 9\u9636\u6bb5 1\u9636\u6bb5 1\u9636\u6bb5 2\u9636\u6bb5 1\u9636\u6bb5 1\u9636\u6bb5 1\u9636\u6bb5 1\u6b63\u5e38\u5f00\u59cb\u9636\u6bb5\u72b6\u6001\u6b63\u5e38\u53d1\u72c2\u8fdb\u5165\u65b9\u5f0f\u51fb\u7834\u8fdb\u5165\u8d85\u65f6\u8fdb\u5165\u89e6\u53d1\u65b9\u5f0f\u8840\u91cf\u89e6\u53d1\u65f6\u95f4\u89e6\u53d1\u8d77\u59cb\u8840\u91cf";}
} }
