import test, {before, after} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {launchBrowser} from './launch.mjs';

const source = await readFile(new URL('../../common/include/eagler/audio/WebSfx.hpp', import.meta.url), 'utf8');
const begin = '// BEGIN_WEB_SFX_JS', end = '// END_WEB_SFX_JS';
assert.equal(source.split(begin).length, 2);
assert.equal(source.split(end).length, 2);
const body = source.split(begin)[1].split(end)[0];
let browser;
before(async () => {browser = await launchBrowser();});
after(async () => {await browser?.close();});

async function render(commandBody) {
  const page = await browser.newPage();
  const errors = [];
  page.on('pageerror', error => errors.push(String(error)));
  page.on('console', message => {if (message.type() === 'error') errors.push(message.text());});
  try {
    const result = await page.evaluate(async commandBody => {
      const rate = 48000, frames = 3072;
      const ctx = new OfflineAudioContext(2, frames, rate);
      const Module = {SDL3: {audioContext: ctx}}, heap = new Uint8Array(frames * 2 * 4);
      const pcm = new Float32Array(heap.buffer);
      for (let i = 0; i < frames; i++) {pcm[i * 2] = .25; pcm[i * 2 + 1] = -.5;}
      const command = new Function('Module', 'HEAPU8', 'op', 'id', 'pcm', 'frames', 'channels', 'rate', 'format', 'value', commandBody);
      const call = (op, value = 0) => command(Module, heap, op, 1, 0, frames, 2, rate, 0, value);

      // A separately scheduled track continues while the production SFX pool pauses.
      const background = ctx.createBuffer(2, 2304, rate);
      background.getChannelData(0).fill(.125); background.getChannelData(1).fill(-.125);
      const track = ctx.createBufferSource(); track.buffer = background; track.connect(ctx.destination); track.start();
      const pauses = [512, 1024, 1536, 1792, 2048].map(frame => ctx.suspend(frame / rate));
      const uploaded = call(0); call(2);
      const rendering = ctx.startRendering(), cursors = [];
      for (let i = 0; i < pauses.length; i++) {
        await pauses[i];
        if (i === 0) {call(10, 1); cursors.push(call(8));}
        if (i === 1) {cursors.push(call(8)); call(5, .4); call(6, -.5); call(10, 0);}
        if (i === 2) call(3);
        if (i === 3) call(2);
        if (i === 4) call(12);
        await ctx.resume();
      }
      const rendered = await rendering;
      const windows = [[0, 512, .375, -.625], [512, 1024, .125, -.125], [1024, 1536, .225, -.225],
        [1536, 1792, .125, -.125], [1792, 2048, .225, -.225], [2048, 2304, .125, -.125], [2304, frames, 0, 0]];
      return {uploaded, cursors, voicesAfterClear: Module.eaglerSfxOutput.voices.size,
        windows: windows.map(([start, end, left, right]) => {
          let deviation = 0;
          for (let channel = 0; channel < 2; channel++) {
            const samples = rendered.getChannelData(channel), expected = channel ? right : left;
            for (let i = start; i < end; i++) deviation = Math.max(deviation, Math.abs(samples[i] - expected));
          }
          return {start, end, deviation};
        })};
    }, commandBody);
    assert.deepEqual(errors, []);
    return result;
  } finally {await page.close();}
}

function assertOutput(result) {
  assert.equal(result.uploaded, 1);
  for (const window of result.windows) assert.ok(window.deviation < 1e-6,
    `PCM window ${window.start}..${window.end} differs by ${window.deviation}`);
  for (const cursor of result.cursors) assert.ok(Math.abs(cursor - 512) < 1e-6, `Paused cursor: ${cursor}`);
  assert.equal(result.voicesAfterClear, 0);
}

test('production WebSfx renders immediate stereo output, gain, pause/resume, stop and clear', {timeout: 30000}, async () => {
  assertOutput(await render(body));
});

test('output assertions reject an SFX start delayed behind a queued track', {timeout: 30000}, async () => {
  const start = 'node.start(0, v.offset);';
  assert.equal(body.split(start).length, 2);
  const broken = body.replace(start, 'node.start(512 / ctx.sampleRate, v.offset);');
  const result = await render(broken);
  assert.throws(() => assertOutput(result), /PCM window 0\.\.512/);
});

test('SFX renders while a separate music loading/decoding task is unresolved', {timeout: 30000}, async () => {
  const page=await browser.newPage();
  try {
    const result=await page.evaluate(async body=>{
      const ctx=new OfflineAudioContext(2,512,48000),heap=new Uint8Array(512*2*4);
      new Float32Array(heap.buffer).fill(.25);
      let completeMusic,musicReady=false;
      const music=new Promise(resolve=>{completeMusic=resolve;}).then(()=>{musicReady=true;});
      const Module={SDL3:{audioContext:ctx}};
      const command=new Function('Module','HEAPU8','op','id','pcm','frames','channels','rate','format','value',body);
      const call=op=>command(Module,heap,op,1,0,512,2,48000,0,0);
      const uploaded=call(0),started=call(2),rendered=await ctx.startRendering();
      const pending=!musicReady;
      let deviation=0;
      for(let channel=0;channel<2;channel++)for(const sample of rendered.getChannelData(channel))deviation=Math.max(deviation,Math.abs(sample-.25));
      completeMusic();await music;call(12);
      return {uploaded,started,pending,deviation};
    },body);
    assert.deepEqual(result,{uploaded:1,started:1,pending:true,deviation:0});
  } finally {await page.close();}
});
