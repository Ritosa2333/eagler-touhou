#include <cstdint>
#include <iostream>
#include <utility>

#include "th03/core/game.hpp"

int main()
{
	th03::GameInit init{};
	init.game_mode = th03::game_mode::story;
	init.story_stage = 8U;
	init.remaining_credits = 3U;
	init.paletted_character = {1, 3};
	auto created = th03::Game::create(init);
	if(!created) {
		std::cerr << "game creation failed\n";
		return 1;
	}
	auto game = std::move(*created);
	auto snapshot = game.capture_rollback_simulation();
	if(!snapshot) {
		std::cerr << "snapshot capture failed\n";
		return 1;
	}
	const auto before = game.canonical_state();
	const auto step = game.step({});
	if(step.status == th03::StepStatus::simulation_not_implemented ||
		step.status == th03::StepStatus::invalid_input_context) {
		std::cerr << "simulation step failed\n";
		return 1;
	}
	if(!game.restore_rollback_simulation(*snapshot)) {
		std::cerr << "snapshot restore failed\n";
		return 1;
	}
	const auto restored = game.canonical_state();
	if(restored.global_frame != before.global_frame ||
		restored.state_hash != before.state_hash) {
		std::cerr << "restored state differs\n";
		return 1;
	}
	std::cout << "TH03 core: create, step, snapshot and restore passed\n";
}
