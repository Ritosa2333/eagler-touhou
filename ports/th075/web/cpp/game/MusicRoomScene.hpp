#pragma once
#include "CharacterSelectScene.hpp"
namespace th075 {
struct MusicTrack {i8 id=0;std::string title,author,description;};
struct MusicCatalog {std::vector<MusicTrack> tracks;std::string error;bool load(const void*,std::size_t);};
struct MusicRoomInput {std::array<i32,2> vertical{},confirm{},cancel{};bool escape=false;};
struct MusicRoomSignals:SceneSignals {
 virtual void render_music_room()=0;virtual void present_music_room()=0;
 virtual void select_music(i16 track)=0;virtual void update_title(u8 index)=0;virtual void update_details(u8 index)=0;
};
struct MusicRoomArtwork {u32 details=0,catalog=0,title=0,credit=0;};
class MusicRoomScene {
public:
 MusicCatalog catalog;u8 selected=0,firstVisible=0,changeDelay=0;
 void initialize(){selected=firstVisible=changeDelay=0;}
 u16 advance(const MusicRoomInput&,const SelectionUnlocks&,MusicRoomSignals&);
 void render(ImageRenderer&,const MusicRoomArtwork&)const;
 static void number(ImageRenderer&,float x,float y,u32 color,i32 value,i32 digits=0);
};
}
