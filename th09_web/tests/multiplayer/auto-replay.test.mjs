import test from 'node:test';
import assert from 'node:assert/strict';
import {createAutoReplay, parseReplayMatch} from '../../sdl-runtime/auto-replay.mjs';
function fixture(extra = {}) {
  const files = new Map(), notices = [], writes = [];
  let opts = {netplayReplayNames: ['晚风', 'abdjfal']}, failed = false, flushFailed = false;
  const root = '/savesth09mp/replay/晚风 vs abdjfal/';
  const folders = new Set();
  const core = {HEAPU8: new Uint8Array(1024), _th09_auto_replay_folder_buffer: () => 16, FS: {
    mkdirTree: path => folders.add(path),
    readdir: directory => [...files.keys()].filter(path => path.startsWith(directory+'/')).map(path => path.slice(directory.length+1)),
    readFile(path) {if (!files.has(path)) throw Error('missing'); return files.get(path);},
    writeFile: (path, data) => files.set(path, data),
    unlink: path => files.delete(path),
  }, _th09_auto_save_replay(slot) {
    writes.push(slot); if (failed) return 0;
    const end=core.HEAPU8.indexOf(0,16),folder=new TextDecoder().decode(core.HEAPU8.subarray(16,end));
    files.set('/savesth09mp/replay/'+folder+'/' + `th9_ud${slot.toString(16).padStart(4, '0')}.rpy`, new Uint8Array([84, 57])); return 1;
  }};
  const archive = createAutoReplay({core, saveRoot: '/savesth09mp', options: () => opts,
    enabled: () => true, now: () => 1792029600000,
    sync: () => flushFailed ? Promise.reject(Error('quota')) : Promise.resolve(), notice: text => notices.push(text), ...extra});
  return {archive, files, writes, notices, root, folders, options: value => opts = {...opts, ...value}, fail: () => failed = true, failFlush: () => flushFailed = true};
}
test('default on, snapshots names, ignores duplicate result, consecutive matches do not overwrite', async () => {
  const f = fixture(); f.files.set(f.root + 'th9_ud0000.rpy', new Uint8Array([123]));
  f.archive.begin(0); f.options({netplayReplayNames: ['改名', '新名']});
  f.archive.end(600, 0); f.archive.end(600, 0); await Promise.resolve();
  assert.deepEqual(f.writes, [1]);
  assert.equal(f.files.get(f.root + 'th9_ud0000.rpy')[0], 123);
  assert.deepEqual(f.archive.readMatch(f.root + 'th9_ud0001.rpy').names, ['晚风', 'abdjfal']);
  f.archive.begin(700); f.archive.end(1300, 1); await Promise.resolve();
  assert.deepEqual(f.writes, [1, 0]);
  assert.ok(f.folders.has('/savesth09mp/replay/改名 vs 新名'));
  assert.deepEqual(f.archive.readMatch('/savesth09mp/replay/改名 vs 新名/th9_ud0000.rpyx').names, ['改名', '新名']);
});
test('disabled setting, spectator and incomplete/invalid results never save', () => {
  const f = fixture(); f.archive.begin(0);
  f.options({netplayReplayAutoSave: false}); f.archive.end(600, 0);
  f.options({netplayReplayAutoSave: true, netplaySpectator: true}); f.archive.end(600, 0);
  f.options({netplaySpectator: false}); f.archive.end(600, -1); f.archive.end(0, 1);
  assert.deepEqual(f.writes, []);
});
test('failed write and persistence failures are notices; archive does not throw or terminate transport', async () => {
  const f = fixture(); f.fail(); f.archive.begin(0); assert.doesNotThrow(() => f.archive.end(600, 0));
  assert.match(f.notices[0], /失败/); assert.equal(f.files.size, 0);
  const g = fixture(); g.failFlush(); g.archive.begin(0); g.archive.end(600, 0); await Promise.resolve();
  assert.match(g.notices[0], /尚未写入/); assert.equal(g.files.size, 2);
});
test('sidecar validation, rename transfer and deletion preserve archive pairing', () => {
  const f = fixture(); f.archive.begin(0); f.archive.end(600, 0);
  const first = f.root + 'th9_ud0000.rpy', target = f.root + 'th9_udabcd.rpy';
  f.archive.writeMatch(target, f.archive.readMatch(first)); f.archive.removeMatch(first);
  assert.equal(f.archive.readMatch(first), null); assert.equal(f.archive.readMatch(target).winner, 0);
  assert.equal(parseReplayMatch({schema: 'wrong'}), null);
  f.files.set('/savesth09mp/score.dat',new Uint8Array([123]));
  f.archive.removeMatch('/savesth09mp/score.dat');f.archive.writeMatch('/savesth09mp/score.dat',f.archive.readMatch(target));
  assert.deepEqual([...f.files.get('/savesth09mp/score.dat')],[123]);
});

test('names form a real safe directory and separate matchups allocate independent slots', () => {
  const f=fixture();f.options({netplayReplayNames:['../a:b','x/y.']});f.archive.begin(0);f.archive.end(600,0);
  assert.ok([...f.files.keys()].every(path=>path.startsWith('/savesth09mp/replay/.._a_b vs x_y/')));
});
