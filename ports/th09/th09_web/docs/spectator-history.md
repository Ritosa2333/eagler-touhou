# Local spectator history

Confirmed begin/input/round/end packets remain owned by `SpectatorTape` until
the viewer leaves. `head` is the local playback cursor; `frontier` is the
furthest packet already delivered to native playback. Replaying old packets
does not charge the unread network backlog quota again. Players, relay wire
formats and server history retention are unchanged.

A new spectator still receives only the current match's initializer and input
from that match's start. Earlier matches are never requested or bootstrapped.
Only a viewer who actually received those matches can rewind into them. A fresh
connection creates a fresh local archive and starts its timeline at zero.

The timeline concatenates actual gameplay input counts across matches. Menu
frame gaps are excluded. A complete T9BG/2 initializer is required for rewind;
legacy initializers retain forward-only behavior. Match marks use a higher,
bold solid tick and a match label; round marks use a lower, dashed tick.

A seek to an older time restores that match's existing complete initializer
in the same viewer, then re-feeds its archived confirmed inputs. Native action
4 supplies a fixed forward target while JS refills history in bounded batches.
Only native-validated input is simulated. The normal full-speed 8 ms work budget
and live delay still apply. Go-live is a one-shot target and keeps local history.
During reconstruction the visible thumb stays at the requested time.

History is session memory, not a persistent recording. Leaving spectating,
closing the Runtime or reloading the page releases it. Local archive memory
therefore grows with the received session. The existing unread input count and
byte limits still protect a viewer that stops consuming new network history.

Regression gates: `tests/server/spectator-{tape,rewind,live,room-state}.test.mjs`
and `tests/multiplayer/spectator-playback.cpp`. Actual Release-WASM same-frame
and changed-character checks are recorded separately from scheduler/device
performance acceptance.
