// th16 无头 harness：不依赖 SDL/GL，驱动 game/ 内核做确定性检查。
// 同一源码编译为 WASI（portable/build-harness.mjs）或本机可执行（tests/run.sh）。
//
//   harness rng <seed> <steps>                 → 每行 "step32 seed calls"（与 golden/rng.json 对照）
//   harness rng-float <seed> <steps>           → 每行 "unit signed angle"（IEEE 位型，十进制无符号）
//   harness archive-list <th16.dat>            → 每行 "name size fnv1a64"（与 thtk 解包对照）
//   harness archive-extract <th16.dat> <name> <out>
//   harness anm <th16.dat> <name.anm>          → 逐 entry 报告：name w h format sprites scripts texture
//   harness anm-script <th16.dat> <x.anm> <script>  → 反汇编单个 ANM 脚本（调试）
//   harness anm-run <th16.dat> <name.anm> <scripts: all|3|0-20,40> <frames> [layer]
//                                              → JSON：每帧世界链表各 VM 的 {id, script, bytes(0..0x568 hex)} 与 36 条 layer 绘制链
//                                                （与 tools/oracle/anm-oracle.py 直接执行原版得到的快照逐字节对照）
//   harness anm-bench <th16.dat> <name.anm> <script> <frames> [layer]
//                                              → "frames vms fnv1a64"：只推进不输出快照（性能口径）
//   harness anm-draw <th16.dat> <name.anm> <script> <frames> [layer]
//                                              → 每行一层绘制结果："frame layer script id blend x0 y0 x1 y1 x2 y2 x3 y3 u0 v0 u1 v1 u2 v2 u3 v3 color"
//                                                （spawn 后 tick frames 次再 emit；对应上屏几何，texture 句柄以 entry_index+1 占位）
//   harness scene-boot <th16.dat> <frames> [skip] [f:bits,...]
//                                              → GameSession（与浏览器同一逻辑层）：Boot→Title→主菜单→难度/机体/季节/关卡；frames 帧后报告
//                                                scene/boot_frames/title_live/screen/sub/cursor/difficulty/shot/season/stage/mode/quit/链数/quads，
//                                                随后 "event sfx|screen|scene|bgm-fade|transition|stub N" 与各四边形。skip = DAT_004c1800&0x180；
//                                                f:bits 在第 f 帧注入 held 位（0x1 Z，0x2 X，0x10 ↑，0x20 ↓，0x100 Esc，0x80000 Enter）；
//                                                Pf:button 第 f 帧手柄按钮按下；config=<th16.cfg> 开机读/回写配置（0x43c050）
//   harness pad-input-bits                      → 0x4018e0 手柄按钮表 → 输入位
//   harness config-file <th16.cfg>              → 0x43c050 读 + 校验 + 回写；打印字段与 100 字节映像
//   harness chain-demo                          → ChainManager 走法自检
//   harness frame-clock <t0> <t1> ...           → 每个墙钟时刻应执行的逻辑步数
//   harness input-edges <held0> <held1> ...     → 0x405580 边沿/连发/释放位
//   harness std <th16.dat> <stNN.std>          → STD 头 / 对象 / 四边形 / 实例 / 脚本（0x40ac30）
//   harness stage-run <th16.dat> <stage> <frames>    → StageManager 单独驱动（0x409db0→0x40add0→每帧 0x409e50），逐帧 JSON
//   harness stage-digest <th16.dat> <stage> <frames> → 同一状态流的 FNV-1a 64（tests/golden/stage.json，tools/oracle/stage-oracle.py 同口径）
//   harness game-run <th16.dat> <stage> <shot> <season> <frames> <input "f:held,..."> [enemies "f:n,..."] [dialogue "f:v,..."] [difficulty] [replay_slot] [lives] [refill] [mode]
//                                              → 直接进入场景 7（GameSession::start_game_direct），逐帧 JSON：Player 规范字段（含射击计时、冷却表、
//                                                自机弹 ×live、伤害源 ×live、本帧 sfx）+ 自机 VM 0x568 字节
//                                                （与 tools/oracle/game-oracle.py 快照同结构；enemies/dialogue 为未移植管理器的 PlayerWorld 桩输入）
//                                                环境变量 TH16_FRAMES_FROM=<n>：只输出第 n 帧（1 起）起的快照（长程跑只看换关附近，避免 GB 级 JSON）
//   harness game-digest <...同上...>              → 同一状态流的 FNV-1a 64（tests/golden/player.json，game-oracle.py --digest 同口径）
//   harness screen-fade <kind> <duration> <color> <frames> [hold_or_flag]
//                                              → 每行 "frame alpha alive"（kind 4/6/7/9 步进；与 tools/oracle 对拍）
//   harness replay-ff <mode> <counter> <held> [gm=1] [loading=0]
//                                              → 打印 0x448e40 返回码（1 Continue / 6 Restart）
//   harness replay-fps-color <byte>            → 打印 0x4482f0 颜色（十进制）
//   harness ending-smoke <th16.dat> <frames> [extra=0] [f:bits,...]
//                                              → 启动后强制进结局（0xf）或 Extra 回标题（0x10）；报告 ending_live/index/finished/scene
#include "../../web/cpp/game/Archive.hpp"
#include "../../web/cpp/game/AnmManager.hpp"
#include "../../web/cpp/game/AnmGeometry.hpp"
#include "../../web/cpp/game/AnmRenderer.hpp"
#include "../../web/cpp/game/AnmResource.hpp"
#include "../../web/cpp/game/Bomb.hpp"
#include "../../web/cpp/game/Chain.hpp"
#include "../../web/cpp/game/FrameClock.hpp"
#include "../../web/cpp/game/GameSession.hpp"
#include "../../web/cpp/game/Input.hpp"
#include "../../web/cpp/game/Laser.hpp"
#include "../../web/cpp/game/Replay.hpp"
#include "../../web/cpp/game/ReplayFile.hpp"
#include "../../web/cpp/game/ReplayRecording.hpp"
#include "../../web/cpp/game/ReplayGlobals.hpp"
#include "../../web/cpp/game/GameGlobals.hpp"
#include "../../web/cpp/game/GameMenu.hpp"
#include "../../web/cpp/game/FrameTimer.hpp"
#include "../../web/cpp/game/GameConfig.hpp"
#include "../../web/cpp/game/Spellcard.hpp"
#include "../../web/cpp/game/SpellPracticeTable.hpp"
#include "../../web/cpp/game/ScoreFile.hpp"
#include "../../web/cpp/game/Rng.hpp"
#include "../../web/cpp/game/ScreenEffect.hpp"
#include "../../web/cpp/game/Music.hpp"
#include "../../web/cpp/game/Sound.hpp"
#include "../../web/cpp/game/Stage.hpp"
#include "../../web/cpp/game/TextPaint.hpp"
#include "../../web/cpp/game/TextRaster.hpp"
#include "../../web/cpp/game/StageTable.hpp"
#include "../../web/cpp/game/Std.hpp"
#include "../../web/cpp/game/Supervisor.hpp"
#include "RecordingDevice.hpp"
#include "DrawDigest.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using namespace th16;

namespace {

bool read_file(const char* path, std::vector<u8>& out) {
    FILE* f = std::fopen(path, "rb");
    if (!f) return false;
    std::fseek(f, 0, SEEK_END);
    const long size = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    out.resize(size_t(size));
    const size_t got = std::fread(out.data(), 1, out.size(), f);
    std::fclose(f);
    return got == out.size();
}

u64 fnv1a64(const u8* p, size_t n) {
    u64 h = 0xcbf29ce484222325ull;
    for (size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 0x100000001b3ull; }
    return h;
}

u32 bits(float f) { u32 u; std::memcpy(&u, &f, 4); return u; }

int fail(const char* message) { std::fprintf(stderr, "harness: %s\n", message); return 1; }

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) return fail("usage: harness <command> ...");
    const std::string command = argv[1];
    if (command == "rng" && argc >= 4) {
        Rng rng{u16(std::strtoul(argv[2], nullptr, 0))};
        const int steps = std::atoi(argv[3]);
        for (int i = 0; i < steps; ++i) {
            const u32 v = rng.next32();
            std::printf("%u %u %u\n", v, rng.seed, rng.calls);
        }
        return 0;
    }
    if (command == "rng16" && argc >= 4) {
        Rng rng{u16(std::strtoul(argv[2], nullptr, 0))};
        const int steps = std::atoi(argv[3]);
        for (int i = 0; i < steps; ++i) {
            const u16 v = rng.next16();
            std::printf("%u %u %u\n", v, rng.seed, rng.calls);
        }
        return 0;
    }
    if (command == "rng-float" && argc >= 4) {
        const u16 seed = u16(std::strtoul(argv[2], nullptr, 0));
        const int steps = std::atoi(argv[3]);
        Rng a{seed}, b{seed}, c{seed};
        for (int i = 0; i < steps; ++i)
            std::printf("%u %u %u\n", bits(a.unit()), bits(b.signed_unit()), bits(c.angle()));
        return 0;
    }
    if (command == "text-paint-flag") {
        // 0x46d990：即便精灵矩形无效也置 flags_lo bit0；有矩形才入队
        AnmManager animations;
        AnmVm vm;
        vm.flags_lo = 0;
        std::vector<TextIntoTextureRequest> queue;
        text_paint_vm(animations, vm, 0xffffffu, 0u, 0, 0, 0, "test", &queue);
        std::printf("flag=%u queued=%zu height0=%d height8=%d\n", vm.flags_lo & 1u, queue.size(),
                    text_font_height(0), text_font_height(8));
        return 0;
    }
    if (command == "text-raster") {
        // 0x459240：合成字形源（每次 TextOut 在 (x, y) 点一个满覆盖像素）记录调用序并给出各遍结果
        struct DotGlyphs final : TextGlyphSource {
            bool alt = false;
            bool meiryo() const override { return alt; }
            void text_out(TextDib& dib, i32 font, i32 x, i32 y, u32 color, const char* bytes, i32 len) override {
                std::printf("out font=%d x=%d y=%d color=%06x len=%d [", font, x, y, color, len);
                for (i32 i = 0; i < len; ++i) std::printf("%02x", u8(bytes[i]));
                std::printf("]\n");
                if (x >= 0 && x < TextDib::kWidth && y >= 0 && y < TextDib::kAllocRows)
                    dib.pixels[size_t(y) * TextDib::kWidth + size_t(x)] =
                        text_dib_blend(dib.pixels[size_t(y) * TextDib::kWidth + size_t(x)], color, 255);
            }
        } glyphs;
        TextDib dib;
        std::vector<u16> rect;
        i32 w = 0, h = 0;
        auto px = [&](i32 x, i32 y) { return unsigned(dib.pixels[size_t(y) * TextDib::kWidth + size_t(x)]); };
        TextIntoTextureRequest req;
        req.text = "AB";
        req.font = 0;
        req.height = 17;
        req.color = 0xc08040;
        req.outline = 0x102030;
        req.shadow = true;
        req.x = 10;
        req.right = 16;
        req.bottom = 8;
        std::printf("case whole-shadow\n");
        text_raster_paint(req, glyphs, dib, rect, w, h);
        std::printf("px12_2=%04x px13_2=%04x px13_1=%04x px15_2=%04x rect=%dx%d r12_2=%04x\n", px(12, 2), px(13, 2),
                    px(13, 1), px(15, 2), w, h, unsigned(rect[2 * w + 12]));
        std::printf("case spaced\n");
        req.text = "\x82\xa0\x82\xa2x";
        req.spacing = 40;
        text_raster_paint(req, glyphs, dib, rect, w, h);
        std::printf("case edge\n");
        req.text = "A";
        req.spacing = 0;
        req.shadow = false;
        req.color = 0xffffff;
        req.x = -1;
        text_raster_paint(req, glyphs, dib, rect, w, h);
        std::printf("px0_2=%04x px1_2=%04x px2_2=%04x px1_1=%04x px0_1=%04x\n", px(0, 2), px(1, 2), px(2, 2), px(1, 1),
                    px(0, 1));
        std::printf("case meiryo\n");
        glyphs.alt = true;
        req.x = 4;
        req.font = 0;
        text_raster_paint(req, glyphs, dib, rect, w, h);
        std::printf("font0 r0_6=%04x\n", unsigned(rect[size_t(0) * w + 6]));
        req.font = 2;
        text_raster_paint(req, glyphs, dib, rect, w, h);
        std::printf("font2 r2_6=%04x\n", unsigned(rect[size_t(2) * w + 6]));
        return 0;
    }
    if (command == "spell-table") {
        // 0x4560b0：按难度字节重排；lock 参数时验证未解锁 → -1
        ScoreFile scores;
        const bool lock = argc >= 3 && std::strcmp(argv[2], "lock") == 0;
        if (!lock) {
            for (int id = 0; id < ScoreFile::kSpellRecords; ++id)
                scores.blocks[ScoreFile::kAllShotsBlock].spells[id].attempts[0] = 1;
        }
        i32 out[5];
        // group2 slot0：表 {20,21} 难度 2/3 → 槽 2/3
        SpellPracticeTable::fill_slot_ids(2, 0, out, &scores);
        std::printf("g2s0 %d %d %d %d %d\n", out[0], out[1], out[2], out[3], out[4]);
        if (!lock) {
            scores.blocks[0].spells[20].high_score = 123450;
            scores.blocks[0].spells[20].captures[1] = 2;
            scores.blocks[0].spells[20].attempts[1] = 5;
            scores.update_spell_high_score(0, 20, 999999);
            std::printf("high %u caps %u/%u\n", scores.blocks[0].spells[20].high_score,
                        scores.blocks[0].spells[20].captures[1], scores.blocks[0].spells[20].attempts[1]);
        }
        return 0;
    }
    if (command == "ending-smoke" && argc >= 4) {
        const char* path = argv[2];
        const int frames = std::atoi(argv[3]);
        bool extra = false;
        bool noskip = false;
        i32 difficulty = 1;
        i32 score = 0;
        bool dump_ascii = false;
        const char* replay_dir = nullptr;
        std::unordered_map<int, u32> inject;
        for (int a = 4; a < argc; ++a) {
            if (std::strncmp(argv[a], "extra=", 6) == 0) extra = std::atoi(argv[a] + 6) != 0;
            else if (std::strncmp(argv[a], "noskip=", 7) == 0) noskip = std::atoi(argv[a] + 7) != 0;
            else if (std::strncmp(argv[a], "diff=", 5) == 0) difficulty = std::atoi(argv[a] + 5);
            else if (std::strncmp(argv[a], "score=", 6) == 0) score = std::atoi(argv[a] + 6);
            else if (std::strcmp(argv[a], "ascii") == 0) dump_ascii = true;
            else if (std::strncmp(argv[a], "replay_dir=", 11) == 0) replay_dir = argv[a] + 11;
            else {
                int f = 0; u32 bits = 0;
                if (std::sscanf(argv[a], "%d:%x", &f, &bits) == 2) inject[f] = bits;
            }
        }
        std::vector<u8> bytes;
        if (!read_file(path, bytes)) return 1;
        GameSession session;
        if (!session.open(bytes.data(), u32(bytes.size()))) { std::fprintf(stderr, "%s\n", session.failure.c_str()); return 1; }
        if (!session.begin_boot()) { std::fprintf(stderr, "%s\n", session.failure.c_str()); return 1; }
        session.globals.shot = 0;
        session.globals.shot_aux = 0;
        session.globals.difficulty = difficulty;
        session.globals.score = score;
        if (replay_dir != nullptr) session.demo_directory = replay_dir;
        session.supervisor.request(extra ? SceneId::ExtraEnding : SceneId::Ending);
        // 先进一步以创建 Ending，再关烟雾短路（noskip 测 staff 全链）
        for (int f = 0; f < frames; ++f) {
            session.injected_input = inject.count(f) ? inject[f] : 0;
            if (noskip) session.ending.smoke_skip = false;
            if (!session.logic_step()) { std::fprintf(stderr, "%s\n", session.failure.c_str()); return 1; }
            if (noskip) session.ending.smoke_skip = false;
        }
        std::printf("scene=%d ending_live=%d ending_index=%d finished=%d title_live=%d title_screen=%d "
                    "title_origin=%d color=%x lines=%d paints=%d bgm=%s msg=%s\n",
                    int(session.supervisor.current), session.ending_live ? 1 : 0, session.ending.ending_index,
                    session.ending.finished ? 1 : 0, session.supervisor.title_menu_live ? 1 : 0,
                    int(session.title.screen), session.globals.title_origin,
                    session.ending.text_color, session.ending.lines_written, session.ending.text_paint_count,
                    session.ending.last_bgm.empty() ? "-" : session.ending.last_bgm.c_str(),
                    session.ending.msg_name.c_str());
        if (!session.ending.last_text.empty()) std::printf("text_bytes=%zu\n", session.ending.last_text.size());
        for (const char* e : session.ending.events) std::printf("event %s\n", e);
        if (dump_ascii) {
            for (i32 i = 0; i < session.ascii.entry_count(); ++i) {
                const AsciiManager::Entry& e = session.ascii.entry(i);
                std::printf("ascii %.1f %.1f %08x %d [%s]\n", double(e.pos.x), double(e.pos.y), e.color,
                            e.shadow ? 1 : 0, e.text);
            }
        }
        std::printf("last_name=[%s] rank0=[%s]\n", session.scores.stats.last_name,
                    session.scores.blocks[session.globals.shot + session.globals.shot_aux].ranks[difficulty][0].name);
        for (const TitleScene::Event& e : session.title.events) {
            std::printf("title %s %d\n", e.kind, e.value);
        }
        return 0;
    }
    if (command == "title-demo-idle") {
        // 0x44af80：主菜单无输入 0x708 帧 → flags bit6 + scene 0xd + demo 下标轮转 %3
        GameGlobals g;
        TitleScene title;
        title.globals = &g;
        title.screen = TitleScene::kMainMenu;
        InputState in;
        std::vector<i32> demos;
        for (int f = 0; f < 0x708 * 4; ++f) {
            title.tick_idle_demo(in);
            if (title.pending_demo >= 0) {
                demos.push_back(title.pending_demo);
                title.pending_demo = -1;
                g.scene_request = -1;
            }
        }
        std::printf("demos=");
        for (size_t i = 0; i < demos.size(); ++i) std::printf("%s%d", i ? "," : "", demos[i]);
        std::printf(" flags=%x\n", g.flags);
        return 0;
    }
    if (command == "replay-info" && argc >= 3) {
        // 0x448c10：打印 name / stages / 每段 stage,seed,frames,input_nz,fps_len,first_held_frame
        ReplayFile file;
        if (!file.open_file(argv[2])) return fail(file.error.c_str());
        std::printf("%s stages=%d\n", file.name, file.stage_count);
        for (size_t i = 0; i < file.stages.size(); ++i) {
            const auto& s = file.stages[i];
            i32 nz = 0, first = -1;
            for (i32 f = 0; f < s.frames; ++f) {
                const u8* p = s.inputs.data() + size_t(f) * 6u;
                if (p[0] | p[1] | p[2] | p[3] | p[4] | p[5]) {
                    ++nz;
                    if (first < 0) first = f;
                }
            }
            u16 held0 = 0;
            if (first >= 0) std::memcpy(&held0, s.inputs.data() + size_t(first) * 6u, 2);
            std::printf("%zu stage=%d seed=%u frames=%d input_nz=%d fps=%zu first=%d held=0x%x\n",
                        i, int(s.stage), unsigned(s.rng_seed), s.frames, nz, s.fps.size(), first,
                        unsigned(held0));
        }
        return 0;
    }
    if (command == "replay-globals" && argc >= 3) {
        // 段头 +0x14 全局快照（0x447760 / 0x449030 读回口径）与 +0xc 自机定点坐标、+0x23c 低速标志、+0x290 新开局位
        ReplayFile file;
        if (!file.open_file(argv[2])) return fail(file.error.c_str());
        for (size_t i = 0; i < file.stages.size(); ++i) {
            const auto& s = file.stages[i];
            if (s.header.size() != ReplayFile::kStageHeaderSize) return fail("stage header missing");
            GameGlobals g;
            load_replay_globals(g, s.header.data() + kReplayGlobalsOffset);
            i32 px = 0, py = 0, focus = 0, flags = 0;
            std::memcpy(&px, s.header.data() + 0xc, 4);
            std::memcpy(&py, s.header.data() + 0x10, 4);
            std::memcpy(&focus, s.header.data() + 0x23c, 4);
            std::memcpy(&flags, s.header.data() + 0x290, 4);
            std::printf("%zu stage=%d shot=%d+%d season=%d diff=%d score=%d lives=%d/%d bombs=%d/%d power=%d/%d/%d "
                        "gauge=%d/%d pv=%d graze=%d replay_slot=%d pos=%d,%d focus=%d new_game=%d seed=%d frames=%d\n",
                        i, g.stage, g.shot, g.shot_aux, g.season, g.difficulty, g.score, g.lives, g.life_fragments,
                        g.bombs, g.bomb_fragments, g.power, g.power_max, g.power_per_level, g.season_gauge,
                        g.season_gauge_max, g.point_value, g.graze, g.replay_slot, px, py, focus, flags & 1,
                        i32(s.rng_seed), s.frames);
        }
        return 0;
    }
    if (command == "replay-roundtrip" && argc >= 3) {
        // decode → encode → decode：段表 / 输入字节一致（LZSS 字节可不同）
        ReplayFile a;
        if (!a.open_file(argv[2])) return fail(a.error.c_str());
        std::vector<u8> encoded;
        if (!a.encode(encoded)) return fail(a.error.c_str());
        ReplayFile b;
        if (!b.open(encoded.data(), u32(encoded.size()))) return fail(b.error.c_str());
        if (std::memcmp(a.name, b.name, 8) != 0) return fail("name mismatch");
        if (a.stage_count != b.stage_count || a.stages.size() != b.stages.size()) return fail("stage count");
        if (a.user != b.user) return fail("user mismatch");
        for (size_t i = 0; i < a.stages.size(); ++i) {
            if (a.stages[i].stage != b.stages[i].stage || a.stages[i].rng_seed != b.stages[i].rng_seed ||
                a.stages[i].frames != b.stages[i].frames || a.stages[i].inputs != b.stages[i].inputs ||
                a.stages[i].fps != b.stages[i].fps) {
                return fail("stage body mismatch");
            }
        }
        std::printf("ok stages=%d bytes=%zu\n", a.stage_count, encoded.size());
        return 0;
    }
    if (command == "replay-record-save" && argc >= 4) {
        // 合成短记录 → 写槽 → 再读回（校验 0x448400 路径与 DiskReplayStore）
        const char* dir = argv[2];
        const int slot = std::atoi(argv[3]);
        ReplayRecording rec;
        GameGlobals g;
        g.shot = 1;
        g.shot_aux = 0;
        g.difficulty = 1;
        g.season = 2;
        g.stage = 1;
        rec.begin_run(g, 0x12345678);
        rec.begin_stage(1, 0xabcd);
        for (int i = 0; i < 90; ++i) {
            const u16 held = u16((i & 1) ? 0x11 : 0x10);
            if (!rec.tick(held, u16(i == 0 ? held : 0), u16(i == 1 ? 0x1 : 0), 60)) return fail(rec.error.c_str());
        }
        if (!rec.write_slot(dir, slot, "PORTTEST", 123450, 1.25f)) return fail(rec.error.c_str());
        ReplaySummary summary;
        if (!ReplayRecording::slot_summary(dir, slot, summary)) return fail("slot_summary");
        char path[512];
        if (!replay_slot_path(dir, slot, path, sizeof(path))) return fail("missing slot file");
        ReplayFile file;
        if (!file.open_file(path)) return fail(file.error.c_str());
        std::printf("ok path=%s name=%.8s stages=%d frames=%d score_line_shot=%d\n", path, summary.name,
                    file.stage_count, file.stages.empty() ? 0 : file.stages[0].frames, summary.shot);
        return 0;
    }
    if (command == "replay-ff" && argc >= 5) {
        // 0x448e40：与 unicorn 直跑同口径（gm / loading / mode / held / +0x20c）
        const i32 mode = i32(std::atoi(argv[2]));
        const i32 counter = i32(std::atoi(argv[3]));
        const u32 held = u32(std::strtoul(argv[4], nullptr, 0));
        const bool gm = argc < 6 || std::atoi(argv[5]) != 0;
        const bool loading = argc >= 7 && std::atoi(argv[6]) != 0;
        const ChainResult r = replay_fast_forward_result(gm, loading, mode, held, counter);
        std::printf("%d\n", i32(r));
        return 0;
    }
    if (command == "replay-fps-timing") {
        ReplayFile replay;
        ReplayStageSegment& segment = replay.stages.emplace_back();
        segment.frames = 62;
        segment.inputs.resize(size_t(segment.frames) * 6);
        segment.fps = {60, 59, 58};
        ReplayPlaybackCursor cursor;
        for (i32 frame = 0; frame < segment.frames; ++frame) {
            u32 held = 0, pressed = 0, released = 0;
            u8 fps = 0;
            if (!replay_playback_step(replay, cursor, held, pressed, released, fps)) return fail("unexpected replay end");
            std::printf("%u\n", u32(fps));
        }
        return 0;
    }
    if (command == "replay-fps-color" && argc >= 3) {
        const u8 fps = u8(std::strtoul(argv[2], nullptr, 0));
        std::printf("%u\n", replay_fps_color(fps));
        return 0;
    }
    if (command == "global-fps-color" && argc >= 3) {
        const float fps = float(std::atof(argv[2]));
        std::printf("%u\n", global_fps_color(fps));
        return 0;
    }
    if (command == "spell-time-encode" && argc >= 3) {
        // 0x417bc0 尾：秒.百分秒 → +0xa4；再经 0x426840 口径自检
        const double sec = std::atof(argv[2]);
        const i32 enc = Spellcard::encode_spell_time(sec);
        const i32 whole = i32(sec);
        const i32 hundredths = i32((sec - double(whole)) * 100.0);
        const i32 s = ((enc / 100) % 1000 + 0x3a6) % 1000;
        const i32 h = (enc % 100 + 0x43) % 100;
        const bool ok = (enc / 100000 - 0x16 == s + h) && s == whole && h == hundredths;
        std::printf("enc=%d s=%d h=%d ok=%d\n", enc, s, h, ok ? 1 : 0);
        return ok ? 0 : 1;
    }
    if (command == "sound-script" && argc >= 3) {
        // 音效请求队列（tools/oracle/sound-oracle.py 同口径）：ops 逐条执行，假缓冲记录每次方法调用
        struct LogOutput final : SoundOutput {
            std::vector<bool> missing = std::vector<bool>(kSoundVoiceCount, false);
            std::vector<bool> on = std::vector<bool>(kSoundVoiceCount, false);
            bool available(i32 v) const override { return !missing[size_t(v)]; }
            bool playing(i32 v) override { std::printf("%d status\n", v); return on[size_t(v)]; }
            void stop(i32 v) override { on[size_t(v)] = false; std::printf("%d stop\n", v); }
            void set_position(i32 v, u32 b) override { std::printf("%d position %d\n", v, i32(b)); }
            void set_pan(i32 v, i32 pan) override { std::printf("%d pan %d\n", v, pan); }
            void set_volume(i32 v, i32 vol) override { std::printf("%d volume %d\n", v, vol); }
            void play(i32 v, u32 flags) override { on[size_t(v)] = true; std::printf("%d play %u\n", v, flags); }
        } out;
        SoundEffects sound;
        sound.output = &out;
        for (int a = 3; a < argc; ++a) {
            if (std::strncmp(argv[a], "master=", 7) == 0) sound.master_volume = std::atoi(argv[a] + 7);
            else if (std::strcmp(argv[a], "disabled") == 0) sound.enabled = false;
            else if (std::strncmp(argv[a], "missing=", 8) == 0) {
                for (const char* p = argv[a] + 8; *p != '\0';) {
                    out.missing[size_t(std::atoi(p))] = true;
                    while (*p != '\0' && *p != ',') ++p;
                    if (*p == ',') ++p;
                }
            }
        }
        std::string ops = argv[2];
        size_t at = 0;
        while (at <= ops.size()) {
            const size_t end = std::min(ops.find(',', at), ops.size());
            const std::string op = ops.substr(at, end - at);
            at = end + 1;
            if (op.empty()) continue;
            const char kind = op[0];
            const char* rest = op.size() > 2 ? op.c_str() + 2 : "";
            if (kind == 'p') sound.play(std::atoi(rest));
            else if (kind == 'a') {
                const i32 id = std::atoi(rest);
                const char* colon = std::strchr(rest, ':');
                sound.play_at(id, colon != nullptr ? float(std::atof(colon + 1)) : 0.0f);
            } else if (kind == 's') sound.stop(std::atoi(rest));
            else if (kind == 'P') sound.process();
            else if (kind == 'S') sound.suspend();
            else if (kind == 'R') sound.resume();
            else return fail("unknown sound op");
            std::printf("-- %s\n", op.c_str());
        }
        return 0;
    }
    if (command == "spell-time-replay" && argc >= 3) {
        // 0x417bc0：记录局把 +0xa4 写进段头槽（按 +0x88 计次），回放局读回；校验 0x417530 不符 → 0x6ad1584
        const double sec = std::atof(argv[2]);
        std::vector<i32> slots;
        auto run = [&](bool playback, i32 tamper) {
            Spellcard s;
            s.replay_playback = playback;
            s.record_time = [&](i32 index, i32 value) { if (i32(slots.size()) <= index) slots.resize(size_t(index) + 1); slots[size_t(index)] = value; };
            s.replay_time = [&](i32 index) { return index < i32(slots.size()) ? slots[size_t(index)] + tamper : 0; };
            for (int k = 0; k < 2; ++k) {
                s.flags = Spellcard::kActive;
                s.wall_clock_seconds = 10.0 + k * 100.0;
                s.tick_realtime();
                s.flags &= ~Spellcard::kActive;
                s.wall_clock_seconds = 10.0 + k * 100.0 + sec * (k + 1);
                s.tick_realtime();
                std::printf("%s%s[%d]=%d count=%d\n", playback ? "play" : "rec", tamper ? "-tampered" : "", k, s.time_record, s.capture_count);
            }
        };
        run(false, 0);
        run(true, 0);
        run(true, 1);
        return 0;
    }
    if (command == "frame-timer" && argc >= 2) {
        // 合成墙钟：每逻辑帧 +1/60，跑满 ≥1s 后打印 fps / slow
        FrameTimer timer;
        const i32 frames = argc >= 3 ? std::atoi(argv[2]) : 120;
        for (i32 i = 0; i < frames; ++i) {
            timer.tick(double(i) / 60.0, 0, true, false);
        }
        std::printf("fps=%.3f slow=%.3f window=%d good=%.1f total=%.1f\n", double(timer.fps),
                    double(timer.slow_rate()), timer.window_frames, timer.elapsed_good, timer.elapsed_total);
        return 0;
    }
    if (command == "pad-input-bits") {
        // 0x4018e0：默认表 {0,1,2,5,...,3} 下按钮位图 → 输入位
        GameGlobals g;
        std::printf("b0=%x b1=%x b2=%x b5=%x b3=%x none=%x\n", g.pad_input_bits(1u << 0), g.pad_input_bits(1u << 1),
                    g.pad_input_bits(1u << 2), g.pad_input_bits(1u << 5), g.pad_input_bits(1u << 3), g.pad_input_bits(1u << 4));
        g.pad_buttons[0] = -1;
        std::printf("unbound=%x\n", g.pad_input_bits(1u << 0));
        return (GameGlobals().pad_input_bits(1u << 3) == InputState::kRelease && g.pad_input_bits(1u << 0) == 0) ? 0 : 1;
    }
    if (command == "config-file" && argc >= 3) {
        // 0x43c050：读 → 校验 → 回写；参数为 th16.cfg 路径（不存在则落默认并创建）
        GameSession session;
        session.config_path = argv[2];
        const bool valid = session.load_config();
        const GameGlobals& g = session.globals;
        std::printf("valid=%d bgm=%u se=%u opt=%u input=%x win=%u bgm_mode=%u se_on=%u res=%u skip=%u pad=", valid ? 1 : 0,
                    unsigned(g.bgm_volume), unsigned(g.se_volume), unsigned(g.option_flags), g.input_config,
                    unsigned(g.window_mode), unsigned(g.bgm_mode), unsigned(g.se_enabled), unsigned(g.resolution),
                    unsigned(g.frame_skip));
        for (int i = 0; i < GameGlobals::kPadButtonCount; ++i) std::printf("%s%d", i ? "," : "", int(g.pad_buttons[i]));
        std::printf("\n");
        u8 image[config::kSize];
        config::encode(g, image);
        std::printf("image=");
        for (u8 b : image) std::printf("%02x", b);
        std::printf("\n");
        return 0;
    }
    if (command == "screen-fade" && argc >= 6) {
        // kind 4 / 6 / 7 / 9 更新函数对拍：每行 frame alpha alive
        const i32 kind = i32(std::atoi(argv[2]));
        const i32 duration = i32(std::atoi(argv[3]));
        const u32 color = u32(std::strtoul(argv[4], nullptr, 0));
        const int frames = std::atoi(argv[5]);
        const i32 extra = argc >= 7 ? i32(std::atoi(argv[6])) : (kind == 4 ? 1 : 0);
        ScreenFade fade;
        fade.kind = kind;
        fade.duration = duration;
        fade.timer.reset();
        if (kind == 4) {
            fade.color = u32(extra);
            fade.color_aux = (color & 0xffffffu) | 0xff000000u;
            fade.alpha = 0xff;
        } else if (kind == 6 || kind == 7) {
            fade.color = color;
            fade.mode_flag = extra;
        } else if (kind == 9) {
            fade.color = color;
            fade.alpha = 0xff;
        } else {
            return fail("screen-fade: kind must be 4/6/7/9");
        }
        for (int f = 0; f < frames; ++f) {
            bool alive = true;
            if (kind == 4) alive = screen_fade_from_aux_step(fade, 0, 1.0f);
            else if (kind == 6 || kind == 7) alive = screen_fade_half_step(fade, 1.0f);
            else alive = screen_fade_hold_step(fade, 0, 1.0f);
            std::printf("%d %d %d\n", f, fade.alpha, alive ? 1 : 0);
            if (!alive) break;
        }
        return 0;
    }
    if (command == "chain-self-remove") {
        ChainManager chains;
        ChainEntry* self = nullptr;
        auto lifetime = std::make_shared<int>(0);
        const std::weak_ptr<int> observer = lifetime;
        std::string trace;
        bool alive_after_remove = false;
        self = chains.create([&, lifetime, calls = 0]() mutable {
            trace += char('0' + ++calls);
            if (calls == 2) {
                chains.remove(self);
                alive_after_remove = !observer.expired();
                ++*lifetime;
            }
            return ChainResult::Continue;
        }, 1, true);
        lifetime.reset();
        ChainEntry* next = chains.create([&]() {
            trace += 'F';
            return ChainResult::Continue;
        }, 2, true);
        chains.enable(self);
        chains.enable(next);
        chains.run_update();
        chains.run_update();
        std::printf("trace=%s alive=%d released=%d count=%u\n", trace.c_str(),
                    alive_after_remove, observer.expired(), chains.update_count());
        return 0;
    }
    if (command == "chain-demo") {
        // ChainManager 语义自检：优先级排序、启用位、Remove/Restart 返回码（0x401460 走法）
        ChainManager chains;
        std::string trace;
        int restarts = 0;
        ChainEntry* a = chains.create([&]() { trace += "A"; return ChainResult::Continue; }, 5, true);
        ChainEntry* b = chains.create([&]() { trace += "B"; return ChainResult::Remove; }, 1, true);
        ChainEntry* c = chains.create([&]() {
            trace += "C";
            return restarts++ == 0 ? ChainResult::Restart : ChainResult::Continue;
        }, 3, true);
        ChainEntry* d = chains.create([&]() { trace += "D"; return ChainResult::Continue; }, 7, true);
        chains.enable(a); chains.enable(b); chains.enable(c);  // d 未启用
        const i32 r1 = chains.run_update();
        trace += "|";
        const i32 r2 = chains.run_update();
        chains.enable(d);
        chains.disable(a);
        trace += "|";
        const i32 r3 = chains.run_update();
        std::printf("%s r=%d,%d,%d count=%u\n", trace.c_str(), r1, r2, r3, chains.update_count());
        return 0;
    }
    if (command == "frame-clock") {
        // FrameClock：按给定墙钟序列（秒）输出每次应执行的逻辑步数
        FrameClock clock;
        std::string out;
        for (int a = 2; a < argc; ++a) {
            const u32 steps = clock.begin_frame(std::atof(argv[a]));
            out += std::to_string(steps);
            if (a + 1 < argc) out += ",";
        }
        std::printf("%s\n", out.c_str());
        return 0;
    }
    if (command == "input-edges") {
        // 0x405580：held 序列 → 每帧 "pressed repeat released"
        InputState in;
        for (int a = 2; a < argc; ++a) {
            in.held = u32(std::strtoul(argv[a], nullptr, 0));
            in.commit();
            std::printf("%x %x %x\n", in.pressed, in.repeat, in.released);
        }
        return 0;
    }
    if (command == "score-tool" && argc >= 6) {
        // tools/oracle/score-oracle.py 同口径：RNG 种子 0 → 0x4497e0（argv[2] 为 '-' 则无文件）→ 镜像；
        // 按 OFF:HEX（相对成绩对象基址）改镜像 → 0x449a00 写出 → 写盘后镜像
        auto write_bytes = [](const char* path, const void* p, size_t n) {
            FILE* f = std::fopen(path, "wb");
            if (f == nullptr) return false;
            std::fwrite(p, 1, n, f);
            std::fclose(f);
            return true;
        };
        constexpr size_t kImage = sizeof(ScoreBlock) * ScoreFile::kShots + sizeof(ScoreStats);
        auto scores = std::make_unique<ScoreFile>();
        Rng rng;
        for (u16& n : scores->stats.noise) n = rng.next16();
        if (std::strcmp(argv[2], "-") != 0) {
            std::vector<u8> bytes;
            if (!read_file(argv[2], bytes)) return fail("cannot read score file");
            scores->load_image(bytes.data(), u32(bytes.size()));
        }
        u8* image = reinterpret_cast<u8*>(scores->blocks);
        if (!write_bytes(argv[3], image, kImage)) return fail("cannot write image");
        for (int a = 6; a < argc; ++a) {
            if (std::strcmp(argv[a], "unlock-all") == 0) {
                scores->unlock_all();
                continue;
            }
            const char* colon = std::strchr(argv[a], ':');
            if (colon == nullptr) return fail("bad poke");
            const size_t off = std::strtoul(argv[a], nullptr, 16);
            const std::string hex(colon + 1);
            if (off < 8 || off - 8 + hex.size() / 2 > kImage) return fail("poke out of range");
            for (size_t i = 0; i + 1 < hex.size(); i += 2)
                image[off - 8 + i / 2] = u8(std::strtoul(hex.substr(i, 2).c_str(), nullptr, 16));
        }
        const std::vector<u8> saved = scores->save_image();
        if (!write_bytes(argv[4], saved.data(), saved.size())) return fail("cannot write saved");
        if (!write_bytes(argv[5], image, kImage)) return fail("cannot write image");
        std::printf("rng_seed=%u saved=%zu\n", rng.seed, saved.size());
        return 0;
    }
    std::vector<u8> dat;
    if (argc < 3 || !read_file(argv[2], dat)) return fail("cannot read archive");
    Archive archive;
    if (!archive.open(dat.data(), u32(dat.size()))) return fail(archive.error.c_str());
    if (command == "music-script" && argc >= 4) {
        // 乐曲命令队列（tools/oracle/music-oracle.py 同口径）：ops 逐条执行，假串流器记录每次调用
        struct LogOutput final : MusicOutput {
            const MusicManager* music = nullptr;
            i32 index(const BgmEntry& e) const { return music->find_track(e.name); }
            void stop(bool close_file) override { std::printf("stop %d\n", close_file ? 1 : 0); }
            void open(const BgmEntry& e) override { std::printf("open %d\n", index(e)); }
            void seek(const BgmEntry& e, u32 offset) override { std::printf("seek %d %u\n", index(e), offset); }
            void play(bool looped, u32 offset) override { std::printf("fill %u\nplay 0 %d %u\n", offset, looped ? 1 : 0, offset); }
            void start() override { std::printf("start\n"); }
            void pause() override { std::printf("pause\n"); }
            void resume() override { std::printf("resume\n"); }
            void halt() override { std::printf("halt\n"); }
            void fade(i32 frames) override { std::printf("fade %d\n", frames); }
            void set_volume(i32 v) override { std::printf("volume %d\n", v); }
            void switch_loop(const BgmEntry& e) override { std::printf("loop %d\n", index(e)); }
            double position_seconds() override { return 0.0; }
            void restart_at(double) override {}
        } out;
        MusicManager music;
        std::vector<u8> fmt;
        if (!archive.read("thbgm.fmt", fmt)) return fail(archive.error.c_str());
        if (!music.load_table(fmt.data(), fmt.size())) return fail("thbgm.fmt empty");
        out.music = &music;
        music.output = &out;
        for (int a = 4; a < argc; ++a)
            if (std::strcmp(argv[a], "disabled") == 0) music.enabled = false;
        std::string ops = argv[3];
        size_t at = 0;
        while (at <= ops.size()) {
            const size_t end = std::min(ops.find(',', at), ops.size());
            const std::string op = ops.substr(at, end - at);
            at = end + 1;
            if (op.empty()) continue;
            const char kind = op[0];
            if (kind == 'q') {
                // q:<kind>:<track>:<name>
                const size_t c1 = op.find(':', 2), c2 = c1 == std::string::npos ? c1 : op.find(':', c1 + 1);
                if (c1 == std::string::npos || c2 == std::string::npos) return fail("bad q op");
                music.enqueue(std::atoi(op.c_str() + 2), std::atoi(op.c_str() + c1 + 1), op.c_str() + c2 + 1);
            } else if (kind == 'P') {
                std::printf("ret %d\n", music.step());
            } else if (kind == 'D') {
                for (int k = 0; k < 64; ++k) {
                    const i32 ret = music.step();
                    std::printf("ret %d\n", ret);
                    if (ret == 0) break;
                }
            } else if (kind == 'F') {
                // 串流器刷新锁（+0x9c）在端口的单线程里恒为 0：无操作
            } else if (kind == 'T') {
                music.tick_fade();
            } else if (kind == 'c') {
                std::printf("current %s\n", music.current.empty() ? "-" : music.current.c_str());
            } else return fail("unknown music op");
            std::printf("-- %s\n", op.c_str());
        }
        return 0;
    }
    if (command == "archive-list") {
        std::vector<u8> bytes;
        for (u32 i = 0; i < archive.entries.size(); ++i) {
            if (!archive.read(i, bytes)) return fail(archive.error.c_str());
            std::printf("%s %zu %016llx\n", archive.entries[i].name.c_str(), bytes.size(), (unsigned long long)fnv1a64(bytes.data(), bytes.size()));
        }
        return 0;
    }
    if (command == "archive-extract" && argc >= 5) {
        std::vector<u8> bytes;
        if (!archive.read(argv[3], bytes)) return fail(archive.error.c_str());
        FILE* f = std::fopen(argv[4], "wb");
        if (!f) return fail("cannot write output");
        std::fwrite(bytes.data(), 1, bytes.size(), f);
        std::fclose(f);
        return 0;
    }
    if (command == "std" && argc >= 4) {
        // STD 解析（0x40ac30）：头 / 对象 / 四边形 / 实例 / 脚本
        std::vector<u8> bytes;
        if (!archive.read(argv[3], bytes)) return fail(archive.error.c_str());
        StdFile std_file;
        if (!std_file.parse(bytes.data(), u32(bytes.size()))) return fail(std_file.error.c_str());
        std::printf("anm=%s faces=%d objects=%zu instances=%zu script=%zu\n", std_file.anm_name.c_str(), std_file.face_vm_count,
                    std_file.objects.size(), std_file.instances.size(), std_file.script.size());
        for (const StdObject& o : std_file.objects) {
            std::printf("object %u layer=%u pos=%.6g,%.6g,%.6g size=%.6g,%.6g,%.6g quads=%zu\n", o.id, o.layer,
                        o.pos.x, o.pos.y, o.pos.z, o.size.x, o.size.y, o.size.z, o.quads.size());
            for (const StdQuad& q : o.quads)
                std::printf("  quad type=%d script=%d pos=%.6g,%.6g,%.6g wh=%.6g,%.6g\n", q.type, q.anm_script, q.pos.x, q.pos.y, q.pos.z, q.width, q.height);
        }
        for (const StdInstance& i : std_file.instances)
            std::printf("instance %u flags=%u pos=%.6g,%.6g,%.6g\n", i.object, i.flags, i.pos.x, i.pos.y, i.pos.z);
        for (const StdInstruction& ins : std_file.script) {
            std::printf("ins @%u t=%d op=%d", ins.offset, ins.time, ins.opcode);
            for (int n = 0; n < (ins.size - 8) / 4; ++n) std::printf(" %d/%.6g", ins.arg_i(n), ins.arg_f(n));
            std::printf("\n");
        }
        return 0;
    }
    if ((command == "stage-run" || command == "stage-digest") && argc >= 5) {
        // stage-digest：同一状态流按 tools/oracle/stage-oracle.py 的规范字节序列做 FNV-1a 64，供 golden 对照
        const bool digest_only = command == "stage-digest";
        // StageManager 单独驱动（0x409db0 → 0x40add0 → 每帧 0x409e50），逐帧导出相机/脚本/雾/面 VM/层精灵 VM，
        // 与 tools/oracle/stage-oracle.py（Unicorn 直跑原版）对照
        const int stage = std::atoi(argv[3]);
        const int frames = std::atoi(argv[4]);
        const StageEntry& entry = stage_entry(stage);
        std::vector<u8> std_bytes;
        if (!archive.read(entry.std_name, std_bytes)) return fail(archive.error.c_str());
        StageManager st;
        if (!st.load(std_bytes.data(), u32(std_bytes.size()), stage)) return fail(st.error.c_str());
        std::vector<u8> anm_bytes;
        if (!archive.read(st.std.anm_name.c_str(), anm_bytes)) return fail(archive.error.c_str());
        AnmResource anm;
        if (!anm.parse(anm_bytes.data(), u32(anm_bytes.size()), st.std.anm_name.c_str())) return fail(anm.error.c_str());
        // 原版启动时 text.anm 已在槽 0：全零层精灵 VM（file_slot 0 / script 0）每帧会推进它的脚本 0
        std::vector<u8> text_bytes;
        AnmResource text;
        if (!archive.read("text.anm", text_bytes) || !text.parse(text_bytes.data(), u32(text_bytes.size()), "text.anm")) return fail("cannot load text.anm");
        AnmManager manager;
        manager.load(0, text);
        st.anm = manager.load(3 + (stage & 1), anm);
        if (st.anm == nullptr) return fail("stage anm load failed");
        st.start(manager);
        constexpr size_t kSnapshotBytes = 0x568;
        auto hex_vm = [&](const AnmVm& vm) {
            const u8* raw = reinterpret_cast<const u8*>(&vm);
            for (size_t i = 0; i < kSnapshotBytes; ++i) std::printf("%02x", raw[i]);
        };
        auto f3 = [&](const Vec3& v) { std::printf("[%u,%u,%u]", bits(v.x), bits(v.y), bits(v.z)); };
        // 规范字节流（小端 u32）：frame, timer.current, timer.current_f, ins, rocking_mode, rocking_timer, stage_frame,
        // flags, bg, cam[19]（pos facing up facing_norm pos_offset facing_offset fov）, fog[7], layer_ids[8], radius_sq,
        // 然后面 VM × 0x568 字节、层精灵 VM 8 × 0x568 字节
        u64 digest = 0xcbf29ce484222325ull;
        auto feed = [&](const u8* p, size_t n) { for (size_t i = 0; i < n; ++i) { digest ^= p[i]; digest *= 0x100000001b3ull; } };
        auto feed_u32 = [&](u32 v) { u8 b[4] = {u8(v), u8(v >> 8), u8(v >> 16), u8(v >> 24)}; feed(b, 4); };
        if (!digest_only) std::printf("{\"stage\":%d,\"std\":\"%s\",\"anm\":\"%s\",\"frames\":[", stage, entry.std_name, st.std.anm_name.c_str());
        for (int f = 0; f < frames; ++f) {
            st.update(manager, 1.0f);
            const StageCamera& c = st.camera;
            const i32 ins_offset = (st.ins_index >= 0 && size_t(st.ins_index) < st.std.script.size()) ? i32(st.std.script[size_t(st.ins_index)].offset) : -1;
            const u32 cam[19] = {bits(c.pos.x), bits(c.pos.y), bits(c.pos.z), bits(c.facing.x), bits(c.facing.y), bits(c.facing.z),
                                 bits(c.up.x), bits(c.up.y), bits(c.up.z), bits(c.facing_norm.x), bits(c.facing_norm.y), bits(c.facing_norm.z),
                                 bits(c.pos_offset.x), bits(c.pos_offset.y), bits(c.pos_offset.z),
                                 bits(c.facing_offset.x), bits(c.facing_offset.y), bits(c.facing_offset.z), bits(c.fov)};
            const u32 fog[7] = {bits(c.fog.near_), bits(c.fog.far_), bits(c.fog.c[0]), bits(c.fog.c[1]), bits(c.fog.c[2]), bits(c.fog.c[3]), c.fog.color};
            const u32 head[9] = {u32(f + 1), u32(st.timer.current), bits(st.timer.current_f), u32(ins_offset), u32(st.rocking_mode),
                                 u32(st.rocking_timer.current), u32(st.frame), st.flags, st.bg_color};
            for (u32 v : head) feed_u32(v);
            for (u32 v : cam) feed_u32(v);
            for (u32 v : fog) feed_u32(v);
            for (int i = 0; i < StageManager::kLayerSprites; ++i) feed_u32(u32(st.layer_ids[i]));
            feed_u32(bits(st.cull_distance_sq));
            for (const AnmVm& vm : st.face_vms) feed(reinterpret_cast<const u8*>(&vm), kSnapshotBytes);
            for (const AnmVm& vm : st.layer_vms) feed(reinterpret_cast<const u8*>(&vm), kSnapshotBytes);
            if (digest_only) continue;
            std::printf("%s{\"frame\":%d,\"timer\":[%d,%u],\"ins\":%d,\"rocking\":[%d,%d],\"stage_frame\":%d,\"flags\":%u,\"bg\":%u,",
                        f == 0 ? "" : ",", f + 1, st.timer.current, bits(st.timer.current_f), ins_offset, st.rocking_mode, st.rocking_timer.current,
                        st.frame, st.flags, st.bg_color);
            std::printf("\"cam\":{\"pos\":"); f3(c.pos);
            std::printf(",\"facing\":"); f3(c.facing);
            std::printf(",\"up\":"); f3(c.up);
            std::printf(",\"facing_norm\":"); f3(c.facing_norm);
            std::printf(",\"pos_offset\":"); f3(c.pos_offset);
            std::printf(",\"facing_offset\":"); f3(c.facing_offset);
            std::printf(",\"fov\":%u},", bits(c.fov));
            std::printf("\"fog\":[%u,%u,%u,%u,%u,%u,%u],", fog[0], fog[1], fog[2], fog[3], fog[4], fog[5], fog[6]);
            std::printf("\"layer_ids\":[");
            for (int i = 0; i < StageManager::kLayerSprites; ++i) std::printf("%s%d", i ? "," : "", st.layer_ids[i]);
            std::printf("],\"radius_sq\":%u,\"faces\":[", bits(st.cull_distance_sq));
            for (size_t i = 0; i < st.face_vms.size(); ++i) { std::printf("%s\"", i ? "," : ""); hex_vm(st.face_vms[i]); std::printf("\""); }
            std::printf("],\"layers\":[");
            for (int i = 0; i < StageManager::kLayerSprites; ++i) { std::printf("%s\"", i ? "," : ""); hex_vm(st.layer_vms[i]); std::printf("\""); }
            std::printf("]}");
        }
        if (digest_only) std::printf("stage=%d frames=%d faces=%zu digest=%016llx\n", stage, frames, st.face_vms.size(), (unsigned long long)digest);
        else std::printf("]}\n");
        return 0;
    }
    if (command == "anm" && argc >= 4) {
        std::vector<u8> bytes;
        if (!archive.read(argv[3], bytes)) return fail(archive.error.c_str());
        AnmResource anm;
        if (!anm.parse(bytes.data(), u32(bytes.size()), argv[3])) return fail(anm.error.c_str());
        for (const auto& e : anm.entries) {
            std::printf("%s %u %u %u %zu %zu", e.name.c_str(), e.width, e.height, e.format, e.sprites.size(), e.scripts.size());
            if (e.texture.present())
                std::printf(" thtx:%u:%ux%u:%016llx", e.texture.format, e.texture.width, e.texture.height,
                            (unsigned long long)fnv1a64(e.texture.bgra.data(), e.texture.bgra.size()));
            else std::printf(" thtx:none");
            std::printf("\n");
            for (const auto& s : e.scripts) {
                u32 instructions = 0, sentinel = 0;
                for (const auto& ins : s.instructions) { ++instructions; if (ins.type == 0xffff) ++sentinel; }
                std::printf("  script %d @%u bytes=%zu ins=%u sentinel=%u\n", s.id, s.offset, s.bytes.size(), instructions, sentinel);
            }
        }
        return 0;
    }
    if (command == "anm-script" && argc >= 5) {
        // 反汇编一个 ANM 脚本：每行 time type mask | 参数（十六进制 / i32 / f32）
        std::vector<u8> bytes;
        if (!archive.read(argv[3], bytes)) return fail(archive.error.c_str());
        AnmResource anm;
        if (!anm.parse(bytes.data(), u32(bytes.size()), argv[3])) return fail(anm.error.c_str());
        const int want = std::atoi(argv[4]);
        for (const auto& e : anm.entries) {
            for (const auto& s : e.scripts) {
                if (s.id != want) continue;
                std::printf("%s script %d\n", e.name.c_str(), s.id);
                for (const auto& ins : s.instructions) {
                    std::printf("  +%-5u t=%-5d op=%-4u mask=%04x |", ins.offset, ins.time, ins.type, ins.param_mask);
                    for (u32 i = 0; i + 4 <= ins.arg_bytes; i += 4) {
                        u32 v; float fv;
                        std::memcpy(&v, ins.args + i, 4);
                        std::memcpy(&fv, ins.args + i, 4);
                        std::printf(" %08x(%d/%g)", v, i32(v), fv);
                    }
                    std::printf("\n");
                }
            }
        }
        return 0;
    }
    if (command == "anm-run" && argc >= 6) {
        std::vector<u8> bytes;
        if (!archive.read(argv[3], bytes)) return fail(archive.error.c_str());
        AnmResource anm;
        if (!anm.parse(bytes.data(), u32(bytes.size()), argv[3])) return fail(anm.error.c_str());
        const int frames = std::atoi(argv[5]);
        const int layer = argc >= 7 ? std::atoi(argv[6]) : 0;
        // 原版启动时 AsciiManager（0x407cb0）先把 ascii.anm 装入槽 2；ins_300 负下标回退到它的 0x102 号精灵
        std::vector<u8> ascii_bytes;
        AnmResource ascii;
        if (!archive.read("ascii.anm", ascii_bytes) || !ascii.parse(ascii_bytes.data(), u32(ascii_bytes.size()), "ascii.anm"))
            return fail("cannot load ascii.anm");
        // 脚本选择："all" / "3" / "0-20,40"（与 anm-oracle.py 同语法）
        std::vector<int> scripts;
        {
            const std::string spec = argv[4];
            if (spec == "all") {
                for (int i = 0; i < int(anm.total_scripts); ++i) scripts.push_back(i);
            } else {
                size_t pos = 0;
                while (pos < spec.size()) {
                    size_t comma = spec.find(',', pos);
                    if (comma == std::string::npos) comma = spec.size();
                    const std::string part = spec.substr(pos, comma - pos);
                    const size_t dash = part.find('-');
                    if (dash == std::string::npos) scripts.push_back(std::atoi(part.c_str()));
                    else for (int i = std::atoi(part.substr(0, dash).c_str()); i <= std::atoi(part.substr(dash + 1).c_str()); ++i) scripts.push_back(i);
                    pos = comma + 1;
                }
            }
        }
        // 原版快照只含 0x568 之前的纯数据区（之后为链表指针，宿主地址无意义）
        constexpr size_t kSnapshotBytes = 0x568;
        std::printf("{\"anm\":\"%s\",\"layer\":%d,\"frames\":%d,\"runs\":[", argv[3], layer, frames);
        bool first_run = true;
        for (const int script : scripts) {
            // 每个脚本都从同一装载完成状态开始（oracle 以内存检查点实现；这里重建管理器）
            AnmManager manager;
            manager.load(AnmManager::kAsciiSlot, ascii);
            AnmFile* file = manager.load(0, anm);
            if (file == nullptr || script < 0 || size_t(script) >= file->scripts.size()) return fail("script out of range");
            const u32 spawned = manager.spawn(*file, script, layer);
            auto snapshot = [&](int frame) {
                std::printf("%s{\"frame\":%d,\"vms\":[", frame == 0 ? "" : ",", frame);
                bool first = true;
                for (const AnmListNode* n = manager.world_head(); n != nullptr; n = n->next) {
                    const AnmVm& vm = *n->owner;
                    std::printf("%s{\"id\":%u,\"script\":%d,\"bytes\":\"", first ? "" : ",", vm.id, vm.script_index);
                    const u8* raw = reinterpret_cast<const u8*>(&vm);
                    for (size_t i = 0; i < kSnapshotBytes; ++i) std::printf("%02x", raw[i]);
                    std::printf("\"}");
                    first = false;
                }
                // 世界表推进（0x46e340）只重建层 0..0x23；UI 层 0x24..0x2a 由 tick_ui 维护，不在此口径内
                std::printf("],\"layers\":[");
                for (int l = 0; l < AnmManager::kWorldLayerCount; ++l) {
                    std::printf("%s[", l == 0 ? "" : ",");
                    bool first_id = true;
                    for (const AnmVm* vm = manager.layer_chain(l); vm != nullptr; vm = vm->layer_next) {
                        std::printf("%s%u", first_id ? "" : ",", vm->id);
                        first_id = false;
                    }
                    std::printf("]");
                }
                std::printf("]}");
            };
            std::printf("%s{\"script\":%d,\"spawnedId\":%u,\"frames\":[", first_run ? "" : ",", script, spawned);
            first_run = false;
            snapshot(0);
            for (int f = 1; f <= frames; ++f) {
                manager.tick_world();
                snapshot(f);
            }
            std::printf("]}");
        }
        std::printf("]}\n");
        return 0;
    }
    if (command == "anm-draw" && argc >= 6) {
        std::vector<u8> bytes;
        if (!archive.read(argv[3], bytes)) return fail(archive.error.c_str());
        AnmResource anm;
        if (!anm.parse(bytes.data(), u32(bytes.size()), argv[3])) return fail(anm.error.c_str());
        std::vector<u8> ascii_bytes;
        AnmResource ascii;
        if (!archive.read("ascii.anm", ascii_bytes) || !ascii.parse(ascii_bytes.data(), u32(ascii_bytes.size()), "ascii.anm"))
            return fail("cannot load ascii.anm");
        const int script = std::atoi(argv[4]);
        const int frames = std::atoi(argv[5]);
        const int layer = argc >= 7 ? std::atoi(argv[6]) : -1;
        AnmManager manager;
        manager.load(AnmManager::kAsciiSlot, ascii);
        AnmFile* file = manager.load(0, anm);
        if (file == nullptr || script < 0 || size_t(script) >= file->scripts.size()) return fail("script out of range");
        manager.spawn(*file, script, layer);
        // 每帧 tick 后以 UI 相机把全部层（0x46e750 0..0x2a）画进录制设备，按四边形打印
        AnmGeometry geometry;
        AnmRenderer renderer(manager, geometry);
        RecordingDevice device;
        renderer.device = &device;
        Camera ui;
        ui.viewport = {0, 0, 640, 480};
        ui.kind = Camera::kUi;
        ui.reset_to_viewport_center();
        for (int f = 0; f <= frames; ++f) {
            manager.tick_world();  // spawn 后每次 tick 建/刷新 layer 链（与 Application::step 同序）
            device.begin_frame();
            renderer.reset_state_cache();   // 0x43d140 每帧清状态缓存（纹理键等），否则上一帧的键会跳过绑定
            renderer.use_camera(ui);
            renderer.begin_frame();
            for (int l = 0; l < AnmManager::kLayerCount; ++l) renderer.draw_layer(l);
            renderer.end_frame();
            for (const RecordingDevice::QuadSummary& q : device.quads()) {
                std::printf("%d ", f);
                RecordingDevice::print_quad(q);
            }
        }
        return 0;
    }
    if (command == "anm-bench" && argc >= 6) {
        // anm-bench <th16.dat> <name.anm> <script> <frames> [layer]：只推进不输出快照，末尾给 VM 数与世界链表全部 VM 的 fnv1a64
        std::vector<u8> bytes;
        if (!archive.read(argv[3], bytes)) return fail(archive.error.c_str());
        AnmResource anm;
        if (!anm.parse(bytes.data(), u32(bytes.size()), argv[3])) return fail(anm.error.c_str());
        std::vector<u8> ascii_bytes;
        AnmResource ascii;
        if (!archive.read("ascii.anm", ascii_bytes) || !ascii.parse(ascii_bytes.data(), u32(ascii_bytes.size()), "ascii.anm"))
            return fail("cannot load ascii.anm");
        const int script = std::atoi(argv[4]);
        const int frames = std::atoi(argv[5]);
        const int layer = argc >= 7 ? std::atoi(argv[6]) : 0;
        AnmManager manager;
        manager.load(AnmManager::kAsciiSlot, ascii);
        AnmFile* file = manager.load(0, anm);
        if (file == nullptr || script < 0 || size_t(script) >= file->scripts.size()) return fail("script out of range");
        manager.spawn(*file, script, layer);
        for (int f = 0; f < frames; ++f) manager.tick_world();
        u64 digest = 0xcbf29ce484222325ull;
        u32 count = 0;
        for (const AnmListNode* n = manager.world_head(); n != nullptr; n = n->next) {
            const u8* raw = reinterpret_cast<const u8*>(n->owner);
            for (size_t i = 0; i < 0x568; ++i) { digest ^= raw[i]; digest *= 0x100000001b3ull; }
            ++count;
        }
        std::printf("%d %u %016llx\n", frames, count, (unsigned long long)digest);
        return 0;
    }
    if (command == "anm-texture" && argc >= 6) {
        // anm-texture <th16.dat> <name.anm> <entry-index> <out.bgra>：解码后的 BGRA8 像素（与 thanm -x 的 PNG 对照）
        std::vector<u8> bytes;
        if (!archive.read(argv[3], bytes)) return fail(archive.error.c_str());
        AnmResource anm;
        if (!anm.parse(bytes.data(), u32(bytes.size()), argv[3])) return fail(anm.error.c_str());
        const size_t index = size_t(std::atoi(argv[4]));
        if (index >= anm.entries.size() || !anm.entries[index].texture.present()) return fail("entry has no texture");
        const AnmTexture& t = anm.entries[index].texture;
        FILE* f = std::fopen(argv[5], "wb");
        if (!f) return fail("cannot write output");
        std::fwrite(t.bgra.data(), 1, t.bgra.size(), f);
        std::fclose(f);
        std::printf("%u %u %u %u %u\n", t.format, t.width, t.height, anm.entries[index].x, anm.entries[index].y);
        return 0;
    }
    if ((command == "game-run" || command == "game-digest" || command == "game-draw") && argc >= 8) {
        // 直接进入场景 7（跳过启动幕/标题），每帧注入 held 位并按 tools/oracle/game-oracle.py 的规范字段导出 Player 状态。
        // 参数：<th16.dat> <stage> <shot> <season> <frames> <input "f:held,..."> [enemies "f:count,..."] [dialogue "f:v,..."]
        // 输入脚本语义与 game-oracle --input 相同：第 f 帧（0 起）起 held 持续到下一事件；"@<路径>" 从文件读入。
        // enemies/dialogue 为其它管理器未移植时的 PlayerWorld 外部输入（oracle 快照 enemy_count / dialogue 字段）。
        const bool digest_only = command == "game-digest";
        // game-draw：与 game-run 同参，另加第 16/17 参 <out.jsonl> [from_frame]；跑绘制链并把每帧设备调用写成 JSON 行。
        // <out.jsonl> 的两种写法：
        //   "digest:<dst>"  每帧一行整段摘要（DrawDigest.hpp，tools/oracle/draw-compare.py frame_digests 同口径；tests/draw-long.test.mjs）
        //   "fd:<n>"        直接使用继承来的文件描述符（两种行格式都可，经管道流式读，整段不落盘）
        const bool draw_mode = command == "game-draw";
        std::FILE* draw_out = nullptr;
        bool digest_mode = false;
        int draw_from = 1;
        if (draw_mode) {
            if (argc < 16) return fail("game-draw needs <out.jsonl>");
            const char* destination = argv[15];
            if (std::strncmp(destination, "digest:", 7) == 0) {
                digest_mode = true;
                destination += 7;
            }
#if defined(__wasi__)
            draw_out = std::fopen(destination, "w");
#else
            if (std::strncmp(destination, "fd:", 3) == 0) draw_out = fdopen(std::atoi(destination + 3), "w");
            else draw_out = std::fopen(destination, "w");
#endif
            if (draw_out == nullptr) return fail("cannot open draw output");
            if (argc >= 17) draw_from = std::atoi(argv[16]);
        }
        const int stage = std::atoi(argv[3]);
        const int shot = std::atoi(argv[4]);
        const int season = std::atoi(argv[5]);
        const int frames = std::atoi(argv[6]);
        struct Event { int frame; i32 value; };
        auto parse_schedule = [&](const char* spec, std::vector<Event>& out) -> bool {
            // "@<路径>"：脚本从文件读入（长通关脚本会超过命令行单参数的长度上限），首尾空白忽略
            std::string text;
            const char* p = spec;
            if (*p == '@') {
                std::vector<u8> bytes;
                if (!read_file(p + 1, bytes)) return false;
                while (!bytes.empty() && (bytes.back() == '\n' || bytes.back() == '\r' || bytes.back() == ' ' || bytes.back() == '\t'))
                    bytes.pop_back();
                text.assign(bytes.begin(), bytes.end());
                p = text.c_str();
            }
            while (*p) {
                char* endp = nullptr;
                const long frame = std::strtol(p, &endp, 10);
                if (endp == p || *endp != ':') return false;
                p = endp + 1;
                const long v = std::strtol(p, &endp, 0);
                out.push_back({int(frame), i32(v)});
                p = endp;
                if (*p == ',') ++p;
            }
            return true;
        };
        std::vector<Event> input_script, enemy_script, dialogue_script;
        if (!parse_schedule(argv[7], input_script)) return fail("bad input script");
        if (argc >= 9 && !parse_schedule(argv[8], enemy_script)) return fail("bad enemies script");
        if (argc >= 10 && !parse_schedule(argv[9], dialogue_script)) return fail("bad dialogue script");

        GameSession session;
        if (!session.open(dat.data(), u32(dat.size()))) return fail(session.failure.c_str());
        session.run_draw_chains = draw_mode;   // 非 draw 模式与不跑 0x4015a0 的 oracle 同口径
        RecordingDevice device;
        if (draw_mode) session.attach_device(&device);
        session.globals.stage = stage;
        session.globals.stage_copy = stage;
        session.globals.shot = shot;
        session.globals.season = season;
        // argv[14] = mode（game-oracle.py --mode 同口径）：0 普通 / 1 练习 / 2 符卡练习；0x449190 普通模式默认 replay_slot = -1
        session.globals.set_mode((argc >= 15 && argv[14][0] != '\0') ? std::atoi(argv[14]) : 0);
        if (!enemy_script.empty()) {
            session.sync_enemy_count = false;
            session.globals.replay_slot = 0;  // 旧 player.json 桩在 replay_slot=0 下录制
        }
        if (argc >= 11 && argv[10][0] != '\0') session.globals.difficulty = std::atoi(argv[10]);
        if (argc >= 12 && argv[11][0] != '\0') session.globals.replay_slot = std::atoi(argv[11]);
        // TH16_DEMO=<demoN.rpy>（game-oracle.py --demo 同口径）：0x43ce10 case 0xd 标题演示——bec bit6、0x42d700(1)，
        // 机体 / 难度 / 季节 / 首段关卡取自回放（stage/shot/season 参数被覆盖）
        i32 replay_mode = 0;
        if (const char* demo = std::getenv("TH16_DEMO")) {
            if (!session.load_replay_file(demo)) return fail(session.failure.c_str());
            session.apply_replay_start(-1);
            session.globals.flags |= 0x40u;
            replay_mode = 1;
        }
        // TH16_REPLAY=<th16_XX.rpy>：标题 Replay 选槽口径（bit6 不置）；TH16_RECORD_DIR=<目录>：跑完后 0x448400 写 th16_01.rpy
        if (const char* replay = std::getenv("TH16_REPLAY")) {
            if (!session.load_replay_file(replay)) return fail(session.failure.c_str());
            session.apply_replay_start(-1);
            replay_mode = 1;
        }
        const char* record_dir = std::getenv("TH16_RECORD_DIR");
        if (record_dir != nullptr) session.save_directory = record_dir;
        if (!session.start_game_direct(replay_mode)) return fail(session.failure.c_str());
        // 可选残机覆盖（game-oracle.py --lives 同口径：装载完成后写 DAT_004a57f4），用于无输入长程对拍不被 Game Over 截断
        if (argc >= 13 && argv[12][0] != '\0') {
            session.globals.lives = std::atoi(argv[12]);
            session.restamp_replay_globals();
        }
        // refill != 0：每帧开始前把残机补回 lives（game-oracle.py --refill-lives 同口径）；原版残机显示 0x42c280 只有 8 槽，
        // 不能直接写入 > 8 的残机数，长程无输入对拍只能靠补满越过 Game Over
        const int refill_lives = (argc >= 14 && argv[13][0] != '\0' && std::atoi(argv[13]) != 0) ? session.globals.lives : -1;

        // TH16_TRACE_GAME_RNG=<frame>（1 起）：该帧每次游戏 RNG（DAT_004a6d80）调用按正在运行的 ANM VM 归因写到 stderr，
        // 与 game-oracle.py --trace-rng --trace-game-rng 的 (脚本号, 精灵号) 对照
        struct RngTraceState { GameSession* session; int frame; int current; } trace_state{&session, 0, 0};
        if (const char* env = std::getenv("TH16_TRACE_GAME_RNG")) {
            trace_state.frame = std::atoi(env);
            static RngTraceState* s_trace = nullptr;
            s_trace = &trace_state;
            g_rng_trace = [](const Rng& rng, int step) {
                if (s_trace == nullptr || s_trace->current != s_trace->frame) return;
                if (&rng != &s_trace->session->animations.game_rng) return;
                const AnmVm* vm = s_trace->session->animations.running_vm;
                std::fprintf(stderr, "rng frame=%d step=%d slot=%d script=%d sprite=%d\n", s_trace->current, step,
                             vm != nullptr ? vm->file_slot : -1, vm != nullptr ? vm->script_index : -1,
                             vm != nullptr ? vm->sprite_index : -1);
            };
        }

        // TH16_VM_DUMP=<帧>：该帧进入 AnmRenderer::draw_vm 的每个 VM 打一行身份（与 game-oracle.py --vm-dump 对照）
        const char* vm_dump_env = std::getenv("TH16_VM_DUMP");
        // 取值 "<帧>" 或 "<起帧>-<止帧>"
        int vm_dump_frame = -1, vm_dump_end = -1;
        if (vm_dump_env != nullptr) {
            if (std::sscanf(vm_dump_env, "%d-%d", &vm_dump_frame, &vm_dump_end) == 1) vm_dump_end = vm_dump_frame;
        }
        const char* dump_layers_env = std::getenv("TH16_DUMP_LAYERS");
        const int dump_layers_frame = dump_layers_env != nullptr ? std::atoi(dump_layers_env) : 0;

        constexpr size_t kSnapshotBytes = 0x568;
        u64 digest = 0xcbf29ce484222325ull;
        u64 enm_digest = 0xcbf29ce484222325ull;
        u64 bul_digest = 0xcbf29ce484222325ull;
        u64 vm_digest = 0xcbf29ce484222325ull;
        u64 item_digest = 0xcbf29ce484222325ull;
        u64 spell_digest = 0xcbf29ce484222325ull;
        u64 laser_digest = 0xcbf29ce484222325ull;
        u64 bomb_digest = 0xcbf29ce484222325ull;
        u64 menu_digest = 0xcbf29ce484222325ull;
        auto feed_menu_u32 = [&](u32 v) { for (int i = 0; i < 4; ++i) { menu_digest ^= u8(v >> (8 * i)); menu_digest *= 0x100000001b3ull; } };
        u64 sound_digest = 0xcbf29ce484222325ull;
        auto feed_sound_u32 = [&](u32 v) { for (int i = 0; i < 4; ++i) { sound_digest ^= u8(v >> (8 * i)); sound_digest *= 0x100000001b3ull; } };
        // GameMenu（game-oracle.py menu_digest 同口径）：[frame, game_flags, game_speed, continues, continues_left, 音效数, 音效…,
        // 菜单有无] + [mode, prev_mode, state, timer×3, timer2×3, cursor×4, name_cursor×3, vm×2, name_len, practice, no_rank,
        // kind, saved_sound, saved_speed, flags]
        auto menu_words = [&](std::vector<u32>& w) {
            const GameGlobals& g = session.globals;
            w.push_back(session.game.flags);
            w.push_back(bits(session.supervisor.game_speed));
            w.push_back(u32(g.continues));
            w.push_back(u32(g.continues_left));
            w.push_back(u32(session.sfx_events.ids.size()));
            for (i32 id : session.sfx_events.ids) w.push_back(u32(id));
            w.push_back(session.menu_live ? 1u : 0u);
            if (!session.menu_live) return;
            const GameMenu& m = session.menu;
            const u32 body[] = {u32(m.mode), u32(m.previous_mode), u32(m.state), u32(m.timer.previous), u32(m.timer.current),
                                bits(m.timer.current_f), u32(m.timer2.previous), u32(m.timer2.current), bits(m.timer2.current_f),
                                u32(m.cursor.index), u32(m.cursor_previous), u32(m.cursor.count), u32(m.cursor.disabled_count),
                                u32(m.name_cursor.index), u32(m.name_cursor_previous), u32(m.name_cursor.count),
                                m.menu_vm != 0 ? 1u : 0u, m.screenshot_vm != 0 ? 1u : 0u, u32(m.name_length), u32(m.practice),
                                u32(m.no_rank), u32(m.kind), u32(m.saved_sound_state), bits(m.saved_game_speed), m.flags};
            w.insert(w.end(), std::begin(body), std::end(body));
        };
        auto feed_spell_u32 = [&](u32 v) { for (int i = 0; i < 4; ++i) { spell_digest ^= u8(v >> (8 * i)); spell_digest *= 0x100000001b3ull; } };
        auto feed_laser_u32 = [&](u32 v) { for (int i = 0; i < 4; ++i) { laser_digest ^= u8(v >> (8 * i)); laser_digest *= 0x100000001b3ull; } };
        auto feed_bomb_u32 = [&](u32 v) { for (int i = 0; i < 4; ++i) { bomb_digest ^= u8(v >> (8 * i)); bomb_digest *= 0x100000001b3ull; } };
        // 炸弹 / 季节解放（game-oracle.py bomb_digest 同口径）：公共区 24 字 + 阴阳玉数 + 每枚 21 字
        auto bomb_class_index = [](const char* name) -> u32 {
            static const char* const kNames[] = {"ReimuA", "CirnoA", "AyaA", "MarisaA", "ReimuSub", "CirnoSub", "AyaSub", "MarisaSub", "AllSub"};
            for (u32 i = 0; i < 9; ++i) if (std::strcmp(kNames[i], name) == 0) return i;
            return 0xffffffffu;
        };
        auto bomb_words = [&](const Bomb& b, std::vector<u32>& w) {
            const u32 head[] = {bomb_class_index(b.class_name()), bits(b.pos.x), bits(b.pos.y), bits(b.pos.z), bits(b.sweep_speed),
                                bits(b.angle), u32(b.active), u32(b.timer.previous), u32(b.timer.current), bits(b.timer.current_f),
                                b.primary_vm != 0 ? 1u : 0u, b.secondary_vm != 0 ? 1u : 0u, u32(b.started_in_spell), u32(b.role),
                                u32(b.release_level), bits(b.release_bonus), bits(b.shown_bonus), u32(b.popup_timer.previous),
                                u32(b.popup_timer.current), bits(b.popup_timer.current_f), u32(b.popup_level),
                                bits(b.popup_pos.x), bits(b.popup_pos.y), bits(b.popup_pos.z)};
            w.insert(w.end(), std::begin(head), std::end(head));
            const ReimuBomb* reimu = b.as_reimu();
            w.push_back(reimu ? u32(reimu->orbs.size()) : 0u);
            if (reimu) for (const ReimuBomb::Orb& o : reimu->orbs) {
                const u32 orb[] = {o.vm != 0 ? 1u : 0u, bits(o.motion.pos.x), bits(o.motion.pos.y), bits(o.motion.pos.z),
                                   bits(o.motion.speed), bits(o.motion.angle), bits(o.motion.radius),
                                   bits(o.motion.velocity.x), bits(o.motion.velocity.y), bits(o.motion.velocity.z), o.motion.mode,
                                   u32(o.active), u32(o.timer.previous), u32(o.timer.current), bits(o.timer.current_f),
                                   bits(o.delta.x), bits(o.delta.y), bits(o.delta.z), o.target_id, u32(o.index), u32(o.damage_source)};
                w.insert(w.end(), std::begin(orb), std::end(orb));
            }
        };
        // 激光（game-oracle.py laser_snapshot 同口径）：公共区 33 字 + 采样点数 + 每采样点 5 字
        auto laser_words = [&](const Laser& l, std::vector<u32>& w) {
            const u32 head[] = {u32(l.kind), l.lifecycle, u32(l.state), u32(l.id), u32(l.bullet_type), u32(l.bullet_color),
                                bits(l.pos.x), bits(l.pos.y), bits(l.pos.z), bits(l.velocity.x), bits(l.velocity.y), bits(l.velocity.z),
                                bits(l.angle), bits(l.length), bits(l.width), bits(l.speed), bits(l.start_drawn_len),
                                u32(l.ex_index), l.ex_flags, u32(l.no_cancel),
                                u32(l.age_timer.previous), u32(l.age_timer.current), bits(l.age_timer.current_f),
                                u32(l.graze_timer.previous), u32(l.graze_timer.current), bits(l.graze_timer.current_f),
                                u32(l.frame_timer.previous), u32(l.frame_timer.current), bits(l.frame_timer.current_f)};
            for (u32 v : head) w.push_back(v);
            const i32 node_count = l.kind == LaserKind::Curve ? std::max(0, std::min(l.curve.node_count, 0x40)) : 0;
            w.push_back(u32(node_count));
            for (i32 i = 0; i < node_count && i < i32(l.curve_nodes.size()); ++i) {
                const CurveNode& nd = l.curve_nodes[u32(i)];
                const u32 words[] = {bits(nd.pos.x), bits(nd.pos.y), bits(nd.pos.z), bits(nd.angle), bits(nd.speed)};
                for (u32 v : words) w.push_back(v);
            }
        };
        constexpr size_t kLaserHeadWords = 29;
        auto feed = [&](const u8* p, size_t n) { for (size_t i = 0; i < n; ++i) { digest ^= p[i]; digest *= 0x100000001b3ull; } };
        auto feed_vm_u32 = [&](u32 v) { for (int i = 0; i < 4; ++i) { vm_digest ^= u8(v >> (8 * i)); vm_digest *= 0x100000001b3ull; } };
        // 敌人 ANM VM（oracle enemy_vm_snapshot 同口径）：[槽, 脚本, 精灵, entity_offset×3, pos×3, rotation.z, flags_lo, flags_hi, 年龄]
        auto enemy_vm_words = [&](const Enemy& e, std::vector<u32>& w) {
            for (int slot = 0; slot < Enemy::kVmSyncSlots; ++slot) {
                const AnmVm* vm = session.animations.find(e.vm_ids[slot]);
                if (vm == nullptr) continue;
                const u32 words[] = {u32(slot), u32(vm->script_index), u32(vm->sprite_index),
                                     bits(vm->entity_offset.x), bits(vm->entity_offset.y), bits(vm->entity_offset.z),
                                     bits(vm->pos.x), bits(vm->pos.y), bits(vm->pos.z), bits(vm->rotation.z),
                                     vm->flags_lo, vm->flags_hi, u32(vm->age.current)};
                for (u32 v : words) w.push_back(v);
            }
        };
        auto feed_u32 = [&](u32 v) { u8 b[4] = {u8(v), u8(v >> 8), u8(v >> 16), u8(v >> 24)}; feed(b, 4); };
        auto feed_enm = [&](const u8* p, size_t n) { for (size_t i = 0; i < n; ++i) { enm_digest ^= p[i]; enm_digest *= 0x100000001b3ull; } };
        auto feed_bul_u32 = [&](u32 v) { for (int i = 0; i < 4; ++i) { bul_digest ^= u8(v >> (8 * i)); bul_digest *= 0x100000001b3ull; } };
        auto feed_item_u32 = [&](u32 v) { for (int i = 0; i < 4; ++i) { item_digest ^= u8(v >> (8 * i)); item_digest *= 0x100000001b3ull; } };
        auto item_vm_color = [](const AnmVm& vm) { return u32(vm.color[0]) | (u32(vm.color[1]) << 8) | (u32(vm.color[2]) << 16) | (u32(vm.color[3]) << 24); };
        auto feed_enm_u32 = [&](u32 v) { u8 b[4] = {u8(v), u8(v >> 8), u8(v >> 16), u8(v >> 24)}; feed_enm(b, 4); };
        auto timer3 = [&](const Timer& t, std::vector<u32>& w) { w.push_back(u32(t.previous)); w.push_back(u32(t.current)); w.push_back(bits(t.current_f)); };
        auto vec3 = [&](const Vec3& v, std::vector<u32>& w) { w.push_back(bits(v.x)); w.push_back(bits(v.y)); w.push_back(bits(v.z)); };
        auto has = [](u32 id) -> u32 { return id != 0 ? 1u : 0u; };
        auto print_words = [&](const char* key, const std::vector<u32>& w, size_t from, size_t count) {
            std::printf("\"%s\":[", key);
            for (size_t i = 0; i < count; ++i) std::printf("%s%u", i ? "," : "", w[from + i]);
            std::printf("]");
        };

        if (!digest_only) std::printf("{\"stage\":%d,\"shot\":%d,\"season\":%d,\"frames\":[", stage, shot, season);
        int frames_from = 1;
        if (const char* env = std::getenv("TH16_FRAMES_FROM")) frames_from = std::atoi(env);
        bool first_printed = true;
        i32 held = 0;
        i32 enemy_hold = -1;
        i32 dialogue_hold = -1;
        // 提供了 enemies/dialogue 时间表时，由桩接管 PlayerWorld 对应字段（保持“最后一次赋值”语义）
        if (!enemy_script.empty()) session.sync_enemy_count = false;
        // 与 game-oracle.py 同口径：游戏场景结束（回标题 / 结局 / 退出）后没有 GameManager 可对拍，
        // 在该帧停止，不再记录帧；scene_exit 为切换到的场景号
        i32 scene_exit = -1;
        i32 exit_frame = 0;
        for (int f = 0; f < frames; ++f) {
            for (const Event& e : input_script) if (e.frame == f) held = e.value;
            for (const Event& e : enemy_script) if (e.frame == f) enemy_hold = e.value;
            for (const Event& e : dialogue_script) if (e.frame == f) dialogue_hold = e.value;
            if (enemy_hold >= 0) session.game.world.enemy_count = enemy_hold;
            if (dialogue_hold >= 0) session.game.world.dialogue = dialogue_hold != 0;
            session.injected_input = u32(held);
            if (refill_lives >= 0 && session.globals.lives < refill_lives) session.globals.lives = refill_lives;
            trace_state.current = f + 1;
            device.begin_frame();
            if (f + 1 >= vm_dump_frame && f + 1 <= vm_dump_end) session.renderer.trace_frame = f + 1;
            session.renderer.trace_current = f + 1;
            if (!session.logic_step()) return fail(session.failure.c_str());
            if (!session.game_live) {
                // 场景在本步切换（exit_frame）：这一帧画的是新场景的首帧，而 oracle 在同一帧只记录请求、仍画游戏场景，
                // 两侧不出图也不记录状态
                scene_exit = i32(session.supervisor.current);
                exit_frame = f + 1;
                break;
            }
            if (draw_out != nullptr && f + 1 >= draw_from) {
                if (digest_mode) write_digest_line(draw_out, f + 1, device.ops);
                else device.write_frame(draw_out, f + 1);
            }
            // TH16_DIGEST_DUMP=<frame>（1 起）：digest 模式下把该帧的结构行写到标准误（DrawDigest.hpp，口径排查用）
            if (digest_mode) {
                if (const char* dump = std::getenv("TH16_DIGEST_DUMP"); dump != nullptr && std::atoi(dump) == f + 1)
                    dump_structure_lines(stderr, device.ops);
            }
            // TH16_DUMP_LAYERS=<frame>（1 起）：该帧绘制后把各层的 VM 列到 stderr（层 / 槽 / 脚本 / 精灵 / 渲染模式 / 位置 /
            // 颜色 / 标志），用于和 oracle 的 draw 记录逐 VM 对账
            if (dump_layers_frame == f + 1) {
                for (int layer = 0; layer < AnmManager::kLayerCount; ++layer) {
                    for (const AnmVm* vm = session.animations.layer_chain(layer); vm != nullptr; vm = vm->layer_next) {
                        std::fprintf(stderr, "layer=%02x id=%u slot=%d script=%d sprite=%d/%d mode=%d pos=(%g,%g,%g) "
                                     "color=%02x%02x%02x%02x flags=%08x/%08x\n", layer, vm->id, vm->file_slot, vm->script_index,
                                     vm->sprite_index, vm->sprite_source_slot, vm->render_mode(), double(vm->pos.x), double(vm->pos.y), double(vm->pos.z),
                                     vm->color[3], vm->color[2], vm->color[1], vm->color[0], vm->flags_lo, vm->flags_hi);
                    }
                }
            }
            const Player& pl = session.game.player;
            const GameGlobals& g = session.globals;
            std::vector<u32> w;
            w.push_back(u32(f + 1));
            w.push_back(u32(g.game_frame));
            w.push_back(u32(g.stage_frame));
            w.push_back(session.game_input.held);
            w.push_back(session.game_input.pressed);
            w.push_back(u32(g.power));
            w.push_back(u32(g.season_gauge));
            const size_t player_begin = w.size();
            w.push_back(u32(pl.state));
            vec3(pl.pos, w);
            w.push_back(u32(pl.fixed_x)); w.push_back(u32(pl.fixed_y));
            timer3(pl.state_timer, w); timer3(pl.timer2, w); timer3(pl.timer3, w);
            w.push_back(u32(pl.direction));
            w.push_back(u32(pl.last_move_x)); w.push_back(u32(pl.last_move_y));
            w.push_back(u32(pl.focus)); w.push_back(has(pl.focus_vm));
            w.push_back(u32(pl.speed)); w.push_back(u32(pl.speed_focus)); w.push_back(u32(pl.speed_diag)); w.push_back(u32(pl.speed_diag_focus));
            vec3(pl.velocity, w); vec3(pl.last_velocity, w);
            w.push_back(u32(pl.velocity_ix)); w.push_back(u32(pl.velocity_iy));
            w.push_back(u32(pl.option_lag)); w.push_back(u32(pl.gather_counter)); w.push_back(bits(pl.speed_mult));
            vec3(pl.push, w);
            w.push_back(u32(pl.option_count)); w.push_back(u32(pl.power_level)); w.push_back(u32(pl.sub_count)); w.push_back(pl.flags);
            timer3(pl.invincible, w);
            timer3(pl.shot_timer, w); timer3(pl.shot_timer2, w);
            vec3(pl.rect_min, w); vec3(pl.rect_max, w);
            vec3(pl.hit_radius, w); vec3(pl.item_radius, w); vec3(pl.graze_radius, w);
            vec3(pl.item_rect_min, w); vec3(pl.item_rect_max, w);
            vec3(pl.graze_rect_min, w); vec3(pl.graze_rect_max, w);
            vec3(pl.item_rect2_min, w); vec3(pl.item_rect2_max, w);
            w.push_back(u32(pl.scale_active)); w.push_back(bits(pl.scale));
            const size_t options_begin = w.size();
            auto push_option = [&](const PlayerOption& o) {
                w.push_back(u32(o.active));
                w.push_back(u32(o.target_x)); w.push_back(u32(o.target_y));
                w.push_back(u32(o.x)); w.push_back(u32(o.y));
                w.push_back(u32(o.offset_x)); w.push_back(u32(o.offset_y)); w.push_back(u32(o.focus_offset_x)); w.push_back(u32(o.focus_offset_y));
                w.push_back(has(o.vm)); w.push_back(has(o.vm_front));
                w.push_back(u32(o.index)); w.push_back(u32(o.snap));
            };
            for (const PlayerOption& o : pl.options) push_option(o);
            for (const PlayerOption& o : pl.sub_options) push_option(o);
            // 射击系统：shot_misc、类型 2 冷却表、上次伤害源下标、活动自机弹、活动伤害源、本帧音效
            const size_t shooting_begin = w.size();
            w.push_back(u32(pl.shot_misc)); w.push_back(u32(pl.shot_misc_b));
            for (i32 c : pl.option_cooldown) w.push_back(u32(c));
            w.push_back(u32(pl.last_damage_index));
            std::vector<const PlayerBullet*> live_bullets;
            for (const PlayerBullet& b : pl.bullets) if (b.active != 0) live_bullets.push_back(&b);
            w.push_back(u32(live_bullets.size()));
            for (const PlayerBullet* b : live_bullets) {
                w.push_back(u32(b->index)); w.push_back(b->flags); w.push_back(has(b->vm));
                timer3(b->timer, w);
                vec3(b->motion.pos, w); w.push_back(bits(b->motion.speed)); w.push_back(bits(b->motion.angle));
                vec3(b->motion.velocity, w);
                w.push_back(b->motion.mode); w.push_back(u32(b->active)); w.push_back(u32(b->target)); w.push_back(u32(b->damage));
                w.push_back(bits(b->hit_w)); w.push_back(bits(b->hit_h));
                w.push_back(b->shot_id); w.push_back(u32(b->damage_source));
            }
            std::vector<int> live_sources;
            for (int i = 0; i < Player::kDamageSourceCount; ++i) if ((pl.damage_sources[i].flags & 1u) != 0) live_sources.push_back(i);
            w.push_back(u32(live_sources.size()));
            for (int i : live_sources) {
                const DamageSource& d = pl.damage_sources[i];
                w.push_back(u32(i + 1)); w.push_back(d.flags); w.push_back(bits(d.f04)); w.push_back(bits(d.f08));
                w.push_back(bits(d.angle)); w.push_back(bits(d.angular_speed)); w.push_back(bits(d.size_x)); w.push_back(bits(d.size_y));
                vec3(d.motion.pos, w);
                const u32* motion_rest = reinterpret_cast<const u32*>(&d.motion) + 3;   // +0x28..+0x5f：pos2 … mode
                for (int k = 0; k < 14; ++k) w.push_back(motion_rest[k]);
                timer3(d.timer, w);
                w.push_back(u32(d.damage));
                w.push_back(u32(d.f78)); w.push_back(u32(d.f7c)); w.push_back(u32(d.f80)); w.push_back(u32(d.f84));
                w.push_back(u32(d.owner)); w.push_back(u32(d.f8c)); w.push_back(u32(d.kind));
            }
            // oracle 只记录 Player 代码段（0x440000..0x449000）登记的音效
            w.push_back(u32(session.player_sfx_events.size()));
            for (i32 s : session.player_sfx_events) w.push_back(u32(s));
            for (u32 v : w) feed_u32(v);
            feed(reinterpret_cast<const u8*>(&pl.vm), kSnapshotBytes);
            {
                u32 live_enm = 0;
                for (const auto& ep : session.game.enemies.enemies) if (ep) ++live_enm;
                feed_enm_u32(u32(f + 1));
                feed_enm_u32(u32(session.game.enemies.enemy_count));
                feed_enm_u32(live_enm);
                for (const auto& ep : session.game.enemies.enemies) {
                    if (!ep) continue;
                    const Enemy& e = *ep;
                    feed_enm_u32(e.id);
                    feed_enm_u32(e.flags);
                    const u32* m1 = reinterpret_cast<const u32*>(&e.motion1);
                    const u32* m2 = reinterpret_cast<const u32*>(&e.motion2);
                    for (int k = 0; k < 17; ++k) feed_enm_u32(m1[k]);
                    for (int k = 0; k < 17; ++k) feed_enm_u32(m2[k]);
                    feed_enm_u32(bits(e.primary.time));
                    feed_enm_u32(u32(e.primary.routine));
                    feed_enm_u32(u32(e.primary.instruction_offset));
                    feed_enm_u32(u32(e.life));
                    feed_enm_u32(u32(e.life_max));
                    if (const char* rn = e.routine_name()) feed_enm(reinterpret_cast<const u8*>(rn), std::strlen(rn));
                }
                // 敌人 ANM VM（game-oracle.py enemy_vm_digest 同口径）：[frame, 敌人数] + 每敌人 [id, VM 数] + 每 VM 13 字
                feed_vm_u32(u32(f + 1));
                feed_vm_u32(live_enm);
                for (const auto& ep : session.game.enemies.enemies) {
                    if (!ep) continue;
                    std::vector<u32> vw;
                    enemy_vm_words(*ep, vw);
                    feed_vm_u32(ep->id);
                    feed_vm_u32(u32(vw.size() / 13));
                    for (u32 v : vw) feed_vm_u32(v);
                }
            }
            // 敌弹（game-oracle.py bullet_digest 同口径）：[frame, count, len] + 每弹 25 字
            {
                u32 live_bul = 0;
                const BulletManager* bm = session.game.bullets_live ? session.game.bullets.get() : nullptr;
                if (bm) for (const Bullet* b = bm->active_head; b; b = b->active_next) ++live_bul;
                feed_bul_u32(u32(f + 1));
                feed_bul_u32(u32(bm ? bm->active_count : 0));
                feed_bul_u32(live_bul);
                if (bm) for (const Bullet* b = bm->active_head; b; b = b->active_next) {
                    const u32 words[] = {u32(b->index), b->flags, u32(b->state), u32(i32(b->type)), u32(i32(b->color)),
                                         bits(b->pos.x), bits(b->pos.y), bits(b->pos.z),
                                         bits(b->velocity.x), bits(b->velocity.y), bits(b->velocity.z),
                                         bits(b->speed), bits(b->angle), bits(b->hitbox.x), bits(b->hitbox.y),
                                         bits(b->scale), u32(b->ex_index), b->ex_flags,
                                         u32(b->timer.previous), u32(b->timer.current), bits(b->timer.current_f),
                                         u32(b->vm_main.sprite_index)};
                    for (u32 v : words) feed_bul_u32(v);
                }
            }
            // 道具 + 资源（game-oracle.py item_digest 同口径）：[frame, active, spawned, 资源 10 字] + 每道具 24 字
            {
                const ItemManager* im = session.game.items_live ? session.game.items.get() : nullptr;
                const u32 head[] = {u32(f + 1), u32(im ? im->active_count : 0), u32(im ? im->spawn_count : 0),
                                    u32(g.score), u32(g.lives), u32(g.bombs), u32(g.bomb_fragments), u32(g.point_value),
                                    u32(g.point_items), u32(g.collect_sum), u32(g.collect_count), u32(g.collect_indicator),
                                    u32(g.bomb_fragment_items)};
                for (u32 v : head) feed_item_u32(v);
                if (im) for (const Item& it : im->items) {
                    if (it.state == 0) continue;
                    const u32 words[] = {u32(it.index), u32(it.state), u32(it.type), u32(it.type_copy),
                                         bits(it.pos.x), bits(it.pos.y), bits(it.pos.z),
                                         bits(it.velocity.x), bits(it.velocity.y), bits(it.velocity.z),
                                         bits(it.speed), bits(it.angle), bits(it.attract_speed), u32(it.counter), u32(it.flag),
                                         u32(it.timer.previous), u32(it.timer.current), bits(it.timer.current_f),
                                         u32(it.vm.script_index), u32(it.vm.sprite_index), it.vm.flags_lo, item_vm_color(it.vm),
                                         u32(it.vm2.script_index), u32(it.vm2.sprite_index), it.vm2.flags_lo};
                    for (u32 v : words) feed_item_u32(v);
                }
            }
            // 激光（game-oracle.py laser_digest 同口径）：[frame, count, len] + 每激光公共区 + 采样点
            {
                const LaserManager* lm = session.game.lasers_live ? session.game.lasers.get() : nullptr;
                feed_laser_u32(u32(f + 1));
                feed_laser_u32(u32(lm ? lm->lasers.size() : 0));
                feed_laser_u32(u32(lm ? lm->lasers.size() : 0));
                if (lm) for (const auto& lp : lm->lasers) {
                    std::vector<u32> lw;
                    laser_words(*lp, lw);
                    for (u32 v : lw) feed_laser_u32(v);
                }
            }
            {
                feed_menu_u32(u32(f + 1));
                std::vector<u32> mw;
                menu_words(mw);
                for (u32 v : mw) feed_menu_u32(v);
            }
            // 音效请求（game-oracle.py sound_digest 同口径）：[frame, 请求数, (声部, 声像)…]
            {
                const SoundRequests& sr = session.sfx_events;
                feed_sound_u32(u32(f + 1));
                feed_sound_u32(u32(sr.ids.size()));
                for (size_t i = 0; i < sr.ids.size(); ++i) {
                    feed_sound_u32(u32(sr.ids[i]));
                    feed_sound_u32(u32(sr.pans[i]));
                }
            }
            // 炸弹 / 季节解放（game-oracle.py bomb_digest 同口径）：[frame] + 两只对象各 [有无] + 字段
            {
                feed_bomb_u32(u32(f + 1));
                for (const Bomb* b : {session.game.bombs.main(), session.game.bombs.release()}) {
                    feed_bomb_u32(b != nullptr ? 1u : 0u);
                    if (b == nullptr) continue;
                    std::vector<u32> bw;
                    bomb_words(*b, bw);
                    for (u32 v : bw) feed_bomb_u32(v);
                }
            }
            // 符卡（game-oracle.py spell_digest 同口径）：[frame, flags, timer×3, id, bonus, bonus_initial, time_limit,
            // count, state, vm 有无×5, dialogue, boss_ids×3, enemy_bonus×2, spell_id]
            {
                const Spellcard& sc = session.spellcard;
                const EnemyManager& em = session.game.enemies;
                const u32 words[] = {u32(f + 1), sc.flags, u32(sc.timer.previous), u32(sc.timer.current), bits(sc.timer.current_f),
                                     u32(sc.id), u32(sc.bonus), u32(sc.bonus_initial), u32(sc.time_limit), u32(sc.capture_count),
                                     u32(sc.state), has(sc.vm_ids[0]), has(sc.vm_ids[1]), has(sc.vm_ids[2]), has(sc.vm_ids[3]),
                                     has(sc.vm_ids[4]), u32(session.game.world.dialogue ? 1 : 0),
                                     em.boss_ids[0], em.boss_ids[1], em.boss_ids[2],
                                     u32(g.enemy_bonus_pending), u32(g.enemy_bonus), u32(g.spell_id)};
                for (u32 v : words) feed_spell_u32(v);
            }
            if (digest_only || f + 1 < frames_from) continue;

            std::printf("%s{\"frame\":%d,\"stage\":%d,\"game_frame\":%d,\"stage_frame\":%d,\"input\":[%u,%u],\"power\":%d,\"season_gauge\":%d,"
                        "\"enemy_count\":%d,\"dialogue\":%d,\"rng\":[%u,%u],\"game_rng\":[%u,%u],\"player\":{",
                        first_printed ? "" : ",", f + 1, g.stage, g.game_frame, g.stage_frame, session.game_input.held, session.game_input.pressed,
                        g.power, g.season_gauge, session.game.world.enemy_count, session.game.world.dialogue ? 1 : 0,
                        u32(session.animations.visual_rng.seed), session.animations.visual_rng.calls,
                        u32(session.animations.game_rng.seed), session.animations.game_rng.calls);
            first_printed = false;
            size_t at = player_begin;
            std::printf("\"state\":%d,", i32(w[at])); at += 1;
            print_words("pos", w, at, 3); at += 3; std::printf(",");
            std::printf("\"pos_fixed\":[%d,%d],", i32(w[at]), i32(w[at + 1])); at += 2;
            std::printf("\"timers\":[");
            for (int t = 0; t < 3; ++t) { std::printf("%s[%d,%d,%u]", t ? "," : "", i32(w[at]), i32(w[at + 1]), w[at + 2]); at += 3; }
            std::printf("],");
            std::printf("\"dir\":%d,\"last_move\":[%d,%d],\"focus\":%d,\"focus_vm\":%u,", i32(w[at]), i32(w[at + 1]), i32(w[at + 2]), i32(w[at + 3]), w[at + 4]); at += 5;
            print_words("speeds", w, at, 4); at += 4; std::printf(",");
            print_words("velocity", w, at, 8); at += 8; std::printf(",");
            std::printf("\"option_lag\":%d,\"counter\":%d,\"speed_mult\":%u,", i32(w[at]), i32(w[at + 1]), w[at + 2]); at += 3;
            print_words("ext_velocity", w, at, 3); at += 3; std::printf(",");
            std::printf("\"option_count\":%d,\"power_level\":%d,\"sub_count\":%d,\"flags\":%u,", i32(w[at]), i32(w[at + 1]), i32(w[at + 2]), w[at + 3]); at += 4;
            print_words("invincible", w, at, 3); at += 3; std::printf(",");
            std::printf("\"shot_timers\":[");
            for (int t = 0; t < 2; ++t) { std::printf("%s[%d,%d,%u]", t ? "," : "", i32(w[at]), i32(w[at + 1]), w[at + 2]); at += 3; }
            std::printf("],");
            print_words("rects", w, at, 6); at += 6; std::printf(",");
            print_words("radii", w, at, 9); at += 9; std::printf(",");
            print_words("rects2", w, at, 18); at += 18; std::printf(",");
            std::printf("\"scale\":[%d,%u],", i32(w[at]), w[at + 1]); at += 2;
            if (at != options_begin) return fail("snapshot layout mismatch");
            std::printf("\"options\":[");
            for (int i = 0; i < Player::kOptionCount + Player::kSubOptionCount; ++i) {
                const u32* o = &w[options_begin + size_t(i) * 13];
                std::printf("%s{\"active\":%d,\"target\":[%d,%d],\"pos\":[%d,%d],\"offset\":[%d,%d,%d,%d],\"vm\":[%u,%u],\"index\":%d,\"snap\":%d}",
                            i ? "," : "", i32(o[0]), i32(o[1]), i32(o[2]), i32(o[3]), i32(o[4]), i32(o[5]), i32(o[6]), i32(o[7]), i32(o[8]),
                            o[9], o[10], i32(o[11]), i32(o[12]));
            }
            std::printf("],");
            at = shooting_begin;
            std::printf("\"shot_misc\":[%d,%u],", i32(w[at]), w[at + 1]); at += 2;
            print_words("cooldown", w, at, 16); at += 16; std::printf(",");
            std::printf("\"last_damage_index\":%d,", i32(w[at])); at += 1;
            const u32 bullet_count = w[at]; at += 1;
            std::printf("\"bullets\":[");
            for (u32 i = 0; i < bullet_count; ++i) {
                const u32* b = &w[at];
                std::printf("%s{\"index\":%d,\"flags\":%u,\"vm\":%u,\"timer\":[%d,%d,%u],\"pos\":[%u,%u,%u],\"speed\":%u,\"angle\":%u,"
                            "\"velocity\":[%u,%u,%u],\"mode\":%u,\"active\":%d,\"target\":%d,\"damage\":%d,\"size\":[%u,%u],\"shot_id\":%u,\"damage_source\":%d}",
                            i ? "," : "", i32(b[0]), b[1], b[2], i32(b[3]), i32(b[4]), b[5], b[6], b[7], b[8], b[9], b[10],
                            b[11], b[12], b[13], b[14], i32(b[15]), i32(b[16]), i32(b[17]), b[18], b[19], b[20], i32(b[21]));
                at += 22;
            }
            std::printf("],");
            const u32 source_count = w[at]; at += 1;
            std::printf("\"damage_sources\":[");
            for (u32 i = 0; i < source_count; ++i) {
                const u32* d = &w[at];
                std::printf("%s{\"index\":%d,\"flags\":%u,\"f04\":[%u,%u],\"angle\":%u,\"angular_speed\":%u,\"size\":[%u,%u],\"pos\":[%u,%u,%u],",
                            i ? "," : "", i32(d[0]), d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9], d[10]);
                print_words("motion", w, at + 11, 14);
                std::printf(",\"timer\":[%d,%d,%u],\"damage\":%d,", i32(d[25]), i32(d[26]), d[27], i32(d[28]));
                print_words("tail", w, at + 29, 7);
                std::printf("}");
                at += 36;
            }
            std::printf("],");
            const u32 sfx_count = w[at]; at += 1;
            print_words("sfx", w, at, sfx_count); at += sfx_count;
            if (at != w.size()) return fail("snapshot layout mismatch (shooting)");
            std::printf(",\"vm\":\"");
            const u8* raw = reinterpret_cast<const u8*>(&pl.vm);
            for (size_t i = 0; i < kSnapshotBytes; ++i) std::printf("%02x", raw[i]);
            std::printf("\"},\"enemies\":{\"count\":%d,\"enemies\":[", session.game.enemies.enemy_count);
            bool first_enm = true;
            for (const auto& ep : session.game.enemies.enemies) {
                if (!ep) continue;
                const Enemy& e = *ep;
                const char* rname = e.routine_name();
                std::printf("%s{\"id\":%u,\"flags\":%u,\"pos\":[%.9g,%.9g,%.9g],",
                            first_enm ? "" : ",", e.id, e.flags, e.pos.x, e.pos.y, e.pos.z);
                first_enm = false;
                auto print_motion = [&](const char* key, const MotionState& m) {
                    const u32* mw = reinterpret_cast<const u32*>(&m);
                    std::printf("\"%s\":[", key);
                    for (int k = 0; k < 17; ++k) std::printf("%s%u", k ? "," : "", mw[k]);
                    std::printf("]");
                };
                print_motion("motion1", e.motion1); std::printf(",");
                print_motion("motion2", e.motion2); std::printf(",");
                std::printf("\"ctx\":[%.9g,%d,%d,", e.primary.time, e.primary.routine, e.primary.instruction_offset);
                if (rname) std::printf("\"%s\"", rname); else std::printf("null");
                std::printf("],\"hp\":[%d,%d,%d,%d],\"vms\":[", e.life, e.life_max, e.life_display, e.life_budget);
                std::vector<u32> vw;
                enemy_vm_words(e, vw);
                for (size_t k = 0; k < vw.size(); k += 13) {
                    std::printf("%s[", k ? "," : "");
                    for (size_t j = 0; j < 13; ++j) std::printf("%s%u", j ? "," : "", vw[k + j]);
                    std::printf("]");
                }
                std::printf("],\"sprite_size\":[%u,%u]}", bits(e.sprite_size.x), bits(e.sprite_size.y));
            }
            std::printf("]}");
            {
                const BulletManager* bm = session.game.bullets_live ? session.game.bullets.get() : nullptr;
                std::printf(",\"bullets\":{\"count\":%d,\"bullets\":[", bm ? bm->active_count : 0);
                bool first_bul = true;
                if (bm) for (const Bullet* b = bm->active_head; b; b = b->active_next) {
                    std::printf("%s{\"index\":%d,\"flags\":%u,\"state\":%u,\"type\":%d,\"color\":%d,\"pos\":[%u,%u,%u],\"vel\":[%u,%u,%u],"
                                "\"speed\":%u,\"angle\":%u,\"hitbox\":[%u,%u],\"scale\":%u,\"ex_index\":%d,\"ex_flags\":%u,"
                                "\"timer\":[%d,%d,%u],\"sprite_id\":%d}",
                                first_bul ? "" : ",", b->index, b->flags, u32(b->state), i32(b->type), i32(b->color),
                                bits(b->pos.x), bits(b->pos.y), bits(b->pos.z), bits(b->velocity.x), bits(b->velocity.y), bits(b->velocity.z),
                                bits(b->speed), bits(b->angle), bits(b->hitbox.x), bits(b->hitbox.y), bits(b->scale), b->ex_index, b->ex_flags,
                                b->timer.previous, b->timer.current, bits(b->timer.current_f), b->vm_main.sprite_index);
                    first_bul = false;
                }
                std::printf("]}");
            }
            {
                // 道具（game-oracle.py item_snapshot 同口径）：按池下标顺序列出 state != 0 的槽
                const ItemManager* im = session.game.items_live ? session.game.items.get() : nullptr;
                std::printf(",\"items\":{\"active\":%d,\"spawned\":%d,\"point_counter\":[%d,%d],\"scale\":%u,\"items\":[",
                            im ? im->active_count : 0, im ? im->spawn_count : 0, im ? im->point_active : 0, im ? im->serial : 0,
                            bits(im ? im->attract_scale : 1.0f));
                bool first_item = true;
                if (im) for (const Item& it : im->items) {
                    if (it.state == 0) continue;
                    std::printf("%s{\"index\":%d,\"state\":%d,\"type\":%d,\"type2\":%d,\"pos\":[%u,%u,%u],\"vel\":[%u,%u,%u],"
                                "\"speed\":%u,\"angle\":%u,\"timer\":[%d,%d,%u,%d,%u],\"attract_speed\":%u,\"counter\":%d,\"serial\":%d,"
                                "\"flag\":%d,\"vm\":[%d,%d,%u,%u,%d,%d,%u]}",
                                first_item ? "" : ",", it.index, it.state, it.type, it.type_copy,
                                bits(it.pos.x), bits(it.pos.y), bits(it.pos.z), bits(it.velocity.x), bits(it.velocity.y), bits(it.velocity.z),
                                bits(it.speed), bits(it.angle), it.timer.previous, it.timer.current, bits(it.timer.current_f),
                                it.timer.speed_slot, it.timer.flags, bits(it.attract_speed), it.counter, it.serial, it.flag,
                                it.vm.script_index, it.vm.sprite_index, it.vm.flags_lo,
                                u32(it.vm.color[0]) | (u32(it.vm.color[1]) << 8) | (u32(it.vm.color[2]) << 16) | (u32(it.vm.color[3]) << 24),
                                it.vm2.script_index, it.vm2.sprite_index, it.vm2.flags_lo);
                    first_item = false;
                }
                std::printf("]},\"resources\":{\"score\":%d,\"extend_index\":%d,\"lives\":%d,\"life_fragments\":%d,\"bombs\":%d,"
                            "\"bomb_fragments\":%d,\"point_value\":%d,\"point_value_max\":%d,\"point_items\":%d,\"graze\":%d,"
                            "\"collect\":[%d,%d,%d,%d],\"collect_pos\":[%u,%u,%u]}",
                            g.score, g.extend_index, g.lives, g.life_fragments, g.bombs, g.bomb_fragments, g.point_value,
                            g.point_value_max, g.point_items, g.graze, g.collect_sum, g.collect_count, g.collect_indicator,
                            g.bomb_fragment_items, bits(g.collect_pos.x), bits(g.collect_pos.y), bits(g.collect_pos.z));
            }
            {
                // 激光快照（game-oracle.py laser_snapshot 同口径）：按链表顺序
                const LaserManager* lm = session.game.lasers_live ? session.game.lasers.get() : nullptr;
                std::printf(",\"lasers\":{\"count\":%d,\"next_handle\":%d,\"lasers\":[", lm ? int(lm->lasers.size()) : 0,
                            lm ? lm->next_handle : 0);
                bool first_laser = true;
                if (lm) for (const auto& lp : lm->lasers) {
                    std::vector<u32> lw;
                    laser_words(*lp, lw);
                    const u32* h = lw.data();
                    std::printf("%s{\"kind\":%d,\"lc\":%u,\"state\":%d,\"id\":%d,\"type\":%d,\"color\":%d,\"pos\":[%u,%u,%u],\"vel\":[%u,%u,%u],"
                                "\"angle\":%u,\"length\":%u,\"width\":%u,\"speed\":%u,\"start_drawn\":%u,\"ex_index\":%d,\"ex_flags\":%u,"
                                "\"no_cancel\":%d,\"age\":[%d,%d,%u],\"graze\":[%d,%d,%u],\"frame\":[%d,%d,%u],\"nodes\":[",
                                first_laser ? "" : ",", i32(h[0]), h[1], i32(h[2]), i32(h[3]), i32(h[4]), i32(h[5]),
                                h[6], h[7], h[8], h[9], h[10], h[11], h[12], h[13], h[14], h[15], h[16], i32(h[17]), h[18], i32(h[19]),
                                i32(h[20]), i32(h[21]), h[22], i32(h[23]), i32(h[24]), h[25], i32(h[26]), i32(h[27]), h[28]);
                    const u32 node_count = lw[kLaserHeadWords];
                    for (u32 i = 0; i < node_count; ++i) {
                        const u32* nd = &lw[kLaserHeadWords + 1 + size_t(i) * 5];
                        std::printf("%s[%u,%u,%u,%u,%u]", i ? "," : "", nd[0], nd[1], nd[2], nd[3], nd[4]);
                    }
                    std::printf("]}");
                    first_laser = false;
                }
                std::printf("]}");
            }
            {
                // 炸弹 / 季节解放快照（game-oracle.py bomb_snapshot 同口径）
                static const char* const kBombClassNames[] = {"ReimuA", "CirnoA", "AyaA", "MarisaA", "ReimuSub", "CirnoSub", "AyaSub", "MarisaSub", "AllSub"};
                std::printf(",\"bombs\":{");
                const Bomb* pair[2] = {session.game.bombs.main(), session.game.bombs.release()};
                for (int k = 0; k < 2; ++k) {
                    std::printf("%s\"%s\":", k ? "," : "", k ? "sub" : "main");
                    const Bomb* b = pair[k];
                    if (b == nullptr) { std::printf("null"); continue; }
                    std::vector<u32> bw;
                    bomb_words(*b, bw);
                    const u32* h = bw.data();
                    std::printf("{\"class\":\"%s\",\"pos\":[%u,%u,%u],\"angular\":%u,\"angle\":%u,\"active\":%d,\"timer\":[%u,%u,%u,%u,%u],"
                                "\"vm\":[%u,%u],\"in_spell\":%d,\"kind\":%d,\"level\":%d,\"bonus\":%u,\"bonus_shown\":%u,"
                                "\"popup\":[%u,%u,%u,%u,%u],\"popup_level\":%d,\"popup_pos\":[%u,%u,%u],\"orbs\":[",
                                h[0] < 9 ? kBombClassNames[h[0]] : "?", h[1], h[2], h[3], h[4], h[5], i32(h[6]), h[7], h[8], h[9],
                                u32(b->timer.speed_slot), b->timer.flags, h[10], h[11], i32(h[12]), i32(h[13]), i32(h[14]), h[15], h[16],
                                h[17], h[18], h[19], u32(b->popup_timer.speed_slot), b->popup_timer.flags, i32(h[20]), h[21], h[22], h[23]);
                    const u32 orb_count = h[24];
                    const ReimuBomb* reimu = b->as_reimu();
                    for (u32 i = 0; i < orb_count; ++i) {
                        const u32* o = &bw[25 + size_t(i) * 21];
                        const Timer& ot = reimu->orbs[i].timer;
                        std::printf("%s{\"vm\":%u,\"pos\":[%u,%u,%u],\"speed\":%u,\"angle\":%u,\"radius\":%u,\"center\":[%u,%u,%u],"
                                    "\"mode\":%u,\"active\":%d,\"timer\":[%u,%u,%u,%u,%u],\"delta\":[%u,%u,%u],\"target\":%u,\"index\":%d,"
                                    "\"source\":%d}",
                                    i ? "," : "", o[0], o[1], o[2], o[3], o[4], o[5], o[6], o[7], o[8], o[9], o[10], i32(o[11]),
                                    o[12], o[13], o[14], u32(ot.speed_slot), ot.flags, o[15], o[16], o[17], o[18], i32(o[19]),
                                    i32(o[20]));
                    }
                    std::printf("]}");
                }
                std::printf("}");
            }
            {
                // 符卡快照（game-oracle.py spell_snapshot 同口径）
                const Spellcard& sc = session.spellcard;
                const EnemyManager& em = session.game.enemies;
                std::printf(",\"spell\":{\"flags\":%u,\"timer\":[%u,%u,%u],\"id\":%d,\"bonus\":%d,\"bonus_initial\":%d,"
                            "\"time_limit\":%d,\"count\":%d,\"state\":%d,\"vm\":[%u,%u,%u,%u,%u],\"dialogue\":%d,"
                            "\"boss_ids\":[%u,%u,%u],\"enemy_bonus\":[%d,%d],\"spell_id\":%d}",
                            sc.flags, u32(sc.timer.previous), u32(sc.timer.current), bits(sc.timer.current_f), sc.id, sc.bonus,
                            sc.bonus_initial, sc.time_limit, sc.capture_count, sc.state, has(sc.vm_ids[0]), has(sc.vm_ids[1]),
                            has(sc.vm_ids[2]), has(sc.vm_ids[3]), has(sc.vm_ids[4]), session.game.world.dialogue ? 1 : 0,
                            em.boss_ids[0], em.boss_ids[1], em.boss_ids[2], g.enemy_bonus_pending, g.enemy_bonus, g.spell_id);
            }
            if (const Msg* m = session.game.gui.dialogue.get()) {
                // 对话 Msg 快照（game-oracle.py msg_snapshot 同口径）
                std::printf(",\"msg\":{\"entry\":%d,\"pc\":%u,\"age\":%d,\"timer\":%d,\"wait\":%d,\"ecl_release\":%d,"
                            "\"flags\":%u,\"lines\":[%d,%d],\"side\":%d,\"text_pos\":[%u,%u],\"width\":%u,\"box_index\":%d,\"vm\":[",
                            m->entry, u32(m->pc - session.game.gui.msg_data.data()), m->age.current, m->timer.current,
                            m->wait.current, m->ecl_release, m->flags, m->lines_written, m->lines_cleared, m->side,
                            bits(m->text_pos.x), bits(m->text_pos.y), bits(m->text_width), m->box_index);
                const u32 vms[12] = {m->portrait_vm, m->face_vms[0], m->face_vms[1], m->face_vms[2], m->face_vms[3], m->frame_vm,
                                     m->text_vms[0], m->text_vms[1], m->text_vms[2], m->text_vms[3], m->name_vm, m->box_vm};
                for (int i = 0; i < 12; ++i) std::printf("%s%u", i ? "," : "", has(vms[i]));
                std::printf("]}");
            } else {
                std::printf(",\"msg\":null");
            }
            {
                // GameMenu 快照（game-oracle.py menu_snapshot 同口径）
                const GameGlobals& g = session.globals;
                std::printf(",\"menu\":{\"game_flags\":%u,\"game_speed\":%u,\"continues_used\":%d,\"continues_left\":%d,\"sfx\":[",
                            session.game.flags, bits(session.supervisor.game_speed), g.continues, g.continues_left);
                for (size_t i = 0; i < session.sfx_events.ids.size(); ++i) std::printf("%s%d", i ? "," : "", session.sfx_events.ids[i]);
                std::printf("],\"menu\":");
                if (!session.menu_live) std::printf("null}");
                else {
                    const GameMenu& m = session.menu;
                    std::printf("{\"mode\":%d,\"prev_mode\":%d,\"state\":%d,\"timer\":[%u,%u,%u,%u,%u],\"timer2\":[%u,%u,%u,%u,%u],"
                                "\"cursor\":[%d,%d,%d,%d],\"name_cursor\":[%d,%d,%d],\"vm\":[%u,%u],\"name_len\":%d,\"practice\":%d,"
                                "\"no_rank\":%d,\"kind\":%d,\"saved_sound\":%d,\"saved_speed\":%u,\"flags\":%u}}",
                                i32(m.mode), i32(m.previous_mode), i32(m.state), u32(m.timer.previous), u32(m.timer.current),
                                bits(m.timer.current_f), u32(m.timer.speed_slot), m.timer.flags, u32(m.timer2.previous),
                                u32(m.timer2.current), bits(m.timer2.current_f), u32(m.timer2.speed_slot), m.timer2.flags,
                                m.cursor.index, m.cursor_previous, m.cursor.count, m.cursor.disabled_count, m.name_cursor.index,
                                m.name_cursor_previous, m.name_cursor.count, m.menu_vm != 0 ? 1u : 0u, m.screenshot_vm != 0 ? 1u : 0u,
                                m.name_length, m.practice, m.no_rank, m.kind, m.saved_sound_state, bits(m.saved_game_speed), m.flags);
                }
            }
            if (const StageManager* sm = session.game.stage.get()) {
                // 背景脚本位置（game-oracle.py stage_vm 同口径：ins 为字节偏移）
                const i32 ins_offset = (sm->ins_index >= 0 && sm->ins_index < i32(sm->std.script.size()))
                                           ? i32(sm->std.script[size_t(sm->ins_index)].offset) : -1;
                std::printf(",\"stage_vm\":{\"frame\":%d,\"ins\":%d,\"flags\":%u}", sm->frame, ins_offset, sm->flags);
            }
            std::printf("}");
        }
        if (digest_only)
            std::printf("stage=%d shot=%d season=%d frames=%d scene_exit=%d exit_frame=%d digest=%016llx enemies=%016llx bullets=%016llx enemy_vms=%016llx items=%016llx spell=%016llx lasers=%016llx bombs=%016llx menu=%016llx sound=%016llx bgm=%s\n",
                        stage, shot, season, frames, scene_exit, exit_frame, (unsigned long long)digest, (unsigned long long)enm_digest,
                        (unsigned long long)bul_digest, (unsigned long long)vm_digest, (unsigned long long)item_digest,
                        (unsigned long long)spell_digest, (unsigned long long)laser_digest, (unsigned long long)bomb_digest,
                        (unsigned long long)menu_digest, (unsigned long long)sound_digest,
                        session.music.current.empty() ? "-" : session.music.current.c_str());
        else std::printf("],\"scene_exit\":%d,\"exit_frame\":%d}\n", scene_exit, exit_frame);
        if (record_dir != nullptr && !session.save_replay_slot(1, "TEST")) return fail(session.failure.c_str());
        if (!session.game.enemies.unknown_ops.empty()) {
            std::fprintf(stderr, "unknown_ecl_ops=");
            for (size_t i = 0; i < session.game.enemies.unknown_ops.size(); ++i)
                std::fprintf(stderr, "%s%d", i ? "," : "", session.game.enemies.unknown_ops[i]);
            std::fprintf(stderr, "\n");
        }
        if (draw_out != nullptr) {
            std::fclose(draw_out);
            // 未实现的 render mode 计数：对拍前先看这里有没有缺口
            for (u32 mode = 0; mode < 32; ++mode)
                if (session.renderer.unimplemented_modes[mode] != 0)
                    std::fprintf(stderr, "unimplemented_render_mode=%u count=%u\n", mode, session.renderer.unimplemented_modes[mode]);
            for (u32 hook = 0; hook < 8; ++hook)
                if (session.renderer.unimplemented_draw_callbacks[hook] != 0)
                    std::fprintf(stderr, "unimplemented_draw_callback=%u count=%u\n", hook, session.renderer.unimplemented_draw_callbacks[hook]);
        }
        return 0;
    }
    if (command == "scene-boot" && argc >= 4) {
        // 与 Application 共用 GameSession：Boot→Title→主菜单。
        // 可选输入脚本："frame:bits,frame:bits,..."（bits 为 InputState 位，十六进制/十进制，持续 1 帧）；
        // "skip" 置 DAT_004c1800 & 0x180 直接放行启动幕等待。
        const int frames = std::atoi(argv[3]);
        GameSession session;
        if (!session.open(dat.data(), u32(dat.size()))) return fail(session.failure.c_str());
        if (const char* demo = std::getenv("TH16_REPLAY_DIR")) session.demo_directory = demo;
        else {
            // games/th16/original 同级的 demo/，或 artifacts/th16/dat 扁平 demoN.rpy
            session.demo_directory = std::string(argv[2]);
            auto slash = session.demo_directory.find_last_of("/\\");
            if (slash != std::string::npos) session.demo_directory.resize(slash);
        }
        struct Key { int frame; u32 bits; };
        struct PadPress { int frame; i32 button; };
        std::vector<Key> script;
        std::vector<PadPress> pads;
        bool unlock_spells = false;
        bool dump_ascii = false;
        bool dump_text = false;
        const char* draw_path = nullptr;
        int draw_from = 1;
        i32 title_origin = -1;
        int spell_return[3] = {-1, -1, -1};
        bool trace_scenes = false;
        bool wall_clock = false;   // 宿主墙钟按 60Hz 推进（成绩文件计时 0x42dbc0 等）
        std::vector<std::pair<int, int>> dik_presses;
        struct Reach { int shot, difficulty, stage; };
        std::vector<Reach> reaches;
        std::vector<Reach> clears;
        for (int a = 4; a < argc; ++a) {
            if (std::strcmp(argv[a], "skip") == 0) { session.supervisor.config_flags |= Supervisor::kSkipBootFlags; continue; }
            if (std::strncmp(argv[a], "cfg=", 4) == 0) { session.config_flags = u32(std::strtoul(argv[a] + 4, nullptr, 0)); continue; }
            if (std::strcmp(argv[a], "ascii") == 0) { dump_ascii = true; continue; }
            if (std::strncmp(argv[a], "draw=", 5) == 0) { draw_path = argv[a] + 5; continue; }
            if (std::strncmp(argv[a], "draw-from=", 10) == 0) { draw_from = std::atoi(argv[a] + 10); continue; }
            if (std::strcmp(argv[a], "text") == 0) { dump_text = true; continue; }
            if (std::strcmp(argv[a], "unlock-spells") == 0) { unlock_spells = true; continue; }
            if (std::strncmp(argv[a], "origin=", 7) == 0) { title_origin = std::atoi(argv[a] + 7); continue; }
            if (std::strncmp(argv[a], "spell-return=", 13) == 0) {
                // DAT_004a2970 / 0049f2dc / 0049f2d8：符卡练习返回标题时要还原的分组 / 列表 / 条目
                std::sscanf(argv[a] + 13, "%d,%d,%d", &spell_return[0], &spell_return[1], &spell_return[2]);
                continue;
            }
            if (std::strncmp(argv[a], "score=", 6) == 0) { session.score_path = argv[a] + 6; continue; }
            if (std::strncmp(argv[a], "config=", 7) == 0) { session.config_path = argv[a] + 7; session.load_config(); continue; }
            if (std::strcmp(argv[a], "wall") == 0) { wall_clock = true; continue; }
            if (std::strcmp(argv[a], "scenes") == 0) { trace_scenes = true; continue; }
            if (std::strncmp(argv[a], "dik=", 4) == 0) {
                // dik=帧:DIK码,…：该帧键盘快照里置 0x80（只持续一帧）
                for (const char* p = argv[a] + 4; *p;) {
                    char* end = nullptr;
                    const long fr = std::strtol(p, &end, 0);
                    if (*end != ':') return fail("bad dik");
                    const long code = std::strtol(end + 1, &end, 0);
                    dik_presses.push_back({int(fr), int(code)});
                    p = *end == ',' ? end + 1 : end;
                }
                continue;
            }
            if (std::strncmp(argv[a], "reach=", 6) == 0) {
                // 练习到达位 +0x519d（机体块, 难度, 关卡）：等价于该关已非回放装载过
                int r[3] = {-1, -1, -1};
                std::sscanf(argv[a] + 6, "%d,%d,%d", &r[0], &r[1], &r[2]);
                reaches.push_back({r[0], r[1], r[2]});
                continue;
            }
            if (std::strncmp(argv[a], "clear=", 6) == 0) {
                // 未续关通关次数 +0x5184（机体块, 难度）置 1：0x44a800 / 0x44a850 / 0x44a8e0 的输入
                int c[2] = {-1, -1};
                std::sscanf(argv[a] + 6, "%d,%d", &c[0], &c[1]);
                clears.push_back({c[0], c[1], 0});
                continue;
            }
            const char* p = argv[a];
            while (*p) {
                char* endp = nullptr;
                // "Pframe:button" → 该帧手柄按钮 button 按下（0x401c30 快照；Key Config 捕获 / 0x4018e0 合成）
                if (*p == 'P' || *p == 'p') {
                    const long frame = std::strtol(p + 1, &endp, 10);
                    if (endp == p + 1 || *endp != ':') return fail("bad pad script");
                    p = endp + 1;
                    const long code = std::strtol(p, &endp, 0);
                    pads.push_back({int(frame), i32(code)});
                    p = endp;
                    if (*p == ',') ++p;
                    continue;
                }
                const long frame = std::strtol(p, &endp, 10);
                if (endp == p || *endp != ':') return fail("bad input script");
                p = endp + 1;
                const unsigned long b = std::strtoul(p, &endp, 0);
                script.push_back({int(frame), u32(b)});
                p = endp;
                if (*p == ',') ++p;
            }
        }
        if (!session.begin_boot()) return fail(session.failure.c_str());
        if (title_origin >= 0) session.globals.title_origin = title_origin;
        session.globals.spell_return_group = spell_return[0];
        session.globals.spell_return_list = spell_return[1];
        session.globals.spell_return_entry = spell_return[2];
        for (const Reach& r : reaches) {
            if (r.shot < 0 || r.shot >= ScoreFile::kShots || r.difficulty < 0 || r.stage < 0 ||
                r.difficulty * 8 + r.stage >= ScoreFile::kPracticeSlots)
                return fail("bad reach");
            session.scores.mark_practice_reached(r.shot, r.difficulty, r.stage);
        }
        for (const Reach& c : clears) {
            if (c.shot < 0 || c.shot >= ScoreFile::kShots || c.difficulty < 0 || c.difficulty >= ScoreFile::kDifficulties)
                return fail("bad clear");
            session.scores.blocks[c.shot].no_continue_clears[c.difficulty] = 1;
        }
        if (unlock_spells) {
            // 标题符卡练习导航自测：把合计块 attempts[0] 置 1，等价于已在主线见过全部符卡
            for (int id = 0; id < ScoreFile::kSpellRecords; ++id)
                session.scores.blocks[ScoreFile::kAllShotsBlock].spells[id].attempts[0] = 1;
        }

        // 绘制链每帧都跑（与 Application 相同）。draw=<path> 时按 game-draw 口径逐帧落 JSON Lines（帧号 1 起）
        RecordingDevice device;
        session.attach_device(&device);
        std::FILE* draw_out = nullptr;
        if (draw_path != nullptr) {
            draw_out = std::fopen(draw_path, "w");
            if (draw_out == nullptr) return fail("cannot open draw output");
        }
        for (int f = 0; f < frames; ++f) {
            session.injected_input = 0;
            for (const Key& k : script) if (k.frame == f) session.injected_input |= k.bits;
            session.pad_buttons = 0;
            for (const PadPress& s : pads) {
                if (s.frame == f && s.button >= 0 && s.button < 32) session.pad_buttons |= 1u << s.button;
            }
            if (wall_clock) session.wall_clock_seconds = f / 60.0;
            std::memset(session.keyboard_state, 0, sizeof(session.keyboard_state));
            for (const auto& [fr, code] : dik_presses)
                if (fr == f && code >= 0 && code < 256) session.keyboard_state[code] = 0x80;
            device.begin_frame();
            const SceneId before = session.supervisor.current;
            if (!session.logic_step()) return fail(session.failure.c_str());
            if (trace_scenes && session.supervisor.current != before)
                std::printf("scene-change frame=%d %d->%d\n", f, int(before), int(session.supervisor.current));
            if (draw_out != nullptr && f + 1 >= draw_from) device.write_frame(draw_out, f + 1);
        }
        if (draw_out != nullptr) std::fclose(draw_out);
        const std::vector<RecordingDevice::QuadSummary> quads = device.quads();
        const Supervisor& sup = session.supervisor;
        const TitleScene& title = session.title;
        const GameGlobals& g = session.globals;
        std::printf("scene=%d boot_frames=%u title_live=%d screen=%d sub=%d cursor=%d difficulty=%d shot=%d season=%d stage=%d mode=%d quit=%d bgm=%u se=%u update_chains=%u draw_chains=%u quads=%zu lives=%d bombs=%d practice_lives=%d\n",
                    int(sup.current), sup.boot_frames, sup.title_menu_live ? 1 : 0, title.screen, title.sub, title.cursor.index,
                    g.difficulty, g.shot, g.season, g.stage, g.mode(), session.quit_requested ? 1 : 0,
                    unsigned(g.bgm_volume), unsigned(g.se_volume),
                    session.chains.update_count(), session.chains.draw_count(), quads.size(), g.lives, g.bombs, g.practice_lives);
        std::printf("spell group=%d list=%d entry=%d shot=%d slots=%d,%d,%d,%d,%d return=%d,%d,%d origin=%d replay_slot=%d\n",
                    title.spell_group, title.spell_list_index, title.spell_entry_index, title.spell_shot_cursor.index,
                    title.spell_slot_ids[0], title.spell_slot_ids[1], title.spell_slot_ids[2], title.spell_slot_ids[3],
                    title.spell_slot_ids[4], g.spell_return_group, g.spell_return_list, g.spell_return_entry,
                    g.title_origin, g.replay_slot);
        std::printf("pad");
        for (int i = 0; i < GameGlobals::kPadButtonCount; ++i) std::printf("%c%d", i ? ',' : ' ', int(g.pad_buttons[i]));
        std::printf("\n");
        {
            const ScoreBlock& b = session.scores.blocks[g.shot + g.shot_aux];
            u32 bgm = 0;
            for (int i = 0; i < ScoreFile::kBgmFlags; ++i) bgm |= session.scores.stats.bgm_heard[i] != 0 ? 1u << i : 0u;
            std::printf("scores play_count=%u play_time=%llu total_time=%llu bgm=%x\n", b.play_count,
                        (unsigned long long)b.play_time, (unsigned long long)session.scores.total_play_time(), bgm);
        }
        for (const TitleScene::Event& e : session.title_events) std::printf("event %s %d\n", e.kind, e.value);
        for (const TitleScene::Event& e : title.events) std::printf("event %s %d\n", e.kind, e.value);
        if (dump_ascii) {
            for (i32 i = 0; i < session.ascii.entry_count(); ++i) {
                const AsciiManager::Entry& e = session.ascii.entry(i);
                std::printf("ascii %.1f %.1f %08x %d [%s]\n", double(e.pos.x), double(e.pos.y), e.color,
                            e.shadow ? 1 : 0, e.text);
            }
        }
        if (dump_text) {
            // 0x459240 实参（逻辑帧入队，harness 不刷像素故累积全部请求）
            for (const TextIntoTextureRequest& r : session.text_into_texture)
                std::printf("text font=%d height=%d x=%d spacing=%d shadow=%d color=%06x outline=%06x rect=%d,%d,%d,%d [%s]\n",
                            r.font, r.height, r.x, r.spacing, r.shadow ? 1 : 0, r.color, r.outline, r.left, r.top,
                            r.right, r.bottom, r.text.c_str());
        }
        for (const RecordingDevice::QuadSummary& q : quads) RecordingDevice::print_quad(q);
        if (session.game_live && session.game.stage) {
            // 场景 7：GameManager / StageManager 状态与关卡背景绘制（0x409f90）
            StageManager& st = *session.game.stage;
            const StageCamera& c = st.camera;
            std::printf("game flags=%x frame=%d game_frame=%d stage_frame=%d std=%s anm=%s objects=%zu instances=%zu faces=%d\n",
                        session.game.flags, session.game.timer.current, g.game_frame, g.stage_frame,
                        session.game.entry ? session.game.entry->std_name : "-", st.std.anm_name.c_str(),
                        st.std.objects.size(), st.std.instances.size(), st.std.face_vm_count);
            std::printf("stage frame=%d ins=%d time=%d rocking=%d pos=%.6g,%.6g,%.6g facing=%.6g,%.6g,%.6g up=%.6g,%.6g,%.6g fov=%.8g fog=%08x,%.6g,%.6g bg=%08x\n",
                        st.frame, st.ins_index, st.timer.current, st.rocking_mode,
                        c.pos.x, c.pos.y, c.pos.z, c.facing.x, c.facing.y, c.facing.z, c.up.x, c.up.y, c.up.z, c.fov,
                        c.fog.color, c.fog.near_, c.fog.far_, st.bg_color);
            // clear = 画面合成开头清场地的颜色（DAT_004c1b0c）；fog_clear = 关卡背景 0x409f90 清场地用的雾色
            u32 field_clear = 0, fog_clear = 0;
            bool first_rect_clear = true;
            for (const RecordingDevice::Op& op : device.ops) {
                if (op.kind != RecordingDevice::OpKind::Clear || !op.has_rect) continue;
                if (first_rect_clear) { field_clear = op.clear_color; first_rect_clear = false; }
                else if ((op.clear_flags & kClearColor) != 0) { fog_clear = op.clear_color; break; }
            }
            size_t stage_quads = 0;
            for (const RecordingDevice::QuadSummary& q : quads) if (q.kind == 'w' && q.texture.slot == 3 + (st.stage_number & 1)) ++stage_quads;
            std::printf("stagepass active=%d clear=%08x fog_clear=%08x quads=%zu drawn_instances=%d culled=%d drawn_quads=%d\n",
                        (st.flags & 1) ? 1 : 0, field_clear, fog_clear, stage_quads, st.drawn_instances, st.culled_instances, st.drawn_quads);
        }
        return 0;
    }
    return fail("unknown command");
}
