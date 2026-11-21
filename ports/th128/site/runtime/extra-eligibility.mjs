const routePaths = Object.freeze({
  A1: Object.freeze([1, 2, 3]), A2: Object.freeze([1, 4, 5]),
  B1: Object.freeze([6, 7, 8]), B2: Object.freeze([6, 9, 10]),
  C1: Object.freeze([11, 12, 13]), C2: Object.freeze([11, 14, 15]),
});
const routeIds = Object.freeze(Object.keys(routePaths));
const factKeys = Object.freeze(['schema', 'route', 'runId', 'inputOwnerId', 'rank', 'mode', 'stages',
  'startedAtRouteOrigin', 'naturalTransitions', 'fullRouteCleared', 'originalEndingCompleted', 'taint']);
const completionKeys = Object.freeze(['startedAtRouteOrigin', 'naturalTransitions', 'fullRouteCleared', 'originalEndingCompleted']);
const taintKeys = Object.freeze(['runTainted', 'debugSession', 'debugJump', 'infiniteRespawn',
  'practice', 'replay', 'checkpointRestore', 'toolAssisted', 'autoplay']);

function freezeTree(value) {
  if (value && typeof value === 'object') {
    for (const child of Object.values(value)) freezeTree(child);
    Object.freeze(value);
  }
  return value;
}

function dataRecord(value, keys) {
  if (!value || typeof value !== 'object') return null;
  const prototype = Object.getPrototypeOf(value);
  if (prototype !== Object.prototype && prototype !== null) return null;
  const names = Reflect.ownKeys(value);
  if (names.length !== keys.length || !names.every(name => keys.includes(name))) return null;
  const descriptors = Object.getOwnPropertyDescriptors(value);
  const copy = Object.create(null);
  for (const key of keys) {
    const descriptor = descriptors[key];
    if (!descriptor || !Object.hasOwn(descriptor, 'value')) return null;
    copy[key] = descriptor.value;
  }
  return copy;
}

function dataArray(value, maximum) {
  if (!Array.isArray(value) || Object.getPrototypeOf(value) !== Array.prototype) return null;
  const descriptors = Object.getOwnPropertyDescriptors(value);
  const length = descriptors.length?.value;
  if (!Number.isInteger(length) || length < 0 || length > maximum || Reflect.ownKeys(value).length !== length + 1) return null;
  const copy = [];
  for (let index = 0; index < length; index += 1) {
    const descriptor = descriptors[index];
    if (!descriptor || !Object.hasOwn(descriptor, 'value')) return null;
    copy.push(descriptor.value);
  }
  return copy;
}

function readFact(value) {
  const fact = dataRecord(value, factKeys);
  if (!fact || fact.schema !== 'th128-normal-clear-facts/1') return null;
  const taint = dataRecord(fact.taint, taintKeys);
  const stages = dataArray(fact.stages, 3);
  if (!taint || !stages || !stages.every(Number.isInteger)
      || !taintKeys.every(key => typeof taint[key] === 'boolean')
      || !completionKeys.every(key => typeof fact[key] === 'boolean')
      || typeof fact.route !== 'string' || typeof fact.mode !== 'string'
      || !Number.isInteger(fact.rank)
      || ![fact.runId, fact.inputOwnerId].every(identifier => typeof identifier === 'string'
        && /^[A-Za-z0-9][A-Za-z0-9._:-]{0,159}$/.test(identifier))) return null;
  return {...fact, taint, stages};
}

function progressResult(qualified, issue = null) {
  const routes = routeIds.map(route => ({route, status: qualified.has(route) ? 'qualified-by-facts' : 'missing-normal-clear'}));
  return freezeTree({
    schema: 'th128-normal-clear-policy-result/1',
    proofAuthenticity: 'not-verified-by-policy',
    count: qualified.size, total: 6, policySatisfied: qualified.size === 6,
    routes, missingRoutes: routes.filter(entry => entry.status === 'missing-normal-clear').map(entry => entry.route),
    issue,
  });
}

function meetsClearRules(fact) {
  const path = Object.hasOwn(routePaths, fact.route) ? routePaths[fact.route] : null;
  return path !== null && fact.rank >= 1 && fact.rank <= 3 && fact.mode === 'normal'
    && completionKeys.every(key => fact[key] === true)
    && taintKeys.every(key => fact.taint[key] === false)
    && fact.stages.length === path.length && path.every((stageId, index) => stageId === fact.stages[index]);
}

export function evaluateNormalClearFacts(values) {
  try {
    const records = dataArray(values, 2048);
    if (!records) return progressResult(new Set(), 'malformed-facts');
    const facts = records.map(readFact);
    if (facts.some(fact => fact === null)) return progressResult(new Set(), 'malformed-facts');
    const identities = new Map();
    const runFingerprints = new Map();
    const conflictingRuns = new Set();
    for (const fact of facts) {
      for (const [kind, identifier] of [['run', fact.runId], ['input', fact.inputOwnerId]]) {
        const identity = `${kind}:${identifier}`;
        if (!identities.has(identity)) identities.set(identity, new Set());
        identities.get(identity).add(fact.route);
      }
      const fingerprint = JSON.stringify(fact);
      if (runFingerprints.has(fact.runId) && runFingerprints.get(fact.runId) !== fingerprint) conflictingRuns.add(fact.runId);
      runFingerprints.set(fact.runId, fingerprint);
    }
    const qualified = new Set();
    for (const fact of facts) {
      if (meetsClearRules(fact) && !conflictingRuns.has(fact.runId)
          && identities.get(`run:${fact.runId}`).size === 1
          && identities.get(`input:${fact.inputOwnerId}`).size === 1) qualified.add(fact.route);
    }
    return progressResult(qualified);
  } catch {
    return progressResult(new Set(), 'malformed-facts');
  }
}

const currentAccess = freezeTree({
  schema: 'th128-extra-access/1',
  eligibility: 'locked', provider: 'missing', runtime: 'unavailable', canStart: false,
  progress: evaluateNormalClearFacts([]),
  blockers: ['trusted-normal-clear-provider-missing', 'six-normal-routes-required', 'extra-runtime-not-accepted'],
  debug: {enabled: false, tainted: true, affectsNormalClear: false, reason: 'debug-extra-runtime-not-accepted'},
});

export function readCurrentExtraAccess() {
  return currentAccess;
}

export function requestNormalExtraStart() {
  const access = readCurrentExtraAccess();
  const error = new Error('Normal Extra 已锁定：缺可信正常通关 provider，六线资格未完成；rank4 运行支持未完成。');
  error.code = 'normal-extra-locked';
  error.access = access;
  throw error;
}

export function assertStageStartAllowed(stageId, difficulty) {
  if (!Number.isInteger(stageId) || stageId < 1 || stageId > 16
      || !Number.isInteger(difficulty) || difficulty < 0 || difficulty > 4) {
    const error = new Error('Invalid stage/difficulty start request');
    error.code = 'invalid-stage-start';
    throw error;
  }
  if (stageId === 16 || difficulty === 4) requestNormalExtraStart();
  return true;
}
