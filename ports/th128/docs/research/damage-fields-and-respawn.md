# Damage fields and bounded respawn host

## Evidence and implemented scope

The fixed Japanese executable is read-only. All observations below are bounded
original-instruction emulation, not a Windows game process or native CPU trace.
No allocator, operating-system API or callback is assigned a fabricated return.

`damage_field_probe.py` records the returned factory at `0x43df90`, independent
growth/expiry tails, complete circular-source queries, cursor wrap/full-pool
behavior, and complete stationary motion with original position quantization.
`DamageFieldPool` compares its 120-byte images directly with these observations
in both ordinary WASM and UBSan builds.

- Pool: 2000 entries, stride `0x78`, player offset `0x8edc`, cursor `0x43a8c`.
  A full scan returns the existing entry without clearing or recreating it.
- Respawn birth: radius32, growth16, remaining30, damage150, tag0. Flags become3;
  cumulative limit999999 and interval4 are stored by the original factory.
- Unit update grows radius and reduces remaining before enemy queries. The active
  bit clears at remaining<=0. Stationary X/Y are quantized by separately stored
  float32(position*100), original floor, then division by100. A negative tiny
  position therefore does not truncate towards zero.
- Changed remaining counters divisible by4 skip contact. Equal counters bypass
  this division. The circular query compares a float32 stored squared distance
  with the retained squared radius, includes tangency, ignores depth, updates
  outFlag bit2 for nonzero tags, and zeroes future damage at the cumulative limit.
- Ordinary-shot impacts and their motivation/power side effects run before the
  field query. Ordinary shots cap at160; the combined query caps at2000. Nonzero
  final damage awards internal score `(damage/10+10)/10`, capped at999999999.

Moving/rotated/rectangular sources, arbitrary clocks, non-finite input and
unsupported numeric domains still stop explicitly. This is not the ice
propagation or Perfect Freeze host.

## Respawn sequence

The player host retains real death state2 for30 unit updates. With the explicitly
configured respawn context, it changes to state0 before requiring the field birth,
then requires the conditional post-field helper, saves the death position, sets
raw(0,61440)/pixel(0,480), resets state time and sets protection280.

State0 entry uses original integer division:

`rawY = 61440 - elapsed*10240/60`

For elapsed<30 the original empty-laser-list helper paths record the saved center;
their two radii are `float(elapsed*512/30+64)` and one quarter of that stored value.
The currently supported stage subset cannot emit lasers: an unregistered laser
ECL operation stops before emission. No populated laser manager is pretended empty.
At elapsed>=30 actual ordinary bullets are changed to state3, their authored ANM
IRQ1 is executed, cancellation effects and their original drift are created, and
age resets only after those required effects succeed. No bullet list is simply
erased. At elapsed60 normal state1 and its movement/shooting continuation resume.

`0x427190` writes the raw global field `0x4b4d04` to2 before refreshing controls.
Consequently it must not be described as an immutable difficulty argument. The
portable bounded control plan retains eight pending interrupts; with value08=0
they are `[2,2,7,3,3,3,3,3]`. Nonzero live callback targets still require a genuine
HUD adapter and stop rather than receiving a dummy return.

## Initialization limitation

The enhanced browser entry explicitly supplies normal player state/protection,
negative death-award selector and eight zero HUD callback targets. These are
diagnostic contexts, not proof of the complete original initializer or HUD.
First death/respawn passes at world tick599 in desktop and390px browser checks.
That verifies this path only, not game-over, continues, every player state, a
whole stage, other routes or original full-frame rendering parity.

Z charge/freezing, populated laser behavior, native HUD construction, bosses,
backgrounds, stage transitions, endings, persistence and replay simulation remain
separate full-port obligations.
