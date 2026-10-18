# Moon cancellation regression (2026-09-30)

Run from the TH20 repository: `node portable/run-th20-moon-probe.mjs`.
Requires the local Emscripten SDK, Puppeteer from the sibling launcher repository,
Chromium at the configured test path, Windows msgothic, and the original DAT at
`build-eagler-smoke/game-data/th20.dat`. Uses loopback port 8099.
The runner adds test-only exports, builds, performs the browser test, and restores
and rebuilds the production host in `finally`. No probe exports are shipped.

The test creates real type-48/color-6 bullets (used by No.37), clears them through
YellowWeapon::update_main, generates 600 additional pattern-115 counter-shots,
and applies the real damage collision/callback path. This exceeds both 256-entry
object pools and forces the heap allocation and retirement path. Twenty rounds
exercise 40,000 enemy bullets and 12,000 additional player counter-shots.

Before the fix, the heap hit test completed its first round then trapped with
`RuntimeError: memory access out of bounds` in the damage/frame path. The original
HitCtrlInf::find registered a nested scheduler::Iterator and overwrote the outer
collision iterator's single observer. A hit callback retired the current region,
so the outer iterator still referenced freed storage. calculate_damage also read
position, group and flags after that callback. The fix uses a non-observing lookup,
captures contact data before dispatch, and postpones limit-driven retirement.

After the fix, 3 heap-hit rounds passed; after removing the old 96-per-frame/512
counter-shot limit, all 20 complete cancellation/hit rounds passed with damage
100 (the normal player damage cap), no page error, and no Wasm trap. Build SHA-256:
8b2b7ca8f36cdbaa60681a37486f23b25b24eadaf624545563c2941de6e2a7df.

This is a deterministic desktop browser lifetime regression, not a full Stage 3
playthrough or a physical mobile device test. The remaining fixed laser masks
were inspected but are not implicated by this reproducer and were not changed.
