// Offline opportunity screen only. No game/native/browser/transport code is imported.
// Usage: node mobile-prefix-prediction-shadow.mjs NEW_OUTPUT.json RAW.json [RAW.json ...]
// Every target f receives the fixed actual prefix [0, f-H], H=1..8. Predictions are
// frozen before scoring reads target f or its post-hoc phase/turn/release labels.
// This does not reconstruct real arrivals, used decisions, corrections, resim or FPS.
import {readFileSync, writeFileSync, existsSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const array = Array.isArray, finite = Number.isFinite, integer = Number.isInteger;
const f32 = Math.fround, json = JSON.stringify;
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
const same = (a, b) => json(a) === json(b);
const HORIZONS = Object.freeze([1, 2, 3, 4, 5, 6, 7, 8]);
const MODES = ['None', 'Joystick', 'DirectTouch'], MAX_ROWS = 24000;
const parameters = Object.freeze({
  horizons: HORIZONS, inputDelay: 0, targetWindow: '[0, raw.parameters.limit)',
  visiblePrefix: 'For target f and H, actual frames <= f-H only; negative cutoff has no anchor.',
  predictorInputs: 'At most the newest two consecutive actual records in that prefix. No predicted history.',
  heldReference: 'Current held model on every retained tape, not the original used decisions of an older baseline.',
  held: {predictableButtons: 0xf5, directionButtons: 0xf0, directionFrames: 3, touchBomb: false},
  cv: {mode: 'DirectTouch', history: 2, maximumHorizon: 8, allowUnlimited: false,
    metadataGate: 'Same mode and flags; unchanged focus/directions (buttons & 0xf4). Shot is not extrapolated.',
    formula: 'v=f32(anchor-previous); xy=f32(anchor+f32(H*v)); each axis independently.',
    fallback: 'No/insufficient history, other mode, unlimited, metadata edge, or invalid predicted input.',
    limit: 'H<=8 plus TH09 input validity. No extra speed clamp; +/-4096 is not a field boundary.'},
  scoring: 'C++ numeric equality for all fields; separate exact coordinates, encoded 12-byte equality, and non-XY ceiling.',
  errorUnits: {directTarget: 'field coordinate units, both modes DirectTouch',
    joystickVector: 'wire velocity units, both modes Joystick',
    staleDirectOffset: 'distance from the last actual DirectTouch target while actual input is None; not ship movement'},
  quantiles: 'Sorted values at floor(n*p), with null for an empty population.',
  groups: 'Mode and source-phase/mode partitions each cover the window; wire-edge groups overlap.',
  examplesPerKindPerHorizon: 3, maximumRetainedRowsPerPeer: MAX_ROWS,
});

function validInput(v) {
  if (!v || !integer(v.buttons) || v.buttons < 0 || v.buttons > 65535 ||
      !integer(v.flags) || v.flags < 0 || (v.flags & ~3) || !finite(v.x) || !finite(v.y)) return false;
  if (v.analogMode === 0) return v.x === 0 && v.y === 0 && v.flags === 0;
  if (v.analogMode === 1) return Math.abs(v.x) <= 16 && Math.abs(v.y) <= 16 && !(v.flags & 1);
  return v.analogMode === 2 && Math.abs(v.x) <= 4096 && Math.abs(v.y) <= 4096;
}
function encode(v) {
  const b = Buffer.alloc(12);
  b.writeUInt16LE(v.buttons, 0); b[2] = v.analogMode; b[3] = v.flags;
  b.writeFloatLE(v.x, 4); b.writeFloatLE(v.y, 8);
  return b.toString('hex');
}
const fields = v => ({buttons: v.buttons, analogMode: v.analogMode, flags: v.flags, x: v.x, y: v.y});
const neutral = () => ({buttons: 0, analogMode: 0, flags: 0, x: 0, y: 0});
const xyEqual = (a, b) => a.x === b.x && a.y === b.y;
const metaEqual = (a, b) => a.analogMode === b.analogMode && a.flags === b.flags;
const nonXYEqual = (a, b) => a.buttons === b.buttons && metaEqual(a, b);
const fullEqual = (a, b) => nonXYEqual(a, b) && xyEqual(a, b);
const distance = (a, b) => Math.hypot(a.x - b.x, a.y - b.y);

// Only these three values enter the predictor; actual target/labels are unavailable here.
export function predictPrefix(targetFrame, anchor, previous) {
  if (!anchor) return {held: Object.freeze(neutral()), cv: Object.freeze(neutral()), reason: 'no-anchor', usedCV: false};
  const h = targetFrame - anchor.frame;
  if (!integer(h) || h < 1 || h > 8) throw Error('Prediction anchor outside H1..8');
  const held = fields(anchor); held.buttons &= 0xf5; held.flags &= ~4;
  if (h > 3) held.buttons &= ~0xf0;
  Object.freeze(held);
  let reason = anchor.analogMode !== 2 ? 'mode-held' : anchor.flags & 1 ? 'unlimited-held' :
    !previous || previous.frame !== anchor.frame - 1 ? 'insufficient-history' :
    previous.analogMode !== 2 ? 'mode-edge' :
    previous.flags !== anchor.flags || ((previous.buttons ^ anchor.buttons) & 0xf4) ? 'metadata-edge' : null;
  if (reason) return {held, cv: held, reason, usedCV: false};
  const vx = f32(anchor.x - previous.x), vy = f32(anchor.y - previous.y);
  const cv = {...held, x: f32(anchor.x + f32(h * vx)), y: f32(anchor.y + f32(h * vy))};
  if (!validInput(cv)) return {held, cv: held, reason: 'invalid-extrapolation', usedCV: false};
  return {held, cv: Object.freeze(cv), reason: vx === 0 && vy === 0 ? 'cv-zero-step' : 'cv-extrapolated', usedCV: true};
}

function distribution(values) {
  const a = values.slice().sort((x, y) => x - y), at = p => a[Math.floor(a.length * p)] ?? null;
  return {count: a.length, sum: a.reduce((s, x) => s + x, 0), p50: at(.5), p95: at(.95), p99: at(.99), max: a.at(-1) ?? null};
}
function modelBucket() {
  return {count: 0, fullNumeric: 0, coordinatesNumeric: 0, wire12B: 0, metadata: 0, buttons: 0,
    coordinatePerfectFullFieldCeiling: 0, onlyCoordinatesMismatch: 0, onlyNonXYMismatch: 0, bothMismatch: 0,
    staleAnalogOnNone: 0, missingAnalogOnActive: 0, analogModeMismatch: 0,
    directTargetErrors: [], joystickVectorErrors: [], staleDirectOffsets: []};
}
function pairBucket() {
  return {count: 0, cvUsed: 0, predictionCoordinatesChanged: 0, reasons: {}, fullImproved: 0, fullRegressed: 0, bothFullHit: 0, bothFullMiss: 0,
    held: modelBucket(), cv: modelBucket()};
}
function scoreModel(p, actual, lastActive) {
  const xy = xyEqual(p, actual), meta = metaEqual(p, actual), buttons = p.buttons === actual.buttons;
  return {full: xy && meta && buttons, xy, meta, buttons, nonXY: meta && buttons,
    wire: encode(p) === actual.rawHex,
    stale: actual.analogMode === 0 && p.analogMode !== 0,
    missing: actual.analogMode !== 0 && p.analogMode === 0,
    modeMismatch: p.analogMode !== actual.analogMode,
    directError: p.analogMode === 2 && actual.analogMode === 2 ? distance(p, actual) : null,
    joystickError: p.analogMode === 1 && actual.analogMode === 1 ? distance(p, actual) : null,
    staleOffset: actual.analogMode === 0 && p.analogMode === 2 && lastActive?.analogMode === 2 ? distance(p, lastActive) : null};
}
function addModel(b, s) {
  b.count++; b.fullNumeric += +s.full; b.coordinatesNumeric += +s.xy; b.wire12B += +s.wire;
  b.metadata += +s.meta; b.buttons += +s.buttons; b.coordinatePerfectFullFieldCeiling += +s.nonXY;
  b.onlyCoordinatesMismatch += +(s.nonXY && !s.xy); b.onlyNonXYMismatch += +(s.xy && !s.nonXY);
  b.bothMismatch += +(!s.xy && !s.nonXY); b.staleAnalogOnNone += +s.stale;
  b.missingAnalogOnActive += +s.missing; b.analogModeMismatch += +s.modeMismatch;
  if (s.directError !== null) b.directTargetErrors.push(s.directError);
  if (s.joystickError !== null) b.joystickVectorErrors.push(s.joystickError);
  if (s.staleOffset !== null) b.staleDirectOffsets.push(s.staleOffset);
}
function addPair(b, decision, held, cv) {
  b.count++; b.cvUsed += +decision.usedCV; b.predictionCoordinatesChanged += +!xyEqual(decision.held, decision.cv); b.reasons[decision.reason] = (b.reasons[decision.reason] ?? 0) + 1;
  b.fullImproved += +(!held.full && cv.full); b.fullRegressed += +(held.full && !cv.full);
  b.bothFullHit += +(held.full && cv.full); b.bothFullMiss += +(!held.full && !cv.full);
  addModel(b.held, held); addModel(b.cv, cv);
}
function finishModel(b) {
  const {directTargetErrors, joystickVectorErrors, staleDirectOffsets, ...counts} = b;
  const rate = key => b.count ? b[key] / b.count : null;
  return {...counts, rates: Object.fromEntries(['fullNumeric', 'coordinatesNumeric', 'wire12B',
    'metadata', 'buttons', 'coordinatePerfectFullFieldCeiling'].map(k => [k, rate(k)])),
    directTargetError: distribution(directTargetErrors), joystickVectorError: distribution(joystickVectorErrors),
    staleDirectTargetOffset: distribution(staleDirectOffsets)};
}
function finishPair(b) {
  return {...b, netFullHits: b.fullImproved - b.fullRegressed, held: finishModel(b.held), cv: finishModel(b.cv)};
}
function wire(v) { return v ? {frame: v.frame, ...fields(v), rawHex: v.rawHex ?? encode(v)} : null; }
function keepWorst(list, rank, example) {
  if (!finite(rank)) return;
  if (list.length < 3 || rank > list.at(-1).rank) {
    list.push({rank, ...example}); list.sort((a, b) => b.rank - a.rank || a.frame - b.frame); list.length = Math.min(3, list.length);
  }
}

function sourceLabels(kind, descriptor, rows, delay) {
  const labels = new Array(rows.length).fill('unavailable'), errors = [], events = descriptor.events;
  if (!array(events)) return {labels, available: false, errors: ['Source event array unavailable']};
  if (kind === 'controlled') {
    if (!integer(delay) || delay < 0 || !events.every((e, i) => e && integer(e.captureSlot) &&
        typeof e.kind === 'string' && (!i || e.captureSlot >= events[i - 1].captureSlot)))
      return {labels, available: false, errors: ['Unsupported controlled capture-slot event trace']};
    let cursor = 0, label = 'pre-gesture';
    rows.forEach((r, i) => {
      while (cursor < events.length && events[cursor].captureSlot <= r.frame - delay) {
        const e = events[cursor++]; if (['pointer', 'stick', 'cancel', 'marker'].includes(e.kind)) label = e.label ?? e.kind;
      }
      labels[i] = r.frame < delay ? 'input-delay-padding' : String(label);
    });
  } else {
    const bySequence = new Map();
    for (const e of events) {
      if (!e || !integer(e.sequence) || bySequence.has(e.sequence)) {errors.push('Invalid/duplicate source event sequence'); continue;}
      bySequence.set(e.sequence, e);
    }
    rows.forEach((r, i) => {
      if (r.latestMoveSequence === null) {labels[i] = 'pre-gesture'; return;}
      const e = bySequence.get(r.latestMoveSequence);
      if (!e || !finite(e.submittedAtMs)) {errors.push('Unresolved submitted movement label at wire frame ' + r.frame); return;}
      labels[i] = String(e.label ?? e.kind);
    });
  }
  return {labels, available: errors.length === 0, errors,
    method: kind === 'controlled' ? 'Submitted capture-slot event <= wire frame minus declared input delay.' :
      'Recorded latestMoveSequence at first wire observation, including markers. Conditional source label, not proof of capture-time causality.'};
}

function classify(rows, index, lastNeutralFrame) {
  const a = rows[index], p = rows[index - 1], pp = rows[index - 2], tags = [];
  if (!p) tags.push('first-frame');
  else {
    if (a.analogMode !== p.analogMode) tags.push('wire-mode-edge');
    if (a.flags !== p.flags) tags.push('wire-flags-edge');
    if ((a.buttons ^ p.buttons) & 4) tags.push('wire-focus-edge');
    if (a.buttons !== p.buttons) tags.push('wire-buttons-edge');
    if (a.analogMode === 0 && p.analogMode !== 0) {tags.push('wire-neutral-edge'); lastNeutralFrame = index;}
    if (a.analogMode !== 0 && p.analogMode === 0) tags.push('wire-analog-entry');
    if (a.analogMode !== 0 && metaEqual(a, p)) {
      tags.push(xyEqual(a, p) ? 'same-active-coordinates' : 'changed-active-coordinates');
      if (pp && metaEqual(p, pp)) {
        const vx = a.x - p.x, vy = a.y - p.y, ux = p.x - pp.x, uy = p.y - pp.y;
        if ((vx || vy) && (ux || uy) && vx * ux + vy * uy <= 0) tags.push('wire-nonforward-turn');
        if (vx !== ux || vy !== uy) tags.push('wire-step-change');
      }
    }
  }
  if (a.analogMode === 0 && lastNeutralFrame !== null && index - lastNeutralFrame < 8)
    tags.push('within-8-frames-of-wire-neutral');
  return {tags, lastNeutralFrame};
}

function analyzeTape(rows, labels, limit) {
  const out = [];
  for (const h of HORIZONS) {
    const all = pairBucket(), groups = new Map(), digest = createHash('sha256');
    const worst = {directCVError: [], directErrorIncrease: [], staleDirectCVOffset: [], fullHitRegressions: []};
    let lastActive = null, lastNeutralFrame = null;
    for (let frame = 0; frame < limit; frame++) {
      const cutoff = frame - h, anchor = cutoff >= 0 ? rows[cutoff] : null;
      const previous = cutoff > 0 ? rows[cutoff - 1] : null;
      const decision = predictPrefix(frame, anchor, previous);
      // Prediction is complete. The scorer may now read target input and post-hoc labels.
      const actual = rows[frame], held = scoreModel(decision.held, actual, lastActive), cv = scoreModel(decision.cv, actual, lastActive);
      if (!validInput(decision.held) || !validInput(decision.cv) || !nonXYEqual(decision.held, decision.cv))
        throw Error('Prediction changed non-XY contract or emitted invalid input at ' + frame + '/' + h);
      if (anchor?.analogMode === 1 && !fullEqual(decision.held, decision.cv))
        throw Error('Joystick prediction changed');
      const classified = classify(rows, frame, lastNeutralFrame); lastNeutralFrame = classified.lastNeutralFrame;
      const keys = ['mode:' + MODES[actual.analogMode], 'source:' + labels[frame] + '/mode:' + MODES[actual.analogMode],
        ...classified.tags.map(t => 'edge:' + t)];
      addPair(all, decision, held, cv);
      for (const key of keys) {
        if (!groups.has(key)) groups.set(key, pairBucket());
        addPair(groups.get(key), decision, held, cv);
      }
      const example = {frame, visibleThrough: cutoff < 0 ? null : cutoff, sourceLabel: labels[frame], tags: classified.tags,
        previous: wire(previous), anchor: wire(anchor), actual: wire(actual), held: wire(decision.held), cv: wire(decision.cv),
        reason: decision.reason, heldFullHit: held.full, cvFullHit: cv.full,
        heldDirectError: held.directError, cvDirectError: cv.directError,
        heldStaleOffset: held.staleOffset, cvStaleOffset: cv.staleOffset, lastActiveActual: wire(lastActive)};
      if (cv.directError !== null) {
        keepWorst(worst.directCVError, cv.directError, example);
        if (cv.directError > held.directError) keepWorst(worst.directErrorIncrease, cv.directError - held.directError, example);
      }
      if (cv.staleOffset !== null) keepWorst(worst.staleDirectCVOffset, cv.staleOffset, example);
      if (held.full && !cv.full) keepWorst(worst.fullHitRegressions, cv.directError ?? cv.staleOffset ?? 0, example);
      digest.update(json([frame, cutoff, encode(decision.held), encode(decision.cv), actual.rawHex, decision.reason]) + '\n');
      if (actual.analogMode !== 0) lastActive = actual;
    }
    const checks = {allTargets: all.count === limit,
      pairPartition: all.fullImproved + all.fullRegressed + all.bothFullHit + all.bothFullMiss === limit,
      modePartition: [...groups].filter(([k]) => k.startsWith('mode:')).reduce((n, [, b]) => n + b.count, 0) === limit,
      sourcePartition: [...groups].filter(([k]) => k.startsWith('source:')).reduce((n, [, b]) => n + b.count, 0) === limit,
      nonXYCeilingUnchanged: all.held.coordinatePerfectFullFieldCeiling === all.cv.coordinatePerfectFullFieldCeiling};
    if (!Object.values(checks).every(Boolean)) throw Error('Score conservation failed at H=' + h);
    out.push({horizon: h, checks, predictionAndActualStreamSha256: digest.digest('hex'),
      streamColumns: ['frame', 'visibleCutoff', 'held12BHex', 'cv12BHex', 'actual12BHex', 'reason'],
      overall: finishPair(all), groups: Object.fromEntries([...groups].map(([k, b]) => [k, finishPair(b)])), worst});
  }
  return out;
}

export function selfChecks() {
  const checks = [], check = (name, ok) => checks.push({name, passed: ok === true});
  const a = {frame: 10, buttons: 0xffff, analogMode: 2, flags: 2, x: 10, y: -4};
  const p = {...a, frame: 9, x: 9.5, y: -3.75};
  const near = predictPrefix(13, a, p), far = predictPrefix(18, a, p);
  check('held-mask-at-3', near.held.buttons === 0xf5 && near.held.x === 10);
  check('directions-expire-while-analog-remains', far.held.buttons === 5 && far.held.analogMode === 2 && far.held.x === 10);
  check('float32-two-point-at-H8', far.cv.x === 14 && far.cv.y === -6 && far.usedCV);
  check('nonXY-contract-preserved', nonXYEqual(far.held, far.cv));
  check('no-anchor-neutral', fullEqual(predictPrefix(0, null, null).cv, neutral()));
  check('no-previous-falls-back', predictPrefix(11, a, null).reason === 'insufficient-history');
  check('joystick-is-held', !predictPrefix(11, {...a, analogMode: 1}, {...p, analogMode: 1}).usedCV);
  check('focus-edge-falls-back', predictPrefix(11, a, {...p, buttons: p.buttons ^ 4}).reason === 'metadata-edge');
  check('flags-edge-falls-back', predictPrefix(11, a, {...p, flags: 0}).reason === 'metadata-edge');
  check('unlimited-is-held', predictPrefix(11, {...a, flags: 3}, {...p, flags: 3}).reason === 'unlimited-held');
  check('coordinate-repeat-zero-step', predictPrefix(11, a, {...p, x: a.x, y: a.y}).reason === 'cv-zero-step');
  check('input-bound-falls-back', predictPrefix(18, {...a, x: 4096}, {...p, x: 0}).reason === 'invalid-extrapolation');
  const z = neutral(), minus = {...z, x: -0};
  check('signed-zero-numeric-not-wire', fullEqual(z, minus) && encode(z) !== encode(minus));
  const oppositeShot = {...far.cv, buttons: far.cv.buttons ^ 1};
  check('perfect-XY-does-not-fix-shot', xyEqual(far.cv, oppositeShot) && !nonXYEqual(far.cv, oppositeShot));
  const release = {...neutral(), rawHex: encode(neutral())}, releaseScore = scoreModel(far.cv, release, a);
  check('unseen-release-does-not-steer-prediction', far.cv.analogMode === 2 && releaseScore.stale &&
    releaseScore.directError === null && releaseScore.staleOffset > 0);
  return {passed: checks.every(c => c.passed), checks,
    scope: 'Pure input invariants only; no native compilation, world-state equivalence, Replay or performance proof.'};
}

function readFile(path, parse = false) {
  let bytes;
  try {bytes = readFileSync(path); return {path, bytes: bytes.length, sha256: sha(bytes), value: parse ? JSON.parse(bytes) : undefined};}
  catch (error) {return {path, bytes: bytes?.length ?? null, sha256: bytes ? sha(bytes) : null, error: String(error)};}
}
const identity = ({value, ...rest}) => rest;
function validateTape(rawRows, limit, expectedSha) {
  const errors = [], decoded = [], invalidRows = [];
  if (!array(rawRows) || rawRows.length > MAX_ROWS) return {errors: ['Missing tape or row capacity exceeded'], decoded,
    retainedRows: array(rawRows) ? rawRows.length : null, maximumRows: MAX_ROWS};
  if (rawRows.length < limit) errors.push('Incomplete [0,limit) tape');
  for (let i = 0; i < rawRows.length; i++) {
    const r = rawRows[i]; let reason = null, v;
    if (!r || r.frame !== i || !/^[0-9a-f]{24}$/i.test(r.rawHex ?? '')) reason = 'Frame/order/12B hex';
    else {
      const b = Buffer.from(r.rawHex, 'hex');
      v = {frame: i, buttons: b.readUInt16LE(0), analogMode: b[2], flags: b[3], x: b.readFloatLE(4), y: b.readFloatLE(8), rawHex: b.toString('hex')};
      if (r.buttons !== v.buttons || r.analogMode !== v.analogMode || r.flags !== v.flags || r.x !== v.x || r.y !== v.y ||
          r.xBits !== b.readUInt32LE(4) || r.yBits !== b.readUInt32LE(8) ||
          (r.word0 !== undefined && r.word0 !== b.readUInt32LE(0))) reason = 'Decoded full-field mismatch';
      else if (!validInput(v)) reason = 'TH09 input validity';
    }
    if (reason) invalidRows.push({index: i, reason});
    decoded.push(v);
  }
  if (invalidRows.length) errors.push('Invalid tape rows: ' + invalidRows.length);
  const prefixSha = sha(json(rawRows.slice(0, limit).map(r => [r?.frame, r?.rawHex])));
  if (typeof expectedSha !== 'string' || prefixSha !== expectedSha) errors.push('Recorded tape prefix SHA mismatch/unavailable');
  return {errors, decoded, retainedRows: rawRows.length, scoredRange: [0, limit], tailRows: Math.max(0, rawRows.length - limit),
    prefixSha256: prefixSha, recordedPrefixSha256: expectedSha ?? null,
    allRowsSha256: sha(json(rawRows.map(r => [r?.frame, r?.rawHex]))), invalidRows};
}

function analyzeRun(path) {
  const input = readFile(path, true), raw = input.value, errors = [], peers = [];
  const result = {input: {before: identity(input)}, errors, peers};
  if (input.error) {errors.push(input.error); return result;}
  const limit = raw?.parameters?.limit;
  result.recorded = {passed: raw?.passed, scope: raw?.scope, parameters: raw?.parameters,
    identity: raw?.identity, identityAfter: raw?.identityAfter, error: raw?.error, errors: raw?.errors};
  if (raw?.passed !== true) errors.push('Source raw did not declare PASS; retained for diagnosis');
  if (!array(raw?.errors) || raw.errors.length) errors.push('Source raw errors unavailable/nonempty');
  if (!raw?.identity || typeof raw.identity !== 'object') errors.push('Recorded source identity unavailable');
  if (!integer(limit) || limit < 1 || limit > MAX_ROWS) {errors.push('Invalid raw.parameters.limit'); return result;}
  const controlled = array(raw?.result?.localTape), release = array(raw?.results);
  if (controlled === release) {errors.push('Expected exactly one supported full raw schema'); return result;}
  result.layout = controlled ? 'controlled/result.localTape' : 'release/results[].touchInput.captures';
  if (controlled && (!raw.identityAfter || !same(raw.identity, raw.identityAfter))) errors.push('Controlled source identity changed/unavailable');
  const descriptors = controlled ? raw.result.localTape.map((rows, side) => ({rows, events: raw.result.events?.[side],
    expectedSha: raw.localTapeSha256?.[side]})) : raw.results.map(p => ({rows: p?.touchInput?.captures,
    events: p?.touchInput?.events, expectedSha: p?.touchInputTapeSha256, probe: p?.touchInput}));
  if (descriptors.length !== 2) errors.push('Expected exactly two peers; every retained peer is still listed');
  descriptors.forEach((d, side) => {
    const validated = validateTape(d.rows, limit, d.expectedSha), {decoded, ...coverage} = validated;
    const peer = {side, coverage, labelCoverage: null, horizons: null}; peers.push(peer);
    if (!controlled) {
      coverage.probe = {valid: d.probe?.valid, errors: d.probe?.errors, overflow: d.probe?.overflow,
        changedHistory: d.probe?.counts?.changedHistory, captureGaps: d.probe?.counts?.captureGaps};
      if (d.probe?.valid !== true || !array(d.probe?.errors) || d.probe.errors.length ||
          !d.probe?.overflow || Object.values(d.probe.overflow).some(Boolean) ||
          d.probe?.counts?.changedHistory !== 0 || d.probe?.counts?.captureGaps !== 0)
        coverage.errors.push('Release touch probe invalid or incomplete');
    }
    if (coverage.errors.length) {errors.push('Peer ' + side + ' tape coverage failed'); return;}
    const labels = sourceLabels(controlled ? 'controlled' : 'release', d, d.rows, raw.parameters.delay);
    peer.labelCoverage = {available: labels.available, errors: labels.errors, method: labels.method,
      range: [0, limit], labels: [...new Set(labels.labels.slice(0, limit))],
      note: 'Scoring strata only. Unavailable labels never remove target frames or change predictions.'};
    try {peer.horizons = analyzeTape(decoded, labels.labels, limit);}
    catch (error) {peer.analysisError = String(error); errors.push('Peer ' + side + ' analysis failed: ' + error);}
  });
  return result;
}

function main() {
  const [outputArg, ...inputArgs] = process.argv.slice(2);
  if (!outputArg || !inputArgs.length) throw Error('Usage: node mobile-prefix-prediction-shadow.mjs NEW_OUTPUT.json RAW.json [RAW.json ...]');
  const output = resolve(outputArg), paths = inputArgs.map(p => resolve(p)), self = fileURLToPath(import.meta.url);
  if ([self, ...paths].some(p => p.toLowerCase() === output.toLowerCase()) || existsSync(output))
    throw Error('Output must be a new path, distinct from inputs and analyzer');
  const before = readFile(self), checks = selfChecks(), errors = [], runs = [];
  if (before.error) errors.push('Analyzer identity unavailable: ' + before.error);
  if (!checks.passed) errors.push('Pure-input self-check failed');
  for (const path of paths) {
    try {runs.push(analyzeRun(path));}
    catch (error) {runs.push({input: {before: identity(readFile(path))}, errors: [String(error)], peers: []});}
  }
  for (const run of runs) {
    run.input.after = identity(readFile(run.input.before.path));
    run.input.stable = !!run.input.before.sha256 && run.input.before.sha256 === run.input.after.sha256 &&
      run.input.before.bytes === run.input.after.bytes;
    if (!run.input.stable) run.errors.push('Input file identity changed/unavailable');
  }
  const after = readFile(self), stable = !!before.sha256 && before.sha256 === after.sha256 && before.bytes === after.bytes;
  if (!stable) errors.push('Analyzer identity changed/unavailable');
  const analysisPassed = !errors.length && runs.every(r => !r.errors.length && r.peers.length === 2 && r.peers.every(p => p.horizons?.length === 8));
  const evidence = {schema: 'th09-mobile-prefix-prediction-shadow/v1', createdAt: new Date().toISOString(),
    analysisPassed, successMeaning: 'Source/tape validation and arithmetic/coverage checks only.',
    scope: 'Open-loop fixed-prefix prediction opportunity screen on retained actual wire tapes. Not real arrival reconstruction or a gameplay performance/correctness result.',
    limitations: [
      'H is a hypothetical newest-actual age, not measured rollback depth or network delay. All target frames, including startup and neutral input, are scored.',
      'No correction/resimulation count or CPU/FPS estimate. Rewinds can change previously used predictions, including turning an initial hit into a later miss.',
      'Actual touch capture can depend on player state, focus, clipping and scheduling. A live predictor change can produce a different future actual tape.',
      'None means disabled movement, not a target at (0,0). Target and velocity errors have separate populations; release offset is only an input-space proxy.',
      'Full-field equality remains exact. No real input is quantized, resampled, coalesced, changed or written. Joystick and button prediction retain held behavior.',
      'CV does not know future turns, releases or source labels. It has no physical field boundary or hidden gesture epoch and is not an approved production policy.',
      'Recorded raw/source identities are retained claims; this script only rereads its own source and the explicit raw files, not old build inputs.',
    ], parameters, analyzer: {before: identity(before), after: identity(after), stable},
    selfChecks: checks, errors, runs};
  const text = json(evidence) + '\n'; writeFileSync(output, text, {flag: 'wx'});
  const summary = {output, bytes: Buffer.byteLength(text), sha256: sha(text), analysisPassed,
    selfChecksPassed: checks.passed, runCount: runs.length, errorCount: errors.length + runs.reduce((n, r) => n + r.errors.length, 0)};
  const detail = json({...summary, runs: runs.map(r => ({path: r.input.before.path, stable: r.input.stable,
    errors: r.errors.length, peers: r.peers.map(p => ({side: p.side, range: p.coverage.scoredRange,
      labelsAvailable: p.labelCoverage?.available, H8: p.horizons?.at(-1)?.overall && {
        heldFull: p.horizons.at(-1).overall.held.fullNumeric, cvFull: p.horizons.at(-1).overall.cv.fullNumeric,
        ceiling: p.horizons.at(-1).overall.held.coordinatePerfectFullFieldCeiling}}))}))});
  console.log(Buffer.byteLength(detail) <= 7500 ? detail : json({...summary, note: 'Full run summaries retained in output.'}));
  if (!analysisPassed) process.exitCode = 1;
}
if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) main();
