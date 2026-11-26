// 原652D80..652ED0音乐切换，以及6134C7..61354B提示数值更新。
#pragma once
#include "core/shared/MusicPresentationContext.hpp"
#include "core/shared/MusicPresentationServices.hpp"
#include "core/shared/MusicPresentationConstants.hpp"
#include "core/shared/MusicPresentationArithmetic.hpp"
#include <string>
namespace kaikeidou {
inline void switch_music(std::uint32_t track,std::uint32_t mode,MusicPresentationContext& context,
 const std::uint32_t& outer_scene,const std::uint32_t& scene,const std::uint32_t& volume,
 unsigned precision,MusicPresentationServices& services){
 services.set_create_sound_data_type(2);
 services.stop_sound(context.audio_handle);
 if(track!=context.current_track){
  if(mode!=0){
   services.delete_sound(context.audio_handle,0);
   if(outer_scene==100&&scene==0){
    context.banner.word(8,420);
    context.banner.quad(0,0x4089000000000000ull);
    const auto title=music_presentation_detail::title(track);
    constexpr char prefix[]="BGM ";
    for(unsigned i=0;i<4;++i)context.banner.data[12+i]=static_cast<std::uint8_t>(prefix[i]);
    for(std::size_t i=0;i<title.size();++i)context.banner.data[16+i]=static_cast<std::uint8_t>(title[i]);
    context.banner.data[16+title.size()]=0;
   }
  }
  std::string path="bgm/bgm00.mp3";
  if(track!=0){
   const auto remainder=music_presentation_detail::signed_word(track)%2;
   const auto bank=music_presentation_detail::signed_word(music_presentation_detail::filename_bank(track,precision));
   path="bgm/bgm"+std::to_string(bank)+std::to_string(1-remainder)+".mp3";
  }
  context.audio_handle=static_cast<std::uint32_t>(services.load_sound(path,3,0xffffffffu));
  const auto live_handle=context.audio_handle;
  const auto adjusted=music_presentation_detail::volume(volume,precision);
  services.set_volume(adjusted,live_handle);
 }
 services.play_sound(context.audio_handle,3,mode);
 context.current_track=track;
}
inline void update_music_presentation(MusicPresentationContext& context,unsigned precision){
 if(context.banner.word(8)==0)return;
 context.banner.word(8,context.banner.word(8)-1u);
 const auto timer=music_presentation_detail::signed_word(context.banner.word(8));
 if(timer>=360)return;
 if(timer>=100)context.banner.quad(0,ease_option_value(context.banner.quad(0),0x4077c00000000000ull,0x4034000000000000ull,precision));
 else if(timer<=60)context.banner.quad(0,ease_option_value(context.banner.quad(0),0x4089000000000000ull,0x403e000000000000ull,precision));
}
}
