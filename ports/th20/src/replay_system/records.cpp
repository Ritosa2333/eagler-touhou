#include "replay.hpp"
#include <cstring>
#include <stdexcept>
namespace th20::source::replay {
FileHeader::FileHeader() noexcept:magic(0x72303274),version(1),byte_06(0),byte_07(0),byte_08(0),field_0c(0),field_10(0x100),field_14(0),byte_18(0),byte_19(0),word_1a(0),header_size(0),user_size(0),stage_size(0),packed_size(0),unpacked_size(0) {}
UserHeader::UserHeader() noexcept:field_00(0),field_04(0),byte_08(0),byte_09(0),byte_0b(0),timestamp(0),fields_18{},fields_d0{},stones{0,8,8,8},inherited{},difficulty(1),finished_stage(0),field_f8(0),spell(0) {flags&=~3u;platform::initialize_configuration(configuration);}
StageRecord::StageRecord() noexcept:stage(0),seed(0),frame_count(0),data_bytes(0),fixed_x(0),fixed_y(0),field_18(0),capture_times{} {game_session::construct_player_table(player_table);flags&=~1u;}
RecordingChunk::RecordingChunk() noexcept:inputs{},fps{},input_cursor(inputs),fps_cursor(fps) {scheduler::initialize_link(link,reinterpret_cast<scheduler::Node*>(this));}
RecordingChunk::~RecordingChunk(){scheduler::unlink(link);}
bool RecordingChunk::append(std::uint16_t current,std::uint16_t pressed,std::uint16_t released){
    if(input_cursor<inputs||input_cursor>=inputs+36000)throw std::out_of_range("Replay recording cursor outside original chunk");
    *input_cursor++={current,pressed,released};return frame_count()>35999;
}
bool RecordingChunk::append_fps(std::uint8_t value){
    if(fps_cursor<fps||fps_cursor>=fps+36000)throw std::out_of_range("Replay FPS cursor outside original chunk");
    *fps_cursor++=value;return fps_count()>1199;
}
int RecordingChunk::frame_count() const noexcept{return static_cast<int>(input_cursor-inputs);}
int RecordingChunk::fps_count() const noexcept{return static_cast<int>(fps_cursor-fps);}
PlaybackCursor::PlaybackCursor() noexcept:inputs(nullptr),input_cursor(nullptr),fps(nullptr),fps_cursor(nullptr),stage(nullptr),frame(0){scheduler::initialize_link(link,reinterpret_cast<scheduler::Node*>(this));}
PlaybackCursor::~PlaybackCursor(){scheduler::unlink(link);}
void PlaybackCursor::rewind(bool active) noexcept{input_cursor=inputs;fps_cursor=fps;frame=active?0:-1;}
}
