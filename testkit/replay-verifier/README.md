# Replay Verifier

This testkit compares title-normalized authoritative state after every complete
Replay simulation tick. It owns trace validation, strict alignment, comparison
and reporting. A game adapter owns Replay semantics, lifecycle, state fields and
the original/candidate execution environments.

The first transport is streaming JSON Lines. Each file begins with `run-start`,
contains explicit segment lifecycle and tick records, and ends with a complete
`run-end`. Sequence gaps, early EOF, incompatible identities and missing
required categories fail closed.

`adapter.mjs` supplies shared lifecycle framing for one or more title-owned
segments plus the versioned legacy JSON digest helper. Title adapters still
own tick boundaries, field selection and lifecycle semantics.

Compare two traces:

```text
node testkit/replay-verifier/cli.mjs compare \
  --expected expected.jsonl --actual actual.jsonl --report result.json
```

The complete architecture and title rollout are in
[`docs/replay-verifier-plan.md`](../../docs/replay-verifier-plan.md).
