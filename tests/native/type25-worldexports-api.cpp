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
const char* th128_world_error();
void th128_world_reset();
}

using namespace ecl_fixture;

static std::vector<unsigned char> readFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) { std::fprintf(stderr, "missing %s\n", path); std::exit(2); }
    return {std::istreambuf_iterator<char>(file), {}};
}

int main(int argc, char** argv) {
    try {
        require(argc == 7, "type8 type25 type30 bullet enemy logo");
        const auto type8 = readFile(argv[1]);
        const auto type25 = readFile(argv[2]);
        const auto type30 = readFile(argv[3]);
        const auto bullet = readFile(argv[4]);
        const auto enemy = readFile(argv[5]);
        const auto logo = readFile(argv[6]);
        const std::string name = "st_a1_3.ecl";
        const auto data = program({{"main", join({
            instruction(44, {floatBits(.25f)}), instruction(45, {floatBits(-9981)}, 0, 1),
            instruction(44, {floatBits(.5f)}), instruction(45, {floatBits(-9980)}, 0, 1),
            instruction(0, {}, 83), instruction(529, {3})})}});
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
        require(th128_world_add_bullet_profile(25, type25.data(), std::uint32_t(type25.size())),
                th128_world_error());
        require(th128_world_add_bullet_profile(30, type30.data(), std::uint32_t(type30.size())),
                th128_world_error());
        require(th128_world_install_type25_conversion(), th128_world_error());
        require(!th128_world_install_type25_conversion(), "install reentry must reject");
        th128_world_reset();
        std::puts("PASS actual WorldExports Type25 API install/reentry/unconfigured/reset");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "FAIL %s\n", error.what());
        return 1;
    }
}
