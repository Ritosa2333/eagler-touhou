# Room connection diagnostics

`src/launcher/room-network.mts` owns Launcher-only, pre-game probes. It does not join a game run, carry gameplay inputs or change Runtime transport selection.

For each other online seated player, the launcher creates a direct RTC data channel (STUN only) and, when configured, a separate RTC data channel forced through TURN. A third echo travels over the lobby WebSocket through the relay to that same peer and back. All three numbers are peer RTT measured by the requesting client's monotonic clock. “Variation” is the absolute difference from the preceding valid RTT sample, not statistical network jitter or one-way latency.

The relay advertises support and ICE servers in the optional `roomProbe` field of its initial `state` message, preserving the existing first-message contract. Probe messages are targeted to an online seated peer in the same lobby. The relay derives sender identity from the socket, validates the envelope and limits messages to 180 per 10 seconds per connection. Spectators cannot send or receive these probes. Settings, ready flags, launch and Runtime signaling are separate.

The lexicographically smaller client ID offers RTC connections. Retry by the other endpoint requests a new offer. Each connection has a generation token; stale async completions and old channel messages cannot update a replacement peer. Polling is every 2.5 seconds. RTT samples expire after 8 seconds and connection setup is bounded at 12 seconds. Leaving a seat, a room disconnect, starting a run or leaving the page closes diagnostics. A missing capability or TURN configuration produces an unavailable state rather than an estimate.

Validation commands after `npm run build:launcher`:

```text
node tests/test-room-network.mjs
node tests/test-room-probe-relay.mjs
node tests/test-netplay-relay-product-policy.mjs
```

UI previews and screenshot evidence from the redesign are local `.cache/room-qa/` artifacts. See `design-qa.md` for the tested interactions and explicit limits. Ship `server/room-probe-policy.mjs` alongside the relay; the Operator delivery mapping includes it.

## Stage lobby UI

The selected room design uses the existing homepage wallpaper without blur, a single desktop bottom dock and a portrait bottom sheet. The warm Material 3 Tonal Spot tokens are derived from the wallpaper. Primary state and the direct/TURN summary stay in the dock; network details, player controls, spectator roster and host room settings share a native dialog. Ready/start follows the existing room protocol. The global settings drawer overlays the page and uses the same palette.

Per the user's explicit direction, avatar artwork is represented by initials. No AI-generated images are part of the public tree or asset manifest. The source image references are design evidence only. All new icons are vendored Phosphor regular assets with their license. See `design-qa.md` for comparison evidence, browser checks, isolation status and the unrelated full-check EPERM blocker.
