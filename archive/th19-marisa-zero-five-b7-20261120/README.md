# Marisa CPU parity: batch 7, five accepted fresh seeds

Five fresh seeds and five preceding regression seeds each pass 36000 cumulative frames across rounds on one build. Primary RNG, CPU controls and player positions match throughout. Enemy bullet coordinates use zero tolerance; isolated minor retirement differences are recorded separately under the user policy.

Build: 599d6f3deba6968e551b1c21c6ad75924b7763f8c738c8c905f4ac959bce7104

| Primary seed | Secondary seed | Frames | Result |
| --- | --- | ---: | --- |
| 1393378999 | 1089741564 | 36000 | PASS |
| 706492591 | 471420670 | 36000 | PASS |
| 360218520 | 151786253 | 36000 | PASS |
| 1234240984 | 538287654 | 36000 | PASS |
| 276342453 | 186181755 | 36000 | PASS |

The shared clear path repeats both native birth-position grid operations. Native frame 21942 uses circle x=132.92; one floor yields x=132.93 and cancels an edge bullet early, affecting the next round seed. Fixed beams retain their registered origin without subtracting and re-adding their offset before flooring.

The user permits animation-only and tiny differences without later gameplay effects. Accepted minor bullet retirement differences last one isolated frame, involve at most two bullets, converge on adjacent frames, and leave primary RNG, CPU controls and player positions identical through frame 36000. Exact coordinates and strict validations accompany accepted validations. Later wave differences fail acceptance.

Screenshots/ contains Web-left/native-right before/after frames 21942 and 27682, and the separately accepted offscreen difference at 3532. fresh-five/ and previous-five/ contain native trace hashes and validations. Ten seeds cover 360000 cumulative frames on this build.
