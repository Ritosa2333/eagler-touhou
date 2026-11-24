# Natural GameOver / Continue branch

These are the exact source files executed by admitted case598 against private100. The test uses the existing original asset manifest and an explicit local request JSON; original resources, runtime binaries and private evidence are not distributed here.

A request must specify a fresh owned output directory, exact runtime/assets/source pins and a bounded single-World failure-only scope. Do not point output at the immutable successful case598 or rerun that accepted case for another PASS. The test creates outputs with exclusive writes.

After a new-purpose request is reviewed and admitted, its command shape is:

```text
node tests/gameover-continue/check.cjs <owned-request.json>
```

The helper runs actual native logic and the production pause/score consumers. Audio output and the strict score store are synthetic. It does not test full Title/Main/browser behavior or concurrent owner replacement. See ../../docs/restoration-gameover598.md for the accepted scope.
