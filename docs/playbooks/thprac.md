# THPrac and Portable Practice Playbook

Status: active

## Purpose

Guide real semantic THPrac integration and portable practice behavior. Prevent a launch popup, host bypass or UI-only harness from being mistaken for a working Practice lifecycle.

## Applicability and authority

THPrac integration must use a topic worktree separate from upstream tracking
and the canonical Eagler integration worktree. Promotion follows
[Adaptation Worktree Isolation](adaptation-worktrees.md).

- TH06/TH07 reallyportable and source/contract evidence provides the reusable owner, lifecycle and proof method.
- A title without proven THPrac integration must not infer support from another
  title's UI or hook layout.
- Game semantics belong to the target portable/source contract. Shared names such as `State` do not prove shared lifecycle semantics.

<!-- knowledge-id: K-THPRAC-001 -->
## Normal design

The observable path is:

```text
Practice → character/shot → original stage-select position → ImGui/overlay
```

Integration must preserve real ECL hook, section, warp, replay, pause, restart and target semantics. The portable approach replaces DLL injection/D3D/Win32 dependencies with source hooks, a render-independent menu and an adapter; it does not replace game semantics with a host-only mock.

The first hook boundary is after ECL decompression and before pointer tables. Ordinary builds stay unhooked. Use an expected-byte/CRC boundary for the instruction site where the source contract requires it; translated ECL may have a different whole-file CRC policy, but the site check remains.

Track owners separately: live config, original menu memory, adapter/context, host session, Replay candidate, THPrac mode/target and each game's globals. Fresh Practice clears stale owners; `Mode=Original`, `fakeType`, Direct Frame, Chapter, Pause and Replay each retain their own semantics.

Important recorded differences include TH06 Pause-R versus Continue, TH06 time-stop gameplay state, TH07 `mRepStatus`, `THSectionPatch`, `THStageWarp`, reset `State(1)` and navigation states. Do not merge same-named fields across games.

<!-- knowledge-id: K-THPRAC-002 -->
## Invariants and pitfalls

- UI visibility is not semantic integration proof.
- Vanilla Pause remains the vanilla owner; THPrac must not create a competing Pause layer.
- Retry/restart replaces the old owner and generation; stale `isInReplay` or candidate state must not poison fresh Practice.
- Preserve ECL transaction/range checks and source proof. Do not weaken correctness checks or use hash gates to hide a mismatch.
- Keep proof debt visible. A historical `closed` state can be reopened by a later user report.

## Superseded approaches

Launch popups, UI-only harnesses, host bypasses and fake character state are not Practice integration. A previous proof-closed label is not a permanent guarantee after a later user report.

## Code anchors

- `integrations/RUNTIME_CONTRACT.md`: shared Host/Runtime integration boundary.
- The target title's Practice lifecycle, ECL/resource owners and THPrac adapter:
  title-specific semantic authority.
- the sibling TH06/TH07 Runtime practice owners and, when available in a full
  maintainer workspace, the external `thprac-reallyportable` reference project.
  These are reference sources, not files shipped inside this repository.

## Verification

1. Verify the hook/source boundary and ordinary-build OFF behavior.
2. Enter fresh Practice, select character/shot, reach original stage-select and open the real overlay.
3. Exercise section/warp, Pause, Replay, restart/retry, target completion and exit/re-entry.
4. Check owner/reset/mutation closure and compare logic-first evidence, not only UI screenshots.
5. Label source contract, automated lifecycle, browser and human/device evidence separately.

## Deliberately omitted claims

This playbook does not promote a visible Practice menu to proof of real ECL,
Replay, pause/restart or target semantics.
