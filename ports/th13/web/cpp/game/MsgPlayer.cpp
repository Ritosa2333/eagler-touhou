#include "MsgPlayer.hpp"

#include "MsgPlayerCp932.hpp"

#include <cstdlib>
#include <cstring>

namespace th13 {

namespace {

bool read_u32(const std::vector<u8>& bytes, u32 at, u32& value) {
    if (at + 4 > bytes.size()) return false;
    std::memcpy(&value, bytes.data() + at, 4);
    return true;
}

void append_utf8(std::string& out, u32 code) {
    if (code < 0x80) {
        out.push_back(char(code));
    } else if (code < 0x800) {
        out.push_back(char(0xC0 | (code >> 6)));
        out.push_back(char(0x80 | (code & 0x3F)));
    } else if (code < 0x10000) {
        out.push_back(char(0xE0 | (code >> 12)));
        out.push_back(char(0x80 | ((code >> 6) & 0x3F)));
        out.push_back(char(0x80 | (code & 0x3F)));
    } else {
        out.push_back(char(0xF0 | (code >> 18)));
        out.push_back(char(0x80 | ((code >> 12) & 0x3F)));
        out.push_back(char(0x80 | ((code >> 6) & 0x3F)));
        out.push_back(char(0x80 | (code & 0x3F)));
    }
}

}  // namespace

bool MsgFile::open(const u8* data, u32 size, std::string& error) {
    entries_.clear();
    bytes_.assign(data, data + size);
    u32 count = 0;
    if (size < 4 || !read_u32(bytes_, 0, count) || count == 0 || count > 64) {
        error = "msg header entry count invalid";
        return false;
    }
    const u32 table_bytes = 4 + count * 8;  // 头 + 表（指令流起点）
    if (size < table_bytes) {
        error = "msg entry table truncated";
        return false;
    }
    for (u32 index = 0; index < count; index++) {
        MsgEntry entry;
        if (!read_u32(bytes_, 4 + index * 8, entry.offset) ||
            !read_u32(bytes_, 4 + index * 8 + 4, entry.id)) {
            error = "msg entry header read failed";
            return false;
        }
        if (entry.offset < table_bytes || entry.offset >= size) {
            error = "msg entry offset out of range";
            return false;
        }
        entries_.push_back(entry);
    }
    return true;
}

std::string msg_decrypt_text(const u8* args, u32 arg_bytes) {
    // FUN_0042a340：key=0x77 起，每字节 key+=inc、inc+=0x10（u8 环绕），
    // 解出 NUL 即止（加密串以「异或后为 0」的字节收尾）。
    std::string out;
    out.reserve(arg_bytes);
    u32 key = 0x77;
    u32 inc = 0x07;
    for (u32 index = 0; index < arg_bytes; index++) {
        const u8 decoded = u8(args[index] ^ key);
        out.push_back(char(decoded));
        if (decoded == 0) break;
        key = (key + inc) & 0xFF;
        inc = (inc + 0x10) & 0xFF;
    }
    return out;
}

std::string cp932_to_utf8(const u8* data, u32 size) {
    std::string out;
    out.reserve(size);
    u32 index = 0;
    while (index < size) {
        const u8 lead = data[index];
        if (lead == 0) break;  // NUL 收尾
        if (lead < 0x80) {
            append_utf8(out, lead);
            index++;
            continue;
        }
        if (lead >= 0xA1 && lead <= 0xDF) {  // 半角片假名 → U+FF61..
            append_utf8(out, 0xFF61u + (lead - 0xA1));
            index++;
            continue;
        }
        if (lead < 0x81 || lead > 0xEF || lead >= 0xF0 || index + 1 >= size) {
            append_utf8(out, 0xFFFD);
            index++;
            continue;
        }
        const u8 row = kCp932Row[lead - 0x81];
        const u8 trail = data[index + 1];
        u32 code = 0xFFFD;
        if (row != 0xFF && trail >= kCp932TrailBase &&
            trail < kCp932TrailBase + kCp932TrailCols) {
            const u16 mapped = kCp932ToUnicode[row][trail - kCp932TrailBase];
            if (mapped != 0) code = mapped;
        }
        append_utf8(out, code);
        index += code == 0xFFFD ? 1 : 2;
    }
    return out;
}

bool build_cp932_font_mapping(std::vector<u8>& out) {
    out.assign(131072, 0);
    for (u32 n = 0; n < 0x10000; n++) {
        u16 value = 0x30FB;  // 无效组合的中点（th15 prepare-fonts.mjs 同约定）
        if (n < 0x100) {
            const u8 byte = u8(n);
            if (byte < 0x80)
                value = byte;  // ASCII 直通
            else if (byte >= 0xA1 && byte <= 0xDF)
                value = u16(0xFF61u + (byte - 0xA1));  // 半角片假名
        } else {
            const u8 lead = u8(n >> 8), trail = u8(n & 0xFF);
            if (lead >= 0x81 && lead <= 0xEF) {
                const u8 row = kCp932Row[lead - 0x81];
                // trail 为 u8：上界 0x40+192=0x100 恒真（kCp932TrailCols=192
                // 覆盖到 0xFF），只判下界即可，与 cp932_to_utf8 同格语义。
                if (row != 0xFF && trail >= kCp932TrailBase) {
                    const u16 mapped = kCp932ToUnicode[row][trail - kCp932TrailBase];
                    if (mapped != 0) value = mapped;
                }
            }
        }
        out[n * 2] = u8(value & 0xFF);
        out[n * 2 + 1] = u8(value >> 8);
    }
    return out.size() == 131072;
}

bool MsgPlayer::open(MsgFile& file, u32 entry_index, MsgHost& host, std::string& error) {
    if (entry_index >= file.entry_count()) {
        error = "msg entry index out of range";
        return false;
    }
    file_ = &file;
    host_ = &host;
    cursor_ = file.bytes() + file.entry(entry_index).offset;
    state_ = MsgDialogState{};
    state_.active = true;
    state_.entry = i32(entry_index);
    clock_ = 0;
    line_index_ = 0;
    block_open_ = 0;
    wait_timeout_ = 0;
    command_delay_ = 0;
    return true;
}

void MsgPlayer::finish() {
    // 0x4265a0：执行器返回非 0 → 当帧销毁对话（+0x198=0），519 下一帧放行。
    state_.done = true;
    state_.active = false;
    if (host_) host_->msg_dialog_end();  // 钩子：宿主释放对话 VM（立绘/名牌/文本区）
}

bool MsgPlayer::step(const MsgAdvance& input) {
    if (!state_.active) return false;
    if (command_delay_ > 0) --command_delay_;
    // 0x428ae0:125-127：Ctrl(0x200) 或 Z(0x1) 按下 → 置快进请求位 0x40。
    // 位值经 GameKeyboard 表（0x200=Ctrl、0x80000=Enter、0x1=Z）。
    const u32 pressed = input.pressed | (input.z_pressed ? 1u : 0u);
    if (pressed & 0x201u) state_.skip_request = true;
    // 428ae0 advances the SCRIPT clock to the next command only when a
    // new Z/Ctrl press has armed skip and that key is now held >=20 frames.
    if (state_.auto_advance && state_.skip_request &&
        (((input.held & 1u) && input.z_frames >= 20) ||
         ((input.held & 0x200u) && input.ctrl_frames >= 20)) &&
        cursor_ + 4 <= file_->bytes() + file_->size()) {
        u16 next_time = 0;
        std::memcpy(&next_time, cursor_, 2);
        clock_ = next_time;
    }
    for (;;) {
        if (cursor_ + 4 > file_->bytes() + file_->size()) {
            finish();  // 越界按终止处理（不应发生：表偏移经 open 校验）
            return false;
        }
        u16 time = 0;
        std::memcpy(&time, cursor_, 2);
        const u8 opcode = cursor_[2];
        const u8 arg_bytes = cursor_[3];
        if (cursor_ + 4 + arg_bytes > file_->bytes() + file_->size()) {
            finish();
            return false;
        }
        if (clock_ < time) { ++clock_; return true; }
        if (opcode == 0) {  // case 0x0：entry 终止, after the instruction time gate
            finish();
            return false;
        }
        const Flow flow = execute(opcode, cursor_ + 4, arg_bytes, input);
        if (flow == Flow::Blocked) return true; // native does not tick script clock here
        cursor_ += 4 + arg_bytes;
    }
    return state_.active;
}

bool MsgPlayer::step(bool z_pressed) {
    MsgAdvance input;
    input.z_pressed = z_pressed;
    input.held = z_pressed ? 1u : 0u;  // 边沿帧即持续首帧（与原 0x4e4c14 首帧一致）
    return step(input);
}

MsgPlayer::Flow MsgPlayer::execute(u8 opcode, const u8* args, u32 arg_bytes,
                                   const MsgAdvance& input) {
    switch (opcode) {
        case 0x01:  // 1：按角色装载立绘 bank（0x4c0aa0 + be7b8*4）
            host_->msg_portrait_load();
            return Flow::Advance;
        case 0x02:  // 2：右立绘创建（0x429335；宿主按 entry 分流中Boss/Boss 对）
        case 0x13:  // 19：Boss 曲播放（0x43ae40(1, stage 表+0x2c)，0x4299df）
        case 0x14:  // 20：名牌 VM（0x429a94；stage 表 +0x54../+0x5c..）
        case 0x1f:  // 31：右立绘 Boss 对（0x42948c）
            host_->msg_bgm(i32(opcode));
            return Flow::Advance;
        case 0x04:  // 4：释放左立绘槽（0x4295cf）
        case 0x05:  // 5：释放右槽（0x4295e9，含名牌 +0x5c）
        case 0x06:  // 6：释放名牌四槽 + 文本区（0x42960c；原版不清 +0x5f/+0x60，
                   // 文本 VM 由 0x46fc30 label1 淡出，像素不清）
            host_->msg_release(i32(opcode));
            return Flow::Advance;
        case 0x0d:  // 13：左立绘表情切换（0x4298d4：label = arg + 0x11 立即中断）
        case 0x0e: {  // 14：右立绘表情切换（0x4298e8）
            i32 label = 0;
            if (arg_bytes >= 4) std::memcpy(&label, args, 4);
            host_->msg_portrait_interrupt(i32(opcode), label + 0x11);
            return Flow::Advance;
        }
        case 0x0c:  // 12：+0x174=1；解释器继续处理同帧指令，不冻结脚本钟。
            command_delay_ = 1;
            return Flow::Advance;
        case 0x07:  // 7：左侧说话人（0x429641：左 label2 亮、右 label3 暗）
        case 0x08:  // 8：右侧说话人（0x4296ee：左 label3 暗、右 label2 亮）
            state_.side = opcode == 0x08 ? 1 : 0;
            // 0x428ae0:343-354（case 7/8 收尾）：+0x5e &= ~2。
            state_.text_attr &= ~2u;
            line_index_ = 0;
            block_open_ = 0;
            host_->msg_speaker_switch(i32(opcode));
            return Flow::Advance;
        case 0x09:  // 9：双立绘名牌变体（0x54.. 位重排）
            // 0x428ae0:352：双立绘分支同样清 +0x5e bit1（视觉位未接）。
            state_.text_attr &= ~2u;
            return Flow::Advance;
        case 0x0a: {  // 10：自动推进位翻转（arg0 & 1）
            i32 value = 0;
            if (arg_bytes >= 4) std::memcpy(&value, args, 4);
            state_.auto_advance = (value & 1) != 0;
            return Flow::Advance;
        }
        case 0x0b: {  // 11：等输入（asm 0x4298fc-0x429988 放行族）
            // 原版放行条件（0x428ae0:366-380 + 0x42991f/0x42992b/0x429930-0x429969
            // 逐指令核，r4 定稿）：
            //  a) Z/Enter 按下位（0x4e4c14 & 0x80001）→ 计声放行（0x460bc0(0)）；
            //  b) arg 倒计时（0x4016c0 置 value-1 起步、每帧 -1）≤0 → 计声放行；
            //  c) 自动位 bit0（429936 test al,0x1）+ 快进请求位 bit0x40
            //     （42993e；0x428ae0:125-127 由 Ctrl 0x200/Z 按下置位）+
            //     Ctrl（0x42b3f0(9)，索引 0x4e49f0+0x194+9*4）或 Z（(0)）
            //     持续 ≥0x14（429955/429966 cmp eax,0x14 双计数器任一）
            //     → 静默放行（42996f 无 0x460bc0 调用）。
            //  bit0/bit0x40 缺一即 429938/429940 跳回阻塞——无被动计时放行
            // （旧 port「wait_frames>=20」臆测路径已删）。
            if (wait_timeout_ == 0 && arg_bytes >= 4) {
                i32 timeout = 0;
                std::memcpy(&timeout, args, 4);
                wait_timeout_ = timeout;
            }
            const bool pressed_z_enter = (input.pressed & (0x1u | 0x80000u)) != 0 || input.z_pressed;
            const bool timed_out = --wait_timeout_ <= 0;
            if (pressed_z_enter || timed_out) {
                host_->msg_advance_blip();
                // 0x428ae0:378-379/393-394：放行只清 +0x5f/+0x60，
                // 不擦 text.anm 的四条像素/VM；旧文字保持到下一块首行
                // 的 " " 清槽（0x428ae0:432-447）。
                line_index_ = 0;
                block_open_ = 0;
                wait_timeout_ = 0;
                return Flow::Advance;
            }
            if (state_.auto_advance && state_.skip_request &&
                (((input.held & 0x200u) && input.ctrl_frames >= 0x14) ||
                 ((input.held & 1u) && input.z_frames >= 0x14))) {
                // 静默快进：0x429946 路径不计声；同样只清行块计数。
                line_index_ = 0;
                block_open_ = 0;
                wait_timeout_ = 0;
                return Flow::Advance;
            }
            return Flow::Blocked;  // 指针不动，下一帧重入本指令
        }
        case 0x11: {  // 17：文本行（保留 CP932 原文与 ruby 元数据）
            std::string decoded = msg_decrypt_text(args, arg_bytes);
            if (!decoded.empty() && decoded[0] == '|') {
                // 原版 00428ae0:455-467 / 520-532 读音注记格式：|x,spacing,ruby
                // atol(第一数) = x（0046e8f0 param_4 → 0045aef0 param_4）；
                // atol(第二数) = 逐字步进（0046e8f0 param_5 → 0045aef0 param_9，
                // 0 = 连续排版，非 0 = 每字 TextOut 后 x += value*2 scratch 像素）。
                // 作为前缀伴随指令，暂存供紧随其后的正文行读取。
                size_t c1 = decoded.find(',', 1);
                size_t c2 = (c1 != std::string::npos) ? decoded.find(',', c1 + 1) : std::string::npos;
                if (c1 != std::string::npos && c2 != std::string::npos) {
                    pending_ruby_ = true;
                    pending_ruby_x_ = std::atoi(decoded.substr(1, c1 - 1).c_str());
                    pending_ruby_spacing_ =
                        std::atoi(decoded.substr(c1 + 1, c2 - c1 - 1).c_str());
                    pending_ruby_cp932_ = decoded.substr(c2 + 1);
                    const u8* ruby_raw = reinterpret_cast<const u8*>(pending_ruby_cp932_.data());
                    pending_ruby_utf8_ = cp932_to_utf8(ruby_raw, u32(pending_ruby_cp932_.size()));
                }
                return Flow::Advance;
            }

            // 正文行
            MsgLine line;
            line.text_cp932 = decoded;
            const u8* raw = reinterpret_cast<const u8*>(decoded.data());
            line.text_utf8 = cp932_to_utf8(raw, u32(decoded.size()));

            if (line_index_ == 0 && block_open_ == 0) {
                state_.lines.clear();
                state_.lines_meta.clear();
                state_.bubble_width = 0.0f;
                state_.block++;
            }
            line.block = state_.block;
            // 原版 00428ae0 气泡宽度计算：((len - 1) * 8) & ~0xf - 28.0f；
            // len 是 NUL 之前的字节数（pcVar5 - (pcVar3 + 1)），不含终止符。
            u32 len = u32(std::strlen(decoded.c_str()));
            u32 align_width = len > 0 ? (((len - 1) * 8) & 0xfffffff0u) : 0;
            float calc_w = float(align_width) - 28.0f;
            if (calc_w > state_.bubble_width) {
                state_.bubble_width = calc_w;
            }
            line.bubble_width = state_.bubble_width;

            if (pending_ruby_) {
                line.has_ruby = true;
                line.ruby_x = pending_ruby_x_;
                line.ruby_spacing = pending_ruby_spacing_;
                line.ruby_cp932 = pending_ruby_cp932_;
                line.ruby_utf8 = pending_ruby_utf8_;
                pending_ruby_ = false;
                pending_ruby_cp932_.clear();
                pending_ruby_utf8_.clear();
            }
            // 0x428ae0:493/558：正文样式取 +0x178 的 bit1；
            // 0x46e8f0 第三参 = ((flags>>1)&1)+4。保留整字属性位供宿主选槽。
            line.text_attr = state_.text_attr;
            line.side = state_.side;

            state_.lines.push_back(line.text_utf8);
            state_.lines_meta.push_back(line);

            if (line_index_ == 0) {
                block_open_ = 1;
                line_index_ = 1;
            } else {
                line_index_ = 0;
                block_open_ = 0;
            }
            return Flow::Advance;
        }
        case 0x12:  // 18：文本区 + 名牌槽释放（0x429250，0x424ba0 + 四槽 0x46fc30）
        case 0x17:  // 23：释放左立绘槽（0x4295a9）
        case 0x18:  // 24：释放右立绘槽（0x4295bc）
            host_->msg_release(i32(opcode));
            return Flow::Advance;
        case 0x19: { // 25：同一 int 参数写四条正文 VM 的 +0x4e4（视觉锚点）
            if (arg_bytes < 4) return Flow::Advance;
            i32 value = 0;
            std::memcpy(&value, args, sizeof value);
            host_->msg_text_anchor_y(float(value));
            return Flow::Advance;
        }
        case 0x1e:  // 30：原版 switch 无 case（dump 18 处用例，空转）
            return Flow::Advance;
        case 0x1a:  // 26：+0x5e |= 2（正文样式 0→1：32px/400→32px/600；
                    // 0x45aef0 表——样式 4/5=40px 仅 0xf/0x10 宣言大字用）
            state_.text_attr |= 2u;
            return Flow::Advance;
        case 0x15:  // 21：★关终了（case 0x15 → 0x42cd40，0x428ae0:673-675）
            host_->msg_stage_clear();
            return Flow::Advance;
        case 0x16:  // 22：仅 0x43af70 音乐淡出
            host_->msg_music_fade(i32(opcode), 0.0f);
            return Flow::Advance;
        case 0x1b: { // 27：淡出秒数为指令浮点参数
            float seconds = 0;
            if (arg_bytes >= 4) std::memcpy(&seconds, args, sizeof seconds);
            host_->msg_music_fade(i32(opcode), seconds);
            return Flow::Advance;
        }
        case 0x1c: {  // 28：对话框 x,y（两 f32 位型存整槽，case 0x1c）
            float x = state_.box_x, y = state_.box_y;
            if (arg_bytes >= 4) std::memcpy(&x, args, 4);
            if (arg_bytes >= 8) std::memcpy(&y, args + 4, 4);
            state_.box_x = x;
            state_.box_y = y;
            return Flow::Advance;
        }
        case 0x1d: {  // 29：文字属性（(arg*4) 异或进 0x5e 的 bit2-5）
            i32 value = 0;
            if (arg_bytes >= 4) std::memcpy(&value, args, 4);
            state_.text_attr = (state_.text_attr & ~0x3Cu) | (u32(value) * 4 & 0x3Cu);
            return Flow::Advance;
        }
        default:
            // 未知 opcode：按 arg_bytes 步进不中断（与原版 switch 落空同效）。
            return Flow::Advance;
    }
}

}  // namespace th13
