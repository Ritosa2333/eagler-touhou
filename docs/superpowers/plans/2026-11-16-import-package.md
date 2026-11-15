# TH128 Import Package Implementation Plan

> Execute with the existing protocol/build DSH resident and MAIN integration. Read the supplied-package evidence and this plan; preserve completed route/core results. User already authorized autonomous development.

**Goal:** Deliver a TH128 resource ZIP and extracted folder accepted by the same package-import contract as the supplied TH12 example.

**Architecture:** Use the `eagler-touhou/package/1` envelope and a TH128-specific resource layout paired with the actual Runtime consumer. Keep the source repository, deployed Runtime and local game resource package separately identified.

**Tech Stack:** Existing Node.js tooling and JS host, current immutable C++/WASM core, upstream launcher Package Store.

**Spec:** [Import package requirements](../../IMPORT_PACKAGE.md).

## Global constraints

- MAIN inspected all 23 supplied TH12 payload files read-only; all sizes/hashes match. This is reference integrity, not TH128 import acceptance.
- Keep private100 and prior successful cores immutable; no rebuild for packaging alone.
- Existing protocol revision2 work completes first; queue the package work without overwriting that candidate.
- DSH writes only `dsh-web-completion-20261115/protocol-build/package-v1/`; MAIN alone integrates into the source repository.
- Do not alter the supplied TH12 folder, copy its game resources into TH128, scan unrelated E-drive folders, or touch user saves/config/replays.
- Existing exact pins/manifest/sourcecheck/MAIN-grant/once/ROOT rules apply to new product tests. Browser jobs remain bounded and isolated.
- No new repository, public-site hotpatch, upstream issue/PR or repeated successful route run.

## Review focus

- A manifest with TH12's DATA identity renamed to TH128 must fail validation.
- A local ZIP must import through the same Package Store generation and completion rules as online installation.
- Unsupported language/audio components must not be advertised by the descriptor.
- Missing/truncated/mismatched resources and path traversal must fail without replacing a complete installed version.
- Offline success requires actual Runtime and resource availability after reopening, not a warm-page or in-memory result.

## Task 1: pin the package/Runtime contract and map real TH128 resources

Files: worker `package-v1/CONTRACT.md`, `package-v1/RESOURCE-MAP.json`, `package-v1/FILES.json`.

- [ ] Read MAIN's `import-package-reference-20261116/inventory.json`, `docs/IMPORT_PACKAGE.md`, and the fixed upstream package/runtime documents.
- [ ] Inspect the matching upstream importer and DATA layout implementation at commit `bd8aa74be0098126ebf23f112c1913cca0f40d95`; pin the exact source files. Do not infer binary offsets or component support from the example alone.
- [ ] Map the existing TH128 catalog/generation/manifest paths to actual package resources and Runtime URLs. Record source paths, byte identities, consumer, mount target and required/optional status.
- [ ] Identify actual font, language and audio needs; keep the existing WAV/PCM behavior until any alternative has matching evidence.
- [ ] Return one finite contract/resource-map handoff for MAIN review before product integration.

## Task 2: implement an auditable package producer and validator

Proposed repository files: `tools/package-import.mjs`, `tests/import-package.test.mjs`. Produce first in the worker's owned folder.

- [ ] Expose a CLI taking an explicit staged-resource input, descriptor/layout specification and new output directory. It must refuse a pre-existing final output and never scan arbitrary user directories.
- [ ] Validate exact source-relative paths, mount targets, supported components, game identity, hashes and dataLayout before completing the descriptor.
- [ ] Cover wrong-game/layout, missing/truncated/changed file, duplicate target, escaped source path, unsupported language/audio and incomplete-update cases with admitted tests.
- [ ] Write the extracted package and root-level ZIP descriptor deterministically. Check entries against the same manifest; no test artifacts or personal data in the package.
- [ ] Return candidate, exact request/manifest/command and expected failures for MAIN's independent validation.

## Task 3: connect imported data to the production Runtime

Proposed areas: `site/play.mjs` resource resolution, a narrow Runtime resource provider and the deployment/launcher registration boundary, chosen from Task 1's concrete mapping.

- [ ] Consume installed, validated TH128 resources through the upstream resource interface while preserving the current standalone loader.
- [ ] Keep configure/launch/first-frame ordering, selected/effective language, installed version, owner cancellation and resource SHA identities consistent.
- [ ] Reuse exact private100 unless a new actual native failure proves a native change necessary.
- [ ] Submit whole actual-caller behavioral tests for the changed paths; Node seams remain explicitly separate from browser acceptance.

## Task 4: actual import, persistence, update and offline delivery

Files: final `th128-import-<revision>.zip`, extracted package folder, hash/readme and separate evidence record.

- [ ] MAIN runs admitted isolated-browser ZIP import and verifies the correct target and available components.
- [ ] Launch the actual Runtime from imported data; observe Title/Main and representative menu/game/audio resources.
- [ ] Reopen and check installed data; exercise an interrupted update and verify the previous complete installation survives.
- [ ] Verify offline reopen with the required Runtime/resources installed. Preserve first failure and use a new ordinal for repairs.
- [ ] Deliver the ZIP and matching extracted directory with exact checksums and concise import instructions. Mark the package complete only after these checks.

No total-progress percentage or completed import claim is introduced by this plan.
