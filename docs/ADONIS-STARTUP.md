# Actual-channel Adonis startup (TH08 / TH09 / TH10 experiment)

`AdonisStartup.hpp` is a bounded pre-frame-zero owner, not another game driver.
Both experimental modes use it after the actual game transport is open and
all participating native worlds have finished construction. TH09 retains its
two-player wire behavior; TH08 and TH10 also admit three players.

After every required input channel and peer HELLO is ready, startup waits one
second before its first probe. A channel closure before probing restarts this
settling interval. The interval contributes no RTT samples and gameplay remains
blocked at frame zero. This user-requested settling interval is separate from
the original Adonis2 probe schedule.

Waiting for peers to finish loading and for the transport/HELLO fence has a
separate 45-second deadline. The 10-second measurement/negotiation deadline
starts only when all peers pass that fence, before the one-second settling
interval. A fast endpoint cannot spend the measurement deadline waiting for a
slower endpoint to open its game connection. Neither deadline supplies fallback
samples or a guessed delay.

129 numbered probes (1..129) are scheduled with 16 ms relative waits on
`PeerTransport::SendTo`, with echoes on that same input lane. Slot zero is
unused; slots 0..9 are excluded, leaving 120 measurement slots (10..129).
The last send is followed by its normal 16 ms wait and then a fixed 200 ms tail,
even if all replies arrive early. At least 96 valid responses are required. Missing/late
responses are reported separately, never silently counted as zero latency.
Each pump sends at most one new probe. A >500 ms pump gap invalidates the
measurement rather than pretending background-throttled samples are normal.
Handshake/measurement times out after ten seconds, without applying a guess.

The statistic uses the supplied Adonis2 report's 95-percentile-style ordering
and even-count average. Both sides publish immutable summaries. The full
buffer estimate is `max(1, ceil(floor(max(peer P95 RTTs in us)/2)*60/1000000))`.
No extra frame is added. Integer halving follows the original converter at the
available microsecond resolution. RTT/2 is still a **symmetric-path estimate**, not measured one-way delay,
and this is not a guarantee that every future loss or blackout fits the buffer.
The probe observes network delivery and Runtime wakeup, not boss simulation,
GPU completion or input-to-photon latency. Actual play continues collecting
input first-arrival/first-due statistics in `AdonisPhase`.

Pure mode uses the full estimate. Automatic hybrid targets one queued input
frame, removing at most the negotiated prediction budget (one/two, two by
default): `P=min(maximumReserve,B-1)`, `D=B-P`. Thus B=1/2/3/4 gives D=1/1/1/2
with the default budget. It never reduces automatic D to zero.
The existing rollback window, button predictor and exact-input preference are
not shortened or overridden. Manual D=0..9 remains authoritative but still
goes through calibration and peer agreement. An automatic D above 9 fails
explicitly; it is not silently clamped and advertised as covering the link.

ADS/2 adds both sides' minimum/mean/maximum RTT to immutable summaries.
TH09 processes startup packets on arrival with a high-resolution clock instead
of waiting for the periodic Runtime pump. Browser timers can run late; matching
the sampling algorithm does not promise Windows Sleep/QPC scheduling equivalence.
The connection UI shows progress and both completed summaries without gating
gameplay for a frontend result display.

Two players retain the exact ADS/2 64-byte packet. Three players use ADS/3,
measure every directed input link, and report the worst outbound link per
participant. Each required link needs 96/120 successful samples. Probes still
advance at most one numbered round per pump; the host requires all accepts and
all commit acknowledgements. Broadcast client controls are ignored by other
clients rather than being mistaken for host authority.

`AdonisConnection.hpp` supplies the shared calibration/gameplay transport
handoff, bounded deferred native HELLOs and 32-word status. Title adapters own
world readiness and apply the committed D/P before native input capture.
`browser/adonis-calibration.mjs` mirrors that status and services arrivals and
native wakeups; it does not choose D, drive the world or change logical time.

Control goes through hello, both summaries, host proposal, client acceptance,
host commit and client commit acknowledgement. Late/duplicate controls are
idempotent; retired session IDs are ignored. Mode, requested D/auto, seed,
gameplay/build ABI and prediction reserve must agree. Gameplay HELLO follows
the reliable commit; the title adapter transfers polling ownership without
losing a packet or delivering calibration packets to the gameplay decoder.

`SessionChannelConfig::adonisPredictionFrames` makes the ongoing phase
controller respect the intentionally reserved prediction lead. Raw waiting
statistics remain raw, but only waiting beyond that allowance delays the
schedule. Otherwise phase correction would slowly put back the latency saved
at startup. Defaults remain zero, preserving all existing consumers.

Native gate: `eagler-common.adonis-startup`; existing phase/core/channel gates
remain active. Tests cover both modes, automatic/manual 0/1/9, one/two reserve,
asymmetric transport, control loss/retry, incompatible configuration,
invalid/stale packets, clock/timeout/interruption and reuse. This is
algorithm/ownership evidence, not a phone or performance claim.
