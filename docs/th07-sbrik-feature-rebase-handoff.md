# TH07 sbrik feature-rebase handoff

> **2026-08-28 local rollback boundary:** at the user's request, the local workspace and local 8136 test build were restored to the point immediately before the first Bomb-performance report, while retaining the difficulty/Extra/Phantasm/shared-Ending update. The later journal interval-index, history-buffer reuse and two-frame-keyframe source changes are not present locally. **The public site was explicitly left untouched and still runs r713, so local source and public Runtime intentionally differ.**

> **2026-08-28 rollback performance update:** the slowdown was broader than Bomb. Every active TH07 object was captured into the sparse rollback journal, so dense enemies/bullets/items/effects also paid the old quadratic overlap scan and repeated multi-megabyte allocator growth. The interval lookup is now `std::map` plus `lower_bound`; evicted history slots retain and reuse their byte-buffer capacity; and production restores final sbrik's mature two-logical-frame keyframe cadence while keeping Web's sparse storage. Corrections between keyframes restore the preceding keyframe and replay forward. The 4096-touch native benchmark improved from `6917 us` to roughly `300-400 us`. A representative dense Web layout (`4,809,956` bytes and `1,263` blocks per captured frame) settles at roughly `374-420 us` per capture after the 16-slot history is warm, and production now captures only every second logical frame. A short real two-browser forced-WS smoke with every seventh relay packet dropped passed 300 frames: P1/P2 confirmed through frame 299 with `45/2` rollbacks and `163/6` replayed frames, maximum replay spans `6/4`, and no state/session failure. Feature-rebase contracts and the Emscripten incremental build pass. WebGL still reports synchronous `ReadPixels` stalls on the original pause/stage-transition screenshot paths; those are separate menu/transition spikes and were not changed here. **This update was deployed to `test.touhou.vip` as r713; dense real-stage 2P/3P device acceptance is now the user's test target.**

> **2026-08-28 local-only update:** multiplayer room difficulty now travels from Launcher to the TH07 Runtime. Easy through Lunatic start the normal route; Extra and Phantasm use TH07's original stage-before-increment mapping and are available in the room UI. The browser netplay session now continues through Ending with confirmed shared input, so any player can advance shared dialogue/Ending input. Final sbrik's score-history normalization is ported so local clear history cannot make Ending skip speed diverge. Gameplay ABI is now `2`, preventing old/new WASM interoperation. Local contracts, room UI persistence, and the Emscripten incremental build pass. **This update is not deployed.**

> **2026-08-26 public-network update:** the public WebRTC/TURN deployment state in this older handoff is no longer current. Before doing any signaling/TURN/VPS/resource-distribution work, read `docs/th07-public-network-deploy-handoff.md`. That newer document supersedes this file's statements that no public deployment/coturn instance had been exercised. This file remains authoritative for gameplay feature-rebase, rollback, touch/interpolation and human-acceptance boundaries.

## Read this first

This document is deliberately self-contained. Assume the receiving Agent has no conversation history, no Codex memory, no browser session and no running test process.

Workspace: `D:\workspace\eagler\th07-eagler`

Current local branch and checkout at handoff time:

- Branch: `eagler`
- Checked-out baseline: `d0f53f52f7976c2b9dc64386ec56e9a955fdc1f1`
- `origin`: `https://github.com/YomotsuHisami/th07.git`
- `upstream`: `https://github.com/some100/th07.git`
- The worktree is intentionally dirty and contains the entire uncommitted feature-rebase. Do not reset, clean, checkout over, move or delete these changes.
- No commit, push, PR, deployment or upload has been made or authorized.

Before inspecting or changing code, read these two files completely:

1. `docs/th07-sbrik-feature-rebase-ledger.md`
2. `docs/th06-th07-multiplayer-feature-rebase-notes.md`

The ledger is authoritative for current hunk dispositions, evidence and remaining human verification. Do not replace it with a shorter plan.

## User requirements that remain binding

> 项目和原rebase项目差距过大，缺失了大量玩法和他们那边更优秀的回滚思路。
>
> 在任何情况下，在项目中加入哈希校验的任何行为都应该被跳过并在以后询问我是否要加入。
>
> 现在要精细审查。
>
> 总体目标是：
>
> 以 sbrik 最终分支 `5b9ebe8… → 022c533` 为唯一玩法基准，重新做一次真正可核销的 feature-rebase：逐文件、逐上游改动恢复 TH07 的 2P/3P 玩法语义，不再凭概括手写近似实现；网络传输继续使用我们的浏览器 WebSocket/触摸架构，但预测回滚的成熟行为也按上游逐项复刻。
>
> 边界同时固定：
>
> - 复刻独立角色、Shot、SHT、资源、掉落、共享 Cherry/Border、Boss/Rank 缩放、转移/复活、菜单与关卡规则。
> - 不搬 WinSock、Windows 线程、LowLatency 和 sbrik 的原生 UDP 外壳。
> - 只允许加入「运行时分区哈希」用于定位 desync；它不得参与入房、拒绝连接、兼容性判断或自动修复。
> - 其余 EXE、资源、协议、会话兼容性哈希一律不保留。
> - 每个工作类都要有上游 hunk 对照、普通/联机构建、针对性测试和真人可验证项；未逐项核销前不再宣称复刻完成。

Additional user requirements:

- Every ported behavior must preserve or improve compatibility with Eagler's current interpolation and new touch movement.
- Existing browser WebSocket, Web input, touch and host architecture must remain.
- Document reusable architectural lessons diligently because TH06 is an explicit follow-up sister-port target after the TH07 multiplayer baseline is stable enough. Reuse TH07's browser transport/rollback/time-sync architecture as a reference, but freeze and audit TH06's own upstream gameplay range before porting gameplay semantics.
- The user later said completed paths do not need redundant retesting. Do not mechanically rerun every old test when no relevant source changed.
- Local work only. Never commit, push, deploy or upload without explicit new authorization.

## Frozen gameplay oracle and exclusions

The only gameplay oracle is:

`5b9ebe892914ff5666ef68c0cd02719dde7d4ee9..022c533e0c4205e07c94047a0523cb267ae46447`

Useful upstream labels:

- `5b9ebe8`: `document y2038 bug`
- `022c533`: `Two and three player netplay for Touhou 7`
- Relevant netplay history recorded in the ledger: `d93bc94`, `be2c35a`, `43d0b61`, `5645bee`, `22b4bcb`, `4beceb8`, `ecd4483`, `022c533`.

Never treat current local approximations as the gameplay standard. Every semantic conclusion must point to the frozen upstream file, commit and hunk/function group.

Explicit exclusions:

- WinSock and socket-specific protocol code.
- Windows threads and native timing/UI wrappers.
- `LowLatency.cpp` / `LowLatency.hpp`.
- Native UDP relay/Host shell.
- EXE, resource, protocol and session compatibility hashes.
- Hash-based joining, rejection, compatibility decisions, RNG repair, state repair or automatic resynchronization.

Hash rule: skip every new or expanded hash-related change and ask the user first. The user's runtime-partition-hash boundary is not implementation authorization. Existing local runtime diagnostic partitions were observed during tests but were not changed, expanded, used as a gate or used for repair.

## Current completion statement

The source-level feature-rebase audit is hunk/function-class complete:

- All 37 upstream changed files are represented in the ledger.
- The frozen range contains 554 ordinary zero-context hunks; every ordinary hunk has a recorded disposition.
- The upstream-only `Netplay.cpp` is one very large new-file hunk, so it is disposed by final-file function classes rather than pretending one Git hunk is sufficient evidence.
- Gameplay-bearing work classes have an upstream-to-local landing, ordinary/multiplayer/Web build evidence, focused evidence and explicit human verification instructions.
- Native shell and forbidden hash slices are explicitly excluded instead of silently omitted.

This does **not** mean every real-person acceptance item was performed. The ledger intentionally retains unverified phone touch, audio, visual, mixed-loadout, Stage 4, replay/result and long lifecycle items. Do not rewrite those as PASS without real evidence.

## Work classes already audited

The ledger contains detailed hunk tables and real-person verification instructions for:

- Items and targeted drops/transfers.
- Player lifecycle, independent resources, SHT/Shot/loadout, death/Spirit/revival and shared Border.
- Controller/input lanes, touch ownership and direct-touch rollback semantics.
- Audio side-effect commit rules.
- Enemy ECL instruction targeting.
- Assets/ANM capacities and P2/P3 offsets.
- Effects ownership, capacity and draw-only attachment smoothing.
- Replay/EAGX policy.
- GameWindow pacing and interpolation.
- Bullet/laser targeting and collision ownership.
- GameManager startup, shared Cherry/Border, Rank/Boss scaling and runtime unlocks.
- HUD/GUI compact 2P/3P presentation.
- Bomb ownership, ANM/effect scripts and damage paths.
- ECL targeting and optional Stage 4 character-card chaining.
- Enemy damage, contribution, targeting, drops and scaling.
- MainMenu, ResultScreen, Supervisor and `main.cpp` entry/session behavior.
- Diagnostics exclusions.
- Mature prediction/rollback behavior mapped from upstream `Netplay.cpp`.

Do not re-summarize these from memory. Use the exact rows in `docs/th07-sbrik-feature-rebase-ledger.md`.

## Latest real defect and fix

The last live 3P loss test exposed a genuine rollback transport defect:

- Relay settings were 20 ms delay, 15 ms jitter and every seventh forwarded message dropped.
- If frame zero, or the packet at the current prediction-window boundary, was dropped, simulation could not advance.
- The local current frame had been sent only once. Because no later frame could be scheduled, no later redundant tail carried that missing frame.
- P0 and P2 could wait forever for one another's frame zero; P1 could advance to the prediction limit and then stall.

The fix is in `src/netplay/Th07LanStageProbe.cpp`:

- `SendLocalFrame(frame)` captures physical input once and schedules it in `RollbackCore`.
- `SendScheduledLocalFrame(frame)` rebuilds only the wire packet from the already scheduled logical input.
- While the current logical frame is stalled, the driver retries that packet every three driver ticks.
- The retry path never calls `CaptureLocalInput`, so keyboard, controller and one-frame direct-touch displacement are not sampled again.
- The application-level redundant tail remains 32 frames, matching final sbrik behavior.

The focused contract is in `audit/feature-rebase/netplay-rollback-feature-rebase-test.py` and explicitly rejects a retry helper that calls `CaptureLocalInput`.

TH06 reuse lesson was added to `docs/th06-th07-multiplayer-feature-rebase-notes.md`: redundancy alone is insufficient at frame zero/prediction-window stalls; resend scheduled logical input without resampling the device.

## Browser RTC/TURN transport baseline

The production browser transport is no longer WebSocket-only. `src/netplay/BrowserPeerTransport.*` now provides the reusable TH07/TH06 browser data-plane boundary:

- normal route: WebRTC `RTCPeerConnection` full mesh;
- ICE may select direct host/reflexive connectivity or TURN without changing gameplay transport semantics;
- reliable/ordered `th07-control` DataChannel carries session/control packets;
- unordered/non-retransmitting `th07-input` DataChannel carries input/ACK traffic and relies on the existing 32-frame redundancy + rollback recovery;
- existing WebSocket gameplay relay is preconnected only as an emergency whole-room fallback;
- a server-side route barrier releases `rtc` only after every expected peer edge has both channels open, or releases `relay` only after RTC failure plus every signaling client and relay socket are present;
- no mid-run hot migration and no 3P per-edge mixed topology are implemented.

The combined `eagler-touhou/server/netplay-relay.mjs` is the current shared development/Host lobby, signaling and emergency-relay server for TH06/TH07. It can inject ICE configuration and can generate coturn TURN REST-style temporary credentials from `TH07_TURN_URLS` + `TH07_TURN_SHARED_SECRET` (+ optional `TH07_TURN_TTL_SECONDS`). The TURN shared secret never enters launcher source or runtime options. Default STUN remains `stun:stun.cloudflare.com:3478` when the server does not override it. TH07 integration tests consume this Host-owned implementation rather than carrying a private copy.

TURN security policy should follow the common deployment baseline rather than invent a project-specific hard gate: keep the shared signing secret server-only, issue short-lived credentials, and use TURN-side allocation/bandwidth quotas plus monitoring. Binding credential issuance to an active room/session is optional defense in depth, not a required precondition for public TURN deployment.

Deployment plumbing now lives in `eagler-touhou/server/render-coturn-config.cjs`, `eagler-touhou/server/coturn.env.example` and `eagler-touhou/docs/SELF_HOSTING_REFERENCE.md`. The renderer emits a minimal coturn config with REST-secret auth, `no-cli`, `no-multicast-peers`, common private/reserved IPv4 peer blocks, allocation quotas and bandwidth caps. Current defaults target the ~10 Mbps VPS conservatively and remain environment-overridable. No actual public coturn instance has been exercised yet, so a selected ICE `relay` candidate is still unverified.

One startup race was found while validating this deployment layer: server route `rtc` is broadcast to both endpoints, but the browser tasks that receive it are not simultaneous. The first endpoint could send HELLO immediately while the second endpoint's DataChannel was already open but its local route was still unset; the old `onmessage` filter discarded that HELLO and the session could remain at frame zero. `BrowserPeerTransport` now buffers RTC packets while route is undecided, symmetric with the existing relay pre-route buffer. A deterministic test delays one endpoint's route message by 300 ms and now passes; the production launcher run subsequently reached frames 133/136 on `rtc/direct`.

Latest evidence for this transport layer:

- focused 2P BrowserPeerTransport: PASS, route `rtc`, both input/control channels bidirectionally exchanged binary messages; selected candidate path in the same-host Chromium test was host↔host UDP;
- forced `RTCPeerConnection` unavailable: PASS, route barrier selected WebSocket `relay` and binary input/control messages crossed without the earlier frame-zero route-propagation loss;
- focused 3P full mesh: PASS, all three clients selected `rtc` and every endpoint exchanged both message classes;
- full two-independent-browser-context launcher room launch: PASS, both TH07 runtimes reached 120+ logical frames (`124/128` in the recorded run), both transports `rtc`, both selected local-test paths `direct`;
- `tests/turn-signaling-contract-test.cjs`: PASS, temporary coturn username TTL and HMAC-SHA1 credential matched independently calculated values;
- final Emscripten TH07MP rebuild with BrowserPeerTransport/control-channel changes: PASS;
- shell protocol and mature rollback focused contract: PASS after the transport changes.

Evidence limits: no real TURN server/candidate has been exercised yet, no public-NAT/CGNAT connectivity rate is claimed, and phone↔PC RTC/TURN behavior remains a real-device acceptance item. Keep the WebSocket gameplay relay until TURN and public beta evidence justify removing it.

## Mature rollback behavior currently mapped

The current portable rollback path includes:

- Stable P1/P2/P3 input rings and independent remote histories.
- Duplicate, conflict, late-input and earliest-mismatch handling.
- Twelve recoverable predicted frames before waiting.
- Predictable held controls limited to direction, Focus, Shoot and Skip.
- Bomb, Menu and touch-Bomb predict released so edges never repeat.
- Direct-touch displacement may repeat only for the first missing frame; second and later missing frames zero the displacement.
- Thirty-two application-level redundant inputs per packet.
- Frame-zero and shared-UI exact-input barriers.
- Sparse first-write rollback journal instead of native full-copy snapshots.
- Rollback capture of deterministic managers, players, RNG, input lanes, interpolation endpoints and Stage 4 coordinator state.
- 1024 live BombEffects per 3P rollback snapshot, matching final upstream.
- Resimulation-only side-effect suppression; normal predicted-forward audio/feedback still commits.
- Stage-change history invalidation and non-freezing abandonment of an impossible stale correction, while true journal corruption remains fatal.
- Bounded GameWindow catch-up so a network stall does not create permanent slow time or an unbounded burst.
- Unexpected browser transport close and 15-second non-advancing required lane become visible room failures; the legacy WebSocket path still surfaces clean close explicitly.
- Browser whole-room termination on a missing required peer. Native 3P healthy-pair continuation is not claimed because the excluded Host-authoritative UDP lifecycle has no browser equivalent.

## Evidence already obtained

Do not rerun these solely to reproduce old output when relevant code has not changed. Exact commands/results are recorded in the ledger verification log.

Latest relevant evidence:

- Ordinary desktop Release build: PASS.
- Multiplayer-gameplay desktop Release build: PASS.
- Production shared-font Emscripten Web build: PASS.
- Acceptance-only embedded-font Emscripten Web build: PASS. This separate cache exists only because a standalone direct server does not run the production package-font installer.
- `audit/feature-rebase/netplay-rollback-feature-rebase-test.py`: PASS, 16/16.
- Fresh C++ `netplay-core-test`: PASS.
- Fresh C++ sparse rollback-journal test: PASS.
- Fresh C++ multiplayer input-lane test: PASS.
- Live 2P WebSocket run with jitter/loss and pause/resume: PASS.
- Live 3P 1200-frame WebSocket run at 20 ms delay, 15 ms jitter, every seventh forwarded message dropped: all peers confirmed frame 1199; rollback counts 70/62/60; maximum rollback spans 6/8/7; final existing diagnostic partitions matched.
- Live 2P 600-frame physical browser-keyboard plus jitter/loss run: PASS; physical input observed and both endpoints reached the same final existing diagnostic state.
- Restarting the relay visibly failed connected endpoints instead of leaving a frozen canvas.

Important evidence limits:

- Scripted browser input is not physical phone touch evidence.
- Physical browser keyboard is not touch evidence.
- Existing diagnostic partition values are observations only; they were not added or expanded in this work.
- A 2P Stage 1-to-Stage 2 long test was started, then deliberately stopped when the user said no more testing. It is not PASS evidence.
- A local production-host room/touch configuration inspection was started, then stopped on the same instruction. It is not an end-to-end host/touch PASS.

## Interpolation and touch invariants

These are mandatory Eagler adaptations and must survive future edits:

- Sample one endpoint's physical keyboard/controller/touch only for its local stable slot.
- Store buttons, analog mode, coordinates, unlimited flag, touch-used and touch-Bomb in the synchronized per-player `FrameInput`.
- Rollback and packet retry must replay captured logical input and must never resample SDL/DOM/touch.
- Treat direct-touch movement as a one-frame displacement, not a held axis.
- Simulation, collision, targeting, ownership, RNG and snapshots use authoritative logical coordinates only.
- Publish logical previous/current endpoints on fixed simulation ticks.
- Apply ordinary interpolation and bounded remote correction smoothing only during drawing.
- Player, Options, hitbox, prompts, names and attached effects consume the same draw-only presentation offset.
- Never write smoothed positions back into player/item/effect state or rollback snapshots.
- Temporary HUD scale/color changes must update and restore matching previous-state fields to avoid high-refresh pulsing.
- Items and Bomb regions/trails retain their own logical previous/current publication and draw-only interpolation.

## Known remaining human verification

The implementation is hunk-complete, but the following must remain visibly unverified until someone performs them:

- Two real endpoints with phone direct touch and keyboard/controller, including ownership, Focus, Bomb and deathbomb under rollback.
- Both touch movement modes, especially two or more consecutive missing frames with no repeated direct-touch displacement.
- Remote player/Options/hitbox/prompts/attached-effect smoothing under real touch corrections.
- Mixed character/Shot/SHT/ANM and all-loadout Bomb visual matrix in 2P/3P.
- Unequal Power firing, death, Spirit/revival, life transfer, eight-tap Power transfer and shared Border activation/break.
- Real Items ownership/drop/transfer/auto-collect behavior.
- Real Bullet/laser and Enemy collision/targeting/damage/drop behavior.
- Youmu redirected-bullet pattern and Stage 4 option OFF/ON character-card chains.
- HUD, pause/retry, lifecycle, BGM/SFX and browser gesture audio behavior.
- Ordinary/EAGX replay round-trip, multiplayer replay suppression and ResultScreen persistence boundaries.
- Full production host-to-iframe room descriptor, touch overlay, background/foreground and abrupt disconnect behavior.

Follow the per-work-class verification lists in the ledger instead of inventing a new checklist.

## Worktree and safety notes

At handoff time `git status --short` shows many modified and untracked paths, including gameplay sources, `src/netplay/`, `src/multiplayer/`, tests, tools and all documents. This is expected. `vendored/SDL` is also dirty as a nested repository/submodule and must be treated as user-owned unrelated state unless a future task proves otherwise.

Never run:

- `git reset --hard`
- `git clean`
- broad `git checkout -- ...`
- recursive deletion or movement of the workspace

Do not assume every dirty file was created by the feature-rebase. Inspect overlaps carefully.

The last audit services were cleaned up. No TH07 audit listener remained on ports `8130`, `8139`, `8140` or `18142` at handoff time. Browser test tabs were not preserved.

## Build and focused-test commands for future edits

Only rerun the affected evidence after a relevant source change; the user explicitly allowed avoiding redundant tests for already completed paths.

Ordinary desktop:

```powershell
& 'C:\Program Files\JetBrains\CLion 2025.2.3\bin\cmake\win\x64\bin\cmake.exe' --build build-desktop-th07-default-audit --config Release --parallel
```

Multiplayer desktop:

```powershell
& 'C:\Program Files\JetBrains\CLion 2025.2.3\bin\cmake\win\x64\bin\cmake.exe' --build build-desktop-th07-multiplayer-audit --config Release --parallel
```

Production Web:

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' --build build-web-th07-netplay --parallel
```

Standalone acceptance Web with embedded fonts:

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' --build build-web-th07-netplay-standalone-audit --parallel
```

Latest rollback contract:

```powershell
python audit/feature-rebase/netplay-rollback-feature-rebase-test.py
```

Before handing off any future change:

```powershell
git diff --check
git status --short
```

## How to resume safely

1. Read this handoff, the complete ledger and the TH06/TH07 reuse notes.
2. Inspect `git status --short`; preserve all existing changes.
3. Identify the exact unresolved ledger row or a newly reported defect.
4. Reopen the frozen upstream file/commit/hunk before changing gameplay.
5. Check the change against the touch/interpolation invariants above.
6. Skip and document every hash-related slice; ask the user before any new/expanded hash behavior.
7. Make the narrow local edit.
8. Update the same ledger and TH06 reuse notes when a reusable lesson is learned.
9. Run only evidence affected by the edit unless the user asks for a wider test pass.
10. Report remaining human verification honestly. Do not turn hunk completeness into a claim that phone/audio/visual acceptance was performed.

## Final status at this handoff

The TH07 source-level sbrik feature-rebase is documented as hunk/function-class complete, including the mature browser rollback corrections and the latest stalled-current-input retransmission fix. It is locally built and has focused plus live 2P/3P loss evidence already recorded. Full real-person phone touch/audio/visual/gameplay acceptance remains intentionally unclaimed. No forbidden native shell or new/expanded hash behavior was introduced, and nothing was committed, pushed or deployed.
