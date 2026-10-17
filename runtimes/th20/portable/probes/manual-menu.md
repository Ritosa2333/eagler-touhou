# Manual texture overflow regression (2026-09-30)

Run `node portable/run-th20-manual-probe.mjs` from the TH20 repository.
The runner temporarily adds a read-only HelpInf state probe, builds the runtime,
serves it on loopback port 8100, then restores/rebuilds the production host.
Requirements match moon-cancellation.md: local SDK, sibling Puppeteer, configured
Chromium, Windows font and the DAT in build-eagler-smoke/game-data/th20.dat.

All nine help_01.png through help_09.png assets are 1024x1024. The help.anm
entry1 dynamic texture is 768x1024, with a visible 768x896 sprite. The old loader
used the PNG dimensions as the destination bounds and copied 4096 bytes per row
into a 3072-byte-pitch surface. It overran the surface by 1024 bytes at the end,
corrupting heap metadata. Before the fix, opening Manual produced a Wasm malloc
memory-access trap and the menu stopped responding.

TextureLoader now bounds a null destination by the surface dimensions, crops
FILTER_NONE copies to the source/destination intersection, and rejects invalid
explicit source or destination rectangles before locking/writing. Triangle
resampling scratch storage is sized by output width and the selected source
height, preventing overflow when upscaling/copying a cropped source.

Browser verification: three Manual entries, all nine pages visited in each,
three successful returns to title. The test waits for the real HelpInf substate,
page cursor and animation age before input, and fails on page errors/timeouts.
Result: PASS 27 pages, 3 menu exits; no Wasm trap. Screenshots showed the normal
title background and readable explanation text. Physical mobile verification
remains for the user.

Production WASM SHA-256 (includes preceding moon lifetime fix):
f61d187e31d0a4ffd9b9c02fe9318490b69e16441555b54a9ccf71949059bdb2.
