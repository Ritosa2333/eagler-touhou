# Marisa historical seed regression

All 35 historical Marisa CPU-vs-CPU seeds passed 36000 cumulative frames across rounds on one build. This covers the earlier 25 seeds and the previous two five-seed batches.

WASM SHA-256: `ed2ddabb8acaad7fe2a6f84a70d3539b899b53136c38f20b148120d2b1fc63d2`.

Strict gameplay validation passed for 34/35 seeds. The remaining 1 seeds satisfy the existing microscopic-coordinate policy (less than 1/4096 pixel) with primary RNG, CPU controls, player positions and bullet counts aligned. Their seeds are `[1641255564]`. Maximum bullet coordinate difference across the batch is 0.0001220703125 pixels.

The run covers 1260000 cumulative simulation frames and 1259984 comparable bullet-state frames. Verified native round-transition gaps are listed in each validation record.

The baseline mismatches were an extra held wolf-arrival visit and a skipped late-manager movement for bomb-split straight-laser children. Player-contact cuts retain their next-visit movement guard. The wolf conversion visit retains both its callback and normal ECL update, preserving the secondary draw sequence used to seed subsequent rounds.

- [Aggregate result](final35-complete.json)
- [Per-seed results](final35-results.json)
- [Capture identities and coverage](final35-status.json)
- [Raw strict and accepted validations](final-validation/)
- [Guarded ECL behavior](guarded-ecl.md)
- [Straight-laser child behavior](laser-child.md)
- [Player-contact registration boundary](contact-split.md)
- [Source delta from the accepted baseline](guarded-ecl-laser.patch)

Paired before/after images use Web on the left and native on the right. They annotate cumulative frame, round-local frame and cumulative gameplay mismatch count.

- 424242: [before](screenshots/seed424242-before/frame-13344.png), [after](screenshots/seed424242-after/frame-13344.png)
- 65537: [before](screenshots/seed65537-before/frame-31351.png), [after](screenshots/seed65537-after/frame-31351.png)
- 1000003: [before](screenshots/seed1000003-before/frame-35570.png), [after](screenshots/seed1000003-after/frame-35570.png)

- 4599481: [before](screenshots/seed4599481-before/frame-26391.png), [after](screenshots/seed4599481-after/frame-26391.png)

Initial and rejected candidate results remain in this archive for comparison. They are not the final aggregate result.
