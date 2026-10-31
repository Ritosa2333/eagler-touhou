import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {animationLayerPriority,orderPresentation} from '../src/draw-scheduler.mjs';

const native=JSON.parse(await readFile(new URL('fixtures/render-schedule-native.json',import.meta.url),'utf8'));
assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');

test('all 28 original layer registrations agree with the portable draw scheduler',()=>{
  assert.equal(native.nativeLayerRegistrations.length,28);
  assert.equal(native.layers.length,28);
  for(const row of native.layers)assert.equal(animationLayerPriority(row.layer),row.priority,`layer${row.layer}`);
  const labels=['layer15','bullet','layer14','laser','layer3','layer21','hud','layer31'];
  const poses=labels.map(label=>label.startsWith('layer')?{label,a:{layer:Number(label.slice(5))}}:
    {label,drawPriority:{bullet:31,laser:29,hud:69}[label]});
  assert.deepEqual(orderPresentation(poses).map(p=>p.label),
    ['layer3','layer14','laser','layer15','bullet','layer21','layer31','hud']);
});

test('native wrappers inherit stage viewport through layer20 and independently switch 21/31/30',()=>{
  assert.deepEqual(native.cameraProfiles[1].viewports,[[32,16,384,448],[0,0,640,480],[13,0,422,480]]);
  for(const row of native.layers){
    const draws=row.events.filter(e=>e.kind==='anm-draw');
    // Original4611d0 skips bit10000000, and invokes the custom callback
    // before its owner's submission. A parent pointer causes no recursion.
    assert.deepEqual(draws.map(e=>e.vm),[row.inputChain[0],row.inputChain[2],row.inputChain[3]]);
    assert.equal(row.events.findIndex(e=>e.kind==='custom-draw')<row.events.findIndex(e=>e.kind==='anm-draw'),true);
    for(const e of draws)assert.deepEqual(e.viewport,row.layer<=20?[13,0,422,480]:row.layer===31?[32,16,384,448]:[0,0,640,480]);
    if(row.layer===31)assert.equal(row.cameraAfter,1);
  }
  assert.equal(native.playerBody.events[0].layer,99);
  assert.deepEqual(native.playerBody.events[0].viewport,[13,0,422,480]);
});

test('native font queue flag and camera flag are independent, including first-record inheritance',()=>{
  assert.equal(native.fontQueues.length,8);
  for(const row of native.fontQueues){
    const draws=row.events.filter(e=>e.kind==='font-draw');
    assert.deepEqual(draws.map(e=>e.index),row.inputs.map((e,i)=>e.queue===row.queue?i:null).filter(i=>i!==null));
    const first=draws[0];
    assert.deepEqual(first.viewport,first.cameraFlag?[13,0,422,480]:[0,0,640,480]);
    assert.equal(row.cameraAfter,1);
    assert.equal(row.priority,row.queue?45:69);
  }
});
