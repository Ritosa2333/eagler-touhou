#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <utility>
#include <vector>

#include "th03/app/direct_vs/direct_vs_launch.hpp"
#include "th03/app/main/main_direct_vs_runner.hpp"
#include "th03/app/runtime/host_surface.hpp"

namespace {
std::unique_ptr<th03::app::MainDirectVsRunner> runner;
std::vector<th03::present::ComposedPixel> pixels(th03::present::graphics_page_pixels);
std::array<std::byte, 640U * 400U * 4U> rgba{};
std::uint64_t tick = 0;
}

extern "C" {
int th03_visual_create()
{
	runner.reset();
	tick = 0;
	th03::app::DirectVsLaunchRequest request{};
	request.content.data_root = "/content";
	request.content.main_executable = "/content/MAIN.EXE";
	request.content.font_rom = "/content/FONT.ROM";
	request.process.seed = 34;
	request.process.player_one = {.character = th03::app::MainCharacter::reimu, .palette = th03::app::MainPaletteVariant::first};
	auto process = th03::app::load_direct_vs_process(std::move(request));
	if(!process) return -100 - static_cast<int>(process.error().code);
	auto created = th03::app::MainDirectVsRunner::create(std::move(*process));
	if(!created) return -200 - static_cast<int>(created.error().code);
	runner = std::move(*created);
	return 1;
}

int th03_visual_step(std::uint16_t p1, std::uint16_t p2)
{
	if(!runner) return -1;
	th03::app::LogicalTickInput sample{};
	sample.tick_index = tick++;
	sample.deadline_ns = tick * 17723000U;
	sample.input.p1 = p1;
	sample.input.p2 = p2;
	auto result = runner->consume(sample);
	return result ? static_cast<int>(result->status) : -100 - static_cast<int>(result.error().code);
}

const std::byte* th03_visual_pixels()
{
	if(!runner) return nullptr;
	auto surface = runner->compose_shown_surface(std::span<th03::present::ComposedPixel, th03::present::graphics_page_pixels>(pixels.data(), pixels.size()));
	if(!surface) return nullptr;
	if(!th03::app::source_surface_to_host_rgba(*surface, {.bytes = rgba, .pitch_bytes = 640U * 4U})) return nullptr;
	return rgba.data();
}

void th03_visual_destroy() { runner.reset(); }
}
