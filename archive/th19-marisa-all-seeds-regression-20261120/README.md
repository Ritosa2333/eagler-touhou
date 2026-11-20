# All historical Marisa seeds: same-build regression

**55/55 PASS** on build 4e37d7915b5568db998ce32b5d009c9298df4b92715b6535a0c3191237b821fb. Every previously tested primary/secondary pair was rerun for 36000 cumulative frames across rounds, totaling 1980000 continuous frames. All 55 have zero coordinate tolerance and maximum observed coordinate difference zero.

The run compares primary RNG, CPU input, player position and complete enemy-bullet kind/color/coordinate multisets against retained full native traces. This is a fresh Web execution and zero-tolerance validation for every seed, including the latest five; no previous PASS is reused. Pure animation remains outside the established gameplay scope.

Native capture directories were audited as well as historical metadata. The audit added the earliest default pair 3058262 / 424182232. See inventory.json and capture-inventory-audit.json.

1979979 battle-state frames were compared. Round-transition intervals without a native battle-state sample are independently verified by the validator and listed in complete.json. No gameplay discrepancy was waived.

| Primary | Secondary | Frames | Result |
| --- | --- | --- | --- |
| 1 | 7 | 36000 | PASS |
| 7 | 7654321 | 36000 | PASS |
| 42 | 2026 | 36000 | PASS |
| 65537 | 104729 | 36000 | PASS |
| 424242 | 131071 | 36000 | PASS |
| 1000003 | 999983 | 36000 | PASS |
| 3058262 | 424182232 | 36000 | PASS |
| 4599481 | 966889228 | 36000 | PASS |
| 8675309 | 42424243 | 36000 | PASS |
| 15485863 | 49979687 | 36000 | PASS |
| 15867748 | 1282578646 | 36000 | PASS |
| 19840124 | 20010401 | 36000 | PASS |
| 20261112 | 424182232 | 36000 | PASS |
| 20261116 | 998244353 | 36000 | PASS |
| 20261118 | 18112026 | 36000 | PASS |
| 32452843 | 67867967 | 36000 | PASS |
| 123456789 | 987654321 | 36000 | PASS |
| 161803398 | 141421356 | 36000 | PASS |
| 179424673 | 433494437 | 36000 | PASS |
| 185362831 | 93017621 | 36000 | PASS |
| 213927170 | 1300782066 | 36000 | PASS |
| 300941098 | 383613775 | 36000 | PASS |
| 314159265 | 271828182 | 36000 | PASS |
| 357622458 | 506629354 | 36000 | PASS |
| 470311227 | 1736728499 | 36000 | PASS |
| 471872317 | 633158572 | 36000 | PASS |
| 536487702 | 1476276067 | 36000 | PASS |
| 536870911 | 1000000007 | 36000 | PASS |
| 554623781 | 287782238 | 36000 | PASS |
| 665982791 | 221820871 | 36000 | PASS |
| 778338950 | 471267855 | 36000 | PASS |
| 839207153 | 619412363 | 36000 | PASS |
| 935616930 | 1914315768 | 36000 | PASS |
| 982451653 | 961748941 | 36000 | PASS |
| 987450218 | 865127664 | 36000 | PASS |
| 987654321 | 1357911 | 36000 | PASS |
| 1073741823 | 524287 | 36000 | PASS |
| 1099740237 | 1764822807 | 36000 | PASS |
| 1193601596 | 1108880612 | 36000 | PASS |
| 1293760327 | 152317181 | 36000 | PASS |
| 1354966619 | 39499591 | 36000 | PASS |
| 1504457712 | 1485358101 | 36000 | PASS |
| 1641255564 | 487793392 | 36000 | PASS |
| 1689226635 | 1374292823 | 36000 | PASS |
| 1761660084 | 2082427414 | 36000 | PASS |
| 1778698539 | 1328397942 | 36000 | PASS |
| 1869345750 | 675054366 | 36000 | PASS |
| 1893703640 | 328577206 | 36000 | PASS |
| 1936728471 | 2071842433 | 36000 | PASS |
| 1947437859 | 1319879470 | 36000 | PASS |
| 1966888597 | 233691784 | 36000 | PASS |
| 2000000011 | 31415926 | 36000 | PASS |
| 2038074743 | 122949823 | 36000 | PASS |
| 2064620443 | 1958715403 | 36000 | PASS |
| 2147483646 | 1234567 | 36000 | PASS |

All 55 passed the same immutable build. No regression was found in the tested gameplay fields, so this batch required no gameplay fixes or new mismatch screenshots. Prior paired evidence remains in th19-marisa-zero-five-b3-20261120. No commit or push was performed.
