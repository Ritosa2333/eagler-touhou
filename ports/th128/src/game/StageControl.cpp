#include "StageControl.hpp"

namespace th128::stage {

void Control::initialize(std::uint32_t existingFlags) noexcept {
    if(endingExecuting_){endingReentry_=true;return;}
    if(terminalExecuting_){terminalReentry_=true;return;}
    if(terminalAttempted_)return;
    if (sessionConstructing_) { sessionReentry_ = true; return; }
    if (sessionStartupAttempted_) return;
    flags_ = existingFlags | ConstructorFlags;
    current_ = 0;
    pending_ = 0;
    sessionFlag_ = 0;
    initialized_ = true;
}

void Control::syncFlags(std::uint32_t value) noexcept {
    if(endingExecuting_){endingReentry_=true;return;}
    if(terminalExecuting_){terminalReentry_=true;return;}
    if(terminalAttempted_)return;
    if (sessionConstructing_) { sessionReentry_ = true; return; }
    if (sessionStartupAttempted_ && !sessionConstructed_) return;
    if (initialized_)
        flags_ = value;
}

RequestResult Control::request(std::int32_t state) noexcept {
    RequestResult result;
    result.requested = state;
    if(endingExecuting_){endingReentry_=true;return result;}
    if(terminalExecuting_){terminalReentry_=true;return result;}
    if(terminalAttempted_)return result;
    if (sessionConstructing_) { sessionReentry_ = true; return result; }
    if (sessionStartupAttempted_ && !sessionConstructed_) return result;
    if (!initialized_ || state < 0 || state > 17)
        return result;
    result.status = RequestStatus::Written;
    result.pending = gateSet() ? 2 : state;
    pending_ = result.pending;
    result.collapsed = result.pending != state;
    return result;
}

bool Control::releaseStageGate() noexcept {
    if(endingExecuting_){endingReentry_=true;return false;}
    if(terminalExecuting_){terminalReentry_=true;return false;}
    if(terminalAttempted_)return false;
    if (sessionConstructing_) { sessionReentry_ = true; return false; }
    if (sessionStartupAttempted_ && !sessionConstructed_) return false;
    if (!initialized_)
        return false;
    const bool changed = gateSet();
    flags_ &= ~TransitionGate;
    return changed;
}

CommitResult Control::constructFreshSession7(
        const std::function<bool(std::int32_t)>& construct) {
    CommitResult result;
    result.previous = current_;
    result.current = current_;
    result.pending = pending_;
    result.sessionFlag = sessionFlag_ != 0;
    result.status = CommitStatus::Unsupported;
    if (sessionConstructing_) { sessionReentry_ = true; return result; }
    if (!initialized_ || current_ != 0 || pending_ != 0 || sessionFlag_ != 0
        || gateSet() || sessionStartupAttempted_ || !construct)
        return result;
    const auto requested = request(7);
    if (requested.status != RequestStatus::Written || requested.pending != 7)
        return result;
    sessionStartupAttempted_ = true;
    sessionFlag_ = 1;
    sessionConstructing_ = true;
    result.pending = pending_;
    result.sessionFlag = true;
    result.action.kind = ActionKind::RecreateSession;
    bool constructed = false;
    try {
        constructed = construct(0);
    } catch (...) {
        sessionConstructing_ = false;
        throw;
    }
    sessionConstructing_ = false;
    if (!constructed || sessionReentry_ || current_ != 0 || pending_ != 7 || gateSet()) {
        result.status = CommitStatus::ConstructionFailed;
        return result;
    }
    sessionConstructed_ = true;
    current_ = pending_;
    result.current = current_;
    result.status = CommitStatus::Applied;
    return result;
}

CommitResult Control::consumeTerminal16(const std::function<bool()>& retire,
        const std::function<bool(unsigned)>& constructMenu) {
    CommitResult result;result.previous=current_;result.current=current_;result.pending=pending_;
    result.sessionFlag=sessionFlag_!=0;result.status=CommitStatus::Unsupported;
    if(terminalExecuting_){terminalReentry_=true;return result;}
    if(!initialized_||current_!=7||pending_!=16||gateSet()||terminalAttempted_||!retire||!constructMenu)return result;
    terminalAttempted_=true;terminalExecuting_=true;
    try{
        if(!retire()||terminalReentry_||current_!=7||pending_!=16){
            terminalExecuting_=false;result.status=CommitStatus::ConstructionFailed;return result;
        }
        pending_=4;menuMode_=3;
        if(!constructMenu(menuMode_)||terminalReentry_||current_!=7||pending_!=4){
            terminalExecuting_=false;result.pending=pending_;result.status=CommitStatus::ConstructionFailed;return result;
        }
        current_=pending_;terminalExecuting_=false;
        result.current=current_;result.pending=pending_;result.action.kind=ActionKind::PostrunMenu;
        result.action.destroyExisting=true;result.status=CommitStatus::Applied;return result;
    }catch(...){terminalExecuting_=false;throw;}
}

CommitResult Control::constructEnding15(const std::function<bool()>& construct) {
    CommitResult result;result.previous=current_;result.current=current_;result.pending=pending_;
    result.sessionFlag=sessionFlag_!=0;result.status=CommitStatus::Unsupported;
    if(endingExecuting_){endingReentry_=true;return result;}
    if(!initialized_||current_!=7||pending_!=15||gateSet()||!sessionConstructed_
        ||endingAttempted_||terminalAttempted_||!construct)return result;
    endingAttempted_=true;endingExecuting_=true;
    bool constructed=false;
    try{constructed=construct();}catch(...){endingExecuting_=false;throw;}
    endingExecuting_=false;
    if(!constructed||endingReentry_||current_!=7||pending_!=15||gateSet()){
        result.status=CommitStatus::ConstructionFailed;return result;
    }
    endingConstructed_=true;current_=pending_;
    result.current=current_;result.status=CommitStatus::Applied;return result;
}

CommitResult Control::consumeEnding16(const std::function<bool()>& retire,
        const std::function<bool(unsigned)>& constructMenu) {
    CommitResult result;result.previous=current_;result.current=current_;result.pending=pending_;
    result.sessionFlag=sessionFlag_!=0;result.status=CommitStatus::Unsupported;
    if(terminalExecuting_){terminalReentry_=true;return result;}
    if(!initialized_||!endingConstructed_||endingExecuting_||current_!=15||pending_!=16
        ||gateSet()||terminalAttempted_||!retire||!constructMenu)return result;
    terminalAttempted_=true;terminalExecuting_=true;
    try{
        if(!retire()||terminalReentry_||current_!=15||pending_!=16){
            terminalExecuting_=false;result.status=CommitStatus::ConstructionFailed;return result;
        }
        pending_=4;menuMode_=3;
        if(!constructMenu(menuMode_)||terminalReentry_||current_!=15||pending_!=4){
            terminalExecuting_=false;result.pending=pending_;
            result.status=CommitStatus::ConstructionFailed;return result;
        }
        current_=pending_;terminalExecuting_=false;
        result.current=current_;result.pending=pending_;result.action.kind=ActionKind::PostrunMenu;
        result.action.destroyExisting=true;result.status=CommitStatus::Applied;return result;
    }catch(...){terminalExecuting_=false;throw;}
}
CommitResult Control::consume(const SessionState& session) noexcept {
    CommitResult result;
    if(endingExecuting_){endingReentry_=true;result.status=CommitStatus::Unsupported;return result;}
    result.previous = current_;
    result.current = current_;
    result.pending = pending_;
    result.sessionFlag = sessionFlag_ != 0;
    if (sessionConstructing_) sessionReentry_ = true;
    if(terminalExecuting_){terminalReentry_=true;result.status=CommitStatus::Unsupported;return result;}
    if(terminalAttempted_&&current_!=4){result.status=CommitStatus::Unsupported;return result;}
    if (!initialized_ || sessionConstructing_ || (sessionStartupAttempted_ && !sessionConstructed_)) {
        result.status = CommitStatus::Unsupported;
        return result;
    }
    if (pending_ == current_) {
        result.status = CommitStatus::NoChange;
        return result;
    }
    switch (pending_) {
    case 2:
        current_ = 2;
        result.status = CommitStatus::Applied;
        break;
    case 7:
        if (!session.present) {
            result.status = CommitStatus::MissingSession;
            return result;
        }
        if (current_ == 4) {
            result.status = CommitStatus::Unsupported;
            return result;
        }
        current_ = 7;
        sessionFlag_ = 1;
        result.status = CommitStatus::Applied;
        result.action.kind = ActionKind::RecreateSession;
        result.action.sessionArgument = 0;
        break;
    case 12:
        if (!session.present) {
            result.status = CommitStatus::MissingSession;
            return result;
        }
        result.action.kind = ActionKind::RecreateSession;
        result.action.destroyExisting = current_ == 7;
        result.action.sessionArgument = session.field74;
        current_ = 7;
        pending_ = 7;
        sessionFlag_ = 0;
        result.status = CommitStatus::Applied;
        break;
    default:
        result.status = CommitStatus::Unsupported;
        return result;
    }
    result.current = current_;
    result.pending = pending_;
    result.sessionFlag = sessionFlag_ != 0;
    return result;
}

}
