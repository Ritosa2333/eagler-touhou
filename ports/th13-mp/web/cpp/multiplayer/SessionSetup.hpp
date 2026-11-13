// TH13 MP 会话身份（M7）。
// 线上身份串：SessionSetup Words 编解码 + GameplayVersion/GameplayAbi
//（含规则版本与构建身份）。坏身份必须在 frame0 前拒绝——
// DecodeSessionSetup 对非法字段整体拒绝（返回 false，不部分接受）。
// 共用约定（docs/MP-DEVELOPMENT.md §M7 前冻结）：
//   - 存储根 /savesth13mp、回放前缀 th13mp_XX.rpy 由宿主层持有；
//   - 输入位掩码以本作 GameInput 为准（C=0x800 且键盘同时置 0x200——
//     归一化在 InputLanes，不在身份串）；
//   - 共用网络栈不 fork：直接链接 common/（NetplayCore/传输/relay）。
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace th13::multiplayer {

struct SessionSetup {
    // 线格式版本：v1 = 纯延迟（M7 首发口径），v2 = 预测/回滚（M8 起）。
    // Legacy 兼容读：旧纯延迟会话与回放。
    static constexpr std::uint32_t Version = 2, LegacyVersion = 1, Words = 22,
                                  GameplayVersion = 1, MaxRollbackFrames = 12,
                                  RulesVersion = 3;
    // TH13 面：难度 0..5（Easy..Lunatic/Overdrive+Extra 由 stage 表达）、
    // 角色 0..3（灵梦/魔理沙/早苗/妖梦，允许重复）、2..3 席。
    static constexpr std::uint32_t MaxCharacters = 4;
    std::uint32_t version = LegacyVersion, mode = 1;
    std::uint64_t session_id = 0;
    std::uint32_t player_count = 2, local_player = 0, difficulty = 0, seed = 1;
    std::uint32_t requested_delay = 0, input_delay = 0, prediction_reserve = 2;
    std::uint32_t prediction_limit = MaxRollbackFrames, measured_prediction = 0;
    bool automatic = true, challenge = false;
    std::array<std::uint32_t, 3> selections{};
    std::array<std::uint32_t, 4> build{};
    bool valid() const;
};

// 坏身份拒绝：词数不符/版本未知/字段越界 → 整体拒绝（frame0 前）。
bool DecodeSessionSetup(SessionSetup&, const std::uint32_t*, std::size_t);
std::array<std::uint32_t, SessionSetup::Words> EncodeSessionSetup(const SessionSetup&);
// GameplayAbi：会话身份的玩法契约摘要（FNV-1a 32；含规则版本与构建身份）。
// 两侧不一致的客户端在握手层拒绝，不进入 frame0。
std::uint32_t GameplayAbi(const SessionSetup&);

}  // namespace th13::multiplayer
