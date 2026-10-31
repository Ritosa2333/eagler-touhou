import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {asciiGlyphs} from '../src/ascii-presentation.mjs';
import {fpsTextDescriptors,createFrameRateMeter} from '../src/fps-presentation.mjs';
import {parseReplay,recordedReplayFps} from '../src/replay.mjs';

test('FPS font0 uses the actual original glyph expansion and nine-pixel advance',()=>{
  const native=JSON.parse(fs.readFileSync(new URL('fixtures/fps-font-glyphs-native.json',import.meta.url)));
  for(const record of native.records){assert.deepEqual(asciiGlyphs(record.input),record.glyphs);assert.equal(record.rngUnchanged,true);}
});

test('recorded FPS follows original playback byte reads, including the first-frame boundary',()=>{
  const native=JSON.parse(fs.readFileSync(new URL('fixtures/replay-input-order-original.json',import.meta.url)));
  const samples=Uint8Array.of(60,59,58,57),stage={frames:91,fpsOffset:0,fpsCount:4};
  for(const read of native.records[0].steps)
    assert.equal(recordedReplayFps(samples,stage,Math.max(0,read.stageCursor)),read.fps,read.step);
  for(const [cursor,value] of [[30,59],[31,59],[32,58],[61,58],[62,57],[90,57],[91,57]])
    assert.equal(recordedReplayFps(samples,stage,cursor),value);
  for(const cursor of [-1,92,NaN,1.5])assert.equal(recordedReplayFps(samples,stage,cursor),null);
  assert.equal(recordedReplayFps(samples,null,1),null);
  const replay=parseReplay(fs.readFileSync(new URL('../site/replays/demo2.rpy',import.meta.url))),record=replay.stages[0];
  const before=Buffer.from(replay.data);
  assert.equal(replay.recordedFps(record.number,1),replay.data[record.inputOffset+record.frames*6]);
  replay.recordedFps(record.number,60);replay.recordedFps(record.number,1);
  assert.deepEqual(Buffer.from(replay.data),before,'drawing and seeking do not modify the replay');
});

test('wall-time FPS counts presented frames and resets around replay seeks',()=>{
  const meter=createFrameRateMeter();meter.reset(0);
  for(let i=1;i<=60;i++)meter.frame(i*1000/60);
  assert.equal(meter.value,60);
  meter.reset(60000);assert.equal(meter.value,60);
  for(let i=1;i<=30;i++)meter.frame(60000+i*1000/30);
  assert.equal(meter.value,30,'seek time and synthetic steps are excluded');
  assert.equal(meter.frame(NaN),30);
  meter.reset(70000);for(let i=1;i<=120;i++)meter.frame(70000+i*1000/120);
  assert.equal(meter.value,120,'measures presentation independently of fixed60 Hz simulation');
});

test('FPS counters stay in their separate original screen locations with their native color boundaries',()=>{
  assert.deepEqual(fpsTextDescriptors(),[]);
  const atlas=JSON.parse(fs.readFileSync(new URL('../site/assets/atlases.json',import.meta.url))).ascii;
  const [recorded,current]=fpsTextDescriptors({recordedFps:60,currentFps:60});
  assert.deepEqual([recorded.text,recorded.x,recorded.y],[' 60',383,450]);
  assert.deepEqual([current.x,current.y],[588,470]);assert.match(current.text,/^60\.[01]fps$/);
  assert.equal(current.drawPriority,69,'native401720 selects font1 and leaves the regular text queue enabled');
  for(const descriptor of [recorded,current])for(const glyph of asciiGlyphs(descriptor))assert.ok(atlas.sprites[glyph.id],'complete original ASCII glyphs including fps');
  const fast=fpsTextDescriptors({currentFps:144})[0];
  for(const glyph of asciiGlyphs(fast))assert.ok(glyph.x+atlas.sprites[glyph.id].w<=640,'high-refresh FPS stays fully visible');
  for(const [value,color] of [[29,0xff5050ff],[30,0xffa0a0ff],[49,0xffa0a0ff],[50,0xffffffff]])
    assert.equal(fpsTextDescriptors({recordedFps:value})[0].colorARGB,color);
  for(const [value,color] of [[30,0xff5050ff],[30.1,0xffa0a0ff],[39.9,0xffa0a0ff],[40,0xffffffff]])
    assert.equal(fpsTextDescriptors({currentFps:value})[0].colorARGB,color);
});
