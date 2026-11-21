# TH128 bounded ice-object birth

## Implemented component and evidence boundary

`src/game/IceBirthRules.hpp/.cpp` ports slot selection and the independent
object-state work of native builder `0x410590`, initializer `0x410480` and
list helper `0x410660`. These functions are new, reusable, pure C++20 rules.
They do not render a circle as a substitute for native ice objects.

The complete selected native builder really returns in 31 fixtures, after
executing its initializer and both intrusive-list insertions. It makes no
ANM, allocator or Windows API call. Four additional builder fixtures stop
at declared selection/pre-list boundaries and do not claim a return.

The production interface returns a **plan requiring two real list
insertions**, not an already registered object. Existing freeze activation,
player-hit, cancellation, manager, animation, collision and runtime files
are not modified or linked into this rules component.

## Public API

All types are in `th128::ice`:

```cpp
std::optional<SlotSelection> selectIceSlot(
    std::span<const int32_t> occupancy, uint32_t cursor) noexcept;
std::optional<IceBirthPlan> prepareIceBirth(
    std::span<const int32_t> occupancy, uint32_t cursor,
    const IceBirthInput&, const IceObjectState& prior, bool linksDetached) noexcept;
std::optional<ActivationIcePlan> planActivationIceBirth(
    int32_t nativeFrozenPower, const Position&) noexcept;
```

`IceBirthInput` contains XYZ, neutral float `value`, signed `duration` and
signed `mode`. `IceObjectState` contains occupancy, XYZ/value, two timers,
neutral `state48`, mode, two opaque animation handles and `maskSelector`.
It is a semantic C++ structure, **not** a packed native-memory overlay.

`prepareIceBirth` returns `BirthContinuation::NeedsListInsertion`. Its
`selection.nextCursor` and `next` state are proposals until the host applies
them and the two ordered `requiredInsertions`. It never mutates the input
span, retains its address, invokes a callback or claims manager publication.

The supplied `prior` must be the selected free slot's actual snapshot in the
same manager generation as the occupancy span. `linksDetached=true` is a
host proof that both old intrusive nodes are detached and clear, not a
request to ignore old links. The host owns slot/ID generations, source
resource and handle lifetimes, synchronization, and atomic publication.
Plans contain values only; do not apply an old plan after changing/reloading
the pool. Native addresses in the metadata are identities/offsets and must
not be dereferenced as host process pointers.

## Actual builder ABI and selection

At `0x410590`, ECX is the pool manager. Four stack arguments are:

1. pointer to three source float32 position values;
2. a float32 scalar, copied into object `+0x1c`;
3. a signed int32 duration;
4. a signed int32 mode, copied into object `+0x4c`.

The genuine return puts the selected object address in EAX and executes
`ret 0x10`: the harness observes stack delta 20 including its return address.
The initializer receives EAX=position pointer, EDX=selected object, and its
own stack scalar/duration; it returns with `ret 8`.

The pool begins at manager `+0x10c`, has exactly 4000 records of `0x68` bytes,
and ends at cursor field `+0x65a0c`. Record occupancy is the signed dword
at `+0xc`; any nonzero bit pattern counts as occupied. The native loop:

- examines the record named by the current cursor;
- stores `(cursor + 1) % 4000` **before** testing that record's occupancy;
- stops on the first zero, or after 4000 examined records;
- on exhaustion selects the **last examined occupied record**, not null.

The exhausted-pool fixture starts cursor19 and genuinely executes 60007
instructions to the selection boundary, choosing slot18 and next cursor19.
`selectIceSlot` exposes this exact result with `overwritesOccupied=true`.
`prepareIceBirth` deliberately rejects that result: occupied-node reuse,
resource teardown and native saturation topology are not implemented by
assuming all existing objects are safe to overwrite.

## Object layout and initial state

| Native offset | Meaning/work proved here |
| --- | --- |
| `+0/+4/+8` | spatial-list owner/next/previous node |
| `+0xc` | occupancy, written 1 |
| `+0x10/+0x14/+0x18` | exact copied XYZ |
| `+0x1c` | exact copied scalar `value` |
| `+0x20/+0x24/+0x28` | lifetime previous/current/float |
| `+0x2c/+0x30` | lifetime rate-source identity / flags |
| `+0x34/+0x38/+0x3c` | delay previous/current/float |
| `+0x40/+0x44` | delay rate-source identity / flags |
| `+0x48` | neutral state field, written 1 |
| `+0x4c` | exact copied signed mode |
| `+0x50/+0x54` | existing opaque animation handles, **retained** |
| `+0x58` | mask selector, written zero |
| `+0x5c/+0x60/+0x64` | global-list owner/next/previous node |

Each timer has initialization flag bit0. If absent, the builder sets it
without clearing other flags and writes rate-source identity `0x4b6f58`.
An already initialized timer retains its existing rate-source identity.
The port does not invent a numeric rate for an unknown identity; the future
timer/update host must provide a genuine supported binding.

Lifetime current is the duration, previous is its int32 wrapping predecessor,
and float is float32(duration). INT_MIN's previous therefore wraps to INT_MAX;
the C++ port implements this with unsigned arithmetic/bit casting, not signed
overflow. Mode0 gets delay previous2/current3/float3. Every nonzero mode gets
previous-1/current0/float0. This is birth state, not a proved complete clock
update or a wall-clock duration claim.

**Object `+0x44` is timer flags, not a radius.** The scalar argument lives at
`+0x1c`. Static overlap helper `0x40fc60` later reads that value and compares
XY squared distance against a squared sum of extents. This supports a local
geometric role, but does not establish a displayed/rendered radius, a filled
mask, or all ice propagation rules. The API conservatively calls it `value`.

## Both mandatory list insertions

The declared subset uses the same active manager in ECX and global
`[0x4b8938]`. `0x410660` calculates:

```text
column = trunc((float32(x) + 192) / -48)
row    = trunc(float32(y) / -44.79999923706055)
bucket = -10 * column - row
```

These are the exact saved EXE constants, not a guessed regular grid using
44.8 double. The selected bucket must be in 0..79. The actual conversion
helper `0x48fac0` executes its original SSE path with explicitly declared
CPU-feature flag1, not a fabricated float-to-int return.

**Review correction:** these formulas specify native arithmetic, not a claim
that evaluating them with intermediate double precision is always equivalent.
The x87 addition/division retains extended precision until the conversion
helper stores a double. For float X=`-2^-46` (bits`0xa8800000`), Y359 produces
native bucket38, not the old duplicated-double bucket48; Y-1792 produces
native bucket-10, not the formerly accepted bucket0. The previous broad
numeric claim and report digest are superseded by this review fix.

The portable API now conservatively rejects every nonzero |X|<2^-16.
It also rejects a horizontal/vertical quotient within2^-20 of a nonzero
integer unless its numerator equals that integer times the denominator
exactly. This general guard is not a special case for the two review inputs.
The minimum X makes X+192 exactly representable in double for the declared
float32 magnitude range; the conservative quotient margin avoids unproved
rounding across a truncation boundary. Exact-ratio boundaries remain allowed.
Some legitimate native births are intentionally unsupported by this bounded
port; broader precision equivalence is pending and is not claimed.

The probe no longer computes a candidate bucket to initialize or inspect
list heads. It first executes to`0x41069c`, before the first bucket read,
and derives the signed observed bucket from executed EAX address arithmetic.
For a real return, object node`+8` must independently equal that observed
sentinel address. Initial nonempty heads and final `bucketFirst` use only
this observation. Native out-of-table cases stop before memory access and
remain incomplete; Python's port-domain predicate only labels intentional
CPP rejection and is not the expected-bucket oracle.

The required operations, in order, are:

1. Insert object node `+0` at the front of spatial sentinel
   `manager + 0x65a1c + bucket*12`.
2. Insert object node `+0x5c` at the front of global sentinel
   `manager + 0x65a10`, with node owner referring to the same object.

For each insertion the new node's next is the old first node; previous is
the sentinel; an old first node's previous is patched; sentinel+4 becomes
the new node. Empty and nonempty heads, both owner links, both neighbor
patches, wraparound and the chosen returned pointer are checked against
the actually executed native writes. These are ice-manager lists, not ANM
primary/secondary registration lists.

## Activation effects, resource identity and nativeFrozenPower

The frozen activation implementation remains the owner of its two `0x54`
activation slots, release/charge guards, saved power, and power consumption.
Calling the new plan does not authorize a below-3000 player release or
consume/clear power. The ordinary supported saved `nativeFrozenPower` is
0..10000; special values above10000 remain unsupported here.

`planActivationIceBirth` exposes the mandatory call order:

1. At `0x410276`, call **the real `0x407ec0` factory**: EAX=23 is the
   **layer**, ECX resolves the resource at manager `+0x65ddc`, stack arguments
   are host scratch-output pointer, script33 and supplied XYZ pointer.
   Only after a real successful result may its opaque handle be stored at
   activation slot `+0x50` and manager active flag `+0x6b218` be set1.
2. At `0x41029b`, call `0x4584a0`: EAX=47, stack float copied from position X.
3. At `0x4102c9`, call the builder with position, float50,
   `trunc(nativeFrozenPower / 100)`, and mode1.

Two separate original-byte prefixes stop before the real effect/sound calls;
five more stop before the real builder call. They are **independent slices**,
not a resumed activation with an assumed allocator/effect return.

Loader bytes `0x40e825..0x40e83f` load string `ice.anm` at `0x4a2200`, pass
resource slot **36** to `0x465600`, and store its returned pointer at manager
`+0x65ddc`. Native factory layer23 must not be confused with resource slot36
or the port's arbitrary host-local ANM registry key. An initial naming error
was caught by a failing regression and corrected before this freeze.

Actual factory `0x407ec0` binds through `0x4593b0`, then calls `0x466d60`
at `0x407f70`. Static registration bytes prove **primary-tail** insertion on
ANM manager head `+0x8b9704` / tail `+0x8b9708`, the primary30 domain.
`0x466de0` is not a different list domain; secondary8 is not selected here.
No TH12 custom-callback semantics are substituted. Neither native ANM bind,
callback execution nor script33's full TH128 behavior is implemented by this
birth component. A missing factory/resource/sound host must remain pending
or unsupported, never return a fake zero/handle or render a placeholder.

## Mask pointer obligations

The builder clears `maskSelector` to0; it does not allocate a bitmap, attach
a mask pointer or call `0x4106e0`. The first activation birth therefore must
not be advertised as having already filled the frozen-area mask.

Static bullet-to-ice caller slices separately show calls to `0x4106e0`
using EAX=position, stack manager/bitmap-pointer/height/words-per-row, then
setting selector1..5 and attaching an actual effect handle at object `+0x54`:

| Selector | Original bitmap pointer | Rows | Dwords/row |
| --- | --- | --- | --- |
| 1 | `0x4b1ef8` | 21 | 1 |
| 2 | `0x4b1f50` | 25 | 1 |
| 3 | `0x4b1fb8` | 33 | 1 |
| 4 | `0x4b2040` | 49 | 2 |
| 5 | `0x4b21c8` | 65 | 2 |

These pointer identities, bytes and hashes are private static evidence, not
host pointers or performed mask operations. The existing mask counter's
manager `+0x65de4..+0x6b1e4` span is separate from the object pool. A future
mask host must own actual bitmap bytes, clipped writes and mask lifetimes;
it cannot treat a selector, scalar50 or missing pointer as a filled circle.

## Supported domain, ownership and limits

- Pool span must contain exactly4000 signed occupancy values; cursor0..3999.
- XYZ/value are finite float32 with absolute value <=2^24. X additionally
  obeys the conservative minimum/quotient guard above; Y obeys its quotient
  guard. Z/value may still be tiny/subnormal, and signed zero is preserved.
  The derived bucket must lie in0..79; near-boundary rejections are explicit.
- Duration and mode are signed int32. All unsigned timer flag bits are
  preserved; initialized clock identities and handles remain opaque values.
- Birth preparation requires a genuinely free selected snapshot and detached
  nodes. All-occupied selection is proved but occupied reuse is rejected.
- Numeric proof covers the declared original x87/SSE environment and sampled
  values, not every FPU mode or a complete native gameplay-state domain.
- No raw input buffer, registry pointer, callback closure or borrowed span
  survives the pure call. Publication, resource lifetimes and generation
  checks remain explicit host obligations, not an internal hidden scheduler.

The probe caps each execution at100000 instructions, code8192 bytes/16 spans,
data524288 bytes/32 spans with each span <=65536, and case generation128.
The imported frozen engine enforces declared-memory access and stops before
unprovided calls. Its instruction counter is the hard execution budget;
no wall-clock emulator timeout is claimed. Full pool memory is explicitly
declared in bounded chunks, not mapped as unbounded game memory.

## Provenance and verification

Pinned original EXE SHA256:
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
The EXE is read-only and never launched; no E-drive file is written.
Evidence is bounded original-byte emulation, not native Windows CPU or a
Windows game-process trace. Before/after executable hashes match.

Actual Japanese raw asset is resolved through `local/catalog.json` -> the
current Japanese generation manifest parent -> `raw/ice.anm`:
`174b4741de8c71eaf3d7877202346c696ca11ba0e69368960d9d31992d9c3ec2`.
The native/WASM test loads that asset with the existing Registry and checks
that the required script33 exists; it does not claim native interpreter
equivalence or produce an approximate freeze effect.

Private report: `local/research/ice-birth-fixtures.json`, schema
`th128-original-byte-ice-birth-fixtures/1`, SHA256:
`52f467fb49b66479d7b4229e3e4cb3daec44527fcfc4be9c5dc841d34b1d4da0`.
Serialization is explicitly UTF-8/LF, not platform-dependent text newlines.
It records35 builders/prefixes, five argument prefixes, two required-call
prefixes, instruction traces, selected before/after object images, declared
memory spans with digests, whole-pool digests, code/constant hashes, generator
and frozen-engine identities. Mask/loader/registration evidence is explicitly
static; only declared executed chunks count as native execution evidence.

| Selected original bytes | SHA256 |
| --- | --- |
| builder `0x410590..0x41065e` | `e43b883ff6ec1f37c9fdb2518846b2b57c91291de00fc6abaea42085ec4378d9` |
| initializer `0x410480..0x4104f2` | `422cdbb6ec68ca5a04b98c7614bfac020e49c20ecac52ec2640beba96f5519fb` |
| list insertion `0x410660..0x4106de` | `e3970da441d85add64062440922400b0ac575413d1b20ee1576d2a08631a7ece` |

Missing Python module and missing C++ API tests were observed RED before
implementation. Additional RED checks caught absent executed effect-prefix
evidence and the layer/resource identity confusion, then became GREEN.

Focused verification on October29,2026: configured Python **5/5 passed**;
the review-fix suite expands that to configured Python **7/7 passed**.
Node **3/3 passed**, including all40 portable state/selection/activation
vectors in ordinary WASM and UBSan with exceptions disabled, plus the actual
ice asset check and native list/effect-prefix evidence assertions.

The two tiny-negative reviewer regressions were observed RED in Python;
the compiled CPP regression also demonstrated both wrong acceptances before
the guard. Added float32 predecessor/exact/successor boundaries are retained
as native returned or safely stopped evidence and CPP accepted/unsupported
vectors. Nonzero bit-sensitive activation XYZ vectors now compare forwarded
positions in birth and both required calls, retained nativeFrozenPower,
result-store offset0x50, manager-active flag0x6b218, and builder call0x4102c9
in addition to every prior call metadata field. These pure plans still do
not imply successful native effects or power consumption.

```powershell
$env:PYTHONPATH = '../../work/reverse-libs'
$env:TH128_BULLET_EXE = 'E:\gee\妖精大战争\th128.exe'
python -B -m unittest discover -s tests -p test_ice_birth_probe.py -v
python -B tools/research/ice_birth_probe.py --exe $env:TH128_BULLET_EXE
node --test --test-concurrency=1 tests/ice-birth.test.mjs
```

Build outputs and a dedicated Emscripten cache stay in
`work/ice-birth-test`, not shared build/site outputs or the shared compiler
cache. The frozen FreezeRules source/probe/fixtures and all main integration
files are untouched. No commit, branch, agent, game process or allocator/
Windows API return stub is created.

## Frozen source identities and assessment

- `IceBirthRules.hpp`: `4b2d6681d138757505ed1faa7643e57def6717477f22173a6de8e1ecd0e5048d`.
- `IceBirthRules.cpp`: `16193353d510e0b898e2c7f917ee31b4b63a4aa41e6a3f10145961ec70e68f16`.
- `ice_birth_probe.py`: `07242ec685a63f28a33c02122f5f1f70ab4e26f681ab9cc117e8a18d74ab5d71`.

The bounded component is ready for independent review and later host
integration. Required lists/effects are explicit work, not fake success.
Native occupied-record reuse, activation completion, per-frame ice update,
propagation, mask rasterization, damage, player-hit/cancellation pipelines,
full-frame ordering and full-port completion are deliberately not claimed.

## Completed inline implementation plan

- [x] Inspect original builder, initializer and activation call sites.
- [x] Observe missing Python module / C++ API RED.
- [x] Recover bounded original-byte prefixes and genuine returned builders.
- [x] Implement portable slot selection, state and mandatory call/list plans.
- [x] Compare original-byte vectors in normal and UBSan WASM.
- [x] Verify provenance and ownership contracts; freeze the component.
