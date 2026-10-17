## Summary

Describe the user-visible or engineering problem this change solves and name
the subsystem that owns the behavior.

## Validation

- [ ] `npm run check`
- [ ] `npm run check:workspace` when this change intentionally depends on sibling Runtime repositories
- [ ] Relevant explicit browser/device/deployment lane, when applicable
- [ ] `git diff --check`

List any additional commands or manual acceptance performed:

```text

```

## Repository boundaries

- [ ] No original game data, music, extracted artwork, user saves, credentials, or other private content is included.
- [ ] Generated output (`.cache`, `dist`, `artifacts`, dependency directories) is not treated as authoritative source.
- [ ] Tests protect stable behavior/contracts rather than incidental implementation shape.
- [ ] Public behavior, deployment contracts, or compatibility boundaries have matching documentation updates when needed.

See [CONTRIBUTING.md](../CONTRIBUTING.md) and the
[documentation index](../docs/README.md) for repository ownership and test-lane
guidance.
