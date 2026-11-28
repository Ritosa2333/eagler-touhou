#include "RouteChoiceHost.hpp"
#include <cmath>
#include <limits>
#include <utility>

namespace th128::routechoice {
namespace {
struct Guard {
    bool& state;
    explicit Guard(bool& value) : state(value) { state = true; }
    ~Guard() { state = false; }
};
}

void Host::runtime() {
    scene_.runtime = {};
    scene_.runtime.instructionBudget = 4096;
    scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.unsupported = [this](int bank, int script, int opcode, const char* reason) {
        if (failure_.empty())
            failure_ = "route choice ANM bank=" + std::to_string(bank) +
                       " script=" + std::to_string(script) +
                       " opcode=" + std::to_string(opcode) + ": " + reason;
    };
}

bool Host::fail(std::string reason, std::string& error) {
    if (failure_.empty())
        failure_ = std::move(reason);
    status_.phase = Phase::Failed;
    status_.handle = 0;
    status_.confirmed = false;
    result_.reset();
    scene_.reset();
    snapshots_.clear();
    started_ = false;
    error = failure_;
    return false;
}

bool Host::checked(std::string& error) {
    if (!failure_.empty())
        return fail(failure_, error);
    std::string reason;
    for (const auto& [id, node] : scene_.nodes()) {
        if (node->vm.faulted()) {
            reason = "route choice ANM execution stopped";
            break;
        }
        if (node->detached || node->customGeometry || node->beforeTick || node->onInterrupt) {
            reason = "unsupported route choice ANM ownership or geometry";
            break;
        }
        const auto& state = node->vm.state();
        for (const float value : {state.x, state.y, state.z, state.sx, state.sy,
                                  state.engineX, state.engineY, state.offsetX, state.offsetY}) {
            if (!std::isfinite(value)) {
                reason = "nonfinite route choice ANM pose";
                break;
            }
        }
        if (!reason.empty())
            break;
        if (state.sprite >= 0) {
            const auto bank = scene_.registry.bank(state.spriteBank);
            if (!bank || std::size_t(state.sprite) >= bank->sprites.size()) {
                reason = "route choice ANM sprite binding unavailable";
                break;
            }
        }
        if (state.ended || state.deleteRequested)
            node->alive = false;
    }
    if (!reason.empty())
        return fail(std::move(reason), error);
    scene_.collect();
    return true;
}

void Host::refresh() {
    snapshots_.clear();
    const auto bank = scene_.registry.bank(BankId);
    for (const auto* node : scene_.chain(anm::Membership::Primary)) {
        Snapshot item;
        item.id = node->id;
        item.parent = node->parent;
        item.state = node->vm.state();
        if (bank && item.state.sprite >= 0 && std::size_t(item.state.sprite) < bank->sprites.size())
            item.sprite = bank->sprites[std::size_t(item.state.sprite)];
        item.x = item.state.x + item.state.engineX + item.state.offsetX;
        item.y = item.state.y + item.state.engineY + item.state.offsetY;
        item.z = item.state.z + item.state.engineZ + item.state.offsetZ;
        if (node->parent) {
            const auto found = scene_.nodes().find(node->parent);
            if (found != scene_.nodes().end()) {
                const auto& owner = found->second->vm.state();
                item.x += owner.x + owner.engineX + owner.offsetX;
                item.y += owner.y + owner.engineY + owner.offsetY;
                item.z += owner.z + owner.engineZ + owner.offsetZ;
            }
        }
        snapshots_.push_back(std::move(item));
    }
}

bool Host::load(std::span<const std::uint8_t> bytes, std::string& error) {
    error.clear();
    if (!failure_.empty())
        return fail(failure_, error);
    if (loaded_ || started_ || executing_)
        return fail("front route choice resource replacement requires a fresh host", error);
    anm::Registry candidate;
    if (!candidate.loadBank(BankId, bytes.data(), bytes.size(), error)) {
        if (failure_.empty())
            failure_ = error.empty() ? "front route choice bank loading failed" : error;
        return fail(failure_, error);
    }
    const auto bank = candidate.bank(BankId);
    if (!bank || bank->scripts.size() <= 169 || bank->sprites.size() <= 113) {
        return fail("actual front scripts166/167/168/169 and sprite113 are required", error);
    }
    for (const std::size_t index : {166u, 167u, 168u, 169u}) {
        if (bank->scripts[index].commands.empty())
            return fail("required front route choice script is empty", error);
    }
    bool spawn166 = false, spawn167 = false, spawn168 = false;
    for (const auto& command : bank->scripts[169].commands) {
        if (command.op != 88 || command.words.empty())
            continue;
        const auto script = std::uint32_t(command.words[0]);
        spawn166 = spawn166 || script == 166u;
        spawn167 = spawn167 || script == 167u;
        spawn168 = spawn168 || script == 168u;
    }
    if (!spawn166 || !spawn167 || !spawn168)
        return fail("front169 route choice child creation is unavailable", error);
    scene_.reset();
    scene_.registry = std::move(candidate);
    loaded_ = true;
    status_ = Status{};
    result_.reset();
    runtime();
    refresh();
    return true;
}

bool Host::start(const StartContext& context, std::string& error) {
    error.clear();
    if (!failure_.empty())
        return fail(failure_, error);
    if (!loaded_)
        return fail("front route choice bank is not loaded", error);
    if (started_ && status_.phase != Phase::Done)
        return fail("route choice host restart requires a completed or destroyed host", error);
    if (context.stage != 1 && context.stage != 6 && context.stage != 11)
        return fail("route choice is not defined for the supplied stage", error);
    started_ = false;
    scene_.reset();
    status_ = Status{};
    status_.stage = context.stage;
    status_.message78 = context.message78;
    status_.message80 = context.message80;
    status_.phase = Phase::Intro;
    result_.reset();
    auto* root = scene_.bind(RootKey, RootKey, BankId, 169, anm::Membership::Primary,
                             0, 0, {}, false, nullptr, 0, 0, 23);
    if (!root)
        return fail("front169 route choice root binding failed", error);
    if (!checked(error)) {
        if (failure_.empty())
            failure_ = error;
        return false;
    }
    status_.handle = RootKey;
    started_ = true;
    refresh();
    return true;
}

bool Host::selectChild(int script, int sprite, std::string& error) {
    std::uint32_t id = 0;
    for (const auto* child : scene_.children(RootKey)) {
        if (child->vm.state().script != script)
            continue;
        if (id)
            return fail("front route choice child is ambiguous", error);
        id = child->id;
    }
    if (!id)
        return fail("required front route choice child is missing", error);
    const auto bank = scene_.registry.bank(BankId);
    if (!bank || sprite < 0 || std::size_t(sprite) >= bank->sprites.size())
        return fail("required front route select sprite is unavailable", error);
    auto& state = scene_.nodes().at(id)->vm.control();
    state.sprite = sprite;
    state.spriteBank = BankId;
    state.spriteWidth = bank->sprites[std::size_t(sprite)].width;
    state.spriteHeight = bank->sprites[std::size_t(sprite)].height;
    state.visible = true;
    state.flags |= 1u;
    return true;
}

bool Host::enterSelect(std::string& error) {
    int first = 0, second = 0;
    if (status_.stage == 1) {
        first = 0x70;
        second = 0x71;
    } else if (status_.stage == 6) {
        first = 0x6f;
        second = 0x71;
    } else if (status_.stage == 11) {
        first = 0x6f;
        second = 0x70;
    } else {
        return fail("route choice stage lost its original selector", error);
    }
    status_.message74 = 1;
    status_.message80 = 3;
    status_.message78 = 2;
    if (!selectChild(167, first, error))
        return false;
    if (!selectChild(168, second, error))
        return false;
    status_.phase = Phase::Select;
    return true;
}

void Host::recordInterrupt(int code) {
    status_.lastInterrupt = std::uint32_t(code);
    ++status_.interruptCount;
}

bool Host::tick(Input input, std::string& error) {
    error.clear();
    if (!failure_.empty())
        return fail(failure_, error);
    if (!started_)
        return fail("route choice host is not started", error);
    if (executing_)
        return fail("route choice host reentered", error);
    Guard guard(executing_);
    if (status_.phase == Phase::Done)
        return true;
    const auto advance = [&]() -> bool {
        scene_.tickPrimary();
        return checked(error);
    };
    if (status_.phase == Phase::Releasing) {
        if (!advance())
            return false;
        if (!scene_.find(RootKey)) {
            status_.phase = Phase::Done;
            status_.handle = 0;
        }
        refresh();
        return true;
    }
    if (!scene_.find(RootKey))
        return fail("route choice ANM handle was lost before confirmation", error);
    if (!advance())
        return false;
    if (!scene_.find(RootKey))
        return fail("route choice ANM handle was lost before confirmation", error);
    if (status_.phase == Phase::Intro) {
        if (status_.message70 == 41 && !enterSelect(error))
            return false;
        ++status_.message70;
        refresh();
        return true;
    }
    if (status_.phase == Phase::Select) {
        if (status_.message70 >= 8) {
            const auto previous = status_.message78;
            status_.message7c = previous;
            if (input.pressed & Up) {
                status_.message78 = 0;
                status_.message74 = 2;
            }
            if (input.pressed & Down) {
                status_.message78 = 1;
                status_.message74 = 2;
            }
            if (previous != status_.message78) {
                const int code = status_.message78 + 7;
                scene_.interrupt(RootKey, code, false);
                recordInterrupt(code);
                status_.message70 = 0;
                status_.lastSound = 0xau;
                ++status_.soundCount;
            }
        }
        if (status_.message74 == 2 && (input.pressed & Confirm)) {
            const int code = status_.message78 + 9;
            scene_.interrupt(RootKey, code, false);
            recordInterrupt(code);
            status_.message70 = 0;
            status_.message74 = 3;
            status_.phase = Phase::Confirm;
            status_.lastSound = 7u;
            ++status_.soundCount;
        }
        ++status_.message70;
        refresh();
        return true;
    }
    if (status_.phase == Phase::Confirm) {
        if (status_.message70 >= 40) {
            scene_.interrupt(RootKey, 1, false);
            recordInterrupt(1);
            status_.handle = 0;
            status_.confirmed = true;
            result_ = status_.message78;
            status_.phase = Phase::Releasing;
        }
        ++status_.message70;
        refresh();
        return true;
    }
    return fail("invalid route choice phase", error);
}

bool Host::destroy(std::string& error) {
    error.clear();
    if (!failure_.empty())
        return fail(failure_, error);
    if (started_ && !status_.confirmed)
        return fail("route choice host destroyed before confirmation", error);
    scene_.reset();
    snapshots_.clear();
    status_.handle = 0;
    if (status_.confirmed)
        status_.phase = Phase::Done;
    else
        status_.phase = Phase::Idle;
    started_ = false;
    return true;
}

}
