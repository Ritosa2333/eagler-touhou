# Marisa CPU parity: five fresh seeds, batch 5

Each seed covers 36000 cumulative frames across rounds. Acceptance requires exact primary RNG, CPU inputs, player positions, and complete enemy bullet kind/color/coordinate multisets. Coordinate tolerance is zero; animation-only differences are recorded separately.

## Mechanism alignment

The unlimited Marisa option beam reads the charge value from before the current charge dispatch when deciding whether to retire its occupied emitter slot (native 0x54d8d8). A released slot permits the normal SHT emitter to construct its next pair of beam animations.

Seed 139269535 first exposed two missing secondary RNG calls at opening-round frame 17737. The remaining gameplay trajectory matched through the first round, but the missing calls changed the round-two wave table. The primary stream first diverged at cumulative frame 21493, round 2 frame 85. Reading the pre-dispatch charge restores slot retirement and the original next-round wave selection.

The batch is rerun on one immutable runtime. Screenshots show Web on the left and the original on the right, with cumulative and round frame numbers.

## Completed verification

Build: d6f73d06e44871021957673c7bd3e2f888b58766b5b8c0f6be28d29d43f78e90

| Primary seed | Secondary seed | Cumulative frames | Result |
| --- | --- | ---: | --- |
| 2068616002 | 852982080 | 36000 | PASS |
| 164848027 | 1933381659 | 36000 | PASS |
| 1080846151 | 961363864 | 36000 | PASS |
| 139269535 | 1381408274 | 36000 | PASS |
| 1670654752 | 1232417122 | 36000 | PASS |

Total: 180000 cumulative frames; 179997 compared battle states; 3 independently verified transition gaps. All gameplay differences are zero.

[Before frame 21493](screenshots/before-139269535/frame-21493.png) and [after frame 21493](screenshots/after-139269535/frame-21493.png) preserve paired original screenshots.

## Previous-batch regression

The preceding five seeds also pass all 36000 cumulative frames on this same build. The combined fresh-batch and previous-batch verification covers 360000 cumulative frames across ten seeds. [Previous-batch results](previous-five-regression.json).
