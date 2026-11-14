# TH14 browser frame pacing

The browser accumulates refresh time separately from the retail deadline
model. Fractional credit survives timestamp jitter; each callback executes
at most two updates. When the refresh is slow, steady live gameplay may run
both updates and submit only the latest scene. Each update still processes
input, collisions, animation, sound and game time. Loading, menus, dialogue,
replay playback, user-configured frame skip and capture requests retain
their existing update and presentation boundaries.

The implicit WebGL canvas presents when the RAF host returns. The TH14
browser backend therefore skips SDL's extra Asyncify sleep after the blit.
The host reserves its next RAF before entering its serialized Wasm callback
and keeps at most one Wasm frame callback in flight. WebGL work can outlive
that callback, so the renderer flushes after the final blit and tracks its
two most recent submissions with fences. If the older fence is still
pending, ordinary live gameplay keeps updating and skips the stale scene
draw. The zero-timeout poll never waits for the GPU. Two slots allow
rendering to overlap the preceding frame; one slot can halve the visible
update rate when fence completion crosses the next refresh.

Hosted keyboard presses survive a keyup that arrives before the next
simulation tick. The sampler consumes the pending press once; a held key
continues to follow the current state. Focus and session resets clear both.

GLES reuses the same eye/clip matrices for fog, projectability and vertex
submission. The ordinary positive-W world batch bypasses the near-clip
scratch; batches that can cross W still use the exact clipper. The color-only
shader omits unused fragment depth exports. D16 comparisons or writes still
use the original quantization path.

TH14 compares clamped fragment alpha with the measured RGBA8 transitions
around the D3D9 reference byte. This preserves the byte comparison and
avoids quantizing each fragment and indexing the full boundary table in GLSL.

The two TH14 opening compositor clears apply depth immediately. Each color
clear waits until that surface is drawn to, sampled, copied, presented or
read. A later color clear can replace it if it covers the whole rectangle.

TH14 reuses the same rounded world-to-clip matrix for the points of a
background object or ANM quad. Background culling can accept the object
as soon as its accumulated bounds intersect the playfield. Long screen
strips and fans upload their original topology when isolated. Compatible
draws still merge in the original triangle order.

TH14 tracks occupied item slots in groups of 64. Updating, drawing and
attracting items skip empty groups and visit the rest in slot order. The
ordinary and cancel-item free lists retain their original allocation order.
Bullet transform diagnostics snapshot the program only when it fails.
Rectangular cancels share one inverse rotation across bullets and lasers.
The ANM draw lists retain each VM's incoming link for constant-time removal.
A generation counter invalidates old links when a list is rebuilt. Each VM
records its published layer separately from the layer set by its script.

Run `node --test tools/tests/test-th14-frame-scheduler.mjs` for the async
frame queue and compile `runtimes/th12/th12_web/tests/cpp/th14-browser-cadence.cpp`
with `-DPORT_TITLE=14` for the browser clock. Measure Stage 1 separately as
logical updates and presentations: on a slow device these are intentionally
different. The game backbuffer and UI remain 1280×960.

For movement latency, hold each direction for about 600 ms in Stage 1,
including a repeated keydown, and then release it. Compare the event to
the first final blit containing the changed player position or stopped
motion. Poll a fence after that blit to separate command submission from
GPU completion. This does not measure compositor or monitor scanout.
