#include "HostTextVmBridge.hpp"

namespace th128::ending::hosttext {

bool HostTextVmBridge::load(int index, std::span<const std::uint8_t> message,
                            std::span<const std::uint8_t> anim, std::span<const std::uint8_t> baseAnim,
                            std::span<const std::uint8_t> textBank, std::string& error) {
    return loadOwned(index,message,anim,baseAnim,textBank,true,error);
}

bool HostTextVmBridge::loadOrdinary(const th128::record::NativeOrdinaryEndingPermit& permit,
        std::span<const std::uint8_t> message,std::span<const std::uint8_t> anim,
        std::span<const std::uint8_t> baseAnim,std::span<const std::uint8_t> textBank,std::string& error){
    if(!permit.ready()){error="ordinary text bridge requires its retained native ending owner";return false;}
    return loadOwned(int(permit.identity().route),message,anim,baseAnim,textBank,false,error);
}

bool HostTextVmBridge::loadOwned(int index,std::span<const std::uint8_t> message,
        std::span<const std::uint8_t> anim,std::span<const std::uint8_t> baseAnim,
        std::span<const std::uint8_t> textBank,bool debugTainted,std::string& error){
    error.clear();
    if (loaded_ && !cleaned_) {
        error = "host text bridge is not clean";
        return false;
    }
    state_ = BridgeState{};
    childSpawns_ = 0;
    if (!registry_.loadBank(0, textBank.data(), textBank.size(), error)) {
        state_.failed = true;
        state_.error = error;
        return false;
    }
    auto bank = registry_.bank(0);
    if (!bank || bank->scripts.size() < 73 || bank->sprites.size() < 9) {
        error = "text.anm lacks original68..72scripts and4..8sprites";
        state_.failed = true;
        state_.error = error;
        return false;
    }
    if (!scene_.load(index, message, anim, baseAnim, debugTainted, error)) {
        state_.failed = true;
        state_.error = error;
        return false;
    }
    for (int slot = 0; slot < 5; ++slot) {
        auto& runtime = runtimes_[std::size_t(slot)];
        runtime.spawnChild = [this](const th128::anm::ChildRequest& request) { request.failure="ending text has unowned original child request";state_.failed=true;state_.error=request.failure; };
        runtime.unsupported = [this](int bankId, int scriptId, int opcode, const char* reason) {state_.failed=true;state_.error="ending text bank="+std::to_string(bankId)+" script="+std::to_string(scriptId)+" opcode="+std::to_string(opcode)+": "+reason;};
        if (!texts_[std::size_t(slot)].bind(registry_, 0, 68+slot, runtime)) {
            error = "text.anm script bind failed";
            state_.failed = true;
            state_.error = error;
            return false;
        }
    }
    loaded_ = true;
    cleaned_ = false;
    collectNewOp3();
    refresh();
    return true;
}

void HostTextVmBridge::collectNewOp3() {
    const auto& contexts = scene_.state().op3;
    while (state_.consumedOp3 < scene_.state().executedOp3) {
        const auto& context = contexts[state_.consumedOp3];
        ++state_.consumedOp3;
        if (context.slot < 0 || context.slot > 4) continue;
        if(context.slot==0)for(auto& text:texts_)text.interrupt(3);
        texts_[std::size_t(context.slot)].interrupt(2);
        ++state_.irqQueued;
    }
}

bool HostTextVmBridge::configureServices(MediaHost media,FadeHost fade,std::string& error){
    return scene_.configureServices(std::move(media),std::move(fade),error);
}

bool HostTextVmBridge::configureOriginalInput(std::uint32_t sceneFlags20, std::function<bool(std::string&)> confirmSound, std::string& error) {
    return scene_.configureOriginalInput(sceneFlags20, std::move(confirmSound), error);
}

bool HostTextVmBridge::tick(bool confirm, std::string& error) {
    return tick(message::Input{confirm ? 1u : 0u, confirm ? 1u : 0u, 0}, error);
}

bool HostTextVmBridge::tick(message::Input input, std::string& error) {
    error.clear();
    if (state_.failed) {
        error = state_.error;
        return false;
    }
    if (!loaded_) {
        error = "host text bridge is not loaded";
        return false;
    }
    for (int slot = 0; slot < 5; ++slot){
        (void)texts_[std::size_t(slot)].tick(runtimes_[std::size_t(slot)]);
        if(texts_[std::size_t(slot)].faulted()||state_.failed){error=state_.error.empty()?"ending actualtextVMfault":state_.error;return false;}
    }
    if (!scene_.tick(input, error)) {
        state_.failed = true;
        state_.error = error;
        refresh();
        return false;
    }
    collectNewOp3();
    refresh();
    return true;
}

bool HostTextVmBridge::reset(std::string& error) {
    error.clear();
    dispose();
    return true;
}

void HostTextVmBridge::dispose() {
    scene_.cleanup();
    for (int slot = 0; slot < 5; ++slot) {
        texts_[std::size_t(slot)] = th128::anm::VM{};
        runtimes_[std::size_t(slot)] = th128::anm::Runtime{};
    }
    registry_.unloadBank(0);
    state_ = BridgeState{};
    state_.normalModeClear = false;
    state_.scene = scene_.state();
    loaded_ = false;
    cleaned_ = true;
}

void HostTextVmBridge::refresh() {
    state_.scene = scene_.state();
    state_.normalModeClear = false;
    for (int slot = 0; slot < 5; ++slot) {
        const auto& state = texts_[std::size_t(slot)].state();
        auto& receipt = state_.texts[std::size_t(slot)];
        receipt.slot = slot;
        receipt.script = state.script;
        receipt.sprite = state.sprite;
        receipt.x = state.x;
        receipt.y = state.y;
        receipt.layer = state.layer;
        receipt.scaleX = state.sx;
        receipt.scaleY = state.sy;
        receipt.visible = state.visible;
        receipt.held = state.held;
        receipt.ended = state.ended;
        receipt.alphaAvailable = true;
        receipt.alpha=float((state.primary>>24)&255u)/255.f;
        receipt.clock = state.clock;
    }
}

}
