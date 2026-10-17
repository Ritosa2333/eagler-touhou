The shared Sprite/ANM CPU oracle additionally checks the Bomb core against the
unmodified original executable. `pool_cpu_cases.inc` supplies 31,168 checks:

- 512 complete-object cases each for 4779b0 Controller and 478430 Bomb construction.
- 64 cases of 477dc0 initialization, original node allocation and registration,
  and 477ae0 destruction with no active Bomb. Node addresses, callback code
  addresses and C++ vptrs are canonicalized; all other object and scheduler bytes
  are compared. Priorities, flags, userdata, links and callback identities are
  included. Source uses its actual scheduler implementation.
- 4,096 cases of 4780a0, comparing the predicate and complete Player bytes,
  including its mutating bomb-count clamp and HUD/Enemy presence conditions.
- 2,048 cases of 477c20, comparing the complete Controller, observed Bomb,
  eight complete 0x428-byte enemy records, controller/list storage, clock states,
  view selection, return value and callback order. Virtual Bomb methods are
  explicit C++ observation fixtures; these checks establish the surrounding
  controller behavior, not the character-specific Bomb implementation.
- 512 cases of 477a60 with real registered animation handles, comparing complete
  Bomb bytes, the Context0 owner reset and the affected Sprite pool/list state.

The aggregate report is `../sprite_renderer/pool_cpu_validation.json`. Its
`bomb_*` groups identify these checks independently of other modules, and its
source hashes bind this fixture and the tested production files. Original
machine code is restricted to the test executable. No instruction is patched.
`trigger`, `Bomb::start`, character subclasses and concurrent lifecycle behavior
are outside this batch. Character-specific tests are described separately in
`character_validation.md`. Function487bb0 is now the real secondary-owner state
notification; the earlier provisional projectile-clear declaration was removed.
