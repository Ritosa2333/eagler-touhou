#include "EndingSceneHost.hpp"

#include <algorithm>
#include <bit>
#include <cstring>
#include <string_view>

namespace th128::ending {
namespace {

constexpr int kBaseBank = 0x20;
constexpr int kEndingBank = 0x21;

std::uint32_t word(std::span<const std::uint8_t> bytes, std::size_t offset) {
    return std::uint32_t(bytes[offset]) | (std::uint32_t(bytes[offset + 1]) << 8) |
           (std::uint32_t(bytes[offset + 2]) << 16) | (std::uint32_t(bytes[offset + 3]) << 24);
}

bool contains(std::span<const std::uint8_t> bytes, std::string_view needle) {
    return !needle.empty() && std::search(bytes.begin(), bytes.end(), needle.begin(), needle.end()) != bytes.end();
}

bool safeRelative(std::string_view path) {
    if (path.empty() || path.size() > 96 || path.find("..") != std::string_view::npos ||
        path.find('\\') != std::string_view::npos || path.front() == '/')
        return false;
    for (const char character : path) {
        if (character < 0x20 || character > 0x7e) return false;
    }
    return true;
}

bool shape(int opcode, std::size_t size) {
    switch (opcode) {
    case 0:
    case 4:
    case 11:
    case 12:
        return size == 0;
    case 5:
    case 6:
    case 9:
    case 14:
        return size == 4;
    case 8:
        return size == 12;
    case 7:
    case 10:
        return size >= 5 && size <= 96;
    case 3:
        return size > 0 && size <= 256;
    case 13:
    case 15:
    case 16:
    case 17:
        return size == 0 || size == 4;
    default:
        return false;
    }
}

std::string stringFrom(std::span<const std::uint8_t> raw, std::size_t start) {
    std::string value;
    for (std::size_t index = start; index < raw.size(); ++index) {
        if (raw[index] == 0) break;
        value.push_back(char(raw[index]));
    }
    return value;
}

}

bool SceneHost::fail(const std::string& reason, std::string& error) {
    state_.failed = true;
    state_.error = reason;
    error = reason;
    return false;
}

bool SceneHost::parse(std::span<const std::uint8_t> message, std::span<const std::uint8_t> anm,
                      std::span<const std::uint8_t> baseAnm, std::string& error) {
    if (message.size() < 16 || word(message, 0) != 1 || word(message, 4) != 12)
        return fail("ending MSG header is not the original single-entry form", error);
    if (!contains(baseAnm, "THTX") || !contains(baseAnm, "ending/ebase.png"))
        return fail("ending base ANM texture is missing", error);
    if (!contains(anm, "THTX") || !contains(anm, "ending/e0"))
        return fail("ending ANM texture is missing", error);
    if (!scene_.registry.loadBank(kBaseBank, baseAnm.data(), baseAnm.size(), error))
        return fail("base ANM bank load failed: " + error, error);
    if (!scene_.registry.loadBank(kEndingBank, anm.data(), anm.size(), error))
        return fail("ending ANM bank load failed: " + error, error);
    boundBanks_ = true;
    std::size_t cursor = 12;
    int textSlot = 0;
    std::vector<Command> parsed;
    bool finish = false;
    bool baseSeen = false;
    bool endingSeen = false;
    while (cursor + 4 <= message.size()) {
        Command command;
        command.time = int(message[cursor] | (message[cursor + 1] << 8));
        command.opcode = message[cursor + 2];
        const std::size_t length = message[cursor + 3];
        cursor += 4;
        if (cursor + length > message.size())
            return fail("ending MSG command exceeds the actual asset", error);
        if (!shape(command.opcode, length))
            return fail("ending MSG has an unknown or malformed opcode shape", error);
        command.bytes.assign(message.begin() + cursor, message.begin() + cursor + length);
        cursor += length;
        if (command.opcode == 7) {
            if (command.bytes.size() < 5)
                return fail("ending ANM resource command is too short", error);
            const auto prefix = word(command.bytes, 0);
            command.path = stringFrom(command.bytes, 4);
            if (!safeRelative(command.path) || prefix > 1)
                return fail("ending ANM resource path is unsafe", error);
            if (command.path == "ebase.anm") {
                if (prefix != 0 || baseSeen) return fail("ending base ANM binding is invalid", error);
                baseSeen = true;
            } else if (command.path == "e0" + std::string(1, char('0' + state_.index)) + ".anm") {
                if (prefix != 1 || endingSeen) return fail("ending ANM binding is invalid", error);
                endingSeen = true;
            } else {
                return fail("ending ANM command references a non-bound resource", error);
            }
        }
        if (command.opcode == 3) {
            Op3Context context;
            context.time = command.time;
            context.ordinal = int(state_.op3.size());
            context.slot = textSlot;
            textSlot=(textSlot+1)%5;
            std::uint8_t key = 0x77, step = 7;
            for (const auto byte : command.bytes) {
                const auto decoded = std::uint8_t(byte ^ key);
                key = std::uint8_t(key + step);
                step = std::uint8_t(step + 16);
                if (decoded == 0) break;
                context.source.push_back(decoded);
            }
            state_.op3.push_back(std::move(context));
        }
        if(command.opcode==6)textSlot=0;
        parsed.push_back(std::move(command));
        if (command.opcode == 0) {
            finish = true;
            state_.finishTime = command.time;
            break;
        }
    }
    if (!finish || !baseSeen || !endingSeen)
        return fail("ending MSG lacks the two authored ANM resources or finish", error);
    commands_ = std::move(parsed);
    cursor_ = 0;
    return true;
}

void SceneHost::snapshot() {
    state_.draws.clear();
    for (const auto* node : scene_.chain(anm::Membership::Primary)) {
        if (!node || !node->alive) continue;
        const auto& vm = node->vm.state();
        SceneDraw draw;
        draw.id = node->id;
        draw.owner = node->owner;
        draw.bank = vm.spriteBank >= 0 ? vm.spriteBank : vm.bank;
        draw.script = vm.script;
        draw.x = vm.x + vm.engineX + vm.offsetX;
        draw.y = vm.y + vm.engineY + vm.offsetY;
        draw.z = vm.z + vm.engineZ + vm.offsetZ;
        draw.alpha = float((vm.primary >> 24) & 255u) / 255.f;
        draw.layer = vm.layer;
        draw.sprite = vm.sprite;
        draw.visible = vm.visible && !vm.ended;
        draw.ended = vm.ended;
        draw.sx = vm.sx;
        draw.sy = vm.sy;
        draw.anchorX = vm.anchorX;
        draw.anchorY = vm.anchorY;
        if (const auto bank = scene_.registry.bank(draw.bank); bank && vm.sprite >= 0 && std::size_t(vm.sprite) < bank->sprites.size()) {
            const auto& sprite = bank->sprites[std::size_t(vm.sprite)];
            draw.width = vm.spriteWidth >= 0 ? vm.spriteWidth : sprite.width;
            draw.height = vm.spriteHeight >= 0 ? vm.spriteHeight : sprite.height;
            draw.u0 = sprite.u0;
            draw.v0 = sprite.v0;
            draw.u1 = sprite.u1;
            draw.v1 = sprite.v1;
            draw.textureEntry = sprite.textureEntry;
        }
        state_.draws.push_back(draw);
    }
}

bool SceneHost::execute(const Command& command, message::Input input, std::string& error) {
    if (command.opcode == 7) {
        state_.events.push_back("anm:" + command.path);
        return true;
    }
    if (command.opcode == 8) {
        if (command.bytes.size() != 12) return fail("ending background command is malformed", error);
        const auto slot = word(command.bytes, 0);
        const auto bankSlot = word(command.bytes, 4);
        const auto script = word(command.bytes, 8);
        if (slot > 5 || bankSlot > 1 || script > 2)
            return fail("ending background binding is outside the authored banks/scripts", error);
        const int bank = bankSlot == 0 ? kBaseBank : kEndingBank;
        if (!scene_.registry.bank(bank))
            return fail("ending background bank is unavailable", error);
        if (!scene_.bind(0x100u + slot, 1u + slot, bank, int(script),
                         anm::Membership::Primary, 0, 0, {}, false, nullptr, 224, 16, 23, false, 0))
            return fail("ending background VM bind failed", error);
        state_.events.push_back("background:" + std::to_string(slot));
        return true;
    }
    if (command.opcode == 3) {
        ++state_.executedOp3;
        std::uint8_t key = 0x77, step = 7;
        for (const auto byte : command.bytes) {
            const auto decoded = std::uint8_t(byte ^ key);
            key = std::uint8_t(key + step);
            step = std::uint8_t(step + 16);
            if (decoded == 0) break;
            if (decoded >= 0x20 && decoded < 0x7f)
                state_.glyphText.push_back(char(decoded));
        }
        state_.events.push_back("glyph:" + std::to_string(command.bytes.size()));
        return true;
    }
    if (command.opcode == 9) {
        state_.textColor = int(word(command.bytes, 0));
        state_.events.push_back("motion:" + std::to_string(command.time));
        return true;
    }
    if (command.opcode == 10) {
        const auto music = stringFrom(command.bytes, 0);
        if (!safeRelative(music) || music.rfind("bgm/", 0) != 0)
            return fail("ending music path is unsafe", error);
        MediaRequest request{++state_.mediaSerial,8,music,0};
        if(!media_||!media_(request,error)||!error.empty())return fail(error.empty()?"originalendingBGM09playownerUNKNOWN":error,error);
        state_.events.push_back("music:" + music);
        return true;
    }
    if (command.opcode == 5 || command.opcode == 6) {
        const auto duration = std::bit_cast<std::int32_t>(word(command.bytes, 0));
        if (state_.waitRemaining <= 0) {
            state_.waitRemaining = duration;
            state_.waitPrevious = duration == INT32_MIN ? INT32_MAX : duration - 1;
            state_.waitAccumulator = float(duration);
        }
        state_.waitPrevious = state_.waitRemaining;
        state_.waitAccumulator -= 1.f;
        state_.waitRemaining = anm::nativeInt(double(state_.waitAccumulator));
        if (command.opcode == 5 && duration < 0) {
            state_.waitPrevious = 998;
            state_.waitRemaining = 999;
            state_.waitAccumulator = 999;
        }
        const bool pressed = (input.pressed & 0x80201u) != 0;
        const bool held = (state_.originalSceneFlags20 & 1u) == 0 && state_.pressSeen &&
            (state_.heldShoot >= 20 || state_.heldRapid >= 20) && state_.waitRemaining % 6 == 0;
        if (!pressed && state_.waitRemaining > 0 && !held) {
            state_.waitingConfirm = true;
            return true;
        }
        if (pressed || state_.waitRemaining <= 0) {
            if (!confirmSound_) return fail("original ending SE0 confirmation owner is not bound", error);
            if (!confirmSound_(error) || !error.empty()) return fail(error.empty() ? "original ending SE0 owner declined" : error, error);
            ++state_.confirmationSoundRequests;
        }
        state_.waitingConfirm = false;
        state_.waitPrevious = -1;
        state_.waitRemaining = 0;
        state_.waitAccumulator = 0;
        state_.events.push_back(std::string(held && !pressed ? "held-skip:" : "confirm-or-timeout:") + std::to_string(command.time));
        return true;
    }
    if (command.opcode == 11) {
        MediaRequest request{++state_.mediaSerial,5,"bgm/th128_09.wav",180};
        if(!media_||!media_(request,error)||!error.empty())return fail(error.empty()?"originalendingBGMfadeownerUNKNOWN":error,error);
        state_.events.push_back("fade-stage");
        return true;
    }
    if (command.opcode == 12) {
        state_.events.push_back("cleanup-effects");
        return true;
    }
    if(command.opcode==14){
        const int duration=int(word(command.bytes,0));
        if(!fade_||!fade_(duration,error)||!error.empty())return fail(error.empty()?"originalendingkind5ownerUNKNOWN":error,error);
        state_.effect14=duration;return true;
    }
    if (command.opcode == 13 || command.opcode == 15 || command.opcode == 16 || command.opcode == 17) {
        state_.effect14 = command.bytes.size() == 4 ? int(word(command.bytes, 0)) : 0;
        state_.events.push_back("effect:" + std::to_string(command.opcode) + "@" + std::to_string(command.time));
        return true;
    }
    if (command.opcode == 0) {
        state_.finished = true;
        state_.events.push_back("finish:" + std::to_string(command.time));
        return true;
    }
    return fail("ending presentation encountered an unowned opcode", error);
}

bool SceneHost::load(int index, std::span<const std::uint8_t> message,
                     std::span<const std::uint8_t> anm, std::span<const std::uint8_t> baseAnm,
                     bool debugTainted, std::string& error) {
    error.clear();
    if (state_.loaded && !state_.cleaned)
        return fail("ending scene host is not clean for reentry", error);
    state_ = SceneState{};
    state_.index = index;
    state_.debugTainted = debugTainted;
    state_.originalSceneFlags20 = inputSceneFlags20_;
    state_.normalModeClear = false;
    if (index < 0 || index > 5) return fail("ending scene index is outside 0..5", error);
    if (!parse(message, anm, baseAnm, error)) return false;
    cursor_ = 0;
    while (cursor_ < commands_.size() && commands_[cursor_].time == 0) {
        if (!execute(commands_[cursor_], message::Input{}, error)) return false;
        ++cursor_;
    }
    snapshot();
    state_.loaded = true;
    state_.clock = 1;
    return true;
}

bool SceneHost::configureServices(MediaHost media,FadeHost fade,std::string& error){
    error.clear();if(state_.loaded||!media||!fade)return fail("freshendingactualmediaandfadeownersrequired",error);
    media_=std::move(media);fade_=std::move(fade);return true;
}

bool SceneHost::configureOriginalInput(std::uint32_t sceneFlags20, std::function<bool(std::string&)> confirmSound, std::string& error) {
    error.clear();
    if (state_.loaded) return fail("ending input owner must be configured before fresh load", error);
    inputSceneFlags20_ = sceneFlags20;
    confirmSound_ = std::move(confirmSound);
    return true;
}

bool SceneHost::tick(bool confirm, std::string& error) {
    return tick(message::Input{confirm ? 1u : 0u, confirm ? 1u : 0u, 0}, error);
}

bool SceneHost::tick(message::Input input, std::string& error) {
    error.clear();
    if (!state_.loaded) return fail("ending scene host is not loaded", error);
    if (state_.failed) { error = state_.error; return false; }
    if (state_.finished) return fail("ending scene host already finished", error);
    state_.heldShoot = (input.holding & 1u) ? state_.heldShoot + 1u : 0;
    state_.heldRapid = (input.holding & 0x200u) ? state_.heldRapid + 1u : 0;
    if (input.pressed & 0x201u) state_.pressSeen = true;
    scene_.tickPrimary();
    snapshot();
    for (const auto& item : scene_.nodes()) {
        if (item.second && item.second->alive && item.second->vm.faulted())
            return fail("ending ANM VM faulted during presentation", error);
    }
    while (cursor_ < commands_.size() && commands_[cursor_].time <= state_.clock) {
        if (!execute(commands_[cursor_], input, error)) return false;
        if (state_.waitingConfirm && (commands_[cursor_].opcode == 5 || commands_[cursor_].opcode == 6)) return true;
        ++cursor_;
        if (state_.finished) return true;
    }
    ++state_.clock;
    return true;
}

bool SceneHost::switchTo(int index, std::span<const std::uint8_t> message,
                         std::span<const std::uint8_t> anm, std::span<const std::uint8_t> baseAnm,
                         std::string& error) {
    if (!state_.cleaned) return fail("ending scene switch requires cleanup first", error);
    return load(index, message, anm, baseAnm, state_.debugTainted, error);
}

void SceneHost::cleanup() {
    scene_.reset();
    if (boundBanks_) {
        scene_.registry.unloadBank(kBaseBank);
        scene_.registry.unloadBank(kEndingBank);
        boundBanks_ = false;
    }
    state_.draws.clear();
    state_.op3.clear();
    state_.loaded = false;
    state_.cleaned = true;
    state_.waitingConfirm = false;
}

}
