#include "RollbackSession.hpp"
#include <eagler/netplay/InputRepairBudget.hpp>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace th09::multiplayer {
bool RollbackSession::Begin(const Netplay::SessionConfig& session, std::uint64_t now, std::uint8_t inputDelay, Netplay::AdonisMode mode) {
    Clear();
    return Configure(session,now,inputDelay,mode);
}
bool RollbackSession::BeginMeasured(const Netplay::SessionConfig& session,std::uint64_t now,
                                  std::uint32_t requestedDelay,Netplay::AdonisMode mode,unsigned predictionReserve) {
    Clear();
    if(session.gameId!=9 || !startup_.Begin(session,now*1000,mode,requestedDelay,predictionReserve))
        return Fail("Invalid TH09 measured startup");
    startupSession_=session;startupNowUs_=now*1000;mode_=mode;measuring_=measured_=true;
    return true;
}
bool RollbackSession::Configure(const Netplay::SessionConfig& session,std::uint64_t now,
                               std::uint8_t inputDelay,Netplay::AdonisMode mode,unsigned predictionReserve) {
    if (session.playerCount != 2 || session.localPlayer > 1 || session.gameId != 9 || inputDelay > 9 || !Netplay::ValidAdonisMode(mode))
        return Fail("Invalid TH09 rollback session");
    auto negotiated = session;
    if (inputDelay) negotiated.gameplayAbi ^= 0x49444c00u ^ inputDelay;
    negotiated.gameplayAbi = Netplay::AdonisGameplayAbi(negotiated.gameplayAbi, mode, inputDelay);
    if(measured_)negotiated.gameplayAbi^=0x4d530100u^(predictionReserve<<20);
    mode_ = mode; inputDelay_ = inputDelay;
    Netplay::CoreConfig config;
    config.sessionId = negotiated.sessionId;
    config.localPlayer = negotiated.localPlayer;
    config.playerCount = 2;
    config.inputDelay = inputDelay;
    config.maxRollbackFrames = History;
    config.allowPrediction = mode != Netplay::AdonisMode::Delay;
    // TH09: shot/charge 1, bomb 2, focus 4, menu 8, directions 0xf0.
    // Keep held charge instead of inventing a release edge; Bomb and menu
    // actions require actual input. Every eventual release is reconciled.
    config.predictableButtons = 1 | 4 | 0xf0;
    config.directionButtons = 0xf0;
    config.maxDirectionPredictionFrames = 3;
    // TH09's network pointer is a held absolute field target, not a fresh
    // displacement. Hybrid can predict the last target (or joystick vector)
    // without repeatedly erasing it. Late moves/releases still use full undo.
    // Keep mode 0 unchanged as the experiment's frozen-policy control.
    config.directTouchIsAbsolute = mode == Netplay::AdonisMode::Hybrid;
    Netplay::SessionChannelConfig policy;
    policy.repairIntervalMs = Netplay::InputRepairBudget::StalledMs;
    policy.adonisPhase = mode != Netplay::AdonisMode::Rollback;
    policy.adonisPredictionFrames=predictionReserve;
    if (policy.adonisPhase) policy.inputResendMs = 16;
    if (!core_.Reset(config) || !gate_.Reset(negotiated) || !channel_.BeginSession(negotiated, now, policy))
        return Fail("TH09 rollback bootstrap failed");
    configured_ = true;
    return true;
}
void RollbackSession::Clear() {
    channel_.Clear(); gate_.Clear(); core_.Clear();hashes_={};
    next_ = replayEnd_ = captures_ = corrections_ = resimulated_ = 0;
    configured_ = failed_ = invalidInput_ = false; error_ = "";
    mode_ = Netplay::AdonisMode::Rollback; inputDelay_ = 0; phaseDebtMs_ = 0;
    startup_=Netplay::AdonisStartup{};startupSession_={};startupNowUs_=0;
    measuring_=measured_=false;startupPending_.clear();
}
bool RollbackSession::PumpStartup(std::uint64_t now,std::uint64_t measurementNowUs) {
    startupNowUs_=measurementNowUs?measurementNowUs:now*1000;
    if(!measuring_)return startup_.Tick(transport_,startupNowUs_)||Fail(startup_.Error());
    std::vector<std::uint8_t> bytes;
    for(unsigned count=0;count<256&&transport_.Poll(&bytes);++count){
        if(Netplay::AdonisStartup::IsPacket(bytes.data(),bytes.size())){
            if(!startup_.Receive(transport_,bytes.data(),bytes.size(),startupNowUs_))return Fail(startup_.Error());
        }else{
            // A retried COMMIT may be overtaken in an impaired control model.
            // Defer only valid native HELLO/READY traffic, bounded in size and
            // count. It cannot open the native gate until we have committed D.
            Netplay::SessionPacket packet;
            if(!Netplay::DecodeSessionPacket(bytes.data(),bytes.size(),&packet))
                return Fail("TH09 peer sent gameplay before measured startup");
            if(packet.sessionId!=startupSession_.sessionId)continue;
            if(startupPending_.size()>=16)return Fail("TH09 startup control backlog exceeded");
            startupPending_.push_back(std::move(bytes));
            if(startup_.Ready())break;
        }
    }
    if(!startup_.Tick(transport_,startupNowUs_))return Fail(startup_.Error());
    if(startup_.Ready()){
        const auto choice=startup_.Selected();
        if(!Configure(startupSession_,now,std::uint8_t(choice.delay),mode_,choice.prediction))return false;
        measuring_=false;
    }
    return true;
}

double RollbackSession::PacedElapsedMs(double elapsedMs) {
    if (!std::isfinite(elapsedMs) || elapsedMs < 0) return 0;
    phaseDebtMs_ += channel_.TakeAdonisDelayMs();
    const auto used = std::min(elapsedMs, phaseDebtMs_);
    phaseDebtMs_ -= used;
    return elapsedMs - used;
}
bool RollbackSession::Pump(std::uint64_t now, bool expectsInput,std::uint64_t measurementNowUs) {
    if(failed_)return false;
    if(measured_&&!PumpStartup(now,measurementNowUs))return false;
    if(measuring_)return true;
    if (!configured_ || failed_) return false;
    if (!channel_.Pump(gate_, core_, now, expectsInput && gate_.CanStart())) return Fail(channel_.ErrorText());
    if (invalidInput_) return Fail("Invalid TH09 peer input");
    return !failed_;
}
bool RollbackSession::Verify(std::uint32_t frame,std::uint32_t hash) {
    auto& slot=hashes_[(frame/120)%hashes_.size()];
    if(slot.frame!=frame)slot={frame};slot.local=hash;slot.hasLocal=true;
    if(slot.hasRemote&&slot.remote!=hash)return Fail("TH09 confirmed world hash mismatch");
    std::uint8_t packet[20]={'T','9','H','C'};const auto id=gate_.Config().sessionId;
    std::memcpy(packet+4,&id,8);std::memcpy(packet+12,&frame,4);std::memcpy(packet+16,&hash,4);
    return transport_.SendControl(packet,sizeof(packet))||Fail("TH09 hash send failed");
}
bool RollbackSession::Poll(std::vector<std::uint8_t>* bytes) {
    for(unsigned count=0;count<256;++count){
        if(invalidInput_||failed_)return false;
        if(!startupPending_.empty()){*bytes=std::move(startupPending_.front());startupPending_.pop_front();}
        else if(!transport_.Poll(bytes))return false;
        if(measured_&&Netplay::AdonisStartup::IsPacket(bytes->data(),bytes->size())){
            if(!startup_.Receive(transport_,bytes->data(),bytes->size(),startupNowUs_)){Fail(startup_.Error());return false;}
            continue;
        }
        if(bytes->size()==20&&!std::memcmp(bytes->data(),"T9HC",4)){
            std::uint64_t id;std::uint32_t frame,hash;
            std::memcpy(&id,bytes->data()+4,8);std::memcpy(&frame,bytes->data()+12,4);std::memcpy(&hash,bytes->data()+16,4);
            if(id!=gate_.Config().sessionId)continue;
            if(frame%120){Fail("Invalid TH09 hash frame");return false;}
            auto& slot=hashes_[(frame/120)%hashes_.size()];if(slot.frame!=frame)slot={frame};slot.remote=hash;slot.hasRemote=true;
            if(slot.hasLocal&&slot.local!=hash){Fail("TH09 confirmed world hash mismatch");return false;}continue;
        }
        Netplay::InputPacket packet;
        if(Netplay::DecodeInputPacket(bytes->data(),bytes->size(),&packet)&&packet.sessionId==gate_.Config().sessionId)
            for(unsigned i=0;i<packet.inputCount;++i)if(!ValidInput(packet.inputs[i])){invalidInput_=true;return false;}
        return true;
    }
    return false;
}
bool RollbackSession::NeedsCapture() const {
    return Ready() && next_ >= replayEnd_ && !core_.HasLocalCapture(next_);
}
bool RollbackSession::ValidInput(const Netplay::FrameInput& in) {
    if (!std::isfinite(in.x) || !std::isfinite(in.y) || in.touchBomb) return false;
    if (in.analogMode == Netplay::AnalogMode::None)
        return in.x == 0 && in.y == 0 && !in.unlimited && !in.touchUsed;
    if (in.analogMode == Netplay::AnalogMode::Joystick)
        return std::abs(in.x) <= 16 && std::abs(in.y) <= 16 && !in.unlimited;
    // The TH09 gameplay ABI interprets this as an absolute field target.
    // It is converted to velocity by GameSession on the simulated frame.
    return in.analogMode == Netplay::AnalogMode::DirectTouch &&
        std::abs(in.x) <= 4096 && std::abs(in.y) <= 4096;
}
bool RollbackSession::Capture(const Netplay::FrameInput& input, std::uint64_t now) {
    if (!NeedsCapture() || !ValidInput(input)) return Fail("Invalid or repeated TH09 input capture");
    if (!core_.ScheduleLocalInput(next_, input) || !channel_.LocalCaptured(core_, next_, now))
        return Fail("TH09 captured input send failed");
    ++captures_;
    return true;
}
Netplay::FrameDecision RollbackSession::Prepare() const {
    if (!Ready() || core_.HasRollbackRequest()) return {};
    auto decision = core_.PrepareFrame(next_);
    if (!decision.canAdvance) return decision;
    for (unsigned seat = 0; seat < 2; ++seat) {
        auto& input = decision.inputs[seat];
        if ((decision.predictedMask & (1u << seat)) && mode_ != Netplay::AdonisMode::Hybrid) {
            // Common DirectTouch prediction models displacement. TH09 ships a
            // field target: zeroing its axes would aim at the origin. Missing
            // pointer input therefore predicts no pointer override.
            input.analogMode = Netplay::AnalogMode::None;
            input.x = input.y = 0;
            input.unlimited = input.touchUsed = input.touchBomb = false;
        }
        if (!ValidInput(input)) return {};
    }
    return decision;
}
bool RollbackSession::Complete(const Netplay::FrameDecision& decision) {
    if (!Ready() || core_.HasRollbackRequest() || !decision.canAdvance) return Fail("Invalid TH09 frame completion");
    const auto expected = Prepare();
    if (!expected.canAdvance || decision.predictedMask != expected.predictedMask || decision.inputs != expected.inputs)
        return Fail("TH09 frame input changed during simulation");
    if (!core_.MarkSimulated(next_, decision)) return Fail("TH09 frame commit failed");
    if (next_ < replayEnd_) ++resimulated_;
    ++next_;
    return true;
}
bool RollbackSession::Restored(std::uint32_t frame) {
    if (mode_ == Netplay::AdonisMode::Delay || !Ready() || frame != core_.RollbackFrame() || frame >= next_)
        return Fail("Invalid TH09 restore frame");
    replayEnd_ = std::max(next_, replayEnd_);
    if (!core_.RewindSimulationTo(frame)) return Fail("TH09 input history rewind failed");
    core_.ClearRollbackRequest(); next_ = frame; ++corrections_;
    return true;
}
std::uint32_t RollbackSession::ConfirmedThrough() const {
    if (!Ready() || core_.HasRollbackRequest() || next_ < replayEnd_ || next_ == 0) return Netplay::INVALID_FRAME;
    const auto confirmed = core_.ConfirmedThroughAllRemotes();
    return confirmed == Netplay::INVALID_FRAME ? confirmed : std::min(confirmed, next_ - 1);
}
bool RollbackSession::ConfirmedInputs(std::uint32_t frame, std::array<Netplay::FrameInput, Netplay::MAX_PLAYERS>* out) const {
    const auto through = ConfirmedThrough();
    return through != Netplay::INVALID_FRAME && frame <= through && core_.ConfirmedInputs(frame, out);
}
bool RollbackSession::CanRetire() const {
    return Ready() && next_ != 0 && next_ >= replayEnd_ && !core_.HasRollbackRequest() &&
        channel_.CanRetire(core_, next_ - 1);
}
bool RollbackSession::Retire(std::uint64_t now) {
    if (!CanRetire() || !channel_.Retire(core_, next_ - 1, now)) return Fail("Unconfirmed TH09 retirement");
    configured_ = false;
    return true;
}
}
