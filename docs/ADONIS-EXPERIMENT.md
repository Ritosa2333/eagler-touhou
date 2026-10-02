# Adonis timing experiment

Branch: `experiment/adonis`. Base: `c239e13`.
This is an opt-in independently authored experiment, not a production timing
policy and not a copy of the attached Adonis/VPatch executables.

## Contract

`AdonisMode::{Rollback,Delay,Hybrid}` are 0/1/2. The adapter negotiates a fixed
session delay (0..9 frames) and mode in its gameplay ABI using
`AdonisGameplayAbi` AFTER any existing delay ABI salt. Do not change D live.
`RecommendAdonisDelay` supplies only a startup RTT-tail suggestion, not a
measurement of one-way latency or an automatic in-game delay controller.

For Delay, use `CoreConfig.allowPrediction=false`. Every player's contiguous
actual input is required. Forged predicted/nonsequential commits and all world
rewinds are rejected. The title must omit world snapshots and resimulation.
The shared input history/confirmation/ACK/redundancy machinery remains useful
without any world rollback. Hybrid retains the existing title rollback owner.

`SessionChannelConfig.adonisPhase=true` replaces its old proportional pacing
advice. It observes the first capture attempt of forward frame N and the first
actual arrival of each remote frame N. Retries/replay cannot resample either.
Statistics close every 16 frames and are exchanged on the control lane with
session/seat/window checks. Match peer-pair windows, use a 4 ms deadband and a
60-frame cooldown. Positive observed input waiting or half of the peer lead
imbalance recommends delaying the next wall-clock work. This implementation
caps each correction at 8 ms and each observation at 250 ms. Those caps are
new bounded policies, not constants claimed from the old binary.

The consumer must drain `TakeAdonisDelayMs()` once in its outer scheduler,
retain any unspent delay across short display callbacks, and leave the fixed
1/60 simulation step unchanged. Do not stack another phase/lead controller.
Raw `AdonisPhase` is available for legacy drivers that do not use
SessionChannel: Reset, ObserveArrival, ObserveDue, PollLocalSample,
ReceiveSample, TakeDelayMs. Encode/decode functions own the 28-byte little-
endian ADP/1 advisory packet. With 3P, samples explicitly identify the peer
pair rather than conflating all remotes.

No binary execution, dynamic D transaction, VPatch busy waiting, scanline
polling, prediction-policy change, simulation speed change or public deployment.

## Evidence

`artifacts/adonis-native` is a local Release GNU 15.2 build. Assertions remain
enabled in tests. All 29 CTest cases passed on 2026-10-02, including the new
Adonis exact-input, delayed capture mapping, forged commit, no-rewind,
first-arrival/retry, deadband, cooldown, codec, 3P and RTT-tail component gate.
Existing core/channel/Replay/touch/journal/repair tests also passed.
These component gates do not prove title-world equivalence, browser transport,
physical-device performance or input-to-photon latency. Those belong to each
title's experiment report.
