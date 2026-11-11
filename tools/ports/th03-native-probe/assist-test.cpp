#include <cstdint>
#include <iostream>
#include <utility>
#include "th03/core/game.hpp"
extern "C" { extern int th03_assist_focus, th03_assist_x, th03_assist_y; }

int move(std::uint8_t mode, bool focus) {
	th03::GameInit init{};
	init.game_mode = mode;
	init.rank = 2;
	init.is_cpu = {0, 1};
	init.paletted_character = {1, 3};
	auto created = th03::Game::create(init);
	if(!created) return -10000;
	auto game = std::move(*created);
	th03_assist_focus = focus;
	game.step({});
	const int start = th03_assist_x;
	for(int i=0; i<12; ++i) game.step({.p1 = 8});
	return th03_assist_x - start;
}
int main() {
	const int normal = move(th03::game_mode::vs_1p_cpu, false);
	const int slow = move(th03::game_mode::vs_1p_cpu, true);
	if(normal <= 0 || slow <= 0 || slow * 2 != normal) {
		std::cerr << "movement mismatch " << normal << ' ' << slow << '\n'; return 1;
	}
	th03_assist_x = -1; th03_assist_y = -1;
	move(th03::game_mode::demo, true);
	if(th03_assist_x != -1) { std::cerr << "demo marker leaked\n"; return 1; }
	std::cout << "TH03 assists: normal and half speed, demo isolation passed\n";
}
