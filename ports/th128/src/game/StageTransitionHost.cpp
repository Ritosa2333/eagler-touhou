#include "StageTransitionHost.hpp"
#include <utility>

namespace th128::stage {
namespace {
bool settlementCallbacksComplete(const StageSettlementCallbacks& callbacks) {
    if (!callbacks.setClearFlag || !callbacks.slot || !callbacks.finish) return false;
    return callbacks.retireTree || (callbacks.walk && callbacks.retire);
}
}

bool StageTransitionHost::fail(std::string reason, std::string& error) {
    if (failure_.empty()) failure_ = std::move(reason);
    error = failure_;
    return false;
}

bool StageTransitionHost::load(std::span<const std::uint8_t> front, std::string& error) {
    error.clear();
    if (!failure_.empty()) return fail(failure_, error);
    if (loaded_) {
        error = "stage transition requires a fresh front resource";
        return false;
    }
    if (!settlement_.load(front, error)) return false;
    loaded_ = true;
    return true;
}

bool StageTransitionHost::attachControl(Control& control, std::string& error) {
    error.clear();
    if (!failure_.empty()) return fail(failure_, error);
    if (begun_ || control_) {
        error = "stage control owner must be attached once before the transition";
        return false;
    }
    control_ = &control;
    return true;
}

bool StageTransitionHost::begin(const StageTransitionContext& context,
                                const StageTransitionCallbacks& callbacks, std::string& error) {
    error.clear();
    if (!failure_.empty()) return fail(failure_, error);
    if (!loaded_) {
        error = "stage transition front resource is not loaded";
        return false;
    }
    if (!control_) {
        error = "stage control owner is not attached";
        return false;
    }
    if (begun_) {
        error = "stage transition already started";
        return false;
    }
    if (context.currentId != context.settlement.stage
        || !StageSettlementHost::supportedStage(context.currentId)) {
        error = "stage transition current id is outside the ordinary stage rows";
        return false;
    }
    if (!settlementCallbacksComplete(callbacks.settlement)) {
        return fail("required stage-clear player-slot owners are pending", error);
    }
    if (!settlement_.begin(context.settlement, callbacks.settlement, error)) return false;
    context_ = context;
    begun_ = true;
    std::string committedError;
    if (callbacks.settlementCommitted
        && !callbacks.settlementCommitted(settlement_.state(), committedError))
        return fail(committedError.empty() ? "stage settlement scalar write-back failed" : committedError,
                    error);
    const auto request = control_->request(kStageTransitionObjectValue);
    if (request.status != RequestStatus::Written)
        return fail("stage control state12 request was rejected", error);
    if (request.pending != kStageTransitionObjectValue)
        return fail("stage control gate collapsed state12 into state2", error);
    const auto commit = control_->consume({context.session.present, context.session.field74});
    if (commit.status == CommitStatus::MissingSession)
        return fail("stage control state12 requires the real current message session", error);
    if (commit.status != CommitStatus::Applied || commit.action.kind != ActionKind::RecreateSession)
        return fail("stage control state12 action is unavailable", error);
    controlApplied_ = true;
    controlActionRecreate_ = true;
    controlDestroyExisting_ = commit.action.destroyExisting;
    controlSessionArgument_ = commit.action.sessionArgument;
    const auto plan = planRouteHandoff(
        {context.currentId, context.progress, context.gate74, context.message78});
    if (!plan) {
        return fail("stage transition route has no next stage", error);
    }
    if (plan->nextId < 1 || plan->nextId > 16) {
        return fail("stage transition produced a non-playable next id", error);
    }
    nextId_ = plan->nextId;
    nextProgress_ = plan->nextProgress;
    routePlanned_ = true;
    requested_ = true;
    settlementDone_ = true;
    return true;
}

bool StageTransitionHost::acknowledgeControl(std::string& error) {
    error.clear();
    if (!failure_.empty()) return fail(failure_, error);
    if (!controlApplied_ || !requested_) {
        error = "stage control action has not been requested";
        return false;
    }
    if (committed_) return true;
    committed_ = true;
    return true;
}

bool StageTransitionHost::messageDoneForEntry(int entry, std::string& error) {
    if (!stageTransitionCompletesMessageEntry(entry)) return true;
    return messageDone(error);
}

bool StageTransitionHost::messageDone(std::string& error) {
    error.clear();
    if (!failure_.empty()) return fail(failure_, error);
    if (!begun_) {
        error = "stage transition MSG0 arrived before the stage-clear owner";
        return false;
    }
    messageDone_ = true;
    return true;
}

bool StageTransitionHost::interrupt(int code, std::string& error) {
    error.clear();
    if (!failure_.empty()) return fail(failure_, error);
    if (!begun_) {
        error = "stage transition is not started";
        return false;
    }
    return settlement_.interrupt(code, error);
}

bool StageTransitionHost::tick(std::string& error) {
    error.clear();
    if (!failure_.empty()) return fail(failure_, error);
    if (!begun_) return true;
    return settlement_.tick(error);
}

StageTransitionState StageTransitionHost::state() const {
    StageTransitionState result;
    result.loaded = loaded_;
    result.begun = begun_;
    result.settlementDone = settlementDone_;
    result.controlApplied = controlApplied_;
    result.routePlanned = routePlanned_;
    result.requested = requested_;
    result.committed = committed_;
    result.messageDone = messageDone_;
    result.failed = !failure_.empty();
    result.currentId = context_.currentId;
    result.progress = context_.progress;
    result.nextId = nextId_;
    result.nextProgress = nextProgress_;
    if (control_) {
        result.controlFlags = control_->flags();
        result.controlGate = control_->gateSet();
        result.controlCurrent = control_->current();
        result.controlPending = control_->pending();
    }
    result.controlActionRecreate = controlActionRecreate_;
    result.controlDestroyExisting = controlDestroyExisting_;
    result.controlSessionArgument = controlSessionArgument_;
    result.error = failure_;
    return result;
}

std::vector<StageSettlementSnapshot> StageTransitionHost::snapshots() const {
    return settlement_.snapshots();
}

StageSettlementState StageTransitionHost::settlementState() const {
    return settlement_.state();
}

}
