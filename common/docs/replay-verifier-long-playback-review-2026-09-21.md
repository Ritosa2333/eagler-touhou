# Long replay evidence review — 2026-09-21

The requested nine-case suite is not complete. This document began as the
checkpoint-only review; subsequent work on the same date completed and passed
the TH08/TH10 current-Eagler long-replay equivalence gates. TH06/TH07 remain
incomplete.

| Title | Samples | Evidence actually obtained | Remaining work |
| --- | --- | --- | --- |
| TH06 | Lunatic, Extra | Format validation; failed retail startup/observer attempts | Verified windowed retail playback and current-candidate capture |
| TH07 | Lunatic, Extra, Phantasm | Historical `th07_web_ref` stage-local harness reports mismatches | Actual retail playback and current-candidate capture; investigate harness mismatches separately |
| TH08 | Lunatic Final B, Extra | Published original-JIT golden; current candidate passed 110851 / 41505 normalized ticks | Expand the currently declared limited field schema |
| TH10 | Lunatic, Extra | Published original-JIT golden; current candidate passed 84797 / 39111 normalized ticks | Expand the currently declared limited field schema |

## Corrections to earlier conclusions

- `Start-Process -WindowStyle Hidden` never established windowed rendering.
  The removed `play-retail-replay.ps1` did not configure or verify it. The
  supported Present capture launcher now rejects configs which would cause
  fullscreen or a retail fallback to fullscreen before creating a process.
- The TH06 attempt that reported an alive process and zero trace bytes did
  not prove a working observer or playback. A surviving error window can
  explain an alive process. The source reads keyboard state / DirectInput;
  posted `WM_KEYDOWN` messages are not an established provider for that path.
  Filename/locale, proxy and initialization hypotheses were not isolated well
  enough to identify a root cause. None is recorded as a proven cause.
- The TH07 historical harness is another implementation, not the original
  executable or current `th07-eagler`. It initializes recorded stages
  independently. Several stages stopped on game over before exhausting their
  input streams. Saying all six stages had fully consumed input was incorrect.
  These mismatches prove a disagreement in that harness run, not which side is
  wrong or that the current Eagler runtime desyncs.
- TH10 previously set `complete` for any mode other than 2, including premature
  exit, without checking expected route or score. The new validator checks
  those conditions, with negative cases for early exit, missing stages,
  wrong loadout, fixture and scores. Existing observations were retained and
  checked offline against the actual replay bytes; games were not rerun during
  this review.
- The old TH10 `playbackFrames` values 84900 / 39000 start after a 300-frame
  startup advance and finish on a 300-frame polling boundary. They are not
  exact input counts. The files contain 85222 / 39112 recorded inputs. New
  reports call the budget metric `applicationFramesAfterStartup`.
- TH08 internal stage 5 was incorrectly called Stage 6A in progress updates;
  the recorded route is kept as its actual IDs `[0,1,2,3,5,7]`. No inference
  from that label is used for validation.

## Rechecked checkpoints

TH08 Lunatic: stages `[0,1,2,3,5,7]`; final displayed score 2036802750.
TH08 Extra: stage `[8]`; final displayed score 1962878380.

TH10 Lunatic: stages `[1,2,3,4,5,6]`; stage-entry displayed scores
`[0,25767530,71255230,174579660,314806550,550463050]`;
final displayed score 1288193400. All match the decoded replay.
TH10 Extra: stage `[7]`; stage-entry score 0; final displayed score 864641590.

Sources are the title-local ignored
`artifacts/replay-verifier/long-playback/` reports and the SHA-256 pinned
`tools/replay-verifier/fixtures/original/` replay files. The TH10 validation
coverage is explicitly `original-playback-route-and-score-checkpoints`, with
`deterministicEquivalence: false`.

## Boundaries and next work

The user permits oracle-side process patches for automation. Such patches
must be identified in run provenance and checked for their impact on the
observed logic; neither repeated output nor successful playback alone certifies
an accelerated or patched oracle. Eagler normal gameplay and built-in Demo
mechanisms remain outside the allowed modification scope. Native originals
must run windowed.

Continue by establishing one reliable, windowed TH06/TH07 retail runner with
positive observer and replay-entry acknowledgements. Then finish their five
retail and current-candidate cases. Preserve incomplete status for those works
until both providers and full logical-tick comparisons exist.

The completed TH08/TH10 traces are stored as content-addressed compressed
golden sets in their title repositories. Their quick and daily gates consume
those assets without launching the original executable; regenerating an oracle
remains a separate advanced operation.

Review verification: 8 TH10 completion-validator tests, 5 windowed-config
tests, and 11 shared replay-verifier tests passed. Both saved TH10 reports
passed the new validator against their original replay files.
