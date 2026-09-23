# TH10 native network correctness slice

Status: verified focused functionality, **not a completed multiplayer product**.

## Ownership and sequence

`NetplayRuntime` binds the shared `SessionChannel` and `BrowserPeerTransport`
to TH10's session descriptor and rollback core. The browser loop pumps network
work even when the native world cannot advance. Only fresh local captures enter
the core; retransmission, ACK repair and resimulation never sample the device.
The native authored update and Draw still execute once per logical frame.

Retirement waits for corrected simulation, confirmed remote input and the peer
ACK of local input. The common channel retains old terminal ACKs while the new
world/session negotiates. The fresh world cannot tick or perform authored Draw
until the new HELLO/READY gate opens. Old-session packets cannot populate it.

This is correctness/lifecycle work. Complete the entire multiplayer functional
profile before performance changes, and keep the ordinary single-player
baseline independently attributable. No optimization is admitted by this slice.

## Actual browser transport evidence

`node portable/multiplayer/check-network.mjs` starts an isolated loopback server
and the real shared relay. Python/Playwright supplies only each endpoint's local
input; the other seats arrive through actual RTC DataChannels or WebSockets.
The harness checks the selected route and compares corrected native gameplay,
ANM, both RNGs and confirmed audio at identical confirmed boundaries. Its
short runs are not a claim of all-stage or mobile smoothness.

Production MP WASM:
`a303e94791e5b0e16801072572c3eb802f0a06358565c1b55a0d46a92901a5de`.

Reports under `artifacts/multiplayer-tests/`:

| Suite directory | Passing case |
| --- | --- |
| `network-320b4ea2-2662-48b9-8701-0bbfad25dd66` | RTC 2P |
| `network-890cd2c5-38a0-4124-94c8-1a2ca49ba7df` | RTC 3P, every third fast-lane send dropped |
| `network-7d4b9098-dcc4-4b12-bb52-9004e39d5a36` | Forced relay 2P |
| `network-c2c1da06-760b-4426-86e1-113cb5f5ff63` | Forced relay 3P |

These compare confirmed frames 59, 119 and 179, then close the native endpoint
and require its partner to detect disconnection. The earlier relay-disconnect
failure remains in `network-0f6b09d8-bfd2-44a5-b49e-3fee4fc9a781`.
The shared relay fix is Launcher experiment commit `d8d0363`; it preserves the
lobby/seat owner while retiring the gameplay run and does not kill an RTC run
when its unused relay sockets close.

The separate diagnostic fixture build has WASM
`d7b9465e500f872c49233c33ab6760620b91e8e199b269ae5e7ac34a5c222e6a`.
Suite `network-3d5d8d97-6ae1-4181-a8cf-5ac177e43cad` passes both RTC and relay
2P: native full-wipe/retry, fresh-session gate, actual old-wire replay/rejection,
then thirty corrected frames in the new run. The fixture supplies the explicit
initial wipe condition; all subsequent lifecycle behavior remains native.

The current source inventories and actual WASM hashes for ordinary, production
MP and diagnostic MP were rechecked on resumption. No source differences were
found. The 19-entry rule lane was rerun successfully; its log is
`artifacts/multiplayer-tests/rules-functional-final.log`.

## Ordinary baseline

The ordinary production WASM before and after this network slice is exactly
`1f5a9557f62f33c1243caa72a8649cd8335944665ceb70da763b8f0fe79f67e1`.
The rebuild log is `artifacts/multiplayer-tests/build-sp-after-network.log`.

The prior full ordinary diagnostic Replay run compared the published golden
without changing it: Lunatic 84,797 ticks and Extra 39,111 ticks, both PASS.
Its diagnostic WASM is
`4396d3bb540edeeae45cd945386ca5bd6cae4edd76a271ac30feb5c0c8660b42`;
the capture and comparison evidence lives under
`artifacts/replay-verifier/daily-audit-74e475e0-6070-4944-9b21-16595d57b6f8/`.
This historical diagnostic evidence is separate from the resumed production
binary identity check; it is not a claim that another daily run was executed.

## Still required

Multiseat Replay recording/playback, read-only start-time spectators,
authoritative analog/touch input, real Launcher configure/start/save lifecycle,
all-stage/result and negative-protocol coverage, ordinary regressions and the
full promotion gates remain required. Device and public deployment acceptance
must be named separately. No push, deployment or canonical promotion is implied.
