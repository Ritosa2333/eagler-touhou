#include "AnmOpcodes.hpp"
#include "AnmVm.hpp"
#include "Archive.hpp"
#include "GameSessionResources.hpp"
#include <cstdio>

using namespace th11;
static_assert(kPortTitle == 14);

namespace {
u32 word(float value) { u32 bits; std::memcpy(&bits, &value, 4); return bits; }
float real(u32 bits) { float value; std::memcpy(&value, &bits, 4); return value; }

bool file_bytes(const char* name, std::vector<u8>& data) {
    FILE* file = std::fopen(name, "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::rewind(file);
    if (size < 0) { std::fclose(file); return false; }
    data.resize(size);
    const bool ok = std::fread(data.data(), 1, data.size(), file) == data.size();
    std::fclose(file);
    return ok;
}

bool scan_archive(const char* name) {
    std::vector<u8> bytes;
    Archive archive;
    if (!file_bytes(name, bytes) || !archive.open(bytes.data(), bytes.size(), 14)) return false;
    for (u32 index = 0; index < archive.entries.size(); ++index) {
        const auto& entry = archive.entries[index].name;
        if (entry.size() < 4 || entry.substr(entry.size() - 4) != ".anm") continue;
        std::vector<u8> data;
        if (!archive.read(index, data)) return false;
        AnmResource anm;
        if (!anm.open(data.data(), data.size())) {
            std::fprintf(stderr, "%s: ANM loader error %d\n", entry.c_str(), anm.last_error);
            return false;
        }
        std::printf("asset %s %zu\n", entry.c_str(), anm.scripts.size());
        if (entry == "ascii.anm" || entry == "ascii_960.anm" || entry == "ascii_1280.anm") {
            for (u32 slot : {239u, 240u, 249u, 253u, 258u}) {
                if (slot >= anm.sprites.size()) return false;
                const auto& sprite = anm.sprites[slot];
                std::printf("glyph %s %u %u %u\n", entry.c_str(), slot,
                            word(sprite.width), word(sprite.height));
            }
        }
        for (u32 slot = 0; slot < anm.scripts.size(); ++slot) {
            const auto& script = anm.scripts[slot];
            for (u32 at = script.file_offset; at + 8 <= data.size();) {
                AnmInstruction raw;
                std::memcpy(&raw, data.data() + at, sizeof raw);
                if (raw.opcode == -1) break;
                if (raw.length < 8 || at + raw.length > data.size()) return false;
                // For interpolators with a mode operand, record the original mode as well.
                i32 mode = -1;
                switch (raw.opcode) {
                case 407: case 408: case 409: case 410: case 411: case 412: case 413:
                case 414: case 427: case 428: case 430: case 433: case 435:
                    if (raw.length >= 16) std::memcpy(&mode, data.data() + at + 12, 4);
                    break;
                }
                std::printf("op %s %u %d %d\n", entry.c_str(), slot, raw.opcode, mode);
                if (raw.opcode == 415 && !raw.references && raw.length == 20) {
                    float speed[3];
                    std::memcpy(speed, data.data() + at + 8, sizeof speed);
                    if (speed[0] == 0 && speed[1] == 0 && speed[2] == 0)
                        std::printf("zero-angular %s %u\n", entry.c_str(), slot);
                }
                at += raw.length;
            }
        }
    }
    return true;
}

bool run_case(unsigned index, const u32 (&args)[19], std::vector<u8>& bytes) {
    auto* code = bytes.data();
    for (size_t offset = 0;;) {
        if (offset + 8 > bytes.size()) return false;
        auto& instruction = *reinterpret_cast<AnmInstruction*>(code + offset);
        if (instruction.opcode == -1) break;
        if (instruction.length < 8 || offset + instruction.length > bytes.size()) return false;
        const u16 op = anm_canonical_opcode(u16(instruction.opcode));
        if (op == anm_opcode_unmapped) {
            std::printf("unmapped %u %d\n", index, instruction.opcode);
            return true;
        }
        instruction.opcode = i16(op);
        offset += instruction.length;
    }
    AnmEnvironment env;
    env.rate = real(args[0]);
    env.script_rng.seed = 12345;
    env.visual_rng.seed = 54321;
    AnmVm vm{};
    vm.initialize();
    vm.flags = 3;
    vm.flags2 = args[1];
    vm.scale = {real(args[2]), real(args[3])};
    vm.scale_velocity = {real(args[4]), real(args[5])};
    vm.rotation = {real(args[6]), real(args[7]), real(args[8])};
    vm.angular_velocity = {real(args[9]), real(args[10]), real(args[11])};
    vm.uv_offset = {real(args[12]), real(args[13])};
    vm.scroll_velocity = {real(args[14]), real(args[15])};
    vm.slowdown_factor = real(args[16]);
    vm.sprite_size = {16, 24};
    vm.script_position = {120, 140, 0};
    vm.color = 0xffffffff;
    vm.secondary_color = 0x88334455;
    vm.timer.set(0, &env.rate);
    vm.age.set(0, &env.rate);
    vm.script_begin = vm.instruction = reinterpret_cast<AnmInstruction*>(code);
    for (u32 frame = 0; frame < args[17]; ++frame) {
        if (frame == args[18]) vm.interrupt(42);
        const int status = vm.update(env);
        const u32 observed[] = {
            u32(status), vm.instruction ? u32(reinterpret_cast<u8*>(vm.instruction) - code) : ~u32(0),
            u32(vm.timer.previous), u32(vm.timer.current), word(vm.timer.fractional),
            u32(vm.age.current), word(vm.age.fractional), u32(vm.flags) & 0xf, vm.flags2,
            word(vm.rotation.x), word(vm.rotation.y), word(vm.rotation.z),
            word(vm.scale.x), word(vm.scale.y), word(vm.uv_offset.x), word(vm.uv_offset.y),
            word(vm.scroll_velocity.x), word(vm.scroll_velocity.y), word(vm.script_position.x),
            word(vm.script_position.y), word(vm.uv[0].x), word(vm.uv[0].y),
            word(vm.uv[1].x), word(vm.uv[1].y), word(vm.uv[2].x), word(vm.uv[2].y),
            word(vm.uv[3].x), word(vm.uv[3].y),
            vm.color, vm.secondary_color, env.script_rng.seed, env.script_rng.calls,
            env.visual_rng.seed, env.visual_rng.calls,
        };
        std::printf("frame %u %u", index, frame);
        for (u32 value : observed) std::printf(" %u", value);
        std::putchar('\n');
        if (status) break;
    }
    return true;
}
}

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) return 2;
    const GameSessionResources resources;
    std::printf("boot-atlas %s\n", resources.ascii_anm_name());
    for (u32 width : {640u, 960u, 1280u})
        std::printf("atlas-width %u %s\n", width, GameConfig::ascii_anm_for_width(width));
    for (u32 i = 0; i <= 634; ++i) std::printf("map %u %u\n", i, anm_canonical_opcode(i));
    if (argc == 3 && !scan_archive(argv[2])) return 3;
    FILE* input = std::fopen(argv[1], "rb");
    if (!input) return 4;
    u32 size;
    for (unsigned index = 0; std::fread(&size, sizeof size, 1, input) == 1; ++index) {
        u32 args[19];
        if (size > 4096 || std::fread(args, sizeof args, 1, input) != 1) return 5;
        std::vector<u8> bytes(size);
        if (std::fread(bytes.data(), 1, size, input) != size || !run_case(index, args, bytes)) return 5;
    }
    std::fclose(input);
    return 0;
}
