# Browser title lifetime and post-clear replay fixes

## Copied vertical strip

The inline browser startup worker enables the loading scene's draw callback and
sets `signature_ready=1`. The title loader skips the native 180-frame logo wait
and unloads slot 1 before that callback consumes the pending logo spawn.
`LoadingScene::draw` then calls `spawn_named_animation` with a freed ANM pointer.

Captured evidence before the fix:

- Title mesh handle array: `0x119fd118`.
- Stale named spawn: file `0x119fd118`, apparent file ID `0x2fffd`, script 0.
- Strip 20 stored handle became `0x16ffea`; its live animation was `0x16ffe9`.
- Mesh destruction could not resolve that handle. The surviving strip rendered
  282 triangle vertices after the gameplay compositor. Its first UV pair was
  approximately `(0.3174604, 0)` / `(0.3333334, 0)`.

The increment of the stale file's animation counter hit element 20 of the reused
handle array. This explains both the copied image column and dependence on heap
reuse. The fix disables the loading draw callback, clears pending readiness, and
nulls the logo pointer before unloading its resource. Mesh geometry and gameplay
compositing are unchanged.

After the fix, strip 20's stored and live handles both read `0x16ffe9`, all mesh
handles resolved during teardown, no stale spawn was logged, and the subsequent
gameplay draw trace contained no leftover title strip. A browser screenshot was
also inspected.

## Ranking to replay save

`00526b1a..00526b21` passes stage marker 8 into the session setter. The recovered
playable-stage selector rejects indices outside the eight resource entries
(0..7), so the production replay-save adapter threw before showing the slots.

The adapter now writes the result marker to PlayerTable +0x1f4 without selecting
an out-of-range stage resource. `prepare_save(..., 1)` still produces the existing
finished-stage replay marker.

Browser regression used a temporary, subsequently removed diagnostic export to
request scene 16 from an active recording. The actual production scene teardown,
ranking page, replay-save adapter, name editor, writer, and metadata reader ran.
Observed title state/phase sequence:

1. Ranking: 15 / 2.
2. Replay slots: 16 / 2.
3. Name entry: 16 / 3.
4. Saved and returned to slots: 16 / 2.

No browser exceptions or WebGL errors were reported. This is a targeted
post-clear transition test, not a complete six-stage playthrough.

## Build and limitations

`node portable/build.mjs --th20` and `node portable/package-eagler.mjs --th20`
succeeded. Production WASM SHA-256:
`06275f7a46ee8abd0b67bc421ce36636786f18eb3a6c39be85dbdc3240c84087`.
Temporary diagnostic exports and logging were removed before packaging.
`git diff --check` passed. The native oracle suite remains unavailable on this
machine because the repository's required Visual Studio 2019 generator is not
installed. Native startup behavior is unchanged by the browser-only lifetime fix.
