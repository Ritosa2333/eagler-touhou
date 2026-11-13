// Narrow evidence adapter. This is not an oracle, a pixel comparator, or a
// substitute for the versioned acceptance contract. Unknown formats stay blocked.
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {execFile} from 'node:child_process';
import {promisify} from 'node:util';
import {join} from 'node:path';
import assert from 'node:assert/strict';
import {assertBuildDelivery} from './formal-delivery.mjs';

const execFileAsync = promisify(execFile);
const HUD_ROOT = '/work/artifacts/mp-hud-framebuffer-20261112';
const BUILD = '/work/eagler-touhou/ports/th13-mp/web/artifacts/hud-fixture-20261112/build.json';
const BUILD_SHA = '4ac9cb34b75d4a61acec951e47d6495280768f8b32ba98dd10b27e2e98314f2d';
const SOURCE_ROOT = '/work/eagler-touhou/ports/th13-mp/web/cpp/game';
const SOURCE_FILES = ['GameHud.cpp','GameHud.hpp','GameHudDraw.cpp','GameHudFrame.cpp',
  'RunScene.cpp','RunScene.hpp','AnmDrawSchedule.cpp','AnmDrawSchedule.hpp','HudDrawData.hpp',
  'AnmRenderer.cpp','AnmCoordinates.cpp','AnmResource.cpp','AsciiText.cpp','AsciiFrame.cpp',
  'SessionState.hpp'];
const ADDITIONAL_SOURCES = [
  '/work/eagler-touhou/ports/th13-mp/portable/sdl/Renderer.cpp',
  '/work/eagler-touhou/ports/th13-mp/portable/sdl/Shaders.hpp',
  '/work/eagler-touhou/ports/th13-mp/portable/sdl/UnormBlendProfile.hpp',
  '/work/eagler-touhou/ports/th13-mp/web/cpp/sdl/Application.cpp',
];
// Exact identity of a reviewed, bounded diagnostic evidence set; these hashes
// identify inputs, not golden pixels or tolerance changes.
const HUD_FILES = {
  "run-5/max.rgba": "a85d98cacc896c2b91e657f7b415ae57cca66a5cb26a63e060a9fe9579f06060",
  "run-5/ghost.rgba": "c97198e3a05711cb2dada9c34badbada44f10043e21a1892be9cea98f9b8ddd8",
  "run-5/report.json": "a5e4c437cda725aef7378f3a283784ea038634276215eab96b918199ce17e494",
  "background-fixture/background.rgba": "da1959645ff981e5927f5288d8c04771434734f48c3e5fbff4e8042a157aa0b7",
  "background-fixture/max.rgba": "a85d98cacc896c2b91e657f7b415ae57cca66a5cb26a63e060a9fe9579f06060",
  "background-fixture/ghost.rgba": "c97198e3a05711cb2dada9c34badbada44f10043e21a1892be9cea98f9b8ddd8",
  "background-fixture/capture-report.json": "e56eef469081b5298c1d5d4c6344a70c3cee4a2ae5294ac1540a709a3ab90407",
  "background-fixture/verify.py": "0a22110b59ebc92b5690474e899e32db12f84319d26571bea8e23c98bf06b23e",
  "background-fixture/build-evidence.json": "c66dc6159f42af9e0bd54ccda8585c40913596aef083098cf090d4edceeed915",
  "background-fixture/fixture.cpp": "f2473dfadb41ee9e23bebbddc00716bf46a123cf12582d6c8fec0eaf9efa3234",
  "background-fixture/hud-fixture.wasm": "318a5c7b1e726d61d609ae656d944d4b7a1dc8760359c5a91fa60e0e5db9669a",
  "submission-trace/max-trace.json": "a9399b42f7af3e8acd93039a13e5ea0ced049ba9d1481614f1fc6ba68174a780",
  "submission-trace/ghost2-trace.json": "9efb77e0931fc9e0aaad1dc45ae6390158bc44231aa216686c2b459318e2a0d7",
  "submission-trace/verify_icons.py": "5dd096fd1aa88ae36bb1862a71c9c754e3fd41b3f6f696d620a904438071f8ca",
  "submission-trace/max.rgba": "a85d98cacc896c2b91e657f7b415ae57cca66a5cb26a63e060a9fe9579f06060",
  "submission-trace/ghost2.rgba": "c97198e3a05711cb2dada9c34badbada44f10043e21a1892be9cea98f9b8ddd8",
  "submission-trace/capture-report.json": "027b219fcac22052b3ed6da026d4c4aa2faef2102b92e786f7a5c05ef6353dc9",
  "submission-trace/capture-report-ghost2-stress.json": "ae14e4584bf11736e6d4ba4afd4dd967888c876fe791f5bbb793c1a58970dcf1",
  "submission-trace/build-evidence.json": "e849d962e546b883633cb5000d5a447a4983be165fb0be8890e6822ca59849ee",
  "submission-trace/fixture.cpp": "844ed3d5edf7230eb5fa2963b5342997bd3a639763ca86035bf94fd59352cb37",
  "submission-trace/hud-fixture.wasm": "74bfcef38ecd57509abf13b26a4f136ffa7f03e61838fda731dec92b8ae3ae7f",
  "submission-clean/full-ghost-trace.json": "fd46e0e235d325c9648918f6f8a0784ee298b477ede8202fad8c26413edea121",
  "submission-clean/full-stress999-trace.json": "efe3bf009575cc21bb88f2cdf685b70454044d9054f36614c2941d93d40e0b3d",
  "submission-clean/full-ghost.rgba": "07740aee9f97a0f68c2900a9be0f3850db5a0327121c74bfb819c9e85325a1c6",
  "submission-clean/full-stress999.rgba": "4592bce062659dd8f7eab39aca5f9c1c99a3681345022543478b7b4e490595de",
  "submission-clean/verify_geometry.py": "b7dc4d848ebd04dd0e26f58d7bd28bad3f5c9dee9deeb6c968415212bfce6887",
  "submission-clean/capture-report-full.json": "5b87a8ebb106af9400d034fdc9cf30de9932d76f7536d934276e6f5b87d52c91",
  "submission-clean/build-evidence.json": "6b7d0508c720aa6cc2d6e94166ccd55063e7dadb7ea84ca80695717b48a0d1f6",
  "submission-clean/fixture.cpp": "9783bb049885715de6f19ae8468d8a72c07ba99440280e589a2ecda67d164d26",
  "submission-clean/trace.js": "a090086c91b19ffa4d882ae83eae062298fc3ed36287b49e6812a9aa048a5448",
  "submission-clean/hud-fixture.wasm": "4c763c3ca0c362b085ce5d8248854b8afb42eda0626ebc537f8a57bcde62c065",
  "submission-terminal/terminal-trace.json": "81e8d9174763636df1db007e5606d3d82e936607af56b3e9191151b6525ae976",
  "submission-terminal/terminal.rgba": "012a6e477ab6766f5aa8c66d0d3a62dd579753130e6780bf36ee5c87e162f4f6",
  "submission-terminal/verify_terminal.py": "3554adef0114a14678ea5cc26c3e2b6b19cf45b74c26f98c111362044dc82e16",
  "submission-terminal/capture-report-full.json": "96388361aee4d85f2b4bc6a244936b1e02f7f8c357d7b6426628fda64307b2d1",
  "submission-terminal/build-evidence.json": "56e71b5b76c6c7ae241cb6c46e658133a5cea46edf84492dde3c3b341fa27930",
  "submission-terminal/fixture.cpp": "52c91033741d38610f111e0b2f8eb461e4337df311aa72b102221fc12455cdb8",
  "submission-terminal/trace.js": "a090086c91b19ffa4d882ae83eae062298fc3ed36287b49e6812a9aa048a5448",
  "submission-terminal/hud-fixture.wasm": "53bdae82a5466311c0c5f7bb4242d2d8f908fa7fa1161057d45e97fac19c5ecc"
};
const VERIFY = ['background-fixture/verify.py','submission-trace/verify_icons.py',
  'submission-clean/verify_geometry.py','submission-terminal/verify_terminal.py'];
const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const status = (state, reason, extra = {}) => ({status:state, reason, ...extra});

async function hudEvidence(readBytes, runtimeManifest) {
  try {
    for (const [file, expected] of Object.entries(HUD_FILES)) {
      if (hash(await readBytes(join(HUD_ROOT,file))) !== expected) throw Error(`HUD evidence hash mismatch: ${file}`);
    }
    const bytes = await readBytes(BUILD);
    if (hash(bytes) !== BUILD_SHA) throw Error('HUD build manifest hash mismatch');
    const manifest = JSON.parse(bytes.toString());
    for (const file of [...SOURCE_FILES.map(name=>join(SOURCE_ROOT,name)), ...ADDITIONAL_SOURCES]) {
      const expected = manifest.sourceFiles?.[file];
      if (!/^[0-9a-f]{64}$/.test(expected || '') || runtimeManifest.sourceFiles?.[file] !== expected || hash(await readBytes(file)) !== expected)
        throw Error(`HUD fixture/runtime source mismatch: ${file}`);
    }
    assert.deepEqual(runtimeManifest.flags,manifest.flags,'HUD compiler flags differ');
    assert.deepEqual(runtimeManifest.toolchain,manifest.toolchain,'HUD toolchain identity differs');
    for (const [file, expected] of [['front.anm','736686b4bf6056302df1036d4f73a9741a98355376e939b2f29277562a7b39a2'],
      ['ascii.anm','705b5f8b2958a49deac157e04c4fff5a66acd7ae279cdc7a03ba3b1673d71f7a']]) {
      if (hash(await readBytes(join('/work/artifacts/reference/raw',file))) !== expected)
        throw Error(`HUD asset mismatch: ${file}`);
    }
    // Execute the fixed-identity checkers on their original raw inputs. They
    // normally write JSON summaries; suppress only those output writes to keep
    // this adapter read-only. Do not suppress validation or alter inputs.
    const bootstrap = "import pathlib,runpy,sys; pathlib.Path.write_text=lambda self,*a,**k: 0; runpy.run_path(sys.argv[1],run_name='__main__')";
    for (const file of VERIFY) {
      await execFileAsync('python3',['-c',bootstrap,join(HUD_ROOT,file)],{cwd:'/work',timeout:30000,maxBuffer:8*1024*1024});
    }
    return status('PASS','Bounded three-seat fixture checks rerun with raw frames, GL trace, assets and matching HUD source identity',{
      scope:'HUD diagnostic only; 19 named HUD/ANM/renderer sources and compiler identity match, not whole-program equivalence; run5 text has stale captions; two alpha samples, not original GPU fidelity',
      buildManifestSha256:BUILD_SHA, sourceFiles:[...SOURCE_FILES.map(name=>join(SOURCE_ROOT,name)), ...ADDITIONAL_SOURCES], verifierCount:VERIFY.length});
  } catch (error) {
    return status('BLOCKED',String(error?.message || error));
  }
}

async function originalCapture(readBytes) {
  const root = '/work/artifacts/mp-original-gpu-20261112';
  try {
    const checks = {
      'qualification-report.json':'469b089ad7bc4389bb9b38212044e61e801c6dbc69fdf461bd0ff213c38543a7',
      'run04/original.bgra':'68f5ad317681159fcb349e2e7f664019255a0c4ecf390b389e0a80c57731d14a',
      'run04/original-index.jsonl':'ddb7d8e8b203bb53606f0e99a43811500188fbf892d743e436af8d0120418fe5',
      'run04/capture.log':'a78e32d084c7a2367dcc114721db14a9b36342d5c8751610f7afec1ebf118a31',
    };
    const files = {};
    for (const [name, expected] of Object.entries(checks)) {
      files[name] = await readBytes(join(root,name));
      if (hash(files[name]) !== expected) throw Error(`Original evidence hash mismatch: ${name}`);
    }
    const report=JSON.parse(files['qualification-report.json'].toString());
    const index=files['run04/original-index.jsonl'].toString().trim().split('\n').map(JSON.parse);
    if (files['run04/original.bgra'].length !== 120*640*480*4 || index.length !== 120 ||
        !index.every((row,i)=>row.frame===i && row.consumed===i+1 && row.present===563+i && row.slot===1) ||
        report.status!=='BLOCKED') throw Error('Original capture incomplete or qualification changed');
    return status('BLOCKED','Original GPU capture environment was exercised, but reference phase qualification failed',{
      evidence:join(root,'qualification-report.json'), scope:'v1.00c Stage 1 Replay, 120 frames', captureIntegrity:'120 continuous indexed frames; evidence identity checked'});
  } catch(error) {
    return status('BLOCKED',`Original capture evidence missing or invalid: ${String(error?.message || error)}`);
  }
}

// Build manifest is the trusted, retained build artifact supplied by the caller;
// equality checks do not cryptographically attest its source inventory. The
// caller is responsible for retaining it from the actual build and acquisition.
export function assertRuntimeBuildBinding({expectedFiles, runtimeManifestBytes}) {
  assert(Buffer.isBuffer(runtimeManifestBytes) || runtimeManifestBytes instanceof Uint8Array,
    'Runtime build manifest bytes required');
  const manifest=JSON.parse(Buffer.from(runtimeManifestBytes).toString('utf8'));
  assert.equal(manifest.game,'th13','Runtime build game');
  assert.equal(manifest.kind,'cpp-sdl3','Runtime build kind');
  assert(Array.isArray(expectedFiles) && expectedFiles.length===2,'Expected loader and WASM');
  assert.deepEqual(expectedFiles.map(file=>file?.name).sort(),['th13-sdl.mjs','th13-sdl.wasm']);
  for (const file of expectedFiles) {
    assert(Number.isSafeInteger(file.bytes) && file.bytes>0,'Expected file byte length');
    assert.equal(typeof file.sha256,'string','Expected file hash');
    assert.match(file.sha256,/^[a-f0-9]{64}$/,'Expected file hash');
  }
  const loader=expectedFiles.find(file=>file.name==='th13-sdl.mjs');
  const wasm=expectedFiles.find(file=>file.name==='th13-sdl.wasm');
  assert.equal(manifest.sha256,wasm.sha256,'Runtime manifest WASM hash');
  assert.equal(manifest.bytes,wasm.bytes,'Runtime manifest WASM size');
  assert.equal(manifest.loaderSha256,loader.sha256,'Runtime manifest loader hash');
  return {manifest,sha256:hash(runtimeManifestBytes)};
}

/** Evaluate a closed delivery and a retained build manifest. The independent
 * reference and full pixel contract are not implemented, so they stay blocked. */
export async function evaluateRequiredAcceptance({formalReport, runtimeManifestBytes, readBytes=readFile} = {}) {
  const original=await originalCapture(readBytes);
  let hud, runtime;
  try {
    const binding=assertRuntimeBuildBinding({expectedFiles:formalReport?.expectedFiles,runtimeManifestBytes});
    assert.equal(formalReport.runtimeBuild?.path,'runtime-build.json','Retained runtime manifest path');
    assert.equal(formalReport.runtimeBuild?.sha256,binding.sha256,'Retained runtime manifest hash');
    assert(Number.isSafeInteger(formalReport.playerCount) && formalReport.playerCount>0,'Formal player count');
    assert(Array.isArray(formalReport.frameGuids) && Array.isArray(formalReport.documentEpochs),
      'Frame and document identity required');
    assert(formalReport.closedDelivery && formalReport.closedDelivery.passed===true,'Closed delivery receipt missing');
    assert.equal(formalReport.deliveryBoundary?.closed,true,'Delivery sources not closed');
    assert.equal(formalReport.deliveryBoundary?.settled,true,'Delivery responses not settled');
    assert.equal(formalReport.deliveryBoundary?.pendingRequests,0,'Pending delivery requests');
    assert.equal(formalReport.deliveryBoundary?.pendingBodies,0,'Pending delivery bodies');
    const checked=assertBuildDelivery({expectedFiles:formalReport.expectedFiles,delivery:formalReport.delivery,
      playerCount:formalReport.playerCount,frameGuids:formalReport.frameGuids,documentEpochs:formalReport.documentEpochs});
    assert.equal(formalReport.closedDelivery.seats,checked.seats,'Closed delivery seat count');
    assert.equal(formalReport.closedDelivery.responses,checked.responses,'Closed delivery response count');
    assert(Array.isArray(formalReport.errors) && formalReport.errors.length===0,'Formal report has errors');
    runtime=status('PASS','Frame/document-bound delivered bytes match the retained build manifest',{
      manifestSha256:binding.sha256, seats:checked.seats, responses:checked.responses,
      scope:'Caller must retain authentic build artifact and close and settle all response sources'});
    hud=await hudEvidence(readBytes,binding.manifest);
  } catch(error) {
    runtime=status('BLOCKED',String(error?.message || error));
    hud=status('BLOCKED','HUD source identity cannot be bound to this runtime delivery');
  }
  return {
    passed:false, status:'BLOCKED', runtime, hud, original,
    tool:status('NOT_EVALUATED','No runner receipt is evaluated by this adapter; TOOL_PASS is a separate gate'),
    determinism:status('NOT_EVALUATED','Repeat capture identity and completeness are not evaluated here'),
    pixel:status('BLOCKED','No independently qualified reference and implemented strict numerical comparison contract are accepted here'),
    coverage:status('BLOCKED','Required full-game and input-path coverage is not evaluated by this adapter'),
  };
}
