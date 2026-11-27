// 标题主菜单对拍：title-oracle.py 直跑原版 0x44ac70 / 0x44af80 / 0x44b530，
// 与 harness scene-boot 比启动幕结束后静止 30 帧的 Ascii 行、一次光标下移的音效与画面号，
// 以及 10 帧 D3D 结构 + 顶点（draw-compare.py --eps 1e-4）。
// 难度画面（0x44fe20）另对第 344–353 帧：第 313 帧确认 Game Start，第 334 帧进入 Difficulty，
// 第 342 帧光标下移，窗口含下移前后各几帧。
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync, spawnSync} from 'node:child_process';
import {existsSync, mkdtempSync, readFileSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join, resolve} from 'node:path';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const exe = process.env.TH16_EXE ?? resolve(monorepo, 'games/th16/original/th16.exe');
const datDir = resolve(monorepo, 'artifacts/th16/dat');
const py = process.env.TH16_PYTHON ?? '/tmp/th16-venv/bin/python';
const oracle = resolve(here, '../tools/oracle/title-oracle.py');
const compare = resolve(here, '../tools/oracle/draw-compare.py');
const ready = existsSync(dat) && existsSync(exe) && existsSync(py) && existsSync(datDir);
const needOracle = {skip: !ready && 'th16.exe / th16.dat / title oracle python not present'};

const Z = '0x1';
const DOWN = '0x20';
// scene-boot 帧号：第 N 次 logic step 之后。主菜单可输入在第 312 帧（sub==2）。
const MENU_STILL = 312;
// 第 313 帧确认 Game Start；第 334 帧 screen=5（0x44fe20 case 0）；第 342 帧可下移。
const DIFF_DRAW_FROM = 344;
const DIFF_DRAW_TO = 353;
// 主菜单第 313 帧起下移到 Option，第 376 帧确认。第 397 帧进入 Option（0x44c570），
// 第 405 帧子状态 2。窗口取画面稳定后的 425–434。
const OPTION_DRAW_FROM = 425;
const OPTION_DRAW_TO = 434;
// 从主菜单连续点按下键，随后确认进入 Option。
const OPTION_ENTRY = '313:0x20,314:0,315:0x20,316:0,317:0x20,318:0,319:0x20,320:0,321:0x20,322:0,323:0x20,324:0,327:1,328:0';
// Extra 禁用时 ↓×3 到 Replay，第 322 帧确认。第 343 帧进入 Replay（0x451750），第 351 帧子状态 2。
// title-oracle 会把输入保持到下一条，所以每拍后写 0。窗口取稳定后的 360–369。
const REPLAY_DRAW_FROM = 360;
const REPLAY_DRAW_TO = 369;
// ↓×4 到 Player Data，第 324 帧确认。第 345 帧进入（0x452330），第 353 帧子状态 2。窗口 362–371。
const PDATA_DRAW_FROM = 362;
const PDATA_DRAW_TO = 371;
// ↓×5 到 Music Room，第 326 帧确认。第 347 帧进入（0x4546f0），第 362 帧子状态 2。窗口 370–379。
const MUSIC_DRAW_FROM = 370;
const MUSIC_DRAW_TO = 379;
// ↓×2 到 Spell Practice，第 320 帧确认。第 341 帧进入（0x4553d0），第 353 帧子状态 2。窗口 360–369。
const SPELL_DRAW_FROM = 360;
const SPELL_DRAW_TO = 369;
// ↓×7 到 Manual，第 330 帧确认。第 450 帧打开第一页，翻到末页并返回标题。
const MANUAL_DRAW_FROM = 356;
const MANUAL_DRAW_TO = 1220;

function asciiLines(text) {
  return text.split('\n').filter(line => line.startsWith('ascii '));
}

function header(text) {
  const line = text.split('\n').find(l => l.startsWith('scene='));
  return Object.fromEntries(line.split(' ').map(kv => kv.split('=')).map(([k, v]) => [k, Number(v)]));
}

test('title menu: 30 still frames of ascii and one cursor-down match the original', needOracle, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-title-'));
  try {
    const staticJson = join(dir, 'static.json');
    execFileSync(py, [oracle, exe, datDir, '341', staticJson], {stdio: ['ignore', 'ignore', 'pipe']});
    const frames = JSON.parse(readFileSync(staticJson, 'utf8')).frames;
    for (let frame = MENU_STILL; frame < MENU_STILL + 30; ++frame) {
      const row = frames[frame - 1];
      assert.equal(row.screen, 1, `oracle screen @${frame}`);
      assert.equal(row.sub, 2, `oracle sub @${frame}`);
      assert.equal(row.cursor, 0, `oracle cursor @${frame}`);
      const port = execFileSync(harness, ['scene-boot', dat, String(frame), 'ascii'], {encoding: 'utf8'});
      assert.deepEqual(asciiLines(port), row.ascii, `ascii @${frame}`);
    }

    const downJson = join(dir, 'down.json');
    execFileSync(py, [oracle, exe, datDir, '317', downJson, '--input', `313:${DOWN}`], {stdio: ['ignore', 'ignore', 'pipe']});
    const moved = JSON.parse(readFileSync(downJson, 'utf8')).frames[313];
    assert.equal(moved.frame, 314);
    assert.equal(moved.screen, 1);
    assert.equal(moved.cursor, 2);
    assert.deepEqual(moved.sfx, [10]);
    const port = execFileSync(harness, ['scene-boot', dat, '317', `313:${DOWN}`], {encoding: 'utf8'});
    const head = header(port);
    assert.equal(head.screen, 1);
    assert.equal(head.sub, 2);
    assert.equal(head.cursor, 2);
    assert.ok(port.split('\n').some(line => line === 'event sfx 10'));
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('title menu: 10 draw frames match original structure and vertices', needOracle, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-title-draw-'));
  try {
    const state = join(dir, 'state.json');
    const oracleDraw = join(dir, 'oracle.draw.jsonl');
    const portDraw = join(dir, 'port.draw.jsonl');
    execFileSync(py, [oracle, exe, datDir, '321', state, '--draw-out', oracleDraw, '--draw-from', '312'],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    execFileSync(harness, ['scene-boot', dat, '321', `draw=${portDraw}`, 'draw-from=312'], {stdio: ['ignore', 'ignore', 'pipe']});
    execFileSync(py, [compare, oracleDraw, portDraw, '--frames', '312-321', '--eps', '1e-4'],
                 {stdio: ['ignore', 'pipe', 'pipe']});
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('difficulty screen: 10 draw frames around a cursor-down match original structure and vertices', needOracle, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-title-diff-'));
  try {
    const state = join(dir, 'state.json');
    const oracleDraw = join(dir, 'oracle.draw.jsonl');
    const portDraw = join(dir, 'port.draw.jsonl');
    const input = `313:${Z},342:${DOWN}`;
    execFileSync(py, [oracle, exe, datDir, String(DIFF_DRAW_TO), state, '--input', input,
                      '--draw-out', oracleDraw, '--draw-from', String(DIFF_DRAW_FROM)],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    execFileSync(harness, ['scene-boot', dat, String(DIFF_DRAW_TO), input, `draw=${portDraw}`,
                           `draw-from=${DIFF_DRAW_FROM}`], {stdio: ['ignore', 'ignore', 'pipe']});
    const compared = execFileSync(py, [compare, oracleDraw, portDraw, '--frames', `${DIFF_DRAW_FROM}-${DIFF_DRAW_TO}`, '--eps', '1e-4'],
                                  {encoding: 'utf8'});
    process.stdout.write(compared);
    assert.match(compared, /frames=10 mismatches=0/);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('option screen: 10 settled draw frames match original structure and vertices', needOracle, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-title-option-'));
  try {
    const state = join(dir, 'state.json');
    const oracleDraw = join(dir, 'oracle.draw.jsonl');
    const portDraw = join(dir, 'port.draw.jsonl');
    const input = `313:${DOWN},338:${DOWN},346:${DOWN},354:${DOWN},362:${DOWN},370:${DOWN},376:${Z}`;
    execFileSync(py, [oracle, exe, datDir, String(OPTION_DRAW_TO), state, '--input', input,
                      '--draw-out', oracleDraw, '--draw-from', String(OPTION_DRAW_FROM)],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    execFileSync(harness, ['scene-boot', dat, String(OPTION_DRAW_TO), input, `draw=${portDraw}`,
                           `draw-from=${OPTION_DRAW_FROM}`], {stdio: ['ignore', 'ignore', 'pipe']});
    const compared = execFileSync(py, [compare, oracleDraw, portDraw, '--frames', `${OPTION_DRAW_FROM}-${OPTION_DRAW_TO}`, '--eps', '1e-4'],
                                  {encoding: 'utf8'});
    process.stdout.write(compared);
    assert.match(compared, /frames=10 mismatches=0/);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('Option and Key Config: navigation, button swapping and return match original drawing', needOracle, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-key-config-'));
  try {
    const state = join(dir, 'state.json');
    const original = join(dir, 'original.jsonl');
    const port = join(dir, 'port.jsonl');
    const input = `${OPTION_ENTRY},357:0x20,358:0,359:0x20,360:0,361:1,362:0,397:2,398:0,403:0x20,404:0,413:0x10,414:0,423:0x10,424:0,433:2,434:0`;
    execFileSync(py, [oracle, exe, datDir, '454', state, '--input', input,
      '--pad', '399:0xa00,400:0,407:0x200,408:0', '--draw-out', original, '--draw-from', '357'],
    {stdio: ['ignore', 'ignore', 'pipe']});
    const result = execFileSync(harness, ['scene-boot', dat, '454', `${input},P399:9,P399:11,P407:9`,
      `draw=${port}`, 'draw-from=357'], {encoding: 'utf8'});
    assert.deepEqual(result.match(/^event key-button \d+$/gm), ['event key-button 9', 'event key-button 9']);
    assert.equal(header(result).screen, 3);
    const compared = execFileSync(py, [compare, original, port, '--frames', '357-454', '--eps', '0'], {encoding: 'utf8'});
    assert.match(compared, /frames=98 mismatches=0/);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('Option: SE previews follow the original menu timer and stop when the cursor leaves', needOracle, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-option-preview-'));
  try {
    const state = join(dir, 'state.json');
    const input = `${OPTION_ENTRY},357:0x20,358:0,490:0x10,491:0,550:0x20,551:0`;
    execFileSync(py, [oracle, exe, datDir, '596', state, '--input', input], {stdio: ['ignore', 'ignore', 'pipe']});
    const frames = JSON.parse(readFileSync(state, 'utf8')).frames;
    const previews = frames.filter(frame => frame.screen === 3 && frame.sfx.includes(2)).map(frame => frame.frame);
    assert.deepEqual(previews, [416, 476, 596]);
    for (const frame of [415, 416, 475, 476, 536, 595, 596]) {
      const result = execFileSync(harness, ['scene-boot', dat, String(frame), input], {encoding: 'utf8'});
      const observed = result.match(/^event sfx 2$/gm)?.length ?? 0;
      assert.equal(observed, previews.filter(preview => preview <= frame).length, `SE previews through frame ${frame}`);
    }
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('replay screen: 10 settled draw frames match original structure and vertices', needOracle, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-title-replay-'));
  try {
    const state = join(dir, 'state.json');
    const oracleDraw = join(dir, 'oracle.draw.jsonl');
    const portDraw = join(dir, 'port.draw.jsonl');
    const input = `314:${DOWN},315:0,316:${DOWN},317:0,318:${DOWN},319:0,322:${Z},323:0`;
    execFileSync(py, [oracle, exe, datDir, String(REPLAY_DRAW_TO), state, '--input', input,
                      '--draw-out', oracleDraw, '--draw-from', String(REPLAY_DRAW_FROM)],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    execFileSync(harness, ['scene-boot', dat, String(REPLAY_DRAW_TO), input, `draw=${portDraw}`,
                           `draw-from=${REPLAY_DRAW_FROM}`], {stdio: ['ignore', 'ignore', 'pipe']});
    const compared = execFileSync(py, [compare, oracleDraw, portDraw, '--frames', `${REPLAY_DRAW_FROM}-${REPLAY_DRAW_TO}`, '--eps', '1e-4'],
                                  {encoding: 'utf8'});
    process.stdout.write(compared);
    assert.match(compared, /frames=10 mismatches=0/);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('player data screen: 10 settled draw frames match original structure and vertices', needOracle, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-title-pdata-'));
  try {
    const state = join(dir, 'state.json');
    const oracleDraw = join(dir, 'oracle.draw.jsonl');
    const portDraw = join(dir, 'port.draw.jsonl');
    const input = `314:${DOWN},315:0,316:${DOWN},317:0,318:${DOWN},319:0,320:${DOWN},321:0,324:${Z},325:0`;
    execFileSync(py, [oracle, exe, datDir, String(PDATA_DRAW_TO), state, '--input', input,
                      '--draw-out', oracleDraw, '--draw-from', String(PDATA_DRAW_FROM)],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    execFileSync(harness, ['scene-boot', dat, String(PDATA_DRAW_TO), input, `draw=${portDraw}`,
                           `draw-from=${PDATA_DRAW_FROM}`], {stdio: ['ignore', 'ignore', 'pipe']});
    const compared = execFileSync(py, [compare, oracleDraw, portDraw, '--frames', `${PDATA_DRAW_FROM}-${PDATA_DRAW_TO}`, '--eps', '1e-4'],
                                  {encoding: 'utf8'});
    process.stdout.write(compared);
    assert.match(compared, /frames=10 mismatches=0/);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('music room: 10 settled draw frames match original structure and vertices', needOracle, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-title-music-'));
  try {
    const state = join(dir, 'state.json');
    const oracleDraw = join(dir, 'oracle.draw.jsonl');
    const portDraw = join(dir, 'port.draw.jsonl');
    const input = `314:${DOWN},315:0,316:${DOWN},317:0,318:${DOWN},319:0,320:${DOWN},321:0,322:${DOWN},323:0,326:${Z},327:0`;
    execFileSync(py, [oracle, exe, datDir, String(MUSIC_DRAW_TO), state, '--input', input,
                      '--draw-out', oracleDraw, '--draw-from', String(MUSIC_DRAW_FROM)],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    execFileSync(harness, ['scene-boot', dat, String(MUSIC_DRAW_TO), input, `draw=${portDraw}`,
                           `draw-from=${MUSIC_DRAW_FROM}`], {stdio: ['ignore', 'ignore', 'pipe']});
    const compared = execFileSync(py, [compare, oracleDraw, portDraw, '--frames', `${MUSIC_DRAW_FROM}-${MUSIC_DRAW_TO}`, '--eps', '1e-4'],
                                  {encoding: 'utf8'});
    process.stdout.write(compared);
    assert.match(compared, /frames=10 mismatches=0/);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('spell practice: 10 settled draw frames match original structure and vertices', needOracle, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-title-spell-'));
  try {
    const state = join(dir, 'state.json');
    const oracleDraw = join(dir, 'oracle.draw.jsonl');
    const portDraw = join(dir, 'port.draw.jsonl');
    const input = `314:${DOWN},315:0,316:${DOWN},317:0,320:${Z},321:0`;
    execFileSync(py, [oracle, exe, datDir, String(SPELL_DRAW_TO), state, '--input', input,
                      '--draw-out', oracleDraw, '--draw-from', String(SPELL_DRAW_FROM)],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    execFileSync(harness, ['scene-boot', dat, String(SPELL_DRAW_TO), input, `draw=${portDraw}`,
                           `draw-from=${SPELL_DRAW_FROM}`], {stdio: ['ignore', 'ignore', 'pipe']});
    const compared = execFileSync(py, [compare, oracleDraw, portDraw, '--frames', `${SPELL_DRAW_FROM}-${SPELL_DRAW_TO}`, '--eps', '1e-4'],
                                  {encoding: 'utf8'});
    process.stdout.write(compared);
    assert.match(compared, /frames=10 mismatches=0/);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('Spell Practice: cancel hides the list scores as soon as the exit animation starts', needOracle, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-title-spell-exit-'));
  try {
    const state = join(dir, 'state.json');
    const original = join(dir, 'original.jsonl');
    const port = join(dir, 'port.jsonl');
    const input = `314:${DOWN},315:0,316:${DOWN},317:0,320:${Z},321:0,360:0x80,361:0,367:${DOWN},368:0,375:${Z},376:0,415:${Z},416:0,422:${DOWN},423:0,430:0x80,431:0,445:2,446:0,490:2,491:0`;
    const from = '341';
    const to = '525';
    execFileSync(py, [oracle, exe, datDir, to, state, '--input', input, '--draw-out', original, '--draw-from', from],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    const frames = JSON.parse(readFileSync(state, 'utf8')).frames;
    assert.equal(frames[444].ascii.filter(line => line.includes('[SCORE')).length, 4);
    assert.ok(frames.slice(445, 451).every(frame => frame.screen === 18 && frame.sub === 4 && frame.ascii.length === 0));
    execFileSync(harness, ['scene-boot', dat, to, input, `draw=${port}`, `draw-from=${from}`],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    const compared = execFileSync(py, [compare, original, port, '--frames', `${from}-${to}`, '--eps', '0'], {encoding: 'utf8'});
    assert.match(compared, /frames=185 mismatches=0/);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('manual screen: nine pages, page bounds and return to title match original drawing', needOracle, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-title-manual-'));
  try {
    const state = join(dir, 'state.json');
    const oracleDraw = join(dir, 'oracle.draw.jsonl');
    const portDraw = join(dir, 'port.draw.jsonl');
    const keys = [
      ...Array.from({length: 7}, (_, i) => [314 + i * 2, DOWN]),
      [330, Z], [395, DOWN], [415, '0x10'], [450, Z],
      ...Array.from({length: 8}, (_, i) => [502 + i * 50, DOWN]),
      [902, DOWN], [952, '0x10'], [1002, DOWN], [1052, '0x2'], [1108, '0x2'],
    ];
    const input = keys.flatMap(([frame, key]) => [`${frame}:${key}`, `${frame + 1}:0`]).join(',');
    execFileSync(py, [oracle, exe, datDir, String(MANUAL_DRAW_TO), state, '--input', input,
                      '--draw-out', oracleDraw, '--draw-from', String(MANUAL_DRAW_FROM)],
                 {stdio: ['ignore', 'ignore', 'pipe']});
    const port = execFileSync(harness, ['scene-boot', dat, String(MANUAL_DRAW_TO), input, `draw=${portDraw}`,
                                        `draw-from=${MANUAL_DRAW_FROM}`], {encoding: 'utf8'});
    const compared = spawnSync(py, [compare, oracleDraw, portDraw, '--frames', `${MANUAL_DRAW_FROM}-${MANUAL_DRAW_TO}`, '--eps', '1e-4'],
                               {encoding: 'utf8'});
    if (compared.error) throw compared.error;
    assert.equal(compared.status, 0, compared.stdout + compared.stderr);
    const frameCount = MANUAL_DRAW_TO - MANUAL_DRAW_FROM + 1;
    assert.ok(compared.stdout.includes(`frames=${frameCount} mismatches=0`), compared.stdout);
    const recording = JSON.parse(readFileSync(state, 'utf8'));
    const pages = [1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 9];
    assert.deepEqual(recording.manual_page_reads, pages.map(page => `help_${String(page).padStart(2, '0')}.png`));
    const last = recording.frames.at(-1);
    assert.equal(last.screen, 1, 'original returned to the title menu');
    const portState = header(port);
    assert.deepEqual([portState.screen, portState.sub, portState.cursor], [last.screen, last.sub, last.cursor]);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});
