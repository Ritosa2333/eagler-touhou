# Marisa CPU parity: batch 8, five accepted fresh seeds

Five fresh seeds and the preceding five regression seeds each pass 36000 cumulative frames across rounds on one build. Primary RNG, CPU inputs and player coordinates match throughout. Enemy bullet coordinates use zero tolerance; any accepted minor retirement differences remain separately recorded with strict results.

Build: d38eda254e4188948c359e123ff97b6d4ec4e9991b85a07dee7b2f533ba2f7ca

| Primary seed | Secondary seed | Frames | Result |
| --- | --- | ---: | --- |
| 1394986086 | 1651715193 | 36000 | PASS |
| 1664487600 | 540468199 | 36000 | PASS |
| 1838652999 | 1996470247 | 36000 | PASS |
| 995047060 | 1107662846 | 36000 | PASS |
| 1130832693 | 285767531 | 36000 | PASS |

At cumulative frame 14811, native ECL525 visits the same 17 enemies twice before their next list update. The second clear repeats death effects and charge drops. The shared clear path retains those hosts for another clear during that update and expires their eligibility on the next enemy-list frame. The regression test also verifies that a later-frame clear cannot revive them and that checkpoints preserve retirement state.

The user permits animation-only and tiny differences without later gameplay effects. Any accepted bullet retirement differences last one isolated frame, involve at most two bullets, converge on adjacent frames, and leave primary RNG, CPU inputs and player coordinates identical through 36000. Exact coordinates and strict validations accompany accepted results. Later wave or damage differences fail acceptance.

Screenshots/ preserves Web-left/native-right before and after frames 14810 and 14811. fresh-five/ and previous-five/ preserve native trace hashes and validations. Ten seeds cover 360000 cumulative frames on this build. The test build and results are local; no commit or push was made.
