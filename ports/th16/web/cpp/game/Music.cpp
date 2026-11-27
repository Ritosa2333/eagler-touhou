#include "Music.hpp"

#include <cstring>

namespace th16 {

namespace {
constexpr size_t kEntrySize = 0x34;
}  // namespace

bool MusicManager::load_table(const u8* data, size_t size) {
    entries.clear();
    for (size_t off = 0; off + kEntrySize <= size; off += kEntrySize) {
        const u8* e = data + off;
        if (e[0] == 0) break;   // 表尾空条目
        BgmEntry entry;
        entry.name.assign(reinterpret_cast<const char*>(e), strnlen(reinterpret_cast<const char*>(e), 16));
        entry.offset = read_u32(e + 0x10);
        entry.size = read_u32(e + 0x14);
        entry.loop_start = read_u32(e + 0x18);
        entry.length = read_u32(e + 0x1c);
        entry.format_tag = read_u16(e + 0x20);
        entry.channels = read_u16(e + 0x22);
        entry.sample_rate = read_u32(e + 0x24);
        entry.bytes_per_second = read_u32(e + 0x28);
        entry.block_align = read_u16(e + 0x2c);
        entry.bits_per_sample = read_u16(e + 0x2e);
        entries.push_back(std::move(entry));
    }
    return !entries.empty();
}

// 0x45ed00：队列满（0x1f）时丢弃
void MusicManager::enqueue(i32 kind, i32 track, const char* name) {
    if (i32(queue.size()) >= kQueueCapacity) return;
    Command c;
    c.kind = kind;
    c.track = track;
    c.name = name != nullptr ? name : "";
    queue.push_back(std::move(c));
}

void MusicManager::register_track(i32 slot, const char* name) {
    enqueue(1, slot, (std::string(name) + ".wav").c_str());
}

void MusicManager::play_slot(i32 slot) { enqueue(2, slot, "dummy"); }

void MusicManager::stop_track() { enqueue(3, 0, "dummy"); }

void MusicManager::fade_out(float seconds, float game_speed) {
    if (game_speed != 0.0f && game_speed <= 1.0f) seconds = seconds / game_speed;
    enqueue(5, i32(seconds), "");
}

// 0x45da30：取最后一个 '/' 或 '\\' 之后的部分与表中各条目名比较；走到表尾空条目时 CMOVNZ 不取下标 → 返回 0
i32 MusicManager::find_track(const std::string& name) const {
    size_t cut = name.find_last_of("/\\");
    const std::string base = cut == std::string::npos ? name : name.substr(cut + 1);
    for (size_t i = 0; i < entries.size(); ++i)
        if (entries[i].name == base) return i32(i);
    return 0;
}

const BgmEntry& MusicManager::entry(i32 index) const {
    static const BgmEntry kEmpty{};
    if (index < 0 || index >= i32(entries.size())) return kEmpty;
    return entries[size_t(index)];
}

i32 MusicManager::ds_volume(i32 base) const {
    if (volume == 0) return -10000;
    const float inv = 1.0f - float(volume) / 100.0f;
    return i32((1.0f - inv * inv) * float(base + 5000)) - 5000;
}

void MusicManager::tick_fade() {
    if (!streamer_live || fade_mode != 1) return;
    if (--fade_remaining < 1) {
        fade_mode = 0;
        if (output != nullptr) output->halt();
        return;
    }
    if (output != nullptr) output->set_volume(fade_remaining * 5000 / fade_total - 5000);
}

i32 MusicManager::step() {
    if (!ready) return 0;
    if (queue.empty()) return 0;
    Command& c = queue.front();
    bool done = false;
    switch (c.kind) {
    case 1:
        if (c.track >= 0 && c.track < kSlotCount) slot_names[size_t(c.track)] = c.name;
        done = true;
        break;
    case 2:
        if (!streamer_live) { done = true; break; }
        switch (c.step) {
        case 0:
            fade_mode = 0;   // 0x471270 置 +0x1c = 0
            if (output != nullptr) output->stop(false);
            ++c.step;
            break;
        case 1: {
            current = (c.track < 0 || c.track >= kSlotCount) ? c.name : slot_names[size_t(c.track)];
            c.track = find_track(current);
            if (output != nullptr) output->open(entry(c.track));
            ++c.step;
            break;
        }
        case 2:
            if (output != nullptr) output->seek(entry(c.track), 0);
            ++c.step;
            break;
        case 3:
            if (output != nullptr) output->play(entry(c.track).length != 0, 0);
            ++c.step;
            break;
        case 4:
            if (output != nullptr) output->start();
            ++c.step;
            break;
        case 5:
        case 6:
            ++c.step;
            break;
        default:
            done = true;
            break;
        }
        break;
    case 3:
        if (!streamer_live) { done = true; break; }
        if (c.step == 0) {
            fade_mode = 0;
            if (output != nullptr) output->stop(true);
            ++c.step;
        } else if (c.step == 1) {
            done = true;
        } else {
            ++c.step;
        }
        break;
    case 4:
        // 声音线程在端口里不存在（DAT_004d9e24 = 0）：stop 后下一步即出队，串流器保留
        if (!streamer_live) { done = true; break; }
        if (c.step == 0) {
            fade_mode = 0;
            if (output != nullptr) output->stop(true);
            ++c.step;
        } else {
            done = true;
        }
        break;
    case 5:
        if (streamer_live) {
            fade_mode = 1;
            fade_remaining = fade_total = i32(float(c.track) * 60.0f);
            if (output != nullptr) output->fade(fade_total);
        }
        done = true;
        break;
    case 6:
        if (enabled && output != nullptr) output->pause();
        done = true;
        break;
    case 7:
        if (enabled && output != nullptr) output->resume();
        done = true;
        break;
    case 8:
        if (streamer_live && output != nullptr) output->set_volume(0);
        done = true;
        break;
    case 9:
        if (output != nullptr) output->switch_loop(entry(find_track(c.name)));
        done = true;
        break;
    default:
        return head_kind();
    }
    if (done) queue.erase(queue.begin());
    return head_kind();
}

}  // namespace th16
