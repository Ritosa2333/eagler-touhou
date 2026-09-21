# Adaptation Worktree Isolation Playbook

Status: active

## Purpose

Keep upstream source tracking, the canonical Eagler integration branch and the
four adaptation areas most likely to carry broad semantic changes from sharing
one mutable checkout. Worktree isolation makes the source baseline, experiment
diff and accepted integration history independently reviewable.

## Required repository topology

For a title under active adaptation, keep these roles separate:

```text
title repository
  upstream worktree       original/upstream tracking only
  Eagler worktree         canonical `eagler` integration and Runtime input
  experiment worktree     one high-refresh, Multiplayer, THPrac or THCRAP topic
```

The upstream worktree must remain on the upstream-tracking branch or exact
upstream commit. It is evidence and merge input; it is never a Launcher Runtime
owner and must not receive Eagler fixes. The Eagler worktree is the canonical
consumer used by workspace checks, Launcher development and accepted builds.
Do not point Launcher metadata at an upstream or experiment worktree merely
because that checkout currently builds.

Use one experiment branch/worktree per bounded topic. A typical command is:

```text
git worktree add ../worktrees/thXX-<topic> -b experiment/<topic> eagler
```

The exact directory name is local policy; the recorded base commit and branch
role are not optional. A separate clone is acceptable only when it preserves
the same explicit upstream/Eagler/experiment identities.

## Changes that require an experiment worktree

This requirement is deliberately limited to:

- high-refresh presentation, interpolation, draw-only execution, scheduler or
  render/update separation;
- Multiplayer, netplay, rollback/prediction/resimulation and spectator/MP
  Replay behavior;
- THPrac hooks, ECL/section/warp/practice lifecycle and practice Replay state;
- THCRAP/localization patch compilation, runtime application, fallback and
  title-specific resource replacement.

Other adapter work does not gain a mandatory experiment worktree from this
policy. Maintainers may still isolate another risky topic voluntarily.

## Promotion into the Eagler worktree

Before merging or cherry-picking an experiment into the canonical `eagler`
branch:

1. record the upstream commit, Eagler base commit and experiment head;
2. state which title-owned behavior and lifecycle owners can change;
3. review the complete experiment diff against its Eagler base, not only the
   final symptom fix;
4. run the topic playbook's source, Runtime and evidence gates;
5. preserve negative, `unknown` and not-run results instead of relabeling them
   as PASS;
6. integrate reviewable commits without generated artifacts, private game
   content or experiment-only diagnostics;
7. rerun the canonical Eagler/workspace gates after integration.

High-refresh promotion must preserve fixed authoritative cadence and draw-only
purity. Multiplayer promotion must preserve ordinary single-player behavior,
determinism and Replay ownership. THPrac promotion must preserve ordinary-build
OFF behavior and original practice lifecycle. THCRAP promotion must preserve
the original-language baseline and validated fallback.

Do not merge an upstream-tracking branch into an experiment merely to make the
diff disappear. First advance the upstream worktree, then deliberately update
the experiment's recorded Eagler base or integrate the upstream change into the
canonical Eagler branch through its normal review path.

## Handoff

A handoff for one of the four isolated topics records:

```text
upstream worktree branch + commit:
Eagler worktree branch + base commit:
experiment worktree path + branch + head:
topic and behavior owners:
passing gates:
failing / unknown / not-run gates:
integration status:
```

An uncommitted working directory or an unlabeled copied source tree is not a
handoff boundary.
