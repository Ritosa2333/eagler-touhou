# TH06/TH07 multiplayer feature-rebase notes

## Purpose

This document records reusable Eagler porting constraints discovered while rebuilding TH07 against sbrik's final gameplay branch. It is supporting material for a likely TH06 sister port, not a gameplay oracle.

- TH07 gameplay remains defined only by its frozen sbrik range and the TH07 ledger.
- TH06 must freeze and audit its own final upstream range, files, commits and hunks. Never copy TH07 constants, state transitions, resource thresholds or player-count rules merely because the architecture looks similar.
- Every reusable pattern below still needs a TH06 upstream hunk or an explicitly documented Eagler adaptation reason before landing there.

## Reusable architectural boundaries

| Concern | Reusable rule | Why it matters to TH06 |
| --- | --- | --- |
| Transport | Keep browser WebSocket/session ownership; do not port WinSock, Windows threads, LowLatency or native UDP wrappers | Gameplay can be rebased independently of the host/network shell |
| Gameplay oracle | Compare the final upstream range hunk by hunk; existing Eagler approximations are audit subjects, never the standard | Prevents a second hand-written near-copy |
| Normal build | Guard multiplayer states, arrays and helpers so the ordinary build keeps its original control flow and data surface | Makes rollback to the Eagler baseline cheap and testable |
| Slot identity | Treat the stable player-array index as chain/input ownership; repair restored `initParam` before reading a lane | Rollback snapshots must not route one player through another endpoint's input |
| Input lanes | Gameplay reads synchronized buttons/analog/direct-touch from the player's logical lane | Keyboard and touch stay deterministic during resimulation |
| Raw touch | Only the local slot may sample global host touch, joystick, touch-used or touch-bomb state | Prevents one phone from controlling multiple players in gameplay-only or partially configured builds |
| Direct-touch prediction | A displacement is a one-frame delta: repeat it for at most the first missing frame, then zero it while retaining held flags/buttons | Prevents remote touch racing and snapback |
| Simulation/presentation split | Targeting, collision, ownership, RNG and snapshots use logical positions only | Draw smoothing must never change gameplay or desync recovery |
| Player interpolation | Publish `prevPosition` before each logical update; draw from `prev.Lerp(current, renderAlpha)` | Keeps fixed-tick simulation smooth at variable render cadence |
| Remote smoothing | Add a bounded draw-only offset after ordinary interpolation; snap on spawn/death or large teleports | Hides rollback corrections without creating a second simulation position |
| Attached presentation | Ship, Options, hitbox, prompts, names and attached effects must consume the same current-frame presentation offset | Avoids a smooth ship with jittering attachments |
| Temporary HUD transforms | When drawing a compact/copy VM at a temporary scale or color, set the corresponding previous field and restore both afterward | Prevents icons, labels and background tiles from pulsing under render interpolation |
| Projectile lifetime | If upstream keeps projectiles alive after owner death/elimination, update, damage and draw ordering must all agree | Prevents invisible damaging bullets |
| Side effects | Suppress audio/visual commands only during rollback resimulation, not normal synchronized forward frames | Avoids missing BGM/Bomb/feedback on healthy network play |
| Resources | Keep P1's original storage where required; add P2/P3 sidecars without expanding original save/replay layouts | Reduces compatibility risk while supporting independent resources |
| Resource initialization | Load the slot's actual SHT/Shot first, then initialize its Bomb/Power sidecar | Avoids copying P1's starting Bomb count into another loadout |
| Bomb ownership | Resolve every portrait, VM script, fixed effect slot, timer and damage/clear box from the actual `Player*`; keep ordinary wrappers identity-preserving | Independent Bomb callbacks are not enough if their visual/effect indices still point at P1 |
| Shared state | Give each shared gauge/transition one deterministic owner; other players may animate but must not overwrite the shared value | Prevents player-count-dependent double/triple writes |
| Startup options | Normalize every simulation-affecting local option before player/resource registration, then preserve the upstream order of difficulty/practice overrides | Local life/slowdown preferences must not make peers start from different worlds |
| Content unlocks | If upstream forces room content, make the decision runtime-only and leave each endpoint's save/unlock records untouched | Avoids turning a session convenience into a persistent save mutation or compatibility gate |
| Local HUD options | Keep optional names/contribution/network diagnostics presentation-only; never negotiate them or use them as compatibility state | Each endpoint may choose what to draw without changing simulation |
| Host gameplay options | Model an upstream host-authored gameplay toggle explicitly, preserve its upstream default, and distribute one room-descriptor value before frame zero; do not replace it with `IsMultiplayer()` | Optional script changes must not silently become mandatory in every browser room |
| Tie breaking | Use fixed lower-slot tie breaks and active-slot order; do not consume RNG for ownership/distribution | Every peer and rollback replay reaches the same result |
| Hash boundary | Skip every hash-related hunk by default and document it for user review; never introduce join, compatibility, refusal or repair gates | TH06 inherits the same explicit authorization boundary as TH07 |

## Per-work-class documentation template

For TH06 and the remaining TH07 classes, record all of the following in the game's ledger:

1. Frozen upstream range, commit and zero-context hunk identifiers.
2. For each hunk: `ported`, `already equivalent`, `Eagler adaptation`, or `excluded with reason`.
3. Local file/function landing and any normal-build guard.
4. Touch/interpolation impact: logical input/position source and draw-only adaptation.
5. Hash/native-shell disposition, including deliberately skipped slices.
6. Ordinary desktop, multiplayer-gameplay desktop and WebSocket/Web build results.
7. Focused source/model/runtime test and what it does not prove.
8. Exact real-person 2P/3P desktop/touch/rollback verification steps.
9. Remaining differences and risk; never convert a build or short probe into a completion claim.

## TH07 findings likely to recur in TH06

- Fixed-target items must have an explicit policy when the receiver becomes absent; a temporary fallback can snap back when the slot returns.
- A shared Border-like state needs a single gauge writer and re-entrancy protection around activation/break propagation.
- Player draw code may need to draw persistent projectiles before hiding an eliminated owner.
- Remote prompts and labels need interpolated positions and the same presentation offset as the ship, not raw logical coordinates.
- Gameplay-only multiplayer builds are a useful regression target: they expose global touch leakage that a fully active network-input override can accidentally hide.
- Registration order is gameplay-relevant when starting resources depend on the selected Shot/SHT.
- Bullet-load slowdown is gameplay state when it skips calculation ticks; multiplayer must disable it even if controller/display preferences remain endpoint-local.
- Player-count threshold changes need an explicit contraction rule so lowering a shared threshold cannot trigger the shared state by itself.
- Pause/retry menus can leave a playfield viewport active; flush their queued sprites before restoring the full portable framebuffer for later HUD/effects.
- A half-width copy of a HUD background tile may need its own synchronized previous scale; changing only the copied VM's current scale still interpolates from the source scale.
- One global Bomb portrait is not compatible with independent simultaneous owners if the upstream final branch removes it; do not preserve the old draw merely because the animation still exists.
- Bomb trails and moving regions need the same logical previous/current publication rule as players/items; interpolate only in their draw callback, never in damage boxes.
- Full-file equivalence is practical for new upstream-only helper files: compare against the final upstream blob and allow only named host API adaptations.
- ECL/script variables that refer to the player must share one closest-active logical-target helper across integer, float, pointer, movement, laser and side-decision paths; never target a presentation-smoothed coordinate.
- A character-dependent Boss-card chain is simulation state: preserve the exact character/difficulty table, distinct-character queue order, phase abort guards, lifecycle reset and rollback restore. Its room toggle remains separate from local presentation preferences.
- Enemy damage must be accumulated per active logical slot before applying Boss/spell/invulnerability transforms; preserve deterministic owner tie-breaking, proportional contribution remainder and per-slot targeting state. Enemy drops should enter the same ownership-aware item path as ordinary item transfers.
- Browser menu ports should separate native connection/quick-start shell behavior from portable launch semantics: keep stage mapping, audio cleanup, runtime-only unlocks and cursor rules, while room descriptors provide loadouts before gameplay starts.
- Result screens need two independent policies: parse local score data for display, but suppress multiplayer writes to single-player persistence and skip replay prompts that cannot represent synchronized guest lanes; leave ordinary replay/EAGX behavior intact.
- Keep Supervisor as a portable lifecycle owner: synchronized game lanes belong to the rollback/input adapter and raw touch/controller sampling belongs only to the local endpoint. Install the room RNG seed before gameplay registration, then verify menu, stage, audio and result transitions as one chain.
- Treat the native entry point as a shell work class: map seed, stable slot, player count, loadouts and host-authored gameplay options into the portable session before stage registration, while fullscreen, instance checks, modal errors, test timeouts and endpoint config stay outside synchronized state.
- Do not translate an upstream test-oriented `NoSave` switch into a blanket multiplayer config ban. Separate gameplay persistence such as score/replay from harmless endpoint-local display/audio preferences, and document each owner explicitly.
- Prediction must classify inputs by semantics: repeat held direction/Shot/Focus/Skip, predict Bomb/Menu and touch-Bomb released, and consume a direct-touch displacement on at most the first missing frame.
- Preserve the upstream loss window even on reliable WebSocket: a 32-frame application-level redundant input tail covers relay loss/reordering tests and the final-frame ACK hole without importing UDP retransmission code.
- Redundancy alone is insufficient when frame zero or the current prediction-window boundary packet is dropped: while simulation is stalled, periodically rebuild the wire packet from the already scheduled logical input. Never call the keyboard/controller/touch sampler again, because repeating a direct-touch displacement would change gameplay during the retry.
- When the final upstream raises a rollback-owned heap-effect ceiling from real 3P measurements, carry the measured capacity (1024 BombEffects here) into the portable snapshot/journal rather than retaining a two-player-sized guess.
- A stage change invalidates rollback history. Distinguish an expected unavailable old-stage snapshot from a corrupted journal: abandon and report the impossible correction so the room does not freeze, but keep capture corruption or a failed resimulation fatal.
- Do not repair post-stall latency by permanently carrying a large prediction lead, hard-gating normal simulation ticks, or forcing held directions neutral after an arbitrary short timeout. The accepted TH07 browser baseline uses standard last-known held-input prediction, keeps rollback capacity as an exceptional safety ceiling, and handles long-lived clock skew separately with GIUROLL/GGPO-style time synchronization.
- Time synchronization is a scheduler/presentation-to-simulation concern, not gameplay state. Exchange an explicit sender simulation-frame signal rather than overloading resend-window fields, estimate frame advantage over a rolling sample window, reject/outweigh single spikes, and apply only small continuous wall-clock pacing corrections. TH07's accepted browser baseline uses a bounded pacing scale around 1.0 rather than periodic hard stalls; simulation remains fixed-step and deterministic.
- Keep resend/ACK recovery and time synchronization orthogonal. The input packet's actual carried-input tail may point at old unacknowledged frames, so it is not a reliable clock signal; carry sender-frame/frame-advantage telemetry separately. This matters for TH06 because the same loss recovery can otherwise poison skew estimation after a stall.
- Expose read-only netplay diagnostics for real-device acceptance (at minimum logical frame, confirmed frame, rollback/resimulation counters, estimated frame advantage and pacing scale). Use these to distinguish true network backlog from presentation smoothing before changing prediction behavior.

## Known low-priority gameplay issue: post-death live regain can block game-over

Observed in current TH07 multiplayer play: if a player dies/eliminates, later receives or regains a Live, and eventually all players die, the run can fail to reach the normal game-over/end state. This is deliberately **not fixed yet** because it crosses multiplayer gameplay semantics rather than transport/presentation plumbing.

Treat the likely fault surface as a lifecycle consistency problem spanning at least: eliminated/dead state, whether a regained Live makes a slot revivable/active again, Spirit/revival eligibility, per-player remaining-life accounting, and the final "no player can continue" game-over predicate. Do not patch this by merely changing one counter or forcing game-over when all visible ships are dead. Re-open the frozen TH07 upstream death/Spirit/revival/game-over hunks first and determine the intended lifecycle. When TH06 receives its sister port, explicitly regression-test this sequence there as well instead of assuming the same fix or constants.

Priority: low. Leave current accepted TH07 netplay timing/prediction behavior untouched unless this lifecycle bug is being investigated directly.

## Public multiplayer transport baseline (implemented)

TH07 now has a browser transport layer that keeps `NetplayCore`, `NetplaySession`, rollback/input ownership and deterministic session contracts transport-agnostic while moving the normal gameplay data path to WebRTC.

- `BrowserPeerTransport` owns browser networking. It establishes a WebRTC full mesh for 2P/3P and keeps the existing WebSocket gameplay relay only as an emergency fallback.
- The signaling/lobby service on the existing relay process exchanges SDP/ICE and applies a whole-room route barrier before frame zero. The runtime sees route `rtc` when DataChannels are ready; whether ICE selected a true direct path or TURN is deliberately hidden from gameplay code.
- RTC uses two channels per peer edge: `th07-control` is reliable/ordered; `th07-input` is `ordered:false, maxRetransmits:0`. Session/control traffic therefore keeps reliable semantics while 60 Hz input/ACK traffic can discard obsolete packets instead of recreating TCP head-of-line blocking.
- The existing 32-frame application redundancy, ACK recovery, last-known prediction and rollback remain the gameplay reliability layer for the input channel. Do not add SCTP retransmission merely to make the transport look reliable.
- 3P v1 remains whole-room full mesh: all three peer edges and both channels per edge must be ready before route `rtc` is released. If RTC setup fails/times out, every expected signaling client and WebSocket relay socket must be present before route `relay` is released. Per-edge hybrid routing is intentionally not implemented.
- Route selection is fixed before frame zero. Mid-game migration between RTC and WebSocket remains intentionally unsupported because it would need an explicit packet-order/duplicate/frame-advantage migration barrier.
- ICE configuration is server-extensible. STUN defaults to `stun:stun.cloudflare.com:3478`; signaling may provide TURN URLs and credentials without changing the launcher/runtime contract.
- The shared Host-owned `eagler-touhou/server/netplay-relay.mjs` supports TURN REST-style short-lived credentials with `TH07_TURN_URLS`, `TH07_TURN_SHARED_SECRET` and `TH07_TURN_TTL_SECONDS`. The shared secret stays server-side; the browser receives only the temporary username/HMAC credential. Static username/credential environment variables exist for local testing only. The relay is infrastructure for both TH06 and TH07 and is not owned by either Runtime repository.
- `eagler-touhou/server/render-coturn-config.cjs` plus `eagler-touhou/server/coturn.env.example` provide the deliberately small production coturn baseline: REST-secret auth, no CLI, multicast/private-target protection, allocation quotas and bandwidth caps. Defaults are conservative for the current ~10 Mbps VPS (`user-quota=12`, `total-quota=256`, `max-bps=131072`, `bps-capacity=786432`) and are environment-overridable. TURN/UDP + TURN/TCP remain available without certificates; TURN/TLS is enabled only when a real cert/key pair is supplied.
- Route release itself is asynchronous at browser task granularity. Do not discard a DataChannel packet merely because the local endpoint has not processed the server's already-decided `route=rtc` message yet. Buffer RTC packets while route is undecided (as the relay path already does), otherwise the first reliable HELLO can be lost at frame zero and create a permanent HELLO/READY deadlock. This is a direct TH06 reuse item.
- Do **not** treat room-membership binding as a mandatory TURN security requirement. Mature TURN deployments generally protect the long-lived signing secret, mint short-lived per-client credentials from a backend, and rely on expiry, quotas/rate limits and usage monitoring. coturn REST usernames may even be timestamp-only when no stable user id exists. Tying credential issuance to an already-authorized room/session is a reasonable low-cost defense-in-depth measure if the surrounding app already has such authorization, but it is not a protocol requirement and must not block TURN rollout by itself.
- `TH07_RTC_TIMEOUT_MS` controls the pre-frame-zero RTC setup window; the older `TH07_DIRECT_TIMEOUT_MS` remains a compatibility alias.
- Read-only diagnostics distinguish transport from path: `__eaglerNetplayTransport` reports `rtc`/`relay`, while `__eaglerNetplayPath` and `__eaglerNetplayRtcPaths` classify selected ICE paths as direct/turn/mixed and protocol without recording candidate addresses/IPs.
- Do not assume a mainland-China direct-connect percentage. Capacity planning must use beta telemetry for direct/turn/relay outcomes, and TURN bandwidth must be budgeted as real relay traffic.
- Do not reduce logical input sampling from 60 Hz to protect a small VPS. Keep the gameplay protocol at 60 Hz and solve data-plane routing/capacity separately.

Current automated evidence: focused 2P RTC binary exchange PASS; forced-no-WebRTC emergency relay PASS; focused 3P full-mesh RTC PASS; full Launcher→room→TH07MP 2P run PASS to 120+ logical frames with both endpoints on `rtc` and local ICE path `direct`; TURN signaling temporary-credential contract PASS. A real TURN relay candidate, cross-public-NAT path and phone/PC RTC path are still unverified and must not be claimed as PASS.

## TH06 follow-up status

TH06 is an explicit next-stage sister-port task, not merely a possible future experiment. Once TH07 multiplayer integration is considered stable enough, perform the same class of work for TH06: room/runtime integration, synchronized logical input lanes, rollback recovery, browser transport/session ownership, mobile touch behavior, and mature post-stall time synchronization. Treat the accepted TH07 implementation as an architectural reference and regression source only; TH06 still requires its own frozen upstream gameplay oracle, hunk audit, constants, lifecycle rules, and real-device verification.

## Required TH06 starting audit

Before modifying TH06:

- Freeze its actual final upstream range and enumerate changed files/hunks.
- Read its current Web/touch/interpolation/rollback implementation and dirty worktree.
- Identify TH06-specific player count, replay layout, resource ownership, death/revival and shared-state rules from its upstream—not from this document.
- Establish ordinary, multiplayer-gameplay and Web build caches plus a focused contract test before the first gameplay work class is marked hunk-complete.
