import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {affectedComponents,nodeBuildTargets} from './affected-components.mjs';
import {nodeBuildTargetsForScope,requiredJobIds,verifyRequiredJobs} from './required-checks.mjs';
import {suites} from '../../tests/suites.mjs';

const root = resolve(import.meta.dirname, '../..');
const workspace = JSON.parse(readFileSync(resolve(root, 'launcher/config/workspace.json'), 'utf8'));
const builds = JSON.parse(readFileSync(resolve(root, 'launcher/config/runtime-builds.json'), 'utf8'));

function workflowJob(workflow, id) {
  const start = workflow.indexOf(`\n  ${id}:\n`);
  assert.notEqual(start, -1, `${id} job exists`);
  const next = /\n  [a-z0-9_-]+:\n/.exec(workflow.slice(start + 1));
  return workflow.slice(start, next ? start + 1 + next.index : undefined);
}

function sparseCheckoutPaths(job) {
  const checkout = job.match(/sparse-checkout: '([\s\S]*?)'/);
  assert(checkout, 'job must declare a sparse checkout');
  return checkout[1].trim().split(/\s+/);
}

const launcherOnly = ['launcher'];
const resultMap = (ids, override = {}) => Object.fromEntries(ids.map(id => [id, {result: override[id] ?? 'success'}]));

test('Launcher-only scope permits only conditionally skipped source contracts', () => {
  const expected = requiredJobIds({affected: launcherOnly});
  assert(expected.includes('check'));
  assert(expected.includes('browser-shell-runtime'));
  assert(expected.includes('ui-behavior'));
  assert(expected.includes('host-bootstrap-windows'));
  assert(expected.includes('port-adaptation'));
  assert(!expected.includes('source-contracts'));
  assert(!expected.includes('port-source-build'));
  const results = resultMap(expected, {'source-contracts': 'skipped', relay: 'skipped', turn: 'skipped'});
  results['source-contracts'] = {result: 'skipped'};
  results.relay = {result: 'skipped'};
  results.turn = {result: 'skipped'};
  assert.equal(verifyRequiredJobs({affected: launcherOnly, results}).ok, true);
});

test('ordinary and MP source changes require both product gates and build variants', () => {
  const affected = ['launcher', 'ports/th08', 'ports/th08-mp'];
  const nodeBuilds = [
    {game: 'th08', variant: 'normal', directory: 'ports/th08'},
    {game: 'th08', variant: 'multiplayer', directory: 'ports/th08-mp'},
  ];
  const expected = requiredJobIds({affected, nodeBuilds});
  for (const id of ['port-source-build', 'ports_th08_presentation_lab_source-contract',
    'ports_th08_replay_verifier_golden-integrity', 'ports_th08_mp_presentation_lab_source-contract',
    'ports_th08_mp_replay_verifier_golden-integrity']) assert(expected.includes(id));
  assert.equal(verifyRequiredJobs({affected, nodeBuilds, results: resultMap(expected)}).ok, true);
});

test('an affected source owner cannot lose its build matrix and silently skip the job', () => {
  const affected = ['launcher', 'ports/th08', 'ports/th08-mp'];
  assert(requiredJobIds({affected}).includes('port-source-build'));
  assert.throws(() => requiredJobIds({affected, nodeBuilds:[
    {game:'th08',variant:'normal',directory:'ports/th08'},
  ]}), /does not match/);
});

test('service scopes require their own service gate and allow other service gates to skip', () => {
  const affected = ['launcher', 'services/relay'];
  const coverage = {sourceContracts:false, portAdaptation:false};
  const expected = requiredJobIds({affected, ...coverage});
  assert(expected.includes('relay'));
  assert(!expected.includes('telemetry'));
  assert(!expected.includes('port-adaptation'));
  assert(!expected.includes('source-contracts'));
  const results = resultMap(expected, {telemetry: 'skipped', turn: 'skipped'});
  results.telemetry = {result: 'skipped'};
  results.turn = {result: 'skipped'};
  results['port-adaptation'] = {result: 'skipped'};
  results['source-contracts'] = {result: 'skipped'};
  assert.equal(verifyRequiredJobs({affected, results, ...coverage}).ok, true);
});

test('unexpected skip of an applicable job fails the aggregate', () => {
  const affected = ['launcher', 'services/relay'];
  const coverage = {sourceContracts:false, portAdaptation:false};
  const expected = requiredJobIds({affected, ...coverage});
  const results = resultMap(expected, {relay: 'skipped'});
  assert.deepEqual(verifyRequiredJobs({affected, results, ...coverage}).errors, ['relay: expected success, received skipped']);
});

test('failed, cancelled, and missing required jobs fail the aggregate', () => {
  const affected = ['launcher', 'services/telemetry'];
  const expected = requiredJobIds({affected});
  const results = resultMap(expected, {telemetry: 'failure', check: 'cancelled'});
  delete results['ui-behavior'];
  const report = verifyRequiredJobs({affected, results});
  assert(report.errors.includes('telemetry: expected success, received failure'));
  assert(report.errors.includes('check: expected success, received cancelled'));
  assert(report.errors.includes('ui-behavior: required job result is missing from needs'));
});

test('required-checks CLI accepts only the successful state of an applicable job', () => {
  const affected = ['launcher', 'services/relay'];
  const expected = requiredJobIds({affected, sourceContracts:false, portAdaptation:false});
  const cli = resolve(import.meta.dirname, 'required-checks.mjs');
  const run = relay => spawnSync(process.execPath, [cli, '--verify'], {encoding:'utf8', env:{...process.env,
    EAGLER_AFFECTED:JSON.stringify(affected), EAGLER_NODE_BUILDS:'[]', EAGLER_SOURCE_CONTRACTS_REQUIRED:'false',
    EAGLER_PORT_ADAPTATION_REQUIRED:'false', EAGLER_RESULTS:JSON.stringify(resultMap(expected,{relay}))}});
  const passing = run('success');
  assert.equal(passing.status, 0, passing.stderr);
  assert.match(passing.stdout, /"status":"PASS"/);
  const skipped = run('skipped');
  assert.equal(skipped.status, 1);
  assert.match(skipped.stderr, /relay: expected success, received skipped/);
});

test('Checks aggregates every job selected for the full workspace scope', () => {
  const affected = affectedComponents(null, workspace, builds);
  const nodeBuilds = nodeBuildTargets(affected, workspace, builds);
  const workflow = readFileSync(resolve(root, '.github/workflows/ci.yml'), 'utf8').replaceAll('\r\n', '\n');
  const checksStart = workflow.indexOf('\n  checks:\n');
  assert.notEqual(checksStart, -1);
  const checksEnd = workflow.indexOf('\n  relay:\n', checksStart);
  assert.notEqual(checksEnd, -1);
  const block = workflow.slice(checksStart, checksEnd);
  const needs = block.match(/^    needs:\r?\n((?:^    - [a-z0-9_-]+\r?\n)+)/m);
  assert(needs, 'Checks must declare its upstream job dependencies');
  const listed = new Set([...needs[1].matchAll(/^    - ([a-z0-9_-]+)$/gm)].map(match => match[1]));
  assert.match(workflow, /^  browser-shell-runtime:\n/m);
  const expected = requiredJobIds({affected, nodeBuilds, sourceContracts:true, portAdaptation:true});
  assert.deepEqual([...listed].sort(), expected,
    'Checks.needs must exactly match the required-check plan for a full run, including optional lanes');
});

test('PR updates run once while main, tags, manual runs and nightly remain covered', () => {
  const workflow = readFileSync(resolve(root, '.github/workflows/ci.yml'), 'utf8').replaceAll('\r\n', '\n');
  const triggerStart = workflow.indexOf("'on':\n");
  const triggerEnd = workflow.indexOf('\npermissions:\n', triggerStart);
  assert.notEqual(triggerStart, -1);
  assert.notEqual(triggerEnd, -1);
  const triggers = workflow.slice(triggerStart, triggerEnd);
  const pushStart = triggers.indexOf('\n  push:\n');
  const pullRequestStart = triggers.indexOf('\n  pull_request:', pushStart);
  assert.notEqual(pushStart, -1);
  assert.notEqual(pullRequestStart, -1);
  const push = triggers.slice(pushStart, pullRequestStart);
  const branches = push.match(/^    branches:\n((?:^    - .+\n)+)/m);
  assert(branches);
  assert.deepEqual(branches[1].trim().split('\n'), ['- main'],
    'feature branch updates are covered through pull_request instead of a second push run');
  assert.match(push, /^    tags:\n    - '\*'$/m, 'tag pushes remain enabled');
  assert.match(triggers, /^  pull_request: null$/m);
  assert.match(triggers, /^  workflow_dispatch: null$/m);
  assert.match(triggers, /^  schedule:\n  - cron: '.+'$/m);
});

test('Runtime Shell Chromium lane is a hard gate in every workflow run', () => {
  const workflow = readFileSync(resolve(root, '.github/workflows/ci.yml'), 'utf8').replaceAll('\r\n', '\n');
  const start = workflow.indexOf('\n  browser-shell-runtime:\n');
  const end = workflow.indexOf('\n  checks:\n', start);
  assert.notEqual(start, -1);
  assert.notEqual(end, -1);
  const job = workflow.slice(start, end);
  assert.doesNotMatch(job, /^    if:/m, 'the browser gate cannot be conditionally skipped');
  assert.match(job, /node tests\/run\.mjs --suite=browser-shell --output=/,
    'CI must execute the same shared suite used by local integration checks');
  const tasks=suites({root,environment:{}})['browser-shell'].tasks();
  assert.equal(suites({root,environment:{}})['browser-shell'].failFast,false,
    'independent input and audio checks must still execute when another contract fails');
  for(const id of ['runtime-protocol','native-gamepad','sfx-queue-output','touch-native-invariants','browser-shell','browser-shell-negative'])
    assert.ok(tasks.some(task=>task.id===id),`${id} remains a required shared Runtime Shell check`);
  assert.ok(tasks.find(task=>task.id==='browser-shell-negative').args.includes('--negative-control'));
  assert.match(job,/if-no-files-found: error/,'the hard gate retains its actual reports');
  const checks = workflow.slice(end, workflow.indexOf('\n  relay:\n', end));
  assert.match(checks, /^    - browser-shell-runtime$/m);
  const selected = requiredJobIds({affected:['launcher'], sourceContracts:false, portAdaptation:false});
  assert(selected.includes('browser-shell-runtime'));
  const skipped = resultMap(requiredJobIds({affected:['launcher'], sourceContracts:false, portAdaptation:false}));
  skipped['browser-shell-runtime'] = {result:'skipped'};
  assert.equal(verifyRequiredJobs({affected:['launcher'], results:skipped, sourceContracts:false, portAdaptation:false}).ok, false);
});

test('port adaptation gate runs registered public-fixture service tests and uploads their reports', () => {
  const workflow = readFileSync(resolve(root, '.github/workflows/ci.yml'), 'utf8').replaceAll('\r\n', '\n');
  const job = workflowJob(workflow, 'port-adaptation');
  assert.match(job, /actions\/setup-python@[^\n]+\n\s+with:\n\s+python-version: '3\.13'/);
  assert.match(job, /npm --prefix ports\/th09\/web ci --ignore-scripts/,
    'the suite package dependencies must be installed in both runner platforms');
  assert.match(job, /node tests\/run\.mjs --suite=ports-unit --output=artifacts\/validation\/ports-unit/,
    'the registered port behavior suite must run as part of the port hard gate');
  assert.match(job, /artifacts\/validation\/ports\/[\s\S]*artifacts\/validation\/ports-unit\//,
    'both source and public-fixture suite reports must be retained');

  const plan = {affected:['ports/th09'], sourceContracts:false};
  const expected = requiredJobIds(plan);
  assert(expected.includes('port-adaptation'));
  const results = resultMap(expected, {'port-adaptation':'skipped'});
  assert.equal(verifyRequiredJobs({...plan, results}).ok, false,
    'an unexpectedly skipped cross-platform port suite must fail the aggregate gate');
});

test('source-contract tooling suite receives a complete checkout', () => {
  const workflow = readFileSync(resolve(root, '.github/workflows/ci.yml'), 'utf8').replaceAll('\r\n', '\n');
  const start = workflow.indexOf('\n  source-contracts:\n');
  const end = workflow.indexOf('\n  port-adaptation:\n', start);
  assert.notEqual(start, -1);
  assert.notEqual(end, -1);
  const job = workflow.slice(start, end);
  assert.match(job, /actions\/setup-python@[^\n]+\n\s+with:\n\s+python-version: '3\.13'/);
  assert.match(job, /actions\/checkout@[^\n]+\n\s+with:\n\s+submodules: false/);
  assert.doesNotMatch(job, /sparse-checkout:/,
    'tooling discovery reads tracked tools, ports, common contracts, and the root test catalog');
  assert.match(job, /node tests\/run\.mjs --suite=tooling/);
  assert.match(job, /node tests\/run\.mjs --suite=ports-touch-wasi/,
    'the fixed source-contract WASI SDK must execute the cataloged production touch-bomb wrappers');
});

test('scope and Launcher core sparse checkouts include their test import closure', () => {
  const workflow = readFileSync(resolve(root, '.github/workflows/ci.yml'), 'utf8').replaceAll('\r\n', '\n');
  const scope = workflowJob(workflow, 'scope');
  for (const path of ['tools/ci', '.github/workflows', 'launcher/config'])
    assert(sparseCheckoutPaths(scope).includes(path), `scope regression tests read ${path}`);
  assert.match(scope, /node --test tools\/ci\/affected-components\.test\.mjs/);

  const core = workflowJob(workflow, 'check');
  for (const path of ['launcher', 'tools/browser', 'tools/testing', 'common/contracts', 'services', 'tests'])
    assert(sparseCheckoutPaths(core).includes(path), `Launcher core check imports or reads ${path}`);
  assert.match(core, /npm run check/);
});

test('UI root-runner checkout includes its catalog and execution dependencies', () => {
  const workflow = readFileSync(resolve(root, '.github/workflows/ci.yml'), 'utf8').replaceAll('\r\n', '\n');
  const start = workflow.indexOf('\n  ui-behavior:\n');
  const end = workflow.indexOf('\n  host-bootstrap-windows:\n', start);
  assert.notEqual(start, -1);
  assert.notEqual(end, -1);
  const job = workflow.slice(start, end);
  const checkout = job.match(/sparse-checkout: '([\s\S]*?)'/);
  assert(checkout, 'UI job must use a declared sparse checkout');
  const selected = checkout[1].trim().split(/\s+/);
  for (const path of ['launcher', 'tools/browser', 'tools/testing', 'common/contracts', 'services/relay', 'tests'])
    assert(selected.includes(path), `UI root suite requires checkout path ${path}`);
  assert.match(job, /npm run test:ui-rewrite/);
});

test('service test jobs have bounded execution time', () => {
  const workflow = readFileSync(resolve(root, '.github/workflows/ci.yml'), 'utf8').replaceAll('\r\n', '\n');
  for (const [id, minutes] of [['relay', 10], ['telemetry', 5], ['turn', 5]]) {
    const header = `\n  ${id}:\n`;
    const start = workflow.indexOf(header);
    assert.notEqual(start, -1, `${id} job exists`);
    const contentStart = start + header.length;
    const next = /\n  [a-z0-9_-]+:\n/.exec(workflow.slice(contentStart));
    const block = workflow.slice(start, next ? contentStart + next.index : undefined);
    assert.match(block, new RegExp(`^    timeout-minutes: ${minutes}$`, 'm'),
      `${id} is bounded so a stuck service suite cannot occupy a runner indefinitely`);
  }
});

test('nightly and manual full scope include the Runtime Shell browser hard gate', () => {
  const affected = ['common', 'launcher', 'ports/th06', 'ports/th07', 'ports/th08', 'ports/th08-mp',
    'ports/th09', 'ports/th10', 'ports/th10-mp', 'ports/th11', 'ports/th11-mp', 'ports/th15',
    'ports/th15-mp', 'ports/th20', 'services/relay', 'services/telemetry', 'services/turn'];
  const nodeBuilds = nodeBuildTargetsForScope(affected);
  const expected = requiredJobIds({affected, nodeBuilds});
  for (const id of ['common_ci_native', 'port-source-build', 'relay', 'telemetry', 'turn', 'browser-shell-runtime'])
    assert(expected.includes(id));
  assert.equal(new Set(expected).size, expected.length);
});
