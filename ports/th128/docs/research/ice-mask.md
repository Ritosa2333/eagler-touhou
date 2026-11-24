# TH128 bounded ice-mask raster and complete bit counter

## Implemented component

`src/game/IceMaskRules.hpp/.cpp` implements byte-span versions of actual
merge helper `0x4106e0`, subtraction helper `0x410820`, and the **complete**
bit counter `0x410960`. It also describes known mask selectors and the
mandatory real effect call after native rasterization.

There is no freeze-radius circle substitute, graphics lock stub, allocator
return stub, reward reimplementation or scheduler. The component does not
depend on IceBirth/FreezeRules sources or edit any main integration file.
The previous IceBirth precision review was resolved and re-frozen before
this work resumed; its reviewed identities are checked below.

## API and ownership

All names are in `th128::ice`:

```cpp
std::optional<uint32_t> countIceMask(std::span<const uint8_t>) noexcept;
std::optional<IceMaskUpdate> updateIceMask(
    std::span<uint8_t> mask, const IceBitmapView& source,
    const IceMaskPosition&, IceMaskOperation) noexcept;
IceMaskSelection selectIceMask(uint32_t selector) noexcept;
std::optional<IceMaskAttachmentPlan> planIceMaskAttachment(
    uint32_t selector, const IceMaskPosition&) noexcept;
```

`IceBitmapView` contains actual source bytes plus height and dwords-per-row.
`IceMaskOperation::Merge` is OR; `Subtract` is AND-NOT. A successful primitive
returns observed-placement-compatible row/word/remainder, actual dword-write
count and net changed-bit count. It does not claim successful animation,
selector attachment, object lifetime advancement or a completed game update.

The host owns every byte buffer. No source span, bitmap pointer, destination
pointer, object pointer or callback is retained. `countIceMask` reads exactly
21504 bytes. `updateIceMask` mutates that exact destination plane in place;
all shape, domain, operation and alias checks happen before any write.
Source and destination spans must not overlap. The source must remain
immutable and valid until return, and the host must hold exclusive access
to the destination during mutation. The four-byte stores are not an atomic
cross-thread publication contract.

The API never casts an original address into a host pointer. Native bitmap
identities appear only in private evidence/documentation. A production
asset owner must supply the actual copied bitmap bytes for a selected
profile; absent bytes are unsupported, not a zero/solid-circle fallback.
Generic rasterization also accepts other correctly bounded source bytes,
as the native helper is not restricted to a particular bitmap table.

The values in attachment plans have no internal pool/resource generation.
The host must apply them to the same live object and resource generation
from which its position/selector decision was obtained; plans must not be
applied to a recycled slot. Object/ANM handle lifetime management remains
outside these pure byte primitives.

## Exact mask layout and complete counter

The manager plane is `+0x65de4..+0x6b1e4` exclusive:

- 21504 bytes = 5376 little-endian dwords;
- 448 scan rows, each 12 dwords / 48 bytes;
- 384 bits per row, **172032 bits total**.

This is not the separate 80-entry spatial-bucket structure used by IceBirth.
Neither bucket membership nor an unproved object state4 controls the recovered
primitives. Caller state-machine eligibility remains an external obligation.

Native `0x410960` takes EAX=manager, runs exactly `0x700` groups, counts
three dwords per group using its actual SWAR instructions, increments its
input cursor12 bytes, and returns the total in EAX with a plain `ret`.
All eight complete counter cases genuinely return. Declared-memory checks
and unchanged readonly input digests cover its entire exact source span;
the trace visits `0x410980` exactly1792 times. It does not sample a prefix
and extrapolate a total. CPP uses `std::popcount` on explicitly decoded
little-endian words; all-one result is172032, not 0x700*32.

`countIceMask` returns an integer count only. The frozen area reward/charge
rules are untouched, and no percentage or reward formula is invented here.

## Native raster ABI and placement

Both helpers receive EAX=pointer to XY and four stack arguments:
manager pointer, readonly source bitmap pointer, signed height, signed
dwords-per-row. Both return with `ret0x10` (harness stack delta20).
Only X/Y are read; Z is not part of raster placement. CPP's position type
also carries Z solely for the later effect plan, where it must be finite.

With explicit source height H and dwords-per-row W, native placement is:

```text
leftInteger     = trunc(x + 384 - 16*W)
signedRemainder = signed_remainder(leftInteger, 32)
startWord       = trunc((x + 192 - 16*W) / 32)
startRow        = trunc(y - H/2)
```

Truncation is toward zero, not floor. X87 input arithmetic and the real
conversion helper `0x48fac0` are executed in evidence. That helper's actual
SSE conversion path is selected by explicitly declared original CPU-flag1;
its return is never replaced by a Python conversion or a fake API stub.

**The placement oracle is executed native state**, not these duplicated
formulas: a separate prefix stops at `0x41074a` / `0x41088a`, and reads
row from EAX, signed remainder from ESI and start-word from the real stack
local. The full function then runs independently to its real return. CPP
placement fields, write counts and every final byte are checked against
these independent observations.

## Shift and clipped-write behavior

For each source dword, the native helper tests destination row0..447 and
word0..11 independently for the primary write and the adjacent spill:

- Primary contribution is `source >> (uint32(signedRemainder) & 31)`.
- A spill exists only when signed remainder is **strictly positive**; its
  contribution is `source << (32 - signedRemainder)` to the next word.
- Each valid contribution is ORed for merge or ANDed with its complement
  for subtraction. No clamping moves an out-of-range write onto an edge.
- A primary write may be clipped while its right spill remains valid.
- Negative native remainders retain x86 low-five-bit shift semantics but
  do not emit the positive-remainder spill. They are not normalized into
  a neat nonnegative pixel-grid remainder.

Concrete original-byte cases include:

- source bit0 with remainder1: the primary writes0, spill writes bit31 of
  the next dword; two dword writes, one changed bit;
- left primary at word-1, remainder31: only its spill reaches word0;
- negative signed remainder-8: actual right-shift24, no spill;
- Y448/H1: start row447 is written; Y449/H1 is clipped;
- Y=-0.25/H1 truncates start row to0, whereas Y=-0.5/H1 gives row-1.

Source reads and writes use actual bitmap bits, not a radius or a newly
generated circle. Repeated OR and AND-NOT contributions are monotone, so
the summed changed-bit count equals the final before/after XOR popcount.
Removal is **not reference counting**: AND-NOT clears overlap bits even
if another merged source also covered them. The CPP unit check explicitly
preserves this native behavior rather than inventing a union scheduler.

## Selector mapping and immutable bitmap bytes

Actual attachment/removal callers establish:

| Selector | Original bitmap identity | Rows | Dwords/row | Required effect script |
| --- | --- | --- | --- | --- |
| 0 | absent | 0 | 0 | none |
| 1 | `0x4b1ef8` | 21 | 1 | 1 |
| 2 | `0x4b1f50` | 25 | 1 | 1 |
| 3 | `0x4b1fb8` | 33 | 1 | 1 |
| 4 | `0x4b2040` | 49 | 2 | 0 |
| 5 | `0x4b21c8` | 65 | 2 | 0 |

Every bitmap is read from the pinned original into an explicit readonly
emulator span of exactly H*W*4 bytes, and its code/data checksums are retained.
No private bitmap blob is embedded in production CPP. The host must own
and validate its actual imported bytes; selector integers are not pointers.

`selectIceMask(0)` reports `Absent`; selectors1..5 report `Known` and their
dimensions; every other value reports `Unsupported`, never silently selector0.
The removal prefix at `0x40f1b0` genuinely skips subtraction for selector0
and for sampled unknown6. That skipped slice is documented, but the port
does not promote an unknown object's complete effects/lifecycle to success.

## Post-raster effect and store obligations

Actual attachment prefixes execute the **complete OR helper**, then stop
before a real `0x407ec0` effect call. Call sites for selectors1..5 are
`0x40eda8`, `0x40edf8`, `0x40ee45`, `0x40ee8f`, `0x40eec2` respectively.
The captured ABI is layer23 in EAX, resource pointer at manager `+0x65ddc`
in ECX, and scratch-output/script/position stack arguments. `ice.anm` is
native resource slot36, distinct from layer23.

`planIceMaskAttachment` returns `NeedsMergeThenEffect`: merge the selected
actual bitmap first, then perform the genuine effect operation with the
specified script and unchanged XYZ. Only **after that real successful host
operation** may the caller write object `+0x54` with its opaque handle and
object `+0x58` with the selector. Native prefixes verify selector remains0
at the unresolved effect call; the plan itself does not store either field.
If the host cannot construct/bind/register that effect, it must remain
pending/unsupported rather than supply an invented zero handle or skip it.

Actual factory `0x407ec0` binds via `0x4593b0` then calls `0x466d60`:
**primary-tail** ANM registration, head`+0x8b9704`/tail`+0x8b9708`, priority30.
This is not secondary8; `0x466de0` is head insertion in that same primary
domain. These are static original-byte obligations and are tested, not
executed ANM callbacks or a new TH12-derived animation implementation.

Actual removal selector dispatch reaches `0x410820`, executes its complete
AND-NOT helper for known selectors, and stops at `0x40f1fe`. The selector
field remains unchanged in every sampled prefix. Thus subtraction does
not itself clear `+0x58`, delete an object, unregister an effect, complete
a timer transition or authorize the later caller continuation.

## Explicit supported numeric and memory domain

- Destination must be exactly21504 bytes; shorter and longer spans reject.
- H0..448, W0..12, H*W<=2048 dwords, with exactly H*W*4 source bytes.
  Empty dimensions genuinely perform no source loads/writes. The source
  budget intentionally excludes a full448*12 source, while the complete
  destination counter always covers all448*12 dwords.
- X/Y must be finite float32 with |value|<=2^24, including subnormals and
  signed zero. Tiny deltas retain the original x87 precision64 lattice
  until the native double conversion boundary; they are not clamped to
  zero. See `ice-mask-tiny-precision.md` for the placement rounding cases.
- Z is ignored by the raster primitive; an effect plan requires finite
  |Z|<=2^24 and forwards its exact float32 bits, including subnormals/-0.
- Source/destination overlap, unknown operation enums, nonfinite
  coordinates, wrong byte counts and excessive dimensions reject before
  destination mutation. No native-address-to-host-pointer cast is used.

Generic dimensions are a bounded helper subset, not new claims about
which gameplay object type may attach a mask. Eligibility, state4 or
80-bucket traversal, propagation, resource import and host publication
remain separately owned work.

## Provenance, budgets and cases

Pinned original EXE SHA256:
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
It is read-only and never launched. Evidence is bounded original-byte
Unicorn execution with synthetic explicit state, **not** native Windows
CPU execution or a Windows game-process run. No E-drive output is written.

Report `local/research/ice-mask-fixtures.json`, schema
`th128-original-byte-ice-mask-fixtures/1`, canonical UTF-8/LF SHA256:
`754e363dff1333172eb99edb612861063554755f2070c70859a60d9690f14438`.

The108 native cases comprise8 complete counters,88 complete raster helpers,
5 intentional unresolved-effect call prefixes and7 intentional removal
continuation prefixes: **96 returns,5 unsupported calls,7 boundaries**.
Placement measurement prefixes remain separately labeled incomplete.
The report retains whole before/after mask bytes, actual source bytes,
span digests, code/constants, generator/engine hashes, input register/stack
ABI, stop reasons and trace summaries. Trace summaries preserve total
length, SHA256 of the full executed address sequence, prefix/suffix and
per-address visit counts rather than embedding a million repeated lines.

| Original code span | SHA256 |
| --- | --- |
| OR `0x4106e0..0x410814` | `1f5e8ea853f1ec318e673cec0156b89a4ec7ddfde10f1d89af27eccb1a5a28e6` |
| AND-NOT `0x410820..0x410958` | `5f191c55dc4d32c21d5b0d76439ac48cf96dcd93797f9a55ead2958724bca798` |
| complete counter `0x410960..0x410a5c` | `479f2068ad2dd61cd778fc4c7fdac7efb310348ca21a592141ce6cb613e8f7b6` |

Code is capped8192 bytes/16 spans, explicit data131072 bytes/32 spans,
each data span<=65536, and report generation128 cases. The complete engine
has a200000-instruction hard cap; selected maximum counter executes123668,
maximum raster70477. Removal slices use the stricter existing bullet engine
with20000 instructions and its500000-microsecond timeout. Complete runs
have the frozen engine's instruction budget, **not** a claimed wall timeout.
Unknown calls are stopped before execution; only original conversion-helper
code is allowed inside raster helpers. No budget fault is accepted as data.

## Historical component verification

Missing module/API tests were observed RED before implementation. Additional
RED checks required explicit tiny-coordinate rejection and native observed
placement rather than a duplicated formula. Configured Python **5/5 pass**;
Node **3/3 pass**, including **65 CPP golden operations in normal WASM and
UBSan**, with exceptions disabled and warnings as errors.

Coverage includes all-zero/all-one, bit0/bit31, last bit/last row, row ends,
unaligned source spill, separate left/right/top/bottom clipping, signed
negative shift remainder, zero dimensions, maximum H and source budget,
immutable actual profiles, selector0/unknown6, invalid spans, overlapping
buffers and no-mutation rejection. Another64 randomized original-byte
rasters match every output byte plus placement/write/changed-bit metadata
in **each** ordinary and UBSan build. The entire report regenerates exactly.

The October31 gameplay follow-up supersedes the former tiny-coordinate
rejection tests: native108-case evidence and full WASM/UBSan raster output
now cover valid tiny positions. The old source hashes below are historical,
not the current build identities.

```powershell
$env:PYTHONPATH = '../../work/reverse-libs'
$env:TH128_BULLET_EXE = 'E:\gee\妖精大战争\th128.exe'
python -B -m unittest discover -s tests -p test_ice_mask_probe.py -v
python -B tools/research/ice_mask_probe.py --exe $env:TH128_BULLET_EXE
node --test --test-concurrency=1 tests/ice-mask.test.mjs
```

Builds, temporary files and a dedicated Emscripten cache stay under
`work/ice-mask-test`; no shared build/site/cache is written. Reviewed
IceBirth source/report identities, frozen FreezeRules/probe identities and
the original EXE digest remain unchanged during this resumed mask work.
No commit, branch, new agent, native game process or fake API return exists.

## Frozen source identities and assessment

- `IceMaskRules.hpp`: `09b091513a0096b01d93175cacac9009a12017d93a75dbd3e86a13a90b76bd55`.
- `IceMaskRules.cpp`: `ed09ed99aa2f57107c59e95c42eb1d3a3ae4064b70e6a5dba9e51502b0219f10`.
- `ice_mask_probe.py`: `2a81eb2f2e7032372fc536cee55b7b9e61607a7572a26586db3aee56e314879d`.

Ready for independent component review and later real-host integration.
This advances actual mask rasterization/counting, not full freeze activation,
propagation, animation implementation, rewards, player-hit/cancellation or a
complete full-port/game-frame claim.

## Completed inline implementation plan

- [x] Recover helper instructions, constants and actual callers read-only.
- [x] Observe missing module/API tests RED.
- [x] Execute bounded complete counters and rasters; retain caller stops.
- [x] Implement bounded CPP byte spans and explicit selector/effect plans.
- [x] Match native bitmap bytes and counts in WASM and UBSan.
- [x] Recheck provenance, frozen dependencies, domains and ownership; freeze.
