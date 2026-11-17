import {fetchResource,isImportedResourceMode} from './resource-fetch.mjs';
import {readWorldDrawRecords} from './runtime/world-presentation.mjs';
import {mountWebInput} from './runtime/web-input.mjs';
import {getPageLaunchContext} from './runtime/page-launch-context.mjs';
import {musicOutput,musicOutputEnabled} from './runtime/music-output.mjs';
import {mountThprac128Menu} from './runtime/thprac128-menu.mjs';
import {createThprac128Tools,Thprac128LogicClock,Thprac128Settings,mountThprac128ToolsPanel,validateThprac128Speed} from './runtime/thprac128-tools.mjs';
import {createThprac128Recorder} from './runtime/thprac128-recording.mjs';
import {readThprac128Replay,Thprac128ReplayPlayback} from './runtime/thprac128-replay.mjs';

import {createThprac128Session,prepareThprac128Native,readThprac128Native,thprac128RuntimeSupported,thprac128Presentation} from './runtime/thprac128.mjs';
import {QualificationOwner,evaluateOwnedQualification,EX_QUALIFICATION_SCHEMA} from './runtime/ex-qualification.mjs';
import {loadOriginalType33ForWorld,originalType33Profile} from './runtime/original-bullet-type33.mjs';
const {assertStageStartAllowed,mountExtraMenu,setDebugExtraSaveEligibility,setOwnedExtraSaveEligibility,readCurrentExtraAccess,requestNormalExtraStart}=(()=>{
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

function evaluateNormalClearFacts(values) {
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

let ownedExtraQualification=null;
function setOwnedExtraSaveEligibility(value){
  if(value!==null&&value?.schema!==EX_QUALIFICATION_SCHEMA)throw new Error('Owned six-route qualification schema differs');
  ownedExtraQualification=value;
}
let debugSavedClearMask=0;
function setDebugExtraSaveEligibility(mask){if(!Number.isInteger(mask)||mask<0||mask>63)throw new Error('Invalid debug save clear mask');debugSavedClearMask=mask;}
const currentAccess = freezeTree({
  schema: 'th128-extra-access/1',
  eligibility: 'locked', provider: 'missing', runtime: 'unavailable', canStart: false,
  progress: evaluateNormalClearFacts([]),
  blockers: ['trusted-normal-clear-provider-missing', 'six-normal-routes-required', 'extra-runtime-not-accepted'],
  debug: {enabled: false, tainted: true, affectsNormalClear: false, reason: 'debug-extra-runtime-not-accepted'},
});

function readCurrentExtraAccess() {
  if(ownedExtraQualification){
    const q=ownedExtraQualification,qualified=new Set(q.qualifiedRoutes);
    const progress={schema:'th128-owned-clear-progress/1',proofAuthenticity:q.proofAuthenticity,count:qualified.size,total:6,policySatisfied:q.eligible,missingRoutes:q.missingRoutes,
      routes:routeIds.map(route=>({route,status:qualified.has(route)?'qualified-by-native-profile':'missing-normal-clear'}))};
    if(q.eligible||debugSavedClearMask!==63)return freezeTree({schema:'th128-extra-access/1',eligibility:q.eligible?'owned-unlocked':'locked',provider:'owned-native-six-route-profile',runtime:'assisted-verification',canStart:q.eligible,progress,
      mask:q.mask,blockers:q.eligible?[]:['six-normal-routes-required'],debug:{enabled:false,tainted:true,affectsNormalClear:false,reason:'assisted-extra-runtime'},profileSHA256:q.record.sha256});
  }
  if(debugSavedClearMask===63)return freezeTree({...currentAccess,eligibility:'debug-unlocked',provider:'explicit-debug-web-save',canStart:true,debug:{...currentAccess.debug,enabled:true,savedClearMask:63,affectsNormalClear:false}});
  return currentAccess;
}

function requestNormalExtraStart() {
  const access = readCurrentExtraAccess();
  if(access.eligibility==='owned-unlocked'&&access.canStart)return {debugExtra:true,tainted:true,profileSHA256:access.profileSHA256};
  if(debugSavedClearMask===63)return {debugExtra:true,tainted:true};
  const error = new Error('Normal Extra 已锁定：缺可信正常通关 provider，六线资格未完成；rank4 运行支持未完成。');
  error.code = 'normal-extra-locked';
  error.access = access;
  throw error;
}

function assertStageStartAllowed(stageId, difficulty, {debugExtra=false}={}) {
  if (!Number.isInteger(stageId) || stageId < 1 || stageId > 16
      || !Number.isInteger(difficulty) || difficulty < 0 || difficulty > 4) {
    const error = new Error('Invalid stage/difficulty start request');
    error.code = 'invalid-stage-start';
    throw error;
  }
  if (debugExtra && (stageId !== 16 || difficulty !== 4)) throw new Error('Debug EX requires original stage16/rank4');
  if (stageId === 16 || difficulty === 4) {
    if (debugExtra === true && stageId === 16 && difficulty === 4) return true;
    requestNormalExtraStart();
  }
  return true;
}


function describeNormalProgress(progress) {
  const satisfied = progress?.policySatisfied === true && progress?.count === 6;
  return Object.freeze({
    eligibilityText: satisfied
      ? '规则条件已满足：6/6（仅事实规则，不是可信原生通关证明）。'
      : `资格未满足：${Number.isInteger(progress?.count) ? progress.count : 0}/6；每线须 Normal rank ≥1、真实全路线 clear、原 ending 完成且全程 no-taint。`,
    runtimeText: 'EX stage 16／rank 4 已验证启动，仍在移植调试；激光与终幕尚未完成。正常解锁资格不变。',
    canStart: false,
  });
}

function mountExtraMenu(documentRef = document) {
  const required = selector => {
    const node = documentRef.querySelector(selector);
    if (!node) throw new Error(`Missing visible Extra menu target: ${selector}`);
    return node;
  };
  const panel = required('#extra-menu-panel');
  const normalButton = required('#normal-extra-start');
  const debugButton = required('#debug-extra-start');
  const status = required('#extra-status');
  const refresh = () => {
    const access = readCurrentExtraAccess();
    const presentation = describeNormalProgress(access.progress),owned=access.provider==='owned-native-six-route-profile';
    required('#extra-progress').textContent = debugSavedClearMask===63?'EX 已解锁 · 调试存档模拟6/6（非真实通关）':`Normal Extra · Locked · ${access.progress.count}/6`;
    required('#extra-provider').textContent = debugSavedClearMask===63?'当前使用你主动写入的网页模拟通关存档；六线条件6/6，仅供解锁验证，不是可信正常通关。':'当前无六线通关存档条件；可用调试按钮验证解锁显示，不会写入真实通关证明。';
    required('#extra-eligibility').textContent = debugSavedClearMask===63?'调试存档条件已满足：6/6；进入EX仍为tainted调试。':presentation.eligibilityText;
    required('#extra-runtime').textContent = presentation.runtimeText;
    required('#extra-missing').textContent = debugSavedClearMask===63?'模拟六线均已标记；真实通关记录仍未伪造。':`缺少通关：${access.progress.missingRoutes.join('、')}。`;
    for (const row of documentRef.querySelectorAll('[data-extra-route]')) {
      const entry = access.progress.routes.find(value => value.route === row.dataset.extraRoute);
      row.textContent = entry
        ? `${entry.route}：未确认 · 缺可信 Normal 全路线 clear + 原 ending；不能用第三面进入、挂机或 control15/16 补数。`
        : '未知路线：不计资格。';
    }
    if(owned){
      required('#extra-progress').textContent='EX · 原生存档 '+access.progress.count+'/6';
      required('#extra-provider').textContent='已读取完整原生档案与独立姓名字段。';
      required('#extra-eligibility').textContent=access.canStart?'六路线通关已满足（任意难度）；当前使用辅助验证流程。':'六路线通关尚未齐全。';
      required('#extra-runtime').textContent='EX 无敌辅助验证中。';
      required('#extra-missing').textContent=access.progress.missingRoutes.length?'缺少通关：'+access.progress.missingRoutes.join('、'):'六路线已齐全。';
      for(const row of documentRef.querySelectorAll('[data-extra-route]'))row.textContent=row.dataset.extraRoute+'：'+(access.progress.missingRoutes.includes(row.dataset.extraRoute)?'未通关':'已通关');
    }
    normalButton.disabled = !access.canStart;
    normalButton.setAttribute('aria-disabled', String(!access.canStart));
    normalButton.textContent = debugSavedClearMask===63?'进入 EX（模拟六线通关存档／调试）':`Normal Extra（锁定 ${access.progress.count}/6）`;
    if(owned)normalButton.textContent=access.canStart?'进入 EX（辅助验证）':'EX（六路线未齐全）';
    const debugReady = debugButton.dataset?.runtimeReady === 'true';
    debugButton.disabled = !debugReady;
    debugButton.setAttribute('aria-disabled', String(!debugReady));
    debugButton.textContent = '进入 EX（调试中）';
    return access;
  };
  normalButton.addEventListener('click', () => {
    try {
      const access=readCurrentExtraAccess();requestNormalExtraStart();
      if(access.provider==='owned-native-six-route-profile')launchNativeRoute(3);
      else if(debugSavedClearMask===63)debugButton.click();
    } catch (error) {
      status.textContent = error.message;
    }
    refresh();
  });
  required('#extra-refresh').addEventListener('click', () => {
    refresh();
    status.textContent = '只读检查完成：Normal Extra 仍锁定；无通关记录或本地存档被修改。';
  });
  panel.addEventListener('keydown', event => event.stopPropagation());
  refresh();
  return Object.freeze({readAccess: readCurrentExtraAccess, requestNormalStart: requestNormalExtraStart, refresh});
}

return {assertStageStartAllowed,mountExtraMenu,setDebugExtraSaveEligibility,setOwnedExtraSaveEligibility,readCurrentExtraAccess,requestNormalExtraStart};
})();
const {installCheckpointAdapter,checkpointFetch}=(()=>{
function readCheckpointNativeState(debug) {
  if (!debug || typeof debug.state !== 'function' || typeof debug.inspectWorld !== 'function')
    throw new Error('Checkpoint native state reader is unavailable');
  const ui = debug.state();
  if (!ui || typeof ui.worldMode !== 'boolean') throw new Error('Checkpoint UI state is unavailable');
  if (!ui.worldMode) return {...ui, menuFrame: ui.frame, frame: null, nativeSnapshot: null, debug: null};
  const inspected = debug.inspectWorld();
  const native = inspected?.snapshot;
  if (!native || !Number.isInteger(native.frame) || native.frame < 0
      || !Number.isInteger(native.flow?.currentId)) throw new Error('Checkpoint native world identity is unavailable');
  return {...ui, menuFrame: ui.frame, frontendStageId: ui.stageId,
    frame: native.frame, stageId: native.flow.currentId,
    difficulty: native.flow.difficulty ?? ui.difficulty,
    debug: native.debug, nativeSnapshot: native, nativeError: inspected.error ?? ''};
}

function createCheckpointDebugFacade(debug) {
  return new Proxy(debug, {
    get(target, name) {
      if (name === 'state') return () => readCheckpointNativeState(target);
      const value = target[name];
      return typeof value === 'function' ? value.bind(target) : value;
    },
  });
}
const SCHEMA = 'th128-debug-checkpoint/1';
const MAX_ROWS = 2000;
const MAX_TICKS = 6000;

class CheckpointError extends Error {
  constructor(code, message) {
    super(message);
    this.name = 'CheckpointError';
    this.code = code;
  }
}

function fail(code, message) {
  throw new CheckpointError(code, message);
}

function integer(value) {
  return Number.isInteger(value);
}

function validateRows(rows, label) {
  if (!Array.isArray(rows) || rows.length === 0) fail('malformed-row', `${label} rows must be a non-empty array`);
  let ticks = 0;
  for (const row of rows) {
    if (!Array.isArray(row) || row.length !== 3) fail('malformed-row', `${label} row shape`);
    const [count, holding, confirm] = row;
    if (!integer(count) || count < 1 || count > 4) fail('malformed-row', `${label} row ticks must be 1..4`);
    if (!integer(holding) || holding < 0 || holding > 65535) fail('malformed-row', `${label} holding`);
    if (!integer(confirm) || !(confirm === 0 || confirm === 1)) fail('malformed-row', `${label} confirm`);
    ticks += count;
  }
  return ticks;
}

function validateBundle(bundle, currentGeneration) {
  if (typeof currentGeneration !== 'string' || !/^[a-f0-9]{64}$/.test(currentGeneration)) {
    fail('stale-core', 'current core generation is unavailable');
  }
  if (!bundle || typeof bundle !== 'object') fail('malformed-bundle', 'bundle is not an object');
  if (bundle.schema !== SCHEMA) fail('malformed-bundle', `bundle schema must be ${SCHEMA}`);
  if (bundle.kind !== 'input-replay') fail('malformed-bundle', 'bundle kind must be input-replay');
  if (typeof bundle.id !== 'string' || !bundle.id) fail('malformed-bundle', 'bundle id is required');
  if (!bundle.source || !bundle.origin || !bundle.target || !bundle.guards) {
    fail('malformed-bundle', 'bundle source/origin/target/guards are required');
  }
  if (!Array.isArray(bundle.segments) || bundle.segments.length < 2) {
    fail('malformed-bundle', 'bundle requires stage1 and stage2 segments');
  }
  const guards = bundle.guards;
  const debugGuard = guards.debugInfiniteRespawn ?? guards.nativeInfiniteRespawn;
  const sessionGuard = guards.session ?? guards.nativeSession;
  const taintGuard = guards.taint ?? guards.runTainted;
  if (debugGuard !== true || sessionGuard !== true || taintGuard !== true) {
    fail('debug-off', 'bundle requires native debug infinite/session/taint guards');
  }
  if (guards.normalModeClear !== false) fail('normal-clear', 'checkpoint must not claim normal clear');
  const identityGuard = guards.sourceTargetIdentity === true || Boolean(bundle.handoff?.stage1ToStage2);
  const carryGuardValue = guards.carryAck === true || bundle.handoff?.stage1ToStage2?.flowCarry?.ACKed === true;
  if (!identityGuard || !carryGuardValue) {
    fail('carry-guard', 'bundle requires source/target identity and carry ACK guards');
  }
  if (bundle.source.generation !== currentGeneration) {
    const crossCoreSafe = bundle.source.crossCore === true
      && guards.allowPortableHeap === false && guards.allowNativeStateRestore === false
      && guards.allowDirectStageStart === false
      && Array.isArray(bundle.anchors?.stage1) && Array.isArray(bundle.anchors?.stage2);
    const explicitAllow = guards.allowCrossCoreInputReplay === true && guards.crossCoreAnchors === true;
    if (!crossCoreSafe && !explicitAllow) fail('cross-core', 'cross-core input replay is not explicitly allowed');
    if (!Array.isArray(bundle.anchors?.stage1) || bundle.anchors.stage1.length === 0
        || !Array.isArray(bundle.anchors?.stage2) || bundle.anchors.stage2.length === 0
        || !/^[a-f0-9]{64}$/.test(bundle.source.generation)
        || !/^[a-f0-9]{64}$/.test(bundle.source.result?.sha256)
        || !/^[a-f0-9]{64}$/.test(bundle.source.inputCache?.sha256)
        || !/^[a-f0-9]{64}$/.test(bundle.source.frontend?.play)
        || !/^[a-f0-9]{64}$/.test(bundle.source.frontend?.renderer)
        || !bundle.origin.entry?.gameRandom || !bundle.handoff?.stage1ToStage2?.gameRandom) {
      fail('cross-core', 'cross-core input replay requires actual anchors and source provenance');
    }
  }
  const first = bundle.segments[0];
  const second = bundle.segments[1];
  if (first.stageId !== 1 || second.stageId !== 2) fail('malformed-bundle', 'segments must be stage1 then stage2');
  if (first.entryFrame !== 0 || second.entryFrame !== 0) fail('malformed-row', 'segment entry must be frame0');
  if (!integer(first.exitFrame) || first.exitFrame < 1 || !integer(second.exitFrame) || second.exitFrame < 1) {
    fail('malformed-row', 'segment exit frames must be positive integers');
  }
  const firstTicks = validateRows(first.rows, 'stage1');
  const secondTicks = validateRows(second.rows, 'stage2');
  if (firstTicks !== first.exitFrame) fail('segment-sum', `stage1 rows ${firstTicks} != exit ${first.exitFrame}`);
  if (secondTicks !== second.exitFrame) fail('segment-sum', `stage2 rows ${secondTicks} != exit ${second.exitFrame}`);
  if (firstTicks !== 21608) fail('segment-sum', `stage1 sum must be 21608, got ${firstTicks}`);
  if (!integer(bundle.target.frame) || bundle.target.frame < 1 || bundle.target.frame > secondTicks) {
    fail('target', 'target frame is outside stage2 replay');
  }
  if (bundle.target.stageId !== 2 || (bundle.target.healthy !== true && bundle.target.healthyBeforeFault !== true)) {
    fail('target', 'target must be healthy stage2');
  }
  if (bundle.target.faultFrame !== undefined && bundle.target.frame >= bundle.target.faultFrame) {
    fail('target', 'target frame must precede the first fault');
  }
  if (bundle.target.healthyFrame !== undefined && bundle.target.frame > bundle.target.healthyFrame) {
    fail('target', 'target frame exceeds healthy anchor');
  }
  return bundle;
}

function chunks(rows) {
  const result = [];
  let current = [];
  let rowCount = 0;
  let tickCount = 0;
  for (const row of rows) {
    if (rowCount >= MAX_ROWS || tickCount + row[0] > MAX_TICKS) {
      result.push(current);
      current = [];
      rowCount = 0;
      tickCount = 0;
    }
    current.push(row);
    rowCount += 1;
    tickCount += row[0];
  }
  if (current.length) result.push(current);
  return result;
}

function isAbort(signal) {
  return Boolean(signal?.aborted);
}

function abortError(signal) {
  const reason = signal?.reason;
  return new CheckpointError('cancelled', reason?.message ?? String(reason ?? 'checkpoint replay cancelled'));
}

function delay() {
  return new Promise(resolve => setTimeout(resolve, 0));
}

function carryGuard(carry, debug, expectedSource) {
  if (!carry) fail('carry', 'native carry receipt is missing');
  if (carry.tokenAvailable !== false || carry.consumed !== true || carry.committed !== true
      || carry.scalarRestored !== true || carry.ACKed !== true) {
    fail('carry', 'native carry token/ACK guard failed');
  }
  if (carry.sourceId !== expectedSource || carry.targetId !== 2
      || !integer(carry.sourceSerial) || carry.sourceSerial < 1
      || !integer(carry.boundGeneration) || carry.boundGeneration < 1) {
    fail('carry', 'native carry identity/guard failed');
  }
  if (JSON.stringify(carry.preInitGameRandom) !== JSON.stringify(debug.gameRandom)
      || JSON.stringify(carry.preInitDisplayRandom) !== JSON.stringify(debug.displayRandom)) {
    fail('carry', 'native carry pre-init RNG mismatch');
  }
  return true;
}

function compareField(actual, expected, label) {
  if (expected !== undefined && expected !== null && JSON.stringify(actual) !== JSON.stringify(expected)) {
    fail('provenance', `${label} mismatch: ${JSON.stringify(actual)} != ${JSON.stringify(expected)}`);
  }
}

function compareNative(expected, native, label) {
  if (!expected) return;
  for (const field of ['scoreRaw', 'motivationRaw', 'powerRaw', 'perfectCharge', 'cumulativeFrozenArea']) {
    compareField(native?.[field], expected[field], `${label} ${field}`);
  }
  for (const field of ['gameRandom', 'displayRandom', 'difficulty', 'shotDifficulty', 'currentId', 'progress']) {
    compareField(native?.flow?.[field], expected[field], `${label} ${field}`);
  }
}

function compareAnchor(expected, native) {
  for (const field of ['frame', 'playerState', 'powerRaw', 'motivationRaw', 'spawns']) {
    compareField(native?.[field], expected[field], `anchor ${expected.frame} ${field}`);
  }
  compareField(native?.deathsCommitted, expected.deaths, `anchor ${expected.frame} deaths`);
}

function createCheckpointDriver(options = {}) {
  const debug = options.debug;
  if (!debug || typeof debug.replayWorldInputs !== 'function' || typeof debug.state !== 'function') {
    throw new Error('checkpoint driver requires the native debug adapter');
  }
  const loadBundle = options.loadBundle;
  if (typeof loadBundle !== 'function') throw new Error('checkpoint driver requires a bundle loader');
  const beginOriginalRoute = options.beginOriginalRoute ?? debug.beginOriginalRoute;
  const currentGeneration = options.currentGeneration ?? (() => null);
  const globalObject = options.global ?? globalThis;
  let active = null;
  let last = {
    phase: 'idle', bundleId: null, stageId: null, frame: null, generation: null,
    elapsedMs: 0, ticks: 0, lastHolding: 0, normalModeClear: false, error: null,
  };

  function snapshot() {
    let live;
    let nativeStatusError = null;
    try { live = debug.state(); } catch (error) { nativeStatusError = error.message; }
    const nativeTainted = live?.worldMode === true && live.debug?.infiniteRespawn === true
      && live.debug?.runTainted === true && live.debug?.sessionEnabled === true;
    return {...last, requestedOptIn: live?.debugInfiniteRespawnRequested === true,
      nativeTainted, debugEnabled: nativeTainted, debug: live?.debug ?? null, nativeStatusError};
  }

  function setPhase(phase, extra = {}) {
    last = {...last, ...extra, phase};
    return snapshot();
  }

  function assertNotActive() {
    if (active) fail('duplicate', 'a checkpoint restore is already active');
  }

  function checkAbort(token) {
    if (token.cancelled || isAbort(token.controller.signal)) throw abortError(token.controller.signal);
  }

  async function ensureEntry(token) {
    checkAbort(token);
    let state = debug.state();
    if (state?.handoff || state?.pendingCarryToken) fail('handoff', 'an original stage handoff/token is still owned');
    if (typeof beginOriginalRoute !== 'function') fail('entry', 'original Normal A entry adapter is unavailable');
    token.entryAttempted = true;
    const started = await beginOriginalRoute(token.controller.signal);
    checkAbort(token);
    if (started === false) fail('entry', 'original Normal A entry was refused');
    state = debug.state();
    if (state.worldMode !== true || state.stageId !== 1 || state.frame !== 0) {
      fail('entry', `checkpoint entry is not original Normal A stage1 frame0: ${JSON.stringify(state)}`);
    }
    if (state.debugInfiniteRespawnRequested !== true || state.debug?.infiniteRespawn !== true
        || state.debug?.runTainted !== true || state.debug?.sessionEnabled !== true) {
      fail('debug-off', 'debug opt-in required: native infinite/session/taint must be enabled before the first replay tick');
    }
    token.entryReady = true;
    return state;
  }

  async function replaySegment(token, segment, initialHolding, limitFrame, anchors = []) {
    let holding = initialHolding ?? 0;
    let index = 0;
    let observedFrame = Number(debug.state()?.frame);
    if (!integer(observedFrame) || observedFrame < 0) fail('execution-window', 'native frame is unavailable');
    if (!integer(limitFrame) || limitFrame < 1) fail('execution-window', 'native target frame is unavailable');
    while (index < segment.rows.length) {
      checkAbort(token);
      if (currentGeneration() !== token.generation) fail('stale-core', 'core generation changed during checkpoint replay');
      observedFrame = Number(debug.state()?.frame);
      if (!integer(observedFrame) || observedFrame < 0) fail('execution-window', 'native frame changed to an invalid value');
      if (observedFrame > limitFrame) fail('execution-window', `native frame overshot ${limitFrame}: ${observedFrame}`);
      if (observedFrame === limitFrame) break;
      const nextAnchor = anchors.find(anchor => anchor.frame > observedFrame && anchor.frame <= limitFrame);
      const remaining = (nextAnchor?.frame ?? limitFrame) - observedFrame;
      const batch = [];
      let batchTicks = 0;
      while (index < segment.rows.length && batch.length < MAX_ROWS) {
        const row = segment.rows[index];
        if (batchTicks + row[0] > MAX_TICKS) break;
        if (batchTicks + row[0] > remaining) break;
        batch.push(row);
        batchTicks += row[0];
        index += 1;
      }
      if (batch.length === 0) {
        const row = segment.rows[index];
        fail('edge-cut', `target window ${remaining} ticks cannot fit the next row ${JSON.stringify(row)} without splitting an input edge`);
      }
      const beforeFrame = observedFrame;
      const result = await debug.replayWorldInputs(batch, {
        initialHolding: holding, releaseAtEnd: false,
      });
      if (integer(result?.lastHolding)) holding = result.lastHolding;
      last.lastHolding = holding;
      last.requestedTicks = (last.requestedTicks ?? 0) + batchTicks;
      const state = debug.state();
      observedFrame = Number(state.frame);
      if (!integer(observedFrame) || observedFrame < 0) fail('execution-window', 'native frame is unavailable after replay');
      last.stageId = state.stageId;
      last.frame = observedFrame;
      last.ticks += Math.max(0, observedFrame - beforeFrame);
      last.elapsedMs = Math.round(performance.now() - token.startedAt);
      token.onProgress?.(snapshot());
      if (result?.error) fail('fault', result.error);
      if (state.stageId !== segment.stageId) break;
      if (observedFrame > limitFrame) fail('execution-window', `native frame overshot ${limitFrame}: ${observedFrame}`);
      if (nextAnchor && observedFrame === nextAnchor.frame) {
        compareAnchor(nextAnchor, debug.inspectWorld()?.snapshot);
        last.anchorChecks = (last.anchorChecks ?? 0) + 1;
      }
      await delay();
    }
    observedFrame = Number(debug.state()?.frame);
    if (debug.state()?.stageId === segment.stageId && observedFrame < limitFrame) {
      fail('execution-window', `native frame ${observedFrame} did not reach window ${limitFrame}`);
    }
    return holding;
  }

  async function waitHandoff(token) {
    if (typeof debug.setAutoFlow !== 'function' || typeof debug.advanceWorld !== 'function') {
      fail('handoff', 'auto-flow handoff adapter is unavailable');
    }
    if (debug.setAutoFlow(true) !== true) fail('handoff', 'native auto-flow handoff was refused');
    let state = debug.state();
    if (!state?.worldMode || state.stageId !== 2 || state.frame !== 0) {
      const result = await debug.advanceWorld(0, {present: false});
      if (result?.error) fail('fault', result.error);
    }
    const deadline = performance.now() + 45000;
    while (performance.now() < deadline) {
      checkAbort(token);
      state = debug.state();
      if (state.handoffFailed) fail('handoff', state.handoffError ?? 'original stage handoff failed; token was retained');
      last.stageId = state.stageId;
      last.frame = state.frame;
      if (state.worldMode === true && state.stageId === 2 && state.frame === 0) return state;
      await new Promise(resolve => setTimeout(resolve, 25));
    }
    fail('handoff', 'native stage2 frame0 handoff did not settle');
  }

  async function verifyTarget(token, bundle) {
    const inspected = debug.inspectWorld();
    const snapshotValue = inspected?.snapshot;
    if (!snapshotValue) fail('target', 'native inspection returned no world');
    if (snapshotValue.flow?.currentId !== 2) fail('target', 'target is not stage2');
    if (!integer(snapshotValue.frame) || snapshotValue.frame !== bundle.target.frame) {
      fail('target', `target frame ${snapshotValue.frame} != ${bundle.target.frame}`);
    }
    if (snapshotValue.debug?.infiniteRespawn !== true || snapshotValue.debug?.runTainted !== true
        || snapshotValue.debug?.sessionEnabled !== true) {
      fail('target', 'target debug taint/session was lost');
    }
    const carry = snapshotValue.flow?.carry;
    carryGuard(carry, token.entryFlow ?? {}, 1);
    compareField(carry, token.entryFlow?.carry, 'retained carry receipt');
    last.stageId = 2;
    last.frame = snapshotValue.frame;
    last.generation = currentGeneration();
    last.sourceGeneration = bundle.source.generation;
    last.normalModeClear = false;
    return snapshotValue;
  }

  async function restore(bundleId, {signal, onProgress} = {}) {
    assertNotActive();
    if (typeof bundleId !== 'string' || !bundleId) fail('malformed-bundle', 'bundle id is required');
    if (signal && typeof signal.aborted === 'boolean' && signal.aborted) throw abortError(signal);
    const controller = new AbortController();
    if (signal) {
      if (signal.aborted) throw abortError(signal);
      signal.addEventListener('abort', () => controller.abort(signal.reason), {once: true});
    }
    active = {controller, cancelled: false, startedAt: performance.now(), onProgress};
    const token = active;
    const startedAt = performance.now();
    const previousFast = globalObject.__TH128_FAST_REPLAY__;
    const previousDiagnosticPause = globalObject.__TH128_DIAGNOSTIC_PAUSED__;
    let previousAutoFlow = false;
    try {
      setPhase('loading', {bundleId, error: null, stageId: null, frame: null, ticks: 0, lastHolding: 0,
        requestedTicks: 0, anchorChecks: 0, originNativeProof: false, handoffReceipt: null});
      let bundle;
      try {
        bundle = await loadBundle(bundleId, {signal: controller.signal});
      } catch (error) {
        fail('unknown-bundle', error?.message ?? String(error));
      }
      bundle = validateBundle(bundle, currentGeneration());
      token.generation = currentGeneration();
      previousAutoFlow = debug.state()?.autoFlow === true;
      setPhase('replaying', {generation: currentGeneration(), sourceGeneration: bundle.source.generation, normalModeClear: false});
      globalObject.__TH128_FAST_REPLAY__ = true;
      globalObject.__TH128_DIAGNOSTIC_PAUSED__ = true;
      await ensureEntry(token);
      compareNative(bundle.origin.entry, debug.inspectWorld()?.snapshot, 'origin');
      last.originNativeProof = true;
      setPhase('replaying', {stageId: 1, frame: 0});
      last.ticks = 0;
      await replaySegment(token, bundle.segments[0], bundle.segments[0].initialHolding ?? 0, bundle.segments[0].exitFrame,
        bundle.anchors?.stage1 ?? []);
      setPhase('handoff');
      await waitHandoff(token);
      const entrySnapshot = debug.inspectWorld()?.snapshot;
      const entryCarry = entrySnapshot?.flow;
      carryGuard(entryCarry?.carry, entryCarry ?? {}, 1);
      compareNative(bundle.handoff?.stage1ToStage2, entrySnapshot, 'carry');
      for (const field of ['sourceId', 'targetId', 'preInitGameRandom', 'preInitDisplayRandom']) {
        compareField(entryCarry?.carry?.[field], bundle.handoff?.stage1ToStage2?.flowCarry?.[field], `carry ${field}`);
      }
      token.entryFlow = structuredClone(entryCarry);
      last.handoffReceipt = {frame: entrySnapshot.frame, flow: structuredClone(entryCarry),
        scoreRaw: entrySnapshot.scoreRaw, motivationRaw: entrySnapshot.motivationRaw,
        powerRaw: entrySnapshot.powerRaw, perfectCharge: entrySnapshot.perfectCharge,
        cumulativeFrozenArea: entrySnapshot.cumulativeFrozenArea, debug: structuredClone(entrySnapshot.debug)};
      setPhase('replaying', {stageId: 2, frame: 0});
      await replaySegment(token, bundle.segments[1], bundle.segments[1].initialHolding ?? 0, bundle.target.frame,
        bundle.anchors?.stage2 ?? []);
      checkAbort(token);
      await verifyTarget(token, bundle);
      if (typeof debug.advanceWorld === 'function') {
        const presented = await debug.advanceWorld(0, {present: true});
        if (presented?.error) fail('target', presented.error);
      }
      setPhase('ready', {
        stageId: 2, frame: bundle.target.frame, elapsedMs: Math.round(performance.now() - startedAt),
        normalModeClear: false, error: null,
      });
      return snapshot();
    } catch (error) {
      const cancelled = error?.code === 'cancelled' || token.cancelled || isAbort(controller.signal);
      setPhase(cancelled ? 'cancelled' : 'fault', {
        error: error?.message ?? String(error), elapsedMs: Math.round(performance.now() - startedAt),
      });
      throw error;
    } finally {
      try {
        if (!token.entryReady && token.entryAttempted && (token.cancelled || isAbort(controller.signal))) {
          if (typeof debug.returnToTitle !== 'function' || await debug.returnToTitle() !== true) {
            fail('entry-cleanup', 'aborted original entry could not release its world');
          }
        }
        if (typeof debug.setAutoFlow === 'function') debug.setAutoFlow(last.phase === 'ready' && previousAutoFlow);
      } catch (error) {
        setPhase('fault', {error: error.message});
        throw error;
      } finally {
        active = null;
        if (previousFast === undefined) delete globalObject.__TH128_FAST_REPLAY__;
        else globalObject.__TH128_FAST_REPLAY__ = previousFast;
        if (previousDiagnosticPause === undefined) delete globalObject.__TH128_DIAGNOSTIC_PAUSED__;
        else globalObject.__TH128_DIAGNOSTIC_PAUSED__ = previousDiagnosticPause;
      }
    }
  }

  function cancel(reason = 'cancelled') {
    if (active) {
      active.cancelled = true;
      active.controller.abort(new Error(String(reason)));
      setPhase('cancelled', {error: String(reason)});
      return snapshot();
    }
    if (last.phase !== 'idle') setPhase('cancelled', {error: String(reason)});
    return snapshot();
  }

  async function step(ticks = 1) {
    if (last.phase !== 'ready') fail('not-ready', 'checkpoint step requires a ready target');
    if (!integer(ticks) || ticks < 1 || ticks > 6000) fail('malformed-row', 'step ticks must be 1..6000');
    if (typeof debug.advanceWorld !== 'function') fail('target', 'native step adapter is unavailable');
    const live = debug.state();
    if (!live?.worldMode || live.debug?.infiniteRespawn !== true || live.debug?.sessionEnabled !== true
        || live.debug?.runTainted !== true) fail('debug-off', 'native debug session/taint was lost before step');
    if (live.stageId !== last.stageId || live.frame !== last.frame || currentGeneration() !== last.generation) {
      fail('stale-world', 'native checkpoint identity changed before step');
    }
    const beforeFrame = last.frame;
    const result = await debug.advanceWorld(ticks, {present: true});
    const after = debug.state();
    if (!after?.worldMode || after.stageId !== last.stageId || currentGeneration() !== last.generation
        || result?.snapshot?.flow?.currentId !== last.stageId || after.debug?.infiniteRespawn !== true
        || after.debug?.sessionEnabled !== true || after.debug?.runTainted !== true) {
      setPhase('fault', {error: 'native checkpoint identity changed during step'});
      fail('stale-world', 'native checkpoint identity changed during step');
    }
    const observedFrame = result?.snapshot?.frame;
    if (!integer(observedFrame) || after.frame !== observedFrame || observedFrame < beforeFrame || observedFrame > beforeFrame + ticks) {
      setPhase('fault', {error: 'native step snapshot is missing or inconsistent'});
      fail('target', 'native step snapshot is missing or inconsistent');
    }
    last.frame = observedFrame;
    last.requestedStepTicks = (last.requestedStepTicks ?? 0) + ticks;
    if (result?.error) {
      setPhase('fault', {error: result.error});
      fail('fault', result.error);
    }
    last.nativeStepFrameAdvance = (last.nativeStepFrameAdvance ?? 0) + observedFrame - beforeFrame;
    return snapshot();
  }

  return {
    version: 1,
    list: () => typeof options.listBundles === 'function' ? options.listBundles() : [],
    restore,
    cancel,
    step,
    status: snapshot,
  };
}
async function checkpointFetch(input, init, settings = {}) {
  if(typeof isImportedResourceMode==='function'&&isImportedResourceMode())return await fetchResource(input,init);
  const fetchImpl = settings.fetchImpl ?? globalThis.fetch;
  const origin = settings.origin ?? globalThis.location?.origin;
  const fastForward = settings.fastForward ?? (globalThis.__TH128_FAST_REPLAY__ === true);
  const wait = settings.wait ?? (milliseconds => new Promise(resolve => setTimeout(resolve, milliseconds)));
  const url = typeof input === 'string' && origin ? new URL(input, origin) : null;
  const retryable = fastForward && url?.origin === origin && /^\/(assets|runtime)\//.test(url.pathname)
    && (!init?.method || init.method === 'GET') && !init?.body;
  for (let attempt = 0; ; attempt++) {
    if (init?.signal?.aborted) throw init.signal.reason ?? new DOMException('Request aborted', 'AbortError');
    try {
      return await fetchImpl(input, init);
    } catch (error) {
      if (!retryable || attempt >= 2 || init?.signal?.aborted || error?.name !== 'TypeError') throw error;
      await wait(100 * (attempt + 1));
    }
  }
}

const INDEX_SCHEMA = 'th128-debug-checkpoint-index/1';

function validId(value) {
  return typeof value === 'string' && value.length > 0;
}

async function fetchJson(url, signal, integrity) {
  const response = await checkpointFetch(url, {cache: 'no-store', signal});
  if (!response.ok) throw new Error(`checkpoint bundle request failed ${response.status}`);
  if (integrity) {
    const bytes = await response.arrayBuffer();
    if (bytes.byteLength !== integrity.bytes) throw new Error('checkpoint byte integrity mismatch');
    const hash = Array.from(new Uint8Array(await globalThis.crypto.subtle.digest('SHA-256', bytes)),
      value => value.toString(16).padStart(2, '0')).join('');
    if (hash !== integrity.sha256) throw new Error('checkpoint SHA256 integrity mismatch');
    return JSON.parse(new TextDecoder().decode(bytes));
  }
  return response.json();
}

function validateIndexEntries(value) {
  const ids = new Set();
  for (const entry of value.bundles) {
    if (!entry || !/^[A-Za-z0-9_-]{1,128}$/.test(entry.id) || ids.has(entry.id)) {
      throw new Error('checkpoint index contains an invalid or duplicate id');
    }
    ids.add(entry.id);
    if (!/^\/assets\/generations\/[a-f0-9]{32}\/(ja|zh)\/animations\/[A-Za-z0-9_-]+\.json$/.test(entry.url)) {
      throw new Error('checkpoint index bundle path is not an allowed public asset');
    }
    if (!Number.isInteger(entry.bytes) || entry.bytes < 1 || !/^[a-f0-9]{64}$/.test(entry.sha256)) {
      throw new Error('checkpoint index bundle integrity is unavailable');
    }
    if (!Number.isInteger(entry.target?.stageId) || entry.target.stageId < 1 || entry.target.stageId > 16
        || !Number.isInteger(entry.target?.frame) || entry.target.frame < 1 || entry.guards?.normalModeClear !== false) {
      throw new Error('checkpoint index target/normal-clear guard is invalid');
    }
  }
  return value;
}

function installCheckpointAdapter({
  debug, indexUrl, currentGeneration, beginOriginalRoute, indexIntegrity, fetchImpl = fetchJson,
} = {}) {
  if (!debug || typeof debug !== 'object') throw new Error('checkpoint adapter requires th128Debug');
  if (typeof indexUrl !== 'string' || !indexUrl) throw new Error('checkpoint adapter requires an index URL');
  let index = null;
  let loadError = null;
  const loading = fetchImpl(indexUrl, undefined, indexIntegrity).then(value => {
    if (!value || value.schema !== INDEX_SCHEMA || !Array.isArray(value.bundles)) {
      throw new Error('checkpoint bundle index schema is invalid');
    }
    index = validateIndexEntries(value);
    return value;
  }).catch(error => {
    loadError = error;
    return null;
  });
  const driver = createCheckpointDriver({
    debug: createCheckpointDebugFacade(debug),
    currentGeneration,
    beginOriginalRoute,
    listBundles: () => (index?.bundles ?? []).map(entry => ({
      id: entry.id, title: entry.title ?? entry.id, url: entry.url,
      sourceGeneration: entry.sourceGeneration ?? null, targetStage: entry.targetStage ?? 2,
      source: entry.source ?? {generation: entry.sourceGeneration ?? null}, origin: entry.origin ?? null,
      target: entry.target ?? null, guards: entry.guards ?? {normalModeClear: false},
    })),
    loadBundle: async (id, options = {}) => {
      await loading;
      if (loadError) throw loadError;
      const entry = index?.bundles?.find(candidate => candidate.id === id);
      if (!entry || !validId(entry.url)) throw new Error(`unknown checkpoint bundle ${id}`);
      const value = await fetchImpl(entry.url, options.signal, entry);
      if (value?.id !== id) throw new Error('checkpoint bundle id does not match index');
      if (value.target?.stageId !== entry.target.stageId || value.target?.frame !== entry.target.frame) {
        throw new Error('checkpoint bundle target does not match its immutable index');
      }
      if (entry.sourceGeneration && value.source?.generation !== entry.sourceGeneration) {
        throw new Error('checkpoint bundle source generation does not match its immutable index');
      }
      return value;
    },
  });
  driver.ready = loading;
  debug.checkpoints = driver;
  return driver;
}
return {installCheckpointAdapter,checkpointFetch};
})();
async function loadingDeadline(operation, resource, cancel){
  let timer;
  try{return await Promise.race([operation,new Promise((resolve,reject)=>{
    timer=setTimeout(()=>{reject(new Error(`本地资源读取超时：${resource}`));cancel?.();},15000);
  })]);}finally{clearTimeout(timer);}
}

function startupFailure(problem){
  document.querySelector('#loading').hidden=true;
  const selector=document.querySelector('#variant'),option=document.createElement('option');
  option.textContent='加载失败，请刷新重试';selector.replaceChildren(option);selector.disabled=true;
  const box=document.querySelector('#error');box.hidden=false;
  box.textContent=`资源加载失败：${problem.message??String(problem)}\n请刷新页面；若资源返回404，需要重启本项目的本地预览服务。`;
  document.querySelector('#status').textContent='加载失败，已停止等待；请查看诊断后刷新重试。';
}

const stageCatalogSchema='th128-stage-catalog/1';
const stageExeSha256='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4';
const stageTableSha256='004205c77aa4858096b2fdd30ce4a93d4394a61707f9507dd4490f65750e5484';
const stageNamePattern=/^[A-Za-z0-9_][A-Za-z0-9_.-]{0,63}$/;
function validStageName(value,extension){
  return typeof value==='string'&&stageNamePattern.test(value)&&!value.includes('..')&&value.endsWith(extension);
}
function validateStageCatalog(catalog){
  if(catalog?.schema!==stageCatalogSchema||catalog.sourceExeSha256!==stageExeSha256
    ||catalog.sourceTableSha256!==stageTableSha256)throw new Error('关卡目录来源校验失败');
  if(!Array.isArray(catalog.stages)||catalog.stages.length!==16)throw new Error('关卡目录必须包含16条记录');
  const ids=new Set();
  for(const stage of catalog.stages){
    if(!Number.isInteger(stage.id)||stage.id<1||stage.id>16||ids.has(stage.id))throw new Error(`非法关卡ID：${stage.id}`);
    ids.add(stage.id);
    if(!validStageName(stage.background,'.std')||!validStageName(stage.ecl,'.ecl')
      ||!validStageName(stage.message,'.msg')||!validStageName(stage.logo,'.anm')
      ||!Array.isArray(stage.music)||stage.music.length!==2
      ||!stage.music.every(name=>validStageName(name,'.wav')))throw new Error(`关卡资源名非法：${stage.id}`);
  }
  if(ids.size!==16)throw new Error('关卡ID未完整覆盖1..16');
  return catalog;
}
function resolveStage(catalog,stageId){
  const stage=catalog?.stages.find(value=>value.id===stageId);
  if(!stage)throw new Error(`未找到关卡：${stageId}`);
  return stage;
}

async function initialize(){
const launcherContext=getPageLaunchContext();launcherContext?.signal.throwIfAborted();
const [renderer,draws,timing,inspector,sound]=await loadingDeadline(Promise.all([
  import('./runtime/renderer-owned-ending-v2.mjs'),import('./runtime/draw.mjs'),import('./runtime/timing.mjs'),
  import('./inspector.mjs'),import('./runtime/sound.mjs')]),'运行页依赖');
launcherContext?.signal.throwIfAborted();
const {OwnedNativeTitleRecordStore,nativeRouteScorePacket,originalResultModel,originalResultPacket,persistOwnedNativeScoreCapture}=sound;
const {WebGlRenderer}=renderer,{OriginalPresentationFPS,originalFPSDraws,originalFPSText,decodeDrawBuffer,orderDrawLayers,messageTextLayout,loadOriginalStageTextBank,drawOriginalStageText,loadOriginalHelpPage,splitOriginalMenuDraws,originalHelpPagePins,loadOriginalMainMusicRasters,disposeOriginalMainMusicRasters,uploadOriginalMainMusicRequest,decodeOriginalMainMusicPNG,loadOriginalResultFont,uploadOriginalResultTextRequest}=draws,{FrameStepper,WorldPresentationInterpolator,renderInterpolationPhase}=timing,
  {safeAssetUrl}=inspector,{GameSoundQueue,StageMusic,OriginalPauseController,MenuAudioGate,OwnedMenuSoundLedger,consumeMenuSoundWindow,OwnedReplayStore,originalReplayPreviewPacket,originalReplayNameContextPacket,OwnedLastNameFieldStore,originalNameProfilePacket,OriginalMainMenuController,OriginalMainSettingsStore,OriginalTitleAudio,TitleSoundWindowConsumer,OwnedReplayPlayback,clearOwnedWebSaveData}=sound,{parseStd,StageBackground}=renderer;
if([WebGlRenderer,decodeDrawBuffer,orderDrawLayers,FrameStepper,WorldPresentationInterpolator,renderInterpolationPhase,OriginalPresentationFPS,originalFPSDraws,originalFPSText,safeAssetUrl,GameSoundQueue,StageMusic].some(value=>typeof value!=='function')){
  throw new Error('运行页依赖接口不完整，请重启本地预览服务后刷新。');
}

const find=selector=>document.querySelector(selector);
const clock=new FrameStepper(),originalMenuClock=new FrameStepper();
const originalPresentationFPS=new OriginalPresentationFPS();
const state={module:null,renderer:null,manifest:null,base:'',catalog:null,stageCatalog:null,stageSelection:null,active:false,paused:true,loading:0,controller:null,
  audio:null,source:null,gain:null,buffer:null,track:null,pressed:new Set(),action:0,worldMode:false,shotMode:false,gameSounds:null,stageMusic:null,background:null,soundCursor:0,soundWindow:null,messageMusicSerial:0,musicControlSerial:0,
  worldSnapshot:null,pendingCarryToken:0,handoff:false,handoffFailed:false,autoFlow:false,handoffGeneration:0,
  stageIntroLoaded:false,stageIntroStage:0,stageIntroCaptionKey:'',
  debugInfiniteRespawnRequested:false,
  endingHost:null,endingResources:null,endingLoading:false,endingBusy:false,endingPromise:null,endingStepPromise:null,endingTicket:0,
  flowCapabilityModule:null,flowReady:false,flowCapabilities:null,nativePause:null,nativePausePending:null,menuSoundLedger:null,manualPageLoaded:-1,replayStore:null,replaySlots:null,replayClockLast:null,replayNativeBound:false};
let practiceMenu=null,practiceLaunchOwner=null,thpracToolsPanel=null;
const thpracLogicClock=new Thprac128LogicClock(clock),thpracOptionsStore=new Thprac128Settings();
let thpracSettings=thpracOptionsStore.launch(launcherContext?.config??{});
state.thpracUIEnabled=thpracSettings.enabled;
const thpracDraft={mask:0,speed:validateThprac128Speed()};
const thpracDraftOwner=state.thpracDraftOwner=Object.freeze({pending:true,playback:false,snapshot:()=>({mask:thpracDraft.mask,frame:0}),speedFor:()=>thpracDraft.speed,setMask(mask){if(!Number.isInteger(mask)||mask<0||mask>127)throw new Error('Invalid practice mask');thpracDraft.mask=mask;},setSpeed(value){thpracDraft.speed=validateThprac128Speed(value);}});
let thpracRouteShown=false;
function syncThpracRouteMenu(snapshot){
 if(snapshot.page!==2){thpracRouteShown=false;return;}
 if(!snapshot.ready||snapshot.transitioning||state.launcherExited||!state.stageCatalog||!thprac128RuntimeSupported(state.module)||find('#variant')?.disabled===true)return;
 if(!thpracRouteShown&&thpracSettings.enabled&&practiceMenu&&!menu.launching&&!state.handoff&&!state.pendingCarryToken&&!state.nativePausePending&&!state.replayPlayback){thpracRouteShown=true;void practiceMenu.open();}
}
function scaleThpracViewport(){
 const screen=find('#game')?.parentElement,layer=find('#thprac-layer');if(!screen||!layer)return;
 const update=()=>layer.style.setProperty('--thprac-scale',String(screen.clientWidth/640));update();
 const observer=new ResizeObserver(update);observer.observe(screen);window.addEventListener('pagehide',()=>observer.disconnect(),{once:true});
}

function inheritThpracTools(selection){
 const prior=state.stageSelection,tools=state.thpracTools;
 if(!selection.thprac||!prior?.thprac||!tools||state.replayPlayback||selection.thprac.difficulty!==prior.thprac.difficulty||JSON.stringify(selection.thprac.params)!==JSON.stringify(prior.thprac.params))return selection;
 const s=tools.snapshot();selection.thpracRestartTools={mask:s.mask,speed:tools.speedFor(s.frame)};
 if(s.mask&64)selection.thpracRetainFrom=prior;return selection;
}
function handleThpracToolKey(event){
 if(!['Backspace','F1','F2','F3','F4','F5','F6','F7','F12'].includes(event.code)||!state.active||state.launcherExited||state.thpracUIEnabled===false)return false;
 event.preventDefault();if(event.repeat)return true;
 if(event.code==='Backspace'||event.code==='F12'){void thpracToolsPanel?.open(event.code==='F12');return true;}
 if(state.replayPlayback)return true;
 const tools=state.thpracTools??state.thpracDraftOwner;if(!tools)return true;
 try{const bit=1<<(Number(event.code.slice(1))-1);tools.setMask(tools.snapshot().mask^bit);syncThpracToolMusic();thpracToolsPanel?.refresh();}
 catch(e){status(e.message);}return true;
}
function syncThpracToolMusic(){
 const tools=state.thpracTools,selection=state.stageSelection;if(!tools||!selection?.thprac||!state.stageMusic)return;
 state.stageMusic.setPracticeOwner(selection,{enabled:!!(tools.snapshot().mask&64),section:selection.thprac.params.section,replay:!!state.replayPlayback});
}
function practiceLogicRate(){
 if(!state.thpracTools)return 60;const tools=state.thpracTools,s=tools.snapshot(),speed=tools.speedFor(s.frame);
 if(state.replayPlayback){if(state.pressed.has('ControlLeft')||state.pressed.has('ControlRight'))return speed.fast===21?15360:speed.fast*60;if(state.pressed.has('ShiftLeft')||state.pressed.has('ShiftRight'))return speed.slow;}
 return speed.fps;
}
function installThpracToolPanel(){
 if(thpracToolsPanel)return;thpracToolsPanel=mountThprac128ToolsPanel({root:find('#thprac-tool-ui'),getOwner:()=>state.thpracTools??thpracDraftOwner,getLocale:()=>thpracSettings.locale,
  onOpen:advanced=>{if(!advanced)return;state.thpracToolsPaused=state.paused;state.paused=true;state.pressed.clear();worldMovementInput(0,763);clock.reset();thpracLogicClock.reset();},
  onClose:advanced=>{if(!advanced)return;if(state.active&&!menu.pauseOpen&&!document.hidden){state.paused=state.thpracToolsPaused===true;}clock.reset();thpracLogicClock.reset();find('#game')?.focus();},
  onChange:()=>{syncThpracToolMusic();clock.reset();thpracLogicClock.reset();}
 });
}

// BEGIN HIGH_FRAMERATE_PRESENTATION
const highFramerateHistory=new WorldPresentationInterpolator();
let highFramerateEnabled=false,highFramerateOwner=null,highFramerateGeneration=0;
let highFramerateRaw=null,highFramerateCatchup=false;
function resetHighFrameratePresentation(){
  highFramerateHistory.reset();highFramerateOwner=null;highFramerateRaw=null;
}
function highFramerateEligible(){
  return highFramerateEnabled&&!highFramerateCatchup&&state.worldMode&&state.shotMode&&state.active&&!state.paused&&!state.handoff
    &&!state.endingHost&&!state.endingLoading&&!menu.pauseOpen&&!menu.fault&&!document.hidden;
}
function highFramerateOwnerMatches(){
  return highFramerateOwner?.module===state.module&&highFramerateOwner?.selection===state.stageSelection
    &&highFramerateOwner?.background===state.background;
}
function captureHighFramerateWorld(){
  if(!highFramerateEligible()){resetHighFrameratePresentation();return false;}
  if(!highFramerateOwnerMatches()){
    resetHighFrameratePresentation();++highFramerateGeneration;
    highFramerateOwner={module:state.module,selection:state.stageSelection,background:state.background};
  }
  const raw=readNativeWorldPresentation();
  const capture=typeof highFramerateHistory.captureOwned==='function'?highFramerateHistory.captureOwned.bind(highFramerateHistory):highFramerateHistory.capture.bind(highFramerateHistory);
  const captured=capture({generation:highFramerateGeneration,stage:state.stageSelection?.stage.id,frame:raw.world.frame,
    records:raw.records,background:state.background?.geometry()??[]});
  highFramerateRaw=raw;return captured;
}
function readNativeWorldPresentation(){
  const module=state.module,records=state.extraPresentation?.draws??(typeof readWorldDrawRecords==='function'?readWorldDrawRecords(module,{fastNumbers:true}):JSON.parse(module.UTF8ToString(module._th128_world_bullet_draws())));
  const drawError=module.UTF8ToString(module._th128_world_bullet_draw_error());if(drawError)throw new Error(drawError);
  const stageError=stageIntroDrawFailure(module);if(stageError)throw new Error(stageError);
  const world=JSON.parse(module.UTF8ToString(module._th128_world_state()));
  return {world,records};
}
function worldPresentationForDraw(){
  if(state.extraPresentation)return readNativeWorldPresentation();
  if(!highFramerateEligible())return readNativeWorldPresentation();
  if(!highFramerateOwnerMatches()||!highFramerateRaw)captureHighFramerateWorld();
  return highFramerateRaw;
}
function highFramerateWorldView(world,records,alpha=1){
  const current={records,background:state.background,interpolated:false};
  if(!highFramerateEligible()){resetHighFrameratePresentation();return current;}
  if(!highFramerateOwnerMatches()){resetHighFrameratePresentation();return current;}
  const sample=typeof highFramerateHistory.sampleForDraw==='function'?highFramerateHistory.sampleForDraw(alpha):highFramerateHistory.sample(alpha);
  if(!sample||sample.frame!==world.frame)return current;
  return {records:sample.records,background:state.background?{geometry:()=>sample.background}:null,interpolated:alpha<1};
}
function setHighFramerateMode(enabled){
  if(typeof enabled!=='boolean')throw new Error('Invalid high framerate mode');
  highFramerateEnabled=enabled;resetHighFrameratePresentation();originalPresentationFPS.reset();
  if(enabled)captureHighFramerateWorld();
}
// END HIGH_FRAMERATE_PRESENTATION
let lastCheckWorldError='';
const routeChoiceStages=new Set([1,6,11]);
const stageTransitionStages=new Set([1,2,4,6,7,9,11,12,14]);
const terminalStageIds=new Set([3,5,8,10,13,15]);
const routeChoiceMessage78=0,routeChoiceMessage80=0;
const type25SourceExeSha256='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4';
const type25ProfileSha256='080190229ed68ee1120238bb7308ae44c16cdc903c2a84fba65a7c273b2236dd';
const type25ProfileBytes=288;
const type13SourceExeSha256=type25SourceExeSha256;
const type13ProfileSha256='1227a00f3769a3ed69cbabcfff6212775f33e6468fa27c633ecbc9ee6789bf37';
const type13ProfileBytes=288;
const type13SourceAddress='0x004b3370';
const type13PerColor=[337,339,341,343,345,347,349,351,353,355,357,359,361,363,365,367];
const type6SourceExeSha256=type13SourceExeSha256;
const type6ProfileSha256='e26b6619e74337ad7e27614e55b095221b8555a8d8152f520e4c74d53c3a93f0';
const type6ProfileBytes=288;
const type6SourceAddress='0x4b2b90';
const type6ColorRemaps=[
  [96,323,336,337],[97,324,337,339],[98,324,338,341],[99,325,339,343],
  [100,325,340,345],[101,326,341,347],[102,326,342,349],[103,327,343,351],
  [104,327,344,353],[105,328,345,355],[106,328,346,357],[107,328,347,359],
  [108,329,348,361],[109,329,349,363],[110,329,350,365],[111,330,351,367]
];
const type6CancellationScripts=[337,339,341,343,345,347,349,351,353,355,357,359,361,363,365,367];
const type6Callback='per-color four-sprite remap table; no identity 0x40ccb0 callback';
const type6ProfileFields=new Set(['schema','type','sourceExeSha256','sourceAddress','bytes','sha256','hex',
  'script','secondaryScript','hitboxSize','auxiliarySize','appearance','selector','freezePolicy','opaque114',
  'identitySpriteRemap','colorRemaps','cancellationScripts','callback','supportedTypes']);
const type16SourceExeSha256=type6SourceExeSha256;
const type16ProfileSha256='af31469c811a57df49b774a459e776d84b8f3185b9f48147226f382b8617eee5';
const type16ProfileBytes=288;
const type16SourceAddress='0x4b36d0';
const type16ColorRemaps=[
  [288,323,336,337],[289,324,337,339],[290,324,338,341],[291,325,339,343],
  [292,325,340,345],[293,326,341,347],[294,326,342,349],[295,327,343,351],
  [296,327,344,353],[297,328,345,355],[298,328,346,357],[299,328,347,359],
  [300,329,348,361],[301,329,349,363],[302,329,350,365],[303,330,351,367]
];
const type16CancellationScripts=[337,339,341,343,345,347,349,351,353,355,357,359,361,363,365,367];
const type16Callback='per-color four-sprite remap table; no identity 0x40ccb0 callback';
const type30SourceExeSha256=type6SourceExeSha256;
const type30ProfileSha256='4950c5e4d6027c3d401708d3ca097ec0a337c642a1068b8287926423460ba5a3';
const type30ProfileBytes=288;
const type30SourceAddress='0x4b4690';
const type30ColorRemaps=[
  [-1,-1,0,373],[-1,-1,-1,0],[-1,-1,-1,0],[-1,-1,-1,0],
  [-1,-1,-1,0],[-1,-1,-1,0],[-1,-1,-1,0],[-1,-1,-1,0],
  [-1,-1,-1,0],[-1,-1,-1,0],[-1,-1,-1,0],[-1,-1,-1,0],
  [-1,-1,-1,0],[-1,-1,-1,0],[-1,-1,-1,0],[0,0,0,0]
];
const type30CancellationScripts=[381];
const type30Callback='identity sprite remap; native cancellation mode9 fixed script381';
const type32SourceExeSha256='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4';
const type32ProfileSha256='356fd3c4387f21411808cc690071305eda090991eb4a11be91ae6076c82c453b';
const type32ProfileBytes=288;
const type32SourceAddress='0x4b48d0';
const type32ProfileUrl='/assets/generations/356fd3c4387f21411808cc690071305e/zh/animations/bullet-a1-type32-profile.json';
const type32ColorRemaps=[[192,323,336,337],[193,324,337,339],[194,324,338,341],[195,325,339,343],[196,325,340,345],[197,326,341,347],[198,326,342,349],[199,327,343,351],[200,327,344,353],[201,328,345,355],[202,328,346,357],[203,328,347,359],[204,329,348,361],[205,329,349,363],[206,329,350,365],[207,330,351,367]];
const type32CancellationScripts=[337,339,341,343,345,347,349,351,353,355,357,359,361,363,365,367];
const type32Callback='per-color four-sprite remap table; cancellation mode6 per-color table';
const type32ProfileFields=new Set([...type6ProfileFields,'cacheMode']);
const type21SourceExeSha256=type6SourceExeSha256;
const type21ProfileSha256='785afc6af751f17007ce0c8bc620f4fa0082e1320cfa36ba7769bef6ff87ccd5';
const type21ProfileBytes=288;
const type21SourceAddress='0x4b3c70';
const type21ColorRemaps=[
  [392,323,0,369],[393,324,0,371],[394,325,0,373],[395,326,0,375],
  [396,327,0,377],[397,328,0,379],[398,329,0,381],[399,330,0,383]
];
const type21CancellationScripts=[369,371,373,375,377,379,381,383];
const type21Callback='per-color four-word sprite/cancellation table; no identity 0x40ccb0 callback';
const type21ProfileFields=new Set(['schema','type','sourceExeSha256','sourceAddress','bytes','sha256','hex',
  'script','secondaryScript','hitboxSize','auxiliarySize','appearance','selector','freezePolicy','opaque114',
  'identitySpriteRemap','colorRemaps','cancellationScripts','callback','supportedTypes']);
const stageTransitionContext={gate74:0,message78:0,sessionPresent:1,sessionField74:0};
const movementKeys=new Map([['ArrowUp',16],['ArrowDown',32],['ArrowLeft',64],['ArrowRight',128],['KeyZ',1],['KeyC',512],['ShiftLeft',8],['ShiftRight',8]]);
const perfectFreezeXBit=2;
const menu={
  page:'main',main:0,difficulty:1,route:0,option:0,music:0,pause:0,
  versionIndex:0,infoMode:'text',infoTitle:'INFO',infoDescription:'',infoReturn:'main',optionsInitialized:false,
  launching:false,launchId:0,launchReturn:'route',launchSelection:null,launchMessage:'',launchPreviousAutoFlow:false,
  audioUnlocked:false,audioUnlockPromise:null,suppressTitleAudio:false,musicRequestId:0,
  fault:false,pauseOpen:false,faultReason:'',
};
const menuDifficultyNames=['RANK 0 / EASY','RANK 1 / NORMAL','RANK 2 / HARD','RANK 3 / LUNATIC'];
const menuRouteStages=[1,6,11,null];
const nativeMainIds=['start','replay','result','music','options','manual','quit'];
const nativePageNames=['main','difficulty','route','options','keyconfig','music','replay','result','manual'];
const nativePageChoices=[7,4,4,5,7,10,25,1,9];
let nativeMainOrderReady=false;
const menuMusic={source:null,track:null,cache:new Map()};
const suppressedKeys=new Set();
function suppressHeldConfirmKeys(){
  for(const key of ['KeyZ','Enter'])if(state.pressed.has(key))suppressedKeys.add(key);
}
function preferenceNumber(key,min,max,fallback){
  try{
    const raw=sessionStorage.getItem('th128.'+key);
    if(raw===null||raw.trim()==='')return fallback;
    const value=Number(raw);
    return Number.isInteger(value)&&value>=min&&value<=max?value:fallback;
  }catch{return fallback;}
}
function preferenceText(key,fallback){
  try{return sessionStorage.getItem('th128.'+key)??fallback;}catch{return fallback;}
}
function savePreference(key,value){try{sessionStorage.setItem('th128.'+key,String(value));}catch{}}
menu.difficulty=preferenceNumber('difficulty',0,3,1);
menu.route=preferenceNumber('route',0,3,0);
menu.volume=preferenceNumber('volume',0,100,35);
state.debugInfiniteRespawnRequested=preferenceText('debugInfiniteRespawn','')==='true';
try{find('#volume').value=String(menu.volume);}catch{}
function debugRespawnSupported(){
  return typeof state.module?._th128_world_set_debug_infinite_respawn==='function';
}
function syncDebugRespawnControl(){
  const checkbox=find('#debug-infinite-respawn'),note=find('#debug-respawn-note');
  if(!checkbox)return;
  const supported=debugRespawnSupported();
  checkbox.disabled=!supported;
  if(!supported){
    checkbox.checked=false;checkbox.title='等待新 core：无限复活 API 尚未可用';
    if(note)note.textContent='等待新 core：无限复活 API 尚未可用；不会伪造启用。';
    return;
  }
  checkbox.checked=state.debugInfiniteRespawnRequested===true;
  checkbox.title='仅开发调试；非正常通关';
  if(note)note.textContent='中弹/死亡/复活动画仍按原规则；normal 验证不算。';
}
function gameplayFrameDetails(snapshot=state.worldSnapshot){
  if(!state.worldMode)return {route:null,stageId:null,frame:null,status:'title',text:'未进入关卡 · 原生帧 —'};
  const stageId=Number.isInteger(snapshot?.flow?.currentId)&&snapshot.flow.currentId>=1&&snapshot.flow.currentId<=16?snapshot.flow.currentId:null;
  const stage=state.stageCatalog?.stages.find(value=>value.id===stageId);
  const match=/^st_([abc][12])_([123])\.ecl$/.exec(stage?.ecl??'');
  const route=match?`${match[1].toUpperCase()}-${match[2]}`:stageId===16?'EX':null;
  const frame=Number.isSafeInteger(snapshot?.frame)&&snapshot.frame>=0?snapshot.frame:null;
  const stopped=menu.fault===true||state.handoffFailed===true;
  const status=stopped?'stopped':state.handoff?'handoff':state.paused?'paused':'running';
  const label=stopped?'已停止':state.handoff?'切关中':state.paused?'已暂停':'运行中';
  const tainted=snapshot?.debug?.runTainted===true||snapshot?.debug?.runTainted===1||state.stageSelection?.debugJump;
  return {route,stageId,frame,status,text:`${route??'路线未知'} · stage ${stageId??'未知'} · 原生帧 ${frame??'未知'} · ${label}${tainted?' · 调试':''}`};
}
function syncGameplayFrameUI(snapshot=state.worldSnapshot){
  const details=gameplayFrameDetails(snapshot);
  for(const selector of ['#game-frame-indicator','#runtime-overlay-frame']){
    const node=find(selector);if(!node)continue;
    if(node.textContent!==details.text)node.textContent=details.text;
    node.dataset.status=details.status;
  }
  return details;
}
function captureGameplayFrameUI(){
  if(state.worldMode){
    try{state.worldSnapshot=JSON.parse(state.module.UTF8ToString(state.module._th128_world_state()));}
    catch{state.worldSnapshot=null;}
  }
  return syncGameplayFrameUI();
}
function syncDebugRespawnUI(snapshot=state.worldSnapshot){
  const indicator=find('#debug-respawn-indicator');if(!indicator)return;
  const debug=state.worldMode?snapshot?.debug:null;
  const enabled=debug?.infiniteRespawn===true||debug?.infiniteRespawn===1;
  const tainted=debug?.runTainted===true||debug?.runTainted===1;
  const jump=state.worldMode?state.stageSelection?.debugJump:null;
  indicator.hidden=!(enabled||tainted||jump);
  indicator.textContent=jump?`调试跳关：${jump.route}（不计正常通关）${enabled?' · 无限复活':''}`:
    enabled?'调试：无限复活':tainted?'本局使用过调试复活':'';
}
function refreshDebugRespawnUIFromWorld(){
  if(!state.worldMode||!state.module?._th128_world_state){syncDebugRespawnUI(null);return;}
  try{const snapshot=JSON.parse(state.module.UTF8ToString(state.module._th128_world_state()));state.worldSnapshot=snapshot;syncDebugRespawnUI(snapshot);}catch{syncDebugRespawnUI(null);}
}
function setDebugInfiniteRespawn(enabled){
  const checkbox=find('#debug-infinite-respawn');
  if(typeof enabled!=='boolean')return false;
  if(!debugRespawnSupported()){
    if(checkbox)checkbox.checked=false;
    status('调试无限复活等待新 core；未启用。');return false;
  }
  const previous=state.debugInfiniteRespawnRequested===true;
  if(!state.module._th128_world_set_debug_infinite_respawn(enabled?1:0)){
    if(checkbox)checkbox.checked=previous;
    status(`调试无限复活${enabled?'启用':'关闭'}失败：${state.module.UTF8ToString(state.module._th128_world_error())||'native rejected'}`);
    return false;
  }
  state.debugInfiniteRespawnRequested=enabled;
  savePreference('debugInfiniteRespawn',enabled?'true':'false');
  if(checkbox)checkbox.checked=enabled;
  status(enabled?'调试无限复活已启用（非正常通关）；中弹/死亡/复活动画仍按原规则。':'调试无限复活已关闭；本局若已复活仍保留 taint。');
  resetHighFrameratePresentation();refreshDebugRespawnUIFromWorld();
  return true;
}
function applyDebugInfiniteRespawn(module=state.module,selection=null){
  const jump=selection?.debugJump;
  if(typeof module?._th128_world_set_debug_infinite_respawn!=='function'){
    if(jump)throw new Error('调试跳关需要原生 taint 接口；本次未启动');
    return;
  }
  const enabled=state.debugInfiniteRespawnRequested===true;
  if(jump&&!enabled&&!module._th128_world_set_debug_infinite_respawn(1))
    throw new Error(`调试跳关 taint 失败：${module.UTF8ToString(module._th128_world_error())||'native rejected'}`);
  if(module._th128_world_set_debug_infinite_respawn(enabled?1:0)){
    if(jump){
      const snapshot=JSON.parse(module.UTF8ToString(module._th128_world_state()));
      if(snapshot.frame!==0||snapshot.debug?.runTainted!==true)
        throw new Error('调试跳关必须在原生 WORLD0 标记 tainted；本次已停止');
    }
    return;
  }
  const message=module.UTF8ToString(module._th128_world_error())||'native rejected';
  state.debugInfiniteRespawnRequested=false;savePreference('debugInfiniteRespawn','false');
  const checkbox=find('#debug-infinite-respawn');if(checkbox)checkbox.checked=false;
  throw new Error(`调试无限复活配置失败：${message}`);
}

function debugThirdStageSelection(route){
  if(route==='EX'){
    const selection=prepareStage(16,{difficulty:4,seed:34772,progress:0,debugExtra:true});
    if(selection.stage?.ecl!=='st_ex.ecl')throw new Error('EX 原版关卡映射不匹配');
    return selection;
  }
  const routes={
    'A1-3':[3,'st_a1_3.ecl'],'A2-3':[5,'st_a2_3.ecl'],
    'B1-3':[8,'st_b1_3.ecl'],'B2-3':[10,'st_b2_3.ecl'],
    'C1-3':[13,'st_c1_3.ecl'],'C2-3':[15,'st_c2_3.ecl'],
  };
  if(typeof route!=='string'||!Object.hasOwn(routes,route))throw new Error('请选择六条路线之一的第三面');
  const [stageId,ecl]=routes[route];
  const selection=prepareStage(stageId,{difficulty:1,seed:34772,progress:0});
  if(selection.stage?.ecl!==ecl)throw new Error(`调试跳关原版关卡映射不匹配：${route}`);
  selection.debugJump={schema:'th128-debug-third-stage-start/1',route,stageId,difficulty:1,
    startingFrame:0,mode:'fresh-direct-stage',carryVerified:false,previousStagesVerified:false,normalModeClear:false};
  return selection;
}

function syncDebugStageJumpControls(){
  practiceMenu?.refresh();
  const select=find('#debug-stage-select'),button=find('#debug-stage-start'),note=find('#debug-stage-status');
  if(!select||!button)return;
  const ready=debugRespawnSupported()&&!!state.stageCatalog&&find('#variant')?.disabled!==true;
  const busy=menu.launching||state.handoff||!!state.pendingCarryToken;
  select.disabled=!ready||busy;button.disabled=!ready||busy;
  const extra=find('#debug-extra-start');
  if(extra){extra.disabled=!ready||busy;extra.dataset.runtimeReady=String(ready&&!busy);extra.setAttribute('aria-disabled',String(extra.disabled));}
  if(note&&!ready)note.textContent='等待原版资源与调试接口加载…';
  else if(note&&busy)note.textContent='正在加载或交接，请稍候…';
  else if(note&&!state.stageSelection?.debugJump)note.textContent='可选六路线第三面（Normal）或 EX（rank 4，调试中）';
}

async function startDebugThirdStage(route){
  if(menu.launching||state.handoff||state.pendingCarryToken)throw new Error('当前正在加载或交接，请稍后跳关');
  if(!debugRespawnSupported()||!state.stageCatalog||find('#variant')?.disabled===true)
    throw new Error('调试跳关尚未就绪，请等待资源加载');
  const selection=debugThirdStageSelection(route),note=find('#debug-stage-status');
  const label=selection.debugExtra?'EX · rank 4（调试中）':`${route} · Normal`;
  if(note)note.textContent=`正在进入 ${label}…`;
  try{
    await startLaunch(selection,'route');
    if(lastCheckWorldError||!state.worldMode||state.stageSelection!==selection)
      throw new Error(lastCheckWorldError||'调试跳关未进入目标第三面');
    const snapshot=JSON.parse(state.module.UTF8ToString(state.module._th128_world_state()));
    if(snapshot.flow?.currentId!==selection.stage.id||snapshot.flow?.difficulty!==selection.difficulty||snapshot.debug?.runTainted!==true)
      throw new Error('调试跳关原生关卡或 taint 校验失败，已停止');
    syncDebugRespawnUI(snapshot);
    if(note)note.textContent=`${label} · 调试跳关，不计正常通关`;
    return {route,stageId:selection.stage.id,difficulty:selection.difficulty,frame:snapshot.frame,nativeTainted:true,
      carryVerified:false,normalModeClear:false};
  }catch(problem){
    if(state.stageSelection===selection){state.active=false;state.paused=true;clock.reset();resetHighFrameratePresentation();}
    if(note)note.textContent=`调试跳关失败：${problem.message??String(problem)}`;
    throw problem;
  }finally{syncDebugStageJumpControls();}
}
async function startPracticeStage(route,{signal,isCurrent}){
 const current=()=>!signal.aborted&&isCurrent();
 if(!current())throw new DOMException('Practice owner exited','AbortError');
 if(practiceLaunchOwner)throw new Error('Practice launch is already pending');
 const owner={};practiceLaunchOwner=owner;let selection=null,launchId=null;
 const abort=()=>{if(practiceLaunchOwner===owner&&menu.launching&&menu.launchId===launchId&&menu.launchSelection===selection)cancelLaunch();};
 try{
  const pending=startDebugThirdStage(route);selection=menu.launchSelection;launchId=menu.launchId;
  signal.addEventListener('abort',abort,{once:true});if(!current())abort();
  let receipt;try{receipt=await pending;}catch(error){if(!current())throw new DOMException('Practice owner exited','AbortError');throw error;}
  if(!current()||menu.launchId!==launchId||state.stageSelection!==selection)throw new DOMException('Practice launch owner changed','AbortError');
  return receipt;
 }finally{signal.removeEventListener('abort',abort);if(practiceLaunchOwner===owner)practiceLaunchOwner=null;}
}
function thpracStageSelection(input){
 if(input?.schema!=='eagler-touhou/thprac-session/1'||input.game!=='th128')throw new Error('TH128 thprac session schema mismatch');
 const session=createThprac128Session(input.params,input.difficulty),p=session.params;
 const selection=prepareStage(p.stage+1,{difficulty:session.difficulty,seed:34772,progress:0,debugExtra:p.mode===1&&p.stage===15});
 if(p.mode===1){delete selection.debugJump;selection.thprac=session;}
 return {selection,session};
}
async function startThprac128Session(input,{signal,isCurrent}){
 const current=()=>!signal.aborted&&isCurrent();
 if(!current())throw new DOMException('Practice owner exited','AbortError');
 if(practiceLaunchOwner||menu.launching||state.handoff||state.pendingCarryToken||state.nativePausePending)
  throw new Error('当前正在加载或已有 pending practice launch');
 if(state.launcherExited||!state.stageCatalog||find('#variant')?.disabled===true||!thprac128RuntimeSupported(state.module))
  throw new Error('TH128 thprac runtime 尚未就绪');
 const {selection,session}=thpracStageSelection(input),owner={};practiceLaunchOwner=owner;let launchId=null;
 if(session.params.mode===1)inheritThpracTools(selection);
 const abort=()=>{if(practiceLaunchOwner===owner&&menu.launching&&menu.launchId===launchId&&menu.launchSelection===selection)cancelLaunch();};
 try{
  const pending=startLaunch(selection,'route');launchId=menu.launchId;
  signal.addEventListener('abort',abort,{once:true});if(!current())abort();
  await pending;
  if(!current()||menu.launchId!==launchId)throw new DOMException('Practice launch owner changed','AbortError');
  if(lastCheckWorldError||!state.worldMode)throw new Error(lastCheckWorldError||'TH128 practice did not enter its selected World');
  if(state.stageSelection!==selection)throw new DOMException('Practice launch owner changed','AbortError');
  const native=session.params.mode===1?readThprac128Native(state.module,session):null;
  return {session,stageId:selection.stage.id,difficulty:session.difficulty,generation:native?.generation??null,
   nativeTainted:session.params.mode===1,normalModeClear:false};
 }catch(error){
  if(state.stageSelection===selection){state.active=false;state.paused=true;clock.reset();resetHighFrameratePresentation();}
  throw error;
 }finally{signal.removeEventListener('abort',abort);if(practiceLaunchOwner===owner)practiceLaunchOwner=null;syncDebugStageJumpControls();}
}
async function thpracReplayResourceIdentity(module,byteCache){
 const coreSHA256=checkpointModuleBuilds.get(module)?.artifacts?.['core.wasm']?.sha256;
 if(!/^[a-f0-9]{64}$/.test(coreSHA256??''))throw new Error('Practice Replay requires the actual compiled core SHA');
 const rows=[];
 for(const [name,pending]of [...byteCache].sort((a,b)=>a[0].localeCompare(b[0]))){if(name==='runtime/title.anm'||name==='runtime/title_v.anm')continue;const bytes=await pending;if(!(bytes instanceof Uint8Array)||!bytes.length)throw new Error('Practice resource identity bytes unavailable');rows.push([name,[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(v=>v.toString(16).padStart(2,'0')).join('')]);}
 const resourceSHA256=[...new Uint8Array(await crypto.subtle.digest('SHA-256',new TextEncoder().encode(JSON.stringify(rows))))].map(v=>v.toString(16).padStart(2,'0')).join('');
 return Object.freeze({coreSHA256,resourceSHA256});
}
async function refreshThpracReplaySlots(){
 const select=find('#thprac-replay-slot');if(!select)return;
 const module=state.module,action=state.action,selection=state.stageSelection;
 const rows=await (state.replayStore??=new OwnedReplayStore()).observe();
 if(module!==state.module||action!==state.action||selection!==state.stageSelection||state.launcherExited)return;
 const prior=select.value;select.replaceChildren(...rows.map((row,slot)=>{const o=document.createElement('option');o.value=String(slot);o.textContent='No.'+String(slot+1).padStart(2,'0')+(row?' · '+(row.source==='thprac-rpy'?'thprac':'Replay'):' · —');return o;}));select.value=/^(?:[0-9]|1[0-9]|2[0-4])$/.test(prior)?prior:'0';state.thpracReplayRows=rows;
 find('#thprac-replay-overwrite').checked=false;find('#thprac-replay-save').disabled=!state.thpracRecorder;find('#thprac-replay-watch').disabled=false;
}
async function saveThpracReplayFromMenu(){
 const owner=state.thpracRecorder,module=state.module,action=state.action,selection=state.stageSelection;
 if(!owner||!selection?.thprac||state.replayPlayback||state.thpracReplaySaving)throw new Error('Practice recording owner unavailable or busy');
 const slot=Number(find('#thprac-replay-slot').value),observed=state.thpracReplayRows?.[slot];
 if(!Number.isInteger(slot)||slot<0||slot>24)throw new Error('Practice Replay slot outside25entries');
 if(observed&&!find('#thprac-replay-overwrite').checked)throw new Error('请先确认覆盖所选录像槽');
 const nameText=find('#thprac-replay-name').value;if(!/^[\x20-\x7e]{1,8}$/.test(nameText))throw new Error('练习录像姓名须为1..8个ASCII字符');
 const name=new TextEncoder().encode(nameText.padEnd(8,' ')),current=()=>owner===state.thpracRecorder&&module===state.module&&action===state.action&&selection===state.stageSelection&&!state.launcherExited;
 state.thpracReplaySaving=true;
 try{const bytes=owner.capture(BigInt(Math.floor(Date.now()/1000)),name),store=state.replayStore??=new OwnedReplayStore();const saved=await store.saveThprac(slot,bytes,{expectedDigest:observed?.digest??null,isCurrent:current});
  if(!current())throw new DOMException('Practice save owner exited','AbortError');
  await refreshThpracReplaySlots();find('#thprac-replay-status').textContent='练习录像已保存 No.'+String(slot+1).padStart(2,'0')+' · 参数/完整RPY已回读';return saved;
 }finally{state.thpracReplaySaving=false;}
}
async function watchThpracReplayFromMenu(slotOverride=null){
 if(menu.launching||state.handoff||state.thpracReplaySaving||state.replayPlayback)throw new Error('Replay playback owner is busy');
 const slot=slotOverride===null?Number(find('#thprac-replay-slot').value):slotOverride,store=state.replayStore??=new OwnedReplayStore(),module=state.module,action=state.action;
 const row=(await store.observe())[slot];if(!row?.bytes||row.digest!==await store.hash(row.bytes))throw new Error('所选练习录像为空或文件SHA不符');
 const metadata=readThprac128Replay(row.bytes);if(!metadata)throw new Error('所选录像不是带TH128 thprac参数的练习录像');
 if(module!==state.module||action!==state.action||state.launcherExited)throw new DOMException('Practice replay page changed','AbortError');
 const owner=new Thprac128ReplayPlayback(row.bytes,slot),next=owner.firstSelection(),selection=thpracStageSelection(next.thprac).selection;
 selection.seed=next.seed;selection.thpracReplayIdentity=owner.identity;state.replayPlayback=owner;state.replayFinished=false;state.thpracRecorder?.close();state.thpracRecorder=null;practiceMenu?.close({focus:false});
 try{await startLaunch(selection,'route');if(state.replayPlayback!==owner||lastCheckWorldError||state.stageSelection!==selection||!state.worldMode)throw new Error(lastCheckWorldError||'Practice replay did not adopt its recorded stage');return owner;}
 catch(error){if(state.replayPlayback===owner){try{owner.cancel();}catch{}state.replayPlayback=null;state.active=false;state.paused=true;}throw error;}
}
function installThpracReplayControls(){
 const controls=find('#thprac-web-controls');
 controls?.addEventListener('toggle',()=>{if(controls.open)refreshThpracReplaySlots().catch(error=>{find('#thprac-replay-status').textContent=String(error.message??error);});});
 for(const [id,run]of [['#thprac-replay-save',saveThpracReplayFromMenu],['#thprac-replay-watch',watchThpracReplayFromMenu]])find(id)?.addEventListener('click',()=>{run().catch(error=>{find('#thprac-replay-status').textContent=String(error.message??error);});});
}
function installPracticeMenu(){
 if(practiceMenu)return practiceMenu;installThpracToolPanel();scaleThpracViewport();
 practiceMenu=mountThprac128Menu({root:find('#practice-menu'),controls:find('#thprac-web-controls'),opener:find('#practice-open'),settings:thpracSettings,onSettings:value=>{thpracSettings=thpracOptionsStore.write(value);state.thpracUIEnabled=value.enabled;},
  getAvailability:()=>({
   ready:!state.launcherExited&&thprac128RuntimeSupported(state.module)&&!!state.stageCatalog&&find('#variant')?.disabled!==true,
   busy:menu.launching||state.handoff||!!state.pendingCarryToken||!!state.nativePausePending||!!state.normalPostrun||!!state.endingHost||state.endingLoading||!!state.replayPlayback,
   inRun:state.worldMode&&state.active,
   practiceSession:state.worldMode?state.stageSelection?.thprac??null:null
  }),
  onOpen:async()=>{loseFocus();await state.nativePausePending;await refreshThpracReplaySlots();},
  onFocusGame:()=>find('#game')?.focus(),launch:startThprac128Session
 });
 window.addEventListener('pagehide',()=>practiceMenu?.dispose(),{once:true});
 return practiceMenu;
}
function menuPageNodes(){return [...document.querySelectorAll('[data-menu-page]')];}
function menuRows(page=menu.page){
  if(page==='main')return [...document.querySelectorAll('[data-menu-main]')];
  if(page==='difficulty')return [...document.querySelectorAll('[data-menu-difficulty]')];
  if(page==='route')return [...document.querySelectorAll('[data-menu-route]')];
  if(page==='pause')return [...document.querySelectorAll('[data-menu-pause]')];
  if(page==='options')return [...document.querySelectorAll('#menu-option-list [data-menu-option]')];
  if(page==='info'&&menu.infoMode==='music')return [...document.querySelectorAll('#menu-music-list [data-menu-music]')];
  return [];
}
function menuIndex(page=menu.page){
  if(page==='main')return menu.main;if(page==='difficulty')return menu.difficulty;
  if(page==='route')return menu.route;if(page==='pause')return menu.pause;
  if(page==='options')return menu.option;if(page==='info'&&menu.infoMode==='music')return menu.music;
  return 0;
}
function setMenuIndex(page,index){
  const rows=menuRows(page);if(!rows.length)return;
  const bounded=Math.max(0,Math.min(rows.length-1,index));
  if(page==='main')menu.main=bounded;else if(page==='difficulty')menu.difficulty=bounded;
  else if(page==='route')menu.route=bounded;else if(page==='pause')menu.pause=bounded;
  else if(page==='options')menu.option=bounded;else if(page==='info')menu.music=bounded;
  updateMenuSelection();
}
function updateMenuSelection(){
  const rows=menuRows(),index=menuIndex();
  rows.forEach((row,rowIndex)=>{
    const selected=rowIndex===index;
    row.classList.toggle('selected',selected);row.setAttribute('aria-selected',String(selected));
  });
  const selected=rows[index];
  find('#menu-screenreader').textContent=`${find('#menu-heading').textContent}。${find('#menu-description').textContent} 当前：${selected?.textContent.trim()??''}`;
}
function menuCopy(heading,description,footer='↑↓ 选择 · Z/ENTER 确认 · X/ESC 返回'){
  find('#menu-heading').textContent=heading;
  find('#menu-description').textContent=description;
  find('#menu-footer').textContent=footer;
}
function updateMenuCopy(){
  if(menu.page==='main')menuCopy('MAIN MENU','START 进入难度与起点选择。REPLAY/RESULT 尚未实现；MUSIC 使用实际 manifest 曲目。EX 请使用页面顶部的独立调试入口。');
  else if(menu.page==='difficulty')menuCopy('DIFFICULTY',`${menuDifficultyNames[menu.difficulty]} · 四档普通难度 0..3；EX 使用独立 rank 4 调试入口，不能当作 Normal。`);
  else if(menu.page==='route')menuCopy('START MAP',`A/B/C 起点：stage 1 / 6 / 11 · 当前 ${menuDifficultyNames[menu.difficulty]}。只启动起点，不代表整条路线已完成。`);
  else if(menu.page==='options')menuCopy('OPTIONS','←→ 调整当前项，Z/ENTER 执行；音量/全屏/版本使用现有网页行为，并保存本次会话选择。');
  else if(menu.page==='manual')menuCopy('MANUAL','键盘说明与当前实现边界。X/ESC 返回主菜单。');
  else if(menu.page==='pause')menuCopy('PAUSED','游戏已暂停。RESUME 继续，RESTART 重开当前起点，TITLE 返回动画标题。');
  else if(menu.page==='launch')menuCopy('LAUNCHING',menu.launchMessage??'正在预加载选中的关卡…',menu.launching?'X/ESC 取消启动':'X/ESC 返回选择');
  else if(menu.page==='info'){
    menuCopy(menu.infoTitle,menu.infoDescription??'当前模式没有伪造结果。','X/ESC 返回主菜单');
    find('#menu-info-body').textContent=menu.infoDescription??'';
  }
}
function showMenuPage(page){
  menu.page=page;
  for(const section of menuPageNodes())section.hidden=section.dataset.menuPage!==page;
  find('#game-menu').hidden=true;find('#game-menu').setAttribute('aria-hidden','true');
  find('#runtime-overlay').hidden=true;
  if(page==='options')buildOptions();
  if(page==='info'&&menu.infoMode==='music')populateMusicRows();
  updateMenuCopy();updateMenuSelection();
}
function setInfoPage(title,description,returnPage='main'){
  menu.infoMode='text';menu.infoTitle=title;menu.infoDescription=description;menu.infoReturn=returnPage;
  find('#menu-info-title').textContent=title;
  find('#menu-info-body').textContent=description;
  find('#menu-music-list').hidden=true;
  find('#menu-info-body').hidden=false;
  showMenuPage('info');
}

function worldMovementInput(pressed=0,released=0){
  if(!state.worldMode||!state.module?._th128_world_input)return;
  if(state.replayPlayback){
    const replay=state.replayPlayback.snapshot();
    if(replay.auxiliaryKnown&&replay.auxiliaryLive){const held=state.pressed.has('KeyZ')||state.pressed.has('Enter')?1:0;state.module._th128_world_input(held,pressed&1,released&1);}
    return;
  }
  if(!supportsPerfectFreezeX()){pressed&=~perfectFreezeXBit;released&=~perfectFreezeXBit;}
  let holding=0;for(const [code,bit] of movementKeys)if(state.pressed.has(code)&&(state.shotMode||(bit&240)!==0))holding|=bit;
  if(gameplayXHeld())holding|=perfectFreezeXBit;
  if(!state.shotMode){pressed&=240;released&=240;}
  if(state.extraRun){state.extraRun.queueInput(holding,pressed,released);return;}
  if(!state.module._th128_world_input(holding,pressed,released))status(state.module.UTF8ToString(state.module._th128_world_error()));
}

function status(message){find('#status').textContent=message;}
function runtimeOverlay(message,paused=false){
  if(!state.worldMode)return;
  find('#runtime-overlay-title').textContent=paused?'游戏已暂停':'运行已停止';
  find('#runtime-overlay-message').textContent=message;
  find('#resume-session').hidden=!paused;find('#runtime-overlay').hidden=false;
}
function faultOverlay(){
  syncGameplayFrameUI();
  const restart=menu.pause===1?'> ':'  ';
  const title=menu.pause===2?'> ':'  ';
  find('#runtime-overlay-title').textContent='运行已停止';
  find('#runtime-overlay-message').textContent=`${menu.faultReason}\n\n↑↓ 选择 · Z/ENTER 执行 · X/ESC 返回标题\n${restart}RESTART 重新开始\n${title}TITLE 返回标题`;
  find('#resume-session').hidden=true;
  find('#runtime-overlay').hidden=false;
}
function enterFaultMenu(reason){
  if(!state.worldMode)return;
  state.active=false;state.paused=true;state.pressed.clear();clock.reset();resetHighFrameratePresentation();
  if(state.audio?.state==='running')state.audio.suspend().catch(()=>{});
  worldMovementInput(0,761);
  menu.page='pause';menu.fault=true;menu.pauseOpen=true;menu.pause=1;menu.faultReason=reason;
  captureGameplayFrameUI();
  find('#game-menu').hidden=true;find('#game-menu').setAttribute('aria-hidden','true');
  find('#error').hidden=false;find('#error').textContent=reason;
  faultOverlay();
  status('运行停止；↑↓ 选择 RESTART/TITLE，Z/ENTER 执行，X/ESC 返回标题。');
}
function restartFaultWorld(){
  if(!state.stageSelection){status('没有可重建的 stage selection');faultOverlay();return;}
  const selection=state.stageSelection;
  menu.fault=false;menu.pauseOpen=false;menu.page='launch';
  find('#runtime-overlay').hidden=true;
  startLaunch(selection,'fault');
}
function handleFaultKey(code){
  if(!menu.fault)return false;
  if(code==='ArrowUp'||code==='ArrowLeft'||code==='ArrowDown'||code==='ArrowRight'){
    menu.pause=code==='ArrowDown'||code==='ArrowRight'?2:1;faultOverlay();return true;
  }
  if(code==='KeyZ'||code==='Enter'){
    if(menu.pause===2)returnToTitle();else restartFaultWorld();
    return true;
  }
  if(code==='KeyX'||code==='Escape'){returnToTitle();return true;}
  return true;
}
function error(problem){state.active=false;state.paused=true;
  if(state.audio?.state==='running')state.audio.suspend().catch(()=>{});
  if(!state.module){startupFailure(problem);return;}
  find('#loading').hidden=true;find('#error').hidden=false;find('#error').textContent=problem.message??String(problem);
  if(state.worldMode){enterFaultMenu(problem.message??String(problem));return;}
  runtimeOverlay(problem.message??String(problem));
  status('运行停止；请查看诊断，不会静默跳过。');}
function coreError(){return state.module?.UTF8ToString(state.module._th128_error())??'';}
function flowCapabilities(){
  const module=state.module;
  if(!module)return null;
  if(state.flowCapabilityModule===module)return state.flowCapabilities;
  state.flowCapabilityModule=module;state.flowReady=false;state.flowCapabilities=null;
  if(typeof module._th128_world_flow_capabilities!=='function')return null;
  try{
    const text=module.UTF8ToString(module._th128_world_flow_capabilities());
    state.flowCapabilities=text?JSON.parse(text):null;
  }catch{state.flowCapabilities=null;}
  return state.flowCapabilities;
}
function flowCapabilitiesReady(){
  const module=state.module,capabilities=flowCapabilities();
  if(!module||!capabilities)return false;
  if(capabilities.protocol!==1||capabilities.opaqueCarry!==true||capabilities.tokenAck!==true
    ||capabilities.ordinaryTransitions!==true||capabilities.terminalTransitions!==false)return false;
  for(const name of ['capture_stage_carry','bind_stage_carry_target','restore_stage_carry_random','restore_stage_carry',
    'acknowledge_stage_transition','discard_stage_carry','enable_stage_transition']){
    if(typeof module['_th128_world_'+name]!=='function')return false;
  }
  state.flowReady=true;return true;
}
function incomingCarryRoute(stageId,carryToken,capabilities){
  if(!carryToken)return null;
  if(stageTransitionStages.has(stageId))return 'ordinary';
  if(terminalStageIds.has(stageId)&&capabilities?.terminalCarry===true)return 'terminal';
  return null;
}
function canRestoreCarryScalars(route,transitionEnabled){
  return route==='terminal'||(route==='ordinary'&&transitionEnabled===true);
}
function preparedType9Profile(){
  return {"schema":"th128-original-typed-bullet-profile/1","type":9,"sourceExeSha256":"ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4","sourceAddress":"0x004b2ef0","bytes":288,"sha256":"6597ee65d7f9d92a503164ea438f0e1b61e133f1e647a6da6be2c55bfcb2cf81","hex":"2c000000a0000000430100005001000051010000a1000000440100005101000053010000a2000000440100005201000055010000a3000000450100005301000057010000a4000000450100005401000059010000a500000046010000550100005b010000a600000046010000560100005d010000a700000047010000570100005f010000a8000000470100005801000061010000a9000000480100005901000063010000aa000000480100005a01000065010000ab000000480100005b01000067010000ac000000490100005c01000069010000ad000000490100005d0100006b010000ae000000490100005e0100006d010000af0000004a0100005f0100006f0100009a9919400000804104000000060000001e0000000000000000000000"};
}
function supportsBulletType9(){
  const capabilities=flowCapabilities();
  return !!state.module?._th128_world_add_bullet_profile
    &&Array.isArray(capabilities?.supportedBulletTypes)&&capabilities.supportedBulletTypes.includes(9);
}
async function type9ProfileBytesFromRecord(profile){
  if(profile?.schema!=='th128-original-typed-bullet-profile/1'||profile.type!==9
    ||profile.sourceExeSha256!=='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
    ||profile.sourceAddress!=='0x004b2ef0'||profile.bytes!==288
    ||profile.sha256!=='6597ee65d7f9d92a503164ea438f0e1b61e133f1e647a6da6be2c55bfcb2cf81'
    ||typeof profile.hex!=='string'||!/^[0-9a-f]{576}$/.test(profile.hex))
    throw new Error('Invalid original type9 bullet profile provenance');
  const bytes=Uint8Array.from(profile.hex.match(/../g),value=>parseInt(value,16));
  const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
  if(digest!==profile.sha256)throw new Error('Original type9 profile checksum mismatch');
  return bytes;
}
async function loadType9ProfileForWorld({supports,readProfile,module,memory}){
  if(!supports())return false;
  const bytes=await type9ProfileBytesFromRecord(await readProfile()),pointer=memory(bytes);
  try{
    if(!module._th128_world_add_bullet_profile(9,pointer,bytes.length))
      throw new Error(module.UTF8ToString(module._th128_world_error())||'Original type9 profile rejected');
  }finally{module._free(pointer);}
  return true;
}
function supportsBulletType25(){
  const capabilities=flowCapabilities();
  return !!state.module?._th128_world_add_bullet_profile
    &&Array.isArray(capabilities?.supportedBulletTypes)&&capabilities.supportedBulletTypes.includes(25);
}
function supportsBulletType13(){
  const capabilities=flowCapabilities();
  return !!state.module?._th128_world_add_bullet_profile
    &&Array.isArray(capabilities?.supportedBulletTypes)&&capabilities.supportedBulletTypes.includes(13);
}
function supportsBulletType6(){
  const capabilities=flowCapabilities();
  return !!state.module?._th128_world_add_bullet_profile
    &&Array.isArray(capabilities?.supportedBulletTypes)&&capabilities.supportedBulletTypes.includes(6);
}
function supportsBulletType21(){
  const capabilities=flowCapabilities();
  return !!state.module?._th128_world_add_bullet_profile
    &&Array.isArray(capabilities?.supportedBulletTypes)&&capabilities.supportedBulletTypes.includes(21);
}
function supportsBulletType16(){
  const capabilities=flowCapabilities();
  return !!state.module?._th128_world_add_bullet_profile
    &&Array.isArray(capabilities?.supportedBulletTypes)&&capabilities.supportedBulletTypes.includes(16);
}
function supportsBulletType30(){
  const capabilities=flowCapabilities();
  return !!state.module?._th128_world_add_bullet_profile
    &&Array.isArray(capabilities?.supportedBulletTypes)&&capabilities.supportedBulletTypes.includes(30);
}
function supportsBulletType24(){
  const capabilities=flowCapabilities();
  return !!state.module?._th128_world_add_bullet_profile
    &&Array.isArray(capabilities?.supportedBulletTypes)&&capabilities.supportedBulletTypes.includes(24);
}
function supportsBulletType32(){
  const capabilities=flowCapabilities();
  return !!state.module?._th128_world_add_bullet_profile
    &&Array.isArray(capabilities?.supportedBulletTypes)&&capabilities.supportedBulletTypes.includes(32);
}
async function type24ProfileBytesFromRecord(profile){
  if(profile?.schema!=='th128-original-typed-bullet-profile/1'||profile.type!==24
    ||profile.sourceExeSha256!=='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
    ||profile.sourceAddress!=='0x004b3fd0'||profile.bytes!==288
    ||profile.sha256!=='92aae4cc2ea4e28e3bf0f5e1d7b7eac75d0577ef27311ab5a0d825434bd7dfd3'
    ||typeof profile.hex!=='string'||!/^[0-9a-f]{576}$/.test(profile.hex)
    ||profile.script!==283||profile.secondaryScript!==284||profile.hitboxSize!==6
    ||profile.auxiliarySize!==22||profile.appearance!==2||profile.selector!==7
    ||profile.freezePolicy!==1||profile.cancellationScript!==375||profile.identitySpriteRemap!==true
    ||!Array.isArray(profile.supportedTypes)||profile.supportedTypes.length!==1||profile.supportedTypes[0]!==24)
    throw new Error('Invalid original type24 bullet profile provenance');
  const bytes=Uint8Array.from(profile.hex.match(/../g),value=>parseInt(value,16));
  if(bytes.length!==288)throw new Error('Invalid original type24 bullet profile byte length');
  const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
  if(digest!==profile.sha256)throw new Error('Original type24 bullet profile checksum mismatch');
  return bytes;
}
async function type30ProfileBytesFromRecord(profile){
  if(profile?.schema!=='th128-original-typed-bullet-profile/1'||profile.type!==30
    ||profile.sourceExeSha256!==type30SourceExeSha256||profile.sourceAddress!==type30SourceAddress
    ||profile.bytes!==type30ProfileBytes||profile.sha256!==type30ProfileSha256
    ||typeof profile.hex!=='string'||!/^[0-9a-f]{576}$/.test(profile.hex)
    ||profile.script!==285||profile.secondaryScript!==286||profile.hitboxSize!==4
    ||profile.auxiliarySize!==22||profile.appearance!==2||profile.selector!==6
    ||profile.freezePolicy!==1||profile.opaque114!==60||profile.identitySpriteRemap!==true
    ||profile.callback!==type30Callback
    ||!Array.isArray(profile.colorRemaps)||profile.colorRemaps.length!==type30ColorRemaps.length
    ||profile.colorRemaps.some((row,color)=>!Array.isArray(row)||row.length!==4
      ||row.some((value,sprite)=>value!==type30ColorRemaps[color][sprite]))
    ||!Array.isArray(profile.cancellationScripts)||profile.cancellationScripts.length!==type30CancellationScripts.length
    ||profile.cancellationScripts.some((value,color)=>value!==type30CancellationScripts[color])
    ||!Array.isArray(profile.supportedTypes)||profile.supportedTypes.length!==1||profile.supportedTypes[0]!==30
    ||Object.keys(profile).some(key=>!type6ProfileFields.has(key)))
    throw new Error('Invalid original type30 bullet profile provenance');
  const bytes=Uint8Array.from(profile.hex.match(/../g),value=>parseInt(value,16));
  if(bytes.length!==type30ProfileBytes)throw new Error('Invalid original type30 bullet profile byte length');
  const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
  if(digest!==type30ProfileSha256)throw new Error('Original type30 bullet profile checksum mismatch');
  return bytes;
}
async function type32ProfileBytesFromRecord(profile){
  if(profile?.schema!=='th128-original-typed-bullet-profile/1'||profile.type!==32
    ||profile.sourceExeSha256!==type32SourceExeSha256||profile.sourceAddress!==type32SourceAddress
    ||profile.bytes!==type32ProfileBytes||profile.sha256!==type32ProfileSha256
    ||typeof profile.hex!=='string'||!/^[0-9a-f]{576}$/.test(profile.hex)
    ||profile.script!==54||profile.secondaryScript!==0||profile.hitboxSize!==3.2
    ||profile.auxiliarySize!==16||profile.appearance!==4||profile.selector!==6
    ||profile.freezePolicy!==0||profile.opaque114!==30||profile.cacheMode!==6
    ||profile.identitySpriteRemap!==false||profile.callback!==type32Callback
    ||!Array.isArray(profile.colorRemaps)||profile.colorRemaps.length!==16
    ||profile.colorRemaps.some((row,color)=>!Array.isArray(row)||row.length!==4
      ||row.some((value,sprite)=>value!==type32ColorRemaps[color][sprite]))
    ||!Array.isArray(profile.cancellationScripts)||profile.cancellationScripts.length!==16
    ||profile.cancellationScripts.some((value,color)=>value!==type32CancellationScripts[color])
    ||!Array.isArray(profile.supportedTypes)||profile.supportedTypes.length!==1||profile.supportedTypes[0]!==32
    ||Object.keys(profile).some(key=>!type32ProfileFields.has(key)))
    throw new Error('Invalid original type32 bullet profile provenance');
  const bytes=Uint8Array.from(profile.hex.match(/../g),value=>parseInt(value,16));
  if(bytes.length!==type32ProfileBytes)throw new Error('Invalid original type32 bullet profile byte length');
  const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
  if(digest!==type32ProfileSha256)throw new Error('Original type32 bullet profile checksum mismatch');
  return {bytes,snapshot:{type:32,script:54,secondaryScript:0,hitboxSize:3.2,auxiliarySize:16,
    appearance:4,freezePolicy:0,identitySpriteRemap:false,cacheMode:6,
    colorRemaps:profile.colorRemaps.map(row=>[...row]),cancellationScripts:[...profile.cancellationScripts],
    spriteRemaps:[...profile.colorRemaps[6]]}};
}
async function loadType32ProfileForWorld({supports,readProfile,module,memory}){
  if(!supports())return null;
  const converted=await type32ProfileBytesFromRecord(await readProfile());
  const pointer=memory(converted.bytes);
  const loaded=module._th128_world_add_bullet_profile(32,pointer,converted.bytes.length);
  module._free(pointer);
  if(!loaded)throw new Error(module.UTF8ToString(module._th128_world_error())||'type32 bullet profile load failed');
  return converted.snapshot;
}
function supportsPerfectFreezeX(){
  return flowCapabilities()?.perfectFreezeNormalEmptyLasers===true;
}
function stageIntroTrackNumber(name){
  const match=/^th128_(\d\d)\.wav$/.exec(name??'');
  return match?Number(match[1]):-1;
}
function stageLogoRecord(stage){
  const id=stage?.id,logo=stage?.logo;
  if(!Number.isInteger(id)||id<1||id>16||typeof logo!=='string'||!/^logo_[a-z0-9_]+\.anm$/.test(logo))
    throw new Error('Stage intro requires the canonical stage id and stage logo asset');
  return {id,logo,metadata:`animations/${logo.slice(0,-4)}.json`,bytes:`runtime/${logo}`};
}
function supportsStageIntro(){
  const module=state.module;
  return !!module&&typeof module._th128_world_load_stage_intro==='function'
    &&typeof module._th128_world_track_caption==='function'
    &&typeof module._th128_world_stage_intro_state==='function'
    &&typeof module._th128_world_stage_intro_draw==='function'
    &&typeof module._th128_world_stage_intro_draw_error==='function';
}
async function installStageIntro(stage,module,renderer,jsonAt,bytesAt,memory,base,signal){
  const record=stageLogoRecord(stage);
  const [metadata,bytes]=await Promise.all([jsonAt(record.metadata),bytesAt(record.bytes)]);
  const provenance=metadata?.provenance;
  if(!provenance||provenance.name!==record.logo||provenance.bytes!==bytes.length
    ||typeof provenance.sha256!=='string'||!/^[0-9a-f]{64}$/.test(provenance.sha256)
    ||!Array.isArray(metadata.entries)||metadata.entries.length===0)
    throw new Error('Stage logo metadata/provenance does not match the selected stage');
  const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))]
    .map(value=>value.toString(16).padStart(2,'0')).join('');
  if(digest!==provenance.sha256)throw new Error('Stage logo checksum mismatch');
  await renderer.loadBank(30,metadata,base,{signal});
  const encoded=new TextEncoder().encode(record.logo),logoPointer=memory(encoded),dataPointer=memory(bytes);
  const loaded=module._th128_world_load_stage_intro(record.id,logoPointer,encoded.length,dataPointer,bytes.length);
  module._free(logoPointer);module._free(dataPointer);
  if(!loaded)throw new Error(module.UTF8ToString(module._th128_world_error())||'Stage logo load rejected');
  return record;
}
function stageIntroCaption(trackName,role){
  if(!state.stageIntroLoaded||typeof state.module?._th128_world_track_caption!=='function')return false;
  const trackFile=stageIntroTrackNumber(trackName);
  if(trackFile<0)throw new Error('Stage music track has no original music atlas row');
  const key=`${trackFile}:${role}`;
  if(state.stageIntroCaptionKey===key)return true;
  if(!state.module._th128_world_track_caption(trackFile,role))
    throw new Error(state.module.UTF8ToString(state.module._th128_world_error())||'Stage track caption rejected');
  state.stageIntroCaptionKey=key;
  return true;
}
function stageIntroDrawFailure(module){
  if(!module||typeof module._th128_world_stage_intro_draw_error!=='function')return '';
  return module.UTF8ToString(module._th128_world_stage_intro_draw_error());
}
function gameplayXHeld(){
  return supportsPerfectFreezeX()&&state.worldMode&&state.shotMode&&state.active&&!state.paused
    &&!menu.fault&&!menu.pauseOpen&&!state.handoff&&state.pressed.has('KeyX');
}
function gameplayXPress(){
  if(!gameplayXHeld())return false;
  worldMovementInput(perfectFreezeXBit,0);return true;
}
function gameplayXRelease(){
  if(!supportsPerfectFreezeX()||!state.worldMode||!state.shotMode||!state.active||state.paused
    ||menu.fault||menu.pauseOpen||state.handoff)return false;
  worldMovementInput(0,perfectFreezeXBit);return true;
}
function gameplayInputMask(){
  return supportsPerfectFreezeX()?761|perfectFreezeXBit:761;
}
function syncOriginalFPSPresentation(rate,draws){
  find('#game').dataset.originalFpsPresentation=JSON.stringify({rate,text:originalFPSText(rate),glyphCount:draws.length,measured:rate!==null});
}
function draw(interpolation=1){
  if(!state.module||!state.renderer)return false;
  if(state.normalPostrun){state.normalPostrun.redraw();syncOriginalFPSPresentation(null,[]);return false;}
  if(state.endingHost){state.endingHost.redraw();syncOriginalFPSPresentation(null,[]);return false;}
  if(state.worldMode){window.th128Debug.redrawEmission({interpolation});return !!state.shotMode;}
  const count=state.module._th128_draw_count();
  if(coreError())throw new Error(coreError());
  const records=decodeDrawBuffer(state.module.HEAPU8.buffer,state.module._th128_draw_ptr(),count,state.module._th128_draw_stride());
  state.renderer.render(records,{nativeMenu:true});
  if(records.length>0)state.presentationSerial=(state.presentationSerial??0)+1;
  syncOriginalFPSPresentation(null,[]);
  const snapshot=nativeMenuSnapshot();
  find('#game').dataset.nativeMenuPresentation=JSON.stringify({
    frame:state.module._th128_frame(),
    page:snapshot.page,selection:snapshot.selection,
    ready:snapshot.ready,transitioning:snapshot.transitioning,
    ...(snapshot.page===7?{resultRoute:snapshot.resultRoute,resultRank:snapshot.resultRank,resultPage:snapshot.resultPage,resultPageCount:snapshot.resultPageCount}:{} )
  });
  return false;
}

function stopAudio({keepContext=false,keepPracticeMusic=null}={}){
  const kept=keepPracticeMusic&&keepPracticeMusic===state.stageMusic&&!keepPracticeMusic.disposed?keepPracticeMusic:null;
  state.extraRun?.close();state.extraRun=null;state.extraPresentation=null;
  closeOrdinaryWorldAudio();
  disposeOriginalMain();
  state.thpracRecorder?.close();state.thpracRecorder=null;state.thpracReplayRows=null;state.thpracTools?.close();state.thpracTools=null;thpracLogicClock.reset();thpracToolsPanel?.close();
  state.replayStore?.close();state.replayStore=null;state.replaySlots=null;state.replayNativeBound=false;state.replayClockLast=null;
  const retiredPause=state.nativePause;state.nativePause=null;state.nativePausePending=null;
  retiredPause?.dispose().catch(problem=>status(String(problem.message??problem)));state.menuSoundLedger?.close();state.menuSoundLedger=null;
  state.messageMusicSerial=0;state.musicControlSerial=0;state.worldSnapshot=null;const overlay=document.querySelector('#dialogue-text');if(overlay)overlay.hidden=true;
  ++menu.musicRequestId;stopMenuMusic();menuMusic.cache.clear();if(!kept)state.stageMusic?.dispose();state.stageMusic=null;
  state.gameSounds?.dispose();state.gameSounds=null;state.soundCursor=0;state.soundWindow=null;
  disposeOriginalStageTextRasters(state.stageTextRasters);state.stageTextRasters=null;
  if(state.source){state.source.stop();state.source.disconnect();state.source=null;}
  menu.audioUnlocked=false;menu.audioUnlockPromise=null;
  if(!keepContext&&!kept&&state.audio){state.audio.close().catch(()=>{});state.audio=null;state.gain=null;}
}
function stopTitleSource(){
  if(!state.source)return;
  if(state.ordinaryMainMusic?.source===state.source){state.ordinaryMainMusic.stopSource();state.source=null;return;}
  try{state.source.stop();}catch{}
  try{state.source.disconnect();}catch{}
  state.source=null;
}
function startTitleSource(){
  if(!state.audio||state.audio.state==='closed'||!state.gain||!state.buffer||!state.track)return;
  stopTitleSource();
  if(state.ordinaryMainMusic&&!state.ordinaryMainMusic.disposed){
    const music=state.ordinaryMainMusic;music.track=state.track;music.buffer=state.buffer;music.start();state.source=music.source;recordOwnedHeardTrack(state.track.name).catch(error);return;
  }
  const source=state.audio.createBufferSource();source.buffer=state.buffer;source.loop=true;
  source.loopStart=state.track.loopStart;source.loopEnd=state.track.loopEnd;
  source.connect(state.titleAudio?.bgmGain??musicOutput(state.audio,state.gain));source.start();state.source=source;
  recordOwnedHeardTrack(state.track.name).catch(error);
}
function stopMenuMusic(){
  if(menuMusic.source){try{menuMusic.source.stop();}catch{};try{menuMusic.source.disconnect();}catch{};menuMusic.source=null;}
  menuMusic.track=null;
}
async function unlockMenuAudio(){
  if(!state.active||state.worldMode||menu.launching||!state.buffer||!state.track)return false;
  if(menu.audioUnlocked&&state.audio?.state==='running')return true;
  if(menu.audioUnlockPromise){
    if(state.audio?.state==='suspended')state.audio.resume().catch(problem=>status('音频解锁失败：'+(problem.message??String(problem))));
    return menu.audioUnlockPromise;
  }
  const ticket=state.mainAudioTicket??0;
  menu.audioUnlockPromise=(async()=>{
    try{
      const ready=await startAudio();
      if(ticket!==(state.mainAudioTicket??0)||state.worldMode||!state.active)return false;
      menu.audioUnlocked=ready===true&&state.audio?.state==='running';
      return menu.audioUnlocked;
    }catch(problem){
      if(ticket===(state.mainAudioTicket??0))status('音频解锁失败：'+(problem.message??String(problem)));
      return false;
    }finally{if(ticket===(state.mainAudioTicket??0))menu.audioUnlockPromise=null;}
  })();
  return menu.audioUnlockPromise;
}
function discardPendingCarry(){
  const token=state.pendingCarryToken;
  if(!token)return;
  if(typeof state.module?._th128_world_discard_stage_carry!=='function')
    throw new Error('stage carry discard runtime is unavailable; the unconsumed token was retained');
  if(!state.module._th128_world_discard_stage_carry(token))
    throw new Error(state.module.UTF8ToString(state.module._th128_world_error())||'stage carry discard failed; the token was retained');
  state.pendingCarryToken=0;
}
function ordinaryWorldAudioState(){
  if(!state.worldMode||!state.module||state.replayPlayback)return null;
  // NativeOrdinaryTerminalOwner can only begin at MSG entry 1. Before that,
  // the small live probe avoids serializing every enemy/bullet for audio polls.
  if(!state.ordinaryWorldAudio&&!state.ordinaryWorldAudioLoading&&typeof state.module._th128_world_message_state==='function'){
    const message=JSON.parse(state.module.UTF8ToString(state.module._th128_world_message_state()));
    if(!message.configured||message.entry!==1)return null;
  }
  const snapshot=JSON.parse(state.module.UTF8ToString(state.module._th128_world_state()));
  if(!snapshot.normalTerminal?.configured||!snapshot.normalTerminal.begun)return null;
  if(typeof state.endingResources?.readAudioIdentity!=='function'||typeof state.endingResources?.createAudio!=='function')
    throw new Error('Ordinary terminal strict audio consumer is unavailable');
  return state.endingResources.readAudioIdentity(snapshot);
}
function closeOrdinaryWorldAudio(){
  state.ordinaryWorldAudioTicket=(state.ordinaryWorldAudioTicket??0)+1;
  state.ordinaryWorldAudio?.owner.close();state.ordinaryWorldAudio=null;state.ordinaryWorldAudioLoading=null;
  state.worldAdvancePromise=null;
}
async function ordinaryWorldAudioLane(){
  if(!ordinaryWorldAudioState())return null;
  if(state.ordinaryWorldAudio){state.ordinaryWorldAudio.owner.current();return state.ordinaryWorldAudio.owner;}
  if(state.ordinaryWorldAudioLoading)return state.ordinaryWorldAudioLoading;
  const module=state.module,action=state.action,selection=state.stageSelection,audio=state.audio,soundQueue=state.gameSounds,legacyMusic=state.stageMusic;
  const ticket=state.ordinaryWorldAudioTicket??0,prepared=state.endingResources;
  const current=()=>ticket===(state.ordinaryWorldAudioTicket??0)&&module===state.module&&action===state.action
    &&selection===state.stageSelection&&state.worldMode&&audio===state.audio&&soundQueue===state.gameSounds&&legacyMusic===state.stageMusic;
  const loading=(async()=>{
    const owner=await prepared.createAudio({module,audio,soundQueue,legacyMusic,isCurrent:current,presentAudio:!window.__TH128_FAST_REPLAY__});
    if(!current()){owner.close();throw new DOMException('Ordinary audio load owner changed','AbortError');}
    state.ordinaryWorldAudio={owner,module,action,selection};return owner;
  })().catch(problem=>{if(!current())throw new DOMException('Ordinary audio initialization owner was cancelled','AbortError');throw problem;});
  state.ordinaryWorldAudioLoading=loading;
  try{return await loading;}finally{if(state.ordinaryWorldAudioLoading===loading)state.ordinaryWorldAudioLoading=null;}
}
async function drainGameSounds(){
  const owner=await ordinaryWorldAudioLane();
  if(owner){await owner.sound();return;}
  return drainGameSoundsLegacy();
}
async function drainMusicControl(){
  const owner=await ordinaryWorldAudioLane();
  if(owner){const receipt=await owner.music();if(receipt)state.musicControlSerial=receipt.serial;return;}
  return drainMusicControlLegacy();
}

function drainGameSoundsLegacy(){
  if(!state.gameSounds||!state.module)return;
  const module=state.module;
  if(typeof module._th128_world_sound_epoch!=='function'||typeof module._th128_world_sound_begin_window!=='function'
      ||typeof module._th128_world_sound_ack_window!=='function'||typeof module._th128_world_sound_window_state!=='function')
    throw new Error('Native sound journal consumption adapter is unavailable');
  const epoch=module._th128_world_sound_epoch();
  if(!Number.isInteger(epoch)||epoch===0)throw new Error('Native sound journal epoch is unavailable or exhausted');
  const nativeWindow=JSON.parse(module.UTF8ToString(module._th128_world_sound_window_state()));
  if(!nativeWindow||!Number.isInteger(nativeWindow.serial)||!Number.isInteger(nativeWindow.epoch)
      ||!Number.isInteger(nativeWindow.baseLow)||!Number.isInteger(nativeWindow.baseHigh)||!Number.isInteger(nativeWindow.count))
    throw new Error('Native sound journal window state is unavailable');
  let readWindow=state.soundWindow??null;
  if(readWindow){
    if(!Number.isInteger(readWindow.serial)||readWindow.serial===0||readWindow.serial!==nativeWindow.serial
        ||readWindow.epoch!==epoch||readWindow.epoch!==nativeWindow.epoch
        ||readWindow.baseLow!==nativeWindow.baseLow||readWindow.baseHigh!==nativeWindow.baseHigh)
      throw new Error('Native sound journal read window identity changed; refusing to resume an unowned window');
    if(!Number.isInteger(readWindow.cursor)||readWindow.cursor<0||readWindow.cursor>nativeWindow.count)
      throw new Error('Native sound journal read cursor is outside the owned window');
  }else{
    if(nativeWindow.serial!==0)throw new Error('Native sound journal has an open read window this consumer does not own');
    const serial=module._th128_world_sound_begin_window();
    if(serial===0)return;
    if(!Number.isInteger(serial)||serial<0)throw new Error('Native sound journal read window could not be opened');
    const opened=JSON.parse(module.UTF8ToString(module._th128_world_sound_window_state()));
    if(!opened||opened.serial!==serial||!Number.isInteger(opened.count)||opened.count<0)
      throw new Error('Native sound journal read window could not be opened');
    readWindow={serial,epoch,baseLow:opened.baseLow,baseHigh:opened.baseHigh,count:opened.count,cursor:0};
    state.soundWindow=readWindow;
  }
  let presentedThisDrain=0;
  const present=!window.__TH128_FAST_REPLAY__;
  if(typeof state.gameSounds?.consumeNativeRequest!=='function')throw new Error('Original sound identity consumer API is unavailable');
  while(readWindow.cursor<readWindow.count){
    const identifier=module._th128_world_sound_id(readWindow.cursor);
    const position=module._th128_world_sound_x(readWindow.cursor);
    const operation=module._th128_world_sound_operation(readWindow.cursor);
    if(!Number.isInteger(identifier)||!Number.isFinite(position)||!Number.isInteger(operation))
      throw new Error('Native sound journal entry is unreadable');
    if(operation!==0&&operation!==1&&operation!==2)throw new Error(`Unimplemented original sound operation ${operation}`);
    if(identifier<0&&!(identifier===-1&&operation===0&&state.gameSounds?.acceptsSilentIdentity?.()===true))throw new Error(`Invalid original sound identifier ${identifier} for operation ${operation}`);
    state.gameSounds.consumeNativeRequest(identifier,position,operation,{present});
    readWindow.cursor+=1;
    presentedThisDrain+=1;
  }
  if(presentedThisDrain>0)state.gameSounds.flush({present});
  const remaining=module._th128_world_sound_ack_window(readWindow.serial,readWindow.count,readWindow.baseLow,readWindow.baseHigh,readWindow.epoch);
  if(!Number.isInteger(remaining)||remaining<0){
    const detail=typeof module._th128_world_error==='function'?module.UTF8ToString(module._th128_world_error()):'';
    throw new Error(`Native sound journal acknowledgement failed${detail?`: ${detail}`:''}`);
  }
  state.soundWindow=null;
}
function drainMusicControlLegacy(){
  if(!state.worldMode||!state.module||typeof state.module._th128_world_acknowledge_music_control!=='function')return;
  let control;
  if(typeof state.module._th128_world_music_control_state==='function'){
    const text=state.module.UTF8ToString(state.module._th128_world_music_control_state());
    control=text?JSON.parse(text):null;
  }else control=state.worldSnapshot?.musicControl;
  if(!control||control.serial===0||control.acknowledged||control.serial===state.musicControlSerial)return;
  if(control.operation!==5)throw new Error(`Unimplemented original music operation ${control.operation}`);
  if(!state.stageMusic)throw new Error('BGM is unavailable; music control was not acknowledged');
  if(typeof state.stageMusic.fadeOutFrames!=='function'||typeof state.stageMusic.fadeOut!=='function')
    throw new Error('BGM fade method is unavailable; music control was not acknowledged');
  if(state.audio?.state!=='running')throw new Error('BGM audio context is not running; music control was not acknowledged');
  if(control.immediate)state.stageMusic.fadeOut(0);else state.stageMusic.fadeOutFrames(control.durationFrames);
  if(!state.module._th128_world_acknowledge_music_control(control.serial))
    throw new Error(state.module.UTF8ToString(state.module._th128_world_error())||'Music control acknowledgement was rejected');
  state.musicControlSerial=control.serial;
}
function syncGameplayMusicState(message){
  const messageTrack=state.stageSelection?.stage.music?.[1];
  if(!message||!messageTrack||!state.stageMusic||message.musicSerial===state.messageMusicSerial)return;
  state.stageMusic.switchTo(messageTrack);recordOwnedHeardTrack(messageTrack).catch(error);state.messageMusicSerial=message.musicSerial;
  if(state.stageIntroLoaded)stageIntroCaption(messageTrack,1);
}
function assertSynchronousWorldDrainAllowed(){
  if(state.worldAdvancePromise||state.ordinaryWorldAudio||state.ordinaryWorldAudioLoading)
    throw new Error('Ordinary audio/World step must use the serial asynchronous consumer');
  if(state.worldMode&&!state.replayPlayback){
    const value=JSON.parse(state.module.UTF8ToString(state.module._th128_world_state()));
    if(value.normalTerminal?.configured&&value.normalTerminal.ownerBound)
      throw new Error('Bound ordinary terminal cannot use synchronous diagnostic ticks');
  }
}
function drainWorld(){assertSynchronousWorldDrainAllowed();drainGameSoundsLegacy();drainMusicControlLegacy();}
async function prepareGameSounds(base,signal,tracks,selection=null){
  const retained=selection?.thpracRetainFrom&&state.audio?.state!=='closed'&&state.stageMusic?.retainPracticeOwner(selection.thpracRetainFrom,selection,tracks[0]);
  const audio=retained?state.audio:new AudioContext(),gain=retained?state.gain:audio.createGain();
  if(!retained){gain.gain.value=Number(find('#volume').value)/100;gain.connect(audio.destination);}
  const queue=new GameSoundQueue(audio,gain),music=retained?state.stageMusic:new StageMusic(audio,gain);
  try{
    await audio.resume();const bindings=await readJson('/assets/runtime/sound-bindings.json',signal);
    await Promise.all([queue.load(bindings,base,safeAssetUrl,{signal}),retained?Promise.resolve():music.load(state.manifest.music,base,safeAssetUrl,{signal,tracks})]);
    return {audio,gain,queue,music,retained};
  }catch(problem){queue.dispose();music.dispose();await audio.close().catch(()=>{});throw problem;}
}

async function startAudio(){
  if(!state.buffer||!state.track)return false;
  if(!state.audio||state.audio.state==='closed'){
    state.audio=new AudioContext({sampleRate:state.track.sampleRate});
    state.gain=state.audio.createGain();state.gain.gain.value=Number(find('#volume').value)/100;
    state.gain.connect(state.audio.destination);
  }
  const audio=state.audio;
  await audio.resume();
  if(state.audio!==audio||audio.state!=='running')return false;
  if(!menu.suppressTitleAudio&&menu.page!=='music'&&!state.mainQuit){
    if(!state.source)startTitleSource();
    else recordOwnedHeardTrack(state.track.name).catch(error);
  }
  return true;
}
function currentVariantKey(){return find('#variant').value;}
function optionItems(){
  const variantKeys=Object.keys(state.catalog?.variants??{});
  if(!menu.optionsInitialized){
    menu.versionIndex=Math.max(0,variantKeys.indexOf(currentVariantKey()));
    menu.optionsInitialized=true;
  }
  const versionKey=variantKeys[menu.versionIndex]??currentVariantKey();
  return [
    {id:'volume',label:`VOLUME ${menu.volume}`,note:'←→'},
    {id:'fullscreen',label:`FULLSCREEN ${document.fullscreenElement?'ON':'OFF'}`,note:'Z/ENTER'},
    {id:'version',label:`VERSION ${versionKey}${versionKey===currentVariantKey()?'':' (Z apply)'}`,note:'←→'},
    {id:'back',label:'BACK 返回主菜单',note:'Z/ENTER'},
  ];
}
function buildOptions(){
  const items=optionItems(),list=find('#menu-option-list');
  menu.option=Math.max(0,Math.min(items.length-1,menu.option));
  list.replaceChildren(...items.map((item,index)=>{
    const row=document.createElement('li');row.className='menu-row';row.setAttribute('role','option');
    row.dataset.menuOption=item.id;
    const number=document.createElement('span');number.className='menu-row-index';number.textContent=String(index).padStart(2,'0');
    const label=document.createElement('span');label.className='menu-row-label';label.textContent=item.label;
    const mark=document.createElement('span');mark.className='menu-row-mark';mark.textContent=item.note;
    row.append(number,label,mark);return row;
  }));
  updateMenuSelection();
}
function setMenuVolume(value){
  menu.volume=Math.max(0,Math.min(100,Math.round(value)));
  find('#volume').value=String(menu.volume);
  if(state.gain)state.gain.gain.value=menu.volume/100;
  savePreference('volume',menu.volume);
  if(menu.page==='options')buildOptions();
}
function adjustOption(delta){
  const item=optionItems()[menu.option];if(!item)return;
  if(item.id==='volume')setMenuVolume(menu.volume+delta*5);
  else if(item.id==='version'){
    const keys=Object.keys(state.catalog?.variants??{});
    if(keys.length)menu.versionIndex=(menu.versionIndex+delta+keys.length)%keys.length;
    buildOptions();
  }
}
function applyVariant(){
  const keys=Object.keys(state.catalog?.variants??{});
  const key=keys[menu.versionIndex];if(!key||key===currentVariantKey())return;
  savePreference('variant',key);
  find('#variant').value=key;
  find('#variant').dispatchEvent(new Event('change'));
}
function toggleFullscreen(){
  const operation=document.fullscreenElement?document.exitFullscreen():find('.screen').requestFullscreen();
  operation.catch(error).finally(()=>{if(menu.page==='options')buildOptions();});
}
function activateOption(){
  const item=optionItems()[menu.option];if(!item)return;
  if(item.id==='volume')setMenuVolume(menu.volume+5);
  else if(item.id==='fullscreen')toggleFullscreen();
  else if(item.id==='version')applyVariant();
  else showMenuPage('main');
}
function musicTracks(){return (state.manifest?.music??[]).filter(track=>track&&typeof track.file==='string');}
function populateMusicRows(){
  const tracks=musicTracks(),list=find('#menu-music-list');
  menu.music=tracks.length?Math.max(0,Math.min(tracks.length-1,menu.music)):0;
  list.replaceChildren(...tracks.map((track,index)=>{
    const row=document.createElement('li');row.className='menu-row';row.setAttribute('role','option');
    row.dataset.menuMusic=String(index);
    const number=document.createElement('span');number.className='menu-row-index';number.textContent=String(index+1).padStart(2,'0');
    const label=document.createElement('span');label.className='menu-row-label';label.textContent=track.name??track.file;
    const mark=document.createElement('span');mark.className='menu-row-mark';mark.textContent=menuMusic.track?.file===track.file?'PLAYING':'Z PLAY';
    row.append(number,label,mark);return row;
  }));
}
async function playMenuTrack(track){
  const request=++menu.musicRequestId;
  try{
    const unlocked=await unlockMenuAudio();
    if(request!==menu.musicRequestId)return;
    if(!unlocked||!state.audio)throw new Error('AudioContext 未能解锁');
    let buffer=menuMusic.cache.get(track.file);
    if(!buffer){
      const response=await checkpointFetch(safeAssetUrl(state.base,track.file),{signal:state.controller.signal});
      if(!response.ok)throw new Error(`曲目读取失败 ${response.status}: ${track.file}`);
      buffer=await state.audio.decodeAudioData(await response.arrayBuffer());
      menuMusic.cache.set(track.file,buffer);
    }
    if(request!==menu.musicRequestId)return;
    stopTitleSource();stopMenuMusic();
    const source=state.audio.createBufferSource();source.buffer=buffer;source.loop=true;
    source.connect(musicOutput(state.audio,state.gain));source.start();
    menuMusic.source=source;menuMusic.track=track;
    await state.audio.resume();
    menu.infoDescription=musicOutputEnabled()?`实际 manifest 曲目 · 正在播放 ${track.name??track.file} · Z 停止 · X 返回`:'本次启动已关闭 BGM · 音效保持开启 · X 返回';
    updateMenuCopy();populateMusicRows();updateMenuSelection();
  }catch(problem){
    if(request===menu.musicRequestId){
      menu.infoDescription=`音乐播放失败：${problem.message??String(problem)}；没有伪造播放。`;
      updateMenuCopy();
    }
  }
}
function showMusicPage(){
  menu.infoMode='music';menu.infoTitle='MUSIC';menu.infoDescription='实际 manifest 曲目列表；Z 播放/停止，X 返回并释放菜单 BGM。';
  menu.suppressTitleAudio=true;++menu.musicRequestId;stopMenuMusic();stopTitleSource();
  find('#menu-info-title').textContent='MUSIC';
  find('#menu-info-body').textContent=menu.infoDescription;
  find('#menu-music-list').hidden=false;
  populateMusicRows();
  showMenuPage('info');
}
function showManual(){
  find('#menu-manual-body').textContent='方向键选择，Z/ENTER 确认，X/ESC 返回。游戏中方向键移动，Z 射击（按住蓄力，松开冻结），C 连射，Shift 低速，Esc 打开暂停菜单。\n\nX 的“完美冻结”尚未接通，不会伪造。REPLAY/RESULT 尚未实现；EX 使用页面顶部调试入口，仍有未完成分支，不计正常通关。\n\n返回标题会释放音频和当前世界输入；网页版本不会关闭浏览器。';
  showMenuPage('manual');
}


async function readJson(url,signal){
  const controller=new AbortController(),abort=()=>controller.abort();
  if(signal?.aborted)controller.abort();else signal?.addEventListener('abort',abort,{once:true});
  try{return await loadingDeadline((async()=>{
    const response=await checkpointFetch(url,{signal:controller.signal});
    if(!response.ok)throw new Error(`资源读取失败 ${response.status}: ${url}`);
    return await response.json();
  })(),url,abort);}finally{signal?.removeEventListener('abort',abort);}
}

const checkpointModuleGenerations=new WeakMap(),checkpointModuleBuilds=new WeakMap();
const startedWorldModules=new WeakSet();
async function runtimeFactory(signal){
  const build=await readJson('/runtime/build.json',signal);
  if(build.game!=='th128'||!/^runtime\/builds\/[a-f0-9]{64}\/core\.mjs$/.test(build.entry))throw new Error('无效的运行时构建清单');
  if(build.entry!==`runtime/builds/${build.generation}/core.mjs`)throw new Error('Checkpoint runtime generation is inconsistent');
  const factory=(await import(new URL(build.entry,location.href).href)).default;
  return async options=>{
    const module=await factory(options);
    checkpointModuleGenerations.set(module,build.generation);checkpointModuleBuilds.set(module,build);
    return module;
  };
}

async function prepareFreshWorldModule(selection,bytesAt,signal){
  const previous=state.module;
  const aborted=()=>{if(signal?.aborted)throw signal.reason??new DOMException("Aborted","AbortError");};
  aborted();
  if(selection?.carryToken)return previous;
  if(!startedWorldModules.has(previous)){startedWorldModules.add(previous);return previous;}
  const createRuntime=await runtimeFactory(signal);
  const module=await createRuntime({print:()=>{},printErr:message=>console.warn(message)});
  aborted();
  const generation=checkpointModuleGenerations.get(previous);
  if(generation&&checkpointModuleGenerations.get(module)!==generation)
    throw new Error("运行时构建已更新，请重新载入页面后再开始新会话");
  for(const [bank,name]of [[24,"title"],[25,"title_v"],[11,"ascii"]]){
    const bytes=await bytesAt("runtime/"+name+".anm");aborted();
    const pointer=module._malloc(bytes.length);
    try{module.HEAPU8.set(bytes,pointer);if(!module._th128_load_anm(bank,pointer,bytes.length))
      throw new Error(module.UTF8ToString(module._th128_error())||"新会话的原版标题资源加载失败");
    }finally{module._free(pointer);}
  }
  startedWorldModules.add(module);return module;
}

async function loadVariant(variant){
  state.replayPlayback?.cancel();state.replayPlayback=null;state.replayReturnSlot=null;
  cancelOriginalEnding();
  try{discardPendingCarry();}catch(problem){error(problem);return;}
  const ticket=++state.loading;
  ++menu.launchId;menu.launching=false;menu.audioUnlocked=false;menu.audioUnlockPromise=null;
  menu.suppressTitleAudio=false;menu.optionsInitialized=false;menu.infoMode='text';menu.page='main';
  ++menu.musicRequestId;stopMenuMusic();
  if(state.pressed.has('KeyZ'))suppressedKeys.add('KeyZ');
  if(state.pressed.has('Enter'))suppressedKeys.add('Enter');
  find('#game-menu').hidden=true;
  state.autoFlow=false;state.handoff=false;state.handoffFailed=false;state.worldSnapshot=null;++state.handoffGeneration;
  state.active=false;state.paused=true;state.pressed.clear();clock.reset();resetHighFrameratePresentation();
  state.controller?.abort();state.controller=new AbortController();
  const {signal}=state.controller;
  state.renderer?.dispose();state.renderer=null;stopAudio();state.buffer=null;state.track=null;
  find('#loading').hidden=false;find('#error').hidden=true;
  state.worldMode=false;state.shotMode=false;
  syncGameplayFrameUI();
  syncDebugRespawnUI(null);
  find('#runtime-overlay').hidden=true;
  state.background=null;
  for(const name of ['start','pause','reset','ecl','world','emission','segment','shots','play-stage','emit-shot','debug-stage-select','debug-stage-start'])find('#'+name).disabled=true;
  const deadline=setTimeout(()=>{
    if(ticket!==state.loading)return;
    ++state.loading;state.controller.abort();
    error(new Error(`原版资源加载超时：${find('#loading-text').textContent}`));
  },15000);
  try{
    const record=state.catalog.variants[variant];
    const manifestUrl=safeAssetUrl(new URL('/assets/',location.href).href,record.manifest);
    const manifest=await readJson(manifestUrl,signal);
    if(manifest.game!=='th128'||manifest.schema!=='th128-prepared-assets/1')throw new Error('不是 TH128 原版资源');
    const base=new URL('./',manifestUrl).href;
    const stageCatalog=validateStageCatalog(await readJson(safeAssetUrl(base,'animations/stage-catalog.json'),signal));
    if(ticket!==state.loading)return;
    const firstStage=resolveStage(stageCatalog,1);
    state.stageCatalog=stageCatalog;
    state.stageSelection={stage:firstStage,difficulty:menu.difficulty,seed:34772,progress:0};
    const createRuntime=await runtimeFactory(signal);
    const module=await createRuntime({print:()=>{},printErr:message=>console.warn(message)});
    if(ticket!==state.loading||signal.aborted||state.launcherExited)return;
    const renderer=new WebGlRenderer(find('#game'));
    renderer.setResolution(1280,960);
    if(ticket!==state.loading){renderer.dispose();return;}
    state.renderer=renderer;
    for(const [bank,name] of [[24,'title'],[25,'title_v'],[11,'ascii']]){
      find('#loading-text').textContent=`读取原版动画：${name}.anm`;
      const metadata=await readJson(safeAssetUrl(base,`animations/${name}.json`),signal);
      const response=await checkpointFetch(safeAssetUrl(base,`runtime/${name}.anm`),{signal});
      if(!response.ok)throw new Error(`ANM 读取失败：${name}`);
      const bytes=new Uint8Array(await response.arrayBuffer());
      const pointer=module._malloc(bytes.length);module.HEAPU8.set(bytes,pointer);
      const loaded=module._th128_load_anm(bank,pointer,bytes.length);module._free(pointer);
      if(!loaded)throw new Error(module.UTF8ToString(module._th128_error()));
      await renderer.loadBank(bank,metadata,base,{signal});
    }
    find('#loading-text').textContent='解码原版标题音乐…';
    const track=manifest.music.find(track=>track.name==='th128_07.wav');
    if(track){
      const response=await checkpointFetch(safeAssetUrl(base,track.file),{signal});if(!response.ok)throw new Error('标题音乐读取失败');
      const audio=new OfflineAudioContext(track.channels,1,track.sampleRate);
      const buffer=await audio.decodeAudioData(await response.arrayBuffer());
      if(ticket!==state.loading)return;
      state.buffer=buffer;state.track=track;
    }
    if(ticket!==state.loading)return;
    state.module=module;state.manifest=manifest;state.base=base;
    syncDebugRespawnControl();
    if(!module._th128_boot_title(0x1234))throw new Error(coreError());
    draw();
    find('#loading-text').textContent='资源就绪，正在进入原版标题与键盘菜单…';
    find('#start').disabled=false;find('#reset').disabled=false;find('#ecl').disabled=false;
    find('#world').disabled=!module._th128_world_load;
    find('#emission').disabled=!module._th128_world_enable_bullets;
    find('#segment').disabled=!module._th128_world_enable_bullet_updates;
    find('#shots').disabled=!module._th128_world_enable_player_shots;
    find('#play-stage').disabled=!module._th128_world_enable_z_fire;
    enterTitleMenu();
    syncDebugStageJumpControls();
  }catch(problem){if(ticket===state.loading&&problem.name!=='AbortError')error(problem);}
  finally{clearTimeout(deadline);}
}

function cancelOriginalEnding(){
  ++state.endingTicket;
  const normalOwner=state.normalPostrun;state.normalPostrun=null;
  state.normalPostrunPausePromise=state.normalPostrunResumePromise=null;
  state.normalPostrunPauseBinding=state.normalPostrunResumeBinding=null;normalOwner?.close();
  const host=state.endingHost;
  state.endingHost=null;state.endingResources=null;state.endingLoading=false;state.endingBusy=false;
  state.endingPromise=null;state.endingStepPromise=null;
  if(host)host.close();
}
async function prepareOriginalEndingResources(stage,base,signal){
  if(!terminalStageIds.has(stage.id))return null;
  const expected=state.manifest?.nativeEndingAssets;
  if(!expected||expected.variant!==find('#variant').value||expected.schema!=='th128-original-recorded-native-ending-resources/1'
      ||!Number.isSafeInteger(expected.bytes)||expected.bytes<1||expected.bytes>1024*1024||!/^[0-9a-f]{64}$/.test(expected.sha256))
    throw new Error('Original ending resource manifest pin is unavailable; no fake ending fallback');
  const response=await checkpointFetch(safeAssetUrl(base,expected.file),{signal});
  if(!response.ok)throw new Error('Original ending resource manifest fetch failed');
  const bytes=new Uint8Array(await response.arrayBuffer());
  const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(byte=>byte.toString(16).padStart(2,'0')).join('');
  if(bytes.length!==expected.bytes||digest!==expected.sha256)throw new Error('Original ending resource manifest full-byte pin rejected');
  const pack=JSON.parse(new TextDecoder('utf8',{fatal:true}).decode(bytes));
  const resources=pack.routes?.find(row=>row.stage===stage.id);
  if(pack.schema!==expected.schema||pack.variant!==expected.variant||pack.sourceExecutableSHA256!==type25SourceExeSha256
      ||pack.routeCount!==6||pack.textContexts!==118||!resources||!Array.isArray(resources.glyphs)
      ||resources.index!==[3,5,8,10,13,15].indexOf(stage.id))
    throw new Error('Original ending route resource identity rejected');
  const module=await import('./runtime/native-ending-browser-host-v4.mjs');
  if(typeof module.createEndingBrowserHost!=='function'||typeof module.originalEndingRequest!=='function'
      ||typeof module.ordinaryWorldAudioIdentity!=='function'||typeof module.createOrdinaryWorldAudioConsumer!=='function')throw new Error('Original ending browser consumer module is unavailable');
  return{resources,createHost:module.createEndingBrowserHost,readRequest:module.originalEndingRequest,
    readAudioIdentity:module.ordinaryWorldAudioIdentity,createAudio:module.createOrdinaryWorldAudioConsumer,base,signal};
}
function pendingOriginalEndingRequest(){
  if(!state.endingResources||state.endingHost||state.endingLoading||!state.worldMode||state.replayPlayback)return null;
  const message=JSON.parse(state.module.UTF8ToString(state.module._th128_world_message_state()));
  if(message.active||message.entry!==1||message.clock<300)return null;
  return state.endingResources.readRequest(JSON.parse(state.module.UTF8ToString(state.module._th128_world_state())));
}
function maybeStartOriginalEnding(){
  const request=pendingOriginalEndingRequest();if(!request)return false;
  const ticket=++state.endingTicket,module=state.module,action=state.action,loading=state.loading,prepared=state.endingResources;
  const current=()=>ticket===state.endingTicket&&module===state.module&&action===state.action&&loading===state.loading;
  state.endingLoading=true;state.autoFlow=false;clock.reset();resetHighFrameratePresentation();
  const overlay=find('#dialogue-text');if(overlay)overlay.hidden=true;
  state.endingPromise=(async()=>{
    let host=null;
    try{
      host=await prepared.createHost({module,renderer:state.renderer,resources:prepared.resources,
        readAsset:async record=>{
          const response=await checkpointFetch(safeAssetUrl(prepared.base,record.file),{signal:prepared.signal});
          if(!response.ok)throw new Error('Original ending asset unavailable: '+record.file);
          return new Uint8Array(await response.arrayBuffer());
        },audio:state.audio,gain:state.gain,soundQueue:state.gameSounds,legacyMusic:state.stageMusic,
        isCurrent:current,presentAudio:!window.__TH128_FAST_REPLAY__,
        prepareOrdinary:request.kind==='ordinary'?async()=>{
          if(!current())throw new Error('Ordinary ending profile owner changed before capture');
          const receipt=await recordOwnedMainSession();
          if(!current())throw new Error('Ordinary ending profile owner changed after persistence');
          return receipt;
        }:null});
      if(!current()){host.close();return;}
      state.endingHost=host;host.owner.input.adoptHeld(state.pressed);
      await host.owner.start();
      if(!current()){host.close();return;}
      state.endingLoading=false;
      status('原结局已按实际 runtime15 请求启动；Z/ENTER 使用原确认输入，不自动确认。');
    }catch(problem){
      if(host&&(!current()||host.owner.phase==='failed'))host.close();
      if(current()){state.endingLoading=false;error(problem);throw problem;}
    }finally{if(current())state.endingPromise=null;}
  })();
  return true;
}
async function finishExtraPostrun(snapshot){
  const module=state.module,action=state.action,selection=state.stageSelection,ticket=state.endingTicket;
  const current=()=>module===state.module&&action===state.action&&selection===state.stageSelection&&ticket===state.endingTicket;
  const native=JSON.parse(module.UTF8ToString(module._th128_world_normal_postrun_state()));
  if(!current()||snapshot?.generation!==native.generation||native.extraSealed!==true||native.provenance!=='sealed-extra-terminal23-return'||!native.present)
    throw new Error('EX return requires its actual same-owner terminal23 retirement');
  const {createNormalPostrunController}=await import('./runtime/native-normal-postrun.mjs');if(!current())return;
  const assets=[[0,'front'],[24,'title'],[25,'title_v'],[11,'ascii']].map(([bank,name])=>{const row=state.manifest.files?.find(r=>r.name===name+'.anm');if(!row)throw new Error('EX original menu asset absent');return{...row,bank,file:'runtime/'+name+'.anm',metadata:'animations/'+name+'.json'};});
  let controller;try{
    controller=await createNormalPostrunController({module,renderer:state.renderer,generation:native.generation,assets,audio:state.audio,gain:state.gain,soundQueue:state.gameSounds,tracks:state.manifest.music,base:state.base,signal:state.controller.signal,resolveUrl:safeAssetUrl,isCurrent:current,retainMusic:state.stageMusic,
      readAsset:async asset=>{const response=await checkpointFetch(safeAssetUrl(state.base,asset.file),{signal:state.controller.signal});if(!response.ok)throw new Error('EX original menu resource unavailable');return new Uint8Array(await response.arrayBuffer());},
      loadBank:async(bank,asset)=>state.renderer.loadBank(bank,await readJson(safeAssetUrl(state.base,asset.metadata),state.controller.signal),state.base,{signal:state.controller.signal})});
    if(!current()){controller.close();return;}await controller.startExtra(native);if(!current()){controller.close();return;}
    controller.input.adoptHeld(state.pressed);if(state.paused)controller.input.release();state.normalPostrun=controller;
    state.normalPostrunPausePromise=state.normalPostrunResumePromise=null;state.normalPostrunPauseBinding=state.normalPostrunResumeBinding=null;
    state.autoFlow=false;clock.reset();resetHighFrameratePresentation();status('EX 按真实终局交接姓名、录像与主菜单；辅助开启。');
  }catch(e){controller?.close();if(current())throw e;}
}
async function finishOrdinaryEndingReturn(host,snapshot){
  const ticket=state.endingTicket,module=state.module,action=state.action,selection=state.stageSelection;
  const current=()=>ticket===state.endingTicket&&module===state.module&&action===state.action&&selection===state.stageSelection;
  const {createNormalPostrunController}=await import('./runtime/native-normal-postrun.mjs');if(!current())return;
  const assets=[[0,'front'],[24,'title'],[25,'title_v'],[11,'ascii']].map(([bank,name])=>{
    const record=state.manifest.files?.find(row=>row.name===name+'.anm');if(!record)throw new Error('Original normal postrun asset pin unavailable');
    return{...record,bank,file:'runtime/'+name+'.anm',metadata:'animations/'+name+'.json'};
  });
  let controller;
  try{
    controller=await createNormalPostrunController({module,renderer:state.renderer,generation:snapshot.generation,assets,
      audio:state.audio,gain:state.gain,soundQueue:state.gameSounds,tracks:state.manifest.music,base:state.base,signal:state.controller.signal,resolveUrl:safeAssetUrl,isCurrent:current,
      readAsset:async asset=>{const response=await checkpointFetch(safeAssetUrl(state.base,asset.file),{signal:state.controller.signal});if(!response.ok)throw new Error('Original postrun asset unavailable');return new Uint8Array(await response.arrayBuffer());},
      loadBank:async(bank,asset)=>state.renderer.loadBank(bank,await readJson(safeAssetUrl(state.base,asset.metadata),state.controller.signal),state.base,{signal:state.controller.signal})});
    if(!current()){controller.close();return;}
    await controller.start(snapshot);if(!current()){controller.close();return;}
    controller.input.adoptHeld(state.pressed);if(state.paused)controller.input.release();
    host.close();state.endingHost=null;state.normalPostrun=controller;state.endingLoading=false;
    state.normalPostrunPausePromise=state.normalPostrunResumePromise=null;
    state.normalPostrunPauseBinding=state.normalPostrunResumeBinding=null;
    closeOrdinaryWorldAudio();state.autoFlow=false;clock.reset();resetHighFrameratePresentation();
    status('原结局按原生封存凭据返回普通菜单，姓名与录像保存按原流程处理。');
  }catch(problem){controller?.close();throw problem;}
}
function pauseNormalPostrun(){
  const owner=state.normalPostrun,module=state.module,action=state.action,selection=state.stageSelection;
  if(!owner)return;state.paused=true;owner.input.release();clock.reset();resetHighFrameratePresentation();
  const current=()=>owner===state.normalPostrun&&module===state.module&&action===state.action&&selection===state.stageSelection;
  const pending=owner.pause();state.normalPostrunPausePromise=pending;state.normalPostrunPauseBinding={owner,module,action,selection};
  pending.catch(problem=>{if(current())error(problem);});return pending;
}
function resumeNormalPostrun(){
  const owner=state.normalPostrun,module=state.module,action=state.action,selection=state.stageSelection;
  if(!owner)return Promise.resolve();
  const matches=b=>b?.owner===owner&&b.module===module&&b.action===action&&b.selection===selection;
  if(state.normalPostrunResumePromise&&matches(state.normalPostrunResumeBinding))return state.normalPostrunResumePromise;
  const current=()=>owner===state.normalPostrun&&module===state.module&&action===state.action&&selection===state.stageSelection;
  const pause=matches(state.normalPostrunPauseBinding)?state.normalPostrunPausePromise:null;
  const pending=(async()=>{
    await pause;if(!current())return;
    if(await owner.resume()===false||!current())return;
    state.paused=false;clock.reset();resetHighFrameratePresentation();
  })();state.normalPostrunResumePromise=pending;state.normalPostrunResumeBinding={owner,module,action,selection};
  pending.catch(problem=>{if(current())error(problem);}).finally(()=>{
    if(state.normalPostrunResumePromise===pending){state.normalPostrunResumePromise=null;state.normalPostrunResumeBinding=null;}
  });return pending;
}
async function finishNormalPostrunMain(controller,snapshot){
  const module=state.module,action=state.action,selection=state.stageSelection,ticket=state.endingTicket;
  const current=()=>controller===state.normalPostrun&&module===state.module&&action===state.action&&selection===state.stageSelection&&ticket===state.endingTicket;
  if(!current()||snapshot?.menu?.mainHandoffReady!==true)throw new Error('Ordinary Main handoff has no exact native current4 owner');
  await state.normalPostrunPausePromise;await state.normalPostrunResumePromise;if(!current())return;
  const committed=await controller.finishMain();if(!current())return;
  if(committed?.nativeCommitted!==true||committed.generation!==snapshot.generation)throw new Error('Ordinary sealed Main commit receipt unavailable');
  state.worldMode=false;state.shotMode=false;state.autoFlow=false;state.worldAdvancePromise=null;state.worldSnapshot=null;
  const next=originalMainController();
  const audio=await ensureOriginalTitleAudio();if(!current())return;if(!audio)throw new Error('Ordinary Main requires actual current Title audio');
  const music=controller.detachMainMusic(),playing=audio.adoptOrdinaryTitleMusic(music);
  state.ordinaryMainMusic=music;state.source=playing.source;state.track=playing.track;state.buffer=playing.buffer;
  next.adoptHeld(state.paused?new Set():state.pressed);
  state.normalPostrun=null;state.normalPostrunPausePromise=state.normalPostrunResumePromise=null;
  state.normalPostrunPauseBinding=state.normalPostrunResumeBinding=null;controller.close();
  state.endingLoading=false;state.active=true;menu.page='main';menu.launching=false;
  state.background=null;state.backgroundInterpolationReady=false;state.playerInterpolationReady=false;
  clock.reset();resetHighFrameratePresentation();syncNativeMenuState();
  status('普通终局已按原生封存凭据交接原Main；原生菜单、Music7与当前按住键继续由现有owner处理。');
}
async function advanceNormalPostrun(){
  const controller=state.normalPostrun;if(!controller||state.paused)return;
  const snapshot=await controller.step();if(controller!==state.normalPostrun)return;
  if(snapshot?.menu?.mainHandoffReady){await finishNormalPostrunMain(controller,snapshot);return;}
  if(controller.phase==='awaiting-name-owner'||controller.phase==='awaiting-replay-owner'){
    state.paused=true;controller.input.release();clock.reset();resetHighFrameratePresentation();
    status(controller.phase==='awaiting-replay-owner'?'普通终局已到原生录像阶段；占用槽预览和完整录像保存owner尚待接线，已停止后续输入。':'普通终局原姓名字体owner尚未就绪，已停止后续输入。');
  }
}
async function advanceOriginalEnding(){
  if(!state.endingHost||state.endingBusy||state.paused)return;
  const ticket=state.endingTicket,host=state.endingHost;
  state.endingBusy=true;
  try{
    state.endingStepPromise=(async()=>{
      const snapshot=await host.owner.step();
      if(ticket!==state.endingTicket||host!==state.endingHost)return;
      if(snapshot.phase==='return-requested'){
        if(snapshot.ownerKind==='ordinary')await finishOrdinaryEndingReturn(host,snapshot);
        else{state.active=false;state.paused=true;clock.reset();resetHighFrameratePresentation();host.owner.input.release();
          status('原结局已请求 scene16；此诊断路线没有普通返回凭据。');}
      }
    })();
    await state.endingStepPromise;
  }catch(problem){if(ticket===state.endingTicket&&host===state.endingHost)throw problem;}
  finally{if(ticket===state.endingTicket&&host===state.endingHost){state.endingBusy=false;state.endingStepPromise=null;}}
}
function pauseOriginalEnding(){
  const ticket=state.endingTicket,module=state.module;
  state.endingHost?.owner.input.release();state.paused=true;clock.reset();resetHighFrameratePresentation();titlePauseOverlay();
  const pending=state.endingStepPromise??state.endingPromise??Promise.resolve();
  state.endingPausePromise=pending.then(async()=>{
    if(ticket!==state.endingTicket||module!==state.module||!state.paused)return;
    if(state.normalPostrun){await pauseNormalPostrun();return;}
    if(!state.endingHost)return;
    state.endingHost.owner.setPaused(true);
    if(state.audio?.state==='running')await state.audio.suspend();
  });state.endingPausePromise.catch(problem=>{if(ticket===state.endingTicket&&module===state.module)error(problem);});
}
function resumeOriginalEnding(){
  if(state.endingResumePromise)return state.endingResumePromise;
  const ticket=state.endingTicket,module=state.module;
  const pending=(async()=>{
    await state.endingPausePromise;if(state.endingPromise)await state.endingPromise;
    if(ticket!==state.endingTicket||module!==state.module||!state.paused)return;
    if(state.normalPostrun){await resumeNormalPostrun();return;}
    if(!state.endingHost)return;
    state.endingHost.owner.setPaused(false);
    if(state.audio?.state==='suspended')await state.audio.resume();
    if(ticket!==state.endingTicket||module!==state.module)return;
    state.paused=false;clock.reset();resetHighFrameratePresentation();find('#runtime-overlay').hidden=true;status('动画继续。');
  })();state.endingResumePromise=pending;
  pending.catch(problem=>{if(ticket===state.endingTicket&&module===state.module)error(problem);})
    .finally(()=>{if(state.endingResumePromise===pending)state.endingResumePromise=null;});
  return pending;
}

async function advance(){
  if(!state.worldMode)return advanceSerial();
  if(state.worldAdvancePromise)return state.worldAdvancePromise;
  const module=state.module,action=state.action,selection=state.stageSelection;
  const pending=advanceSerial();state.worldAdvancePromise=pending;
  try{return await pending;}
  catch(problem){if(problem.name==='AbortError'&&(module!==state.module||action!==state.action||selection!==state.stageSelection))return;throw problem;}
  finally{if(state.worldAdvancePromise===pending)state.worldAdvancePromise=null;}
}

async function advanceSerial(){
  if(state.endingHost||state.endingLoading)throw new Error('Original ending must use its serial asynchronous scene owner');
  if(state.worldMode){
    if(state.extraRun)return advanceExtraRun();
    const endingRequest=pendingOriginalEndingRequest();
    if(endingRequest){
      if(endingRequest.kind==='ordinary'&&(!state.module._th128_world_terminal_preupdate||!state.module._th128_world_terminal_preupdate()))
        throw new Error(state.module.UTF8ToString(state.module._th128_world_error())||'Original ordinary priority1 owner unavailable');
      if(maybeStartOriginalEnding())return;
      throw new Error('Original ending request changed at the logical preupdate boundary');
    }
    if(state.thpracRecorder){const seconds=performance.now()/1000,delta=seconds-state.replayClockLast;state.thpracRecorder.clock(seconds,delta>0?1/delta:0);state.replayClockLast=seconds;}
    if(state.replayNativeBound){
      const seconds=performance.now()/1000,delta=seconds-state.replayClockLast;
      const fps=delta>0?1/delta:0;
      if(!state.module._th128_world_replay_clock(seconds,fps))throw new Error(state.module.UTF8ToString(state.module._th128_world_error()));
      state.replayClockLast=seconds;
    }
    if(state.replayPlayback?.snapshot().complete){
      if(!state.replayPlayback.hasNext())finishOwnedReplay();
      else if(!state.worldSnapshot?.stageTransition?.requested)finishOwnedReplay('录像段已结束，但实际关卡未请求下一段；已停止');
      return;
    }
    const advanced=state.module._th128_world_tick();
    if(advanced===2){await drainGameSounds();if(state.replayPlayback)finishOwnedReplay('实际游戏进入结束状态，回放已停止');else await openOriginalNativePause(true);return;}
    await drainGameSounds();
    if(advanced){await drainMusicControl();state.background?.tick();if(state.replayPlayback){const replay=state.replayPlayback.snapshot();if(replay.complete&&!state.replayPlayback.hasNext())finishOwnedReplay();}}
    if(!advanced){
      state.active=false;state.paused=true;clock.reset();resetHighFrameratePresentation();
      if(state.audio?.state==='running')state.audio.suspend().catch(()=>{});
      const reason=state.module.UTF8ToString(state.module._th128_world_error());
      const snapshot=JSON.parse(state.module.UTF8ToString(state.module._th128_world_state()));
      state.worldSnapshot=snapshot;
      syncDebugRespawnUI(snapshot);
      find(state.shotMode?'#shots-result':'#segment-result').textContent=`真实首段在控制 tick ${snapshot.frame} 明确停止：${reason}\n${JSON.stringify(snapshot,null,2)}\n不是完整玩法；没有跳过未实现分支。`;
      find('#emit-shot').disabled=true;
      enterFaultMenu(`当前分支停止：${reason}\n可以直接重新选择 RESTART 或 TITLE，不需要刷新整个网页。`);
      draw();status(`真实首段停在未实现分支：${reason}`);
    }
    return;
  }
  if(state.mainSaveDebugBusy)return;
  await originalMainController().step();
  syncNativeMenuState();
}

async function advanceExtraRun(){
  const owner=state.extraRun,module=state.module,action=state.action;
  const current=()=>state.extraRun===owner&&state.module===module&&state.action===action;
  const result=await owner.step({
    drainAudio:async()=>{await drainGameSounds();await drainMusicControl();},
    render:presentation=>{
      if(!current())throw new DOMException('EX presentation owner changed','AbortError');
      state.background?.tick();state.extraPresentation=presentation;
      try{draw();}finally{state.extraPresentation=null;}
    },
    onGameOver:()=>openOriginalNativePause(true)
  });
  if(result.retired){
    if(!current())throw new DOMException('EX terminal owner changed','AbortError');
    await finishExtraPostrun(result.snapshot);
    if(current()){owner.close();state.extraRun=null;}
  }
  return result;
}

function nativeMainRows(){
  const list=find('#menu-main-list');if(!list)return [];
  if(nativeMainOrderReady)return [...list.children];
  const rows=new Map([...list.children].filter(row=>row.matches?.('[data-menu-main]')).map(row=>[row.dataset.menuMain,row]));
  if(rows.size!==nativeMainIds.length)return [...list.children];
  const ordered=nativeMainIds.map(id=>rows.get(id));
  if(ordered.some(row=>!row))return [...list.children];
  for(const row of ordered)list.append(row);
  menu.main=Math.max(0,Math.min(ordered.length-1,menu.main));
  nativeMainOrderReady=true;return ordered;
}
const ORIGINAL_MAIN_MUSIC_PACK_SHA256='a546636b979c5b01d387434e9c1514a9507190cb78f0c85585e1fa8107fd6190';

function originalMainSettingsStore(){
  state.mainSettings??=new OriginalMainSettingsStore();return state.mainSettings;
}
function updateOwnedMainSettings(change){
  const prior=state.mainStorageTask??Promise.resolve();
  const operation=prior.catch(()=>{}).then(async()=>{
    const store=originalMainSettingsStore();
    for(let attempt=0;attempt<3;++attempt){
      const observed=await store.observe(),changes=change(observed);
      if(!changes)return observed;
      try{return await store.commit(observed.revision,changes);}
      catch(problem){if(attempt===2)throw problem;}
    }
  });
  state.mainStorageTask=operation;return operation;
}
function recordOwnedHeardTrack(name){
  const row=['07','00','02','01','05','06','10','11','09','08'].findIndex(value=>name==='th128_'+value+'.wav');
  if(!musicOutputEnabled()||state.replayPlayback||row<0||state.audio?.state!=='running')return Promise.resolve();
  return updateOwnedMainSettings(observed=>(observed.seenMask&(1<<row))?null:{seenMask:observed.seenMask|(1<<row)}).then(saved=>{
    if(!state.worldMode){const snapshot=nativeMenuSnapshot();if(snapshot.page===0&&snapshot.ready&&!snapshot.transitioning)state.module._th128_title_music_prepare(saved.seenMask);}
    return saved;
  });
}
function recordOwnedMainSession(){
  if(state.replayPlayback||state.replayReturning||!state.worldMode||!state.active||!state.stageSelection)return;
  const module=state.module,snapshot=JSON.parse(module.UTF8ToString(module._th128_world_state())),owner=snapshot.normalRecord;
  if(!owner?.bound||!owner.started||owner.tainted||snapshot.debug?.runTainted||owner.frames<1)return;
  if(owner.acknowledged)return state.nativeScoreSaveTask;
  if(state.nativeScoreSaveTask)return state.nativeScoreSaveTask;
  const date=new Date(),timestamp=BigInt(Math.floor(date.getTime()/1000));
  if(timestamp<0n||timestamp>0x793406fffn)throw new Error('本局成绩时间不可用');
  const bytes=module._th128_world_records_prepare(Number(timestamp&0xffffffffn),Number(timestamp>>32n),performance.now()/1000);
  if(bytes!==52232)throw new Error(module.UTF8ToString(module._th128_world_error())||'原生成绩保存准备失败');
  const actual=JSON.parse(module.UTF8ToString(module._th128_world_state())),native={...actual.normalRecord,generation:actual.flow.worldGeneration};
  const pointer=module._th128_world_records_data(),profile=module.HEAPU8.slice(pointer,pointer+bytes);
  const capture={profile,native,runId:state.nativeScoreRunId};
  const store=state.nativeScoreStore??=new OwnedNativeTitleRecordStore();
  const prior=state.mainStorageTask??Promise.resolve();
  const operation=prior.then(()=>persistOwnedNativeScoreCapture(module,store,state.nativeScoreRevision??0,capture)).then(receipt=>{state.nativeScoreRevision=receipt.revision;return receipt;});
  state.nativeScoreSaveTask=operation;state.mainStorageTask=operation;operation.catch(error);return operation;
}
function nativeMainMemory(bytes,operation){
  const pointer=state.module._malloc(bytes.byteLength);
  try{state.module.HEAPU8.set(bytes,pointer);return operation(pointer,bytes.byteLength);}finally{state.module._free(pointer);}
}
function actualMainControllerButtons(){
  let buttons=0,held=0;
  const pads=navigator.getGamepads?.()??[];
  const pad=Array.from(pads).find(value=>value?.connected);
  if(!pad)return{buttons,held};
  for(let index=0;index<Math.min(31,pad.buttons.length);++index)if(pad.buttons[index].pressed||pad.buttons[index].value>=0.5)buttons=(buttons|(1<<index))>>>0;
  if((pad.axes[1]??0)< -0.5||pad.buttons[12]?.pressed)held|=0x10;
  if((pad.axes[1]??0)>0.5||pad.buttons[13]?.pressed)held|=0x20;
  if((pad.axes[0]??0)< -0.5||pad.buttons[14]?.pressed)held|=0x40;
  if((pad.axes[0]??0)>0.5||pad.buttons[15]?.pressed)held|=0x80;
  const mapping=state.mainSavedSettings?.mapping??[0,1,3,2,4];
  if(buttons&(1<<mapping[0]))held|=1;
  if(buttons&(1<<mapping[1]))held|=2;
  if(buttons&(1<<mapping[2]))held|=0x80000;
  if(buttons&(1<<mapping[4]))held|=0x100;
  if(nativeMenuSnapshot().page===4)held&=0xf0;
  return{buttons,held};
}
async function loadOriginalMainRasters(){
  const signal=state.controller.signal,url=safeAssetUrl(state.base,'animations/original-main-music.json');
  const response=await checkpointFetch(url,{signal});if(!response.ok)throw new Error('Original Music raster catalogue unavailable');
  const bytes=new Uint8Array(await response.arrayBuffer());
  const digest=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),value=>value.toString(16).padStart(2,'0')).join('');
  if(digest!==ORIGINAL_MAIN_MUSIC_PACK_SHA256)throw new Error('Original Music catalogue full-byte pin differs');
  const metadata=JSON.parse(new TextDecoder('utf8',{fatal:true}).decode(bytes));
  const bank=await loadOriginalMainMusicRasters(metadata,async(file,expected)=>{
    const response=await checkpointFetch(safeAssetUrl(state.base,file),{signal});if(!response.ok)throw new Error('Original Music glyph PNG unavailable');
    const bytes=await response.arrayBuffer(),actual=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),value=>value.toString(16).padStart(2,'0')).join('');
    if(actual!==expected)throw new Error('Original Music glyph PNG full-byte pin differs');
    return await decodeOriginalMainMusicPNG(bytes);
  },{signal});
  return{metadata,bank};
}
const ORIGINAL_RESULT_GLYPH_PACK_SHA256='137f7e9b1a49036504d0ed620306d153dfbb4bdfafe82cc065bfca85b93501b6';
async function loadOriginalResultRasters(){
  const signal=state.controller.signal,glyphBase=new URL('/assets/'+state.catalog.variants.zh.manifest.replace(/manifest\.json$/,''),state.base).href;
  const response=await checkpointFetch(safeAssetUrl(glyphBase,'animations/original-result-glyphs.json'),{signal});if(!response.ok)throw new Error('Original RESULT glyph catalogue unavailable');
  const bytes=new Uint8Array(await response.arrayBuffer()),digest=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),b=>b.toString(16).padStart(2,'0')).join('');if(digest!==ORIGINAL_RESULT_GLYPH_PACK_SHA256)throw new Error('Original RESULT glyph catalogue whole bytes differ');
  const metadata=JSON.parse(new TextDecoder('utf8',{fatal:true}).decode(bytes));return await loadOriginalResultFont(metadata,async(file,expected)=>{const response=await checkpointFetch(safeAssetUrl(glyphBase,file),{signal});if(!response.ok)throw new Error('Original RESULT glyph page unavailable');const page=new Uint8Array(await response.arrayBuffer()),actual=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',page)),b=>b.toString(16).padStart(2,'0')).join('');if(actual!==expected)throw new Error('Original RESULT glyph page whole bytes differ');return await decodeOriginalMainMusicPNG(page);},{signal});
}
async function observeOriginalExtraQualification(module,signal){
  const action=state.action,selection=state.stageSelection,controller=state.controller;
  const owner=state.extraQualificationOwner??=new QualificationOwner(),token=owner.begin({signal});
  const current=()=>!signal.aborted&&state.module===module&&state.controller===controller&&state.action===action&&state.stageSelection===selection&&owner.isCurrent(token);
  const check=()=>{if(!current())throw new DOMException('Original EX qualification owner replaced','AbortError');};
  check();const record=await new OwnedNativeTitleRecordStore().observe();check();
  if(!record)return {record:null,qualification:null,token};
  const names=new OwnedLastNameFieldStore();let name;try{name=await names.observe();}finally{names.close();}check();
  let qualification;
  try{qualification=await evaluateOwnedQualification({record,name,owner,token});}
  catch(problem){
    check();
    // A failed run can save scores before any replay name was ever saved.
    // That leaves optional EX proof unavailable, while the Main score record
    // remains valid. Never manufacture or persist the missing name receipt.
    if(problem.code!=='name-absent'||name.strictRead!==true||name.absent!==true||name.revision!==0||name.digest!==null)throw problem;
    originalNameProfilePacket(name);
    return {record,name,qualification:null,token};
  }
  check();
  return {record,name,qualification,token};
}
async function prepareOriginalMain(){
  const module=state.module,signal=state.controller.signal;
  await state.mainStorageTask;
  const saved=await originalMainSettingsStore().observe();
  state.mainSavedSettings=saved;
  setOwnedExtraSaveEligibility(null);state.extraQualification=null;
  const observedExtra=await observeOriginalExtraQualification(module,signal),nativeTitleRecord=observedExtra.record;
  if(signal.aborted||state.module!==module)throw new DOMException('Original EX qualification replaced','AbortError');
  state.extraQualification=observedExtra.qualification;state.extraQualificationToken=observedExtra.token;
  setOwnedExtraSaveEligibility(observedExtra.qualification);setDebugExtraSaveEligibility(saved.debugClearMask??0);state.extraMenu?.refresh?.();
  const extraMask=observedExtra.qualification?.eligible?observedExtra.qualification.mask:(saved.debugClearMask??0);
  if(module._th128_title_extra_save(extraMask)!==1)throw new Error('Original observed route flags rejected');
  const mapping=new Int32Array(saved.mapping);
  if(nativeMainMemory(new Uint8Array(mapping.buffer),(pointer)=>module._th128_title_settings_restore(saved.bgm,saved.se,saved.rapid,pointer,5))!==1)
    throw new Error('Original Title owned settings restore rejected');
  for(const [bank,name]of [[6,'text'],[46,'help']]){
    const metadata=await readJson(safeAssetUrl(state.base,'animations/'+name+'.json'),signal);
    const response=await checkpointFetch(safeAssetUrl(state.base,'runtime/'+name+'.anm'),{signal});if(!response.ok)throw new Error('Original main ANM unavailable');
    const bytes=new Uint8Array(await response.arrayBuffer());
    if(bank===6){if(nativeMainMemory(bytes,(pointer,length)=>module._th128_load_anm(bank,pointer,length))!==1)throw new Error(coreError());}
    else if(state.mainHelpModule!==module){
      if(nativeMainMemory(bytes,(pointer,length)=>module._th128_title_help_load(pointer,length))!==1)throw new Error('Original main Help bank rejected');
      state.mainHelpModule=module;
    }
    await state.renderer.loadBank(bank,metadata,state.base,{signal});
  }
  const rasters=await loadOriginalMainRasters();
  if(signal.aborted||state.module!==module){disposeOriginalMainMusicRasters(rasters.bank);throw new DOMException('Aborted','AbortError');}
  disposeOriginalMainMusicRasters(state.mainRasters);state.mainRasters=rasters.bank;
  const variant=find('#variant').value,catalog=rasters.metadata.variantCatalogs?.[variant];
  if(!catalog||!/^(?:[a-f0-9]{2})+$/.test(catalog.rawHex))throw new Error('Original Music variant catalogue unavailable');
  const bytes=Uint8Array.from(catalog.rawHex.match(/../g),value=>parseInt(value,16));
  if(nativeMainMemory(bytes,(pointer,length)=>module._th128_title_music_catalog(pointer,length,variant==='zh'?0:1))!==1
    ||module._th128_title_music_prepare(saved.seenMask)!==1)throw new Error('Original Music owned catalogue rejected');
  state.mainReplayStore??=new OwnedReplayStore();state.mainReplayRows=await state.mainReplayStore.observe();
  const packet=originalReplayPreviewPacket(state.mainReplayRows);
  if(nativeMainMemory(packet,(pointer,length)=>module._th128_title_replay_prepare(pointer,length))!==1)throw new Error('Original owned main Replay rows rejected');
  const results=new Uint8Array(8+saved.results.length*16),view=new DataView(results.buffer);
  view.setUint32(0,0x5352454d,true);view.setUint32(4,saved.results.length,true);
  saved.results.forEach((row,index)=>{[row.stage,row.rank,row.score,Number(row.complete)].forEach((word,field)=>view.setUint32(8+index*16+field*4,word,true));});
  if(nativeMainMemory(results,(pointer,length)=>module._th128_title_results_prepare(pointer,length))!==1)throw new Error('Owned actual main Results rejected');
  const routeScores=nativeRouteScorePacket(nativeTitleRecord?.profile??null);
  if(nativeMainMemory(routeScores,(pointer,length)=>module._th128_title_results_prepare(pointer,length))!==1)throw new Error('Owned native Route score snapshot rejected');
  const fullResult=originalResultPacket(originalResultModel(nativeTitleRecord?.profile??null));
  if(nativeMainMemory(fullResult,(pointer,length)=>module._th128_title_results_prepare(pointer,length))!==1)throw new Error('Owned complete RESULT snapshot rejected');
  state.mainResultGlyphs=await loadOriginalResultRasters();state.mainResultTextCache=new Map();
  if(signal.aborted||state.module!==module)throw new DOMException('Original RESULT load replaced','AbortError');
  state.mainTextCache=new Map();state.mainHelpPage=-1;
}
async function ensureOriginalTitleAudio(){
  if(state.titleAudio)return state.titleAudio;
  if(!menu.audioUnlocked||state.audio?.state!=='running')return null;
  if(state.mainAudioLoading)return state.mainAudioLoading;
  const module=state.module,signal=state.controller.signal,ticket=state.mainAudioTicket??0;
  state.mainAudioLoading=(async()=>{
    const ledger=new OwnedMenuSoundLedger(),bgmLedger=new OwnedMenuSoundLedger();
    const audio=new OriginalTitleAudio({context:state.audio,gain:state.gain,base:state.base,tracks:state.manifest.music,resolveUrl:safeAssetUrl,signal,bgmLedger,
      onStopTitle:()=>stopTitleSource(),onStartTitle:()=>{startTitleSource();return{source:state.source,track:state.track,buffer:state.buffer};},
      onPreview:async row=>{state.mainPreviewTrack=row;}});
    try{
      await audio.load(await readJson('/assets/runtime/sound-bindings.json',signal));
      if(signal.aborted||state.module!==module||state.worldMode||ticket!==(state.mainAudioTicket??0))throw new DOMException('Title audio load replaced','AbortError');
      state.titleAudio=audio;state.mainSoundLedger=ledger;state.mainBgmLedger=bgmLedger;
      state.titleSoundConsumer=new TitleSoundWindowConsumer(module,audio.sounds,ledger,{present:true});
      if(state.source){state.source.disconnect();state.source.connect(audio.bgmGain);}
      const saved=state.mainSavedSettings??(state.normalPostrun?await originalMainSettingsStore().observe():OriginalMainSettingsStore.defaults());
      if(signal.aborted||state.module!==module||ticket!==(state.mainAudioTicket??0))throw new DOMException('Title settings owner replaced','AbortError');
      state.mainSavedSettings=saved;audio.setVolumes(saved.bgm,saved.se);return audio;
    }catch(problem){audio.dispose();ledger.close();bgmLedger.close();throw problem;}
    finally{if(ticket===(state.mainAudioTicket??0))state.mainAudioLoading=null;}
  })();return state.mainAudioLoading;
}
async function syncOriginalMainPresentation(){
  if(!state.mainRasters)return;
  const snapshot=nativeMenuSnapshot(),module=state.module;
  if(snapshot.page===5){
    const count=module._th128_title_music_text_count();
    if(!Number.isInteger(count)||count<0||count>18)throw new Error('Original Music text snapshot capacity differs');
    for(let index=0;index<count;++index){
      const raw=module.UTF8ToString(module._th128_title_music_text_request(index));if(!raw||state.mainTextCache.get(index)===raw)continue;
      const request=JSON.parse(raw);await uploadOriginalMainMusicRequest(state.renderer,state.mainRasters,request,find('#variant').value,{signal:state.controller.signal});
      state.mainTextCache.set(index,raw);
    }
  }else state.mainTextCache.clear();
  if(snapshot.page===7){
    if(!state.mainResultGlyphs)throw new Error('Original RESULT glyph owner unavailable');const count=module._th128_title_music_text_count();if(!Number.isInteger(count)||count<0||count>10)throw new Error('Original RESULT text request bounds differ');if(!count)state.mainResultTextCache.clear();
    for(let index=0;index<count;index++){const raw=module.UTF8ToString(module._th128_title_music_text_request(index));if(!raw||state.mainResultTextCache.get(index)===raw)continue;await uploadOriginalResultTextRequest(state.renderer,state.mainResultGlyphs,JSON.parse(raw),{signal:state.controller.signal});state.mainResultTextCache.set(index,raw);}
  }else state.mainResultTextCache?.clear();
  if(snapshot.page===8&&snapshot.helpPage>=0&&state.mainHelpPage!==snapshot.helpPage){
    const reference=originalHelpPagePins[find('#variant').value]?.[snapshot.helpPage];if(!reference)throw new Error('Original main Help page unavailable');
    await loadOriginalHelpPage(state.renderer,{...reference,url:safeAssetUrl(state.base,reference.file)},{signal:state.controller.signal});state.mainHelpPage=snapshot.helpPage;
  }
}
async function syncOriginalMainSettings(snapshot){
  syncOriginalReplayImportControls(snapshot);await refreshOriginalReplayImportRows(snapshot);
  if(!state.mainSavedSettings||snapshot.keyPrepared!==1||!state.mainController?.prepared||state.mainController.disposed)return;
  const changes={bgm:snapshot.bgmVolume,se:snapshot.seVolume,rapid:snapshot.rapid,mapping:snapshot.keyCommitted};
  const saved=state.mainSavedSettings;
  if(saved.bgm!==changes.bgm||saved.se!==changes.se||saved.rapid!==changes.rapid||JSON.stringify(saved.mapping)!==JSON.stringify(changes.mapping))
    state.mainSavedSettings=await updateOwnedMainSettings(()=>changes);
  state.titleAudio?.setVolumes(changes.bgm,changes.se);
  find('#export-owned-replay').hidden=snapshot.page!==6||snapshot.replayDetails===1;
  if(snapshot.page===0&&snapshot.ready&&!snapshot.transitioning&&state.replayReturnSlot!==null&&state.replayReturnSlot!==undefined){const slot=state.replayReturnSlot;state.replayReturnSlot=null;nativeMenuPage(6,slot);return;}
  if(snapshot.page===0)status('原版主菜单 · 方向键选择，Z/Enter 确认，X/Esc 返回。');
}

function syncOriginalReplayImportControls(snapshot){
  const button=find('#import-original-replay');
  button.hidden=state.worldMode||!state.active||!snapshot.ready||!!snapshot.transitioning
    ||(snapshot.page!==0&&(snapshot.page!==6||snapshot.replayDetails===1));
  button.disabled=!!state.mainSaveDebugBusy||!!state.mainReplayImportPending||!!menu.launching;
}
async function importOriginalReplayFile(file){
  const before=nativeMenuSnapshot();
  if(state.worldMode||!state.active||menu.launching||state.mainSaveDebugBusy||state.mainReplayImportPending
      ||!before.ready||before.transitioning||(before.page!==0&&(before.page!==6||before.replayDetails===1)))
    throw new Error('请在主菜单或 REPLAY 列表导入原版录像');
  if(!file||typeof file.arrayBuffer!=='function'||!Number.isSafeInteger(file.size)||file.size<36
      ||file.size>16*1024*1024||typeof file.name!=='string'||!file.name.toLowerCase().endsWith('.rpy'))
    throw new Error('请选择原版 .rpy 录像文件（最大16MiB）');
  const module=state.module,action=state.action,controller=originalMainController();
  const current=()=>state.module===module&&state.action===action&&!state.worldMode&&state.active;
  state.mainSaveDebugBusy=true;controller.release();suppressHeldConfirmKeys();
  syncOriginalReplayImportControls(before);find('#export-owned-replay').disabled=true;
  try{
    const deadline=Date.now()+10000;
    while(controller.busy){
      if(!current()||Date.now()>deadline)throw new DOMException('录像导入等待菜单结束失败','AbortError');
      await new Promise(resolve=>setTimeout(resolve,10));
    }
    if(!current())throw new DOMException('录像导入页面已变更','AbortError');
    const settled=nativeMenuSnapshot();
    if(!settled.ready||settled.transitioning||(settled.page!==0&&(settled.page!==6||settled.replayDetails===1)))
      throw new DOMException('录像导入前菜单页面已变更','AbortError');
    const selected=new Uint8Array(await file.arrayBuffer());
    if(selected.length!==file.size)throw new Error('所选录像文件读取长度不一致');
    state.mainReplayStore??=new OwnedReplayStore();
    const receipt=await state.mainReplayStore.importOriginal(selected,{fileName:file.name,isCurrent:current});
    if(!current())throw new DOMException('录像导入页面已变更','AbortError');
    if(receipt.strictFullReadback!==true||receipt.externalSource!==true||receipt.scoreProfileImported!==false)
      throw new Error('原版录像完整文件回读凭据不可用');
    state.mainReplayImportPending={slot:receipt.slot,module,action};state.mainReplayChoice=null;
    if(settled.page===6){controller.keyDown('Escape');controller.keyUp('Escape');}
    status(`原版录像${receipt.alreadyPresent?'已存在于':'已导入'}第 ${receipt.slot+1} 槽；正在更新 REPLAY 列表。仅导入录像文件。`);
  }finally{
    find('#import-original-replay-file').value='';find('#export-owned-replay').disabled=false;
    state.mainSaveDebugBusy=false;clock.reset();resetHighFrameratePresentation();
    syncOriginalReplayImportControls(nativeMenuSnapshot());
  }
}
async function refreshOriginalReplayImportRows(snapshot){
  const pending=state.mainReplayImportPending;
  if(!pending||snapshot.page!==0||!snapshot.ready||snapshot.transitioning)return;
  if(state.module!==pending.module||state.action!==pending.action||state.worldMode){state.mainReplayImportPending=null;return;}
  const rows=await state.mainReplayStore.observe();
  if(state.module!==pending.module||state.action!==pending.action||state.worldMode){state.mainReplayImportPending=null;return;}
  const packet=originalReplayPreviewPacket(rows);
  if(nativeMainMemory(packet,(pointer,length)=>state.module._th128_title_replay_prepare(pointer,length))!==1)
    throw new Error('原版录像已保存，但原生 REPLAY 列表更新被拒绝');
  state.mainReplayRows=rows;state.replayReturnSlot=pending.slot;state.mainReplayImportPending=null;
  status('原版录像已加入 REPLAY；选择关卡后按 Z/Enter 观看。');
}

async function showOwnedReplayStages(snapshot){
  if(state.module._th128_title_sound_count()!==0)return;
  const row=(await state.mainReplayStore.observe())[snapshot.selection];
  if(row?.bytes&&readThprac128Replay(row.bytes)){
    if(state.module._th128_title_main_action_ack(snapshot.mainActionSerial,3)!==1)throw new Error('Practice Replay selection action rejected');
    state.replayStore=state.mainReplayStore;await watchThpracReplayFromMenu(snapshot.selection);return;
  }
  const owner=await OwnedReplayPlayback.fromStore(state.mainReplayStore,snapshot.selection),packet=owner.stageChoicePacket();
  if(nativeMainMemory(packet,(pointer,length)=>state.module._th128_title_replay_stage_prepare(pointer,length))!==1)
    throw new Error('原录像关卡选择页不可用');
  state.mainReplayChoice=owner;
  if(state.module._th128_title_main_action_ack(snapshot.mainActionSerial,3)!==1)throw new Error('Replay detail action rejected');
  status('选择录像关卡；灰色横线表示该关没有录制，Z/Enter播放，X返回录像列表。');
}
async function startOwnedReplayFromMain(snapshot){
  if(state.module._th128_title_sound_count()!==0)return;
  const owner=state.mainReplayChoice;if(!owner)throw new Error('尚未选择实际录像');
  const selection=owner.chooseStage(snapshot.selection),module=state.module;
  if(module._th128_title_main_action_ack(snapshot.mainActionSerial,4)!==1)throw new Error('Replay selected-stage action rejected');
  state.replayPlayback=owner;state.replayFinished=false;state.replayReturnSlot=null;
  find('#export-owned-replay').hidden=true;find('#import-original-replay').hidden=true;
  try{
    await startStage(selection.stage,selection);
    if(state.replayPlayback!==owner)throw new Error('Replay playback load ownership changed');
    status('正在观看录像 · Esc打开回放专用暂停菜单。');
  }catch(problem){
    if(state.replayPlayback!==owner)return;
    try{owner.cancel();}catch{}
    state.replayPlayback=null;state.replayReturnSlot=owner.slot;
    state.replayReturning=true;try{returnToTitle();}finally{state.replayReturning=false;}
    throw problem;
  }
}
async function restartOwnedReplay(){
  const prior=state.replayPlayback;if(!prior)throw new Error('Replay Again lacks its owned file');
  const next=prior.restartSelected(),selection=next.firstSelection();prior.cancel();
  state.replayPlayback=next;state.replayFinished=false;menu.pauseOpen=false;
  const prepared=prepareStage(selection.stage,selection);
  return startLaunch(prepared,'route');
}
function returnOwnedReplayToTitle(){
  const owner=state.replayPlayback;if(!owner)return;owner.cancel();state.replayPlayback=null;state.replayReturnSlot=null;
  state.replayReturning=true;try{returnToTitle();}finally{state.replayReturning=false;}
}
async function exportSelectedOwnedReplay(){
  const snapshot=nativeMenuSnapshot();
  if(snapshot.page!==6||snapshot.replayDetails===1||!snapshot.ready||snapshot.transitioning||!state.mainReplayStore)return;
  const bytes=await state.mainReplayStore.export(snapshot.selection),url=URL.createObjectURL(new Blob([bytes],{type:'application/octet-stream'}));
  try{
    const link=document.createElement('a');link.href=url;
    link.download='th128_'+String(snapshot.selection+1).padStart(2,'0')+'.rpy';link.click();
  }finally{setTimeout(()=>URL.revokeObjectURL(url),1000);}
  status('已按独立导出操作下载录像；Z/Enter 是进入游戏观看。');
}
function stopOwnedReplayToList(){
  const owner=state.replayPlayback;if(!owner)return;
  const slot=owner.slot;owner.cancel();state.replayPlayback=null;state.replayFinished=false;state.replayReturnSlot=slot;
  state.replayReturning=true;
  try{returnToTitle();}finally{state.replayReturning=false;}
}
function finishOwnedReplay(message='录像播放结束'){
  state.replayFinished=true;state.paused=true;clock.reset();resetHighFrameratePresentation();state.pressed.clear();
  state.audio?.suspend().catch(()=>{});
  find('#runtime-overlay-title').textContent=message;
  find('#runtime-overlay-message').textContent='Z/Enter 或 Esc 返回录像列表。没有继续生成录像之外的输入。';
  find('#resume-session').hidden=true;find('#restart-stage').hidden=true;find('#runtime-overlay').hidden=false;
  status(message);
}

async function handleOriginalMainAction(snapshot){
  const module=state.module;
  if(snapshot.mainAction===1){
    if(module._th128_title_sound_count()!==0)return;
    if(module._th128_title_main_action_ack(snapshot.mainActionSerial,1)!==1)throw new Error('Title launch action ownership rejected');
    launchNativeRoute(snapshot.selection);return;
  }
  if(snapshot.mainAction===2){
    stopTitleSource();state.titleAudio?.music.stopSource();
    if(module._th128_title_main_action_ack(snapshot.mainActionSerial,2)!==1)throw new Error('Title Quit action ownership rejected');
    state.mainQuit=true;state.paused=true;clock.reset();resetHighFrameratePresentation();state.mainController.release();
    titlePauseOverlay();find('#runtime-overlay-title').textContent='已退出游戏';
    find('#runtime-overlay-message').textContent='Z/Enter 返回标题；网页不会强制关闭浏览器。';
    status('已退出游戏，音频已停止。');return;
  }
  if(snapshot.mainAction===3)await showOwnedReplayStages(snapshot);
  if(snapshot.mainAction===4)await startOwnedReplayFromMain(snapshot);}
function originalMainController(){
  if(state.mainController)return state.mainController;
  state.mainController=new OriginalMainMenuController({module:state.module,prepare:prepareOriginalMain,getButtons:actualMainControllerButtons,
    consumeSound:async()=>{if(!state.module._th128_title_sound_count())return;const audio=await ensureOriginalTitleAudio();if(audio)await state.titleSoundConsumer.consume();},
    consumeMusic:async()=>{const request=JSON.parse(state.module.UTF8ToString(state.module._th128_title_music_bgm_request()));if(!request.serial)return;
      const audio=await ensureOriginalTitleAudio();if(audio)while(JSON.parse(state.module.UTF8ToString(state.module._th128_title_music_bgm_request())).serial){if(!await audio.consumeMusic(state.module))break;}},
    syncText:syncOriginalMainPresentation,onState:syncOriginalMainSettings,onAction:handleOriginalMainAction});
  return state.mainController;
}
function disposeOriginalMain(){
  state.mainAudioTicket=(state.mainAudioTicket??0)+1;state.mainAudioLoading=null;
  state.mainController?.dispose();state.mainController=null;state.titleAudio?.dispose();state.titleAudio=null;state.ordinaryMainMusic=null;
  state.mainSoundLedger?.close();state.mainBgmLedger?.close();state.mainSoundLedger=null;state.mainBgmLedger=null;state.titleSoundConsumer=null;
  disposeOriginalMainMusicRasters(state.mainRasters);state.mainRasters=null;state.mainReplayStore?.close();state.mainReplayStore=null;
  state.mainResultGlyphs=null;state.mainResultTextCache?.clear();
}

function nativeMenuSnapshot(){
  if(!state.module)return {available:false,page:null,selection:null,ready:false,transitioning:false,supportedPages:[],error:'module not ready'};
  if(typeof state.module._th128_title_menu_page!=='function'||
      typeof state.module._th128_title_menu_select!=='function'||
      typeof state.module._th128_title_menu_state!=='function'){
    return {available:false,page:null,selection:null,ready:false,transitioning:false,supportedPages:[],error:'native title menu API missing from build'};
  }
  try{
    const raw=state.module.UTF8ToString(state.module._th128_title_menu_state());
    const value=JSON.parse(raw);
    if(!Number.isInteger(value.page)||value.page<0||value.page>8||
        !Number.isInteger(value.selection)||!Array.isArray(value.supportedPages)||
        typeof value.ready!=='number'||typeof value.transitioning!=='number')
      throw new Error('invalid native title menu state JSON');
    return {...value,available:true,page:value.page,selection:value.selection,ready:value.ready===1,
      transitioning:value.transitioning===1,supportedPages:[...value.supportedPages],
      error:typeof value.error==='string'?value.error:'',raw};
  }catch(problem){
    return {available:false,page:null,selection:null,ready:false,transitioning:false,supportedPages:[],error:problem.message??String(problem)};
  }
}
function mainAudioDiagnosticSnapshot(){
  const module=state.module,title=state.titleAudio,pending=title?.pendingBgm;
  let nativeBgm=null,nativeSoundCount=null,commentBytes=[],nativeReadError=null;
  const snapshot=nativeMenuSnapshot();
  try{
    if(!state.worldMode&&module&&typeof module._th128_title_music_bgm_request==='function'){
      nativeBgm=JSON.parse(module.UTF8ToString(module._th128_title_music_bgm_request()));
      nativeSoundCount=module._th128_title_sound_count();
      const count=module._th128_title_music_text_count();
      for(let index=10;snapshot.page===5&&index<Math.min(count,18);++index){
        const request=JSON.parse(module.UTF8ToString(module._th128_title_music_text_request(index)));
        commentBytes.push(typeof request.textHex==='string'?request.textHex.length/2:null);
      }
    }
  }catch(problem){nativeReadError=problem.message??String(problem);}
  return {
    mode:state.worldMode?'world':'title',page:snapshot.page,selection:snapshot.selection,ready:snapshot.ready,transitioning:snapshot.transitioning,
    prepared:state.mainController?.prepared===true,controllerBusy:state.mainController?.busy===true,
    audioUnlocked:menu.audioUnlocked===true,unlockPending:!!menu.audioUnlockPromise,titleAudioLoading:!!state.mainAudioLoading,
    audioContext:state.audio?.state??'absent',titleAudioContext:title?.context?.state??'absent',
    titleBufferReady:!!state.buffer,titleTrack:state.track?.name??null,titleSourceActive:!!state.source,
    nativeBgm,nativeSoundCount,commentBytes,nativeReadError,
    pendingBgm:pending?{request:pending.request,performed:pending.performed===true,
      strictReceiptReadback:pending.committed?.strictReadback===true,receiptTrack:pending.receipt?.track??null}:null,
    previewTrack:title?.music?.track?.name??null,previewSourceActive:!!title?.music?.source,
    sessionPreviewRow:state.mainPreviewTrack??null,gameHeardMask:state.mainSavedSettings?.seenMask??null
  };
}
function syncNativeMenuState(){
  if(!state.active||state.worldMode){find("#import-original-replay").hidden=true;return null;}
  const snapshot=nativeMenuSnapshot();
  if(!snapshot.available||snapshot.page===null)return snapshot;
  if(menu.page!=='launch')menu.page=nativePageNames[snapshot.page];
  if(snapshot.page===0)menu.main=snapshot.selection;
  else if(snapshot.page===1)menu.difficulty=snapshot.selection;
  else if(snapshot.page===2)menu.route=snapshot.selection;
  syncThpracRouteMenu(snapshot);return snapshot;
}
function domMenuVisible(){
  const element=find('#game-menu');if(!element)return false;
  const style=getComputedStyle(element);
  return !element.hidden&&style.display!=='none'&&style.visibility!=='hidden'&&style.opacity!=='0';
}
function coreFlowEvidence(){
  if(!state.worldMode||!state.module?._th128_world_state)return null;
  try{
    const snapshot=JSON.parse(state.module.UTF8ToString(state.module._th128_world_state()));
    const flow=snapshot?.flow;
    if(!flow||typeof flow!=='object')return null;
    return {currentId:flow.currentId??null,difficulty:flow.difficulty??null,
      shotDifficulty:flow.shotDifficulty??null,playerShotsConfigured:flow.playerShotsConfigured??null,
      debug:snapshot?.debug??null};
  }catch{return null;}
}
function selectNativePage(delta){
  if(!state.active||state.paused||state.worldMode||state.mainSaveDebugBusy)return;
  const snapshot=nativeMenuSnapshot();
  if(!snapshot.available){status(`原生菜单不可用：${snapshot.error}`);return;}
  if(!snapshot.ready||snapshot.transitioning){status('原生菜单正在转页；等待原动画 ready。');return;}
  const limit=nativePageChoices[snapshot.page];
  if(!limit)return;
  const next=(snapshot.selection+delta+limit)%limit;
  if(!state.module._th128_title_menu_select(next)){status(`原生选择被拒绝：${nativeMenuSnapshot().error}`);return;}
  syncNativeMenuState();
  status(`原生 ${nativePageNames[snapshot.page]} · 选择 ${next+1}/${limit}`);
}
function nativeMenuPage(page,choice){
  if(state.mainSaveDebugBusy)return false;
  const snapshot=nativeMenuSnapshot();
  if(!snapshot.available){status(`原生菜单不可用：${snapshot.error}`);return false;}
  if(!snapshot.ready||snapshot.transitioning){status('原生菜单正在转页；等待原动画 ready。');return false;}
  if(!state.module._th128_title_menu_page(page,choice)){
    status(`原生转页被拒绝：${nativeMenuSnapshot().error}`);return false;
  }
  syncNativeMenuState();
  status(`原生菜单切到 ${nativePageNames[page]}，等待 ready…`);
  return true;
}
function launchNativeRoute(route=menu.route){
  if(route===3){
    let access;try{access=requestNormalExtraStart();assertStageStartAllowed(16,4,{debugExtra:access.debugExtra});}
    catch(problem){status('Locked Stage · '+(problem.message??String(problem)));return;}
    if(access.profileSHA256&&(!state.extraQualificationOwner?.isCurrent(state.extraQualificationToken)||state.controller.signal.aborted)){
      status('EX 存档观察已过期，请重新准备主菜单。');return;
    }
    const selection=prepareStage(16,{difficulty:4,seed:34772,progress:0,debugExtra:true});
    if(access.profileSHA256)selection.ownedExtraProfileSHA256=access.profileSHA256;
    startLaunch(selection,'route');return;
  }
  const stageId=menuRouteStages[route];if(!stageId)return;
  let selection;
  try{selection=prepareStage(stageId,{difficulty:menu.difficulty,seed:34772,progress:0});}
  catch(problem){status(problem.message??String(problem));return;}
  startLaunch(selection,'route');
}
function confirmNativePage(){handleNativeTitleKey('Enter');state.mainController?.keyUp('Enter');}
function backNativePage(){handleNativeTitleKey('Escape');state.mainController?.keyUp('Escape');}
function handleNativeTitleKey(code){if(state.mainSaveDebugBusy)return;if(code==='KeyD'&&nativeMenuSnapshot().page===6&&nativeMenuSnapshot().replayDetails!==1){exportSelectedOwnedReplay().catch(error);return;}originalMainController().keyDown(code);}
function select(delta){
  if(!state.active||state.paused)return;
  if(!state.worldMode){selectNativePage(delta);return;}
  if(state.module._th128_frame()<72)return;
  state.module._th128_select((state.module._th128_selection()+delta+7)%7);
  status(`原版标题动画 · 逻辑帧 ${state.module._th128_frame()} · 原生选择 ${state.module._th128_selection()+1}/7`);
}

function titlePauseOverlay(){
  find('#runtime-overlay-title').textContent='动画已暂停';
  find('#runtime-overlay-message').textContent='Z/ENTER/X/ESC 继续动画。';
  find('#resume-session').hidden=true;find('#restart-stage').hidden=true;
  find('#runtime-overlay').hidden=false;
}
function resumeTitlePause(){
  if(state.normalPostrun){resumeNormalPostrun();return;}
  if(state.endingHost||state.endingLoading){resumeOriginalEnding();return;}
  state.paused=false;clock.reset();resetHighFrameratePresentation();find('#runtime-overlay').hidden=true;
  if(state.audio?.state==='suspended')state.audio.resume().catch(error);
  if(state.mainQuit){state.mainQuit=false;unlockMenuAudio().then(()=>startTitleSource());}
  status('动画继续。');
}
function pause(){
  if(state.normalPostrun){if(state.paused)resumeNormalPostrun();else pauseNormalPostrun();return;}
  if(state.endingHost||state.endingLoading){if(state.paused)resumeTitlePause();else pauseOriginalEnding();return;}
  if(state.worldMode){if(state.paused)resumeGame();else openPauseMenu();return;}
  if(!state.active)return;
  state.paused=!state.paused;clock.reset();resetHighFrameratePresentation();find('#pause').textContent=state.paused?'继续动画':'暂停动画';
  if(state.gameSounds){const operation=state.paused?state.audio.suspend():state.audio.resume();operation.catch(error);}
  if(state.paused){
    if(!state.worldMode){titlePauseOverlay();if(state.audio?.state==='running')state.audio.suspend().catch(error);}
  }else{
    find('#runtime-overlay').hidden=true;
    if(!state.worldMode&&state.audio?.state==='suspended')state.audio.resume().catch(error);
  }
  status(state.worldMode?(state.paused?'游戏已暂停；Z/ENTER 或 X/ESC 继续。':'正在运行关卡。'):
    (state.paused?'动画已暂停；Z/ENTER/X/ESC 继续。':'正在执行原版标题 ANM 脚本。'));
}

find('#start').addEventListener('click',async()=>{
  if(!returnToTitle())return;
  await unlockMenuAudio();
  find('#game-menu').hidden=true;find('#game').focus();
});
find('#pause').addEventListener('click',pause);
find('#resume-session').addEventListener('click',()=>{if(state.active&&state.paused){pause();find('#game').focus();}});
find('#restart-stage').addEventListener('click',()=>{
  if(state.handoff){status('下一关正在预加载；请等待或切换 variant/重置');return;}
  checkWorld(true,true,true);
});
find('#reset').addEventListener('click',()=>{returnToTitle();});
find('#variant').addEventListener('change',event=>{savePreference('variant',event.target.value);menu.optionsInitialized=false;loadVariant(event.target.value);});
find('#fullscreen').addEventListener('click',()=>find('.screen').requestFullscreen().catch(error));
find('#volume').addEventListener('input',event=>{
  menu.volume=Number(event.target.value);
  if(state.gain)state.gain.gain.value=menu.volume/100;
  savePreference('volume',menu.volume);
  if(menu.page==='options')buildOptions();
});
find('#debug-infinite-respawn').addEventListener('change',event=>{
  setDebugInfiniteRespawn(event.target.checked);
  find('#game').focus();
});
syncDebugRespawnControl();
find('#debug-stage-start').addEventListener('click',()=>{
  startDebugThirdStage(find('#debug-stage-select').value).catch(problem=>status(`调试跳关失败：${problem.message??String(problem)}`));
});
find('#debug-extra-start').addEventListener('click',()=>{
  startDebugThirdStage('EX').catch(problem=>status(`EX 启动失败：${problem.message??String(problem)}`));
});
installPracticeMenu();installThpracReplayControls();
syncDebugStageJumpControls();
find('#previous').addEventListener('click',()=>select(-1));find('#next').addEventListener('click',()=>select(1));
find('#ecl').addEventListener('click',async()=>{
  const ticket=state.loading;
  const module=state.module;
  find('#ecl').disabled=true;
  try{
    const response=await checkpointFetch(safeAssetUrl(state.base,`runtime/${state.stageSelection.stage.ecl}`),{signal:state.controller.signal});
    if(!response.ok)throw new Error('ECL 读取失败');
    const bytes=new Uint8Array(await response.arrayBuffer());
    if(ticket!==state.loading)return;
    module._th128_ecl_reset();
    const pointer=module._malloc(bytes.length);module.HEAPU8.set(bytes,pointer);
    const loaded=module._th128_load_ecl(0,pointer,bytes.length);module._free(pointer);
    if(!loaded||!module._th128_ecl_start(0,state.stageSelection.difficulty))throw new Error(module.UTF8ToString(module._th128_ecl_error()));
    let steps=0;
    while(steps<600){steps++;if(!module._th128_ecl_tick())break;}
    const reason=module.UTF8ToString(module._th128_ecl_error());
    find('#ecl-result').textContent=`原版 ${state.stageSelection.stage.ecl} · 难度 ${state.stageSelection.difficulty} · 尝试 ${steps} 个控制 tick\n${reason||'尚未遇到诊断停止'}\n\n这是明确的未实现边界，不是首关已可玩，也不证明这些控制 tick 与原版逐帧一致。`;
  }catch(problem){if(ticket===state.loading&&problem.name!=='AbortError')find('#ecl-result').textContent=problem.message;}
  finally{if(ticket===state.loading)find('#ecl').disabled=false;}
});
const originalStageFontLoads=new Map();
async function ensureOriginalStageTextFont(variant){
  if(!['zh','ja'].includes(variant))throw new Error('Unsupported original text font variant');
  if(!originalStageFontLoads.has(variant))originalStageFontLoads.set(variant,(async()=>{
    const source=variant==='zh'?'local("SimHei"), local("黑体")':'local("MS Gothic"), local("ＭＳ ゴシック")';
    const face=new FontFace(variant==='zh'?'TH128OriginalHeiti':'TH128OriginalGothic',source,{style:'normal',weight:'400'});
    await face.load();document.fonts.add(face);return face.family;
  })());
  return originalStageFontLoads.get(variant);
}
async function prepareOriginalStageTextRasters(base,variant,signal){
  if(variant!=='zh')return null;
  const response=await checkpointFetch(safeAssetUrl(base,'animations/original-stage-text-full42.json'),{signal});
  if(!response.ok)throw new Error('原版对话/符卡字形资源缺失');
  const metadata=await response.json();
  return loadOriginalStageTextBank(metadata,async(file,expected)=>{
    const loaded=await checkpointFetch(safeAssetUrl(base,file),{signal});
    if(!loaded.ok)throw new Error('原版文字位图未准备');
    const bytes=await loaded.arrayBuffer(),digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
    if(digest!==expected)throw new Error('原版文字位图完整校验失败');
    return createImageBitmap(new Blob([bytes],{type:'image/png'}),{premultiplyAlpha:'none',colorSpaceConversion:'none'});
  },{signal});
}
function disposeOriginalStageTextRasters(bank){
  if(bank)for(const page of bank.pages){page.image.close();page.outlineImage.close();}
}
function originalStageTextFamily(variant){
  if(!originalStageFontLoads.has(variant))throw new Error('Original stage text font is not ready');
  return variant==='zh'?'"TH128OriginalHeiti"':'"TH128OriginalGothic"';
}
function enemyAnimationBankLimit(selection,names){
  const expected=['enemy.anm','enemy_ll.anm','boss04.anm','enemy_bf.anm'];
  if(selection?.debugExtra===true){
    if(selection.stage?.id!==16||selection.difficulty!==4||selection.stage.ecl!=='st_ex.ecl'
      ||names.length!==4||names.some((name,index)=>name!==expected[index]))
      throw new Error('EX requires the original four ANM banks in authored order');
    return 4;
  }
  if(!Array.isArray(names)||names.length>3)throw new Error('Ordinary stages require original ANM bank1..3');
  return 3;
}
function bindDirectTerminalOwner(module,selection,pointer,size,progress,context){
  const restoreFinite=!!(selection?.debugJump||selection?.thprac)&&state.debugInfiniteRespawnRequested!==true;
  if(restoreFinite&&module._th128_world_set_debug_infinite_respawn(1)!==1)
    throw new Error(module.UTF8ToString(module._th128_world_error())||'direct terminal debug configuration failed');
  try{
    const bound=module._th128_world_enable_stage_transition(pointer,size,selection.stage.id,progress,
      context.gate74,context.message78,context.sessionPresent,context.sessionField74);
    if(bound!==1)throw new Error(module.UTF8ToString(module._th128_world_error())||'direct terminal original owner binding failed');
    return bound;
  }finally{
    if(restoreFinite&&module._th128_world_set_debug_infinite_respawn(0)!==1)
      throw new Error(module.UTF8ToString(module._th128_world_error())||'direct terminal finite mode restoration failed');
  }
}
function verifyFreshExtraOwner(snapshot,selection){
  if(selection?.debugExtra!==true)return true;
  const flow=snapshot?.flow,owner=flow?.playerShotGroupOwner;
  if(snapshot?.frame!==0||flow?.currentId!==16||flow.difficulty!==4||flow.shotDifficulty!==4
    ||snapshot.cumulativeFrozenArea!==0||snapshot.debug?.runTainted!==true
    ||!owner?.loaded||!owner.configured||!owner.nativeDefault||owner.group!==0||owner.selector!==0
    ||owner.failed!==false||owner.retired!==false||!Number.isInteger(owner.activeHandles)||owner.activeHandles<=0)
    throw new Error('EX fresh stage16/rank4/area0/native SHT group0 owner readback rejected');
  return true;
}
async function checkWorld(emission=false,updates=false,shots=false,selection=state.stageSelection){
  assertStageStartAllowed(selection?.stage?.id,selection?.difficulty,{debugExtra:selection?.debugExtra===true});
  lastCheckWorldError='';
  const flowReady=flowCapabilitiesReady();
  if(state.autoFlow&&!flowReady)state.autoFlow=false;
  const ticket=state.loading,base=state.base;let module=state.module;
  const signal=state.controller.signal,renderer=state.renderer,stage=selection?.stage;
  if(selection?.progress!==undefined&&!Number.isInteger(selection.progress))throw new Error('Invalid stage progress');
  const progress=Number.isInteger(selection?.progress)?selection.progress:0;
  const rawCarryToken=selection?.carryToken??0;
  if(rawCarryToken!==0&&(!Number.isInteger(rawCarryToken)||rawCarryToken<1))throw new Error('Invalid stage carry token');
  const carryToken=rawCarryToken,thprac=selection?.thprac??null;
  if(thprac&&carryToken)throw new Error('TH128 practice requires a fresh owner without stage carry');
  const presentation=thprac?thprac128Presentation(thprac):{skipIntro:false,bossMusic:false};
  let transitionEnabled=false,routeChoiceEnabled=false;
  const action=emission?++state.action:state.action;
  const stale=()=>ticket!==state.loading||(emission&&action!==state.action);
  let mutated=false,observedExtra=null,extraRun=null;
  if(selection?.ownedExtraProfileSHA256){
    observedExtra=await observeOriginalExtraQualification(module,signal);
    if(stale()||!observedExtra.qualification?.eligible||observedExtra.qualification.record.sha256!==selection.ownedExtraProfileSHA256)
      throw new Error('EX owned six-route profile changed before original World start');
  }
  let pendingAudio=null,preparedEnding=null,preparedTextRasters=null;
  const resultTarget=shots?'#shots-result':updates?'#segment-result':emission?'#emission-result':'#world-result';
  find('#world').disabled=true;
  find('#emission').disabled=true;
  find('#segment').disabled=true;
  find('#shots').disabled=true;
  find('#play-stage').disabled=true;
  const byteCache=new Map(),jsonCache=new Map();
  const bytesAt=async relative=>{
    if(!byteCache.has(relative))byteCache.set(relative,(async()=>{
      const response=await checkpointFetch(safeAssetUrl(base,relative),{signal});
      if(!response.ok)throw new Error(`世界资源读取失败 ${relative}`);
      const bytes=new Uint8Array(await response.arrayBuffer());
      if(!bytes.length)throw new Error(`世界资源为空 ${relative}`);
      return bytes;
    })());
    return byteCache.get(relative);
  };
  const jsonAt=async relative=>{
    if(!jsonCache.has(relative))jsonCache.set(relative,readJson(safeAssetUrl(base,relative),signal));
    return jsonCache.get(relative);
  };
  const memory=bytes=>{const pointer=module._malloc(bytes.length);module.HEAPU8.set(bytes,pointer);return pointer;};
  try{
    if(!stage)throw new Error('关卡选择不可用');
    const carryRoute=incomingCarryRoute(stage.id,carryToken,flowCapabilities());
    if(carryToken){
      if(carryToken!==state.pendingCarryToken)throw new Error('stage carry token is stale or does not belong to this handoff');
      if(!flowReady)throw new Error('core flow capabilities are not ready for stage carry; the token was retained');
      if(!carryRoute)throw new Error(`core stage carry scalar restore for stage ${stage.id} is neither an ordinary nor terminal-receive target; the token was retained`);
      if(typeof module._th128_world_enable_stage_transition!=='function'||typeof module._th128_world_restore_stage_carry_random!=='function'
        ||typeof module._th128_world_restore_stage_carry!=='function'||typeof module._th128_world_bind_stage_carry_target!=='function'
        ||typeof module._th128_world_acknowledge_stage_transition!=='function'||typeof module._th128_world_discard_stage_carry!=='function')
        throw new Error('core stage carry/transition runtime exports are incomplete; the token was retained');
    }else{
      discardPendingCarry();
      state.handoffFailed=false;state.handoff=false;++state.handoffGeneration;
    }
    if(shots){await ensureOriginalStageTextFont(find('#variant').value);preparedTextRasters=await prepareOriginalStageTextRasters(base,find('#variant').value,signal);}
    if(shots){
      const tracks=presentation.bossMusic?[stage.music[1],...stage.music.filter(name=>name!==stage.music[1])]:stage.music;
      pendingAudio=await prepareGameSounds(base,signal,tracks,selection);
    }
    if(shots)preparedEnding=await prepareOriginalEndingResources(stage,base,signal);
    const parsedEcl=await jsonAt(`ecl/${stage.ecl.slice(0,-4)}.json`);
    const includes=parsedEcl?.includes;
    if(parsedEcl?.format!=='SCPT/1'||!Array.isArray(includes?.ECLI)||!Array.isArray(includes?.ANIM))
      throw new Error(`ECL 包含信息不完整：${stage.ecl}`);
    const eclNames=[stage.ecl,...includes.ECLI];
    const enemyBankLimit=enemyAnimationBankLimit(selection,includes.ANIM);
    if(new Set(eclNames).size!==eclNames.length||!eclNames.every(name=>validStageName(name,'.ecl'))
      ||includes.ANIM.length<3||!includes.ANIM.every(name=>validStageName(name,'.anm')))
      throw new Error(`ECL include 顺序或名称无效：${stage.ecl}`);
    const resources=await Promise.all(eclNames.map(async name=>[name,await bytesAt(`runtime/${name}`)]));
    let bulletBytes,profileBytes,enemyBanks,bulletMetadata,shotBytes,playerBytes,playerMetadata;
    let textBytes,textMetadata,backgroundStd,backgroundAnimation,backgroundMetadata,backgroundData,backgroundFactory;
    let messageBytes,frontBytes,frontMetadata,fontBytes,cardBytes,cardMetadata;
    let type4Bytes,type10Bytes,type18Bytes,type20Bytes,type24Bytes,type25Bytes,type13Bytes,type6Bytes,type16Bytes,type30Bytes,type21Bytes,spellProfiles,iceBytes,iceMetadata,iceMasks;
    if(emission){
      [bulletBytes,profileBytes,enemyBanks]=await Promise.all([bytesAt('runtime/bullet.anm'),checkpointFetch('/assets/runtime/bullet-profile-8.bin',{signal}).then(async response=>{
        if(!response.ok)throw new Error('原版子弹表尚未准备');return new Uint8Array(await response.arrayBuffer());
      }),Promise.all(includes.ANIM.map(async(name,index)=>{
        if(index>=enemyBankLimit)throw new Error(`ANM include 超出当前关卡原版 bank 上限：${name}`);
        const [bytes,metadata]=await Promise.all([bytesAt(`runtime/${name}`),jsonAt(`animations/${name.slice(0,-4)}.json`)]);
        return {bank:index+1,bytes,metadata};
      }))]);
      bulletMetadata=await jsonAt('animations/bullet.json');
      if(updates){
        [shotBytes,playerBytes]=await Promise.all([bytesAt('runtime/pl00a.sht'),bytesAt('runtime/pl00.anm')]);
        playerMetadata=await jsonAt('animations/pl00.json');
      }
      if(module._th128_world_enable_geometry526||shots)
        [textBytes,textMetadata]=await Promise.all([bytesAt('runtime/text.anm'),jsonAt('animations/text.json')]);
      if(shots){
        const backgroundStem=stage.background.slice(0,-4),backgroundAnm=`${backgroundStem}.anm`;
        if(!state.manifest.animations?.some(entry=>entry.name===backgroundAnm))throw new Error(`背景 ANM 关联缺失：${backgroundAnm}`);
        [backgroundStd,backgroundAnimation,backgroundMetadata]=await Promise.all([bytesAt(`runtime/${stage.background}`),bytesAt(`runtime/${backgroundAnm}`),jsonAt(`animations/${backgroundStem}.json`)]);
        backgroundData=parseStd(backgroundStd);
        backgroundFactory=await runtimeFactory(signal);
        if(module._th128_world_enable_messages)messageBytes=await bytesAt(`runtime/${stage.message}`);
        const nativeProfiles=await jsonAt('animations/bullet-native-profiles.json');
        if(nativeProfiles.schema!=='th128-original-bullet-profiles/1'||nativeProfiles.sourceExeSha256!=='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
          ||nativeProfiles.profiles?.length!==1)throw new Error('Original native bullet profile provenance unavailable');
        const profile4=nativeProfiles.profiles[0];
        if(profile4.type!==4||typeof profile4.hex!=='string'||!/^[0-9a-f]{576}$/.test(profile4.hex))throw new Error('Invalid original type4 profile');
        type4Bytes=Uint8Array.from(profile4.hex.match(/../g),value=>parseInt(value,16));
        const digest4=[...new Uint8Array(await crypto.subtle.digest('SHA-256',type4Bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
        if(digest4!==profile4.sha256)throw new Error('Original type4 profile checksum mismatch');
        const bossProfiles=await jsonAt('animations/bullet-boss-profiles.json');
        const profile10=bossProfiles.profiles?.find(value=>value.type===10);
        if(bossProfiles.schema!=='th128-original-boss-bullet-profiles/1'||bossProfiles.sourceExeSha256!==nativeProfiles.sourceExeSha256
          ||!profile10||typeof profile10.hex!=='string'||!/^[0-9a-f]{576}$/.test(profile10.hex))throw new Error('原版 type10 子弹表尚未准备');
        type10Bytes=Uint8Array.from(profile10.hex.match(/../g),value=>parseInt(value,16));
        const digest10=[...new Uint8Array(await crypto.subtle.digest('SHA-256',type10Bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
        if(digest10!==profile10.sha256)throw new Error('Original type10 profile checksum mismatch');
        [type18Bytes,type20Bytes]=await Promise.all([18,20].map(async type=>{
          const response=await checkpointFetch(`/assets/runtime/bullet-profile-${type}.bin`,{signal});
          if(!response.ok)throw new Error(`原版 type${type} 子弹表尚未准备`);
          return new Uint8Array(await response.arrayBuffer());
        }));
        const spellProfilesJson=await jsonAt('animations/bullet-spell-profiles.json');
        if(spellProfilesJson.schema!=='th128-original-spell-bullet-profiles/1'||spellProfilesJson.sourceExeSha256!==nativeProfiles.sourceExeSha256||spellProfilesJson.profiles?.length!==2)throw new Error('原版符卡子弹表尚未准备');
        spellProfiles=[];
        for(const profile of spellProfilesJson.profiles){
          if(![23,29].includes(profile.type)||typeof profile.hex!=='string'||!/^[0-9a-f]{576}$/.test(profile.hex))throw new Error('无效符卡子弹表');
          const bytes=Uint8Array.from(profile.hex.match(/../g),value=>parseInt(value,16));
          const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
          if(digest!==profile.sha256)throw new Error('符卡子弹表校验失败');
          spellProfiles.push({type:profile.type,bytes});
        }
        if(supportsBulletType25()){
          const profile=await jsonAt('animations/bullet-type25-profile.json');
          if(profile?.schema!=='th128-original-typed-bullet-profile/1'||profile.type!==25
            ||profile.sourceExeSha256!==type25SourceExeSha256||profile.sourceAddress!=='0x004b40f0'
            ||profile.bytes!==type25ProfileBytes||profile.sha256!==type25ProfileSha256
            ||typeof profile.hex!=='string'||!/^[0-9a-f]{576}$/.test(profile.hex)
            ||profile.script!==102||profile.secondaryScript!==103||profile.hitboxSize!==6
            ||profile.auxiliarySize!==22||profile.appearance!==2||profile.selector!==8
            ||profile.freezePolicy!==1||profile.cancellationScript!==371||profile.identitySpriteRemap!==true
            ||!Array.isArray(profile.supportedTypes)||profile.supportedTypes.length!==1||profile.supportedTypes[0]!==25)
            throw new Error('Invalid original type25 bullet profile provenance');
          type25Bytes=Uint8Array.from(profile.hex.match(/../g),value=>parseInt(value,16));
          if(type25Bytes.length!==type25ProfileBytes)throw new Error('Invalid original type25 bullet profile byte length');
          const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',type25Bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
          if(digest!==type25ProfileSha256)throw new Error('Original type25 bullet profile checksum mismatch');
        }
        if(supportsBulletType13()){
          const profile=await jsonAt('animations/bullet-type13-profile.json');
          if(profile?.schema!=='th128-original-typed-bullet-profile/1'||profile.type!==13
            ||profile.sourceExeSha256!==type13SourceExeSha256||profile.sourceAddress!==type13SourceAddress
            ||profile.bytes!==type13ProfileBytes||profile.sha256!==type13ProfileSha256
            ||typeof profile.hex!=='string'||!/^[0-9a-f]{576}$/.test(profile.hex)
            ||profile.script!==48||profile.secondaryScript!==0||profile.hitboxSize!==2.4000000953674316
            ||profile.auxiliarySize!==16||profile.appearance!==4||profile.selector!==6
            ||profile.freezePolicy!==0||profile.identitySpriteRemap!==false
            ||!Array.isArray(profile.perColor)||profile.perColor.length!==type13PerColor.length
            ||profile.perColor.some((value,index)=>value!==type13PerColor[index])
            ||!Array.isArray(profile.supportedTypes)||profile.supportedTypes.length!==1||profile.supportedTypes[0]!==13)
            throw new Error('Invalid original type13 bullet profile provenance');
          type13Bytes=Uint8Array.from(profile.hex.match(/../g),value=>parseInt(value,16));
          if(type13Bytes.length!==type13ProfileBytes)throw new Error('Invalid original type13 bullet profile byte length');
          const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',type13Bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
          if(digest!==type13ProfileSha256)throw new Error('Original type13 bullet profile checksum mismatch');
        }
        if(supportsBulletType6()){
          const profile=await jsonAt('animations/bullet-type6-profile.json');
          if(profile?.schema!=='th128-original-typed-bullet-profile/1'||profile.type!==6
            ||profile.sourceExeSha256!==type6SourceExeSha256||profile.sourceAddress!==type6SourceAddress
            ||profile.bytes!==type6ProfileBytes||profile.sha256!==type6ProfileSha256
            ||typeof profile.hex!=='string'||!/^[0-9a-f]{576}$/.test(profile.hex)
            ||profile.script!==41||profile.secondaryScript!==0||profile.hitboxSize!==4
            ||profile.auxiliarySize!==16||profile.appearance!==3||profile.selector!==6
            ||profile.freezePolicy!==0||profile.opaque114!==30||profile.identitySpriteRemap!==false
            ||profile.callback!==type6Callback
            ||!Array.isArray(profile.colorRemaps)||profile.colorRemaps.length!==type6ColorRemaps.length
            ||profile.colorRemaps.some((row,color)=>!Array.isArray(row)||row.length!==4
              ||row.some((value,sprite)=>value!==type6ColorRemaps[color][sprite]))
            ||!Array.isArray(profile.cancellationScripts)||profile.cancellationScripts.length!==type6CancellationScripts.length
            ||profile.cancellationScripts.some((value,color)=>value!==type6CancellationScripts[color])
            ||!Array.isArray(profile.supportedTypes)||profile.supportedTypes.length!==1||profile.supportedTypes[0]!==6
            ||Object.keys(profile).some(key=>!type6ProfileFields.has(key)))
            throw new Error('Invalid original type6 bullet profile provenance');
          type6Bytes=Uint8Array.from(profile.hex.match(/../g),value=>parseInt(value,16));
          if(type6Bytes.length!==type6ProfileBytes)throw new Error('Invalid original type6 bullet profile byte length');
          const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',type6Bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
          if(digest!==type6ProfileSha256)throw new Error('Original type6 bullet profile checksum mismatch');
        }
        if(supportsBulletType16()){
          const profile=await jsonAt('animations/bullet-type16-profile.json');
          if(profile?.schema!=='th128-original-typed-bullet-profile/1'||profile.type!==16
            ||profile.sourceExeSha256!==type16SourceExeSha256||profile.sourceAddress!==type16SourceAddress
            ||profile.bytes!==type16ProfileBytes||profile.sha256!==type16ProfileSha256
            ||typeof profile.hex!=='string'||!/^[0-9a-f]{576}$/.test(profile.hex)
            ||profile.script!==51||profile.secondaryScript!==0||profile.hitboxSize!==4
            ||profile.auxiliarySize!==16||profile.appearance!==3||profile.selector!==6
            ||profile.freezePolicy!==0||profile.opaque114!==30||profile.identitySpriteRemap!==false
            ||profile.callback!==type16Callback
            ||!Array.isArray(profile.colorRemaps)||profile.colorRemaps.length!==type16ColorRemaps.length
            ||profile.colorRemaps.some((row,color)=>!Array.isArray(row)||row.length!==4
              ||row.some((value,sprite)=>value!==type16ColorRemaps[color][sprite]))
            ||!Array.isArray(profile.cancellationScripts)||profile.cancellationScripts.length!==type16CancellationScripts.length
            ||profile.cancellationScripts.some((value,color)=>value!==type16CancellationScripts[color])
            ||!Array.isArray(profile.supportedTypes)||profile.supportedTypes.length!==1||profile.supportedTypes[0]!==16
            ||Object.keys(profile).some(key=>!type6ProfileFields.has(key)))
            throw new Error('Invalid original type16 bullet profile provenance');
          type16Bytes=Uint8Array.from(profile.hex.match(/../g),value=>parseInt(value,16));
          if(type16Bytes.length!==type16ProfileBytes)throw new Error('Invalid original type16 bullet profile byte length');
          const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',type16Bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
          if(digest!==type16ProfileSha256)throw new Error('Original type16 bullet profile checksum mismatch');
        }
        if(supportsBulletType24()){
          type24Bytes=await type24ProfileBytesFromRecord(await jsonAt('animations/bullet-type24-profile.json'));
        }
        if(supportsBulletType30()){
          type30Bytes=await type30ProfileBytesFromRecord(await jsonAt('animations/bullet-type30-profile.json'));
        }
        if(supportsBulletType21()){
          const profile=await jsonAt('animations/bullet-type21-profile.json');
          if(profile?.schema!=='th128-original-typed-bullet-profile/1'||profile.type!==21
            ||profile.sourceExeSha256!==type21SourceExeSha256||profile.sourceAddress!==type21SourceAddress
            ||profile.bytes!==type21ProfileBytes||profile.sha256!==type21ProfileSha256
            ||typeof profile.hex!=='string'||!/^[0-9a-f]{576}$/.test(profile.hex)
            ||profile.script!==75||profile.secondaryScript!==0||profile.hitboxSize!==6
            ||profile.auxiliarySize!==22||profile.appearance!==2||profile.selector!==6
            ||profile.freezePolicy!==0||profile.opaque114!==60||profile.identitySpriteRemap!==false
            ||profile.callback!==type21Callback
            ||!Array.isArray(profile.colorRemaps)||profile.colorRemaps.length!==type21ColorRemaps.length
            ||profile.colorRemaps.some((row,color)=>!Array.isArray(row)||row.length!==4
              ||row.some((value,sprite)=>value!==type21ColorRemaps[color][sprite]))
            ||!Array.isArray(profile.cancellationScripts)||profile.cancellationScripts.length!==type21CancellationScripts.length
            ||profile.cancellationScripts.some((value,color)=>value!==type21CancellationScripts[color])
            ||!Array.isArray(profile.supportedTypes)||profile.supportedTypes.length!==1||profile.supportedTypes[0]!==21
            ||Object.keys(profile).some(key=>!type21ProfileFields.has(key)))
            throw new Error('Invalid original type21 bullet profile provenance');
          type21Bytes=Uint8Array.from(profile.hex.match(/../g),value=>parseInt(value,16));
          if(type21Bytes.length!==type21ProfileBytes)throw new Error('Invalid original type21 bullet profile byte length');
          const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',type21Bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
          if(digest!==type21ProfileSha256)throw new Error('Original type21 bullet profile checksum mismatch');
        }
        [frontBytes,frontMetadata]=await Promise.all([bytesAt('runtime/front.anm'),jsonAt('animations/front.json')]);
        fontBytes=await bytesAt('runtime/ascii.anm');
        if(module._th128_world_enable_spells)[cardBytes,cardMetadata]=await Promise.all([bytesAt('runtime/card.anm'),jsonAt('animations/card.json')]);
        const maskProfile=await jsonAt('animations/ice-masks.json');
        if(maskProfile.schema!=='th128-original-ice-masks/1'||maskProfile.sourceExeSha256!=='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
          ||maskProfile.profiles?.length!==5)throw new Error('Original ice masks are unavailable');
        const parts=[];let total=0;
        for(let index=0;index<5;++index){
          const profile=maskProfile.profiles[index],rows=[21,25,33,49,65][index],words=index<3?1:2;
          if(profile.selector!==index+1||profile.rows!==rows||profile.words!==words
            ||typeof profile.hex!=='string'||!new RegExp(`^[0-9a-f]{${rows*words*8}}$`).test(profile.hex))throw new Error('Invalid original ice bitmap');
          const bytes=Uint8Array.from(profile.hex.match(/../g),value=>parseInt(value,16));
          const digest=[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
          if(digest!==profile.sha256)throw new Error('Original ice mask checksum mismatch');parts.push(bytes);total+=bytes.length;
        }
        iceBytes=await bytesAt('runtime/ice.anm');
        iceMetadata=await jsonAt('animations/ice.json');
        iceMasks=new Uint8Array(total);let offset=0;for(const part of parts){iceMasks.set(part,offset);offset+=part.length;}
      }
    }
    if(stale())return;
    if(shots&&!carryToken)module=await prepareFreshWorldModule(selection,bytesAt,signal);
    if(stale())return;
    mutated=true;
    if(emission){state.active=false;state.paused=true;state.worldMode=false;state.shotMode=false;find('#emit-shot').disabled=true;clock.reset();resetHighFrameratePresentation();stopAudio({keepPracticeMusic:pendingAudio?.retained?pendingAudio.music:null});syncDebugRespawnUI(null);}
    find('#runtime-overlay').hidden=true;find('#error').hidden=true;state.pressed.clear();
    state.worldSnapshot=null;
    state.stageIntroLoaded=false;state.stageIntroStage=0;state.stageIntroCaptionKey='';
    cancelOriginalEnding();
    state.module=module;flowCapabilitiesReady();
    if(observedExtra){
      const {OriginalExtraRunController}=await import('./runtime/native-extra-run.mjs');
      if(stale())return;
      extraRun=new OriginalExtraRunController({module,isCurrent:()=>!stale()&&!signal.aborted&&module===state.module});
      await extraRun.prepare({...observedExtra,seed:selection.seed});
      if(stale())return;
    }else module._th128_world_reset();
    syncDebugRespawnUI(null);
    if(emission){
      await renderer.loadBank(0,bulletMetadata,base,{signal});
      for(const bank of enemyBanks)await renderer.loadBank(bank.bank,bank.metadata,base,{signal});
      if(updates)await renderer.loadBank(7,playerMetadata,base,{signal});
      if(module._th128_world_enable_geometry526)await renderer.loadBank(6,textMetadata,base,{signal});
      if(shots){
        await renderer.loadBank(8,backgroundMetadata,base,{signal});
        await renderer.loadBank(5,frontMetadata,base,{signal});
        if(module._th128_world_enable_spells)await renderer.loadBank(12,cardMetadata,base,{signal});
        await renderer.loadBank(36,iceMetadata,base,{signal});
      }
      if(stale())return;
    }
    for(const [name,bytes] of resources){
      const encoded=new TextEncoder().encode(name),namePointer=memory(encoded),dataPointer=memory(bytes);
      const loaded=module._th128_world_load(namePointer,encoded.length,dataPointer,bytes.length);
      module._free(namePointer);module._free(dataPointer);
      if(!loaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
    }
    if(thprac)prepareThprac128Native(module,thprac);
    const encoded=new TextEncoder().encode(stage.ecl),pointer=memory(encoded);
    const started=module._th128_world_start(pointer,encoded.length,selection.difficulty,selection.seed);module._free(pointer);
    if(!started)throw new Error(module.UTF8ToString(module._th128_world_error()));
    if(state.replayPlayback){if(module._th128_world_set_debug_infinite_respawn(0)!==1)throw new Error('Replay requires original finite gameplay mode');}
    else if(extraRun){if(module._th128_world_set_debug_infinite_respawn(1)!==1)throw new Error(module.UTF8ToString(module._th128_world_error()));}
    else applyDebugInfiniteRespawn(module,selection);
    if(carryToken){
      const target=new TextEncoder().encode(stage.ecl),targetPointer=memory(target);
      const bound=module._th128_world_bind_stage_carry_target(carryToken,targetPointer,target.length,selection.difficulty);
      module._free(targetPointer);
      if(!bound)throw new Error(module.UTF8ToString(module._th128_world_error())||'stage carry target binding failed; the token was retained');
      if(!module._th128_world_restore_stage_carry_random(carryToken,stage.id,progress,selection.difficulty))
        throw new Error(module.UTF8ToString(module._th128_world_error())||'stage carry random restore failed; the token was retained');
    }
    if(supportsStageIntro()&&!presentation.skipIntro){
      const record=await installStageIntro(stage,module,renderer,jsonAt,bytesAt,memory,base,signal);
      if(stale())return;
      state.stageIntroLoaded=true;state.stageIntroStage=record.id;state.stageIntroCaptionKey='';
    }
    if(emission){
      for(const bank of enemyBanks){
        const enemyPointer=memory(bank.bytes);
        const enemyLoaded=module._th128_world_load_enemy_anm(bank.bank,enemyPointer,bank.bytes.length);
        module._free(enemyPointer);
        if(!enemyLoaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
      }
      if(module._th128_world_enable_geometry526){
        const textPointer=memory(textBytes),loaded=module._th128_world_enable_geometry526(textPointer,textBytes.length);module._free(textPointer);
        if(!loaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
      }
      if(shots){
        const stageModule=await backgroundFactory({print:()=>{},printErr:message=>console.warn(message)});
        if(stale())return;
        const pointer=stageModule._malloc(backgroundAnimation.length);stageModule.HEAPU8.set(backgroundAnimation,pointer);
        const loaded=stageModule._th128_load_anm(8,pointer,backgroundAnimation.length);stageModule._free(pointer);
        if(!loaded)throw new Error(stageModule.UTF8ToString(stageModule._th128_error()));
        state.background=new StageBackground(backgroundData,stageModule);
      }else state.background=null;
      const profilePointer=memory(profileBytes),animationPointer=memory(bulletBytes);
      const enabled=module._th128_world_enable_bullets(profilePointer,profileBytes.length,animationPointer,bulletBytes.length,0,400);
      module._free(profilePointer);module._free(animationPointer);
      if(!enabled)throw new Error(module.UTF8ToString(module._th128_world_error()));
      if(updates){
        const shotPointer=memory(shotBytes);
        const updateReady=module._th128_world_enable_bullet_updates(shotPointer,shotBytes.length);module._free(shotPointer);
        if(!updateReady)throw new Error(module.UTF8ToString(module._th128_world_error()));
        if(!module._th128_world_enable_graze(0,thprac?.params.ice_power??10000,0x1234))throw new Error(module.UTF8ToString(module._th128_world_error()));
        const playerPointer=memory(playerBytes);
        const playerReady=module._th128_world_enable_player_context(playerPointer,playerBytes.length,1,extraRun?2:0,extraRun?65535:120,0,0,0,0,0,thprac?.params.perfect_freeze??0);
        module._free(playerPointer);
        if(!playerReady)throw new Error(module.UTF8ToString(module._th128_world_error()));
        if(!module._th128_world_enable_player_motion(state.replayPlayback?.stageInitial().horizontal??0,state.replayPlayback?.stageInitial().vertical??51200))throw new Error(module.UTF8ToString(module._th128_world_error()));
        if(shots){
          if(module._th128_world_enable_messages){
            const pointers=[];
            try{
              for(const bytes of [messageBytes,textBytes,playerBytes,enemyBanks[2].bytes])pointers.push(memory(bytes));
              const ready=module._th128_world_enable_messages(pointers[0],messageBytes.length,pointers[1],textBytes.length,pointers[2],playerBytes.length,pointers[3],enemyBanks[2].bytes.length);
              const messageError=module.UTF8ToString(module._th128_world_error());
              if(!ready||messageError)throw new Error(messageError||'original message resources rejected');
            }finally{for(const pointer of pointers)module._free(pointer);}
            if(typeof module._th128_world_enable_message_front!=='function')throw new Error('current runtime is missing the original message front resource interface');
            const messageFrontPointer=memory(frontBytes);
            try{
              const frontReady=module._th128_world_enable_message_front(messageFrontPointer,frontBytes.length);
              const frontError=module.UTF8ToString(module._th128_world_error());
              if(!frontReady||frontError)throw new Error(frontError||'original message front resource rejected');
            }finally{module._free(messageFrontPointer);}
          }
          if(module._th128_world_enable_route_choice&&routeChoiceStages.has(stage.id)){
            const pointer=memory(frontBytes);
            const ready=module._th128_world_enable_route_choice(pointer,frontBytes.length,stage.id,routeChoiceMessage78,routeChoiceMessage80);
            module._free(pointer);
            if(!ready)throw new Error(module.UTF8ToString(module._th128_world_error()));
            routeChoiceEnabled=true;
          }
          const transitionEligible=flowReady&&stageTransitionStages.has(stage.id)&&(!routeChoiceStages.has(stage.id)||routeChoiceEnabled);
          if(module._th128_world_enable_stage_transition&&module._th128_world_enable_messages&&transitionEligible){
            const pointer=memory(frontBytes);
            const ready=module._th128_world_enable_stage_transition(pointer,frontBytes.length,stage.id,progress,
              stageTransitionContext.gate74,stageTransitionContext.message78,stageTransitionContext.sessionPresent,stageTransitionContext.sessionField74);
            module._free(pointer);
            if(ready)transitionEnabled=true;
            else if(carryToken)throw new Error(module.UTF8ToString(module._th128_world_error())||'stage transition host is unavailable; the carry token was retained');
            else state.autoFlow=false;
          }else if(!carryToken&&stageTransitionStages.has(stage.id))state.autoFlow=false;
          const type4Pointer=memory(type4Bytes),type4Loaded=module._th128_world_add_bullet_profile(4,type4Pointer,type4Bytes.length);module._free(type4Pointer);
          if(!type4Loaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
          for(const [type,bytes] of [[10,type10Bytes],[18,type18Bytes],[20,type20Bytes]]){
            const pointer=memory(bytes),loaded=module._th128_world_add_bullet_profile(type,pointer,bytes.length);module._free(pointer);
            if(!loaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
          }
          await loadType9ProfileForWorld({supports:supportsBulletType9,readProfile:async()=>preparedType9Profile(),module,memory});
          if(type25Bytes){
            const pointer=memory(type25Bytes),loaded=module._th128_world_add_bullet_profile(25,pointer,type25Bytes.length);module._free(pointer);
            if(!loaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
          }
          if(type13Bytes){
            const pointer=memory(type13Bytes),loaded=module._th128_world_add_bullet_profile(13,pointer,type13Bytes.length);module._free(pointer);
            if(!loaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
          }
          if(type6Bytes){
            const pointer=memory(type6Bytes),loaded=module._th128_world_add_bullet_profile(6,pointer,type6Bytes.length);module._free(pointer);
            if(!loaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
          }
          if(type16Bytes){
            const pointer=memory(type16Bytes),loaded=module._th128_world_add_bullet_profile(16,pointer,type16Bytes.length);module._free(pointer);
            if(!loaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
          }
          if(type24Bytes){
            const pointer=memory(type24Bytes),loaded=module._th128_world_add_bullet_profile(24,pointer,type24Bytes.length);module._free(pointer);
            if(!loaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
          }
          if(type30Bytes){
            const pointer=memory(type30Bytes),loaded=module._th128_world_add_bullet_profile(30,pointer,type30Bytes.length);module._free(pointer);
            if(!loaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
          }
          if(supportsBulletType32())
            await loadType32ProfileForWorld({supports:supportsBulletType32,readProfile:()=>readJson(type32ProfileUrl,signal),module,memory});
          state.type33ProfileSnapshot=await loadOriginalType33ForWorld({profile:originalType33Profile,module,memory,
            isCurrent:()=>!signal.aborted&&!stale()&&state.module===module});
          if(type21Bytes){
            const pointer=memory(type21Bytes),loaded=module._th128_world_add_bullet_profile(21,pointer,type21Bytes.length);module._free(pointer);
            if(!loaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
          }
          for(const profile of spellProfiles){
            const pointer=memory(profile.bytes),ready=module._th128_world_add_bullet_profile(profile.type,pointer,profile.bytes.length);module._free(pointer);
            if(!ready)throw new Error(module.UTF8ToString(module._th128_world_error()));
          }
          const shotData=memory(shotBytes),playerData=memory(playerBytes),damageFlags=memory(new Uint8Array(8000));
          const configured=module._th128_world_enable_player_shots(shotData,shotBytes.length,playerData,playerBytes.length,damageFlags,2000,thprac?.params.ice_area??state.replayPlayback?.stageInitial().frozenArea??0,selection.difficulty,0,0,0,0);
          module._free(shotData);module._free(playerData);module._free(damageFlags);
          if(!configured)throw new Error(module.UTF8ToString(module._th128_world_error()));
          if(!module._th128_world_enable_shot_hits(thprac?.params.motivation??state.replayPlayback?.stageInitial().motivation??20000,0,0,0,0)||!module._th128_world_enable_rapid_shot(-1))
            throw new Error(module.UTF8ToString(module._th128_world_error()));
          if(!module._th128_world_enable_player_death(-1))throw new Error(module.UTF8ToString(module._th128_world_error()));
          const frontPointer=memory(frontBytes),frontLoaded=module._th128_world_enable_perfect_hud(frontPointer,frontBytes.length);module._free(frontPointer);
          if(!frontLoaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
          if(typeof module._th128_world_pause_load_manual!=='function')throw new Error('原版暂停/死亡菜单内核尚未载入');
          const helpBytes=await bytesAt('runtime/help.anm'),helpMetadata=await jsonAt('animations/help.json');
          const helpPointer=memory(helpBytes);
          const helpLoaded=module._th128_world_pause_load_manual(helpPointer,helpBytes.length);module._free(helpPointer);
          if(!helpLoaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
          await renderer.loadBank(46,helpMetadata,base,{signal});state.manualPageLoaded=-1;
          const controlTargets=memory(new Uint8Array(32));
          const respawnReady=module._th128_world_enable_player_respawn(1,0,controlTargets,8);module._free(controlTargets);
          if(!respawnReady)throw new Error(module.UTF8ToString(module._th128_world_error()));
          const fontPointer=memory(fontBytes),fontLoaded=module._th128_world_load_score_font(fontPointer,fontBytes.length);module._free(fontPointer);
          if(!fontLoaded)throw new Error(module.UTF8ToString(module._th128_world_error()));
          if(module._th128_world_enable_spells){
            const inputs=[bulletBytes,enemyBanks[2].bytes,textBytes,fontBytes,cardBytes,playerBytes],pointers=inputs.map(memory),argumentsList=inputs.flatMap((bytes,index)=>[pointers[index],bytes.length]);
            const ready=module._th128_world_enable_spells(...argumentsList);for(const pointer of pointers)module._free(pointer);
            if(!ready)throw new Error(module.UTF8ToString(module._th128_world_error())||`Spell graphics initialization rejected for stage ${stage.id} at _th128_world_enable_spells (boss bank3 ${enemyBanks[2].bytes.length} bytes); native diagnostic empty`);
          }
          if(module._th128_world_enable_head_power&&!module._th128_world_enable_head_power())throw new Error(module.UTF8ToString(module._th128_world_error()));
          const icePointer=memory(iceBytes),maskPointer=memory(iceMasks);
          const zReady=module._th128_world_enable_z_fire?.(icePointer,iceBytes.length,maskPointer,iceMasks.length);
          module._free(icePointer);module._free(maskPointer);
          if(!zReady)throw new Error(module.UTF8ToString(module._th128_world_error())||'Z/ice runtime is unavailable; rebuild required');
          if(!module._th128_world_enable_focus_indicator?.())throw new Error(module.UTF8ToString(module._th128_world_error())||'Original focus marker runtime is unavailable');
          if(terminalStageIds.has(stage.id)||extraRun){
            if(!flowReady||typeof module._th128_world_enable_stage_transition!=='function')
              throw new Error('terminal requires the original scene owner interface');
            const terminalPointer=memory(frontBytes);
            try{
              if(carryToken){
                if(carryRoute!=='terminal')throw new Error('terminal resources require the same incoming carry route');
                const ready=module._th128_world_enable_stage_transition(terminalPointer,frontBytes.length,stage.id,progress,
                  stageTransitionContext.gate74,stageTransitionContext.message78,stageTransitionContext.sessionPresent,stageTransitionContext.sessionField74);
                if(!ready)throw new Error(module.UTF8ToString(module._th128_world_error())||'ordinary terminal resources rejected; the carry token was retained');
                transitionEnabled=true;
              }else bindDirectTerminalOwner(module,selection,terminalPointer,frontBytes.length,progress,stageTransitionContext);
            }finally{module._free(terminalPointer);}
          }
        }
      }
      if(shots){
        if(carryToken){
          if(!canRestoreCarryScalars(carryRoute,transitionEnabled)||typeof module._th128_world_restore_stage_carry!=='function')
            throw new Error('the selected carry route is not fully configured for stage carry scalars; the token was retained');
          if(!module._th128_world_restore_stage_carry(carryToken))
            throw new Error(module.UTF8ToString(module._th128_world_error())||'stage carry scalar restore failed; the token was retained');
          if(typeof module._th128_world_acknowledge_stage_transition!=='function')
            throw new Error('stage transition token ACK runtime is unavailable; the token was retained');
          if(!module._th128_world_acknowledge_stage_transition(carryToken))
            throw new Error(module.UTF8ToString(module._th128_world_error())||'stage transition token ACK failed; the token was retained');
          state.pendingCarryToken=0;
          delete selection.carryToken;
        }
        if(extraRun){
          const assets=[];
          for(const [bank,name]of [[0,'front'],[24,'title'],[25,'title_v'],[11,'ascii']])assets.push({bank,bytes:await bytesAt('runtime/'+name+'.anm')});
          if(stale())return;extraRun.bind(assets);
        }
        if(thprac){
          readThprac128Native(module,thprac);selection.thpracIdentity=await thpracReplayResourceIdentity(module,byteCache);
          if(stale())return;
          const expected=selection.thpracReplayIdentity;if(expected&&(expected.coreSHA256!==selection.thpracIdentity.coreSHA256||expected.resourceSHA256!==selection.thpracIdentity.resourceSHA256))throw new Error('Practice replay core/resource identity differs from loaded bytes');
        }
        else verifyFreshExtraOwner(JSON.parse(module.UTF8ToString(module._th128_world_state())),selection);
        state.shotMode=shots;find('#emit-shot').disabled=!shots;
        state.worldMode=true;state.active=true;state.paused=window.__TH128_DIAGNOSTIC_PAUSED__===true;clock.reset();resetHighFrameratePresentation();stopAudio({keepContext:pendingAudio?.retained===true,keepPracticeMusic:pendingAudio?.retained?pendingAudio.music:null});
        find('#game-menu').hidden=true;
        if(pendingAudio){state.audio=pendingAudio.audio;state.gain=pendingAudio.gain;state.gameSounds=pendingAudio.queue;
          state.stageMusic=pendingAudio.music;if(!state.stageMusic.source)state.stageMusic.start();recordOwnedHeardTrack(state.stageMusic.track.name).catch(error);pendingAudio=null;state.soundCursor=0;state.soundWindow=null;}
        state.stageSelection=selection;state.endingResources=preparedEnding;thpracLogicClock.reset();
        if(extraRun){state.extraRun=extraRun;extraRun=null;}
        disposeOriginalStageTextRasters(state.stageTextRasters);state.stageTextRasters=preparedTextRasters;preparedTextRasters=null;
        if(state.replayPlayback){state.replayNativeBound=false;state.replayPlayback.bindNativeStage(module);}
        if(thprac){const ownedSelection=selection;state.thpracTools=createThprac128Tools(module,thprac,{isCurrent:()=>state.module===module&&state.stageSelection===ownedSelection&&!state.launcherExited,playback:!!state.replayPlayback,history:state.replayPlayback?.tools,initial:selection.thpracRestartTools??thpracDraft});syncThpracToolMusic();}
        if(!state.replayPlayback&&thprac){
          const clockSeconds=performance.now()/1000,ownedSelection=selection;
          state.thpracRecorder=createThprac128Recorder(module,thprac,selection.thpracIdentity,{seconds:clockSeconds,isCurrent:()=>state.module===module&&state.stageSelection===ownedSelection&&!state.launcherExited,tools:frames=>state.thpracTools.history(frames)});
          state.replayClockLast=clockSeconds;
        }
        else if(!state.extraRun&&!selection.debugJump&&!selection.thprac&&!find('#debug-infinite-respawn').checked){
          if(typeof module._th128_world_replay_begin!=='function')throw new Error('正常录像录制内核未载入');
          if(!carryToken){
            const store=state.nativeScoreStore??=new OwnedNativeTitleRecordStore(),observed=await store.observe();
            const profile=observed?.profile??null;
            const restored=profile?nativeMainMemory(profile,(pointer,length)=>module._th128_world_records_restore(pointer,length)):module._th128_world_records_restore(0,0);
            if(restored!==1)throw new Error(module.UTF8ToString(module._th128_world_error())||'原生最高分档案载入失败');
            state.nativeScoreRevision=observed?.revision??0;state.nativeScoreRunId=crypto.randomUUID();state.nativeScoreSaveTask=null;
          }
          const clockSeconds=performance.now()/1000;
          if(!module._th128_world_replay_begin(carryToken?65536:selection.seed,clockSeconds))throw new Error(module.UTF8ToString(module._th128_world_error()));
          state.replayClockLast=clockSeconds;state.replayNativeBound=true;
        }
        if(state.stageIntroLoaded&&state.stageMusic)stageIntroCaption(stage.music?.[0],0);
        captureHighFramerateWorld();
        refreshDebugRespawnUIFromWorld();
        if(!state.replayPlayback)worldMovementInput();
        find('#loading').hidden=true;find('#pause').disabled=false;find('#game').focus();
        find(resultTarget).textContent=shots?'方向键移动；点按 Z 射击，按住 Z 蓄力，至少 45 帧且头顶 ≥30% 时松开 Z 冻结附近弹幕；C 连射、Shift 低速。使用该关原曲与原始动态 STD 背景。局部子弹、冻结、伤害与分数链路仍按此前证据，未证明整关或全路线可玩；缺失分支明确停止。':
          '方向键使用原版速度/边界与转向ANM移动琪露诺。初始位置(0,400)，碰撞上下文显式给定state1/flags0/保护120/系统门控0，这是局部集成而非完整原版初始化。首次命中执行受击特效与8tick窗口；死亡提交、射击、低速与冻结未完成时会明确停止。';
        status('真实首段运行中：原始ECL、子弹池、运动与ANM；不是完整可玩关卡。');
        draw();return;
      }
    }
    for(let step=0;step<600;step++)if(!module._th128_world_tick())break;
    const snapshot=JSON.parse(module.UTF8ToString(module._th128_world_state()));
    syncDebugRespawnUI(snapshot);
    const reason=module.UTF8ToString(module._th128_world_error());
    find(resultTarget).textContent=`难度 ${selection.difficulty} · seed ${selection.seed}\n${JSON.stringify(snapshot,null,2)}\n\n停点：${reason||'本次上限内未触发错误'}\n\n${emission?'已接入有取证支持的局部插值/运动-before-ECL、构造-before-link与guard调度，但全局帧时序未验收。首发创建已接原版ANM；玩家目标坐标显式设为(0,400)，不是完整玩家系统。这不是可玩关卡。':'坐标仅保留出生值，尚未接入运动阶段；'}tick 为本控制内核的调度计数。仅证明已执行的脚本和状态记录；尚无完整关卡、玩家射击、冻结传播与原版整局对照。`;
    if(emission){
      const records=JSON.parse(module.UTF8ToString(module._th128_world_bullet_draws()));
      const drawError=module.UTF8ToString(module._th128_world_bullet_draw_error());
      if(drawError)throw new Error(drawError);
      const stageDrawError=stageIntroDrawFailure(module);
      if(stageDrawError)throw new Error(stageDrawError);
      state.active=false;state.paused=true;state.worldMode=false;clock.reset();resetHighFrameratePresentation();stopAudio();
      find('#loading').hidden=true;find('#pause').disabled=true;
      renderer.render(orderDrawLayers(records,{world:true}),{world:true});
      status('已显示真实首发出生对象；后续碰撞更新未完成，明确停止。点击进入动画验证可返回标题。');
    }
  }catch(problem){if(!stale()&&problem.name!=='AbortError'){
    lastCheckWorldError=String(problem.message??problem).trim()||`Stage ${stage?.id??"unknown"} initialization rejected without a diagnostic`;
    find(resultTarget).textContent=problem.message;
    if(mutated){state.active=false;state.paused=true;state.worldMode=false;state.shotMode=false;find('#emit-shot').disabled=true;clock.reset();resetHighFrameratePresentation();find('#pause').disabled=true;status('关卡配置失败，已停止；旧局已不可继续，不能假装回退。');}
  }}
  finally{extraRun?.close();disposeOriginalStageTextRasters(preparedTextRasters);if(pendingAudio){pendingAudio.queue.dispose();pendingAudio.music.dispose();pendingAudio.audio.close().catch(()=>{});}if(ticket===state.loading){find('#world').disabled=false;find('#emission').disabled=!module._th128_world_enable_bullets;find('#segment').disabled=!module._th128_world_enable_bullet_updates;find('#shots').disabled=!module._th128_world_enable_player_shots;find('#play-stage').disabled=!module._th128_world_enable_z_fire;}}
}
function prepareStage(stageId,{difficulty=1,seed=34772,progress=0,debugExtra=false,thprac=null,thpracReplayIdentity=null}={}){
  assertStageStartAllowed(stageId,difficulty,{debugExtra});
  if(!state.stageCatalog)throw new Error('关卡目录尚未加载');
  if(!Number.isInteger(stageId)||stageId<1||stageId>16)throw new Error(`非法关卡ID：${stageId}`);
  if(!Number.isInteger(difficulty)||difficulty<0||difficulty>(debugExtra?4:3))throw new Error(`非法难度：${difficulty}`);
  if(!Number.isInteger(seed)||seed<0||seed>65535)throw new Error(`非法 seed：${seed}`);
  if(!Number.isInteger(progress))throw new Error(`非法 stage progress：${progress}`);
  const selection={stage:resolveStage(state.stageCatalog,stageId),difficulty,seed,progress};
  if(debugExtra){
    if(progress!==0)throw new Error('Debug EX requires a fresh original area0 start');
    selection.debugExtra=true;
    selection.debugJump={schema:'th128-debug-extra-start/1',route:'EX',stageId:16,difficulty:4,
      startingFrame:0,mode:'fresh-direct-stage',carryVerified:false,previousStagesVerified:false,normalModeClear:false};
  }
  if(thprac){selection.thprac=createThprac128Session(thprac.params,thprac.difficulty);delete selection.debugJump;if(thpracReplayIdentity)selection.thpracReplayIdentity=thpracReplayIdentity;}
  return selection;
}
async function startStage(stageId,options={}){
  assertStageStartAllowed(stageId,options.difficulty??1,{debugExtra:options.debugExtra===true});
  state.autoFlow=options.autoFlow===true;
  state.handoffFailed=false;
  const selection=prepareStage(stageId,options);
  await checkWorld(true,true,true,selection);
  if(lastCheckWorldError)throw new Error(lastCheckWorldError);
  if(!state.worldMode||state.stageSelection!==selection)throw new Error('关卡启动未完成，未切换 active stage');
  return {stageId:selection.stage.id,difficulty:selection.difficulty,seed:selection.seed,progress:selection.progress};
}
function handoffFailure(message){
  if(!lastCheckWorldError)lastCheckWorldError=String(message??" ").trim()||"Stage handoff failed without a diagnostic";
  message=lastCheckWorldError;
  state.active=false;state.paused=true;state.handoff=false;state.handoffFailed=true;state.autoFlow=false;clock.reset();resetHighFrameratePresentation();state.pressed.clear();
  if(state.worldMode){enterFaultMenu(message);return;}
  find('#runtime-overlay-title').textContent='跨关已停止';
  find('#runtime-overlay-message').textContent=message;
  find('#resume-session').hidden=true;find('#runtime-overlay').hidden=false;
  status(message);
}
function transitionSelection(transition){
  if(!state.stageSelection)throw new Error('当前 stage context 不可用');
  if(transition.configured!==true||transition.currentId!==state.stageSelection.stage.id
    ||!Number.isInteger(transition.progress)||transition.progress!==state.stageSelection.progress)
    throw new Error('stage transition does not belong to the current stage/progress context');
  if(!Number.isInteger(transition.nextId)||transition.nextId<1||transition.nextId>16||transition.nextId===16)
    throw new Error('stage transition did not publish a legal non-extra nextId');
  if(!Number.isInteger(transition.nextProgress))throw new Error('stage transition did not publish a legal nextProgress');
  const recorded=state.replayPlayback?.prepareNaturalHandoff(transition);
  return prepareStage(transition.nextId,{difficulty:state.stageSelection.difficulty,seed:recorded?.seed??state.stageSelection.seed,progress:transition.nextProgress});
}
function maybeAutoHandoff(){
  if(!state.autoFlow)return;
  if(!flowCapabilitiesReady()){state.autoFlow=false;return;}
  if(!state.worldMode||state.handoff||state.handoffFailed||state.pendingCarryToken)return;
  const transition=state.worldSnapshot?.stageTransition;
  if(!transition?.requested)return;
  if(transition.error){handoffFailure(`跨关失败：${transition.error}`);return;}
  let selection;
  try{selection=transitionSelection(transition);}catch(problem){handoffFailure(problem.message??String(problem));return;}
  if(typeof state.module._th128_world_capture_stage_carry!=='function'){
    handoffFailure('core stage carry capture runtime is unavailable; no transition token was consumed');return;
  }
  state.handoff=true;
  const generation=++state.handoffGeneration;
  let token;
  try{token=state.module._th128_world_capture_stage_carry();}catch(problem){handoffFailure(problem.message??String(problem));return;}
  if(!token){handoffFailure(state.module.UTF8ToString(state.module._th128_world_error())||'stage carry capture returned no token');return;}
  state.pendingCarryToken=token;selection.carryToken=token;
  state.active=false;state.paused=true;clock.reset();resetHighFrameratePresentation();state.pressed.clear();worldMovementInput(0,state.shotMode?761:240);
  if(state.audio?.state==='running')state.audio.suspend().catch(()=>{});
  find('#runtime-overlay').hidden=true;
  status('正在进入下一关…');
  (async()=>{
    await checkWorld(true,true,true,selection);
    if(generation!==state.handoffGeneration)return;
    state.handoff=false;
    if(lastCheckWorldError||!state.worldMode||state.stageSelection!==selection){
      handoffFailure(lastCheckWorldError||'下一关配置未完成');
      return;
    }
    state.handoffFailed=false;state.pendingCarryToken=0;
  })().catch(problem=>{if(generation===state.handoffGeneration)handoffFailure(problem.message??String(problem));});
}
function enterTitleMenu(){
  menu.page='main';menu.launching=false;menu.audioUnlocked=false;menu.audioUnlockPromise=null;
  menu.suppressTitleAudio=false;menu.infoMode='text';menu.optionsInitialized=false;
  menu.fault=false;menu.pauseOpen=false;menu.faultReason='';
  syncDebugRespawnUI(null);
  ++menu.musicRequestId;stopMenuMusic();
  state.autoFlow=false;state.handoff=false;state.handoffFailed=false;
  state.worldMode=false;state.shotMode=false;state.active=true;state.paused=false;state.pressed.clear();clock.reset();resetHighFrameratePresentation();
  find('#loading').hidden=true;find('#runtime-overlay').hidden=true;find('#error').hidden=true;
  find('#pause').disabled=false;find('#emit-shot').disabled=true;
  nativeMainRows();showMenuPage('main');setMenuIndex('main',Math.min(menu.main,6));
  find('#game-menu').hidden=true;find('#game-menu').setAttribute('aria-hidden','true');
  draw();syncNativeMenuState();
  unlockMenuAudio();
  status('原版标题动画运行中。方向键选择，Z/Enter 确认；音频自动恢复，受浏览器限制时首次按键或点击启用。');
}
function returnToTitle(){
  try{recordOwnedMainSession();}catch(problem){error(problem);return false;}
  cancelOriginalEnding();
  try{discardPendingCarry();}catch(problem){error(problem);return false;}
  ++state.action;++state.handoffGeneration;
  state.autoFlow=false;state.handoff=false;state.handoffFailed=false;
  state.worldMode=false;state.shotMode=false;state.active=false;state.paused=true;state.pressed.clear();clock.reset();resetHighFrameratePresentation();
  ++menu.launchId;menu.launching=false;menu.suppressTitleAudio=false;
  stopMenuMusic();stopAudio({keepContext:true});state.background=null;state.worldSnapshot=null;
  syncGameplayFrameUI();
  if(!state.module?._th128_boot_title||!state.module._th128_boot_title(0x1234)){error(new Error(coreError()||'无法重新启动标题场景'));return false;}
  enterTitleMenu();return true;
}
function releaseOriginalPauseInput(){
  for(const key of state.pressed)suppressedKeys.add(key);
  state.pressed.clear();
  if(state.worldMode&&state.module?._th128_world_input
    &&!state.module._th128_world_input(0,0,763))
    throw new Error(state.module.UTF8ToString(state.module._th128_world_error())||'原版暂停输入释放失败');
}
function bindOriginalReplayPreviews(){
  const observed=originalReplayPreviewPacket(state.replaySlots),pointer=state.module._malloc(observed.length);
  try{
    state.module.HEAPU8.set(observed,pointer);
    if(!state.module._th128_world_replay_previews(pointer,observed.length))
      throw new Error(state.module.UTF8ToString(state.module._th128_world_error()));
  }finally{state.module._free(pointer);}
}

function bindOriginalReplayNameContext(snapshot){
  const observed=originalReplayNameContextPacket(snapshot),pointer=state.module._malloc(observed.length);
  try{
    state.module.HEAPU8.set(observed,pointer);
    if(!state.module._th128_world_replay_previews(pointer,observed.length))
      throw new Error(state.module.UTF8ToString(state.module._th128_world_error()));
  }finally{state.module._free(pointer);}
}


function bindOriginalNameProfile(record,serial=0){
  const observed=originalNameProfilePacket(record,serial),pointer=state.module._malloc(observed.length);
  try{
    state.module.HEAPU8.set(observed,pointer);
    if(!state.module._th128_world_replay_previews(pointer,observed.length))
      throw new Error(state.module.UTF8ToString(state.module._th128_world_error()));
  }finally{state.module._free(pointer);}
}

function originalPauseConsumer(){
  if(state.nativePause)return state.nativePause;
  if(!state.module||!state.gameSounds)throw new Error('原版暂停菜单尚未准备完成');
  const module=state.module,sounds=state.gameSounds,action=state.action,selection=state.stageSelection;
  const current=()=>!controller.closed&&state.nativePause===controller&&state.module===module
    &&state.gameSounds===sounds&&state.action===action&&state.stageSelection===selection;
  const owned=async work=>{
    if(!current())return;
    try{return await work();}catch(problem){if(current())throw problem;}
  };
  const ledger=new OwnedMenuSoundLedger();state.menuSoundLedger=ledger;
  const controller=new OriginalPauseController({
    module,isCurrent:current,releaseInput:releaseOriginalPauseInput,
    consumeSound:()=>owned(async()=>{
      const owner=await ordinaryWorldAudioLane();
      if(!current())return;
      if(owner)return owner.sound();
      return consumeMenuSoundWindow(module,sounds,ledger,{present:!window.__TH128_FAST_REPLAY__});
    }),
    audio:new MenuAudioGate(sounds),
    onReplayOpen:()=>owned(async()=>{
      const store=state.replayStore??=new OwnedReplayStore(),slots=await store.observe();
      if(!current()||state.replayStore!==store)return;
      state.replaySlots=slots;bindOriginalReplayPreviews();
      if(!module._th128_world_replay_save_open())throw new Error(module.UTF8ToString(module._th128_world_error()));
      const names=state.nameFieldStore??=new OwnedLastNameFieldStore(),profile=await names.observe();
      if(!current()||state.nameFieldStore!==names||state.replayStore!==store)return;
      state.nameFieldProfile=profile;bindOriginalNameProfile(profile);
    }),
    onResume:()=>{
      if(!current())return;
      menu.pauseOpen=false;menu.fault=false;menu.page='main';
      state.paused=false;clock.reset();resetHighFrameratePresentation();originalMenuClock.reset();state.pressed.clear();
      find('#runtime-overlay').hidden=true;find('#game-menu').hidden=true;
      worldMovementInput(0,763);find('#game').focus();status('游戏继续。');
    },
    onTitle:()=>{if(!current())return;menu.pauseOpen=false;if(state.replayPlayback)returnOwnedReplayToTitle();else returnToTitle();},
    onRestart:()=>owned(async()=>{
      if(state.replayPlayback)return restartOwnedReplay();
      await recordOwnedMainSession();
      if(!current())return;
      const selected=state.stageSelection;
      if(!selected)throw new Error('重新开始缺少当前路线');
      if(selected.thprac){menu.pauseOpen=false;return startLaunch(inheritThpracTools(thpracStageSelection(selected.thprac).selection),'route');}
      const stageId=selected.stage.id===16?16:selected.stage.id<=5?1:selected.stage.id<=10?6:11;
      const next=prepareStage(stageId,{difficulty:selected.difficulty,seed:34772,progress:0,debugExtra:selected.debugExtra===true});
      menu.pauseOpen=false;return startLaunch(next,'route');
    }),
    onPresentation:snapshot=>owned(async()=>{
      if(snapshot.replayPhase===2&&!snapshot.replayNameContextReady)bindOriginalReplayNameContext(snapshot);
      if(snapshot.replayPhase===3){
        const store=state.replayStore??=new OwnedReplayStore(),slots=state.replaySlots;
        const names=state.nameFieldStore,profile=state.nameFieldProfile;
        const storageCurrent=()=>current()&&state.replayStore===store&&state.replaySlots===slots
          &&state.nameFieldStore===names&&state.nameFieldProfile===profile;
        const timestamp=BigInt(Math.floor(Date.now()/1000));
        const size=module._th128_world_replay_snapshot(Number(timestamp&0xffffffffn),Number(timestamp>>32n));
        if(!Number.isSafeInteger(size)||size<292||size>16*1024*1024)throw new Error(module.UTF8ToString(module._th128_world_error())||'原生录像数据不完整');
        const pointer=module._th128_world_replay_data();
        if(!Number.isSafeInteger(pointer)||pointer<=0||pointer+size>module.HEAPU8.length)throw new Error('原生录像内存范围无效');
        const native=module.HEAPU8.slice(pointer,pointer+size),checksum=module._th128_world_replay_checksum()>>>0;
        if(OwnedReplayStore.nativeChecksum(native)!==checksum)throw new Error('原生录像完整读回校验失败');
        const saved=await store.save(snapshot.replaySlot,native,{expectedDigest:slots?.[snapshot.replaySlot]?.digest??null});
        if(!storageCurrent())return;
        if(!saved.strictFullReadback)throw new Error('原生录像完整保存读回失败');
        if(!names||!profile)throw new Error('原上次姓名字段的真实存储owner不可用');
        const storedName=await names.save(new Uint8Array(snapshot.replayName),profile);
        if(!storageCurrent())return;
        bindOriginalNameProfile(storedName,snapshot.replaySerial);
        if(!module._th128_world_replay_saved(snapshot.replaySerial,snapshot.replaySlot,saved.rawBytes,saved.rawChecksum))
          throw new Error(module.UTF8ToString(module._th128_world_error())||'原生录像保存确认失败');
        state.nameFieldProfile=storedName;
        slots[snapshot.replaySlot]={digest:saved.digest,bytes:saved.bytes};
        bindOriginalReplayPreviews();
        status('录像已保存到 No.'+String(snapshot.replaySlot+1).padStart(2,'0'));
      }
      if(snapshot.manual&&snapshot.helpPage>=0&&snapshot.helpPage!==state.manualPageLoaded){
        const renderer=state.renderer,host=state.controller,base=state.base;
        const reference=originalHelpPagePins[find('#variant').value]?.[snapshot.helpPage];
        if(!reference)throw new Error('原版操作说明页缺失');
        const helpCurrent=()=>current()&&state.renderer===renderer&&state.controller===host&&state.base===base;
        await loadOriginalHelpPage(renderer,{...reference,url:safeAssetUrl(base,reference.file)},{signal:host.signal,isCurrent:helpCurrent});
        if(!helpCurrent())return;
        state.manualPageLoaded=snapshot.helpPage;
      }
    })
  });
  state.nativePause=controller;return controller;
}
async function openOriginalNativePause(alreadyOpened=false){
  const module=state.module,action=state.action,selection=state.stageSelection;
  if(!alreadyOpened&&state.worldAdvancePromise)await state.worldAdvancePromise;
  if(module!==state.module||action!==state.action||selection!==state.stageSelection||state.endingLoading||state.endingHost)return;
  if(!state.worldMode||!state.active||menu.fault)return;
  if(!alreadyOpened&&pendingOriginalEndingRequest())return;
  if(state.nativePause?.active||state.nativePausePending)return;
  menu.pauseOpen=true;menu.page='pause';state.paused=true;clock.reset();resetHighFrameratePresentation();originalMenuClock.reset();
  find('#runtime-overlay').hidden=true;find('#game-menu').hidden=true;
  const controller=originalPauseConsumer();
  const pending=controller.open({alreadyOpened});
  state.nativePausePending=pending;
  const current=()=>module===state.module&&action===state.action&&selection===state.stageSelection&&controller===state.nativePause&&!controller.closed;
  try{await pending;if(current())status(alreadyOpened?'满身疮痍 · 选择再挑战或返回标题。':'游戏暂停 · 方向键选择，Z/Enter 确认，X/Esc 返回。');}
  catch(problem){if(current())throw problem;}
  finally{if(state.nativePausePending===pending)state.nativePausePending=null;}
}
function handlePauseKey(code){state.nativePause?.keyDown(code);return true;}
function openPauseMenu(){
  if(!state.worldMode||!state.active||menu.pauseOpen||menu.fault)return;
  openOriginalNativePause().catch(error);
}
function resumeGame(){
  if(!state.worldMode||menu.fault||!state.nativePause?.active)return;
  state.nativePause.keyDown('Escape');state.nativePause.keyUp('Escape');
}
function restoreNativePageAfterLaunchFailure(message){
  if(!state.mainController?.prepared||state.mainController.disposed)return false;
  const snapshot=nativeMenuSnapshot();
  if(!snapshot.available||!snapshot.ready||snapshot.transitioning||snapshot.page<1||snapshot.page>2)return false;
  menu.page=nativePageNames[snapshot.page];menu.fault=false;menu.pauseOpen=false;
  state.autoFlow=menu.launchPreviousAutoFlow;state.active=true;state.paused=false;state.pressed.clear();clock.reset();resetHighFrameratePresentation();
  find('#game-menu').hidden=true;find('#game-menu').setAttribute('aria-hidden','true');
  find('#runtime-overlay').hidden=true;
  find('#error').hidden=false;find('#error').textContent=message;
  syncNativeMenuState();
  status(`原生 ${menu.page} 页保留；关卡启动失败：${message}`);
  return true;
}
function showLaunchError(message){
  menu.launching=false;menu.launchMessage=message;menu.launchBody=message;
  if(menu.launchReturn==='fault'){enterFaultMenu(message);return;}
  if(!state.worldMode){
    if(restoreNativePageAfterLaunchFailure(message))return;
    returnToTitle();
    find('#error').hidden=false;find('#error').textContent=message;
    status(`原版标题已返回；关卡启动失败：${message}`);
    return;
  }
  state.autoFlow=menu.launchPreviousAutoFlow;state.active=!!state.worldMode;state.paused=true;clock.reset();resetHighFrameratePresentation();state.pressed.clear();
  find('#menu-launch-body').textContent=message;showMenuPage('launch');status(message);
}
function startLaunch(selection,returnPage='route'){
  assertStageStartAllowed(selection?.stage?.id,selection?.difficulty,{debugExtra:selection?.debugExtra===true});
  if(menu.launching)return;
  menu.launching=true;menu.launchReturn=returnPage;menu.launchSelection=selection;
  syncDebugStageJumpControls();
  menu.launchPreviousAutoFlow=state.autoFlow;const id=++menu.launchId;
  find('#error').hidden=true;
  suppressHeldConfirmKeys();
  state.autoFlow=!selection.thprac;state.pressed.clear();
  menu.launchMessage=`正在预加载 stage ${selection.stage.id} · rank ${selection.difficulty}…`;
  find('#menu-launch-body').textContent=menu.launchMessage;showMenuPage('launch');
  return (async()=>{
    try{await checkWorld(true,true,true,selection);}catch(problem){if(id===menu.launchId)showLaunchError(problem.message??String(problem));return;}
    if(id!==menu.launchId){
      if(!menu.launching&&state.worldMode&&state.active&&state.stageSelection===selection)returnToTitle();
      return;
    }
    menu.launching=false;
    if(lastCheckWorldError||!state.worldMode||state.stageSelection!==selection){
      showLaunchError(lastCheckWorldError||'关卡启动失败，未进入 active world');return;
    }
    find('#game-menu').hidden=true;
    suppressHeldConfirmKeys();
    state.pressed.clear();
    menu.page='main';menu.fault=false;menu.pauseOpen=false;
    find('#runtime-overlay').hidden=true;find('#error').hidden=true;
    if(state.worldMode)worldMovementInput(0,761);
    status(`stage ${selection.stage.id} · rank ${selection.difficulty} 已启动。`);
  })().catch(problem=>{if(id===menu.launchId)showLaunchError(problem.message??String(problem));})
    .finally(()=>syncDebugStageJumpControls());
}
function cancelLaunch(){
  if(!menu.launching)return;
  ++menu.launchId;menu.launching=false;++state.action;state.autoFlow=menu.launchPreviousAutoFlow;state.pressed.clear();
  if(menu.launchReturn==='fault'){enterFaultMenu('已取消重启；原 world 仍停止。');return;}
  if(state.worldMode){state.active=true;state.paused=true;showMenuPage('pause');}
  else if(!state.mainController?.prepared||state.mainController.disposed)returnToTitle();
  else{state.active=true;state.paused=false;showMenuPage(menu.launchReturn==='pause'?'route':menu.launchReturn);}
  status('已取消启动；没有切换 active stage。');
}
function backFromLaunch(){
  if(menu.launching){cancelLaunch();return;}
  ++menu.launchId;++state.action;state.autoFlow=menu.launchPreviousAutoFlow;
  if(menu.launchReturn==='fault'){enterFaultMenu('没有重建新的 world。');return;}
  if(state.worldMode){state.active=true;state.paused=true;showMenuPage('pause');}
  else if(!state.mainController?.prepared||state.mainController.disposed)returnToTitle();
  else{state.active=true;state.paused=false;showMenuPage(menu.launchReturn==='pause'?'route':menu.launchReturn);}
}
function launchSelectedRoute(){
  const stageId=menuRouteStages[menu.route];if(!stageId)return;
  let selection;
  try{selection=prepareStage(stageId,{difficulty:menu.difficulty,seed:34772,progress:0});}
  catch(problem){showLaunchError(problem.message??String(problem));return;}
  startLaunch(selection,'route');
}
function leaveMusicPage(){
  ++menu.musicRequestId;stopMenuMusic();menu.suppressTitleAudio=false;
  if(menu.audioUnlocked)startTitleSource();
  showMenuPage('main');
}
function confirmMusicSelection(){
  const track=musicTracks()[menu.music];if(!track)return;
  if(menuMusic.source&&menuMusic.track?.file===track.file){
    stopMenuMusic();menu.infoDescription='MUSIC · 已停止；X 返回并恢复标题 BGM。';
    updateMenuCopy();populateMusicRows();updateMenuSelection();return;
  }
  playMenuTrack(track);
}
function moveMenu(delta){
  const rows=menuRows(),length=rows.length;if(!length)return;
  setMenuIndex(menu.page,(menuIndex()+delta+length)%length);
  if(menu.page==='difficulty'){savePreference('difficulty',menu.difficulty);updateMenuCopy();}
  else if(menu.page==='route'){savePreference('route',menu.route);updateMenuCopy();}
}
function adjustMenu(delta){
  if(menu.page==='difficulty'||menu.page==='route'){moveMenu(delta);return;}
  if(menu.page==='options'){adjustOption(delta);updateMenuCopy();return;}
  if(menu.page==='info'&&menu.infoMode==='music'){
    const tracks=musicTracks();if(!tracks.length)return;
    ++menu.musicRequestId;
    menu.music=(menu.music+delta+tracks.length)%tracks.length;
    if(menuMusic.source){stopMenuMusic();menu.infoDescription='MUSIC · 选择已切换，旧 BGM 已停止。';updateMenuCopy();}
    populateMusicRows();updateMenuSelection();
  }
}
function confirmMenu(){
  const row=menuRows()[menuIndex()];
  if(menu.page==='main'){
    const action=row?.dataset.menuMain;
    if(action==='start')showMenuPage('difficulty');
    else if(action==='replay')setInfoPage('REPLAY · 尚未实现','网页版本暂无回放数据或回放播放器；不会伪造通关回放。');
    else if(action==='result')setInfoPage('RESULT · 尚未实现','尚无跨关结果/结算页；当前只显示明确边界，不伪造成绩。');
    else if(action==='music')showMusicPage();
    else if(action==='options'){
      menu.option=0;
      menu.versionIndex=Math.max(0,Object.keys(state.catalog?.variants??{}).indexOf(currentVariantKey()));
      menu.optionsInitialized=true;showMenuPage('options');
    }
    else if(action==='manual')showManual();
    else if(action==='quit'){suppressHeldConfirmKeys();if(returnToTitle())status('网页版 Quit 仅返回标题，不会关闭浏览器。');}
  }else if(menu.page==='difficulty'){
    savePreference('difficulty',menu.difficulty);showMenuPage('route');
  }else if(menu.page==='route'){
    savePreference('route',menu.route);launchSelectedRoute();
  }else if(menu.page==='options')activateOption();
  else if(menu.page==='pause'){
    const action=row?.dataset.menuPause;
    if(action==='resume')resumeGame();
    else if(action==='restart'){if(state.stageSelection)startLaunch(state.stageSelection,'pause');}
    else if(action==='title'){suppressHeldConfirmKeys();returnToTitle();}
  }else if(menu.page==='info'){
    if(menu.infoMode==='music')confirmMusicSelection();else showMenuPage('main');
  }else if(menu.page==='manual')showMenuPage('main');
  else if(menu.page==='launch')backFromLaunch();
}
function backMenu(){
  if(menu.page==='main'){status('主菜单：方向键选择；START 进入难度与起点。');return;}
  if(menu.page==='difficulty'){showMenuPage('main');return;}
  if(menu.page==='route'){showMenuPage('difficulty');return;}
  if(menu.page==='options'||menu.page==='manual'){showMenuPage('main');return;}
  if(menu.page==='info'){if(menu.infoMode==='music')leaveMusicPage();else showMenuPage('main');return;}
  if(menu.page==='pause'){resumeGame();return;}
  if(menu.page==='launch')backFromLaunch();
}
function handleMenuKey(code){
  if(code==='Escape'||code==='KeyX'){backMenu();return;}
  if(menu.page==='launch')return;
  if(code==='ArrowUp')moveMenu(-1);
  else if(code==='ArrowDown')moveMenu(1);
  else if(code==='ArrowLeft')adjustMenu(-1);
  else if(code==='ArrowRight')adjustMenu(1);
  else if(code==='KeyZ'||code==='Enter')confirmMenu();
}


async function runOwnedSaveDebug(operation){
  const snapshot=nativeMenuSnapshot(),owner=state.mainController,module=state.module;
  if(state.mainSaveDebugBusy||state.worldMode||!state.active||menu.launching||!snapshot.available||snapshot.page!==0
    ||!snapshot.ready||snapshot.transitioning||!owner?.prepared||owner.disposed||owner.busy){
    status('请等待主菜单准备完成，再操作网页存档调试。');return;
  }
  const message=operation==='clear'?'清理本网页游戏的设置、已听曲目、成绩记录、录像和上次姓名？不会修改Windows原版存档。':
    '向本网页调试存档写入六线通关条件以测试EX解锁？这是假定数据，不计真实通关；进入EX仍标记调试。';
  if(!window.confirm(message))return;
  state.mainSaveDebugBusy=true;owner.release();suppressHeldConfirmKeys();
  find('#debug-clear-save').disabled=true;find('#debug-unlock-save').disabled=true;
  try{
    await state.mainStorageTask;
    if(operation==='clear'){
      state.mainController?.dispose();state.controller?.abort();
      for(const owner of [state.mainSettings,state.mainReplayStore,state.replayStore,state.nameFieldStore])await owner?.close?.();
      state.mainSettings=null;state.mainReplayStore=null;state.replayStore=null;state.nameFieldStore=null;
      const receipt=await clearOwnedWebSaveData();state.mainSavedSettings=null;setDebugExtraSaveEligibility(0);
      for(let index=sessionStorage.length-1;index>=0;--index){const key=sessionStorage.key(index);if(key?.startsWith('th128.'))sessionStorage.removeItem(key);}
      state.debugInfiniteRespawnRequested=false;state.mainStorageTask=null;
      await loadVariant(find('#variant').value);
      status('已清理本网页存档：'+receipt.clearedDatabases.length+'个游戏存档库；原版存档未动。');
    }else{
      const saved=await updateOwnedMainSettings(()=>({debugClearMask:63}));
      state.mainSavedSettings=saved;setDebugExtraSaveEligibility(63);
      if(state.module!==module||state.mainController!==owner||owner.disposed){
        status('调试存档已保存；主菜单准备完成后将恢复EX解锁条件。');return;
      }
      if(module._th128_title_extra_save(saved.debugClearMask??0)!==1)throw new Error('Native EX debug save flags rejected');
      state.extraMenu?.refresh?.();
      status('调试存档：六线6/6，EX已解锁（模拟数据，不算真实通关）。');
    }
  }catch(problem){error(problem);}
  finally{state.mainSaveDebugBusy=false;owner.release();clock.reset();resetHighFrameratePresentation();find('#debug-clear-save').disabled=false;find('#debug-unlock-save').disabled=false;}
}

find('#main-audio-refresh').addEventListener('click',()=>{find('#main-audio-state').textContent=JSON.stringify(mainAudioDiagnosticSnapshot(),null,2);});
find('#debug-clear-save').addEventListener('click',()=>runOwnedSaveDebug('clear'));
find('#debug-unlock-save').addEventListener('click',()=>runOwnedSaveDebug('unlock'));
find('#export-owned-replay').addEventListener('click',()=>exportSelectedOwnedReplay().catch(error));
find('#import-original-replay').addEventListener('click',()=>find('#import-original-replay-file').click());
find('#import-original-replay-file').addEventListener('change',event=>{const file=event.currentTarget.files?.[0];if(file)importOriginalReplayFile(file).catch(error);});
find('#world').addEventListener('click',()=>{state.autoFlow=false;checkWorld();});
find('#emission').addEventListener('click',()=>{state.autoFlow=false;checkWorld(true);});
find('#segment').addEventListener('click',()=>{state.autoFlow=false;checkWorld(true,true);});
find('#shots').addEventListener('click',()=>{state.autoFlow=false;checkWorld(true,true,true);});
find('#play-stage').addEventListener('click',()=>{state.autoFlow=true;checkWorld(true,true,true);});
find('#emit-shot').addEventListener('click',async()=>{
  const module=state.module,action=state.action,selection=state.stageSelection;
  try{
    if(state.worldAdvancePromise)await state.worldAdvancePromise;
    if(module!==state.module||action!==state.action||selection!==state.stageSelection||state.endingLoading||state.endingHost)return;
    if(!state.worldMode||!state.shotMode||!state.active||state.handoff||pendingOriginalEndingRequest())return;
    if(!module._th128_world_emit_player_shots(0))throw new Error(module.UTF8ToString(module._th128_world_error()));
    await drainGameSounds();
    resetHighFrameratePresentation();draw();
  }catch(problem){if(module===state.module&&action===state.action&&selection===state.stageSelection)error(problem);}
});
find('#game').addEventListener('pointerdown',()=>{
  if(state.active&&!state.worldMode&&!menu.launching)unlockMenuAudio();
});
find('#high-framerate-mode').addEventListener('change',event=>{
  setHighFramerateMode(event.target.checked);find('#game').focus();
});
window.addEventListener('keydown',event=>{
  if(thpracToolsPanel?.isOpen)return;
  if(event.target instanceof HTMLSelectElement||event.target instanceof HTMLInputElement)return;
  if(handleThpracToolKey(event))return;
  if(practiceMenu?.isOpen)return;
  if(state.launcherExited)return;
  if(event.target instanceof HTMLSelectElement||event.target instanceof HTMLInputElement)return;
  const menuKeys=['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','KeyZ','KeyX','Enter','Escape'];
  if(menuKeys.includes(event.code))event.preventDefault();
  if(event.code==='KeyZ'||event.code==='Enter')event.preventDefault();
  if(state.webInput&&!state.webInput.acceptKeyDown(event))return;
  if(state.pressed.has(event.code)||suppressedKeys.has(event.code))return;
  state.pressed.add(event.code);
  if(state.normalPostrun){
    if(state.paused){if(['Escape','Enter','KeyZ','KeyX'].includes(event.code))resumeNormalPostrun();return;}
    if(event.code==='Escape')pauseNormalPostrun();else state.normalPostrun.input.down(event.code);return;
  }
  if(state.endingHost){
    if(state.paused){if(['KeyZ','Enter','KeyX','Escape'].includes(event.code))resumeTitlePause();return;}
    state.endingHost.owner.input.down(event.code);return;
  }
  if(state.endingLoading)return;
  if(state.worldMode){
    if(menu.fault){handleFaultKey(event.code);return;}
    if(menu.pauseOpen){handlePauseKey(event.code);return;}
    if(menu.page==='launch'){handleMenuKey(event.code);return;}
    if(!state.active||state.handoff)return;
    if(state.replayPlayback){
      if(state.replayFinished&&['Escape','Enter','KeyZ'].includes(event.code))stopOwnedReplayToList();
      else if(event.code==='Escape')openPauseMenu();
      else if(['Enter','KeyZ'].includes(event.code))worldMovementInput(1,0);
      return;
    }
    if(movementKeys.has(event.code))worldMovementInput(movementKeys.get(event.code),0);
    else if(event.code==='Escape')openPauseMenu();
    else if(event.code==='KeyX'){
      if(!gameplayXPress())status('X 完美冻结需要新 core 的 perfectFreezeNormalEmptyLasers 能力，且仅在玩家射击关卡可用；当前未发送。');
    }
    return;
  }
  if(menu.page==='launch'){handleMenuKey(event.code);return;}
  if(state.active){
    if(state.paused&&!find('#runtime-overlay').hidden){
      if(event.code==='KeyZ'||event.code==='Enter'||event.code==='KeyX'||event.code==='Escape')resumeTitlePause();
      return;
    }
    if(nativePageNames.includes(menu.page)){
      if(state.paused){state.paused=false;clock.reset();resetHighFrameratePresentation();}
      if(menuKeys.includes(event.code))unlockMenuAudio();
      if(menu.audioUnlocked&&state.audio?.state==='suspended')state.audio.resume().catch(error);
      handleNativeTitleKey(event.code);
      return;
    }
    handleMenuKey(event.code);return;
  }
});
window.addEventListener('keyup',event=>{
  if(practiceMenu?.isOpen)return;
  if(state.launcherExited)return;
  if(state.webInput&&!state.webInput.acceptKeyUp(event))return;
  if(state.normalPostrun){state.pressed.delete(event.code);suppressedKeys.delete(event.code);state.normalPostrun.input.up(event.code);return;}
  if(!state.worldMode)state.mainController?.keyUp(event.code);
  if(menu.pauseOpen&&state.nativePause){state.pressed.delete(event.code);suppressedKeys.delete(event.code);state.nativePause.keyUp(event.code);return;}
  if(state.endingHost){state.pressed.delete(event.code);suppressedKeys.delete(event.code);state.endingHost.owner.input.up(event.code);return;}
  if(state.endingLoading){state.pressed.delete(event.code);suppressedKeys.delete(event.code);return;}
  if(state.replayPlayback){state.pressed.delete(event.code);if(['KeyZ','Enter'].includes(event.code))worldMovementInput(0,1);return;}
  const wasGameplayX=event.code==='KeyX'&&state.pressed.has('KeyX');
  state.pressed.delete(event.code);suppressedKeys.delete(event.code);
  if(!state.handoff&&movementKeys.has(event.code))worldMovementInput(0,movementKeys.get(event.code));
  else if(wasGameplayX)gameplayXRelease();
});
function releaseGameInput(){
  state.mainController?.release();
  if(state.normalPostrun){state.normalPostrun.input.release();state.pressed.clear();return;}
  if(state.replayPlayback){state.pressed.clear();return;}
  if(menu.pauseOpen&&state.nativePause){state.pressed.clear();state.nativePause.held=0;state.nativePause.pressed=0;return;}
  if(state.endingHost||state.endingLoading){state.pressed.clear();state.endingHost?.owner.input.release();return;}
  const hadGameplayX=gameplayXHeld();
  state.pressed.clear();
  let released=state.shotMode?761:240;
  if(hadGameplayX)released|=perfectFreezeXBit;
  worldMovementInput(0,released);
}
function loseFocus(){
  if(menu.pauseOpen&&state.nativePause){state.pressed.clear();state.nativePause.suspend().catch(error);return;}
  if(state.normalPostrun){state.pressed.clear();pauseNormalPostrun();return;}
  if(state.endingHost||state.endingLoading){state.pressed.clear();pauseOriginalEnding();return;}
  releaseGameInput();
  if(menu.launching){cancelLaunch();return;}
  if(state.worldMode&&state.active&&!state.paused)openPauseMenu();
  else if(state.active&&!state.worldMode&&!state.paused){state.paused=true;clock.reset();resetHighFrameratePresentation();state.audio?.suspend().catch(()=>{});}
}
window.addEventListener('blur',loseFocus);
document.addEventListener('visibilitychange',()=>{if(document.hidden)loseFocus();});
window.addEventListener('pagehide',()=>{state.webInput?.dispose();cancelOriginalEnding();state.controller?.abort();state.renderer?.dispose();stopAudio();});

function installWebInput(){
  if(state.webInput)return state.webInput;
  const canvas=find('#game');let previous=[],owner=0;
  state.webInput=mountWebInput({canvas,container:canvas.closest('.screen'),
    getContext:()=>{
      const next=[state.module,state.action,state.stageSelection,state.worldMode,state.normalPostrun,state.endingHost,state.endingLoading,
        state.handoff,state.nativePause,state.replayPlayback,state.paused,menu.launchId,menu.page,menu.pauseOpen,menu.fault];
      if(next.some((value,index)=>value!==previous[index])){previous=next;owner++;}
      return owner;
    },
    isEnabled:()=>Boolean(state.active||menu.launching||state.normalPostrun||state.endingHost||menu.fault)});
  return state.webInput;
}
installWebInput();
function installLauncherRuntime(){
  const context=getPageLaunchContext();if(!context)return;
  state.launcherManaged=true;const owner=Symbol('launcher-keyboard');
  context.bindRuntime({
    keyboard(value){
      if(context.signal.aborted||state.launcherExited)throw new Error('Runtime page exited');
      const input=state.webInput,keys=new Set(input.readKeySource(owner));
      if(value.down)keys.add(value.code);else keys.delete(value.code);
      if(!input.setKeySource(owner,[...keys]))throw new Error('Runtime input owner is inactive');
    },
    clearInput(){state.webInput?.clearKeySource(owner);},
    exit(){
      if(state.launcherExited)return;
      state.launcherExited=true;state.active=false;state.paused=true;state.loading=(state.loading??0)+1;++menu.launchId;
      practiceMenu?.dispose();
      state.controller?.abort();
      try{cancelOriginalEnding();}finally{
        try{state.webInput?.dispose();releaseGameInput();}finally{
          try{state.renderer?.dispose();}finally{state.renderer=null;stopAudio();clock.reset();resetHighFrameratePresentation();}
        }
      }
    }
  });
  const options=context.config.options??{};
  if(options.limitPresentationTo60!==undefined)setHighFramerateMode(!options.limitPresentationTo60);
  if(options.touchEnabled!==undefined)state.webInput.layer.hidden=!options.touchEnabled;
}
function finishLauncherStartup(){
  const context=getPageLaunchContext();if(!context)return;
  if(context.signal.aborted||state.launcherExited||!state.module||!state.renderer||!state.active){
    context.fail(new Error('Runtime startup did not reach a ready title'));return;
  }
  context.finish();
}
function noteLauncherPresentation(before){
  if((state.presentationSerial??0)>before)getPageLaunchContext()?.notePresented(true);
}
installLauncherRuntime();

async function present(timestamp){
  if(state.launcherExited)return;
  try{
    state.webInput?.syncContext();
    let interpolation=1,presentationSteps=0;
    const nativeMenu=state.active&&state.worldMode&&menu.pauseOpen&&!menu.fault?state.nativePause:null;
    if(nativeMenu){
      resetHighFrameratePresentation();clock.advance(timestamp,false);
      const presentationOnly=nativeMenu.canStepPresentation();
      const ready=nativeMenu.active&&!nativeMenu.busy&&!state.nativePausePending&&!document.hidden&&(!nativeMenu.suspended||presentationOnly);
      const timing=originalMenuClock.advance(timestamp,ready);presentationSteps=timing.steps;
      if(timing.stalled&&ready&&!nativeMenu.suspended){await nativeMenu.suspend();status('菜单帧调度停顿，已暂停菜单输入与音频；按键后恢复。');}
      for(let count=0;count<timing.steps&&state.nativePause===nativeMenu&&nativeMenu.active&&menu.pauseOpen;++count){
        if(nativeMenu.suspended)await nativeMenu.stepPresentation();else await nativeMenu.step();
      }
    }else{
      originalMenuClock.advance(timestamp,false);
      const active=state.active&&!state.paused&&!state.endingLoading&&!document.hidden&&!state.thpracReplaySaving&&!practiceMenu?.isOpen&&!thpracToolsPanel?.isOpen;
       const timing=state.thpracTools?thpracLogicClock.advance(timestamp,active,practiceLogicRate()):clock.advance(timestamp,active);presentationSteps=timing.steps;
      highFramerateCatchup=timing.steps>1;
      if(highFramerateCatchup)resetHighFrameratePresentation();
      if(timing.stalled&&state.active&&!state.paused){pause();status('浏览器帧调度停顿，已暂停而不是跳过游戏逻辑帧。');}
      for(let count=0;count<timing.steps&&state.active&&!state.paused;count++){
        if(state.normalPostrun)await advanceNormalPostrun();else if(state.endingHost)await advanceOriginalEnding();else await advance();
        if(!highFramerateCatchup)captureHighFramerateWorld();
        if(state.endingPromise)await state.endingPromise;
      }
      interpolation=renderInterpolationPhase(clock);
    }
    const fixedWorld=state.worldMode&&state.shotMode&&!state.normalPostrun&&!state.endingHost&&!state.endingLoading;
    const shouldPresent=highFramerateEnabled||!fixedWorld||presentationSteps>0;
    const beforePresentation=state.presentationSerial??0;
    const presented=state.active&&shouldPresent?draw(interpolation):false;
    if(state.launcherManaged)noteLauncherPresentation(beforePresentation);
    if(presented&&state.worldMode&&state.shotMode&&!document.hidden)originalPresentationFPS.observe(performance.now());
    else if(!state.active||!state.worldMode||!state.shotMode||document.hidden||state.endingHost||state.endingLoading)originalPresentationFPS.reset();
    if(state.autoFlow&&state.worldMode&&!state.paused&&!state.normalPostrun&&!state.endingHost&&!state.endingLoading)maybeAutoHandoff();
    syncGameplayFrameUI();
  }catch(problem){error(problem);}
  requestAnimationFrame(present);
}
requestAnimationFrame(present);

function checkpointCoreGeneration(){
  return state.module?checkpointModuleGenerations.get(state.module)??null:null;
}
async function checkpointBeginNormalA(signal){
  const checkAbort=()=>{if(signal?.aborted)throw signal.reason??new DOMException('Checkpoint entry aborted','AbortError');};
  let ownedSelection=null;
  try{
  checkAbort();
  if(state.handoff||state.pendingCarryToken)throw new Error('checkpoint cannot replace an owned original stage handoff');
  if(state.worldMode&&!returnToTitle())throw new Error('checkpoint could not return to the original title');
  if(!state.worldMode&&state.paused)resumeTitlePause();
  const menuDeadline=Date.now()+30000;
  while(Date.now()<menuDeadline){checkAbort();const menu=nativeMenuSnapshot();if(menu.available&&menu.ready&&!menu.transitioning)break;await new Promise(resolve=>setTimeout(resolve,25));}
  for(const [page,choice] of [[0,0],[1,1],[2,0]]){
    checkAbort();
    if(!nativeMenuPage(page,choice))throw new Error(`checkpoint native menu page ${page} was refused`);
    const deadline=Date.now()+30000;
    while(Date.now()<deadline){checkAbort();const menu=nativeMenuSnapshot();if(menu.page===page&&menu.ready&&!menu.transitioning)break;await new Promise(resolve=>setTimeout(resolve,25));}
  }
  checkAbort();launchNativeRoute();ownedSelection=menu.launchSelection;
  const worldDeadline=Date.now()+45000;
  while(Date.now()<worldDeadline){
    checkAbort();
    const current=window.th128Debug.state();
    if(current.worldMode&&current.stageId===1){
      const native=window.th128Debug.inspectWorld();
      if(native.error)throw new Error(native.error);
      if(native.snapshot?.frame===0)return true;
      if(Number.isInteger(native.snapshot?.frame))throw new Error(`checkpoint entry advanced before native frame0: ${native.snapshot.frame}`);
    }
    if(String(current.error??'').trim())throw new Error(current.error);
    await new Promise(resolve=>setTimeout(resolve,25));
  }
  throw new Error('checkpoint original Normal A entry did not reach stage1 frame0');
  }catch(problem){
    if(signal?.aborted&&ownedSelection){
      if(menu.launching&&menu.launchSelection===ownedSelection)cancelLaunch();
      if(state.worldMode&&state.stageSelection===ownedSelection&&!returnToTitle())
        throw new Error('checkpoint aborted entry cleanup failed');
    }
    throw problem;
  }
}

window.th128Debug={
  inspectWorld:()=>{
    if(!state.worldMode)throw new Error('World inspection requires an active world');
    const snapshot=JSON.parse(state.module.UTF8ToString(state.module._th128_world_state()));
    state.worldSnapshot=snapshot;
    syncGameplayMusicState(snapshot.message);
    syncDebugRespawnUI(snapshot);
    syncGameplayFrameUI(snapshot);
    const draws=JSON.parse(state.module.UTF8ToString(state.module._th128_world_bullet_draws()));
    const error=state.module.UTF8ToString(state.module._th128_world_error()),drawError=state.module.UTF8ToString(state.module._th128_world_bullet_draw_error());
    const stageDrawError=stageIntroDrawFailure(state.module);
    if(stageDrawError)throw new Error(stageDrawError);
    return {snapshot,draws,error,drawError,stageDrawError};
  },
  redrawEmission:({interpolation=1}={})=>{
    if(!state.module?._th128_world_bullet_draws||!state.renderer)throw new Error('Bullet presentation is unavailable');
    const raw=worldPresentationForDraw();let records=raw.records.slice();const world=raw.world;
    state.worldSnapshot=world;
    const nativePlayer=records.find(record=>record.id===4000000&&record.drawPass==='player');
    const presentation=highFramerateWorldView(world,records,interpolation);records=presentation.records;
    const displayedPlayer=records.find(record=>record.id===4000000&&record.drawPass==='player');
    const fpsRate=state.shotMode?originalPresentationFPS.value:null,fpsDraws=originalFPSDraws(fpsRate);
    records.push(...fpsDraws);
    syncDebugRespawnUI(world);
    syncGameplayFrameUI(world);
    if(menu.pauseOpen&&!menu.fault&&state.nativePause){
      const layers=splitOriginalMenuDraws(records);
      state.renderer.render(orderDrawLayers(layers.battle,{world:true}),{world:true,background:state.background,shake:world.screenShake??[0,0]});
      if(state.nativePause.active)state.renderer.renderOriginalPauseFrame(orderDrawLayers(layers.menu,{world:true}),state.nativePause.snapshot());
      syncOriginalFPSPresentation(fpsRate,fpsDraws);
      const dialogue=find('#dialogue-text');if(dialogue)dialogue.hidden=true;
      return records.length;
    }
    state.renderer.render(orderDrawLayers(records,{world:true}),{world:true,background:presentation.background,shake:world.screenShake??[0,0]});
    syncOriginalFPSPresentation(fpsRate,fpsDraws);
    if(!state.replayPlayback&&world.normalRecord?.bound&&!world.normalRecord?.tainted&&!world.normalRecord?.prepared&&state.replayNativeBound)
      if(state.module._th128_world_records_present(performance.now()/1000)!==1)throw new Error(state.module.UTF8ToString(state.module._th128_world_error())||'原生成绩显示时钟被拒绝');
    const message=world.message,canvas=find('#game');let overlay=find('#dialogue-text');
    canvas.dataset.nativePresentation=JSON.stringify({frame:world.frame,highFramerate:highFramerateEnabled,interpolation:presentation.interpolated?interpolation:1,nativePlayerX:nativePlayer?.x??null,displayedPlayerX:displayedPlayer?.x??null,stage:world.flow?.currentId??null,messageActive:!!message?.active,messageClock:message?.clock??null,dialogueText:message?.active?messageTextLayout(message,find('#variant').value).map(line=>line.text).join('\n'):'',bossAnimations:records.filter(record=>record.bank===3&&record.entry===0).slice(0,16).map(record=>({id:record.id,script:record.script??null,sprite:record.sprite,clock:record.clock??null,visualTicks:record.visualTicks??null,x:record.x,y:record.y,flags:record.flags,flags2:record.flags2}))});
    if(!overlay){overlay=document.createElement('canvas');overlay.id='dialogue-text';overlay.setAttribute('aria-label','原版剧情对话');canvas.parentElement.append(overlay);}
    if(overlay.width!==canvas.width||overlay.height!==canvas.height){overlay.width=canvas.width;overlay.height=canvas.height;}
    const context=overlay.getContext('2d');context.clearRect(0,0,overlay.width,overlay.height);
    const spell=world.spell;overlay.hidden=(!message?.active||!message.lines?.length)&&!spell?.pose&&!spell?.active&&!spell?.resultFrames;
    if(!overlay.hidden){context.setTransform(overlay.width/640,0,0,overlay.height/480,0,0);context.lineJoin='round';context.textBaseline='top';
      for(const line of messageTextLayout(message,find('#variant').value,{nativeRasters:find('#variant').value==='zh'})){if(find('#variant').value==='zh'){drawOriginalStageText(context,state.stageTextRasters,{...line,kind:line.rasterKind,rawHex:line.rawHex});}else{context.save();context.translate(line.x,line.y);context.scale(line.sx,line.sy);context.textAlign=line.align;context.font=`400 ${line.font}px ${originalStageTextFamily(find('#variant').value)}`;context.lineWidth=3;context.strokeStyle='#101018';context.fillStyle='#fff';context.globalAlpha=line.outlineAlpha;if(line.outlineAlpha>0)context.strokeText(line.text,0,0);context.globalAlpha=line.alpha;if(line.alpha>0)context.fillText(line.text,0,0);context.restore();}}
      if(spell?.pose&&spell.title?.length){
        const pose=spell.pose,title=new TextDecoder(find('#variant').value==='ja'?'shift_jis':'gbk',{fatal:true}).decode(Uint8Array.from(spell.title));
        if(find('#variant').value==='zh'){drawOriginalStageText(context,state.stageTextRasters,{kind:'spell',rawHex:spell.title.map(value=>value.toString(16).padStart(2,'0')).join(''),x:pose.x,y:pose.y,sx:pose.sx,sy:pose.sy,alpha:(pose.color>>>24)/255,outlineAlpha:(pose.color>>>24)/255,align:'right'});}else{context.save();context.translate(pose.x,pose.y);context.scale(pose.sx,pose.sy);context.globalAlpha=(pose.color>>>24)/255;context.font='18px "Microsoft YaHei","Yu Gothic",sans-serif';context.textAlign='right';context.lineWidth=3;context.strokeStyle='#101018';context.fillStyle='#fff';context.strokeText(title,0,0);context.fillText(title,0,0);context.restore();}
      }
      context.resetTransform();
    }
    syncGameplayMusicState(message);
    if(!state.replayPlayback)state.renderer.renderOriginalTerminalFade(world,overlay.hidden?null:context);
    return records.length;
  },
  state:()=>{
    const flow=coreFlowEvidence(),native=nativeMenuSnapshot();
    return {frame:state.module?._th128_frame(),nodes:state.module?._th128_nodes(),selection:state.module?._th128_selection(),error:coreError(),paused:state.paused,worldMode:state.worldMode,
      progress:state.stageSelection?.progress??0,autoFlow:state.autoFlow,handoff:state.handoff,pendingCarryToken:state.pendingCarryToken,
      handoffFailed:state.handoffFailed===true,handoffError:state.handoffFailed?(lastCheckWorldError||menu.faultReason):null,
      stageId:flow?.currentId??null,difficulty:flow?.difficulty??null,shotDifficulty:flow?.shotDifficulty??null,
      playerShotsConfigured:flow?.playerShotsConfigured??null,flow,debug:flow?.debug??null,
      debugStageJump:state.worldMode?state.stageSelection?.debugJump??null:null,
      debugInfiniteRespawnRequested:state.debugInfiniteRespawnRequested===true,
      menuPage:menu.page,nativeMainIndex:menu.main,menuPreselect:{main:menu.main,difficulty:menu.difficulty,route:menu.route},
      domMenuVisible:domMenuVisible(),nativeMenu:native,nativePage:native.page,nativeReady:native.ready,nativeTransitioning:native.transitioning,
      renderOutput:{width:find('#game').width,height:find('#game').height,logicalWidth:640,logicalHeight:480},
      stageMusic:state.stageMusic?.snapshot()??null,audioState:state.audio?.state??null,background:state.background?.snapshot()??null};
  },
  nativeMenu:()=>nativeMenuSnapshot(),
  nativeMenuProtocol:()=>nativeMenuSnapshot(),
  replayWorldInputs:(rows,options={})=>{
    if(options===null||typeof options!=='object'||Array.isArray(options))throw new Error('Invalid replay input options');
    const initialHolding=options.initialHolding===undefined?0:options.initialHolding;
    const releaseAtEnd=options.releaseAtEnd===undefined?true:options.releaseAtEnd;
    const inputMask=gameplayInputMask();
    if(!Number.isInteger(initialHolding)||initialHolding<0||initialHolding>inputMask||(initialHolding&~inputMask)!==0)
      throw new Error('Invalid replay initial holding');
    if(typeof releaseAtEnd!=='boolean')throw new Error('Invalid replay release flag');
    if(!state.worldMode||!Array.isArray(rows)||rows.length>2000||rows.some(row=>!Array.isArray(row)||row.length!==3||!Number.isInteger(row[0])||row[0]<1||row[0]>4||!Number.isInteger(row[1])||row[1]<0||(row[1]&~inputMask)!==0||![0,1].includes(row[2]))||rows.reduce((total,row)=>total+row[0],0)>6000)throw new Error('Invalid recorded diagnostic input range');
    assertSynchronousWorldDrainAllowed();
    state.paused=true;clock.reset();resetHighFrameratePresentation();let previous=initialHolding;
    for(const [ticks,holding,confirm] of rows){
      if(!state.module._th128_world_input(holding,(holding&~previous)|confirm,(previous&~holding)|confirm))break;
      previous=holding;
      for(let tick=0;tick<ticks;++tick){if(!state.module._th128_world_tick())break;state.background?.tick();drainWorld();}
      if(state.module.UTF8ToString(state.module._th128_world_error()))break;
    }
    let lastHolding=previous;
    if(releaseAtEnd&&state.module._th128_world_input(0,0,previous))lastHolding=0;
    state.pressed.clear();
    const snapshot=JSON.parse(state.module.UTF8ToString(state.module._th128_world_state()));
    state.worldSnapshot=snapshot;
    syncDebugRespawnUI(snapshot);
    return {snapshot,error:state.module.UTF8ToString(state.module._th128_world_error()),lastHolding};
  },
  advanceWorld:(ticks,{present=true}={})=>{
    if(!state.worldMode||!Number.isInteger(ticks)||ticks<0||ticks>6000||typeof present!=='boolean')throw new Error('Invalid world diagnostic tick count');
    assertSynchronousWorldDrainAllowed();
    state.paused=true;clock.reset();resetHighFrameratePresentation();
    for(let step=0;step<ticks;++step){if(!state.module._th128_world_tick())break;state.background?.tick();drainWorld();}
    if(present)draw();
    const snapshot=JSON.parse(state.module.UTF8ToString(state.module._th128_world_state()));
    state.worldSnapshot=snapshot;
    syncDebugRespawnUI(snapshot);
    if(present&&state.autoFlow)maybeAutoHandoff();
    return {snapshot,error:state.module.UTF8ToString(state.module._th128_world_error())};
  },
  advance:frames=>{
    if(!Number.isInteger(frames)||frames<0||frames>6000)throw new Error('Invalid diagnostic frame count');
    assertSynchronousWorldDrainAllowed();
    state.paused=true;clock.reset();resetHighFrameratePresentation();for(let count=0;count<frames;count++)advanceSerial();draw();if(state.autoFlow)maybeAutoHandoff();return window.th128Debug.state();
  },
  setAutoFlow:enabled=>{
    if(typeof enabled!=='boolean')throw new Error('Invalid auto-flow setting');
    if(enabled&&!flowCapabilitiesReady()){state.autoFlow=false;return false;}
    state.autoFlow=enabled;if(enabled)maybeAutoHandoff();return state.autoFlow;
  },
};

window.th128Debug.prepareStage=prepareStage;
window.th128Debug.startStage=startStage;
window.th128Debug.startDebugThirdStage=startDebugThirdStage;
window.th128Debug.returnToTitle=returnToTitle;
state.extraMenu=mountExtraMenu(document);
window.th128Debug.checkpointStartNormalA=checkpointBeginNormalA;
window.th128Debug.checkpoints=installCheckpointAdapter({
  debug:window.th128Debug,
  currentGeneration:checkpointCoreGeneration,
  beginOriginalRoute:checkpointBeginNormalA,
  indexUrl:'/assets/generations/834d7d98ff800aff24972ad0f243878c/zh/animations/checkpoint-index.json',
  indexIntegrity:{bytes:8475,sha256:'a3bbfc948b621e5c3410aa61e51d9820fb71248b3b159542fec525b9df3238f9'},
});

try{
  state.catalog=await readJson('/assets/catalog.json',launcherContext?.signal);launcherContext?.signal.throwIfAborted();
  const selector=find('#variant');selector.replaceChildren(...Object.entries(state.catalog.variants).map(([key,record])=>{
    const option=document.createElement('option');option.value=key;option.textContent=record.title;return option;
  }));
  selector.disabled=false;
  const preferredVariant=preferenceText('variant','');
  selector.value=state.catalog.variants[preferredVariant]?preferredVariant:(state.catalog.variants.zh?'zh':Object.keys(state.catalog.variants)[0]);
  if(launcherContext?.config.language){
    const variant=launcherContext.config.language;if(!state.catalog.variants[variant])throw new Error('Requested runtime language is unavailable');selector.value=variant;
  }
  await loadVariant(selector.value);finishLauncherStartup();
}catch(problem){launcherContext?.fail(problem);error(problem);}
}

initialize().catch(problem=>{getPageLaunchContext()?.fail(problem);startupFailure(problem);});

(() => {
  'use strict';
const REAL_CHECKPOINT_BRIDGE = 'real-checkpoint-panel-bridge/1';

function validVersion(driver) {
  const version = driver?.contractVersion ?? driver?.version;
  return version === 1 || version === '1' || version === 'th128-debug-checkpoints/1';
}

function numberOrNull(value) {
  return Number.isInteger(value) && value >= 0 ? value : null;
}

function targetFor(entry) {
  const stageId = entry?.target?.stageId ?? entry?.targetStage;
  const frame = numberOrNull(entry?.target?.frame);
  const bounds = entry?.target?.bounds ?? entry?.bounds ?? null;
  if (!Number.isInteger(stageId) || stageId < 1 || stageId > 16) return null;
  return {stageId, frame, frameKnown: frame !== null, bounds};
}

function readNative(debug) {
  try {
    const state = debug?.state?.();
    const nativeTainted = state?.worldMode === true
      && state?.debug?.infiniteRespawn === true
      && state?.debug?.runTainted === true
      && state?.debug?.sessionEnabled === true;
    return {nativeTainted, worldMode: state?.worldMode === true,
      debugInfiniteRespawnRequested: state?.debugInfiniteRespawnRequested === true};
  } catch {
    return {nativeTainted: false, worldMode: false, debugInfiniteRespawnRequested: false};
  }
}

async function waitRegistry(driver) {
  const ready = driver?.ready ?? driver?.indexReady ?? driver?.indexLoaded;
  if (ready === false) throw new Error('checkpoint registry is not ready');
  if (ready && typeof ready.then === 'function') {
    const value = await ready;
    if (value === false) throw new Error('checkpoint registry is not ready');
  }
}

function createCheckpointPanelDriver({driver, debug, requestedOptIn = () => false, registryReady = null} = {}) {
  if (!driver || typeof driver.list !== 'function' || typeof driver.status !== 'function'
      || typeof driver.restore !== 'function' || typeof driver.cancel !== 'function'
      || typeof driver.step !== 'function') throw new Error('real checkpoint driver is incomplete');
  if (!validVersion(driver)) throw new Error('real checkpoint driver contract version is invalid');
  if (!debug || typeof debug.state !== 'function') throw new Error('native debug state reader is required');
  const requestedFallback = () => {
    try {return requestedOptIn() === true;} catch {return false;}
  };

  return {
    version: 1,
    async list() {
      if (registryReady) await registryReady;
      await waitRegistry(driver);
      const raw = await driver.list();
      const entries = Array.isArray(raw) ? raw : raw?.bundles;
      if (!Array.isArray(entries) || entries.length === 0) throw new Error('checkpoint registry is not loaded');
      return {contractVersion: 1, bundles: entries.map(entry => {
        const target = targetFor(entry);
        if (!target) throw new Error('checkpoint registry descriptor target is invalid');
        return {id: entry.id, title: entry.title ?? entry.id, source: entry.source ?? null,
          origin: entry.origin ?? null, provenance: entry.provenance ?? entry.source ?? null,
          targetStage: target.stageId, target: {stageId: target.stageId, frame: target.frame, bounds: target.bounds},
          bounds: target.bounds, frameKnown: target.frameKnown,
          guards: entry.guards ?? {normalModeClear: false}};
      })};
    },
    async status() {
      const raw = await driver.status();
      const native = readNative(debug);
      const requestedValue = raw?.requestedOptIn === true || (raw?.requestedOptIn === undefined && requestedFallback());
      const nativeTainted = raw?.nativeTainted === true;
      const originNativeProof = raw?.originNativeProof === true;
      return {...raw, requestedOptIn: requestedValue, nativeTainted, originNativeProof,
        debugEnabled: nativeTainted,
        debug: raw?.debug ?? {requested: requestedValue, nativeTainted, originNativeProof,
          worldMode: native.worldMode, nativeInfiniteRequested: native.debugInfiniteRespawnRequested},
        debugTaint: raw?.debugTaint ?? {requested: requestedValue, nativeTainted, originNativeProof,
          nativeInfiniteRequested: native.debugInfiniteRespawnRequested}};
    },
    async restore(bundleId, options = {}) {
      const raw = await driver.status();
      const requestedValue = raw?.requestedOptIn === true
        || (raw?.requestedOptIn === undefined && requestedFallback());
      if (!requestedValue) {
        const error = new Error('debug requested opt-in is off');
        error.code = 'debug-requested-off';
        throw error;
      }
      return driver.restore(bundleId, options);
    },
    async cancel(reason = 'panel-cancel') {
      return driver.cancel(reason);
    },
    async step(ticks = 1) {
      const raw = await driver.status();
      if (raw?.phase !== 'ready' || raw?.nativeTainted !== true) {
        const error = new Error('checkpoint step requires ready and native taint');
        error.code = 'step-gate';
        throw error;
      }
      return driver.step(ticks);
    }
  };
}
const PANEL_CONTRACT = 'th128-debug-checkpoints/1';
const PANEL_STATUS = 'DEBUG_CHECKPOINT_PANEL';

const PHASES = new Set(['idle', 'loading', 'replaying', 'handoff', 'ready', 'cancelled', 'fault']);
const VERSIONS = new Set([1, '1', PANEL_CONTRACT]);

function contractVersion(driver) {
  const version = driver?.contractVersion ?? driver?.version;
  if (!VERSIONS.has(version)) return null;
  return PANEL_CONTRACT;
}

function phaseOf(status) {
  return typeof status?.phase === 'string' ? status.phase : null;
}

function stageOf(status) {
  const value = status?.stageId ?? status?.stage?.id;
  return Number.isInteger(value) ? value : null;
}

function frameOf(status) {
  const value = status?.frame ?? status?.stage?.frame;
  return Number.isInteger(value) && value >= 0 ? value : null;
}

function debugEnabled(status) {
  return status?.debugEnabled === true || status?.debug?.enabled === true || status?.taint?.debug === true;
}

function normalizeBundle(bundle) {
  if (!bundle || typeof bundle !== 'object') return null;
  if (typeof bundle.id !== 'string' || bundle.id.length === 0) return null;
  const stageId = bundle.target?.stageId ?? bundle.targetStage;
  const frame = bundle.target?.frame;
  if (!Number.isInteger(stageId) || stageId < 1 || stageId > 16) return null;
  if (frame !== undefined && frame !== null && (!Number.isInteger(frame) || frame < 0)) return null;
  if (bundle.guards?.normalModeClear !== false) return null;
  const bounds = bundle.target?.bounds ?? bundle.bounds ?? null;
  return Object.freeze({id: bundle.id, title: typeof bundle.title === 'string' ? bundle.title : bundle.id,
    source: bundle.source ?? null, origin: bundle.origin ?? null,
    target: {stageId, frame: Number.isInteger(frame) ? frame : null, bounds},
    framePublished: Number.isInteger(frame), bounds,
    provenance: bundle.provenance ?? bundle.source ?? null, raw: bundle});
}

function normalizeList(result, driverVersion) {
  if (!driverVersion) return {ok: false, reason: 'driver contract version missing'};
  const bundles = Array.isArray(result) ? result : result?.bundles;
  if (!Array.isArray(bundles) || bundles.length === 0) return {ok: false, reason: 'checkpoint bundle list empty or malformed'};
  const normalized = [];
  for (const bundle of bundles) {
    const item = normalizeBundle(bundle);
    if (!item) return {ok: false, reason: 'checkpoint bundle is malformed'};
    normalized.push(item);
  }
  return {ok: true, bundles: normalized};
}

function normalizeStatus(raw) {
  if (!raw || typeof raw !== 'object') return {ok: false, reason: 'driver status malformed'};
  if (raw.normalModeClear !== false) return {ok: false, reason: 'normalModeClear must be false'};
  const phase = phaseOf(raw);
  if (!phase || !PHASES.has(phase)) return {ok: false, reason: 'driver phase invalid'};
  const requestedOptIn = raw.requestedOptIn === true || raw.debug?.requested === true;
  const nativeTainted = raw.nativeTainted === true || raw.debug?.nativeTainted === true;
  const originNativeProof = raw.originNativeProof === true || raw.debug?.originNativeProof === true;
  return {ok: true, status: {
    phase,
    stageId: stageOf(raw),
    frame: frameOf(raw),
    generation: raw.generation ?? null,
    elapsedMs: Number.isFinite(raw.elapsedMs) ? raw.elapsedMs : null,
    requestedOptIn,
    nativeTainted,
    originNativeProof,
    debugEnabled: nativeTainted,
    debugTaint: raw.debugTaint ?? raw.taint ?? raw.debug ?? null,
    normalModeClear: false,
    core: raw.core ?? raw.generation ?? null,
    provenance: raw.provenance ?? null,
    bundleId: raw.bundleId ?? null
  }};
}

class DebugCheckpointPanel {
  constructor({elements, driver = null, windowRef = globalThis, documentRef = null, refreshHandler = null} = {}) {
    if (!elements) throw new Error('panel elements are required');
    this.elements = elements;
    this.driver = driver;
    this.windowRef = windowRef;
    this.documentRef = documentRef;
    this.refreshHandler = refreshHandler;
    this.destroyed = false;
    this.inflight = false;
    this.generation = 0;
    this.bundles = [];
    this.ready = false;
    this.requestedOptIn = false;
    this.nativeTainted = false;
    this.abortController = null;
    this.pageHide = () => this.handlePageHide();
    this.handlers = [];
  }

  bind(element, type, handler) {
    if (!element?.addEventListener) return;
    element.addEventListener(type, handler);
    this.handlers.push({element, type, handler});
  }

  async initialize() {
    if (this.refreshHandler) this.bind(this.elements.refresh, 'click', this.refreshHandler);
    this.bind(this.elements.restore, 'click', () => this.restore());
    this.bind(this.elements.cancel, 'click', () => this.cancel('panel-cancel'));
    this.bind(this.elements.step, 'click', () => this.step());
    this.bind(this.elements.select, 'change', () => this.render());
    this.windowRef?.addEventListener?.('pagehide', this.pageHide);
    this.renderUnavailable('driver unavailable or not loaded');
    return this;
  }

  useDriver(driver) {
    if (this.destroyed) return;
    this.driver = driver ?? null;
    if (!this.driver) this.renderUnavailable('driver unavailable or not loaded');
    else this.render();
    return this;
  }

  setBusy(busy) {
    this.inflight = busy;
    this.render();
  }

  async refresh() {
    if (this.destroyed) return false;
    const driver = this.driver;
    if (!driver) return this.renderUnavailable('driver unavailable or not loaded');
    const version = contractVersion(driver);
    if (!version) return this.renderUnavailable('driver contract version unavailable');
    if (typeof driver.list !== 'function' || typeof driver.status !== 'function')
      return this.renderUnavailable('driver list/status missing');
    try {
      const result = await driver.list();
      if (this.destroyed) return false;
      const normalized = normalizeList(result, version);
      if (!normalized.ok) return this.renderUnavailable(normalized.reason);
      this.bundles = normalized.bundles;
      this.populate();
      this.render();
      await this.refreshStatus();
      return true;
    } catch (error) {
      return this.renderUnavailable(error?.message ?? String(error));
    }
  }

  populate() {
    const select = this.elements.select;
    if (!select) return;
    while (select.firstChild) select.removeChild(select.firstChild);
    for (const bundle of this.bundles) {
      const option = this.documentRef?.createElement?.('option') ?? {value: '', textContent: ''};
      option.value = bundle.id;
      option.textContent = `${bundle.title} · stage${bundle.target.stageId}/frame${bundle.target.frame}`;
      select.appendChild(option);
    }
    if (this.bundles[0]) select.value = this.bundles[0].id;
  }

  selected() {
    const id = this.elements.select?.value;
    return this.bundles.find(bundle => bundle.id === id) ?? null;
  }

  async refreshStatus() {
    if (!this.driver || typeof this.driver.status !== 'function') return this.renderUnavailable('driver status unavailable');
    try {
      const raw = await this.driver.status();
      const normalized = normalizeStatus(raw);
      if (!normalized.ok) return this.renderUnavailable(normalized.reason);
      this.status = normalized.status;
      this.ready = normalized.status.phase === 'ready';
      this.requestedOptIn = normalized.status.requestedOptIn;
      this.nativeTainted = normalized.status.phase === 'ready' && normalized.status.nativeTainted;
      this.render();
      return normalized.status;
    } catch (error) {
      return this.renderUnavailable(error?.message ?? String(error));
    }
  }

  async restore() {
    const bundle = this.selected();
    if (!bundle || this.inflight || !this.driver || !this.requestedOptIn || typeof this.driver.restore !== 'function') return false;
    const token = ++this.generation;
    this.abortController = new AbortController();
    this.ready = false;
    this.setBusy(true);
    this.setText('loading', 'loading checkpoint input replay');
    const onProgress = progress => {
      if (token !== this.generation || this.destroyed) return;
      this.setText('status', `replaying ${Number(progress?.ticks ?? 0)} ticks · frame ${progress?.frame ?? '?'}`);
    };
    try {
      const result = await this.driver.restore(bundle.id, {signal: this.abortController.signal, onProgress});
      if (token !== this.generation || this.destroyed) return false;
      const status = await this.driver.status();
      if (token !== this.generation || this.destroyed) return false;
      const normalized = normalizeStatus(status);
      if (!normalized.ok) return this.fail(normalized.reason);
      if (normalized.status.phase !== 'ready') return this.fail(`restore did not reach ready: ${normalized.status.phase}`);
      if (normalized.status.stageId !== bundle.target.stageId) return this.fail('restore status does not match bundle target stage');
      if (bundle.target.frame !== null && normalized.status.frame !== bundle.target.frame)
        return this.fail('restore status does not match bundle target frame');
      const bounds = bundle.bounds;
      if (bounds && Number.isInteger(normalized.status.frame)) {
        if (Number.isInteger(bounds.minFrame) && normalized.status.frame < bounds.minFrame)
          return this.fail('restore frame is below bundle bounds');
        if (Number.isInteger(bounds.maxFrame) && normalized.status.frame > bounds.maxFrame)
          return this.fail('restore frame is above bundle bounds');
      }
      this.status = normalized.status;
      this.ready = true;
      this.requestedOptIn = normalized.status.requestedOptIn;
      this.nativeTainted = normalized.status.nativeTainted;
      this.setBusy(false);
      this.render();
      return true;
    } catch (error) {
      if (token !== this.generation || this.destroyed || error?.name === 'AbortError')
        return false;
      return this.fail(error?.message ?? String(error));
    } finally {
      if (token === this.generation) this.setBusy(false);
    }
  }

  async cancel(reason = 'panel-cancel') {
    if (this.destroyed || !this.driver || typeof this.driver.cancel !== 'function') return false;
    const token = ++this.generation;
    this.ready = false;
    try {this.abortController?.abort();} catch {}
    this.abortController = null;
    try {
      await this.driver.cancel(reason);
      if (token !== this.generation || this.destroyed) return false;
      await this.refreshStatus();
      return true;
    } catch (error) {
      if (token !== this.generation || this.destroyed) return false;
      return this.fail(error?.message ?? String(error));
    } finally {
      if (token === this.generation) this.setBusy(false);
    }
  }

  async step() {
    if (!this.ready || !this.nativeTainted || this.inflight || !this.driver || typeof this.driver.step !== 'function') return false;
    const token = this.generation;
    try {
      await this.driver.step(1);
      if (token !== this.generation || this.destroyed) return false;
      await this.refreshStatus();
      return true;
    } catch (error) {
      if (token !== this.generation || this.destroyed) return false;
      return this.fail(error?.message ?? String(error));
    }
  }

  fail(message) {
    this.ready = false;
    this.status = null;
    this.setBusy(false);
    this.setText('status', `error: ${message}`);
    this.render();
    return false;
  }

  setText(key, text) {
    if (this.elements[key]) this.elements[key].textContent = text;
  }

  renderUnavailable(reason) {
    this.ready = false;
    this.requestedOptIn = false;
    this.nativeTainted = false;
    this.bundles = [];
    this.status = null;
    this.setBusy(false);
    this.setText('status', `unavailable: ${reason}`);
    this.setText('provenance', 'No checkpoint authority. No restore can be started.');
    this.render();
    return false;
  }

  render() {
    if (!this.elements) return;
    const busy = this.inflight;
    const driverReady = Boolean(this.driver) && !this.destroyed;
    this.elements.refresh.disabled = this.destroyed;
    this.elements.select.disabled = this.destroyed || this.bundles.length === 0 || busy || !this.requestedOptIn;
    this.elements.restore.disabled = this.destroyed || !driverReady || !this.requestedOptIn || busy || !this.selected();
    this.elements.cancel.disabled = this.destroyed || !driverReady || !busy;
    this.elements.step.disabled = this.destroyed || !driverReady || !this.nativeTainted || busy || !this.ready;
    if (!this.status) return;
    const status = this.status;
    this.setText('status', `${status.phase} · stage${status.stageId ?? '?'} · frame${status.frame ?? '?'}`
      + ` · ${status.elapsedMs === null ? 'elapsed?' : `${status.elapsedMs.toFixed(1)}ms`}`
      + ` · requested-optin=${status.requestedOptIn ? 'yes' : 'no'}`
      + ` · native-tainted=${status.nativeTainted ? 'yes' : 'no'}`
      + ` · origin-native-proof=${status.originNativeProof ? 'yes' : 'no'} · normalModeClear=false`);
    const provenance = [status.core && `core=${status.core}`, status.generation && `generation=${status.generation}`,
      status.provenance && `provenance=${typeof status.provenance === 'string' ? status.provenance : JSON.stringify(status.provenance)}`]
      .filter(Boolean).join(' · ');
    this.setText('provenance', provenance || 'provenance unavailable');
  }

  handlePageHide() {
    if (this.destroyed) return;
    const active = this.inflight;
    this.generation += 1;
    try {this.abortController?.abort();} catch {}
    this.abortController = null;
    if (active && this.driver && typeof this.driver.cancel === 'function') {
      try {Promise.resolve(this.driver.cancel('pagehide')).catch(() => {});} catch {}
    }
    this.inflight = false;
    this.destroy();
  }

  destroy() {
    if (this.destroyed) return;
    this.destroyed = true;
    this.ready = false;
    this.generation += 1;
    try {this.abortController?.abort();} catch {}
    this.abortController = null;
    this.windowRef?.removeEventListener?.('pagehide', this.pageHide);
    for (const {element, type, handler} of this.handlers) {
      try {element.removeEventListener?.(type, handler);} catch {}
    }
    this.handlers = [];
    this.elements.restore.disabled = true;
    this.elements.cancel.disabled = true;
    this.elements.step.disabled = true;
    this.elements.refresh.disabled = true;
    this.elements.select.disabled = true;
    this.setText('status', 'panel disposed');
  }
}
const IDS = {
  select: 'checkpoint-select',
  refresh: 'checkpoint-refresh',
  restore: 'checkpoint-restore',
  cancel: 'checkpoint-cancel',
  step: 'checkpoint-step',
  status: 'checkpoint-status',
  provenance: 'checkpoint-provenance'
};

function elementsFromDocument(documentRef = document) {
  return Object.fromEntries(Object.entries(IDS).map(([key, id]) => [key, documentRef.getElementById(id)]));
}

function requestedFrom(documentRef) {
  return documentRef?.getElementById?.('debug-infinite-respawn')?.checked === true;
}

function adapt(raw, debug, documentRef, registryReady) {
  if (!raw) return null;
  return createCheckpointPanelDriver({driver: raw, debug, requestedOptIn: () => requestedFrom(documentRef), registryReady});
}

async function mountPanel({documentRef = document, windowRef = window, driver = windowRef.th128Debug?.checkpoints,
  debug = windowRef.th128Debug, registryReady = null} = {}) {
  const elements = elementsFromDocument(documentRef);
  if (Object.values(elements).some(element => !element)) throw new Error('checkpoint panel markup is incomplete');
  let raw = driver;
  let adapted = adapt(raw, debug, documentRef, registryReady);
  let panel = null;
  const refreshHandler = () => {
    raw = windowRef.th128Debug?.checkpoints;
    adapted = adapt(raw, windowRef.th128Debug, documentRef, registryReady);
    panel.useDriver(adapted);
    panel.refresh();
  };
  panel = new DebugCheckpointPanel({elements, driver: adapted, windowRef, documentRef, refreshHandler});
  await panel.initialize();
  if (!adapted) panel.renderUnavailable('driver unavailable or not loaded');
  else await panel.refresh();
  return {panel, elements, contractStatus: PANEL_STATUS, get driver() {return adapted;}};
}

if (typeof document !== 'undefined') {
  const start = () => mountPanel().catch(error => {
    const status = document.getElementById('checkpoint-status');
    if (status) status.textContent = `panel initialization failed: ${error.message}`;
  });
  if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', start, {once: true});
  else start();
}
})();
