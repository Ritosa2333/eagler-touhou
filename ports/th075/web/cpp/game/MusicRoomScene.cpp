#include "MusicRoomScene.hpp"
namespace th075 {
bool MusicCatalog::load(const void* source,std::size_t size){
 tracks.clear();error.clear();if(size>16*1024*1024){error="Music catalog exceeds resource limit";return false;}
 const auto* p=static_cast<const u8*>(source);std::vector<u8> text(p,p+size);u8 key=0x5c,step=0x5a;
 for(auto& b:text){b^=key;key=u8(key+step);step=u8(step+0x3d);}
 MusicTrack track;u32 column=0;std::size_t at=0;
 while(at<size){const bool quoted=text[at]=='"';if(quoted)at++;const auto start=at;auto end=at;u8 quoting=quoted?1:0;
  while(at<size&&text[at]!=','&&text[at]!='\n'){if(!quoting)end++;else if(quoting==1){if(text[at]=='"')quoting=2;else end++;}at++;}
  if(at==size){error="Truncated music catalog row";tracks.clear();return false;}
  const std::string field(reinterpret_cast<const char*>(text.data()+start),end-start);
  if(column==0){track={};for(const auto ch:field)track.id=i8(u8(i32(track.id)*10+u8(ch)-'0'));}
  else if(column==1)track.title=field;else if(column==2)track.author=field;
  else if(column==3){track.description=field;if(tracks.size()>=60){error="Music catalog has too many rows";tracks.clear();return false;}tracks.push_back(std::move(track));}
  column=text[at]=='\n'?0:column+1;at++;
 }
 return true;
}
u16 MusicRoomScene::advance(const MusicRoomInput& input,const SelectionUnlocks& flags,MusicRoomSignals& events){
 events.render_music_room();events.present_music_room();if(catalog.tracks.empty())return 0x2202;
 const auto unlocked=[&](){return flags.storyFlags[u8(catalog.tracks[selected].id+100)]!=0;};
 if(input.vertical[0]==1||input.vertical[1]==1){selected=u8(selected+1);if(i8(selected)>=i8(catalog.tracks.size()))selected=0;if(unlocked())events.update_details(selected);}
 else if(input.vertical[0]==-1||input.vertical[1]==-1){selected=u8(selected-1);if(i8(selected)<0)selected=u8(catalog.tracks.size()-1);if(unlocked())events.update_details(selected);}
 if(i8(selected)<i8(firstVisible))firstVisible=selected;if(i8(firstVisible)+9<i8(selected))firstVisible=u8(selected-9);
 if((input.confirm[0]==1||input.confirm[1]==1)&&changeDelay==0&&unlocked()){
  events.update_title(selected);events.select_music(catalog.tracks[selected].id);events.fade_music(-10000,-170,1,1);changeDelay=60;
 }
 if(((input.cancel[0]==1||input.cancel[1]==1)&&changeDelay==0)||input.escape){events.fade_music(-10000,-160,1,0);return 0x2202;}
 if(i8(changeDelay)>0)changeDelay--;return 12;
}
void MusicRoomScene::number(ImageRenderer& images,float x,float y,u32 color,i32 value,i32 digits){
 auto q=rectangle_quad(float(double(x)-15.0),y,x,float(double(y)+16.0));
 const auto draw=[&](){const i32 column=(value%10)*15;images.quad(1,q,{column,0,column+15,16},color);translate_quad(q,-14,0,0);};
 if(digits<1)while(value>=0){draw();value=value<10?-1:value/10;}else while(digits-->0){draw();value/=10;}
}
}
