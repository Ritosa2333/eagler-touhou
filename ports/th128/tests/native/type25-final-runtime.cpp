#include "../ecl-fixture.hpp"
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

extern "C" {
int th128_world_load(const char*,std::uint32_t,const void*,std::uint32_t);
int th128_world_start(const char*,std::uint32_t,int,unsigned);
int th128_world_load_stage_intro(int,const char*,std::uint32_t,const void*,std::uint32_t);
int th128_world_load_enemy_anm(int,const void*,std::uint32_t);
int th128_world_enable_bullets(const void*,std::uint32_t,const void*,std::uint32_t,float,float);
int th128_world_add_bullet_profile(int,const void*,unsigned);
int th128_world_install_type25_conversion();
int th128_world_enable_bullet_updates(const void*,std::uint32_t);
int th128_world_tick();
const char* th128_world_error();
const char* th128_world_bullet_draws();
const char* th128_world_bullet_draw_error();
const char* th128_world_state();
void th128_world_reset();
}

using namespace ecl_fixture;

static std::vector<unsigned char> readFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) { std::fprintf(stderr, "missing %s\n", path); std::exit(2); }
    return {std::istreambuf_iterator<char>(file), {}};
}
static std::size_t countOccurrences(const std::string& text, const std::string& needle) {
    std::size_t count = 0;
    for (std::size_t position = 0; (position = text.find(needle, position)) != std::string::npos;
         position += needle.size()) ++count;
    return count;
}

int main(int argc, char** argv) {
    try {
        require(argc == 8, "type8 type25 type30 bullet enemy logo shot");
        const auto type8 = readFile(argv[1]);
        const auto type25 = readFile(argv[2]);
        const auto type30 = readFile(argv[3]);
        const auto bullet = readFile(argv[4]);
        const auto enemy = readFile(argv[5]);
        const auto logo = readFile(argv[6]);
        const auto shot = readFile(argv[7]);
        const auto data = program({{"main", join({
            instruction(44, {floatBits(.25f)}), instruction(45, {floatBits(-9981)}, 0, 1),
            instruction(44, {floatBits(.5f)}), instruction(45, {floatBits(-9980)}, 0, 1),
            instruction(0, {}, 83),
            instruction(500, {0}), instruction(502, {0, 8, 10}),
            instruction(503, {0, floatBits(0), floatBits(0)}),
            instruction(505, {0, floatBits(0), floatBits(0)}),
            instruction(506, {0, 1, 1}), instruction(501, {0}),
            instruction(304, {floatBits(-9981), floatBits(-9980)}, 0, 3),
            instruction(529, {3}), instruction(0, {}, 100)})}});
        const std::string name = "st_a1_3.ecl";
        th128_world_reset();
        require(th128_world_load(name.c_str(), std::uint32_t(name.size()), data.data(),
                                 std::uint32_t(data.size())), th128_world_error());
        require(th128_world_start(name.c_str(), std::uint32_t(name.size()), 1, 1), th128_world_error());
        require(th128_world_load_stage_intro(3, "logo_a1_3.anm", 13, logo.data(),
                                             std::uint32_t(logo.size())), th128_world_error());
        require(th128_world_load_enemy_anm(1, enemy.data(), std::uint32_t(enemy.size())), th128_world_error());
        require(th128_world_enable_bullets(type8.data(), std::uint32_t(type8.size()), bullet.data(),
                                           std::uint32_t(bullet.size()), 0, 400), th128_world_error());
        require(!th128_world_install_type25_conversion(), "unconfigured type25 target must reject install");
        std::vector<unsigned char> bad = type25;
        for (std::size_t index = 0; index < bad.size(); ++index) bad[index] = 0;
        require(!th128_world_add_bullet_profile(25, bad.data(), std::uint32_t(bad.size())),
                "bad type25 profile must be rejected");
        require(th128_world_add_bullet_profile(25, type25.data(), std::uint32_t(type25.size())),
                th128_world_error());
        require(th128_world_add_bullet_profile(30, type30.data(), std::uint32_t(type30.size())),
                th128_world_error());
        require(th128_world_install_type25_conversion(), th128_world_error());
        require(!th128_world_install_type25_conversion(), "host install is one-shot");
        require(th128_world_enable_bullet_updates(shot.data(), std::uint32_t(shot.size())),
                th128_world_error());
        require(th128_world_tick() == 1, th128_world_error());
        for (int tick = 0; tick < 90; ++tick) {
            if (!th128_world_tick()) {
                std::fprintf(stderr, "tick %d: %s\n", tick + 1, th128_world_error());
                return 1;
            }
        }
        const std::string state = th128_world_state();
        require(countOccurrences(state, "\"type\":25") == 1, "one live converted type25 bullet; state=" + state);
        const char* draws = th128_world_bullet_draws();
        const std::string drawError = th128_world_bullet_draw_error();
        require(drawError.empty(), drawError);
        const std::string drawText = draws ? draws : "";
        require(countOccurrences(drawText, "\"drawPass\":\"hostile-bullets\"") == 2,
                "one primary + one secondary hostile draw; got " + drawText);
        th128_world_reset();
        require(th128_world_load(name.c_str(), std::uint32_t(name.size()), data.data(),
                                 std::uint32_t(data.size())), th128_world_error());
        require(th128_world_start(name.c_str(), std::uint32_t(name.size()), 1, 1), th128_world_error());
        require(th128_world_load_enemy_anm(1, enemy.data(), std::uint32_t(enemy.size())), th128_world_error());
        require(th128_world_enable_bullets(type8.data(), std::uint32_t(type8.size()), bullet.data(),
                                           std::uint32_t(bullet.size()), 0, 400), th128_world_error());
        require(th128_world_add_bullet_profile(25, type25.data(), std::uint32_t(type25.size())),
                th128_world_error());
        require(th128_world_add_bullet_profile(30, type30.data(), std::uint32_t(type30.size())),
                th128_world_error());
        require(th128_world_install_type25_conversion(), th128_world_error());
        require(th128_world_enable_bullet_updates(shot.data(), std::uint32_t(shot.size())),
                th128_world_error());
        require(th128_world_tick() == 1, th128_world_error());
        for (int tick = 0; tick < 90; ++tick) {
            if (!th128_world_tick()) {
                std::fprintf(stderr, "second tick %d: %s\n", tick + 1, th128_world_error());
                return 1;
            }
        }
        const std::string secondState = th128_world_state();
        require(countOccurrences(secondState, "\"type\":25") == 1,
                "second world session has one type25 bullet; state=" + secondState);
        const std::string secondDraws = th128_world_bullet_draws();
        require(th128_world_bullet_draw_error()[0] == '\0', th128_world_bullet_draw_error());
        require(countOccurrences(secondDraws, "\"drawPass\":\"hostile-bullets\"") == 2,
                "second world session keeps one primary + one secondary draw");
        std::puts("PASS actual Type25 WorldExports two-session install/type25/prepass/reset/no-dual-draw flow");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "FAIL %s\n", error.what());
        return 1;
    }
}
