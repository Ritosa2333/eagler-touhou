# Geometry526: owning mesh and actual@R presentation

Verified checkpoint on2026-10-30, main thread only. Runtime:
`182a39317c5a4843cedb7bb744b4b84da32d471b2cbcfbdc3c141cb1100c6ecb`.
This is a bounded feature checkpoint, not a complete game or native frame trace.

## Original-byte scope

-526 handler41c292 first retires a previous object, then writes the authored
  radius/color, initial scale16 and zero angles. Positive radius reaches the
  original allocation boundary; nonpositive radius does not allocate.
-412550 fills original28-byte vertices and12-byte base positions;412700 copies
  real neighbor-column triangle strips. Four complete preallocated grid/copy
  returns are recorded separately from allocation/retirement prefixes.
-416d89..41722d completes its real geometry-update prefix, including the actual
  installed32-bit D3DXVec3Normalize SSE2 setter/export/dispatch/helper bytes.
  The strict new geometry-only emulator permits only the exact declared readonly
  import slots. It does not load the Windows DLL, invent API/allocator returns,
  or change shared probe implementations/old fixture hashes. CPU dispatch and
  legacy CRT x87 state are declared inputs, not a loader execution claim.
- Packed ANM flags are TH128-specific:65 anchors use20/22;67 mode uses24.
  Original handler probes expose the inherited19/21/23 mismatch. Script82 sets
  Main blend3;412520 then clears row blend bits5..7. Thus Main is mode0/blend3,
  but custom rows are mode12/blend0. Merely reading the ANM resource is not enough
  to recover the factory's final state.

The portable normalization seed is correctly rounded rather than a promise of
hardware-specific RSQRTSS approximation. Two displaced vertices differ by oneULP
in one sampled17x17 update (six words including duplicate strip copies), maximum
1/32768pixel. Colors, UVs, base positions, angles and other words match exactly.
Tests bound displacedXY to at most twoULPs and1/16384pixel; no golden-specific
value is inserted into production math.

## Owning production path

`Geometry526Host` binds16 ordered actualtext82 VMs and owns their grids, snapshots
and lifetimes. Creation is followed by actual geometry updating in the same
local enemy pass; the duplicate constructor guard does not update twice.
Replacement retires old rows; zero/negative radius retires without creating
dummy geometry. Invalid input/resource, missing owner and script faults stop.

`th128_world_enable_geometry526` requires a fresh world and actualtext.anm.
Without that owning host,526 remains unregistered and stops as before. With it,
the world exports actual Main82 plus typed34-vertex strips. Presentation is pure:
it does not tick ANM, advance the game or consume RNG.

## Renderer pipeline

Original registration recovery provides31 layer priorities. Numeric layer order
would incorrectly place layer29 after enemy/Player/HUD. Actual435d20 aliases
text@R entry2 to Main's first target and binds Main82. Draw12 switches targets;
draw14 copies Main82 then draws layer29 rows. The browser mirrors this bounded
stage pass with an opaque640x480 RGB capture and flipped framebufferV, never a
fake PNG. Unknown layer27/28 priorities and unestablished dynamic targets stop.

Desktop/mobile WebGL2 tests use actualtext82 state and a declared synthetic
identity strip to verify orientation, capture-before-layer5, Main copy order,
same-frame image reuse, zeroGL errors and zero pixel differences. This is not
evidence of original GPU rasterization, complete multi-target Main IRQ lifetime,
STD backgrounds or full native presentation equivalence.

## Actual execution and verification

Normal/seed34772 passes the former3170/op526 boundary. At3172 it has16 rows and
scale18;20ticks later scale58. Both1280/390 widths reach3270/main offset508/op420
with8 deaths/8 respawns.420 is a real boss-owner wait; it is not bypassed.

154 Node/C++/WASM,339 Python and3 visual-probe tests pass without skips;14 browser
scripts pass.95 source hashes match the immutable generation. Original/prepared
Japanese/Chinese assets are unchanged. Evidence: `../geometry526-verification.json`,
`../geometry526-midboss-browser.json`, `../geometry526-framebuffer-browser.json`.

Next: genuine420 per-context hold,404 center/extent clamp, MBoss attacks and real
414 VM reset/resolve/invoke. Z/freezing/Perfect Freeze, lasers, full initialization,
backgrounds, routes/endings and replay simulation remain incomplete.
