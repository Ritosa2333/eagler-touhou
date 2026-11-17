#include <bit>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#include "th03/core/game.hpp"

namespace {
std::optional<th03::Game> game;
std::unique_ptr<th03::RollbackSimulationSnapshot> snapshot;
}

extern "C" {
int th03_create(std::uint32_t seed, int p1_character, int p2_character)
{
	game.reset();
	snapshot.reset();
	if(p1_character < 1 || p1_character > 18 || p2_character < 1 || p2_character > 18) return 0;
	th03::GameInit init{};
	init.game_mode = th03::game_mode::vs_1p_2p;
	init.resident_rand = std::bit_cast<std::int32_t>(seed);
	init.random_seed = seed;
	init.rank = 2U;
	init.paletted_character = {static_cast<std::int8_t>(p1_character), static_cast<std::int8_t>(p2_character)};
	auto created = th03::Game::create(init);
	if(!created) return 0;
	game.emplace(std::move(*created));
	return 1;
}

int th03_step(std::uint16_t p1, std::uint16_t p2)
{
	if(!game) return -1;
	return static_cast<int>(game->step(th03::CoreInput{.p1 = p1, .p2 = p2}).status);
}

std::uint32_t th03_frame() { return game ? game->canonical_state().global_frame : 0; }
std::uint32_t th03_hash() { return game ? game->canonical_state().state_hash : 0; }

int th03_save()
{
	if(!game) return 0;
	auto value = game->capture_rollback_simulation();
	if(!value) return 0;
	snapshot = std::make_unique<th03::RollbackSimulationSnapshot>(std::move(*value));
	return 1;
}

int th03_restore()
{
	return game && snapshot && game->restore_rollback_simulation(*snapshot).has_value();
}

void th03_destroy() { snapshot.reset(); game.reset(); }
}
