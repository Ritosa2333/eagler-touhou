# TH128 bounded player-shot resource and first-birth plans

## Implemented scope and actual entry points

`src/game/PlayerShotBirthRules.hpp/.cpp` adds an owning, bounded `ShtBank`,
actual group-selection helpers and first-shot state/required-call plans.
Only the eight new files in this scope are changed. Existing PlayerRules,
Python shot decoder, freeze/ice components and Main runtime are untouched.

The address relationship is important:

- `0x43f1b0` is the firing dispatcher, not the per-record constructor.
- Its call at `0x43f25d` reaches scheduler `0x43f160`.
- The due-record sequence begins at `0x43f190`; its actual call instruction
  at `0x43f199` targets birth routine **`0x43ed60`**.
- `0x43f760` is the later256-slot shot-update pass, not another birth entry.

The selected CPP component does not replace the frozen shot-cycle logic,
claim whole-emitter lifetime, or duplicate PlayerDeath integration. It does
not infer TH12 damage, option formations, focus/power group aliases, or
runtime callback behavior from PE initial bytes.

## API and missing native inputs

All types live in `th128::playershot`:

```cpp
bool ShtBank::load(std::span<const uint8_t>, std::string& error);
std::optional<ShotGroupPlan> planShotGroup(
    const ShtBank&, std::optional<float> actualCumulativeArea,
    std::optional<int32_t> actualDifficulty) noexcept;
std::optional<uint32_t> selectShotGroup(float cumulativeFrozenArea, int32_t difficulty) noexcept;
std::optional<bool> shotRecordDue(const ShtRecord&, int32_t shotCount) noexcept;
std::optional<FirstShotBirthPlan> planFirstShotBirth(
    const ShtBank&, uint32_t group, uint32_t record,
    std::span<const int32_t> occupancy, const ShotBirthInput&) noexcept;
std::optional<FirstShotBirthPlan> completeFirstShotMotion(
    const ShtBank&, const FirstShotBirthPlan&, const ShotMotionResult& genuineMotion) noexcept;
```

`planShotGroup` explicitly returns `NeedsRuntimeInputs` with no selected
group when either native value is missing. It identifies the required
cumulative-area global`0x4b4d60`, difficulty global`0x4b4d0c` and actual SHT
pointer field player`+0xf7c`. Missing state is **not** defaulted to group0.
With supplied actual values it returns `Ready`, the selected real resource
group offset and the table's unclassified `unknownGroupWord`.

The lower-level `selectShotGroup` is the proved numerical helper, not a
source of startup state. The host must read/wire actual native-equivalent
cumulative-area/difficulty state, validate the loaded SHT resource, respect
the dispatcher's player`+0xf78 == 1` gate and current-versus-previous timer
gate, then schedule actual records. A plan's ability to accept values does
not prove that they describe the current game frame.

## Real resource, owning reader and lifetimes

Current Japanese generation is resolved through `local/catalog.json` ->
Japanese manifest parent -> `raw/pl00a.sht`. The actual name is `pl00a.sht`,
not an invented `pl00.sht`. Pinned resource:

- 6736 bytes, tag3, **16 groups and114 records**;
- SHT SHA256 `aed90a45b565510e97cc7d9ab4846d8c93914e57faafe9339e9ab03d47b1511b`;
- accompanying actual `raw/pl00.anm` SHA256
  `ba336476a5171d811b6aef0ef4e713c1ad826e723e5de895dcf8e1297374f976`.

`ShtBank` requires real input bytes; it has no default shot recipe or embedded
private payload. It copies the whole file and each raw record into owned
storage. Tag3, uint16 group count at+2, group table at0x268/stride8, file-
relative record pointers and signed-negative one-byte terminators match
the actual loader evidence. A group terminator does not need52 bytes.

The reader validates every region before reading:4MiB maximum file,
1..256 groups, at most65536 logical records, table/record extents bounded
by the next distinct group offset, complete52-byte records and period>0.
Aliased groups remain safely readable copies of immutable byte values;
logical duplicates count toward the bounded record budget. Table second
dwords, unused header/tail bytes, and all record bytes are retained without
giving them guessed gameplay meanings.

A successful reload changes a weak generation token and rejects all old
pending motion plans. A failed reload leaves the current bank and its plans
usable. A foreign/destroyed owner's token cannot authorize continuation.
The bank is neither copyable nor movable. Const `group()`/`bytes()` views
are only valid until successful reload/destruction; plans copy their record
bytes and never retain caller buffers or native pointers.

The host must pair an actual slot snapshot with the same player/pool/resource
generation and not reuse a plan after recycling a shot slot. `owns()` guards
bank generations, not arbitrary edits to a public plan or live pool mutations;
plans and genuine-motion inputs are trusted host values, not authentication
tokens. The API is not thread-safe or an atomic shot-pool scheduler.

## Actual group selection: cumulative area, not power/focus guesses

Scheduler `0x43f160` sets ESI=`0x4b4cc0`, calls `0x4271c0`, loads the actual
SHT pointer from player`+0xf7c`, and indexes group table`+0x268` by EAX*8.
The helper reads float ESI`+0xa0` = **cumulative frozen area`0x4b4d60`**.
It selects one of two actual15-threshold tables based on difficulty global
`0x4b4d0c == 0` versus nonzero:

```text
difficulty0: 0,0,50,100,150,200,300,450,600,800,1000,1200,1400,1600,2000
otherwise:   50,100,200,350,500,700,900,1100,1300,1500,1700,2000,2300,2600,3000
```

Return is the first index whose threshold is **strictly greater** than the
supplied area; if none matches, return15. At supplied area0, difficulty0
returns **group2**, difficulties1..3 return **group0**. These are numerical
facts, not a claim that a real game session has already initialized area0.
The helper neither reads a focus key nor current frozen-power`0x4b4d6c`.
No16-group-to-focus/power-level arrangement is inferred.

36 complete original helper cases cover threshold neighbors and all four
supported difficulty values. Fourteen scheduler runs execute the real
group helper and signed period/phase `idiv` comparison, then genuinely return
when no record is due or stop before actual birth`0x43ed60`. The CPP due
helper is restricted to shot-count0..14 and positive signed-byte periods;
it does not duplicate or advance the frozen shot-cycle timer.

## Record fields and measured initial object

Record stride is0x34/52 bytes. The actual114 records all have owner0/kind0
and all four callback file IDs0. These observed fields are not a runtime
callback-table snapshot.

| Record offset | Proved use in selected original birth |
| --- | --- |
| `+0/+1` | signed period/phase used by record scheduler |
| `+2` | signed int16 copied/sign-extended into object damage`+0x60` |
| `+4/+8` | float local X/Y offsets, applied after motion |
| `+0xc/+0x10` | float size operands copied to object`+0x68/+0x6c` |
| `+0x14` | float angle, including unsupported special-angle branches outside subset |
| `+0x18` | float speed copied to object`+0x2c` |
| `+0x1c` | signed owner/source-position selector;0 uses supplied XYZ |
| `+0x1d` | kind byte;2 has extra gate/ANM behavior, not ported here |
| `+0x1e` | signed int16 ANM script base; required bind script = base+5 |
| `+0x20` | raw unknown uint16 retained in record bytes; not invented as formation/damage |
| `+0x22` | signed sound ID; negative means skip, **0 remains an actual sound request** |
| `+0x24/+0x28/+0x2c/+0x30` | four raw callback IDs, requiring distinct live-table resolution |

First group0 record has damage13, local offset(-10,-8), size(18,48),
angle float bits`0xbfc90fdb` (-1.5707963705062866), speed24, owner0/kind0,
script base0 -> required script5, unknown+0x20 value1, sound0, callback IDs
`[0,0,0,0]`. Other real records include damage3..20, script bases0/2/4,
and several real angles; all114 raw records and both numerical birth stages
are checked, not replaced by a TH12 recipe.

Birth takes ECX=source XYZ pointer; stack(player pointer, record pointer,
shot-count). It scans exactly256 records starting player`+0xfa8`, stride
`0x78`, testing occupancy dword`+0x48` for zero, in ascending order. There is
no cursor/wraparound search. A full pool really returns EAX0 with `ret0xc`
without constructing a shot; CPP reports `PoolFull` with slot256/examined256.
Do not treat that returned EAX0 as a completed nonempty shot birth.

For a selected slot the prefix sets occupancy1 and source record identity,
resets timer previous-1/current0/float0, initializes rate-source`0x4b6f58`
only if timer flag0 was absent, retains other timer flags/initialized rate
identity, stores damage/size/speed and copies base XYZ for owner0. It stops
before the first genuine normalize call at`0x43ef72 -> 0x46a700`.

`ShotBirthInput.zeroUnmodeledStateConfirmed` defaults **false**. This bounded
first-use port requires explicit host confirmation that unmodeled selected
slot bytes are zero, matching the original measured first-birth snapshots.
It does not silently zero a recycled shot's unknown fields. Slot timer
flags/rate identity are supplied separately. Non-pristine reuse or option/
kind2 origins require a future implementation or an unsupported stop.

`slotImage` is a portable52/120-byte semantic payload carrier, not live native
memory: at object`+0x74` it stores **source file offset**, not a host pointer
and not an original native address. Native golden comparison converts only
that source identity from observed SHOT+offset to file offset. No pointer
cast is used to access host memory.

## Motion continuation and precision contract

`NeedsMotion` requires the actual two normalizations through`0x46a700`, then
the vector helper`0x46ae00` with explicit native clock identity`0x4b6f58`
and clock1. Its selected branch calls `0x46c1f0`, whose **original FSINCOS**
multiplies by speed*clock and stores float32 velocity. The prefix does not
substitute libm output or a zero vector. Original shot angle/speed constants
are exposed so a host can use an independently proved compatible engine.

`completeFirstShotMotion` accepts a supplied **genuine** normalized angle
and velocity. It cannot authenticate external execution. Calling it with
guessed/default values is a host contract violation; a missing motion
implementation must keep the plan at `NeedsMotion`, not return a fake result.
The completed stage uses the supplied result exactly once and computes:

```text
birthX = float32(record.offsetX - velocityX + baseX)
birthY = float32(record.offsetY - velocityY + baseY)
birthZ = baseZ
```

This negative-velocity backshift is native birth behavior, not a guessed
formation offset. The native helper stores velocity first; source offsets
are then combined in x87 before one final position store.

To avoid the IceBirth intermediate-double trap, the selected post-motion
domain requires each nonzero X/Y term to have magnitude in[2^-24,4096].
Each float32 term is represented as an exact integer multiple of2^-48;
three terms fit signed int64 and the native x87 exact intermediate precision
in this subset. CPP sums exact integers and rounds once to float32, then
rescales by a power of two; zero uses an explicit signed-zero-preserving
path. Velocity Z must be positive zero, matching the actual helper. Outside
this declared domain, return unsupported rather than clamp or claim generic
x87/host-double/trigonometric equivalence. Other used floats are finite with
absolute value<=2^20, angle magnitude<=4. Clock1/owner0/kind0 are explicit
limits; special angle branches involving RNG/option state remain unsupported.

All114 native motion cases execute the actual two normalizer calls and
FSINCOS/vector code, then stop **before allocator`0x468010`**. Their actual
velocity/normalized-angle outputs feed the CPP continuation golden test.
This validates post-motion composition, not a CPP trig implementation.

## Required animation, callbacks and sound

The resulting `NeedsAnimationAllocation` plan still requires genuine:

1. Debug/lock context handling for global`0x4d33f8` bit`0x8000`. Evidence
   declares that bit clear; no Windows lock API is supplied. If the host's
   actual context takes that branch, it must provide real handling or stop.
2. Increment resource`[player+0x10]+0x130` before allocation, then execute
   allocator call`0x43efd4 -> 0x468010`. No return handle is fabricated.
3. Use layer23 and required record-base+5 script with the actual ANM resource
   at player`+0x10`; bind`0x43f011 -> 0x4593b0`.
4. Register via`0x43f01c -> 0x466d60`: **primary-tail**, ANM head`+0x8b9704`,
   tail`+0x8b9708`, priority30; not secondary8. No resource-slot number is
   guessed from the factory's layer23.
5. Resolve the four actual runtime callback tables below. The birth slot0
   callback, when genuinely nonnull, is reached at`0x43f0f6` after bind and
   receives the actual shot-count. Native per-frame update`0x43f760` reads
   slot1/record+0x28 before motion. Slots2/3 retain raw unresolved semantics.
6. If sound ID>=0, perform actual positioned sound`0x43f10b -> 0x4584a0`.
   Sound0 is **not** a no-op sentinel. Later native ANM position/callback/
   child work is outside this prefix; completing these requests alone must
   not be advertised as a fully returned native emitter.

| Record callback offset | Required native table identity |
| --- | --- |
| `+0x24` | `0x4b1cd8` |
| `+0x28` | `0x4b1a14` |
| `+0x2c` | `0x4d2e2c` |
| `+0x30` | `0x4b1cf8` |

The reader retains file IDs as uint32, even an unknown0xffffffff ID; the
planner never dereferences tableIdentity+ID*4. Every real record's four0
IDs still produce four **required live-resolution requests**. PE first words
and declared BSS zero-fill at`0x4d2e2c` are saved as static structural evidence
only, not used to skip calls or impersonate a runtime callback snapshot.
The host must prove real absence or obtain an actual supported callback;
unknown behavior remains unsupported, never a synthesized zero return.

## Native evidence, provenance and budgets

Pinned original EXE SHA256:
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
It is only read, never launched, and never written. Evidence is original-byte
bounded emulation with explicit synthetic memory, not native Windows CPU or
a full game process/frame. No allocator, callback or Windows API is stubbed.

Report: `local/research/player-shot-birth-fixtures.json`, canonical UTF-8/LF,
schema`th128-original-byte-player-shot-birth-fixtures/1`, SHA256
`0007f7a112693248514cea7e244384c39f50e99fb29f23358354ee6e33d12439`.

It contains **283 native cases**:36 complete group helpers,14 scheduler runs,
119 pre-motion birth/full-pool cases and114 motion-to-allocator prefixes.
Across these:45 genuine returns and238 intentional unsupported-call stops.
Full-pool return means no shot was created; the other returned functions do
not imply a returned emitter. Raw source records, callback IDs, group unknown
words, actual slot before/after images, register/stack ABI, code/constants,
span digests, instruction traces and generator/frozen-engine hashes remain
distinguishable. `pl00a.sht` table pointers are relocated only in the explicit
scheduler copy; callback file IDs are not invented live pointers.

| Original span | SHA256 |
| --- | --- |
| birth`0x43ed60..0x43f15f` | `e4fb6b1df4f185f6d5b931c01e6b768d464d14b21f04d30a528187aa4e3f476f` |
| group helper`0x4271c0..0x427257` | `b069333892f50eff6657fa8a2b48dd9c23ee197c75c679281029283ab80a35ac` |
| scheduler`0x43f160..0x43f1ac` | `b4e74e482fd6963da763cfb2fd39ecfc546a200d8494d715f44e7ec21dc71eb9` |

Probe caps:code8192 bytes/16 spans, data131072 bytes/32 spans, per-span65536,
20000 instructions/execution and512 cases. Only explicit allowlisted original
helper code runs; unknown calls stop first. The frozen engine has a hard
instruction budget, not a claimed wall-clock timeout. No fault/budget stop
is accepted as a successful record. Counter/span counts are bounded before
execution, original before/after hashes match, and output paths are restricted
to this component's private reports before EXE access.

## TDD, verification and freeze

Missing Python module and CPP implementation/API were observed RED before
production implementation. A missing-context group-plan API regression was
also observed RED, then GREEN. Configured Python **3/3 passed**; Node **3/3
passed**, including **397 golden rows in both ordinary WASM and UBSan**,
with exceptions disabled and warnings as errors. The rows cover every raw
record/group unknown word, all native group/scheduler cases, first/last/free/
full-pool scans, timer flags, nonzero XYZ and both state-prefix stages.

The native harness also validates invalid size/tag/count/regions/period,
owning copies, finite-use checks, unknown callback retention, missing real
group values, failed/successful reload, stale/destroyed/foreign resource
generations and unproved non-pristine-slot rejection. All animation/motion/
callback plan metadata is compared, including script/layer/resource-offset,
allocation/bind/registration sites/targets, pre-allocation counter/debug-lock
obligations and original clock identity/rate.

```powershell
$env:PYTHONPATH = '../../work/reverse-libs'
$env:TH128_BULLET_EXE = 'E:\gee\妖精大战争\th128.exe'
python -B -m unittest discover -s tests -p test_player_shot_birth_probe.py -v
python -B tools/research/player_shot_birth_probe.py --exe $env:TH128_BULLET_EXE
node --test --test-concurrency=1 tests/player-shot-birth.test.mjs
```

Generated binaries/cache/temp live under`work/player-shot-birth-test`;
no shared runtime/source/site integration is performed. No commit, branch,
additional agent, original game process or fake native call result exists.

Frozen source identities:

- `PlayerShotBirthRules.hpp`: `15a053963acab5d7acaece8b09a681e3a9c705bc8565d84473e33f15ab3fb393`.
- `PlayerShotBirthRules.cpp`: `0bebcad50bf4e510910ca6142c3722af9e3fd3362b747b42e6462cd62dd3e04b`.
- `player_shot_birth_probe.py`: `1230ba881496b177550cf3db99975abb50bba2f1121830f2c2fb59ccf25a976d`.

Ready for fresh component review and a genuine host integration, not a claim
that shots already hit enemies or that unknown runtime callbacks, option
positions, special kinds, per-frame lifetime/collision, player death or the
complete TH128 port are implemented.

## Completed inline implementation plan

- [x] Recover actual callers, file record shape and object pool ABI.
- [x] Observe missing module/API RED before production changes.
- [x] Recover complete group helper and original birth/motion/allocator stops.
- [x] Implement safe owning bank and mandatory first-birth continuations.
- [x] Compare native states/group/record bytes in WASM and UBSan.
- [x] Verify budgets, reload lifetimes, provenance and freeze the new scope.

## Main-Thread Recycled Birth Extension

`planRecycledShotBirth` consumes an exact inactive120-byte slot image, preserves
native-unwritten fields and applies the actual timer reset/position/size/damage/
record stores. Initialized clock identity and flag bits survive as native;
current/previous/time reset before motion. The motion object's angular bit at
slot+0x44 changes the native branch and therefore explicitly rejects bit0 set.
This was found by a dirty-slot original-byte vector, not cleared as a shortcut.

Six new dirty-slot cases cover timer flag0/1/0x80000001 before and after real
motion. Report SHA256 is now
`407566c2bd453af4e5747abb921f3d2d3ebafe4ae0267b6b84f18ff2b4fe8511`;
the older frozen identities above describe the historical fresh-only version.
Ordinary/UBSan vectors and actual-host emit/move/retire/re-emit checks pass.
The host does not recycle while its old primary animation is still alive.
