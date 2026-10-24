// Source-level contract for the TH11 Presentation Lab integration.
// Runtime evidence lives in logs/lab/report.json; this gate protects the
// wiring and the diagnostic/production boundary from silent drift.
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'..');
const read=path=>readFileSync(resolve(root,path),'utf8');
const adapter=read('portable/presentation-lab/adapter.mjs');
assert(adapter.includes('simulationTick'),'adapter must expose simulationTick');
assert(adapter.includes('referenceDrawSerial'),'adapter must expose referenceDrawSerial');
assert(adapter.includes('presentation_lab_start_stage'),'adapter must seed stages through the diagnostic export');
assert(adapter.includes('next_occurrence')===false,'occurrence identity belongs to the native audit');
const abi=read('portable/presentation-lab/native-abi.mjs');
for(const name of ['presentation_lab_start_stage','presentation_lab_freeze','presentation_lab_tick','presentation_lab_draw','audit_state','audit_stride'])
  assert(abi.includes(name),'missing ABI export '+name);
const app=read('web/cpp/sdl/Application.cpp');
assert(app.includes('#ifdef TH_PRESENTATION_AUDIT'),'diagnostic exports must be profile-gated');
assert(app.includes('presentation_lab_start_stage'),'missing stage-seed export');
assert(app.includes('th11::high_refresh::smooth_submission=high'),'rAF must select high-refresh submission');
const audit=read('web/cpp/game/PresentationAudit.cpp');
assert(audit.includes('reserved_420[0]'),'audit must use the stable VM identity');
assert(audit.includes('next_occurrence'),'audit must separate repeated submissions');
assert(audit.includes('fault_phase'),'negative control must alternate across frames');
const build=read('portable/build.mjs');
assert(build.includes('artifacts/presentation-diagnostics'),'diagnostic builds need an isolated output');
assert(build.includes("TH_PRESENTATION_AUDIT=1"),'lab build must enable the audit profile');
const packaging=read('portable/package-eagler.mjs');
assert(packaging.includes('validateProductionBuild('),'production packager must inspect the actual binary');
assert(read('portable/production-build.mjs').includes('Production packaging rejects diagnostic'),'packaging must reject diagnostic builds');
console.log('TH11 Presentation Lab source contract PASS');
