# Concrete next gameplay boundary

The complete-game request is still open. A successful resource parse, title
animation run or scalar proof does not complete a playable port.

Update: the raw control-VM diagnostic still stops at 402, while the new
`StageWorld` integration now handles the first flags and actual first birth,
retaining GirlA1b's raw extension parameters and reaching its first unresolved
emission opcode 501 at offset 816. See
`first-world.md` for exact observed state, evidence and limits. The original
402 paragraph below is retained as the earlier raw-VM boundary, not a claim
that the new world remains stuck at 402.

The independent VM currently loads all 33 actual ECL resources. All 16 stage
entry subroutines start with a local stack allocation (opcode 40), followed by
opcode **402 at offset 20**. This is the first explicit unregistered game-world
operation. The diagnostic WASM wrapper deliberately stops there and preserves
its structured fault across other resource loads.

What must follow before any first-stage acceptance:

1. Recover TH128 enemy/world state and game-global variable bindings. Assigning
   guessed globals or reusing TH12 player/score fields would invalidate native
   script behavior.
2. Link actual included ECL resources with verified override and call rules,
   including `default.ecl` and each route's boss file. The current Program type
   reports includes but does not silently resolve them.
3. Implement and verify actual enemy/game opcodes, animation bank mappings,
   movement, emitters, bullet profiles/ETEX, laser behavior, callbacks and RNG
   consumption. Host acceptance is not permission to no-op an unknown command.
4. Recover player/SHT processing, input semantics, hit/graze, shot damage and
   item/score interactions. The three built-in demos provide actual native
   input/seed/state records, not a simulated gameplay oracle by themselves.
5. Integrate freeze activation, collision/propagation, nonfreezable objects,
   ice lifetime, motivation/area scoring and Perfect Freeze lifecycle. The
   current scalar functions omit engine scheduling and callbacks on purpose.
6. Recover STD/background and camera rendering semantics; use actual stage
   table pointers rather than matching filename prefixes.
7. Run actual first-stage normal input paths, then full route and each
   route/difficulty/Extra/ending feature, with original comparison records.

There is no claim that skipping opcode 402 or importing TH12's dispatcher would
solve this. The native EXE/static/emulated evidence, inherited code and host
policy remain distinguishable in the implementation ledger. Current browser
entries label the project incomplete and never present a replacement stage.
