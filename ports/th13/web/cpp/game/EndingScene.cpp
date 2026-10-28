// Ending 场景实现（原版 0x416130 初始化 / 0x416a10 解释器 / 0x4163b0 拆卸）。
// 关键反汇编依据见 EndingScene.hpp 头注释与
// artifacts/verification/s6-full-20261028/{report.md Part B, disasm-00416a10.txt}。
#include "EndingScene.hpp"

#include "MsgPlayer.hpp"

#include <cmath>
#include <cstring>

namespace th13 {

namespace {
// 0x4bba20 结局 msg 名表（EXE .data 实读：staff.msg, e01..e12.msg）。
constexpr const char* kEndingMessages[13] = {
    "staff.msg", "e01.msg", "e02.msg", "e03.msg", "e04.msg", "e05.msg", "e06.msg",
    "e07.msg",   "e08.msg", "e09.msg", "e10.msg", "e11.msg", "e12.msg",
};
// score 存档缓冲内结局字节区基偏移（0x416130:0x41629a 起）。
constexpr i32 kEndingByteBase = 0x1b26a;
// op5 负参数的替代等待长度（0x416bc8 push 0x3e7）。
constexpr i32 kNegativeWaitFallback = 999;
}  // namespace

i32 ending_index_for(const SessionState& progress, const EndingSaveBytes& save) {
    // 0x416130:0x41623a：base = DAT_004be7bc(shot) + DAT_004be7b8(char)。
    // th13 单机体 → shot 恒 0；char 侧钳 0..3（越界档防越界读）。
    const i32 base = (progress.character & 3);
    i32 index = base;
    if (progress.continues != 0) {
        // 续关 → 坏结局（base+9，e09..e12 段）。
        return index + 9;
    }
    index = 2 * base + 1;
    // 真结局变体门：难度>=1 && 好结局资格(4be83c)>=3 && save[2*base+0x1b26b]&1
    //（= 0x1b26a+(2*base+1)，即标准好结局 e0(2*base+1) 的已见位）。
    const bool seen_standard = (save.read(2 * base + 1) & 1) != 0;
    if (progress.difficulty >= 1 && progress.good_endings >= 3 && seen_standard)
        index += 1;
    return index;
}

EndingScene::EndingScene(SessionState& progress, EndingPlatform& platform)
    : progress_(progress), platform_(platform) {}

void EndingScene::interp_set(Interp& it, i32 param) {
    // 0x4016c0（fild 实证：int→float 转换，非位重解释）。
    it.current = param - 1;
    it.target = param;
    it.acc = float(param);
}

void EndingScene::interp_tick(Interp& it, float delta) {
    // 0x4732c0：速度取 0x4c0a28（1.0；Ending 期无 ECL 改写）。
    it.current = it.target;
    constexpr float speed = 1.0f;
    if (speed > 0.99f && speed < 1.01f)
        it.acc += delta;
    else
        it.acc += speed * delta;
    it.target = i32(it.acc);  // 0x4971f0 ftol（截断）
}

void EndingScene::advance_line() {
    // 0x4170fa-0x417141：正常路径行 +1、acc += 1.0。
    line_.current = line_.target;
    line_.acc += 1.0f;
    line_.target += 1;
}

bool EndingScene::record_at(u32 offset, Record& out) const {
    if (offset + 4 > msg_bytes_.size()) return false;
    const u8* raw = msg_bytes_.data() + offset;
    out.line = u32(raw[0]) | u32(raw[1]) << 8;
    out.op = raw[2];
    out.length = raw[3];
    out.payload = raw + 4;
    return offset + 4 + out.length <= msg_bytes_.size();
}

i32 EndingScene::payload_i32(const Record& r, u32 index) const {
    const u32 at = 4 * index;
    if (r.length < at + 4) return 0;
    i32 value = 0;
    std::memcpy(&value, r.payload + at, 4);
    return value;
}

std::string EndingScene::payload_cstring(const Record& r, u32 byte_offset) const {
    if (byte_offset >= r.length) return {};
    const u8* start = r.payload + byte_offset;
    const u8* end = r.payload + r.length;
    const u8* nul = static_cast<const u8*>(
        std::memchr(start, 0, size_t(end - start)));
    const u8* stop = nul ? nul : end;
    return std::string(reinterpret_cast<const char*>(start), size_t(stop - start));
}

bool EndingScene::initialize() {
    if (finished_) return fail("Ending scene already finished");
    // ---- 0x416130：结局索引 + 落档位（内存）----
    EndingSaveBytes* save = platform_.save_bytes();
    if (!save) return fail("Ending scene has no save view");
    ending_index_ = ending_index_for(progress_, *save);
    // scene+0x20：bit0 = 该结局首见（save[idx+0x1b26a]==0）、bit1 = 好结局
    // 全未见过（save[0x1b276]==0）——op5/6 的跳过许可门与 0x416610 节拍消费。
    if (save->read(ending_index_) == 0) scene_flags_ |= 1u;
    if (save->read(0x1b276 - kEndingByteBase) == 0) scene_flags_ |= 2u;
    // save[idx+0x1b26a] |= 1；难度>0 → |= 0x10；idx<8 → save[0x1b276]=1。
    {
        const u8 seen = u8(save->read(ending_index_) | 1);
        save->write(ending_index_, seen);
        if (progress_.difficulty > 0)
            save->write(ending_index_, u8(seen | 0x10));
        if (ending_index_ < 8) save->write(0x1b276 - kEndingByteBase, 1);
    }
    // ---- e##.msg 装载（0x4724c0；表 0x4bba20[idx]）----
    message_name_ = kEndingMessages[ending_index_ >= 0 && ending_index_ < 13
                                         ? ending_index_ : 0];
    if (!platform_.read_asset(message_name_, msg_bytes_) || msg_bytes_.empty())
        return fail("Ending message unavailable: " + message_name_);
    // ---- 0x4166d0：滚屏对象初值（行 0、等待清、0x74|=1、颜色 0xffffff）----
    msg_bytes_.resize(msg_bytes_.size() + 4, 0);  // 尾部读护板（越界记录=截断）
    line_ = Interp{-1, 0, 0.f};
    text_wait_ = Interp{-1, 0, 0.f};
    state_flags_ = 1;
    color_ = 0x00ffffff;
    page_lines_ = 0;
    page_text_.fill({});
    record_ = 0;
    {
        // 脚本起点 = 文件 + u32@+4（0x416130 尾；= entry0.offset，MsgFile 同式）。
        u32 script = 0;
        if (msg_bytes_.size() >= 8) std::memcpy(&script, msg_bytes_.data() + 4, 4);
        if (script + 4 > msg_bytes_.size()) return fail("Ending message has no script");
        record_ = script;
    }
    return true;
}

bool EndingScene::update(const GameInput& input) {
    if (!error.empty()) return false;
    if (finished_) return true;
    // ---- 0x416a10 主体（+0x74 bit2 = ANM 装载中 → 直接返回；切片 4 同步装载不置位）----
    bool ended = false;
    while (true) {
        Record record;
        if (!record_at(record_, record)) {
            // 越界记录（损坏脚本）：原版未定义；port 显式失败不静默。
            return fail("Ending script record out of range: " + message_name_);
        }
        if (record.line > u32(line_.target)) {
            // 下一记录在未来：推进滚屏行，返回（0x4170fa）。
            advance_line();
            break;
        }
        bool handled = true;
        switch (record.op) {
        case 0:  // END → 0x416610 置 DAT_004dcc24 = 0x10（练习位不达）。
            ended = true;
            break;
        case 1:
        case 2:  // 空转（跳转表 0x4170dd）。
            break;
        case 3:
            handled = op_page_line(record);
            break;
        case 4:
            handled = op_page_clear();
            break;
        case 5:
            handled = op_text_wait(input, record, false);
            break;
        case 6:
            handled = op_text_wait(input, record, true);
            break;
        case 7:
            handled = op_anm_load(record);
            break;
        case 8:
            handled = op_anm_vm(record);
            break;
        case 9:  // 0x416f77：+0x7c = payload dword。
            color_ = u32(payload_i32(record, 0));
            break;
        case 10:
            handled = op_music(record);
            break;
        case 11:  // 0x416fdc：0x43af70(3.0f)；+0x74 &= ~1。
            if (!platform_.fade_out_music(180)) return fail("Ending music fade failed");
            state_flags_ &= ~1u;
            break;
        case 12:
            if (!op_reload_next(record)) {
                // 0x417018：新 msg 读失败 → 返 -1（同 END 出口）。
                ended = true;
                break;
            }
            // op12 成功：record_ 已重置为新流起点，不推进指针、同帧继续
            // 解释（0x4170eb → 0x416a40 分支）。
            continue;
        case 13:  // 0x4170b9：0x45fa60(0, param,0,0,0)。
            handled = platform_.open_ending_window(0, payload_i32(record, 0));
            break;
        case 14:  // 0x4170c7：0x45fa60(5, param,0,0,0)。
            handled = platform_.open_ending_window(5, payload_i32(record, 0));
            break;
        case 15:
        case 16:
        case 17: {
            // 0x416da9/0x416e43/0x416edd：DAT_004be7c4 == 1/2/3 时同 op8。
            const i32 gate = record.op - 14;
            if (progress_.difficulty == gate) handled = op_anm_vm(record);
            break;
        }
        default:  // > 0x11：跳转表外套直接推进（0x416a4a jbe 检查后）。
            break;
        }
        if (ended) break;
        if (!handled) {
            if (!error.empty()) return false;
            break;
        }
        record_ += 4 + record.length;  // 0x4170dd。
        // 00417144 advances the record and returns immediately while the
        // asynchronous ANM request is pending. Even the synchronous host must
        // not interpret the next record in the same update.
        if (record.op == 7) break;
    }
    if (ended) {
        // 0x416610：解释器返 -1 → DAT_004dcc24 = (0x4dcc5c&0x2000)?2:0x10。
        (void)platform_.request_destination(0x10);
        finished_ = true;
        return true;
    }
    // 0x416610 返 0 分支：帧计数 +0x24++（返回 1/6 的跳过节拍不 port——
    // 「Now Loading」闪烁/任务层细节，行为面由 op5/6 的 %6 门承载）。
    frames_++;
    return true;
}

bool EndingScene::op_page_line(const Record& r) {
    // 0x416a57：+0x78 为 0 时重建 5 文本 VM（0x46e8f0(0x4aaaf0,0xffffff)+
    // 0x46fc30(3)），随后 0x42a340 解密正文入 VM[+0x78]（0x46fc30(2)），
    // +0x78++。渲染面走平台近似通道；行为面保留页内行序与颜色。
    // 0x42a340 的解出含 NUL 终结字节；文本面按 C 串语义剥除。
    std::string text = msg_decrypt_text(r.payload, r.length);
    while (!text.empty() && text.back() == '\0') text.pop_back();
    if (page_lines_ == 0) {
        if (!platform_.ending_page_reset()) return fail("Ending page reset failed");
        page_text_.fill({});
    }
    page_text_[u32(page_lines_)] = text;
    if (!platform_.ending_page_line(page_lines_, color_, text))
        return fail("Ending page line submission failed");
    page_lines_ = (page_lines_ + 1) % i32(page_text_.size());
    return true;
}

bool EndingScene::op_page_clear() {
    // 0x416b73：5 × 0x46fc30(vm,3)——文本面清空（页计数不动）。
    page_text_.fill({});
    if (!platform_.ending_page_clear()) return fail("Ending page clear failed");
    return true;
}

bool EndingScene::op_text_wait(const GameInput& input, const Record& r,
                               bool page_break) {
    // 0x416b9b（op5）/ 0x416c4d（op6）。+0x30 = 文本等待插值器目标值。
    if (text_wait_.target <= 0) interp_set(text_wait_, payload_i32(r, 0));
    interp_tick(text_wait_, -1.0f);
    if (!page_break && payload_i32(r, 0) < 0) {
        // op5 负参数（staff.msg 尾段 0xffffffff）→ 置 999 帧自然等待。
        interp_set(text_wait_, kNegativeWaitFallback);
    }
    // 0x460bc0(0) + 0x4016c0(0) 后推进的三路放行：
    if ((input.pressed & (0x1u | 0x80000u)) != 0    // a) Z/Enter 按下（0x4e49fc&0x80001）
        || text_wait_.target <= 0) {                // b) 自然到点（+0x30<=0）
        if (!platform_.advance_sound()) return fail("Ending advance sound failed");
        interp_set(text_wait_, 0);
    } else {
        // c) 持键快进（0x416be7-0x416c2b）：场景 bit0（首见）封锁；
        //    Ctrl(0x200) 或 Z(bit0) 持续且重复计数(0x4e4b04)>=0x14；
        //    且 +0x30 % 6 == 0。
        if ((scene_flags_ & 1u) != 0) return false;
        const bool held_ok = (input.held & 0x200u) != 0 ||
                             ((input.held & 0x1u) != 0 &&
                              input.repeat_age[0] >= 0x14);
        if (!held_ok) return false;
        if (text_wait_.target % 6 != 0) return false;
        interp_set(text_wait_, 0);
    }
    if (page_break) {
        // op6 推进附加：+0x78 = 0、DAT_004dc318 = 0（0x416cd1）。
        page_lines_ = 0;
    }
    return true;
}

bool EndingScene::op_anm_load(const Record& r) {
    // 0x417144：+0xec=idx、+0x70=名（payload+4 起）、+0x74|=4、异步回调
    // 0x417230 → 0x46df40(槽=idx+0x14) 装载 + 存 +0x80[idx]。生产同步装载（B8）。
    const i32 index = payload_i32(r, 0);
    const std::string name = payload_cstring(r, 4);
    if (index < 0 || index > 3) return fail("Ending ANM slot out of range");
    if (!name.empty() && !platform_.load_ending_anm(0x14 + index, name))
        return fail("Ending ANM load failed: " + name);
    return true;
}

bool EndingScene::op_anm_vm(const Record& r) {
    // 0x416ce8：payload = [vm 槽(+0x90)][ANM bank(+0x80)][脚本]。
    // bank 即 op7 装载的 0x14+idx 槽；VM 注册进 ANM 管理器由平台承接。
    const i32 vm_slot = payload_i32(r, 0);
    const i32 bank_index = payload_i32(r, 1);
    const i32 script = payload_i32(r, 2);
    if (vm_slot < 0 || vm_slot >= 24 || bank_index < 0 || bank_index > 3)
        return fail("Ending ANM cue indices out of range");
    return platform_.cue_ending_anm(vm_slot, 0x14 + bank_index, script) ||
           fail("Ending ANM cue failed");
}

bool EndingScene::op_music(const Record& r) {
    // 0x416f82：0x43adc0(0, "bgm/th13_18") → 排队 "<name>.wav"；字符串
    // == "bgm/th13_18"（0x4aaaf4）→ 0x43ae40(0,0xf) 否则 (0,0x10)。
    // port 映射（play_track 同式）：预载槽 0 + 播放；0xf/0x10 副参未建模。
    std::string name = payload_cstring(r);
    const i32 mode = name == "bgm/th13_18" ? 0xf : 0x10;
    // 原版曲名带 bgm/ 前缀；生产 fmt 名表（thbgm.fmt）为 th13_XX.wav 本名。
    const auto slash = name.find('/');
    if (slash != std::string::npos) name.erase(0, slash + 1);
    if (name.empty() || name.rfind(".wav", name.size() - 4) != std::string::npos)
        return fail("Ending music name invalid");
    return platform_.queue_music(1, 0, name + ".wav") &&
           platform_.queue_music(2, 0, "") && platform_.record_music_unlocked(mode);
}

bool EndingScene::op_reload_next(const Record& r) {
    // 0x416ff4 + 0x416000：反注册 5 文本 VM、按名读新 msg（失败 → END 出口）、
    // 0xf0 对象 memset 重置（行 0/页 0/颜色 0xffffff/+0x74|=2）。
    const std::string name = payload_cstring(r);
    platform_.release_ending_page();
    std::vector<u8> bytes;
    if (name.empty() || !platform_.read_asset(name, bytes) || bytes.size() < 8)
        return false;
    msg_bytes_ = std::move(bytes);
    message_name_ = name;
    msg_bytes_.resize(msg_bytes_.size() + 4, 0);
    line_ = Interp{-1, 0, 0.f};
    text_wait_ = Interp{-1, 0, 0.f};
    color_ = 0x00ffffff;
    page_lines_ = 0;
    page_text_.fill({});
    u32 script = 0;
    std::memcpy(&script, msg_bytes_.data() + 4, 4);
    if (script + 4 > msg_bytes_.size()) return false;
    record_ = script;
    // 00416a10 clears the complete scroll object before setting bit 1.
    state_flags_ = 2u;
    return true;
}

void EndingScene::teardown() {
    // 0x4163b0：5 文本 VM（0x415f60）与 ANM 槽 20-23（0x46e620）反装载。
    platform_.release_ending_page();
    page_text_.fill({});
    page_lines_ = 0;
    for (i32 slot = 0x14; slot < 0x14 + 4; slot++) platform_.release_ending_anm(slot);
    msg_bytes_.clear();
    msg_bytes_.shrink_to_fit();
}

}  // namespace th13
