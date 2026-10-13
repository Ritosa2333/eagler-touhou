# TH11 bullet subpixel presentation

Authority: Eagler `ADAPTER_BEHAVIOR_INVARIANTS.md` (final rendering must not
discard subpixel motion), and `playbooks/interpolation.md` /
`playbooks/performance-rendering.md`.

The ordinary ANM quad path floors centered mode-0 geometry and rounds final
pixel-aligned vertices. Moving bullets therefore lose fractional coordinates
even though BulletState stores their position as floats.

`BulletManager::draw_group` now explicitly opts into continuous quad geometry
and submission. This skips both quantization steps, retains the D3D -0.5
half-pixel convention and uses the same UV, color, filtering and batching.
Default ANM calls, ASCII sprites and logical screen-UV calculations retain
their original pixel-aligned behavior. No simulation, hitbox, RNG, ECL, Replay
or scheduling changes are included. This does not add high-refresh support or
claim Presentation Lab admission.

Verification command:

```text
EMSDK=<explicit Emscripten SDK> node portable/check-subpixel.mjs
EMSDK=<explicit Emscripten SDK> node portable/build.mjs --thprac
```

The focused test exercises 288 mode/anchor/size/rotation combinations through
the real ANM renderer: fractional translation, unchanged VM state, repeated
draw identity, UV/color preservation, half-pixel offset, default ASCII and
logical geometry alignment. It also verifies the production bullet owner
selects the continuous path. GPU atlas sampling and physical device visual
acceptance are not proven by this CPU vertex-submission test.

Local integration base: `3f748534e0e7835cce40b11a64cac900de12b807` (`eagler`).
Experiment: `_scratch/th11-subpixel`, `experiment/th11-subpixel`, same base.
The reviewed fix is integrated into the canonical TH11 checkout and the
shared-purple Runtime branch. Publication is tracked by the operator release
manifest; this document does not claim physical-device visual acceptance.
