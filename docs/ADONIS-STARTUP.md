# Actual-channel Adonis startup (TH09-first experiment)

`AdonisStartup.hpp` is a bounded pre-frame-zero owner, not another game driver.
Both experimental modes use it after the actual game transport is open and
both native worlds have finished construction. No other title is enabled here.

130 numbered probes are scheduled at 60 Hz on `PeerTransport::SendTo`, with
echoes on that same input lane. Ten warm-up slots are excluded, leaving 120
measurement slots; at least 96 valid responses are required. Missing/late
responses are reported separately, never silently counted as zero latency.
Each pump sends at most one new probe. A >500 ms pump gap invalidates the
measurement rather than pretending background-throttled samples are normal.
Handshake/measurement times out after ten seconds, without applying a guess.

The statistic uses the supplied Adonis2 report's 95-percentile-style ordering
and even-count average. Both sides publish immutable summaries. The full
buffer estimate is `ceil(max(peer P95 RTTs) / 2 / (1000/60)) + 1` frames.
The extra frame is an explicit conservative Runtime consumption-boundary
margin. RTT/2 is still a **symmetric-path estimate**, not measured one-way delay,
and this is not a guarantee that every future loss or blackout fits the buffer.
The probe observes network delivery and Runtime wakeup, not boss simulation,
GPU completion or input-to-photon latency. Actual play continues collecting
input first-arrival/first-due statistics in `AdonisPhase`.

Pure mode uses the full estimate. Hybrid removes a negotiated one/two-frame
prediction reserve (two by default) from that estimate; D is floored at zero.
The existing rollback window, button predictor and exact-input preference are
not shortened or overridden. Manual D=0..9 remains authoritative but still
goes through calibration and peer agreement. An automatic D above 9 fails
explicitly; it is not silently clamped and advertised as covering the link.

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
