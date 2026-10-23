#!/usr/bin/env node
import {readFileSync} from 'node:fs';
import {pathToFileURL} from 'node:url';
import {resolve} from 'node:path';
import {nodeBuildTargets} from './affected-components.mjs';

const root = resolve(import.meta.dirname, '../..');
const workspace = JSON.parse(readFileSync(resolve(root, 'launcher/config/workspace.json'), 'utf8'));
const builds = JSON.parse(readFileSync(resolve(root, 'launcher/config/runtime-builds.json'), 'utf8'));

const ALWAYS_REQUIRED = [
  'scope',
  'check',
  'ui-behavior',
  'host-bootstrap-windows',
  'source-publication',
  'browser-shell-runtime',
];

const PORT_JOBS = Object.freeze({
  'ports/th06': ['ports_th06_eagler_ci_core-tests', 'ports_th06_eagler_ci_web-build'],
  'ports/th07': ['ports_th07_eagler_ci_core-tests', 'ports_th07_eagler_ci_web-build'],
  'ports/th08': ['ports_th08_presentation_lab_source-contract', 'ports_th08_replay_verifier_golden-integrity'],
  'ports/th08-mp': ['ports_th08_mp_presentation_lab_source-contract', 'ports_th08_mp_replay_verifier_golden-integrity'],
  'ports/th10': ['ports_th10_input_regressions_input', 'ports_th10_presentation_lab_source-contract', 'ports_th10_replay_verifier_golden-integrity'],
  'ports/th10-mp': ['ports_th10_mp_performance_regressions_native-regressions', 'ports_th10_mp_performance_regressions_wasm-owner-regressions',
    'ports_th10_mp_presentation_lab_source-contract', 'ports_th10_mp_replay_verifier_golden-integrity'],
});

const SERVICE_JOBS = Object.freeze({
  'services/relay': 'relay',
  'services/telemetry': 'telemetry',
  'services/turn': 'turn',
});

function isObject(value) {
  return value !== null && typeof value === 'object' && !Array.isArray(value);
}

function validatePlan({affected, nodeBuilds = [], sourceContracts, portAdaptation}) {
  if (!Array.isArray(affected) || !affected.every(path => typeof path === 'string'))
    throw new TypeError('affected must be an array of component paths');
  if (new Set(affected).size !== affected.length) throw new TypeError('affected contains duplicate components');
  if (!Array.isArray(nodeBuilds) || !nodeBuilds.every(target => isObject(target) &&
      typeof target.game === 'string' && typeof target.variant === 'string' && typeof target.directory === 'string'))
    throw new TypeError('nodeBuilds must be an array of {game, variant, directory} targets');
  if (sourceContracts != null && typeof sourceContracts !== 'boolean') throw new TypeError('sourceContracts must be a boolean');
  if (portAdaptation != null && typeof portAdaptation !== 'boolean') throw new TypeError('portAdaptation must be a boolean');
}

function sameBuildTargets(left, right) {
  const key = target => `${target.game}/${target.variant}/${target.directory}`;
  return left.map(key).sort().join('\n') === right.map(key).sort().join('\n');
}

export function nodeBuildTargetsForScope(affected) {
  return nodeBuildTargets(affected, workspace, builds);
}

/** Return the workflow jobs that must succeed for this scope. */
export function requiredJobIds(plan) {
  const {affected} = plan;
  const nodeBuilds = plan.nodeBuilds;
  validatePlan({affected, nodeBuilds: nodeBuilds ?? [],
    sourceContracts: plan.sourceContracts, portAdaptation: plan.portAdaptation});
  const selectedBuilds = nodeBuildTargetsForScope(affected);
  if (nodeBuilds != null && !sameBuildTargets(nodeBuilds, selectedBuilds))
    throw new Error('node-builds output does not match the affected source owners and runtime recipes');
  const sourceContracts = plan.sourceContracts ?? !(affected.length === 1 && affected[0] === 'launcher');
  const portAdaptation = plan.portAdaptation ?? true;
  const required = new Set(ALWAYS_REQUIRED);

  if (sourceContracts) required.add('source-contracts');
  if (portAdaptation) required.add('port-adaptation');
  if (affected.includes('common')) required.add('common_ci_native');
  for (const [component, jobs] of Object.entries(PORT_JOBS))
    if (affected.includes(component)) jobs.forEach(job => required.add(job));
  if (selectedBuilds.length) required.add('port-source-build');
  for (const [component, job] of Object.entries(SERVICE_JOBS))
    if (affected.includes(component)) required.add(job);
  return [...required].sort();
}

/** Reject any missing or non-success result for a job required by the plan. */
export function verifyRequiredJobs({affected, nodeBuilds, results, sourceContracts, portAdaptation}) {
  const expected = requiredJobIds({affected, nodeBuilds, sourceContracts, portAdaptation});
  if (!isObject(results)) throw new TypeError('results must be the needs object from GitHub Actions');
  const errors = [];
  for (const jobId of expected) {
    if (!Object.hasOwn(results, jobId)) {
      errors.push(`${jobId}: required job result is missing from needs`);
      continue;
    }
    const result = typeof results[jobId] === 'string' ? results[jobId] : results[jobId]?.result;
    if (result !== 'success') errors.push(`${jobId}: expected success, received ${String(result ?? 'missing')}`);
  }

  // A skipped job is valid only when the scope did not select it. Any job that
  // actually ran and failed still fails the aggregate, even after a bad plan.
  for (const [jobId, value] of Object.entries(results)) {
    const result = typeof value === 'string' ? value : value?.result;
    if (!['success', 'failure', 'cancelled', 'skipped'].includes(result)) {
      errors.push(`${jobId}: unknown job result ${String(result)}`);
    } else if (!expected.includes(jobId) && ['failure', 'cancelled'].includes(result)) {
      errors.push(`${jobId}: unexpected ${result} result`);
    }
  }
  return {expected, errors, ok: errors.length === 0};
}

function readJsonArgument(args, name, environment, fallback) {
  const prefix = `--${name}=`;
  const argument = args.find(value => value.startsWith(prefix));
  const source = argument?.slice(prefix.length) ?? process.env[environment];
  return source == null ? fallback : JSON.parse(source);
}

function main(args) {
  if (args.includes('--help')) {
    console.log('node tools/ci/required-checks.mjs --plan|--verify --affected=JSON --node-builds=JSON [--source-contracts=true] [--port-adaptation=true] [--results=JSON]');
    return;
  }
  const plan = {
    affected: readJsonArgument(args, 'affected', 'EAGLER_AFFECTED', undefined),
    nodeBuilds: readJsonArgument(args, 'node-builds', 'EAGLER_NODE_BUILDS', undefined),
    sourceContracts: readJsonArgument(args, 'source-contracts', 'EAGLER_SOURCE_CONTRACTS_REQUIRED', undefined),
    portAdaptation: readJsonArgument(args, 'port-adaptation', 'EAGLER_PORT_ADAPTATION_REQUIRED', undefined),
  };
  if (args.includes('--plan')) {
    console.log(JSON.stringify(requiredJobIds(plan)));
    return;
  }
  if (!args.includes('--verify')) throw new Error('Choose --plan or --verify');
  const results = readJsonArgument(args, 'results', 'EAGLER_RESULTS', undefined);
  const report = verifyRequiredJobs({...plan, results});
  for (const error of report.errors) console.error(error);
  if (!report.ok) process.exitCode = 1;
  else console.log(JSON.stringify({status: 'PASS', requiredJobs: report.expected}));
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  try { main(process.argv.slice(2)); }
  catch (error) { console.error(error.message); process.exitCode = 1; }
}
