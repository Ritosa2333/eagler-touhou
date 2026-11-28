#pragma once
#include <cstdint>
#include <functional>

namespace th128::stage {

enum class RequestStatus { Written, Rejected };
enum class CommitStatus { NoChange, Applied, Unsupported, MissingSession, ConstructionFailed };
enum class ActionKind { None, RecreateSession, PostrunMenu };

struct RequestResult {
    RequestStatus status = RequestStatus::Rejected;
    std::int32_t requested = 0;
    std::int32_t pending = 0;
    bool collapsed = false;
};

struct SessionState {
    bool present = false;
    std::int32_t field74 = 0;
};

struct Action {
    ActionKind kind = ActionKind::None;
    bool destroyExisting = false;
    std::int32_t sessionArgument = 0;
};

struct CommitResult {
    CommitStatus status = CommitStatus::NoChange;
    Action action;
    std::int32_t previous = 0;
    std::int32_t current = 0;
    std::int32_t pending = 0;
    bool sessionFlag = false;
};

class Control {
public:
    static constexpr std::uint32_t TransitionGate = 0x2000;
    static constexpr std::uint32_t ConstructorFlags = 0x4240;

    void initialize(std::uint32_t existingFlags = 0) noexcept;
    void syncFlags(std::uint32_t value) noexcept;
    bool initialized() const noexcept { return initialized_; }
    std::uint32_t flags() const noexcept { return flags_; }
    std::int32_t current() const noexcept { return current_; }
    std::int32_t pending() const noexcept { return pending_; }
    std::int32_t sessionFlag() const noexcept { return sessionFlag_; }
    bool gateSet() const noexcept { return (flags_ & TransitionGate) != 0; }
    bool sessionStartupAttempted() const noexcept { return sessionStartupAttempted_; }
    bool sessionConstructing() const noexcept { return sessionConstructing_; }
    bool sessionConstructed() const noexcept { return sessionConstructed_; }
    CommitResult constructFreshSession7(const std::function<bool(std::int32_t)>& construct);
    RequestResult request(std::int32_t state) noexcept;
    bool releaseStageGate() noexcept;
    CommitResult consume(const SessionState& session) noexcept;
    CommitResult consumeTerminal16(const std::function<bool()>& retire,const std::function<bool(unsigned)>& constructMenu);
    CommitResult constructEnding15(const std::function<bool()>& construct);
    CommitResult consumeEnding16(const std::function<bool()>& retire,const std::function<bool(unsigned)>& constructMenu);
    unsigned menuMode() const noexcept { return menuMode_; }

private:
    std::uint32_t flags_ = 0;
    std::int32_t current_ = 0;
    std::int32_t pending_ = 0;
    std::int32_t sessionFlag_ = 0;
    bool initialized_ = false;
    bool sessionStartupAttempted_ = false;
    bool sessionConstructing_ = false;
    bool sessionConstructed_ = false;
    bool sessionReentry_ = false;
    bool terminalAttempted_=false,terminalExecuting_=false,terminalReentry_=false;
    unsigned menuMode_=0;
    bool endingAttempted_=false,endingExecuting_=false,endingConstructed_=false,endingReentry_=false;
};

}
