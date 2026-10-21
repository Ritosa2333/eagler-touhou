import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createVisualReview, validateVisualReview} from './review-record.mjs';

const png = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR4nGNgYAAAAAMAAWgmWQ0AAAAASUVORK5CYII=';
const hash = character => character.repeat(64);
const checks = ['DISPLAY-02', 'DISPLAY-04', 'DISPLAY-05', 'DISPLAY-06', 'DISPLAY-08'];
const environment = () => ({displayModel: 'Studio Display', refreshHz: 120,
  browser: {name: 'Firefox', version: '132.0'}, os: {name: 'Windows', version: '11'}});
const reportObject = ({previousTick = 99, tick = 100, commit = '1'.repeat(40), valid = true,
  negativeControl = false, images, objects} = {}) => ({
  schema: 'th08/presentation-audit/1', valid, negativeControl,
  previousTick, tick, purityStatus: 'pass', scene: {status: [2, 1], diagnostics: [0, 1, 2]},
  build: {schema: 'th08/presentation-lab-build/1', game: 'th08',
    commit, wasm: hash('a'), loader: hash('b'), commonCommit: '2'.repeat(40),
    sourceDigest: hash('c'), toolDigest: hash('d'), toolSources: {'th08/controller.mjs': hash('e')},
    adapterVersion: 'th08-presentation-lab/1', nativeAbi: 'th08-presentation-lab-native/1',
    inputs: {data: {bytes: 4096, sha256: hash('f')}, replay: null}},
  objects: objects ?? [{key: `1:2:3:4:${tick}`, ownerId: 1, object: 2}],
  images: images ?? [{alpha: 0.5, png, boxes: [{key: `1:2:3:4:${tick}`, bounds: [10, 20, 30, 40]}]}]
});
const material = (name, options = {}) => ({filename: `${name}.json`, reportText: JSON.stringify(reportObject(options))});
const inputs = () => ({materials: [material('title-idle'), material('title-animate', {previousTick: 100, tick: 101})],
  reviewer: {name: 'Visual reviewer', kind: 'human'}, verdict: 'passed', notes: '', selectedChecks: checks,
  environment: environment()});
const digest = value => createHash('sha256').update(value).digest('hex');

test('creates one verdict for multiple hash-bound windows without per-check prose or completion claims', async () => {
  const input = inputs(), record = await createVisualReview(input);
  assert.equal(record.schema, 'presentation-lab/human-visual-review/2');
  assert.equal(record.scope, 'human-visual-review');
  assert.equal(record.verdictScope, 'sampled-material-set');
  assert.equal(record.verdict, 'passed');
  assert.equal(record.notes, '');
  assert.equal(record.materials.length, 2);
  for (const [index, source] of input.materials.entries()) {
    assert.equal(record.materials[index].filename, source.filename);
    assert.equal(record.materials[index].report.bytes, new TextEncoder().encode(source.reportText).byteLength);
    assert.equal(record.materials[index].report.sha256, digest(source.reportText));
    assert.match(record.materials[index].imageEvidence[0].sha256, /^[a-f0-9]{64}$/);
  }
  assert.deepEqual(record.checks, checks.map(id => ({id, complete: false})));
  assert.equal(record.observations, undefined);
  assert.deepEqual(record.claims, {adaptationComplete: false, automaticPurityOverridden: false, checksComplete: false});
  assert.deepEqual(await validateVisualReview({record, materials: input.materials}), {valid: true, diagnostics: []});
});

test('pass requires actual display settings and a nonempty material set', async () => {
  const input = inputs();
  await assert.rejects(createVisualReview({...input, materials: []}), /at least one scene report/);
  const missingRate = inputs(); delete missingRate.environment.refreshHz;
  await assert.rejects(createVisualReview(missingRate), /refreshHz is missing or invalid/);
  const diagnostic = await createVisualReview({...missingRate, verdict: 'inconclusive'});
  assert.equal(diagnostic.environment.refreshHz, null);
  assert.ok(diagnostic.diagnostics.some(item => item.includes('refreshHz is missing')));
  assert.deepEqual((await validateVisualReview({record: diagnostic, materials: missingRate.materials})).diagnostics, diagnostic.diagnostics);
});

test('rejects replaced, reordered, omitted, or duplicated source reports', async () => {
  const input = inputs(), record = await createVisualReview(input);
  const changed = [...input.materials]; changed[1] = material('replacement', {previousTick: 100, tick: 102});
  await assert.rejects(validateVisualReview({record, materials: changed}), /do not match the review material set/);
  await assert.rejects(validateVisualReview({record, materials: input.materials.slice(0, 1)}), /do not match the review material set/);
  await assert.rejects(validateVisualReview({record, materials: [...input.materials].reverse()}), /do not match the review material set/);
  await assert.rejects(createVisualReview({...input, materials: [input.materials[0], input.materials[0]]}), /duplicate report/);
});

test('requires complete report, build, tool, input, image, and object identities to pass', async () => {
  const missingData = inputs();
  missingData.materials[0] = material('no-data', {previousTick: 99, tick: 100});
  const report = JSON.parse(missingData.materials[0].reportText); delete report.build.inputs.data;
  missingData.materials[0].reportText = JSON.stringify(report);
  await assert.rejects(createVisualReview(missingData), /inputs.data is missing/);

  for (const [name, options, pattern] of [
    ['invalid-report', {valid: false}, /report.valid is not true/],
    ['negative-report', {negativeControl: true}, /negativeControl:false/],
    ['empty-images', {images: []}, /report.images is missing|report images/],
    ['empty-objects', {objects: []}, /no identified runtime objects|do not identify an object/],
    ['broken-image', {images: [{alpha: .5, png: 'data:image/png;base64,AAAA', boxes: []}]}, /complete PNG image/]
  ]) {
    const input = inputs(); input.materials[0] = material(name, options);
    await assert.rejects(createVisualReview(input), pattern);
  }
});

test('keeps one material set on a single build and input identity', async () => {
  const input = inputs(); input.materials[1] = material('other-build', {previousTick: 100, tick: 101, commit: '3'.repeat(40)});
  await assert.rejects(createVisualReview(input), /share the same Runtime and input identity/);
  const diagnostic = await createVisualReview({...input, verdict: 'inconclusive'});
  assert.ok(diagnostic.diagnostics.some(item => item.includes('share the same Runtime')));
});

test('requires named human reviewer, explicit verdict, and failure detail while preserving automatic purity', async () => {
  const input = inputs(); input.reviewer = {name: 'Automation', kind: 'automated'};
  await assert.rejects(createVisualReview(input), /kind:"human"/);
  await assert.rejects(createVisualReview({...inputs(), verdict: undefined}), /verdict must be explicitly passed/);
  await assert.rejects(createVisualReview({...inputs(), verdict: 'failed'}), /failed review needs a short issue description/);
  const record = await createVisualReview(inputs());
  for (const claims of [
    {...record.claims, adaptationComplete: true},
    {...record.claims, automaticPurityOverridden: true},
    {...record.claims, checksComplete: true}
  ]) await assert.rejects(validateVisualReview({record: {...record, claims}, materials: inputs().materials}), /cannot grant adaptation completion/);
  await assert.rejects(validateVisualReview({record: {...record, checks: record.checks.map(row => ({...row, complete: true}))}, materials: inputs().materials}), /cannot mark an adaptation check complete/);
  await assert.rejects(validateVisualReview({record: {...record, verdictScope: 'adaptation-check'}, materials: inputs().materials}), /verdict scope/);
});

test('validates prior version 1 records read-only', async () => {
  const source = material('legacy'), report = JSON.parse(source.reportText), imageBytes = Buffer.from(png.split(',')[1], 'base64');
  const key = report.objects[0].key, build = report.build;
  const legacy = {schema: 'presentation-lab/human-visual-review/1', scope: 'human-visual-review', verdictScope: 'sampled-window',
    report: {schema: report.schema, bytes: Buffer.byteLength(source.reportText), sha256: digest(source.reportText)},
    identity: {game: build.game,
      build: {schema: build.schema, commit: build.commit, wasm: build.wasm, loader: build.loader,
        commonCommit: build.commonCommit, sourceDigest: build.sourceDigest},
      tool: {sha256: build.toolDigest, sources: build.toolSources}, nativeAbi: build.nativeAbi,
      adapterVersion: build.adapterVersion, inputs: {data: build.inputs.data, replay: null, replayProvided: true}},
    imageEvidence: [{index: 0, alpha: .5, bytes: imageBytes.length, sha256: digest(imageBytes), boxes: [{key, bounds: [10, 20, 30, 40]}]}],
    objectEvidence: {count: 1, keys: [key], matchedImageObject: key},
    scene: {label: 'Legacy sample', report: report.scene}, tickRange: {from: report.previousTick, to: report.tick},
    selectedChecks: ['DISPLAY-06'], checks: [{id: 'DISPLAY-06', complete: false}],
    observations: [{id: 'DISPLAY-06', operation: 'Inspect one saved window', expected: 'The scene is readable.', actual: 'This window is readable.'}],
    reviewer: {name: 'Earlier reviewer', kind: 'human'}, verdict: 'inconclusive', notes: '',
    environment: {displayModel: 'Studio Display', refreshHz: {value: 120, unit: 'Hz', source: 'human-reported'},
      browser: {name: 'Firefox', version: '132.0'}, os: {name: 'Windows', version: '11'}},
    automaticEvidence: {valid: true, purityStatus: 'pass', negativeControl: false},
    claims: {adaptationComplete: false, automaticPurityOverridden: false, checksComplete: false},
    createdAt: new Date().toISOString(), diagnostics: []};
  assert.deepEqual(await validateVisualReview({record: legacy, reportText: source.reportText}), {valid: true, diagnostics: []});
  await assert.rejects(createVisualReview({reportText: source.reportText, reviewer: {name: 'Earlier reviewer', kind: 'human'},
    verdict: 'passed', selectedChecks: checks, environment: environment()}), /at least one scene report/);
});
