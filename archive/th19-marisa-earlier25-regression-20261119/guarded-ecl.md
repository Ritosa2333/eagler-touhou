# Conversion callback and guarded ECL visit

A registered wolf inside the Aura has already consumed its first arrival visit. The following visit remains guarded for displacement and HP, but executes both the Aura conversion callback and the normal ECL step. Native frame 13335 records those two ECL calls for the 424242 diagnostic actor.

The Web registration guard previously cleared its marker while leaving the arrival phase unchanged. That scheduled another held visit after conversion. Moving the phase to Hold removes the duplicate visit. The held branch must still run normal ECL after a callback advanced by the CPU prepass.

Without that normal ECL step, seeds 65537 and 536870911 keep first-round primary RNG and controls aligned but change secondary draw assignment. In seed 65537 the first additional draw group occurs at frame 27922; the round-private stream copied from secondary RNG then generates a different second-round wave table. This is a gameplay regression despite the initially visual source.

Combined WASM: `bfb038833c731897f8bdfae36a527ff2a2c917c97ac57c577eb1cf74316633f1`. It includes the guarded ECL correction and late movement of split straight-laser children.

All four critical seeds passed 36000 cumulative frames on this build: 424242 / 131071, 65537 / 104729, 536870911 / 1000000007, and 1000003 / 999983. Exact secondary RNG is separately recorded; acceptance checks full gameplay coverage. See `guarded-four-results.json` for the actual secondary seed values and detailed validation. A complete 35-seed same-build regression is still required before aggregate acceptance.

The 65537 comparison at cumulative frame 31351 (round two, frame 85) is in `screenshots/seed65537-before/frame-31351.png` and `screenshots/seed65537-after/frame-31351.png`. Web is left; native is right.
