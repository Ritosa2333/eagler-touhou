# Entity ECL opcode recovery

`enemy_opcode_dispatch.cpp` now routes the real `48c010` entity entry through
the recovered 300–344, 400–448, 500–575, 600–633, 700–714, 800–802 and
1001–1003 handlers. 569 and unknown opcodes use the original zero-return default.
Production uses no native game instructions. Heavy endpoints call the actual
Bullet, Laser, mesh, background, score, special-state and script source modules.

`enemy_cpu_validation.json` currently records 1,254,114 passed checks, including:

* 20 animation/state mutations, 800 scenes each: 48,000 checks of the full
  Enemy, animation links, return value and all bounded ANM pool bytes.
* The other 25 animation/creation instructions, 512 scenes each: 38,400 checks.
  Original `48c010` and `496fb0` are unchanged. The isolated oracle records six
  external endpoints: named ANM allocation (`450cb0`), effect registration
  (`497d20`), effect spawning (`4974d0`), Enemy creation (`4a8920`), animation
  interruption (`44ee90`) and ANM execution (`42b5d0`). This validates exact
  boundary arguments and surrounding behavior, not those endpoint internals.
  Cases include all nine Enemy spawn variants, bounded names of 4–32 bytes,
  logical argument masks after variable-size strings, capacity/selection gates,
  parent assignment, script changes, target lookup and writable stack outputs.
* All 49 movement instructions, 640 scenes each: 94,080 checks of all Enemy
  bytes, every movement/interpolation record, return values and the shared RNG.
  The entire original `48c010` movement body is unchanged. Tests use four
  preallocated movement records, finite coordinates and angles, sentinel values,
  timer durations from −2 through 4, 34 interpolation modes, mirror flags,
  populated targets and rates from 0 through 2. Native and source paths use the
  same isolated RNG and player-position boundary; scalar RNG source is covered
  separately by runtime-state tests. Vector reallocation, invalid targets and
  malformed instructions are outside this particular comparison domain.
* State handler fixtures contribute another 217,088 checks over 53 opcodes;
  heavy Card/drop/defeat/resource endpoints are excluded from that scalar group.
* Drop/phase helpers add 49,408 comparisons, including actual original RNG and
  native PMR auxiliary-vector growth. The item-creation endpoint is recorded.
* 24 Bullet parameter instructions add 36,864 checks: complete queued values,
  metadata/ETEX words, capacity, shared-owner aliasing and copy-on-write, stack,
  controller and return values. Queue growth and initially null owners are used.
* All 15 Laser instructions add 23,040 checks: complete parameter bytes and
  command vectors for all four beam kinds, nullable target lookup, deletion of
  up to three equal identifiers, position/scalar writes and original virtual
  slot arguments. Beam creation, lookup and rectangle cancellation are recorded
  endpoints; three test virtual methods observe their argument/field boundary.
  Beam update, allocation, drawing and cancellation internals are outside this
  group. The comparison exposed opcode710's erase arguments `(0,0)`.
* All six misc instructions add 18,432 checks: actual identifier/selected-boss
  lookups, stack aliases, phase name selection and flags, exact clear/reset/
  select order, stone-attach arguments and return values. Script execution and
  special-state attachment internals are separate recorded boundaries here.
* `4a4190` adds 3,072 checks over 1,024 meshes with 2–17 rows/columns, both views,
  nonzero z, edge clamping, colors, phases, radius growth and rates 0–2. The
  complete native deformation body is unchanged; its pre-grid initialization
  and strip-copy helpers supply the same fixture boundary to both paths. All
  owner, vertex, position and final strip bytes compare. Source production calls
  the separately recovered render-mesh helpers; GPU rendering is not measured.
* `4a5640` adds 16,384 checks over 4,096 cases with zero through three recursive
  children, parent-link removal, real `std::function<void(Enemy*)>` callbacks,
  actual Player/feedback Timer updates, combo multipliers, original shared RNG,
  controller angle and return values. Sound, effect allocation/registration,
  drop emission, overlay reward and death-script execution are recorded endpoints.
  Native Player counter helpers are the shared dependency boundary; the source
  adapter calls the separately recovered Item/Bomb implementations. File slots
  2/3 provide valid resource fixtures. Death scripts can change bit27, testing
  the observed zero-return/revival path and callback suppression.

Still pending direct CPU coverage of the 600 adapter bodies: 601 shot creation,
613 full cancellation, 615/616/627/628 circle cancellation, 621 mesh replacement,
622 background event seek, 629 slowdown/reset callbacks and 630 score emission.
Their production paths are implemented; this report does not claim those
composition boundaries or all gameplay frames have been proven equivalent.

Observed quirks are retained: RGB interpolation starts in BGR order; 412/413
initialize speed interpolation without replacing its old mode, instead writing
the angular interpolation mode twice; 497470 consumes a raw LCG low 16-bit
value without modulus division; global/per-axis position setup retains its old
`current` vector until sampling. These checks establish their stated domains,
not whole-game frame identity.
