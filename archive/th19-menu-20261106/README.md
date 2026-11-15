# TH19 menu navigation parity

Original EXE SHA256: `8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9`.

Difficulty controller 0x5596a0 sets TitleInf+0x34 to 4 and +0xfc to zero. VS menu 0x5593b0 sets the same fields to 5 and zero. Shared cursor helper 0x4d4fa0 clamps to [0,count-1] when cursor+0xd0 is zero; otherwise it wraps. Cursor begins at TitleInf+0x2c. Both controllers compare the old/new index before issuing selection sound 10 and animation interrupts 3/selection+7. The browser now shares clamping and only sends feedback after the final selection changes.

Title dispatcher 0x55ac60 uses state 0xb and 0x556340 for character selection. Difficulty confirmation calls 0x4d5090 to push cursor index/count. Character cancel state 5 waits until timer >5, returns to difficulty state 8 and calls 0x4d5040 to restore the saved index/count. Story now uses the same six-tick return path as VS, preserving the selected difficulty and retiring the character animations.

The browser regression checks Easy/Up, Lunatic/Down, VS first/Up and fifth/Down, horizontal equivalents, and all four difficulty return values for both Story and VS. Return timing is measured in update ticks. These comparisons cover the listed menu branches; they do not establish whole-game EXE equivalence.

Evidence: [difficulty](th19-menu-native-20261106.c), [VS/cursor/character functions](th19-menu-helpers-20261106.c), [title dispatch](th19-title-dispatch-20261106.c).

Validated WASM: `ed3497e7fdc47ad12199b2a3c9459a010e2d04a2fbda47b957f4bdf4a56a4da5`. [Browser results](browser-report.json): all eight returns take six updates; both difficulty boundaries and both VS menu boundaries pass.
