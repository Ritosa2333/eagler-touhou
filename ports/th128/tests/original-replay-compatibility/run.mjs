#!/usr/bin/env node
// MAIN617 snapshot of the independently reviewed revision3, with source-proven inert-record checks.
// Revision 3 executable runner: original authorized user replay, A1 stage 2 / rank 3 / 23707 frames,
// actual core100 native replay playback. No browser, GPU, WebAudio device, IndexedDB writer or compiler.
//
//   node run.mjs <request.json> --verify-inputs        (pins + codec + source ABI audit; no native factory)
//   node run.mjs <request.json> --run --grant <grant.json>   (requires a real MAIN grant object)
//
// The v1 packet in the parent folder stays frozen; this folder is the corrected packet.
import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import { pathToFileURL } from 'node:url';

const HERE = path.dirname(new URL(import.meta.url).pathname.replace(/^\/([A-Za-z]:)/, '$1'));
const sha256 = (bytes) => crypto.createHash('sha256').update(bytes).digest('hex');
const read = (p) => new Uint8Array(fs.readFileSync(p));
const enc = new TextEncoder();

// Exact signature expectations audited against the pinned core100 runtime/WorldExports.cpp.
// ret + parameter TYPE sequence (names/whitespace ignored, order enforced).
const SOURCE_SIGNATURES = {
  reset: { ret: 'void', params: [] },
  error: { ret: 'const char*', params: [] },
  load: { ret: 'int', params: ['const char*', 'std::uint32_t', 'const void*', 'std::uint32_t'] },
  start: { ret: 'int', params: ['const char*', 'std::uint32_t', 'int', 'unsigned'] },
  load_stage_intro: { ret: 'int', params: ['int', 'const char*', 'std::uint32_t', 'const void*', 'std::uint32_t'] },
  track_caption: { ret: 'int', params: ['int', 'int'] },
  stage_intro_state: { ret: 'const char*', params: [] },
  enable_geometry526: { ret: 'int', params: ['const void*', 'std::uint32_t'] },
  load_enemy_anm: { ret: 'int', params: ['int', 'const void*', 'std::uint32_t'] },
  enable_bullets: { ret: 'int', params: ['const void*', 'std::uint32_t', 'const void*', 'std::uint32_t', 'float', 'float'] },
  enable_bullet_updates: { ret: 'int', params: ['const void*', 'std::uint32_t'] },
  enable_graze: { ret: 'int', params: ['int', 'int', 'unsigned'] },
  enable_player_context: { ret: 'int', params: ['const void*', 'std::uint32_t', 'int', 'unsigned', 'int', 'unsigned', 'unsigned', 'int', 'int', 'int', 'int'] },
  enable_player_motion: { ret: 'int', params: ['int', 'int'] },
  set_debug_infinite_respawn: { ret: 'int', params: ['int'] },
  enable_messages: { ret: 'int', params: ['const void*', 'unsigned', 'const void*', 'unsigned', 'const void*', 'unsigned', 'const void*', 'unsigned'] },
  enable_message_front: { ret: 'int', params: ['const void*', 'unsigned'] },
  enable_stage_transition: { ret: 'int', params: ['const void*', 'unsigned', 'int', 'int', 'int', 'int', 'int', 'int'] },
  add_bullet_profile: { ret: 'int', params: ['int', 'const void*', 'unsigned'] },
  enable_player_shots: { ret: 'int', params: ['const void*', 'std::uint32_t', 'const void*', 'std::uint32_t', 'const void*', 'std::uint32_t', 'float', 'int', 'unsigned', 'unsigned', 'unsigned', 'unsigned'] },
  enable_shot_hits: { ret: 'int', params: ['int', 'int', 'int', 'int', 'unsigned'] },
  enable_rapid_shot: { ret: 'int', params: ['int'] },
  enable_player_death: { ret: 'int', params: ['int'] },
  enable_perfect_hud: { ret: 'int', params: ['const void*', 'unsigned'] },
  pause_load_manual: { ret: 'int', params: ['const void*', 'unsigned'] },
  enable_player_respawn: { ret: 'int', params: ['int', 'int', 'const void*', 'unsigned'] },
  load_score_font: { ret: 'int', params: ['const void*', 'unsigned'] },
  enable_spells: { ret: 'int', params: ['const void*', 'unsigned', 'const void*', 'unsigned', 'const void*', 'unsigned', 'const void*', 'unsigned', 'const void*', 'unsigned', 'const void*', 'unsigned'] },
  enable_head_power: { ret: 'int', params: [] },
  enable_z_fire: { ret: 'int', params: ['const void*', 'unsigned', 'const void*', 'unsigned'] },
  enable_focus_indicator: { ret: 'int', params: [] },
  replay_playback_restore: { ret: 'int', params: ['const std::uint8_t*', 'unsigned', 'int'] },
  replay_playback_load: { ret: 'int', params: ['const void*', 'unsigned', 'int'] },
  replay_playback_state: { ret: 'const char*', params: [] },
  replay_playback_cancel: { ret: 'int', params: [] },
  tick: { ret: 'int', params: [] },
  state: { ret: 'const char*', params: [] },
  sound_epoch: { ret: 'unsigned', params: [] },
  sound_base_low: { ret: 'unsigned', params: [] },
  sound_base_high: { ret: 'unsigned', params: [] },
  sound_begin_window: { ret: 'int', params: [] },
  sound_window_serial: { ret: 'unsigned', params: [] },
  sound_window_count: { ret: 'int', params: [] },
  sound_window_state: { ret: 'const char*', params: [] },
  sound_id: { ret: 'int', params: ['unsigned'] },
  sound_x: { ret: 'float', params: ['unsigned'] },
  sound_operation: { ret: 'int', params: ['unsigned'] },
  sound_ack_window: { ret: 'int', params: ['unsigned', 'unsigned', 'unsigned', 'unsigned', 'unsigned'] },
  music_control_state: { ret: 'const char*', params: [] },
  acknowledge_music_control: { ret: 'int', params: ['unsigned'] },
};


// B1 repair: source-signature inventory vs required callable inventory.
// The four names below are DEFINED in WorldExports.cpp but are NOT exported by core100 and are
// never called by production 609 or this runner; they stay in the source-signature inventory only.
const SOURCE_ONLY = ['sound_base_low', 'sound_base_high', 'sound_window_serial', 'sound_window_count'];
// Exactly the names this runner invokes through api()/jsonCall()/callBytes(). deriveInvokedNames() re-parses
// this file at verification time and requires set equality, so a new call site cannot escape the
// callability check. Every name must exist in the pinned core100 request.json exports list.
const REQUIRED_CALLABLE = [
  'acknowledge_music_control',
  'add_bullet_profile',
  'enable_bullet_updates',
  'enable_bullets',
  'enable_focus_indicator',
  'enable_geometry526',
  'enable_graze',
  'enable_head_power',
  'enable_message_front',
  'enable_messages',
  'enable_perfect_hud',
  'enable_player_context',
  'enable_player_death',
  'enable_player_motion',
  'enable_player_respawn',
  'enable_player_shots',
  'enable_rapid_shot',
  'enable_shot_hits',
  'enable_spells',
  'enable_stage_transition',
  'enable_z_fire',
  'error',
  'load',
  'load_enemy_anm',
  'load_score_font',
  'load_stage_intro',
  'music_control_state',
  'pause_load_manual',
  'replay_playback_load',
  'replay_playback_restore',
  'replay_playback_state',
  'reset',
  'set_debug_infinite_respawn',
  'sound_ack_window',
  'sound_begin_window',
  'sound_epoch',
  'sound_id',
  'sound_operation',
  'sound_window_state',
  'sound_x',
  'stage_intro_state',
  'start',
  'state',
  'tick',
  'track_caption',
];
function deriveInvokedNames() {
  const source = fs.readFileSync(path.join(HERE, 'run.mjs'), 'utf8');
  return [...new Set([...source.matchAll(/(?:api|jsonCall|callBytes)\(\s*'([a-z0-9_]+)'/g)].map((m) => m[1]))].sort();
}
function callabilityAudit(exportList) {
  const exported = new Set(exportList);
  const derived = deriveInvokedNames();
  const required = [...REQUIRED_CALLABLE].sort();
  const missingExports = required.filter((name) => !exported.has('_th128_world_' + name));
  const missingFromRequired = derived.filter((name) => !required.includes(name));
  const extraRequired = required.filter((name) => !derived.includes(name));
  const sourceOnlyExported = SOURCE_ONLY.filter((name) => exported.has('_th128_world_' + name));
  return {
    exportCount: exportList.length,
    exportCountIs205: exportList.length === 205,
    requiredCount: required.length,
    derivedCount: derived.length,
    invokedMatches: missingFromRequired.length === 0 && extraRequired.length === 0,
    missingExports,
    sourceOnlyExported,
    missingFromRequired,
    extraRequired,
    mallocExported: exported.has('_malloc'),
    freeExported: exported.has('_free'),
    errorExported: exported.has('_th128_world_error'),
  };
}

function splitParams(text) {
  const out = [];
  let depth = 0;
  let start = 0;
  for (let i = 0; i < text.length; i++) {
    const c = text[i];
    if (c === '(' || c === '<') depth++;
    else if (c === ')' || c === '>') depth--;
    else if (c === ',' && depth === 0) { out.push(text.slice(start, i)); start = i + 1; }
  }
  const tail = text.slice(start).trim();
  if (tail.length) out.push(tail);
  return out.map((raw) => {
    let p = raw.replace(/\s+/g, ' ').trim();
    if (!p) return '';
    if (!p.endsWith('*') && /^[A-Za-z_]\w*$/.test(p.split(' ').pop()) && p.split(' ').length > 1) {
      p = p.split(' ').slice(0, -1).join(' ');
    }
    return p;
  }).filter((p) => p.length);
}

function parseSignatures(source) {
  const defs = new Map();
  const re = /th128_world_([a-z0-9_]+)\s*\(/g;
  let match;
  while ((match = re.exec(source))) {
    const name = match[1];
    const open = source.indexOf('(', match.index);
    let depth = 0;
    let close = -1;
    for (let i = open; i < source.length; i++) {
      const c = source[i];
      if (c === '(') depth++;
      else if (c === ')') { depth--; if (depth === 0) { close = i; break; } }
    }
    if (close < 0) continue;
    let after = close + 1;
    while (after < source.length && /\s/.test(source[after])) after++;
    if (source[after] !== '{') continue; // declaration or call, not the definition
    const lineStart = source.lastIndexOf('\n', match.index) + 1;
    const decl = source.slice(lineStart, open).trim();
    const suffix = `th128_world_${name}`;
    if (!decl.endsWith(suffix)) continue;
    const ret = decl.slice(0, decl.length - suffix.length).trim().replace(/\s+/g, ' ');
    defs.set(name, { ret, params: splitParams(source.slice(open + 1, close)) });
  }
  return defs;
}

function auditSourceSignatures(sourceText) {
  const defs = parseSignatures(sourceText);
  const rows = [];
  let passed = true;
  for (const [name, expected] of Object.entries(SOURCE_SIGNATURES)) {
    const actual = defs.get(name) ?? null;
    const ok = !!actual && actual.ret === expected.ret
      && actual.params.length === expected.params.length
      && actual.params.every((p, i) => p === expected.params[i]);
    if (!ok) passed = false;
    rows.push({ name, ok, expected, actual });
  }
  return { passed, count: rows.length, failures: rows.filter((r) => !r.ok), rows };
}

// The native state writer always serializes an inert normalRecord diagnostic object.
function noNormalRecord(record) {
  return !!record && record.bound === false && record.started === false
    && record.prepared === false && record.acknowledged === false
    && record.frames === 0 && record.serial === 0 && record.bytes === 0
    && record.terminalStatsPrepared === false;
}

function main() {
  const argv = process.argv.slice(2);
  const requestPath = argv[0];
  if (!requestPath) throw new Error('usage: run.mjs <request.json> --verify-inputs | --run --grant <grant.json>');
  const mode = argv.includes('--run') ? 'run' : argv.includes('--verify-inputs') ? 'verify' : null;
  if (!mode) throw new Error('mode must be --verify-inputs or --run');
  const grantIndex = argv.indexOf('--grant');
  const grantPath = grantIndex >= 0 ? argv[grantIndex + 1] : null;

  const requestBytes = read(requestPath);
  const request = JSON.parse(Buffer.from(requestBytes).toString('utf8'));
  const base = path.dirname(path.resolve(requestPath));
  const runDir = path.join(base, 'run');
  fs.mkdirSync(runDir, { recursive: true });

  const checks = [];
  const check = (id, ok, detail) => { checks.push({ id, ok: !!ok, detail }); if (!ok) throw new Error(`${id}: ${detail}`); };

  // ---- pins / manifest ----
  const manifestPath = path.join(base, request.manifestName ?? 'actual-manifest.original-a1-stage2-rank3-23707.v2.json');
  const manifestBytes = read(manifestPath);
  const manifest = JSON.parse(Buffer.from(manifestBytes).toString('utf8'));
  check('P1', manifest.requestSha256 === sha256(requestBytes), 'manifest request hash differs');
  check('P2', manifest.pinCount === request.pins.length && manifest.pins.length === request.pins.length, 'pin count differs');
  const byRole = new Map();
  for (const pin of request.pins) {
    const bytes = read(pin.path);
    check(`P3:${pin.role}`, bytes.length === pin.bytes && sha256(bytes) === pin.sha256, `pin mismatch ${pin.path}`);
    byRole.set(pin.role, { ...pin, bytes });
  }

  // ---- authorization identity of the user replay ----
  const user = byRole.get('user-replay');
  check('P4', user.bytes.length === 76403 && user.sha256 === request.userReplay.sha256, 'authorized user replay pin differs');
  check('P5', request.userReplay.path.replace(/\\/g, '/').toLowerCase().endsWith('downloads/th128_ud014c.rpy'), 'user replay path is not the explicitly authorized file');

  // ---- ABI audit from pinned source ----
  const abi = auditSourceSignatures(Buffer.from(byRole.get('core-source-world-exports').bytes).toString('utf8'));
  check('A9', abi.passed, `source ABI mismatches: ${abi.failures.map((f) => f.name).join(',') || 'none'}`);
  check('A9b', abi.count === Object.keys(SOURCE_SIGNATURES).length && abi.count === 50, 'source-signature inventory did not cover exactly 50 rows');
  const exportList = JSON.parse(Buffer.from(byRole.get('core-request').bytes).toString('utf8')).exports;
  const callability = callabilityAudit(exportList);
  check('A9c', callability.exportCountIs205, `core100 export list length ${callability.exportCount} != 205`);
  check('A9d', callability.missingExports.length === 0, `required callable names missing from core100 exports: ${callability.missingExports.join(',')}`);
  check('A9e', callability.invokedMatches, `invoked call sites differ from REQUIRED_CALLABLE (missing ${callability.missingFromRequired.join(',') || 'none'}; extra ${callability.extraRequired.join(',') || 'none'})`);
  check('A9f', callability.sourceOnlyExported.length === 0 && SOURCE_ONLY.every((name) => !REQUIRED_CALLABLE.includes(name)), 'source-only getters leaked into the callable inventory');
  check('A9g', callability.mallocExported && callability.freeExported && callability.errorExported, 'module _malloc/_free/_th128_world_error are not in the pinned export list');

  // ---- codec decode ----
  const codecUrl = pathToFileURL(byRole.get('frontend-replay-codec').path).href;
  return (async () => {
    const codec = await import(codecUrl);
    const decoded = codec.decodeReplay(user.bytes);
    const stage = decoded.stages.find((s) => s.stage === 2);
    check('A2a', !!stage, 'decoded replay has no stage 2');
    check('A2b', decoded.metadata.route === 0 && decoded.metadata.rank === 3 && decoded.metadata.stageCount === 3, 'decoded route/rank/stage count differs');
    const header = stage.rawHeader;
    const headerView = new DataView(header.buffer, header.byteOffset, header.byteLength);
    check('A2c', header.length === 144, 'stage header is not 144 bytes');
    check('A2d', stage.numFrames === 23707 && stage.inputRaw.length === 23707 * 6, 'stage frames/input length differ');
    check('A2e', sha256(header) === request.expectedHeader.headerSha256, 'stage header sha differs');
    check('A2f', sha256(stage.inputRaw) === request.expectedHeader.inputSha256, 'stage input sha differs');
    check('A2g', headerView.getUint16(0, true) === 2 && headerView.getUint16(2, true) === 41297 && headerView.getUint32(4, true) === 23707, 'header identity differs');

    const profileMap = collectProfiles(byRole, request, check);
    const resultPath = path.join(base, request.outputs.result);
    if (mode === 'run') {
      const refusal = grantRefusal(grantPath, requestBytes, manifestBytes, resultPath);
      if (refusal) {
        process.stdout.write(JSON.stringify(refusal, null, 2) + '\n');
        process.exitCode = 3;
        return;
      }
    }
    const summary = {
      schema: 'th128-restoration-original23707-input-verify/3',
      request: path.basename(requestPath),
      requestSha256: sha256(requestBytes),
      pins: request.pins.length,
      totalPinBytes: request.pins.reduce((n, p) => n + p.bytes, 0),
      abiAudit: { passed: abi.passed, count: abi.count, sourceOnly: SOURCE_ONLY },
      callability: { ...callability },
      userReplay: { bytes: user.bytes.length, sha256: user.sha256 },
      stage: { id: 2, seed: 41297, frames: 23707, headerSha256: sha256(header), inputSha256: sha256(stage.inputRaw) },
      profileTypes: [...profileMap.keys()].sort((a, b) => a - b),
      runtime: 'not-created',
    };
    if (mode === 'verify') {
      const stamp = new Date().toISOString().replace(/[:.]/g, '-');
      const outPath = path.join(runDir, `input-verify.v3.${stamp}-${process.pid}.json`);
      fs.writeFileSync(outPath, JSON.stringify({ passed: true, checks, summary }, null, 2), { flag: 'wx' });
      process.stdout.write(JSON.stringify({ passed: true, mode: 'verify', output: outPath, checks: checks.length, abi: summary.abiAudit, callability: summary.callability, profileTypes: summary.profileTypes }, null, 2) + '\n');
      return;
    }

    // ---- grant object (MAIN owns grant/ROOT; a bare boolean is refused; checked before any native creation) ----
    check('G1', !!grantPath, 'missing --grant <grant.json>');
    const grantBytes = read(grantPath);
    const grant = JSON.parse(Buffer.from(grantBytes).toString('utf8'));

    // ---- native ----
    const module = await instantiate(request, byRole);
    check('A3', module.__factories === 1, 'factory count differs');
    const api = (name) => {
      const fn = module[`_th128_world_${name}`];
      if (typeof fn !== 'function') throw new Error(`missing native export th128_world_${name}`);
      return fn;
    };
    for (const name of REQUIRED_CALLABLE) api(name);
    const nativeError = () => String(module.UTF8ToString(api('error')()));
    const jsonCall = (name) => {
      const pointer = api(name)();
      const text = pointer ? String(module.UTF8ToString(pointer)) : '';
      if (!text) throw new Error(`native ${name} returned no JSON: ${nativeError()}`);
      return JSON.parse(text);
    };
    const need = (value, label) => { if (value !== 1) throw new Error(`${label}: ${nativeError()}`); };
    const fill = (bytes) => {
      const pointer = module._malloc(bytes.length || 1);
      if (!pointer) throw new Error(`_malloc failed for ${bytes.length} bytes`);
      module.HEAPU8.set(bytes, pointer);
      return pointer;
    };
    const free = (pointer) => { if (pointer) module._free(pointer); };
    const callBytes = (name, bytes, extra = []) => {
      const pointer = fill(bytes);
      try { return api(name)(pointer, bytes.length, ...extra); } finally { free(pointer); }
    };

    const evidence = {
      schema: 'th128-restoration-original23707-playback-result/3',
      request: path.basename(requestPath),
      requestSha256: sha256(requestBytes),
      manifestSha256: sha256(manifestBytes),
      grant: { ordinal: grant.ordinal, approved: grant.approved, executeOnceRoot: grant.executeOnceRoot },
      core: request.core,
      stage: request.stage,
      userReplay: { path: request.userReplay.path, bytes: user.bytes.length, sha256: user.sha256 },
      abiAudit: { passed: abi.passed, count: abi.count },
      configure: {},
      intro: {},
      transition: {},
      profiles: [...profileMap.keys()].sort((a, b) => a - b),
      playback: {},
      audio: { windows: 0, requests: 0, musicOperations: 0, syntheticOutput: true },
      checks: checks,
      limits: [
        'full playback execution result is what this run proves; the 23707-frame compatibility remains unproven until this runner actually runs under a MAIN grant',
        'per-frame RNG equality with the original game is not provable from the rpy format (no per-frame RNG record); only end-of-segment RNG state is recorded',
        'stage intro draw/raster is skipped; the native intro load and track caption are exercised',
        'audio PCM output is synthetic (journal ACK only); no WebAudio device and no music file decoding',
        'stage3 handoff is out of scope for this request',
      ],
      runtimeStarted: true,
      actual: { Factories: 1, WorldStarts: 0, worldTickCalls: 0 },
    };

    const fails = [];
    const guard = async (label, fn) => {
      try { return await fn(); } catch (error) {
        fails.push({ label, message: String(error && error.message || error) });
        throw error;
      }
    };

    try {
      api('reset')(); // void native API -- no return value exists

      const loadOne = (role) => {
        const record = byRole.get(role);
        const name = record.path.split(/[\\/]/).pop();
        const nameBytes = enc.encode(name);
        const namePointer = fill(nameBytes);
        const dataPointer = fill(record.bytes);
        try {
          need(api('load')(namePointer, nameBytes.length, dataPointer, record.bytes.length), `world_load ${name}`);
        } finally { free(namePointer); free(dataPointer); }
      };
      loadOne('ecl-st_a1_2'); loadOne('ecl-default'); loadOne('ecl-st_a1_2b');
      const eclName = enc.encode('st_a1_2.ecl');
      const eclPointer = fill(eclName);
      try { need(api('start')(eclPointer, eclName.length, 3, 41297), 'world_start st_a1_2.ecl rank3 seed41297'); }
      finally { free(eclPointer); }
      evidence.actual.WorldStarts += 1;
      need(api('set_debug_infinite_respawn')(0), 'finite playback requires set_debug_infinite_respawn(0)');
      check('C1', jsonCall('state').flow?.currentId === 2, 'world start currentId differs');

      // stage-2 intro load + caption (native resource closure used by production installStageIntro)
      const logo = byRole.get('logo-logo_a1_2');
      const logoName = enc.encode('logo_a1_2.anm');
      const logoNamePointer = fill(logoName);
      const logoPointer = fill(logo.bytes);
      try {
        need(api('load_stage_intro')(2, logoNamePointer, logoName.length, logoPointer, logo.bytes.length), 'stage intro load');
      } finally { free(logoNamePointer); free(logoPointer); }
      evidence.intro = jsonCall('stage_intro_state');
      check('C2', evidence.intro.loaded === true && evidence.intro.stageId === 2, 'native stage intro state differs');

      const enemyBanks = ['anm-enemy', 'anm-enemy_bf', 'anm-boss02'];
      enemyBanks.forEach((role, index) => {
        const record = byRole.get(role);
        const pointer = fill(record.bytes);
        try { need(api('load_enemy_anm')(index + 1, pointer, record.bytes.length), `load_enemy_anm bank ${index + 1}`); }
        finally { free(pointer); }
      });
      need(callBytes('enable_geometry526', byRole.get('anm-text').bytes), 'enable_geometry526');

      const bullet8 = byRole.get('runtime-bullet-profile-8').bytes;
      const bulletAnim = byRole.get('anm-bullet').bytes;
      const p8 = fill(bullet8); const pa = fill(bulletAnim);
      try { need(api('enable_bullets')(p8, bullet8.length, pa, bulletAnim.length, 0, 400), 'enable_bullets'); }
      finally { free(p8); free(pa); }
      need(callBytes('enable_bullet_updates', byRole.get('sh-pl00a').bytes), 'enable_bullet_updates');
      need(api('enable_graze')(0, 10000, 0x1234), 'enable_graze');

      const player = byRole.get('anm-pl00').bytes;
      const pp = fill(player);
      try { need(api('enable_player_context')(pp, player.length, 1, 0, 120, 0, 0, 0, 0, 0, 0), 'enable_player_context'); }
      finally { free(pp); }
      need(api('enable_player_motion')(-7156, 51534), 'enable_player_motion');

      const msg = byRole.get('msg-st_a1_2').bytes, text = byRole.get('anm-text').bytes, boss = byRole.get('anm-boss02').bytes, front = byRole.get('anm-front').bytes;
      const pm = fill(msg), pt = fill(text), pp2 = fill(player), pb = fill(boss);
      try { need(api('enable_messages')(pm, msg.length, pt, text.length, pp2, player.length, pb, boss.length), 'enable_messages'); }
      finally { free(pm); free(pt); free(pp2); free(pb); }
      need(callBytes('enable_message_front', front), 'enable_message_front');

      // production attempts the ordinary stage-transition host for stage 2 even during playback; without an
      // incoming carried terminal/session it is expected to be refused, and production then sets autoFlow=false.
      const transitionResult = (() => {
        const pointer = fill(front);
        try { return api('enable_stage_transition')(pointer, front.length, 2, 0, 0, 0, 1, 0); }
        finally { free(pointer); }
      })();
      evidence.transition = { attempted: true, accepted: transitionResult === 1, error: transitionResult === 1 ? '' : nativeError() };

      for (const type of [4, 10, 18, 20, 9, 25, 13, 6, 16, 24, 30, 32, 33, 21, 23, 29]) {
        const bytes = profileMap.get(type);
        check(`C3:${type}`, !!bytes && bytes.length === 288, `bullet profile closure missing type ${type}`);
        const pointer = fill(bytes);
        try { need(api('add_bullet_profile')(type, pointer, bytes.length), `add_bullet_profile type ${type}`); }
        finally { free(pointer); }
      }

      const shot = byRole.get('sh-pl00a').bytes, flags = new Uint8Array(2000 * 4);
      const ps = fill(shot), pplay = fill(player), pf = fill(flags);
      try {
        need(api('enable_player_shots')(ps, shot.length, pplay, player.length, pf, 2000, 1842.457763671875, 3, 0, 0, 0, 0), 'enable_player_shots');
      } finally { free(ps); free(pplay); free(pf); }
      need(api('enable_shot_hits')(43806, 0, 0, 0, 0), 'enable_shot_hits');
      need(api('enable_rapid_shot')(-1), 'enable_rapid_shot');
      need(api('enable_player_death')(-1), 'enable_player_death');
      need(callBytes('enable_perfect_hud', front), 'enable_perfect_hud front.anm');
      need(callBytes('pause_load_manual', byRole.get('anm-help').bytes), 'pause_load_manual');
      const controls = new Uint8Array(32);
      const ctrlPointer = fill(controls);
      try { need(api('enable_player_respawn')(1, 0, ctrlPointer, 8), 'enable_player_respawn'); }
      finally { free(ctrlPointer); }
      need(callBytes('load_score_font', byRole.get('anm-ascii').bytes), 'load_score_font');

      const spell = [
        byRole.get('anm-bullet').bytes, byRole.get('anm-boss02').bytes, text, byRole.get('anm-ascii').bytes, byRole.get('anm-card').bytes, player,
      ];
      const spellPointers = spell.map(fill);
      try {
        need(api('enable_spells')(
          spellPointers[0], spell[0].length, spellPointers[1], spell[1].length, spellPointers[2], spell[2].length,
          spellPointers[3], spell[3].length, spellPointers[4], spell[4].length, spellPointers[5], spell[5].length), 'enable_spells');
      } finally { spellPointers.forEach(free); }

      need(api('enable_head_power')(), 'enable_head_power');
      const ice = byRole.get('anm-ice').bytes;
      const maskMetadata = JSON.parse(Buffer.from(byRole.get('anim-ice-masks').bytes).toString('utf8'));
      const masks = Buffer.concat(maskMetadata.profiles.map((profile, index) => {
        const bytes = Buffer.from(profile.hex, 'hex');
        check('C-ice-mask-' + index, sha256(bytes) === profile.sha256, 'original ice mask digest differs');
        return bytes;
      }));
      check('C-ice-mask-size', masks.length === 1228, 'native original ice masks require1228 bytes');
      const pi = fill(ice), pmasks = fill(masks);
      try { need(api('enable_z_fire')(pi, ice.length, pmasks, masks.length), 'enable_z_fire'); }
      finally { free(pi); free(pmasks); }
      need(api('enable_focus_indicator')(), 'enable_focus_indicator');

      const captionResult = api('track_caption')(1, 0);
      evidence.intro.caption = captionResult === 1 ? jsonCall('stage_intro_state').caption : null;
      check('C4', captionResult === 1, `stage track caption rejected: ${nativeError()}`);

      const before = jsonCall('state');
      check('C5', before.flow?.currentId === 2 && before.flow?.difficulty === 3 && before.flow?.shotDifficulty === 3, 'world identity differs');
      check('C6', before.flow?.gameRandom?.seed === 41297 && before.flow?.gameRandom?.calls === 0, 'fresh RNG identity differs');
      check('A6a', noNormalRecord(before.normalRecord), 'unexpected normal record owner or active record state');
      evidence.configure.beforePlayback = { frame: before.frame, currentId: before.flow?.currentId, gameRandom: before.flow?.gameRandom, displayRandom: before.flow?.displayRandom };

      // native playback bind: restore validates + applies the header, load binds the recorded input packet
      const packet = Buffer.concat([Buffer.from(header), Buffer.from(stage.inputRaw)]);
      const headerPointer = fill(Buffer.from(header));
      try { need(api('replay_playback_restore')(headerPointer, 144, 3), 'replay_playback_restore'); }
      finally { free(headerPointer); }
      const packetPointer = fill(packet);
      try { need(api('replay_playback_load')(packetPointer, packet.length, 3), 'replay_playback_load'); }
      finally { free(packetPointer); }
      const bound = jsonCall('replay_playback_state');
      check('C7', bound.active === true && bound.stage === 2 && bound.rank === 3 && bound.frames === 23707 && bound.consumed === 0, 'playback binding identity differs');

      const startedAt = Date.now();
      let ticks = 0;
      let playback = bound;
      let world = before;
      while (ticks < request.budget.maxGameTicks) {
        evidence.actual.worldTickCalls += 1;
        const advanced = api('tick')();
        if (advanced === 2) throw new Error(`World left finite playback at tick ${ticks} (native game-over/pause branch) state=${JSON.stringify(world.frame)} error=${nativeError()}`);
        need(advanced, `world_tick ${ticks}`);
        ticks += 1;
        drainSounds(api, module, jsonCall, evidence.audio, nativeError);
        drainMusic(api, jsonCall, evidence.audio, nativeError);
        playback = jsonCall('replay_playback_state');
        world = jsonCall('state');
        if (playback.complete) break;
        if (ticks % 1000 === 0) {
          process.stdout.write(JSON.stringify({ ticks, frame: world.frame, consumed: playback.consumed, elapsedMs: Date.now() - startedAt }) + '\n');
        }
        if (Date.now() - startedAt > request.budget.maxWallMs - 20000) throw new Error(`wall budget reached at tick ${ticks}`);
      }
      evidence.playback = {
        complete: playback.complete === true,
        frames: playback.frames,
        consumed: playback.consumed,
        ticks,
        worldFrame: world.frame,
        generation: playback.generation,
        auxiliaryKnown: playback.auxiliaryKnown,
        elapsedMs: Date.now() - startedAt,
        final: playback,
      };
      evidence.rng = {
        endGameRandom: world.flow?.gameRandom ?? null,
        endDisplayRandom: world.flow?.displayRandom ?? null,
        perFrameEqualityClaimed: false,
      };
      evidence.configure.afterPlayback = {
        currentId: world.flow?.currentId,
        normalTerminal: world.normalTerminal ?? null,
        debugTerminal: world.debugTerminal ?? null,
        normalRecord: world.normalRecord ?? null,
        clearRecordEvents: world.debugRecordPrefix?.clearRecordEvents ?? null,
      };

      check('A5a', playback.complete === true, 'playback did not complete');
      check('A5b', playback.consumed === 23707 && playback.frames === 23707, 'consumed/frames differ from 23707');
      check('A5c', world.frame === 23707, `world frame ${world.frame} differs from 23707`);
      check('A6b', !world.normalTerminal?.configured && !world.debugTerminal?.configured, 'unexpected terminal settlement was configured');
      check('A6c', noNormalRecord(world.normalRecord), 'unexpected ordinary record credential or active record state');

      evidence.checks = checks;
      evidence.passed = checks.every((c) => c.ok);
      const serialized = JSON.stringify(evidence, null, 2);
      const fd = fs.openSync(resultPath, 'wx');
      try { fs.writeFileSync(fd, serialized); fs.fsyncSync(fd); } finally { fs.closeSync(fd); }
      process.stdout.write(JSON.stringify({ passed: evidence.passed, mode: 'run', result: resultPath, ticks, audio: evidence.audio, transition: evidence.transition, abi: evidence.abiAudit }, null, 2) + '\n');
    } catch (error) {
      await guard('failure', async () => {
        const snapshot = { message: String(error && error.message || error), stack: String(error && error.stack || '') };
        try { snapshot.nativeState = jsonCall('state'); } catch { snapshot.nativeState = null; }
        try { snapshot.nativeError = nativeError(); } catch { snapshot.nativeError = null; }
        try { snapshot.playback = jsonCall('replay_playback_state'); } catch { snapshot.playback = null; }
        snapshot.failures = fails;
        snapshot.actual = { ...evidence.actual };
        snapshot.configure = evidence.configure;
        snapshot.audio = evidence.audio;
        snapshot.normalClearClaimed = false;
        const stamp = new Date().toISOString().replace(/[:.]/g, '-');
        let target = path.join(runDir, `first-failure-${stamp}-${process.pid}.json`);
        let suffix = 0;
        while (fs.existsSync(target)) target = path.join(runDir, `first-failure-${stamp}-${process.pid}-${++suffix}.json`);
        const fd = fs.openSync(target, 'wx');
        try { fs.writeFileSync(fd, JSON.stringify(snapshot, null, 2)); fs.fsyncSync(fd); } finally { fs.closeSync(fd); }
        process.stderr.write(`FIRST-FAILURE ${target}\n${snapshot.message}\n`);
      });
      process.exitCode = 1;
    }
  })().catch((error) => {
    process.stderr.write(`RUNNER-FAILURE ${String(error && error.stack || error)}\n`);
    process.exitCode = 1;
  });
}

function collectProfiles(byRole, request, check) {
  const map = new Map();
  const push = (type, bytes, origin) => {
    if (!Number.isInteger(type) || type < 1 || type > 64) throw new Error(`invalid bullet profile type ${type} from ${origin}`);
    if (bytes.length !== 288) throw new Error(`bullet profile type ${type} from ${origin} is ${bytes.length} bytes`);
    const existing = map.get(type);
    if (existing && Buffer.compare(Buffer.from(existing), Buffer.from(bytes)) !== 0) throw new Error(`conflicting bullet profile type ${type} (${origin})`);
    map.set(type, new Uint8Array(bytes));
  };
  const jsonRoles = [
    'anim-bullet-boss-profiles', 'anim-bullet-native-profiles', 'anim-bullet-spell-profiles',
    'anim-bullet-type13', 'anim-bullet-type16', 'anim-bullet-type21', 'anim-bullet-type24',
    'anim-bullet-type25', 'anim-bullet-type30', 'anim-bullet-type6', 'anim-bullet-type32',
  ];
  for (const role of jsonRoles) {
    const record = byRole.get(role);
    if (!record) continue;
    const parsed = JSON.parse(Buffer.from(record.bytes).toString('utf8'));
    const entries = Array.isArray(parsed) ? parsed : Array.isArray(parsed.profiles) ? parsed.profiles : [parsed];
    for (const entry of entries) {
      if (!entry || typeof entry.hex !== 'string') throw new Error(`${role} has no profile hex`);
      push(entry.type, Buffer.from(entry.hex, 'hex'), role);
    }
  }
  for (const [key, type] of [['runtime-bullet-profile-18', 18], ['runtime-bullet-profile-20', 20]]) {
    const record = byRole.get(key);
    if (record) push(type, record.bytes, key);
  }
  for (const key of ['9', '32', '33']) {
    const entry = request.typedProfiles?.[key];
    check(`C3b:${key}`, !!entry && entry.bytes === 288 && sha256(Buffer.from(entry.hex, 'hex')) === entry.sha256, `typed profile ${key} differs`);
    push(Number(key), Buffer.from(entry.hex, 'hex'), `typed-${key}`);
  }
  return map;
}

async function instantiate(request, byRole) {
  const moduleUrl = pathToFileURL(byRole.get('core-module').path).href;
  const factoryImport = await import(moduleUrl);
  const factory = factoryImport.default;
  if (typeof factory !== 'function') throw new Error('pinned core.mjs has no default factory');
  const wasmBinary = byRole.get('core-wasm').bytes;
  const module = await factory({ wasmBinary });
  module.__factories = 1;
  return module;
}

function soundWindowShapeOk(window) {
  return !!window && Number.isInteger(window.serial) && Number.isInteger(window.epoch)
    && Number.isInteger(window.baseLow) && Number.isInteger(window.baseHigh) && Number.isInteger(window.count);
}

function drainSounds(api, module, jsonCall, tally, nativeError) {
  // N1 parity with production drainGameSoundsLegacy: the epoch must be live, the window packet must
  // be a readable five-integer record, and a pre-existing open window this consumer does not own is
  // refused instead of adopted.
  const epoch = api('sound_epoch')();
  if (!Number.isInteger(epoch) || epoch === 0) throw new Error('native sound journal epoch is unavailable or exhausted');
  const prior = jsonCall('sound_window_state');
  if (!soundWindowShapeOk(prior)) throw new Error('native sound journal window state is unavailable');
  if (prior.serial !== 0) throw new Error('native sound journal has an open read window this consumer does not own');
  for (let guard = 0; guard < 32; guard++) {
    const serial = api('sound_begin_window')();
    if (serial === 0) return;
    const window = jsonCall('sound_window_state');
    if (!soundWindowShapeOk(window)) throw new Error('native sound journal window state is unavailable after open');
    if (window.serial !== serial) throw new Error(`sound window serial ${window.serial} != ${serial}`);
    if (window.epoch !== epoch) throw new Error(`sound window epoch ${window.epoch} != ${epoch}`);
    if (!Number.isInteger(window.count) || window.count < 0 || window.count > 65536) throw new Error(`sound window count ${window.count}`);
    for (let i = 0; i < window.count; i++) {
      const id = api('sound_id')(i), x = api('sound_x')(i), operation = api('sound_operation')(i);
      if (!Number.isInteger(id) || id < -1 || id > 75) throw new Error(`sound id ${id}`);
      if (!Number.isFinite(x)) throw new Error(`sound x ${x}`);
      if (!Number.isInteger(operation) || operation < 0 || operation > 2) throw new Error(`sound operation ${operation}`);
      if (id === -1 && operation !== 0) throw new Error('sound journal emitted an empty identifier with an operation');
    }
    const remaining = api('sound_ack_window')(window.serial, window.count, window.baseLow, window.baseHigh, window.epoch);
    if (remaining < 0) throw new Error(`sound window ACK rejected: ${nativeError()}`);
    tally.windows += 1;
    tally.requests += window.count;
  }
  throw new Error('sound journal did not drain within 32 windows');
}

function drainMusic(api, jsonCall, tally, nativeError) {
  for (let guard = 0; guard < 64; guard++) {
    const state = jsonCall('music_control_state');
    const serial = Number(state.serial ?? 0);
    if (!serial || state.acknowledged) return;
    if (![3, 4, 5].includes(state.operation)) throw new Error(`unsupported music control operation ${state.operation}`);
    const acked = api('acknowledge_music_control')(serial);
    if (acked !== 1) throw new Error(`music control ACK rejected: ${nativeError()}`);
    tally.musicOperations += 1;
  }
  throw new Error('music control journal did not drain within 64 operations');
}

function grantRefusal(grantPath, requestBytes, manifestBytes, resultPath) {
  const base = { refused: true, reason: 'MAIN_GRANT_REQUIRED', worldStarts: 0, factories: 0 };
  if (!grantPath) return { ...base, detail: 'no --grant <grant.json> provided' };
  let grant;
  try { grant = JSON.parse(fs.readFileSync(grantPath, 'utf8')); }
  catch (error) { return { ...base, detail: `grant unreadable: ${error.message}` }; }
  if (grant.approved !== true) return { ...base, detail: 'grant.approved is not true' };
  if (!Number.isInteger(grant.ordinal) || grant.ordinal < 600) return { ...base, detail: 'grant.ordinal is missing or below 600' };
  if (grant.requestSha256 !== sha256(requestBytes)) return { ...base, detail: 'grant.requestSha256 does not match this request' };
  if (grant.manifestSha256 !== sha256(manifestBytes)) return { ...base, detail: 'grant.manifestSha256 does not match this manifest' };
  if (typeof grant.executeOnceRoot !== 'string' || !fs.existsSync(grant.executeOnceRoot)) return { ...base, detail: 'grant.executeOnceRoot does not exist' };
  if (fs.existsSync(resultPath)) return { ...base, detail: 'run/result.json already exists; first raw results are never overwritten' };
  return null;
}

main();
