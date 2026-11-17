# Story difficulty ranks and replay audit

## Story ranks

Build `bbcf3ff813a20830a5bd5292468db8632c2ca7b1fa90aa070faefd8475f9741e`.
Original EXE SHA256 `8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9`.

`0x517a45..0x517a73` selects `0x5c5f90 + stage*0x80 + difficulty*0x20`.
The stage getter `0x40eb70` reads PlayerStat+0x9c0; difficulty getter `0x40e4d0` reads PlayerStat+0x190.
`0x517f0a..0x517f59` initializes player zero to zero and the opponent from table offsets 0x18/0x1c.
All six stage rows contain zero-based EX/Boss ranks E=0/0, N=0/0, H=4/4, L=4/4.
These are displayed as levels 1,1,5,5 for the opponent. VS remains 1,1,3,5 for both players.

The shared normal/multiplayer core now applies this Story opponent initialization.
`story-rank-native.json` contains the extracted six-stage table; `story-rank-browser.json` records actual menu entry into all four difficulties in Story and local VS.
This verification covers initial battle ranks; it does not establish every later attack-rank transition.

## Replay evidence and remaining work

`0x551130` writes replay/%s: uncompressed main header 0x140, stage header 0x2558, six-byte input rows, and a separate slowdown stream. It appends three 0xffff words as the end marker, compresses via 0x55d2d0, then encrypts via 0x4d5750 with parameter pairs (0x7d,0x3a,0x100) and (0x5c,0xe1,0x400).
`0x5389cb` calls this writer from the replay-name menu. The presence of this writer does not by itself prove recording is populated during network matches.
The matching loader 0x550250 accepts magic t18r and version 6. Playback 0x550750 restores one input context: 0x6240dc/0x6240e8/0x6240ec.
The live-input callback 0x550130 instead updates four local contexts and two network contexts (NetworkInf+0x98 and +0x358; derived held-state destinations +0x334 and +0x5f4). Both use edge/repeat helper 0x430340.
`0x54fd10` resets input and restores stage state for playback; its normal-mode branch does not initialize a recording stage.

A full native network recording path and spectator lifecycle have not yet been established. The existing web recorder/spectator implementation has not been replaced in this build. Its prior tests must not be described as native replay compatibility tests. The network-only save policy remains active.

Disassembly/decompilation evidence accompanies this note. Original binaries remain outside the repository.
