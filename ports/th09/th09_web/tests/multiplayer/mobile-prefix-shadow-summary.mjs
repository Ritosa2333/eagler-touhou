// Read-only extraction. Usage: node mobile-prefix-shadow-summary.mjs NEW_OUTPUT.json INPUT_EVIDENCE.json
// No predictor rerun, raw-tape processing, arrival model, game, browser or performance estimate.
import {readFileSync, writeFileSync, existsSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve, basename} from 'node:path';
import {fileURLToPath} from 'node:url';
const json = JSON.stringify, list = v => Array.isArray(v) ? v : [], int = Number.isSafeInteger;
const sha = b => createHash('sha256').update(b).digest('hex');
const EXPECTED = {bytes: 8415566, sha256: '24422b707ae5a87e015db2db933a35a849df502b515793204162d03e259e367e'};
const GROUPS = {DirectTouch: 'mode:DirectTouch', Joystick: 'mode:Joystick', None: 'mode:None',
  moving: 'edge:changed-active-coordinates', repeat: 'edge:same-active-coordinates', turn: 'edge:wire-nonforward-turn',
  neutral: 'edge:wire-neutral-edge', afterNeutral: 'edge:within-8-frames-of-wire-neutral',
  entry: 'edge:wire-analog-entry', focus: 'edge:wire-focus-edge', flags: 'edge:wire-flags-edge'};
const errors = [], check = (ok, message) => {if (!ok) errors.push(message);};
function read(path, parse = false) {
  let bytes;
  try {bytes = readFileSync(path); return {path, bytes: bytes.length, sha256: sha(bytes), value: parse ? JSON.parse(bytes) : undefined};}
  catch (error) {return {path, bytes: bytes?.length ?? null, sha256: bytes ? sha(bytes) : null, error: String(error)};}
}
const identity = ({value, ...meta}) => meta;
const stable = (a, b) => !!a.sha256 && a.sha256 === b.sha256 && a.bytes === b.bytes;
function model(m, count, at) {
  check(m && m.count === count, at + ': model count');
  for (const k of ['fullNumeric', 'coordinatesNumeric', 'wire12B', 'coordinatePerfectFullFieldCeiling', 'metadata', 'buttons'])
    check(int(m?.[k]) && m[k] >= 0 && m[k] <= count, at + ': ' + k);
  check(m?.fullNumeric <= m?.coordinatePerfectFullFieldCeiling, at + ': full exceeds non-XY ceiling');
  return {full: m?.fullNumeric, xy: m?.coordinatesNumeric, wire: m?.wire12B,
    ceiling: m?.coordinatePerfectFullFieldCeiling, metadata: m?.metadata, buttons: m?.buttons};
}
function pair(g, at, optional = true) {
  if (g === undefined) {if (!optional) check(false, at + ': required group absent'); return {present: false, count: 0};}
  check(int(g?.count) && g.count >= 0, at + ': count');
  for (const k of ['cvUsed', 'predictionCoordinatesChanged', 'fullImproved', 'fullRegressed', 'bothFullHit', 'bothFullMiss'])
    check(int(g?.[k]) && g[k] >= 0 && g[k] <= g.count, at + ': ' + k);
  check(g?.fullImproved + g?.fullRegressed + g?.bothFullHit + g?.bothFullMiss === g?.count, at + ': pair partition');
  check(g?.netFullHits === g?.fullImproved - g?.fullRegressed &&
    g?.netFullHits === g?.cv?.fullNumeric - g?.held?.fullNumeric, at + ': net hits');
  check(g?.held?.coordinatePerfectFullFieldCeiling === g?.cv?.coordinatePerfectFullFieldCeiling, at + ': non-XY changed');
  return {present: true, count: g?.count, cvUsed: g?.cvUsed, coordinatesChanged: g?.predictionCoordinatesChanged,
    improve: g?.fullImproved, regress: g?.fullRegressed, net: g?.netFullHits,
    held: model(g?.held, g?.count, at + '/held'), cv: model(g?.cv, g?.count, at + '/cv')};
}
const [outputArg, inputArg, ...extra] = process.argv.slice(2);
if (!outputArg || !inputArg || extra.length) throw Error('Usage: node mobile-prefix-shadow-summary.mjs NEW_OUTPUT.json INPUT_EVIDENCE.json');
const output = resolve(outputArg), input = resolve(inputArg), self = fileURLToPath(import.meta.url);
if ([input, self].some(p => p.toLowerCase() === output.toLowerCase()) || existsSync(output)) throw Error('Output must be a new, separate file');
const before = read(self), source = read(input, true), evidence = source.value;
check(!before.error && !source.error, 'Source/analyzer read or JSON parse failed');
check(source.sha256 === EXPECTED.sha256 && source.bytes === EXPECTED.bytes, 'Unexpected c1 evidence identity');
check(evidence?.schema === 'th09-mobile-prefix-prediction-shadow/v1' && evidence?.analysisPassed === true && Array.isArray(evidence?.errors) && !evidence.errors.length, 'Source schema/analysis PASS');
check(evidence?.analyzer?.stable === true && evidence?.selfChecks?.passed === true &&
  list(evidence?.selfChecks?.checks).length === 15 && list(evidence?.selfChecks?.checks).every(c => c.passed === true), 'Original source/self-check coverage');
check(list(evidence?.runs).length === 9, 'Expected all nine runs');
const runs = list(evidence?.runs).map((r, ri) => {
  const prefix = 'run' + ri;
  check(r.input?.stable === true && Array.isArray(r.errors) && !r.errors.length && list(r.peers).length === 2, prefix + ': original input/peer coverage');
  return {index: ri, input: r.input, layout: r.layout, recorded: r.recorded, errors: r.errors, peers: list(r.peers).map((p, pi) => {
    const at = prefix + '/peer' + pi, horizons = list(p.horizons), limit = r.recorded?.parameters?.limit;
    check(p.side === pi && horizons.length === 8 && horizons.every((h, i) => h.horizon === i + 1), at + ': ordered H1..8');
    check(Array.isArray(p.coverage?.errors) && !p.coverage.errors.length && !p.analysisError, at + ': tape coverage');
    return {side: p.side, coverage: p.coverage, labelCoverage: p.labelCoverage, analysisError: p.analysisError,
      horizons: horizons.map(h => {
        const where = at + '/H' + h.horizon;
        check(h.checks && Object.values(h.checks).every(v => v === true) && h.overall?.count === limit, where + ': original checks/window');
        const groups = Object.fromEntries(Object.entries(GROUPS).map(([name, key]) => [name, pair(h.groups?.[key], where + '/' + name)]));
        check(groups.DirectTouch.count + groups.Joystick.count + groups.None.count === h.overall?.count, where + ': mode partition');
        return {horizon: h.horizon, originalChecks: h.checks, predictionAndActualStreamSha256: h.predictionAndActualStreamSha256,
          overall: pair(h.overall, where + '/overall', false), groups};
      }),
      H8Worst: Object.fromEntries(['directErrorIncrease', 'staleDirectCVOffset'].map(key => {
        const rows = horizons.find(h => h.horizon === 8)?.worst?.[key];
        check(Array.isArray(rows) && rows.length <= 3, at + ': worst ' + key);
        return [key, rows ?? null]; // Full original instances, including actual/held/CV and visible history.
      }))};
  })};
});
const afterSource = read(input), after = read(self);
check(stable(source, afterSource), 'Input evidence changed/unavailable'); check(stable(before, after), 'Extractor changed/unavailable');
const rows = ['H1/H8 table: full, xy and ceiling are exact-input counts; H8 M=changed active coordinates; source run index is zero based.',
  'run/peer\tH1 all H/C\tH8 all H/C\tH8 DT N:full H/C:xy H/C:ceiling\tH8 M N:full H/C:+/-\tH8 turn N:H/C\tH8 neutral N:H/C\tH8 max increase/release-offset'];
const counts = g => g?.present ? g.held.full + '/' + g.cv.full : '-';
const n = g => g?.count ?? '?', max = a => a?.length ? a[0].rank : null;
for (const r of runs) for (const p of r.peers) {
  const h1 = p.horizons.find(h => h.horizon === 1), h8 = p.horizons.find(h => h.horizon === 8), g = h8?.groups;
  const dt = g?.DirectTouch, m = g?.moving, coord = dt?.present ? dt.held.xy + '/' + dt.cv.xy : '-';
  rows.push([r.index + '/P' + (p.side + 1), counts(h1?.overall), counts(h8?.overall),
    n(dt) + ':' + counts(dt) + ':' + coord + ':' + (dt?.held?.ceiling ?? '-'),
    n(m) + ':' + counts(m) + ':' + (m?.improve ?? 0) + '/' + (m?.regress ?? 0),
    n(g?.turn) + ':' + counts(g?.turn), n(g?.neutral) + ':' + counts(g?.neutral),
    json([max(p.H8Worst.directErrorIncrease), max(p.H8Worst.staleDirectCVOffset)])].join('\t'));
}
rows.push(...runs.map(r => 'run' + r.index + '=' + basename(r.input?.before?.path ?? 'unknown')));
const summary = {schema: 'th09-mobile-prefix-shadow-summary/v1', createdAt: new Date().toISOString(), extractionPassed: !errors.length,
  expectedInput: EXPECTED, input: {before: identity(source), after: identity(afterSource), stable: stable(source, afterSource)},
  extractor: {before: identity(before), after: identity(after), stable: stable(before, after)}, errors,
  original: {schema: evidence?.schema, analysisPassed: evidence?.analysisPassed, analyzer: evidence?.analyzer,
    selfChecks: evidence?.selfChecks, parameters: evidence?.parameters, errors: evidence?.errors, limitations: evidence?.limitations},
  scope: 'Selected counts and original H8 examples from all nine runs, 18 peers and H1..8. No predictor rerun, correction/resim/FPS estimate, or production acceptance.',
  groupKeys: GROUPS, countMeaning: 'Shared group count is the denominator for both models. Absent groups have present=false/count=0. Edge groups overlap; source labels do not select samples.',
  errorMeaning: 'H8 rank is the original input-space distance/increase. Release is relative to the last active actual target, not a disabled target at zero or a ship-position error.',
  runs, stdoutTable: rows.join('\n')};
const bytes = json(summary) + '\n'; writeFileSync(output, bytes, {flag: 'wx'});
const head = json({output, bytes: Buffer.byteLength(bytes), sha256: sha(bytes), extractionPassed: summary.extractionPassed, errorCount: errors.length, firstErrors: errors.slice(0, 3)});
const table = head + '\n' + summary.stdoutTable;
console.log(Buffer.byteLength(table) <= 7500 ? table : head + '\nFull table retained in output; stdout size limit reached.');
if (!summary.extractionPassed) process.exitCode = 1;
