const SCHEMA_V1 = 'presentation-lab/human-visual-review/1';
const SCHEMA = 'presentation-lab/human-visual-review/2';
const SCOPE = 'human-visual-review';
const VERDICTS = new Set(['passed', 'failed', 'inconclusive']);
const SELECTABLE_CHECKS = new Set(['DISPLAY-02', 'DISPLAY-04', 'DISPLAY-05', 'DISPLAY-06', 'DISPLAY-08']);
const SHA256 = /^[a-f0-9]{64}$/;
const PNG_SIGNATURE = [137, 80, 78, 71, 13, 10, 26, 10];
const PNG_IEND = [0, 0, 0, 0, 73, 69, 78, 68, 174, 66, 96, 130];

const isObject = value => value !== null && typeof value === 'object' && !Array.isArray(value);
const nonempty = value => typeof value === 'string' && value.trim().length > 0;
const clone = value => value === undefined ? null : JSON.parse(JSON.stringify(value));
const equal = (left, right) => canonical(left) === canonical(right);

function canonical(value) {
  if (Array.isArray(value)) return `[${value.map(canonical).join(',')}]`;
  if (isObject(value)) return `{${Object.keys(value).sort().map(key => `${JSON.stringify(key)}:${canonical(value[key])}`).join(',')}}`;
  return JSON.stringify(value);
}

async function digest(bytes) {
  const subtle = globalThis.crypto?.subtle;
  if (!subtle || typeof TextEncoder !== 'function') throw Error('WebCrypto SHA-256 is required');
  const output = new Uint8Array(await subtle.digest('SHA-256', bytes));
  return Array.from(output, byte => byte.toString(16).padStart(2, '0')).join('');
}

function parseReport(reportText) {
  if (typeof reportText !== 'string') throw Error('reportText must be the exact UTF-8 report text');
  try { return {value: JSON.parse(reportText), parseError: null}; }
  catch (error) { return {value: null, parseError: `Report JSON is invalid: ${error.message}`}; }
}

function inputIdentity(value, label, issues, {nullable = false} = {}) {
  if (nullable && value === null) return null;
  if (!isObject(value)) { issues.push(`report.build.inputs.${label} is missing`); return null; }
  const result = {bytes: Number.isSafeInteger(value.bytes) && value.bytes > 0 ? value.bytes : null,
    sha256: typeof value.sha256 === 'string' && SHA256.test(value.sha256) ? value.sha256 : null};
  if (result.bytes === null) issues.push(`report.build.inputs.${label}.bytes is missing or invalid`);
  if (result.sha256 === null) issues.push(`report.build.inputs.${label}.sha256 is missing or invalid`);
  return result;
}

function extractIdentity(report) {
  const issues = [], build = report?.build;
  if (!isObject(build)) {
    issues.push('report.build identity is missing');
    return {identity: null, issues};
  }
  const buildFields = ['schema', 'commit', 'wasm', 'loader', 'commonCommit', 'sourceDigest'];
  const buildIdentity = {};
  for (const field of buildFields) {
    const value = nonempty(build[field]) ? build[field] : null;
    buildIdentity[field] = value;
    if (value === null) issues.push(`report.build.${field} is missing`);
  }
  for (const field of ['wasm', 'loader', 'sourceDigest']) {
    if (buildIdentity[field] !== null && !SHA256.test(buildIdentity[field])) {
      issues.push(`report.build.${field} is not a SHA-256 identity`);
      buildIdentity[field] = null;
    }
  }
  for (const field of ['commit', 'commonCommit']) {
    if (buildIdentity[field] !== null && !/^[a-f0-9]{40,64}$/i.test(buildIdentity[field])) {
      issues.push(`report.build.${field} is not a commit identity`);
      buildIdentity[field] = null;
    }
  }

  const inputs = isObject(build.inputs) ? build.inputs : {};
  if (!isObject(build.inputs)) issues.push('report.build.inputs is missing');
  const data = inputIdentity(inputs.data, 'data', issues);
  const replayProvided = Object.hasOwn(inputs, 'replay');
  if (!replayProvided) issues.push('report.build.inputs.replay must be present as an identity or null');
  const replay = replayProvided ? inputIdentity(inputs.replay, 'replay', issues, {nullable: true}) : null;
  const game = nonempty(build.game) ? build.game : null;
  if (game === null) issues.push('report.build.game is missing');
  const nativeAbi = nonempty(build.nativeAbi) ? build.nativeAbi : null;
  if (nativeAbi === null) issues.push('report.build.nativeAbi is missing');
  const adapterVersion = nonempty(build.adapterVersion) ? build.adapterVersion : null;
  if (adapterVersion === null) issues.push('report.build.adapterVersion is missing');
  const toolDigest = typeof build.toolDigest === 'string' && SHA256.test(build.toolDigest) ? build.toolDigest : null;
  if (toolDigest === null) issues.push('report.build.toolDigest is missing or invalid');
  const toolSources = isObject(build.toolSources) && Object.keys(build.toolSources).length > 0
    ? Object.fromEntries(Object.entries(build.toolSources).sort(([a], [b]) => a.localeCompare(b))) : null;
  if (toolSources === null) issues.push('report.build.toolSources is missing');
  else for (const [name, hash] of Object.entries(toolSources)) if (!nonempty(name) || typeof hash !== 'string' || !SHA256.test(hash)) {
    issues.push(`report.build.toolSources.${name || '?'} is missing or invalid`);
  }

  return {identity: {game, build: buildIdentity,
    tool: {sha256: toolDigest, sources: toolSources}, nativeAbi, adapterVersion,
    inputs: {data, replay, replayProvided}}, issues};
}

function imageIdentities(report, issues) {
  if (!Array.isArray(report?.images)) {
    issues.push('report.images is missing');
    return [];
  }
  const results = [];
  for (let index = 0; index < report.images.length; index++) {
    const image = report.images[index];
    if (!isObject(image) || !Number.isFinite(image.alpha) || image.alpha < 0 || image.alpha > 1 || typeof image.png !== 'string') {
      issues.push(`report.images[${index}] is missing a valid alpha or PNG`);
      continue;
    }
    const match = /^data:image\/png;base64,([A-Za-z0-9+/]+={0,2})$/.exec(image.png);
    if (!match) { issues.push(`report.images[${index}] is not a base64 PNG`); continue; }
    let binary;
    try { binary = atob(match[1]); }
    catch { issues.push(`report.images[${index}] has invalid base64`); continue; }
    const bytes = Uint8Array.from(binary, character => character.charCodeAt(0));
    const readU32 = offset => (bytes[offset] * 0x1000000 + (bytes[offset + 1] << 16) + (bytes[offset + 2] << 8) + bytes[offset + 3]) >>> 0;
    let offset = 8, firstChunk = true, hasPixelData = false, ended = false, complete = PNG_SIGNATURE.every((byte, at) => bytes[at] === byte);
    if (bytes.length < 45) complete = false;
    while (complete && offset + 12 <= bytes.length) {
      const length = readU32(offset), type = String.fromCharCode(...bytes.slice(offset + 4, offset + 8));
      if (length > bytes.length - offset - 12) { complete = false; break; }
      if (firstChunk && (type !== 'IHDR' || length !== 13 || readU32(offset + 8) === 0 || readU32(offset + 12) === 0)) {
        complete = false; break;
      }
      firstChunk = false;
      if (type === 'IDAT' && length > 0) hasPixelData = true;
      offset += length + 12;
      if (type === 'IEND') {
        ended = true;
        complete = length === 0 && offset === bytes.length && hasPixelData &&
          PNG_IEND.every((byte, at) => bytes[bytes.length - PNG_IEND.length + at] === byte);
        break;
      }
    }
    if (!complete || !ended) {
      issues.push(`report.images[${index}] is not a complete PNG image`);
      continue;
    }
    const boxes = Array.isArray(image.boxes) ? image.boxes : [];
    results.push({index, alpha: image.alpha, bytes: bytes.byteLength, sha256: null,
      boxes: boxes.filter(box => isObject(box) && nonempty(box.key) && Array.isArray(box.bounds) && box.bounds.length === 4 && box.bounds.every(Number.isFinite))
        .map(box => ({key: box.key, bounds: box.bounds}))});
  }
  return results;
}

async function identifyImages(report, issues) {
  const preliminary = imageIdentities(report, issues);
  for (const image of preliminary) {
    const source = report.images[image.index].png.slice('data:image/png;base64,'.length);
    const binary = atob(source);
    const bytes = Uint8Array.from(binary, character => character.charCodeAt(0));
    image.sha256 = await digest(bytes);
  }
  return preliminary;
}

function objectIdentity(report, imageIds, issues) {
  if (!Array.isArray(report?.objects)) {
    issues.push('report.objects is missing');
    return {count: 0, keys: [], matchedImageObject: null};
  }
  const keys = report.objects.filter(object => isObject(object) && nonempty(object.key) && Number.isInteger(object.ownerId) && object.ownerId > 0)
    .map(object => object.key);
  if (!keys.length) issues.push('report.objects has no identified runtime objects');
  const known = new Set(keys);
  const matchedImageObject = imageIds.flatMap(image => image.boxes.map(box => box.key)).find(key => known.has(key)) ?? null;
  if (!matchedImageObject) issues.push('report images do not identify an object from report.objects');
  return {count: keys.length, keys: [...new Set(keys)].sort(), matchedImageObject};
}

function normalizedEnvironment(value, issues) {
  const displayModel = nonempty(value?.displayModel) ? value.displayModel.trim() : null;
  if (displayModel === null) issues.push('environment.displayModel is missing');
  const refreshValue = typeof value?.refreshHz === 'number' ? value.refreshHz : value?.refreshHz?.value;
  const refreshSource = typeof value?.refreshHz === 'number' ? 'human-reported' : value?.refreshHz?.source;
  const refreshHz = Number.isFinite(refreshValue) && refreshValue > 0 && refreshValue <= 1000 && refreshSource === 'human-reported'
    ? {value: refreshValue, unit: 'Hz', source: 'human-reported'} : null;
  if (refreshHz === null) issues.push('environment.refreshHz is missing or invalid');
  const browser = {name: nonempty(value?.browser?.name) ? value.browser.name.trim() : null,
    version: nonempty(value?.browser?.version) ? value.browser.version.trim() : null};
  const os = {name: nonempty(value?.os?.name) ? value.os.name.trim() : null,
    version: nonempty(value?.os?.version) ? value.os.version.trim() : null};
  for (const [label, info] of [['browser', browser], ['os', os]]) for (const field of ['name', 'version']) {
    if (info[field] === null) issues.push(`environment.${label}.${field} is missing`);
  }
  return {displayModel, refreshHz, browser, os};
}

function normalizedReviewer(value) {
  if (!isObject(value) || !nonempty(value.name) || value.kind !== 'human')
    throw Error('reviewer must explicitly identify a named reviewer with kind:"human"');
  return {name: value.name.trim(), kind: 'human'};
}

function normalizedChecks(value, issues) {
  if (!Array.isArray(value)) { issues.push('selectedChecks must be an explicit array'); return []; }
  const checks = [];
  for (const item of value) {
    if (!SELECTABLE_CHECKS.has(item)) throw Error(`selectedChecks contains unsupported visual check: ${String(item)}`);
    if (checks.includes(item)) throw Error(`selectedChecks contains a duplicate: ${item}`);
    checks.push(item);
  }
  if (!checks.length) issues.push('selectedChecks is empty');
  return checks;
}

function normalizedObservations(value, selectedChecks, issues) {
  if (!Array.isArray(value)) {
    issues.push('observations must be an explicit array');
    value = [];
  }
  const rows = new Map();
  for (const item of value) {
    if (!isObject(item) || !SELECTABLE_CHECKS.has(item.id) || !selectedChecks.includes(item.id))
      throw Error('Each observation must belong to a selected visual check');
    if (rows.has(item.id)) throw Error(`observations contains duplicate row ${item.id}`);
    const row = {id: item.id};
    for (const field of ['operation', 'expected', 'actual']) {
      row[field] = nonempty(item[field]) ? item[field].trim() : '';
      if (!row[field]) issues.push(`observations.${item.id}.${field} is empty`);
    }
    rows.set(item.id, row);
  }
  return selectedChecks.flatMap(id => {
    if (!rows.has(id)) {
      issues.push(`observations is missing row ${id}`);
      return [];
    }
    return [rows.get(id)];
  });
}

function normalizedScene(value, report, issues) {
  const label = nonempty(value?.label) ? value.label.trim() : null;
  if (label === null) issues.push('scene.label is missing');
  const reportScene = isObject(report?.scene) ? clone(report.scene) : null;
  if (reportScene === null) issues.push('report.scene snapshot is missing');
  return {label, report: reportScene};
}

function normalizedTickRange(value, report, issues) {
  const actualFrom = report?.previousTick, actualTo = report?.tick;
  if (!Number.isInteger(actualFrom) || !Number.isInteger(actualTo) || actualTo < actualFrom) {
    issues.push('report scene tick range is missing or invalid');
    return null;
  }
  if (!isObject(value) || !Number.isInteger(value.from) || !Number.isInteger(value.to)) {
    issues.push('tickRange.from and tickRange.to are missing');
    return null;
  }
  if (value.from !== actualFrom || value.to !== actualTo) issues.push('tickRange must exactly match report.previousTick → report.tick');
  return {from: value.from, to: value.to};
}

function reportIssues(report, parseError) {
  const issues = [];
  if (parseError) issues.push(parseError);
  if (!isObject(report)) { issues.push('report must be a JSON object'); return issues; }
  if (!nonempty(report.schema)) issues.push('report.schema is missing');
  if (report.valid !== true) issues.push('report.valid is not true');
  if (report.negativeControl !== false) issues.push('report is missing an explicit negativeControl:false');
  return issues;
}

function reviewDiagnostics({report, parseError, identityIssues, images, objects, environment, scene, tickRange, selectedChecks, observations, notes}) {
  return [...reportIssues(report, parseError), ...identityIssues, ...images.issues, ...objects.issues,
    ...environment.issues, ...scene.issues, ...tickRange.issues, ...selectedChecks.issues, ...observations.issues,
    ...(notes.verdict === 'passed' && !nonempty(notes.value) ? ['notes is empty'] : [])];
}

async function collect(reportText, input) {
  if (typeof reportText !== 'string') throw Error('reportText must be the exact UTF-8 report text');
  const bytes = new TextEncoder().encode(reportText), reportHash = await digest(bytes);
  const parsed = parseReport(reportText), report = parsed.value;
  const identity = extractIdentity(report);
  const imageIssues = [], imageIds = await identifyImages(report, imageIssues);
  const objectIssues = [], objects = objectIdentity(report, imageIds, objectIssues);
  const environmentIssues = [], environment = normalizedEnvironment(input.environment, environmentIssues);
  const sceneIssues = [], scene = normalizedScene(input.scene, report, sceneIssues);
  const tickIssues = [], tickRange = normalizedTickRange(input.tickRange, report, tickIssues);
  const checkIssues = [], selectedChecks = normalizedChecks(input.selectedChecks, checkIssues);
  const observationIssues = [], observations = normalizedObservations(input.observations, selectedChecks, observationIssues);
  const diagnostics = reviewDiagnostics({report, parseError: parsed.parseError, identityIssues: identity.issues,
    images: {issues: imageIssues}, objects: {issues: objectIssues}, environment: {issues: environmentIssues},
    scene: {issues: sceneIssues}, tickRange: {issues: tickIssues}, selectedChecks: {issues: checkIssues},
    observations: {issues: observationIssues}, notes: {verdict: input.verdict, value: input.notes}});
  return {bytes: bytes.byteLength, sha256: reportHash, parsed, identity, imageIds, objects, environment, scene, tickRange,
    selectedChecks, observations, diagnostics};
}

async function validateVisualReviewV1({record, reportText} = {}) {
  if (!isObject(record) || record.schema !== SCHEMA_V1 || record.scope !== SCOPE || record.verdictScope !== 'sampled-window')
    throw Error('Unsupported human visual review record or verdict scope');
  if (!record.claims || record.claims.adaptationComplete !== false || record.claims.automaticPurityOverridden !== false || record.claims.checksComplete !== false)
    throw Error('Human visual review cannot grant adaptation completion or override automatic purity');
  if (!VERDICTS.has(record.verdict)) throw Error('Review verdict is missing or invalid');
  const reviewer = normalizedReviewer(record.reviewer);
  if (!equal(reviewer, record.reviewer)) throw Error('Reviewer fields are not normalized');
  if (typeof record.notes !== 'string') throw Error('Review notes are missing');
  if (typeof record.createdAt !== 'string' || !Number.isFinite(Date.parse(record.createdAt))) throw Error('Review timestamp is missing');

  const evidence = await collect(reportText, {environment: record.environment, scene: record.scene,
    tickRange: record.tickRange, selectedChecks: record.selectedChecks, observations: record.observations,
    verdict: record.verdict, notes: record.notes});
  if (record.report?.bytes !== evidence.bytes || record.report?.sha256 !== evidence.sha256)
    throw Error('Report text does not match the report identity recorded during review');
  if ((record.report.schema ?? null) !== (evidence.parsed.value?.schema ?? null)) throw Error('Report schema does not match the bound report');
  if (!equal(record.identity, evidence.identity.identity)) throw Error('Build or input identities do not match the bound report');
  if (!equal(record.imageEvidence, evidence.imageIds)) throw Error('Image identities do not match the bound report');
  if (!equal(record.objectEvidence, evidence.objects)) throw Error('Object evidence does not match the bound report');
  if (!equal(record.scene, evidence.scene)) throw Error('Scene evidence does not match the bound report');
  if (!equal(record.tickRange, evidence.tickRange)) throw Error('Scene tick range does not match the bound report');
  if (!equal(record.selectedChecks, evidence.selectedChecks)) throw Error('Selected visual checks are invalid');
  if (!equal(record.checks, evidence.selectedChecks.map(id => ({id, complete: false}))) || record.checks.some(check => check.complete !== false))
    throw Error('A sampled window cannot mark an adaptation check complete');
  if (!equal(record.observations, evidence.observations)) throw Error('Human observations were changed or are not readable');
  if (!equal(record.environment, evidence.environment)) throw Error('Review environment is invalid');
  const automaticEvidence = {valid: evidence.parsed.value?.valid === true,
    purityStatus: evidence.parsed.value?.purityStatus ?? evidence.parsed.value?.purity ?? 'unknown',
    negativeControl: evidence.parsed.value?.negativeControl ?? null};
  if (!equal(record.automaticEvidence, automaticEvidence)) throw Error('Automatic report evidence was changed');
  if (!equal(record.diagnostics, evidence.diagnostics)) throw Error('Review diagnostics do not match report and environment fields');
  if (record.verdict === 'passed' && evidence.diagnostics.length)
    throw Error(`Cannot validate passed visual review: ${evidence.diagnostics.join('; ')}`);
  return {valid: true, diagnostics: evidence.diagnostics};
}

async function collectMaterial(reportText, filename, issues) {
  if (!nonempty(filename)) issues.push('material filename is missing');
  if (typeof reportText !== 'string') {
    issues.push(`material ${filename || '?'} has no exact UTF-8 report text`);
    return {filename: nonempty(filename) ? filename.trim() : null, report: null, identity: null,
      imageEvidence: [], objectEvidence: null, scene: null, tickRange: null, automaticEvidence: null};
  }
  const bytes = new TextEncoder().encode(reportText);
  const parsed = parseReport(reportText), report = parsed.value;
  const identity = extractIdentity(report), imagesIssues = [], imageEvidence = await identifyImages(report, imagesIssues);
  const objectsIssues = [], objectEvidence = objectIdentity(report, imageEvidence, objectsIssues);
  const materialIssues = [...reportIssues(report, parsed.parseError), ...identity.issues, ...imagesIssues, ...objectsIssues];
  const previousTick = report?.previousTick, tick = report?.tick;
  const tickRange = Number.isInteger(previousTick) && Number.isInteger(tick) && tick >= previousTick
    ? {from: previousTick, to: tick} : null;
  const scene = isObject(report?.scene) ? clone(report.scene) : null;
  if (!scene) materialIssues.push('report.scene snapshot is missing');
  if (!tickRange) materialIssues.push('report scene tick range is missing or invalid');
  issues.push(...materialIssues.map(issue => `${filename || 'material'}: ${issue}`));
  return {filename: nonempty(filename) ? filename.trim() : null,
    report: {schema: report?.schema ?? null, bytes: bytes.byteLength, sha256: null, reportText},
    identity: identity.identity, imageEvidence, objectEvidence, scene, tickRange,
    automaticEvidence: {valid: report?.valid === true,
      purityStatus: report?.purityStatus ?? report?.purity ?? 'unknown', negativeControl: report?.negativeControl ?? null},
    issues: materialIssues};
}

async function digestReportText(text) {
  return digest(new TextEncoder().encode(text));
}

async function collectSessionMaterials(materials, input) {
  const issues = [];
  if (!Array.isArray(materials)) {
    issues.push('materials must be an explicit array');
    materials = [];
  }
  if (!materials.length) issues.push('at least one scene report is required');
  const rows = [];
  for (const material of materials) {
    if (!isObject(material)) {
      issues.push('each material must include a report and filename');
      continue;
    }
    const row = await collectMaterial(material.reportText, material.filename, issues);
    if (typeof material.reportText === 'string') row.report.sha256 = await digestReportText(material.reportText);
    delete row.issues;
    rows.push(row);
  }
  const fingerprints = rows.map(row => row.report?.sha256).filter(Boolean);
  if (new Set(fingerprints).size !== fingerprints.length) issues.push('the material set contains a duplicate report');
  const firstIdentity = rows.find(row => row.identity)?.identity ?? null;
  if (rows.some(row => row.identity && !equal(row.identity, firstIdentity)))
    issues.push('all scene reports in one review must share the same Runtime and input identity');
  const environmentIssues = [], environment = normalizedEnvironment(input.environment, environmentIssues);
  const checkIssues = [], selectedChecks = normalizedChecks(input.selectedChecks, checkIssues);
  issues.push(...environmentIssues, ...checkIssues);
  if (input.verdict === 'failed' && !nonempty(input.notes)) issues.push('failed review needs a short issue description');
  return {materials: rows.map(({report, ...row}) => ({...row,
    report: report ? {schema: report.schema, bytes: report.bytes, sha256: report.sha256} : null})),
    environment, selectedChecks, identity: firstIdentity, issues};
}

/** Create one verdict for a collected set of sampled scene reports. */
export async function createVisualReview({materials, reviewer, verdict, notes = '', selectedChecks, environment} = {}) {
  if (!VERDICTS.has(verdict)) throw Error('verdict must be explicitly passed as passed, failed, or inconclusive');
  const humanReviewer = normalizedReviewer(reviewer);
  if (typeof notes !== 'string') throw Error('notes must be a string');
  if (verdict === 'failed' && !nonempty(notes)) throw Error('failed review needs a short issue description');
  const evidence = await collectSessionMaterials(materials, {environment, selectedChecks, verdict, notes});
  if (verdict === 'passed' && evidence.issues.length)
    throw Error(`Cannot record passed visual review: ${evidence.issues.join('; ')}`);
  return {schema: SCHEMA, scope: SCOPE, verdictScope: 'sampled-material-set',
    materials: evidence.materials, selectedChecks: evidence.selectedChecks,
    checks: evidence.selectedChecks.map(id => ({id, complete: false})),
    reviewer: humanReviewer, verdict, notes, environment: evidence.environment,
    claims: {adaptationComplete: false, automaticPurityOverridden: false, checksComplete: false},
    createdAt: new Date().toISOString(), diagnostics: evidence.issues};
}

/** Validate current material-set records and accept version 1 only as a read-only format. */
export async function validateVisualReview({record, reportText, materials} = {}) {
  if (record?.schema === SCHEMA_V1) return validateVisualReviewV1({record, reportText});
  if (!isObject(record) || record.schema !== SCHEMA || record.scope !== SCOPE || record.verdictScope !== 'sampled-material-set')
    throw Error('Unsupported human visual review record or verdict scope');
  if (!record.claims || record.claims.adaptationComplete !== false || record.claims.automaticPurityOverridden !== false || record.claims.checksComplete !== false)
    throw Error('Human visual review cannot grant adaptation completion or override automatic purity');
  if (!VERDICTS.has(record.verdict)) throw Error('Review verdict is missing or invalid');
  const reviewer = normalizedReviewer(record.reviewer);
  if (!equal(reviewer, record.reviewer)) throw Error('Reviewer fields are not normalized');
  if (typeof record.notes !== 'string') throw Error('Review notes are missing');
  if (record.verdict === 'failed' && !nonempty(record.notes)) throw Error('Failed review is missing its issue description');
  if (typeof record.createdAt !== 'string' || !Number.isFinite(Date.parse(record.createdAt))) throw Error('Review timestamp is missing');
  if (record.observations !== undefined || record.report !== undefined || record.scene !== undefined || record.tickRange !== undefined)
    throw Error('A material-set review cannot carry per-window verdict fields');
  const evidence = await collectSessionMaterials(materials, {environment: record.environment,
    selectedChecks: record.selectedChecks, verdict: record.verdict, notes: record.notes});
  if (!equal(record.materials, evidence.materials)) throw Error('Scene reports or their identities do not match the review material set');
  if (!equal(record.selectedChecks, evidence.selectedChecks)) throw Error('Selected visual checks are invalid');
  if (!equal(record.checks, evidence.selectedChecks.map(id => ({id, complete: false}))) || record.checks.some(check => check.complete !== false))
    throw Error('A sampled material set cannot mark an adaptation check complete');
  if (!equal(record.environment, evidence.environment)) throw Error('Review environment is invalid');
  if (!equal(record.diagnostics, evidence.issues)) throw Error('Review diagnostics do not match materials and environment');
  if (record.verdict === 'passed' && evidence.issues.length)
    throw Error(`Cannot validate passed visual review: ${evidence.issues.join('; ')}`);
  return {valid: true, diagnostics: evidence.issues};
}
