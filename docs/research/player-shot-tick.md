# Bounded ordinary player-shot tick

## Design and implementation plan

This component owns only the new PlayerShotTickRules files and its new probe,
tests, documentation and private fixtures. It does not modify shot birth,
runtime integration, animation, collision, player-death or shared research engines.
The workspace is non-Git; no worktree, branch, commit or agent is created.

The selected native pass is `0x43f760`, called at `0x43c000` and `0x43c1a6`.
It visits 256 slots at player `+0xfa8`, stride `0x78`. Active `+0x48`,
position `+0x14..+0x1c`, velocity `+0x20..+0x28`, animation `+0x4c`, damage
`+0x60`, size `+0x68/+0x6c`, and runtime record identity `+0x74` remain native
field identities, not a packed host-pointer overlay.

The bounded production deliverable is the ordinary-kind0, non-angular,
unit-clock pre-animation state transition. It clears `+0x58` before the real
record callback, requests real polar-motion output rather than substituting
trigonometry, integrates XYZ and applies native hundredth-floor XY rounding.
Actual record callback absence must be supplied by the host; missing binding
or nonzero targets remain explicit required work. ANM lookup, bounds and
cleanup are host-conditional; a missing primary animation can be retired only
after a genuine lookup miss. A separately named age-tail primitive applies
only after the real animation/bounds survivor continuation is established.

### Ordered work

- [x] Recover native pass, calls, integrator, timer and lookup instructions.
- [x] Observe missing probe and C++ API tests fail.
- [x] Execute original-byte movement, no-match, callback and timer fixtures.
- [x] Implement independent value-only staged state/call plans.
- [x] Match every selected native state in normal and UBSan WASM.
- [x] Verify provenance, supported numeric domain and immutable dependencies.

### Required review cases

- Nonfinite values, unsafe arithmetic, uninitialized clocks and non-unit rate reject.
- Nonzero or unresolved runtime callbacks never silently become null callbacks.
- Negative XY uses floor, not truncation; Z is integrated but not quantized.
- Genuine primary lookup misses do not advance the survivor-only age timer.
- Damage, size, record identity and opaque handles survive unrelated movement.

## Production API and order

All names are in `th128::playershot`; `PlayerShotTickRules.hpp` has no birth,
ANM, registry, runtime or collision dependency. `ShotTickPlan::next` is an
owning 120-byte little-endian image, not a packed object or borrowed span.
No native identity is dereferenced by C++. Native addresses in requests
identify actual targets and fields; they are never host function pointers.

1. `prepareOrdinaryShotTick(slot, actualRecord, clock=1)` rejects malformed
   spans/non-unit clocks. An inactive slot returns unchanged without touching
   the record. Active kind0/non-angular state requires actual record identity
   matching slot `+0x74`. A missing binding or unknown tick target returns
   `NeedsRecordBinding`, not a guessed null callback.
2. With a resolved target, native `+0x58` is cleared. A nonzero target returns
   `NeedsRecordCallback`, site `0x43f7ae`, retaining the genuine target. No
   callback return or post-callback state is modeled. Callback-zero proof
   instead yields `NeedsMotion`, site `0x43f7d4`, target `0x46c1f0`, original
   angle at slot `+0x30` and speed at `+0x2c`, explicitly clock1.
3. `completeOrdinaryShotMotion` takes actual native-compatible velocity.
   Ordinary native code writes positive-zero velocity Z after FSINCOS.
   It returns `NeedsAnimationLookup`: actual primary handle `+0x4c` is looked
   up in the native primary/secondary ANM lists. This is an inlined search at
   `0x43f823..0x43f87d`, so the request's target0 means **no direct call**,
   not a known-null callback or a missing animation.
4. `completeShotAnimationLookup(..., false)` is permitted only on a genuine
   miss. It clears active `+0x48` and requests real `0x467420` at `0x43f88c`
   for secondary `+0x50`, including zero handles. A real found secondary
   requires native flag `0x20000000` propagation to its applicable children;
   a real miss needs no such writes. Only after the host finishes that work
   may `completeShotAnimationCleanup` clear both handles and return `Retired`.
   It does not advance age or invent allocator/lookup returns.
5. A genuine primary hit returns `NeedsAnimationBounds` with conditional
   targets `0x45f110`, `0x45fad0`, `0x45f610`, not a rendered-radius estimate.
   `completeShotAnimationBounds` consumes four actual/helper-compatible corners.
6. A bounds miss requests `0x4679b0` at `0x43fb63`, the genuine ANM deletion
   intent. `completeShotAnimationDelete` may be called only after real host
   execution; it clears primary handle/active, preserves secondary, and retires.
7. A survivor yields `NeedsAnimationSync`. The host must perform real primary
   VM position stores and conditional rotation synchronization. Secondary
   handle work stays explicit at `0x467420`; the current completion deliberately
   rejects nonzero secondary handles rather than guessing a lookup result.
   Only `completeShotAnimationSync` authorizes `ReadyForAgeTail`.
8. `finishShotTickAge` performs the separate native survivor tail, after real
   animation work. No-match cleanup/deletion must not advance this timer.

The host owns slot/resource generations, callback bindings, synchronization,
ANM object lifetimes and atomic publication. Do not apply old plans to recycled
slots. Plans carry no hidden pool generation or callback capture. Retired native
images are not certified pristine birth slots: birth-reuse cleanup remains the
birth host's separate proof obligation. No timeout/default age silently frees
a live shot.

## Native callback ABI and record meaning

The tick loads the runtime record pointer at slot `+0x74`, clears `+0x58`,
then reads its actual callback pointer at record `+0x28`. Before `call eax`
at `0x43f7ae`, EAX is that target, ECX is actual player `[0x4b8a80]`, EDX is
the current shot, EDI is shot `+0x20`, and ESI is0. No fabricated stack args
or TH12 behavior are substituted. The live table used during record binding
is distinct from the raw file's callback ID and PE initial bytes.

The private resource identity is actual Japanese `pl00a.sht`, SHA256
`aed90a45b565510e97cc7d9ab4846d8c93914e57faafe9339e9ab03d47b1511b`.
Group0's first 52-byte record is at file offset744, damage13, speed24, kind0.
Its raw source bytes are retained. Each tick fixture separately declares the
runtime tick binding at record `+0x28`; zero is an explicit synthetic live
context, **not** an inference from file ID0. A nonzero callback prefix stops
at the genuine indirect call before any unknown return.

Damage `+0x60`, size `+0x68/+0x6c`, record identity `+0x74` and unrelated
state are preserved by movement. This pass is not the later `0x43fbf0`
shot/enemy damage query. That query and its real return, collision callbacks,
graze, enemy protection/HP/death and firing cadence are not implemented here.

## Exact movement, quantization and age

At `0x43f7b0`, motion flag bit0 in shot `+0x44` selects angular behavior.
That branch is unsupported by this component. With bit0 clear, native
FSINCOS helper `0x46c1f0` recomputes XY from angle/speed*clock, then forces
velocity Z to positive0. The portable API requires genuine velocity rather
than implementing approximate sine/cosine.

`0x43f81e -> 0x46ae80` first stores float32(position + velocity) for all
three components. It then quantizes each XY component as follows:

```text
scaled = float32(position * 100)
integer = float32(native_floor(float64(scaled)))
position = float32(integer / 100)
```

This is floor, including negative values, not truncation and not a raw
unquantized position step. Z is integrated but not quantized. Zero crossing,
negative/signed-zero and adjacent float32 examples are covered. There is no
absolute tiny-position cutoff in this API.

At `0x43fa5e`, the survivor tail copies current age `+4` into previous `+0`,
increments signed current modulo2^32, and stores float32(time `+8` +1).
The supported clock is exactly1, timer initialized flag bit0 at `+0x10`
must be present and identity `+0xc` must be `0x4b6f58`. INT_MAX wrap is
implemented with unsigned arithmetic, not C++ signed overflow. General
clock rates and uninitialized timers remain unsupported.

## Bounds source and genuine ANM sync

Native reads `(VM.byte[+0x4b3] & 31)`: mode0 calls `0x45f110` at `0x43f8fd`,
mode1 calls `0x45fad0` at `0x43f914`, modes2/3 call `0x45f610` at `0x43f928`.
The latter are **not** silently treated as axis aligned. Native kind0 computes
bounds from the existing ANM state **before this tick's position sync**.

For mode0: ESI is VM; EDI points to corner4, EBX to corner3; stack args are
corner1/corner2 pointers. At the tick call site those stack locals are
EBP-0x34, EBP-0x28, EBP-0x1c and EBP-0x10 in corner1..4 order. Corners are
XYZ outputs; the tick tests only XY. The other helper ABIs differ: at mode1
EAX=VM, ESI=corner4, EDI=corner3, EBX=corner2, stack corner1; at modes2/3
stack VM, EDX=corner4, ESI=corner3, EDI=corner2, EBX=corner1.

`axisAlignedShotAnimationBounds` ports the executed no-parent, zero-local/
secondary-offset mode0 subset of `0x45f110`. Input position is actual VM
`+0x464/+0x468/+0x46c`, sprite dimensions `+0x60/+0x64`, scale `+0x40/+0x44`,
secondary scale `+0x48/+0x4c` (required exactly1), and the original flag word
`+0x4b0`. The host must explicitly confirm no parent at `+0x3c` and zero offsets
`+0x458..+0x460/+0x470..+0x478`. Parent transforms are not omitted by default.
Dimensions are integral0..4096 and scale magnitude<=16. Negative scales are
supported. Anchor bits20..21/22..23 select centered, positive-edge or
negative-edge placement (0/1/2); value3 rejects. Centered low coordinates
use native float32 subtraction then floor; high coordinates are low+extent.
All nine supported anchor pairs and the sampled `.1/.8` scale are compared
with the helper's **actual returned corner bytes**. No generic image
half-height or render-quad approximation is claimed.

Below age10, native skips the corner-culling predicate. At age>=10 it survives
iff at least one of the four corners is strictly inside X(128,512), Y(16,464).
Equality is outside. A huge quad spanning the view without an interior corner
is not automatically retained by an invented rectangle-overlap rule.

**Primary sync X is float32((shotX +128) +192), not shotX+192.** Original
`0x43f9e6` loads the constants at `0x4a65f8` (128) and `0x4a65d8` (192),
retaining both x87 additions before the store at VM `+0x464`. Y is
float32(shotY+16), Z is copied. The independent prefix stops at `0x43fa1c`,
before secondary lookup. Node compares all requested XYZ bits against the
executed VM stores, including `(-128.25,401.5,.1)` -> `(191.75,417.5,.1)`.
The original +192-only metadata bug was observed RED on this vector and fixed
at its source; prior 39-vector checks did **not** cover this obligation.

After primary/secondary sync, if actual primary flags `+0x4b0` have
`0x40000000`, the host must write angle to VM `+0x2c` and OR flag4 into
`+0x4b0`; `required.angle` retains that actual shot angle. The plan is not a
fake completed ANM operation.

## Numeric guard and evidence boundary

- Byte span is exactly120; input fields actually used must be finite with
  magnitude<=2^20. Unsupported ordinary flags or kind do not get fallback physics.
- Float32 additions require either operand zero or exponent difference<=28.
  This conservative pair guard ensures the selected two-term sum is exact in
  double before its single float32 store. Native x87 is not generalized to
  arbitrary adversarial exponent gaps. It is not an absolute position cutoff.
- Sync uses the corresponding checks against320 and16, preserving both native
  horizontal additions. Scale products use integral bounded dimensions so the
  chosen float32 product can be formed exactly in double first.
- No native pointer, buffer, callback closure or span survives a pure call.
- New builds/temp/private compiler cache stay under `work/player-shot-tick-test`;
  shared caches/build/site outputs and other workers' modules are not edited.

The probe declares x87 control `0x037f`, MXCSR `0x1f80`, original floor CPU
feature flag1 and clock1. The reused frozen engine does not expose an MXCSR
parameter, so a scoped factory wrapper constructs the **real Unicorn machine**
and initializes only that CPU register before engine setup. It supplies no game
call return. Original `0x48fdf0..0x48ff11` floor instructions, FSINCOS, inline ANM
search, lookup helper and security-cookie bytes execute without stubs.

Evidence comprises42 cases: 5 movement prefixes, 4 complete256-slot no-match/
inactive passes, 2 unresolved callback/motion calls, 5 returned integrators,
6 independent survivor-age tails, 6 bounds-decision slices, 11 returned axis
bounds helpers, and 3 independent sync prefixes. It is **bounded original-byte
emulation with declared synthetic state**, not native Windows CPU tracing or
a full game-process/frame. Prefixes do not claim to return a whole live tick.

Limits are40000 instructions/run,8192 code bytes/16 spans,131072 data bytes/
32 spans, each span<=65536, and160 generated cases. Unknown calls stop before
execution. Faults, undeclared accesses and budget exhaustion are rejected.

Missing Python module/C++ API tests were observed RED before implementation.
Focused final verification uses configured Python6 tests and Node3 tests,
including all42 portable vectors in ordinary and UBSan WASM, with exceptions
disabled and warnings as errors. Broader main runtime tests are intentionally
not run while the parent owns that active integration path.

Pinned original EXE SHA256:
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
The executable is read-only and never launched. No E-drive writes, original
game window, allocator/Windows API returns, agents, commits or branches exist.
The private report is `local/research/player-shot-tick-fixtures.json`, schema
`th128-original-byte-player-shot-tick-fixtures/1`, serialized UTF-8/LF. It retains
raw selected before/after images, observed corners/VM XYZ, declared-memory
digests, full-trace digests/visit counts, code/constants and generator/engine hashes.

## Historical Identities Before Mode1 Extension

- Header: `715d0f29ba2046a6eab2e42811e7f322d4baeef274b91eb23d647b2d2757262c`.
- CPP: `1b43a4ff9e6764cd9a1869b9ccb52998438284224656d5e210952e038728e8bb`.
- Probe: `fb8b1d49659359101f34cb6d3a3908d72502387eed95a3961be91fcfd8b7af5d`.
- Report: `345ea1a25cc8bb6a19ab0869c348f4a1237fd3d0603cc4c6a7cfad8d29aeae7b`.

The bounded component is ready for real-host integration after the final
targeted green checks. It is not full shot-engine or full FreezeSystem
completion, does not authorize fake animation success, and does not provide
enemy-hit/damage results. Future collision work is independently owned.

## Bounded mode1 extension plan

The original mode0 and +320 proofs remain in the report. Actual shot scripts
using semantic mode1 must not be relabeled mode0. This extension remains in
these same eight files and recovers returning helper `0x45fad0`, not render
quad code. The host translates its semantic mode/anchor flags to native bits.

- [x] Observe missing mode1 probe/API checks RED.
- [x] Execute full original mode1 helpers; independently measure their stored FSINCOS pair.
- [x] Publish a value-only mode1 API consuming actual VM state and genuine rotation outputs.
- [x] Match returned corner bits in ordinary/UBSan WASM and retain previous sync vectors.
- [x] Document numeric/transform limits and re-freeze source/report identities.

The extended report has59 cases, SHA256
`376d480aa49cf65c71de155333ae80bfd7c0e7c7d3b2b46e431a04094b1e39e3`.
The17 added mode1 cases execute complete `0x45fad0` without substituting the
renderer. Separate real `0x46c1f0` calls measure the stored FSINCOS basis;
PlayerShotMotion reproduces those17 basis pairs as well as all114 actual SHT
motions. The host translates semantic mode/anchor bits to the native layout.

Mode1 accepts explicit no-parent/zero-offset ownership, unit secondary scale,
integral sprite sizes0..4096, scale magnitude<=16 and nonzero magnitude>=2^-16.
Nonzero basis components below2^-32 reject. Product/sum rounding uses bounded
double error intervals and refuses uncertain float32 midpoint decisions instead
of inventing an x87 rounding result. All nine anchor pairs and negative scales
have original-byte vectors. Other parent/3D/geometry modes remain unsupported.

The host now advances actual pl00 scripts5/7/9, prior-position bounds, native
sync, survivor-only age and genuine primary retirement. These statements do
not establish the complete fire dispatcher, enemy damage or freeze system.
