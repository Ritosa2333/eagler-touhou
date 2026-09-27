# TH10MP `Game error -5` follow-up (2026-09-27)

Reported screenshot: during a live Normal spell attack, the local screen displayed
`Game error -5: Confirmed peer input stopped advancing`. The other player's
screen/error at that moment is unknown. A screenshot of this endpoint does not
show whether the peer stopped first, the input route stopped delivering, or a
local lifecycle fence stranded both endpoints. No root cause for this incident
is established.

The exact string comes from `SessionChannel::Failure::ConfirmedTimeout`. Its
15-second watchdog is armed while a started player session expects input and a
local frame has been captured. It resets only when that peer's contiguous
confirmed-input frontier advances. The title maps a failed network pump to
application `-5` and pauses. Existing bounded retransmission and RTC reliable
repair remain active. Neither the watchdog interval nor simulation/input
rules were changed in this follow-up.

The TH10 worktree started with no tracked modifications. Preexisting untracked
`.codex-tmp/`, `build-eagler-multiplayer/` and Python cache directories were
left intact. This change modifies only TH10MP title diagnostics and its local
test harness. It is uncommitted and undeployed.

At the first failed network pump, TH10 now preserves one compact error string
containing the original transport/session error, local seat and next frame,
all seats' confirmed frontiers, each peer's advertised next frame, latest
local capture, sent/received/repair packet counts, queued bytes and current/
pending screen. `multiplayer_error_detail` exposes that same frozen context
to the existing shell error UI for `-5`, as it already does for `-4`.
`-1` in these fields means no frame has been observed. These numbers allow a
future paired observation to separate a stopped peer from continued packet
delivery without inventing a cause from one screenshot.

Validation:

- Fixture WASM SHA256 `f171edd9f3d549c2e076b92157b75e6ca22996466e053573786a7304f44675f5`.
- `artifacts/multiplayer-tests/error-5-relay-diagnostic.json`: two real relay
  browser endpoints reach confirmed frames 59/119/179 with matching canonical
  state, ANM/RNG and audio. After closing P2, P1 reaches the unchanged
  confirmed-input timeout and reports the new context; PASS.
- `artifacts/multiplayer-tests/error-5-rtc-repair-diagnostic.json`: two real RTC
  endpoints drop every third fast packet, still reach the same confirmed
  checkpoints with matching state/audio, then close P2; P1 reports the RTC
  channel failure and the same context; PASS.
- `git diff --check` PASS (line-ending warnings only).

The controlled relay timeout yielded `next=180`, `confirmed=[179,179,-1]`,
`peer next=[-1,179,-1]`, `capture=179`, `recv=362`, `repair=0`, `queued=46`.
This intentionally induced test is not evidence about the user's actual run.

Remaining evidence needed for root cause: the first error (if any) on the other
endpoint, its local/confirmed/peer-next fields, and whether its receive count
continued changing before the timeout. The production relay records signaling
joins/leaves but no per-frame input frontier, so it cannot reconstruct that
missing evidence after the fact. Do not resolve the symptom by simply increasing
the timeout; it would only delay the freeze if input truly stopped.
