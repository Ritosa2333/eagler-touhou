#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <utility>
#include <vector>

#include "th03/app/classic/classic_audio_session.hpp"
#include "th03/app/classic/classic_runner.hpp"
#include "th03/app/runtime/host_surface.hpp"
#include "th03/formats/v14.hpp"

extern "C" { extern int th03_assist_focus, th03_assist_focus_p2, th03_assist_x, th03_assist_y, th03_assist_x2, th03_assist_y2, th03_netplay_paused; extern unsigned th03_assist_seen; }
extern "C" { extern int th03_touch_active[2], th03_touch_unlimited[2], th03_touch_x[2], th03_touch_y[2]; }
extern "C" { extern int th03_surrender_request, th03_surrender_allowed; }
extern "C" { extern int th03_semantic_speculative, th03_semantic_boundary, th03_match_winner, th03_assist_charge; extern unsigned th03_match_serial; }
extern "C" { int th03_semantic_capture(int); int th03_semantic_restore(int); }
extern "C" { extern int th03_charge_enabled, th03_charge_mode[2], th03_charge_input[2][4], th03_charge_output[2][4], th03_charge_reset[2], th03_charge_overflow; }

namespace {
std::unique_ptr<th03::app::ClassicRunner> runner;
std::unique_ptr<th03::app::ClassicAudioSession> audio;
std::vector<th03::present::ComposedPixel> pixels(th03::present::graphics_page_pixels);
std::array<std::byte, 640U * 400U * 4U> rgba{};
std::vector<th03::audio::PmdCanonicalStereoSample> samples;
std::vector<std::byte> replay_bytes;
unsigned replay_serial = 0;
std::array<th03::app::ClassicAudioCommand,64> frame_commands{};
static_assert(sizeof(th03::app::ClassicAudioCommand) == 4);
std::size_t frame_command_count = 0;
bool deferred_audio = false;
int music_external = 0;
std::uint64_t tick = 0;
unsigned point_age = 99;
int scene = 0;
struct HostSnapshot {
	int focus[2],x[2],y[2],touch_active[2],touch_unlimited[2],touch_x[2],touch_y[2];
	int surrender_request,surrender_allowed;unsigned seen,age;
};
std::array<HostSnapshot,8> hosts{};
int drain() {
	while(auto command = runner->take_audio_command()) {
		if(frame_command_count == frame_commands.size()) return -30;
		frame_commands[frame_command_count++] = *command;
		if(!deferred_audio) {
			if(!audio->apply(*command)) return -30;
			if(music_external && !audio->set_user_volumes(0,100)) return -30;
		}
	}
	return 0;
}
}
extern "C" {
void th03_classic_external_music(int enabled) { music_external = enabled != 0; }
int th03_classic_create_mode(bool versus) {
	runner.reset(); audio.reset(); th03_charge_enabled = 0; th03_charge_mode[0] = th03_charge_mode[1] = 0; th03_match_winner = -1; th03_match_serial = 0; tick = 0; samples.clear(); replay_bytes.clear(); replay_serial = 0; deferred_audio = false; frame_command_count = 0; point_age = 99; scene = 0; th03_netplay_paused = 0; th03_assist_focus = 0; th03_assist_x = -1; th03_assist_y = -1;
	for(int seat = 0; seat < 2; ++seat) th03_touch_active[seat] = th03_touch_unlimited[seat] = th03_touch_x[seat] = th03_touch_y[seat] = 0;
	th03_surrender_request = th03_surrender_allowed = 0;
	th03::app::ClassicRunnerRequest request{};
	request.th03_direct_versus = versus;
	request.content.data_root = "/content";
	request.content.main_executable = "/content/MAIN.EXE";
	request.content.mainl_executable = "/content/MAINL.EXE";
	request.content.op_executable = "/content/OP.EXE";
	request.content.font_rom = "/content/FONT.ROM";
	request.content.language_overlay = "/content/TH3EN.DAT";
	request.content.new_classic_menu_assets = "/content/PODDUI";
	request.content.replay_root = "/user/REPLAY";
	request.content.config = "/user/YUME.CFG";
	request.content.score = "/user/YUME.NEM";
	request.fm_available = true;
	request.experience = th03::app::ExperienceProfile::new_classic;
	auto created = th03::app::ClassicRunner::load(std::move(request));
	if(!created) return -1000 - static_cast<int>(created.error().code)*100 - static_cast<int>(created.error().detail_code);
	runner = std::move(*created);
	auto sound = th03::app::ClassicAudioSession::create({.data_root = "/content", .tick_rate = th03::app::pc98_24khz_nominal_tick_rate, .effects_follow_bgm = false, .music_volume_percent = static_cast<std::uint8_t>(music_external ? 0 : 100)});
	if(!sound) { runner.reset(); return -200 - static_cast<int>(sound.error().code); }
	audio = std::move(*sound);
	return drain() == 0 ? 1 : -30;
}
int th03_classic_create() { return th03_classic_create_mode(false); }
int th03_classic_create_versus() { return th03_classic_create_mode(true); }
int th03_classic_step_pair(std::uint16_t input, std::uint16_t second, std::uint16_t system, std::uint8_t charge) {
	if(!runner || !audio) return -1;
	frame_command_count = 0;
	th03_semantic_boundary = 0;
	th03_charge_overflow = 0;
	for(int seat=0;seat<2;++seat) { th03_charge_reset[seat]=0; th03_charge_output[seat][0]=0; th03_charge_output[seat][1]=0; th03_charge_output[seat][2]=-1; th03_charge_output[seat][3]=0; }
	th03::app::LogicalTickInput sample{};
	sample.tick_index = tick++;
	sample.deadline_ns = tick * 17723000U;
	sample.input = {.p1 = input, .p2 = second, .system = system, .charge_mask = static_cast<std::uint8_t>(charge & 3U)};
	th03_assist_charge = charge & 3U;
	const auto previous_seen = th03_assist_seen;
	auto result = runner->consume(sample, audio->status());
	if(th03_charge_overflow) return -32;
	if(!result) return -100 - static_cast<int>(result.error().code);
	scene = static_cast<int>(result->pixel_oracle_identity.scene);
	if(!th03_semantic_speculative) if(auto replay = runner->take_completed_replay()) {
		auto encoded = th03::formats::write_v14_replay(*replay);
		if(!encoded) return -31;
		replay_bytes = std::move(*encoded); ++replay_serial;
	}
	point_age = th03_assist_seen != previous_seen ? 0 : (point_age < 99 ? point_age + 1 : 99);
	if(drain() != 0) return -30;
	if(deferred_audio) return static_cast<int>(result->status);
	auto rendered = audio->advance_tick();
	if(!rendered) return -200 - static_cast<int>(rendered.error().code);
	samples.assign(rendered->canonical_frames.begin(), rendered->canonical_frames.end());
	return static_cast<int>(result->status);
}
void th03_classic_charge_enable(int enabled) { th03_charge_enabled = enabled != 0; th03_charge_mode[0]=th03_charge_mode[1]=0; }
void th03_classic_charge_set(int seat, int mode, int charge, int ready, int released, int flags) {
	if(seat<0 || seat>1) return;
	th03_charge_mode[seat] = mode;
	th03_charge_input[seat][0]=charge; th03_charge_input[seat][1]=ready; th03_charge_input[seat][2]=released; th03_charge_input[seat][3]=flags;
}
int th03_classic_charge_get(int seat,int field) { return seat>=0 && seat<2 && field>=0 && field<4 ? th03_charge_output[seat][field] : -1; }
int th03_classic_step(std::uint16_t input, std::uint16_t system, std::uint8_t charge) { return th03_classic_step_pair(input, 0, system, charge); }
const std::byte* th03_classic_pixels() {
	if(!runner) return nullptr;
	auto surface = runner->compose_shown_surface(std::span<th03::present::ComposedPixel, th03::present::graphics_page_pixels>(pixels.data(), pixels.size()));
	if(!surface || !th03::app::source_surface_to_host_rgba(*surface, {.bytes = rgba, .pitch_bytes = 640U * 4U})) return nullptr;
	return rgba.data();
}
unsigned th03_classic_match_serial() { return th03_match_serial; }
int th03_classic_match_winner() { return th03_match_winner; }
unsigned th03_classic_replay_serial() { return replay_serial; }
const std::byte* th03_classic_replay_data() { return replay_bytes.data(); }
unsigned th03_classic_replay_size() { return static_cast<unsigned>(replay_bytes.size()); }
const void* th03_classic_audio() { return samples.data(); }
int th03_classic_audio_frames() { return static_cast<int>(samples.size()); }
void th03_classic_defer_audio() { deferred_audio = true; }
void th03_classic_speculative(int value) { th03_semantic_speculative = value != 0; }
int th03_classic_boundary() { return th03_semantic_boundary; }
void* th03_classic_audio_commands() { return frame_commands.data(); }
int th03_classic_audio_command_count() { return static_cast<int>(frame_command_count); }
int th03_classic_audio_commit(int count) {
	if(!audio || count < 0 || count > static_cast<int>(frame_commands.size())) return -1;
	for(int i=0;i<count;i++) { if(!audio->apply(frame_commands[static_cast<std::size_t>(i)])) return -30; if(music_external && !audio->set_user_volumes(0,100)) return -30; }
	auto rendered=audio->advance_tick(); if(!rendered) return -200-static_cast<int>(rendered.error().code);
	samples.assign(rendered->canonical_frames.begin(),rendered->canonical_frames.end());return 0;
}
void th03_classic_assist(int focus) { th03_assist_focus = focus != 0; }
void th03_classic_assist_pair(int p1, int p2) { th03_assist_focus = p1 != 0; th03_assist_focus_p2 = p2 != 0; }
int th03_classic_capture(int slot) {
	if(slot<0||slot>=8||!th03_semantic_capture(slot))return 0;
	auto &s=hosts[static_cast<std::size_t>(slot)];
	s.focus[0]=th03_assist_focus;s.focus[1]=th03_assist_focus_p2;s.x[0]=th03_assist_x;s.y[0]=th03_assist_y;s.x[1]=th03_assist_x2;s.y[1]=th03_assist_y2;
	for(int i=0;i<2;i++){s.touch_active[i]=th03_touch_active[i];s.touch_unlimited[i]=th03_touch_unlimited[i];s.touch_x[i]=th03_touch_x[i];s.touch_y[i]=th03_touch_y[i];}
	s.surrender_request=th03_surrender_request;s.surrender_allowed=th03_surrender_allowed;s.seen=th03_assist_seen;s.age=point_age;return 1;
}
int th03_classic_restore(int slot) {
	if(slot<0||slot>=8||!th03_semantic_restore(slot))return 0;
	const auto &s=hosts[static_cast<std::size_t>(slot)];
	th03_assist_focus=s.focus[0];th03_assist_focus_p2=s.focus[1];th03_assist_x=s.x[0];th03_assist_y=s.y[0];th03_assist_x2=s.x[1];th03_assist_y2=s.y[1];
	for(int i=0;i<2;i++){th03_touch_active[i]=s.touch_active[i];th03_touch_unlimited[i]=s.touch_unlimited[i];th03_touch_x[i]=s.touch_x[i];th03_touch_y[i]=s.touch_y[i];}
	th03_surrender_request=s.surrender_request;th03_surrender_allowed=s.surrender_allowed;th03_assist_seen=s.seen;point_age=s.age;return 1;
}
void th03_classic_touch(int seat, int active, int begin, int unlimited, int x, int y) {
	if(seat < 0 || seat > 1) return;
	if(!active || begin) { th03_touch_x[seat]=0; th03_touch_y[seat]=0; }
	th03_touch_active[seat]=active != 0; th03_touch_unlimited[seat]=unlimited != 0;
	if(active) { th03_touch_x[seat]=std::clamp(th03_touch_x[seat]+x,-8192,8191); th03_touch_y[seat]=std::clamp(th03_touch_y[seat]+y,-8192,8191); }
}
int th03_classic_point_x() { return point_age < 4 ? th03_assist_x : -1; }
int th03_classic_point_y() { return point_age < 4 ? th03_assist_y : -1; }
int th03_classic_point_x2() { return scene == 9 ? th03_assist_x2 : -1; }
int th03_classic_point_y2() { return scene == 9 ? th03_assist_y2 : -1; }
int th03_classic_scene() { return scene; }
void th03_classic_start_versus() { (void)th03_classic_create_versus(); }
int th03_classic_boot_state() { return 0; }
int th03_classic_paused() { return th03_netplay_paused; }
int th03_classic_surrender_allowed() { return th03_surrender_allowed; }
void th03_classic_surrender(int seat, int match) { if(seat >= 0 && seat < 2) th03_surrender_request = 1 + seat + (match ? 2 : 0); }
void th03_classic_destroy() { runner.reset(); audio.reset(); samples.clear(); }
}
