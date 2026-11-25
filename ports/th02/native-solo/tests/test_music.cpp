#include "music_room.hpp"
#include "music_room_polygons.hpp"
#include <vector>
#include <fstream>
#include <iostream>
unsigned checks=0;void check(bool value){++checks;if(!value)throw std::runtime_error("Music contract failed");}
void check_music_polygons(){
    using Animation=soew::MusicRoomPolygons;
    const auto size=Animation::width*Animation::height;
    std::vector<std::uint32_t> base(size),blue(size);
    for(int i=0;i<size;++i){
        const unsigned index=i%16;
        base[i]=index;
        blue[i]=index|1;
    }
    Animation animation;
    auto first=base;
    animation.paint(first,blue);
    check(first!=base);
    auto overlap=first;
    animation.paint(overlap,blue);
    check(overlap==first);
    for(unsigned step=1;step<=600;++step){
        animation.tick();
        if(step!=1&&step%150)continue;
        auto frame=base;
        animation.paint(frame,blue);
        check(frame!=base&&frame!=first);
        for(int i=0;i<size;++i)check(frame[i]==base[i]||frame[i]==blue[i]);
    }
}
int main(int argc,char**argv){try{
    check_music_polygons();
    std::vector<std::byte> fixture(12602,std::byte{32});
    for(unsigned line=0;line<300;++line){fixture[line*42+40]=std::byte{13};fixture[line*42+41]=std::byte{10};}
    const auto result=soew::decode_music_comments(fixture);check(result.size()==15&&result[14][19].size()==40);
    for(unsigned line=0;line<300;++line){auto broken=fixture;broken[line*42+40]=std::byte{0};bool rejected=false;try{soew::decode_music_comments(broken);}catch(const std::runtime_error&){rejected=true;}check(rejected);}
    check(soew::decode_music_comments(std::span<const std::byte>(fixture).first(12600))==result);
    for(unsigned length:{0u,839u,12601u}){bool rejected=false;try{soew::decode_music_comments(std::span<const std::byte>(fixture).first(length));}catch(const std::runtime_error&){rejected=true;}check(rejected);}
    int selected=0;for(int i=0;i<160;++i){selected=soew::music_move(selected,1);check(selected>=0&&selected<=16&&selected!=15);}check(selected==0);
    check(soew::music_move(0,-1)==16&&soew::music_move(16,-1)==14);
    for(int selected_track=0;selected_track<15;++selected_track)for(int active=-1;active<15;++active){
        check(soew::preview_f4_target(selected_track,active,false)==selected_track);
        check(soew::preview_f4_target(selected_track,active,true)==(selected_track==active?-1:selected_track));
    }
    for(int invalid:{-1,15,16,99}){check(soew::preview_f4_target(invalid,0,false)==-1);check(soew::preview_f4_target(invalid,0,true)==-1);}
    for(int stage=0;stage<6;++stage){check(soew::gameplay_music_track(stage,false,false)==(stage==5?13:stage+1));check(soew::gameplay_music_track(stage,true,false)==(stage==0?6:stage==5?13:stage+1));}
    check(soew::gameplay_music_track(0,false,true)==6);for(unsigned mask=0;mask<16;++mask)check(soew::gameplay_music_paused(mask&1,mask&2,mask&4,mask&8)==(!(mask&1)||bool(mask&14)));
    for(int i=1;i<argc;++i){std::ifstream f(argv[i],std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("Music source missing");const auto size=f.tellg();if(size!=12600&&size!=12602)throw std::runtime_error("Music source size mismatch");std::vector<std::byte>b(static_cast<std::size_t>(size));f.seekg(0);f.read(reinterpret_cast<char*>(b.data()),size);const auto actual=soew::decode_music_comments(b);check(actual[0][0].starts_with("No.1"));check(actual[14][0].starts_with(size==12600?"NO.15":"No.15"));std::cout<<"source_comments=300 bytes="<<size<<'\n';}
    std::cout<<"music_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
