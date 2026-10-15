/** Original main6440–6580 Replay behavior through synthetic native ports only. */
import test, {before, after} from 'node:test';
import assert from 'node:assert/strict';
import {mkdir, mkdtemp, rm} from 'node:fs/promises';
import {existsSync} from 'node:fs';
import {resolve, dirname} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
const root = fileURLToPath(new URL('../../', import.meta.url));
let work, createReplayModel, formatReplayBytes;
before(async () => {
  await mkdir(resolve(root, '.cache'), {recursive: true}); work = await mkdtemp(resolve(root, '.cache/replays-main-'));
  const outfile = resolve(work, 'replays.mjs');
  await build({entryPoints: [resolve(root, 'app/models/replays.ts')], outfile, bundle: true, platform: 'node', format: 'esm', packages: 'external', logLevel: 'silent',
    plugins: [{name: 'authored-main', setup(ctx) {ctx.onResolve({filter: /^\.\.?\/.*\.mjs$/}, args => {
      const path = resolve(dirname(args.importer), args.path), source = path.slice(0, -4) + '.mts';
      if (path.startsWith(resolve(root, 'src') + '/') && existsSync(source)) return {path: source};
    });}}]});
  ({createReplayModel, formatReplayBytes} = await import(pathToFileURL(outfile).href));
});
after(async () => {if (work) await rm(work, {recursive: true, force: true});});
function deferred() {let resolve, reject; const promise = new Promise((r, j) => {resolve = r; reject = j;}); return {promise, resolve, reject};}
const tick = () => new Promise(resolve => setImmediate(resolve));
function fixture({cold = false, launched = false, prepareGate = null, broken = false, watch = null, viewer = false, storage = null} = {}) {
  const files = new Map([['replay/th6_02.rpy', [2]], ['replay/th6_01.rpy', [1, 1]], ['score.dat', [9]]]);
  const events = [], toasts = [], downloads = [], confirms = [], matches = new Map();
  const state = {ready: !cold, epoch: cold ? null : 1, launched, game: 'th06'};
  let answer = null, confirmed = true;
  const model = createReplayModel({
    ...(storage ? {storage} : {}),
    runtime: {getSnapshot: () => state, async withFileSession(game, op) {
      events.push('lock');
      try {return await op({epoch: state.epoch, async sync() {events.push('sync');if(state.failSyncAfterWrite&&state.wrote)throw Error('flush failed');}, async send(command, payload) {
        events.push(`${command}:${payload.path ?? ''}`);
        if (broken) throw new Error('fixture read failure');
        if (command === 'list') return {files: [...files].map(([path, bytes]) => ({path, size: bytes.length,...(matches.has(path)?{replayMatch:matches.get(path)}:{})}))};
        if (command === 'read') return {bytes: state.corruptRead===payload.path?[255]:files.get(payload.path)};
        if (command === 'write') {if(state.failWrite)throw Error('write failed');state.wrote=true;files.set(payload.path, payload.bytes);if(payload.replayMatch)matches.set(payload.path,payload.replayMatch);}
        if (command === 'remove') {files.delete(payload.path);matches.delete(payload.path);}
        return {};
      }});} finally {events.push('unlock');}
    }},
    ...(viewer ? {prepareViewer: async () => {events.push('viewer'); if (prepareGate) await prepareGate.promise; state.ready = state.launched = true; state.epoch ??= 1;}, closeViewer: () => events.push('closeViewer')} : {}),
    ...(watch ? {watch: async (...args) => {events.push('watch'); await watch(...args);}} : {}),
    async prepareFiles() {events.push('prepare'); if (prepareGate) await prepareGate.promise; state.ready = true; state.epoch ??= 1;},
    async releasePrepared(product, epoch) {events.push(`release:${product}:${epoch}`); state.ready = false; state.epoch = null;},
    translate: (key, params) => key + (params ? JSON.stringify(params) : ''),
    prompt(message, name) {events.push(`prompt:${message}:${name}`); return answer;},
    async confirm(decision) {confirms.push(decision); return confirmed;},
    download(name, bytes, mime) {downloads.push({name, bytes: [...new Uint8Array(bytes)], mime});},
    toast(message) {toasts.push(message);}, async afterPaint() {events.push('paint');}, afterClose() {events.push('afterClose');},
  });
  return {model, files, matches, events, toasts, downloads, confirms, state, answer(value) {answer = value;}, confirm(value) {confirmed = value;}};
}
test('TH09 archive names survive rename even when the manager switches products during the mutation', async () => {
  const f=fixture(),path='replay/xxx vs abdjfal/th9_ud0000.rpy',target='replay/xxx vs abdjfal/th9_ud0001.rpy';
  const match={schema:'eagler-touhou/th09-match-replay/1',names:['xxx','abdjfal'],startedAt:1792029600000,frames:7200,winner:0};
  f.files.clear();f.files.set(path,[1,2,3]);f.matches.set(path,match);
  await f.model.open('th09mp');assert.deepEqual(f.model.getSnapshot().rows[0].replayMatch,match);
  const renaming=f.model.rename(path,'th9_ud0001.rpy');
  f.model.close();const opening=f.model.open('th06');await Promise.all([renaming,opening]);
  assert.deepEqual(f.files.get(target),[1,2,3]);assert.deepEqual(f.matches.get(target),match);
  assert.equal(f.files.has(path),false);
});
test('TH09 shared rpy/rpyx file slots cannot overwrite or delete each other during rename', async () => {
  const f=fixture(),path='replay/th9_ud0000.rpyx';f.files.clear();f.files.set(path,[1,2,3]);f.files.set('replay/th9_ud0001.rpy',[4]);
  await f.model.open('th09mp');await f.model.rename(path,'th9_ud0000.rpy');
  assert.deepEqual(f.files.get(path),[1,2,3]);
  await f.model.rename(path,'th9_ud0001.rpyx');
  assert.deepEqual(f.files.get(path),[1,2,3]);assert.deepEqual(f.files.get('replay/th9_ud0001.rpy'),[4]);
  assert.ok(f.toasts.some(text=>text.includes('replay.nameExists')));
});
test('opens original loading state before paint, lists only sorted replay files, initial animation only', async () => {
  const f = fixture(); const opening = f.model.open('th06');
  assert.equal(f.model.getSnapshot().phase, 'loading'); await opening;
  assert.deepEqual(f.model.getSnapshot().rows.map(x => x.path), ['replay/th6_01.rpy', 'replay/th6_02.rpy']);
  assert.equal(f.model.getSnapshot().animateRows, true);
  assert.deepEqual(f.events.slice(0, 5), ['paint', 'prepare', 'lock', 'sync', 'list:']);
  await f.model.refresh(); assert.equal(f.model.getSnapshot().animateRows, false);
  assert.equal(formatReplayBytes(2), '0.0 KiB'); assert.equal(formatReplayBytes(1048576), '1.00 MiB');
});
test('load failure leaves original failure phase and propagates to action feedback', async () => {
  const f = fixture({broken: true}); await assert.rejects(f.model.open('th06'), /fixture read failure/);
  assert.equal(f.model.getSnapshot().phase, 'error'); assert.deepEqual(f.model.getSnapshot().rows, []);
});
test('rename refreshes current collision list, writes new bytes before removing old path', async () => {
  const f = fixture(); await f.model.open('th06'); f.events.length = 0; f.answer('th6_03.rpy');
  await f.model.rename('replay/th6_01.rpy');
  assert.ok(f.events[0].startsWith('prompt:replay.renamePrompt:th6_01.rpy'));
  assert.deepEqual(f.events.slice(1, 9), ['prepare', 'lock', 'sync', 'list:', 'read:replay/th6_01.rpy', 'write:replay/th6_03.rpy', 'remove:replay/th6_01.rpy', 'unlock']);
  assert.deepEqual(f.files.get('replay/th6_03.rpy'), [1, 1]); assert.equal(f.files.has('replay/th6_01.rpy'), false);
});
test('rename cancellation/empty/invalid/case-only no-op never mutates file storage', async () => {
  const f = fixture(); await f.model.open('th06'); f.events.length = 0;
  await f.model.rename('replay/th6_01.rpy');
  await f.model.rename('replay/th6_01.rpy', ' ');
  await f.model.rename('replay/th6_01.rpy', 'evil.rpy');
  await f.model.rename('replay/th6_01.rpy', 'TH6_01.RPY');
  assert.equal(f.events.some(x => x.startsWith('write:') || x.startsWith('remove:') || x === 'prepare'), false);
  assert.ok(f.toasts.some(x => x.includes('replay.nameEmpty'))); assert.ok(f.toasts.some(x => x.includes('replay.nameInvalid')));
});
test('rename collision fails before read/write/remove', async () => {
  const f = fixture(); await f.model.open('th06'); f.events.length = 0;
  await f.model.rename('replay/th6_01.rpy', 'th6_02.rpy');
  assert.equal(f.events.some(x => x.startsWith('read:') || x.startsWith('write:') || x.startsWith('remove:')), false);
  assert.ok(f.toasts.some(x => x.includes('replay.nameExists')));
});
test('delete danger confirmation cancellation leaves stored replay untouched', async () => {
  const f = fixture(); await f.model.open('th06'); f.confirm(false); f.events.length = 0;
  await f.model.remove('replay/th6_01.rpy');
  assert.deepEqual(f.events, []); assert.equal(f.confirms[0].tone, 'danger'); assert.equal(f.files.has('replay/th6_01.rpy'), true);
});
test('download uses original filename and bytes without replay mutation', async () => {
  const f = fixture({launched: true}); await f.model.open('th06'); await f.model.download('replay/th6_01.rpy');
  assert.deepEqual(f.downloads, [{name: 'th6_01.rpy', bytes: [1, 1], mime: 'application/octet-stream'}]);
  f.model.close(); await tick(); assert.equal(f.events.some(x => x.startsWith('release:')), false);
});
test('owned cold preload closes only after shared mutation queue settles', async () => {
  const f = fixture({cold: true}); await f.model.open('th06'); const gate = deferred();
  const pending = f.model.mutations.run(() => gate.promise); f.model.close(); await tick();
  assert.equal(f.events.some(x => x.startsWith('release:')), false);
  gate.resolve(); await pending; await tick(); assert.equal(f.events.includes('release:th06:1'), true);
});
test('late close cleanup cannot retire a newer Runtime epoch', async () => {
  const f = fixture({cold: true}); await f.model.open('th06'); const gate = deferred();
  const pending = f.model.mutations.run(() => gate.promise); f.model.close(); f.state.epoch = 2;
  gate.resolve(); await pending; await tick(); assert.equal(f.events.some(x => x.startsWith('release:')), false);
});
test('close during cold preparation never republishes rows and retires only acquired epoch', async () => {
  const gate = deferred(), f = fixture({cold: true, prepareGate: gate});
  const opening = f.model.open('th06'); await tick(); f.model.close(); gate.resolve(); await opening; await tick();
  assert.equal(f.model.getSnapshot().open, false); assert.deepEqual(f.model.getSnapshot().rows, []);
  assert.equal(f.events.includes('release:th06:1'), true);
});

test('closing manager waits for an already committed queued rename rather than cancelling it', async () => {
  const f = fixture({cold: true}); await f.model.open('th06');
  const gate = deferred(); const blocker = f.model.mutations.run(() => gate.promise);
  const renamed = f.model.rename('replay/th6_01.rpy', 'th6_03.rpy');
  f.model.close(); gate.resolve(); await blocker; await renamed; await tick();
  assert.deepEqual(f.files.get('replay/th6_03.rpy'), [1, 1]);
  assert.equal(f.files.has('replay/th6_01.rpy'), false);
  assert.equal(f.events.includes('release:th06:1'), true);
});

for (const completeWhileClosed of [false,true]) test(`retained Forward reuses pending read, completed while closed=${completeWhileClosed}`,async()=>{
 const gate=deferred(),f=fixture({prepareGate:gate});const opening=f.model.open('th06');await tick();
 f.model.close();await tick();assert.equal(f.model.isOpen(),false);
 if(completeWhileClosed){gate.resolve();await opening;await tick();}
 f.model.reopenRetained();assert.equal(f.model.isOpen(),true);
 if(!completeWhileClosed){gate.resolve();await opening;}
 await tick();assert.equal(f.model.getSnapshot().phase,'ready');assert.equal(f.model.getSnapshot().rows.length,2);
 assert.equal(f.events.filter(e=>e==='prepare').length,1,'Forward does not duplicate an in-flight/completed read');
 await f.model.rename('replay/th6_01.rpy','th6_03.rpy');assert.ok(f.files.has('replay/th6_03.rpy'));
});
test('retained read completion cannot repaint an explicitly newer manager session',async()=>{
 const gate=deferred(),f=fixture({prepareGate:gate});const old=f.model.open('th06');await tick();f.model.close();f.model.reopenRetained();f.model.close();
 const fresh=f.model.open('th07');await tick();gate.resolve();await Promise.all([old,fresh]);await tick();
 assert.equal(f.model.getSnapshot().productId,'th07');assert.equal(f.model.getSnapshot().open,true);
});
test('cold retained Forward before queued cleanup must release the same temporary epoch on final close',async()=>{
 const f=fixture({cold:true});await f.model.open('th06');const gate=deferred();const pending=f.model.mutations.run(()=>gate.promise);
 f.model.close();f.model.reopenRetained();gate.resolve();await pending;await tick();
 assert.equal(f.events.some(e=>e.startsWith('release:')),false,'Forward keeps the existing temporary owner');
 f.model.close();await tick();assert.equal(f.events.filter(e=>e==='release:th06:1').length,1,'final close releases the retained temporary owner exactly once');
});
for(const replaceAt of ['before-forward','after-forward']) test(`retained cleanup never claims newer epoch ${replaceAt}`,async()=>{
 const f=fixture({cold:true});await f.model.open('th06');const gate=deferred();const pending=f.model.mutations.run(()=>gate.promise);f.model.close();
 if(replaceAt==='before-forward')f.state.epoch=2;
 f.model.reopenRetained();if(replaceAt==='after-forward')f.state.epoch=2;
 gate.resolve();await pending;await tick();f.model.close();await tick();
 assert.equal(f.events.some(e=>e.startsWith('release:')),false);assert.equal(f.state.epoch,2);
});
test('held cold read across close Forward final close releases only its eventually acquired epoch',async()=>{
 const gate=deferred(),f=fixture({cold:true,prepareGate:gate});const opening=f.model.open('th06');await tick();
 f.model.close();f.model.reopenRetained();f.model.close();gate.resolve();await opening;await tick();
 assert.equal(f.events.filter(e=>e==='release:th06:1').length,1);assert.equal(f.state.epoch,null);
});

test('TH09 moves between actual directories and root, allocating colliding shared rpy/rpyx slots without overwriting', async () => {
  const f=fixture(), source='replay/A/th9_ud0000.rpyx';f.files.clear();f.files.set(source,[1,2,3]);f.files.set('replay/B/th9_ud0000.rpy',[9]);
  const match={schema:'eagler-touhou/th09-match-replay/1',names:['x','a'],startedAt:1000,frames:600,winner:1};f.matches.set(source,match);
  await f.model.open('th09mp');assert.equal(await f.model.move(source,'replay/B'),true);
  assert.deepEqual(f.files.get('replay/B/th9_ud0000.rpy'),[9]);assert.deepEqual(f.files.get('replay/B/th9_ud0001.rpyx'),[1,2,3]);
  assert.equal(f.files.has(source),false);assert.deepEqual(f.matches.get('replay/B/th9_ud0001.rpyx'),match);assert.equal(f.matches.has(source),false);
  assert.equal(await f.model.move('replay/B/th9_ud0001.rpyx','replay'),true);assert.deepEqual(f.files.get('replay/th9_ud0001.rpyx'),[1,2,3]);
  assert.deepEqual(f.matches.get('replay/th9_ud0001.rpyx'),match);
});
test('TH09 move rejects invalid destinations and preserves source on write, flush or verification failure', async () => {
  for (const mode of ['failWrite','failSyncAfterWrite','corruptRead']) {
    const f=fixture(),source='replay/th9_01.rpy';f.files.clear();f.files.set(source,[1,2,3]);await f.model.open('th09');
    if(mode==='corruptRead')f.state.corruptRead='replay/backup/th9_01.rpy';else f.state[mode]=true;
    assert.equal(await f.model.move(source,'replay/backup'),false);assert.deepEqual(f.files.get(source),[1,2,3]);assert.ok(f.toasts.length);
  }
  const f=fixture();f.files.clear();f.files.set('replay/th9_01.rpy',[1]);await f.model.open('th09');f.events.length=0;
  for(const target of ['replay/../bad','replay/..','replay/a/b','replay/bad?','/save/replay/a','replay/a.'])assert.equal(await f.model.move('replay/th9_01.rpy',target),false);
  assert.equal(f.events.some(e=>e.startsWith('write:')),false);
});
test('folder ZIP contains only that directory, original bytes and matching sidecars', async () => {
  const {unzipSync}=await import('fflate');const f=fixture();f.files.clear();
  const match={schema:'eagler-touhou/th09-match-replay/1',names:['x','a'],startedAt:1000,frames:600,winner:1};
  f.files.set('replay/A/th9_ud0000.rpy',[1,2]);f.matches.set('replay/A/th9_ud0000.rpy',match);
  f.files.set('replay/A/th9_ud0001.rpyx',[3]);f.files.set('replay/AB/th9_ud0000.rpy',[9]);f.files.set('replay/th9_01.rpy',[8]);
  await f.model.open('th09mp');await f.model.downloadFolder('replay/A');
  assert.equal(f.downloads[0].name,'A.zip');assert.equal(f.downloads[0].mime,'application/zip');
  const zip=unzipSync(new Uint8Array(f.downloads[0].bytes));assert.deepEqual(Object.keys(zip).sort(),['replay/A/th9_ud0000.json','replay/A/th9_ud0000.rpy','replay/A/th9_ud0001.rpyx']);
  assert.deepEqual([...zip['replay/A/th9_ud0000.rpy']],[1,2]);assert.deepEqual(JSON.parse(new TextDecoder().decode(zip['replay/A/th9_ud0000.json'])),match);
});

test('watch preserves selected folder path, waits for mutations and retires its cold file Runtime before playback', async () => {
  const calls=[],f=fixture({cold:true,watch:async(product,path)=>calls.push({product,path,released:f.events.some(e=>e.startsWith('release:'))})});
  f.state.game='th09';const path='replay/收藏/th9_ud0000.rpyx';f.files.clear();f.files.set(path,[1,2,3]);await f.model.open('th09mp');
  const gate=deferred(),pending=f.model.mutations.run(()=>gate.promise),watching=f.model.watch(path);await tick();assert.equal(calls.length,0);gate.resolve();await pending;await watching;
  assert.deepEqual(calls,[{product:'th09mp',path,released:true}]);assert.equal(f.model.getSnapshot().open,false);assert.deepEqual(f.files.get(path),[1,2,3]);
  assert.equal(f.events.some(e=>e.startsWith('write:')||e.startsWith('remove:')),false);
});
test('watch rejects a vanished file and unsupported titles without launching or closing the manager', async () => {
  const calls=[],f=fixture({watch:async(...args)=>calls.push(args)});f.files.clear();f.files.set('replay/th9_ud0000.rpy',[1]);await f.model.open('th09');
  await f.model.watch('replay/missing/th9_ud0000.rpy');assert.equal(calls.length,0);assert.equal(f.model.getSnapshot().open,true);
  await f.model.open('th06');await f.model.watch('replay/th9_ud0000.rpy');assert.equal(calls.length,0);assert.equal(f.model.getSnapshot().open,true);
});

test('watch never replaces an active game through a file-manager click', async () => {
  const calls=[],f=fixture({launched:true,watch:async(...args)=>calls.push(args)});f.files.clear();f.files.set('replay/th9_ud0000.rpy',[1]);await f.model.open('th09mp');await f.model.watch('replay/th9_ud0000.rpy');
  assert.equal(calls.length,0);assert.equal(f.model.getSnapshot().open,true);assert.ok(f.toasts.some(t=>t.includes('replay.stopBeforeWatch')));
});


test('persistent TH09 viewer switches files while keeping the manager and native epoch alive', async()=>{
  const selected=[], f=fixture({viewer:true,watch:async(_product,path)=>selected.push(path)});
  f.state.game='th09'; f.files.clear();for(const n of ['0000','0001'])f.files.set(`replay/th9_ud${n}.rpy`,[1]);
  await f.model.open('th09mp');const epoch=f.state.epoch;
  await f.model.watch('replay/th9_ud0000.rpy');await f.model.watch('replay/th9_ud0001.rpy');
  assert.deepEqual(selected,['replay/th9_ud0000.rpy','replay/th9_ud0001.rpy']);assert.equal(f.model.getSnapshot().open,true);assert.equal(f.state.epoch,epoch);
  assert.equal(f.events.includes('prepare'),false);assert.equal(f.events.includes('closeViewer'),false);
  f.model.close();assert.equal(f.events.filter(e=>e==='closeViewer').length,1);
});
test('closing a cold viewer cancels presentation before its pending resource read resolves',async()=>{
  const gate=deferred(),f=fixture({cold:true,viewer:true,prepareGate:gate});f.state.game='th09';
  const opening=f.model.open('th09');await tick();f.model.close();
  assert.ok(f.events.includes('closeViewer'));assert.equal(f.model.getSnapshot().open,false);
  gate.resolve();await opening;assert.equal(f.model.getSnapshot().open,false);
});

test('TH09 eagerly prepares its player while listing and downloads finish before game readiness',async()=>{
  const reads=[], selected=[], match={schema:'eagler-touhou/th09-match-replay/1',names:['甲','乙'],startedAt:1000,frames:600,winner:1};
  const stored=[{path:'replay/A/th9_ud0000.rpyx',bytes:new Uint8Array([1,2,3]),size:3,modifiedAt:2000,replayMatch:match}];
  const gate=deferred(),f=fixture({cold:true,viewer:true,prepareGate:gate,watch:async(_product,path)=>selected.push(path),storage:{async read(product,selection){reads.push({product,selection});return stored;}}});
  f.state.game='th09';await f.model.open('th09mp');
  assert.equal(f.state.ready,false);assert.deepEqual(f.events,['paint','viewer']);assert.equal(f.model.getSnapshot().phase,'ready');
  assert.equal('bytes' in f.model.getSnapshot().rows[0],false,'the UI retains metadata, not whole Replay byte arrays');
  await f.model.download(stored[0].path);await f.model.downloadFolder('replay/A');
  assert.deepEqual(f.events,['paint','viewer']);assert.equal(f.state.ready,false);assert.deepEqual([...new Uint8Array(f.downloads[0].bytes)],[1,2,3]);
  const {unzipSync}=await import('fflate');const entries=unzipSync(new Uint8Array(f.downloads[1].bytes));
  assert.deepEqual([...entries[stored[0].path]],[1,2,3]);assert.deepEqual(JSON.parse(new TextDecoder().decode(entries['replay/A/th9_ud0000.json'])),match);
  assert.equal(reads.length,3);const watching=f.model.watch(stored[0].path);await tick();
  assert.equal(f.events.filter(e=>e==='viewer').length,1);assert.deepEqual(selected,[],'selection waits for the already-started player');
  gate.resolve();await watching;assert.deepEqual(selected,[stored[0].path]);assert.equal(f.model.isOpen(),true);
});
test('closing during independent listing cancels its viewer and never publishes a late result',async()=>{
  const gate=deferred(), f=fixture({cold:true,viewer:true,storage:{async read(){await gate.promise;return [];}}});f.state.game='th09';
  const opening=f.model.open('th09');await tick();f.model.close();gate.resolve();await opening;await tick();
  assert.equal(f.model.isOpen(),false);assert.equal(f.events.filter(event=>event==='viewer').length,1);
  assert.equal(f.events.filter(event=>event==='closeViewer').length,1);assert.deepEqual(f.model.getSnapshot().rows,[]);
});
test('TH09 game load failure preserves the independent list and downloads',async()=>{
  const gate=deferred(),path='replay/th9_01.rpy',f=fixture({cold:true,viewer:true,prepareGate:gate,storage:{async read(){return [{path,bytes:new Uint8Array([1,2]),size:2}];}}});
  f.state.game='th09';await f.model.open('th09');gate.reject(Error('game resource unavailable'));await tick();
  assert.equal(f.model.getSnapshot().phase,'ready');assert.equal(f.model.getSnapshot().rows[0].path,path);
  await f.model.download(path);assert.deepEqual(f.downloads[0].bytes,[1,2]);assert.equal(f.toasts.length,1);
});

test('closing before game readiness cancels a queued Watch and suppresses late load failures',async()=>{
  const gate=deferred(),selected=[],path='replay/th9_01.rpy',f=fixture({cold:true,viewer:true,prepareGate:gate,watch:async(...args)=>selected.push(args),storage:{async read(){return [{path,bytes:new Uint8Array([1]),size:1}];}}});
  f.state.game='th09';await f.model.open('th09');const watching=f.model.watch(path);await tick();
  f.model.close();gate.reject(Error('cancelled load'));await watching;await tick();
  assert.deepEqual(selected,[]);assert.equal(f.model.isOpen(),false);assert.deepEqual(f.toasts,[]);
});

test('closing independent manager during a committed cold rename releases its eventual file Runtime',async()=>{
  const gate=deferred(),f=fixture({cold:true,prepareGate:gate,storage:{async read(){return [{path:'replay/th9_01.rpy',bytes:new Uint8Array([1]),size:1}];}}});
  f.state.game='th09';f.files.clear();f.files.set('replay/th9_01.rpy',[1]);await f.model.open('th09');
  const rename=f.model.rename('replay/th9_01.rpy','th9_02.rpy');await tick();f.model.close();gate.resolve();await rename;await tick();
  assert.deepEqual(f.files.get('replay/th9_02.rpy'),[1]);assert.equal(f.model.isOpen(),false);
  assert.equal(f.events.filter(event=>event==='release:th09:1').length,1);
});
