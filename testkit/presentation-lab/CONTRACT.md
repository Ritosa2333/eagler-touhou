# Presentation Lab consumer contract

The common lab owns session orchestration, UI, classification infrastructure,
report envelopes and local tooling. A game supplies two versioned boundaries:

- `RuntimeDriverV1` controls the title's real lifecycle, complete fixed-tick
  transaction, presentation draw and input path.
- `ObservationAdapterV1` decodes title-owned native evidence and explains its
  owner, lifecycle and continuity semantics.

The JavaScript interface is described in `contracts.d.ts` and enforced at
runtime by `contracts.mjs`. The native record ABI may differ between titles.
The v1 compatibility boundary is the pair of exported API major constants.
Breaking a required method, receipt, normalized record or purity rule requires
a new major. Additive report fields are allowed, but reports always carry the
exact common revision, title adapter version and native ABI identifier.

## Runtime invariants

`freeze()` is not application shutdown or an in-game pause. It returns a token
for the current session generation. `resume(token)` rejects a stale token,
preserves logical time and discards wall-clock debt accumulated while the lab
was frozen.

`advanceOneTick()` performs exactly one complete title-owned fixed-tick
transaction. It must not call a bare calculation chain when input capture,
rollback ownership, audio queues, overlays or reference publication live
outside that call. It returns an explicit receipt; loading or a finished scene
is not reported as a successful tick.

`drawOnly()` executes the real presentation path without consuming input,
Replay data, random numbers, audio queues or simulation counters. Native
suppression flags are implementation mechanisms, not proof of this property.

## Observation invariants

References are published by an original authoritative draw and carry their
actual simulation tick and draw serial. They must not be synthesized from the
interpolation implementation under test.

State evidence names its coverage. Missing evidence produces `unknown`, never
a purity pass. Every draw-only sample supplies the same versioned coverage
descriptor; an absent sample or malformed `missingGroups` list is unknown.
Object identity includes lifecycle or generation evidence; memory addresses
alone are not stable identities.

The title owner registry documents the authoritative writer, endpoint
publisher, presentation consumer, final submission point, identity,
continuity/discontinuity rules, restoration rule and fingerprint coverage.

## Build boundary

Diagnostic exports are compiled only with the title's presentation-audit
profile. Production packaging rejects those exports and does not ship this
testkit. Every consumer pins the exact `eagler-common` revision used to create
its report.
