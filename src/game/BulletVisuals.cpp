#include "BulletVisuals.hpp"
#include <utility>

namespace th128::bullet {

void BulletVisuals::setRandomSources(std::function<uint32_t()> game, std::function<uint32_t()> display) {
    gameRandom32_ = std::move(game);
    displayRandom32_ = std::move(display);
}

bool BulletVisuals::load(const void* bytes, size_t size, std::string& error) {
    error.clear();
    if (!registry_.loadBank(BankId, static_cast<const uint8_t*>(bytes), size, error))
        return false;
    generation_ = std::make_shared<const uint8_t>(0);
    return true;
}

bool BulletVisuals::valid(const Visual& visual, std::string& error) const {
    if (!generation_ || visual.generation_.lock() != generation_) {
        error = "bullet visual belongs to an unloaded, foreign or stale bank";
        return false;
    }
    return checked(visual, error);
}

bool BulletVisuals::checked(const Visual& visual, std::string& error) const {
    if (!visual.failure_.empty()) {
        error = visual.failure_;
        return false;
    }
    for (const auto* machine : {&visual.primary, visual.hasSecondary ? &visual.secondary : nullptr}) {
        if (machine && machine->faulted()) {
            const auto diagnostic = machine->diagnostics();
            error = "ANM script=" + std::to_string(machine->state().script) +
                    " opcode=" + std::to_string(diagnostic.faultOpcode) + ": " + diagnostic.reason;
            return false;
        }
    }
    return true;
}

anm::Runtime BulletVisuals::runtimeFor(Visual& visual, bool secondary) {
    anm::Runtime runtime;
    if (gameRandom32_)
        runtime.gameRandom32 = [this] { return gameRandom32_(); };
    if (displayRandom32_)
        runtime.displayRandom32 = [this] { return displayRandom32_(); };
    runtime.unsupported = [&visual](int bank, int script, int opcode, const char* reason) {
        if (visual.failure_.empty())
            visual.failure_ = "ANM bank=" + std::to_string(bank) + " script=" + std::to_string(script) +
                              " opcode=" + std::to_string(opcode) + ": " + reason;
    };
    runtime.remapSprite = [this, &visual, secondary](int source) {
        int result = source;
        if (!secondary) {
            if (visual.identityRemap_) {
                result = source;
            } else {
                if (source < 0 || size_t(source) >= visual.remap_.size()) {
                    if (visual.failure_.empty())
                        visual.failure_ = "unsupported bullet sprite callback input=" + std::to_string(source);
                    return 0;
                }
                result = visual.remap_[source];
            }
        }
        if (!spriteInfo(result)) {
            if (visual.failure_.empty())
                visual.failure_ = "invalid bullet sprite=" + std::to_string(result);
            return 0;
        }
        return result;
    };
    return runtime;
}

std::optional<BulletVisuals::Visual> BulletVisuals::create(const CreateInfo& info, std::string& error) {
    return createInternal(info, {}, error);
}

std::optional<BulletVisuals::Visual> BulletVisuals::createInitialExtension1048576(const CreateInfo& info,
    const InitialExtensionOwner& initialize, std::string& error) {
    error.clear();
    if (info.type != 20 || info.script != 74 || info.secondaryScript || info.birthInterrupt != 2
        || info.deferPostBindAdvance || info.identitySpriteRemap || !initialize) {
        error = "initial1048576 visual bridge requires original type20/74 primary and initialization owner";
        return std::nullopt;
    }
    return createInternal(info, initialize, error);
}

std::optional<BulletVisuals::Visual> BulletVisuals::createInternal(const CreateInfo& info,
    const InitialExtensionOwner& initialize, std::string& error) {
    error.clear();
    const auto bank = registry_.bank(BankId);
    if (!generation_ || !bank) {
        error = "bullet ANM bank is not loaded";
        return std::nullopt;
    }
    if ((info.type != 4&&info.type != 6&&info.type != 8&&info.type!=9&&info.type!=10&&info.type!=13&&info.type!=16&&info.type!=18&&info.type!=20&&info.type!=21&&info.type!=23&&info.type!=24&&info.type!=25&&info.type!=29&&info.type!=30&&info.type!=32&&info.type!=33) || info.color < 0 || info.color > 15||(info.type==4&&info.script!=39)||(info.type==6&&info.script!=41)||(info.type==9&&info.script!=44)||(info.type==10&&info.script!=45)||(info.type==13&&info.script!=48)||(info.type==16&&info.script!=51)||(info.type==18&&info.script!=72)||(info.type==20&&info.script!=74)||(info.type==21&&info.script!=75)||(info.type==23&&info.script!=77)||(info.type==24&&info.script!=283)||(info.type==25&&info.script!=102)||(info.type==29&&info.script!=282)||(info.type==30&&info.script!=285)||(info.type==32&&info.script!=54)||(info.type==33&&info.script!=55)) {
        error = "only proven bullet8/18 profile/ANM bindings are supported";
        return std::nullopt;
    }
    if(info.type==24&&(!info.secondaryScript||*info.secondaryScript!=284)){
        error="type24 requires its original secondary ANM 284";return std::nullopt;
    }
    if(info.type==25&&(!info.secondaryScript||*info.secondaryScript!=103)){
        error="type25 requires its original secondary ANM 103";return std::nullopt;
    }
    if(info.type==30&&(!info.secondaryScript||*info.secondaryScript!=286)){
        error="type30 requires its original secondary ANM 286";return std::nullopt;
    }
    auto scriptExists = [&bank](int script) { return script >= 0 && size_t(script) < bank->scripts.size(); };
    if (!scriptExists(info.script) || (info.secondaryScript && !scriptExists(*info.secondaryScript))) {
        error = "invalid bullet profile ANM script";
        return std::nullopt;
    }
    if(!info.identitySpriteRemap)
        for (const auto sprite : info.spriteRemap) {
            if (!spriteInfo(sprite)) {
                error = "invalid bullet profile sprite remap=" + std::to_string(sprite);
                return std::nullopt;
            }
        }
    const bool a2BlueDeflectionRemap = info.color == 6
        && info.spriteRemap == std::array<int32_t, 4>{70, 326, 342, 349};
    if (info.deferPostBindAdvance && (info.type != 4 || (info.color != 13 && !a2BlueDeflectionRemap) || info.script != 39
        || info.secondaryScript.has_value() || info.birthInterrupt != 0 || info.identitySpriteRemap)) {
        error = "deferred conversion advance requires exact type4/color13 or Blue Deflection color6 primary";
        return std::nullopt;
    }
    Visual visual;
    visual.generation_ = generation_;
    visual.remap_ = info.spriteRemap;
    visual.identityRemap_ = info.identitySpriteRemap;
    visual.hasSecondary = info.secondaryScript.has_value();
    auto primaryRuntime = runtimeFor(visual, false);
    if (!visual.primary.bind(registry_, BankId, info.script, primaryRuntime) || !checked(visual, error))
        return std::nullopt;
    if (visual.hasSecondary) {
        auto secondaryRuntime = runtimeFor(visual, true);
        if (!visual.secondary.bind(registry_, BankId, *info.secondaryScript, secondaryRuntime) || !checked(visual, error))
            return std::nullopt;
    }
    if (info.birthInterrupt != 0)
        visual.primary.interrupt(info.birthInterrupt);
    if (initialize && (!initialize(visual, error) || !error.empty())) {
        if (error.empty()) error = "initial1048576 actual post-bind owner rejected";
        return std::nullopt;
    }
    if (!info.deferPostBindAdvance && !tick(visual, true, error))
        return std::nullopt;
    return visual;
}

bool BulletVisuals::convertType25(Visual& visual,int color,std::string& error){
    error.clear();
    if(!valid(visual,error)||color<0||color>15){if(error.empty())error="type25 conversion stale visual/color owner";return false;}
    const auto bank=registry_.bank(BankId);
    if(!bank||bank->scripts.size()<=103){error="type25 conversion requires real102/103 programs";return false;}
    Visual candidate;
    candidate.generation_=generation_;candidate.identityRemap_=true;candidate.hasSecondary=true;
    auto primaryRuntime=runtimeFor(candidate,false);
    if(!candidate.primary.bind(registry_,BankId,102,primaryRuntime)||!checked(candidate,error))return false;
    auto secondaryRuntime=runtimeFor(candidate,true);
    if(!candidate.secondary.bind(registry_,BankId,103,secondaryRuntime)||!checked(candidate,error))return false;
    visual=std::move(candidate);
    return true;
}
bool BulletVisuals::interrupt(Visual& visual, int code, std::string& error) {
    error.clear();
    if (!valid(visual, error))
        return false;
    visual.primary.interrupt(code);
    return true;
}

bool BulletVisuals::tick(Visual& visual, bool secondaryUnconditional, std::string& error) {
    auto primaryRuntime = runtimeFor(visual, false);
    visual.primary.tick(primaryRuntime);
    if (!checked(visual, error))
        return false;
    if (visual.hasSecondary && (secondaryUnconditional
        || (!visual.primary.state().ended && !visual.primary.state().deleteRequested))) {
        auto secondaryRuntime = runtimeFor(visual, true);
        visual.secondary.tick(secondaryRuntime);
    }
    return checked(visual, error);
}

bool BulletVisuals::advance(Visual& visual, uint32_t ticks, std::string& error) {
    error.clear();
    if (!valid(visual, error))
        return false;
    if (ticks > MaxAdvanceTicks) {
        error = "bullet visual tick budget exceeded";
        return false;
    }
    for (uint32_t index = 0; index < ticks; ++index) {
        if (!tick(visual, false, error))
            return false;
    }
    return true;
}

const anm::State* BulletVisuals::animationState(const Visual& visual, bool secondary) const {
    if (!generation_ || visual.generation_.lock() != generation_ || visual.faulted() ||
        (secondary && !visual.hasSecondary))
        return nullptr;
    return secondary ? &visual.secondary.state() : &visual.primary.state();
}

const anm::SpriteInfo* BulletVisuals::spriteInfo(int sprite) const {
    const auto bank = registry_.bank(BankId);
    return bank && sprite >= 0 && size_t(sprite) < bank->sprites.size() ? &bank->sprites[sprite] : nullptr;
}

}
