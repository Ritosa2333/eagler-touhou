import test from 'node:test';import assert from 'node:assert/strict';
import {LoadingPresentation} from '../src/loading-presentation.mjs';
import {Renderer} from '../src/renderer.mjs';
test('stage resource wait preserves its supplied HUD base and releases it after loading',()=>{
  const calls=[],loading=new LoadingPresentation({atlases:{},renderMenu:()=>calls.push('black')});
  loading.showIndicator=()=>{};loading.renderOverlay=()=>calls.push('indicator');
  loading.startResources({renderBase:()=>calls.push('held HUD')});loading.renderBase();
  assert.deepEqual(calls,['held HUD','indicator']);
  loading.completeResources();calls.length=0;loading.startResources();loading.renderBase();
  assert.deepEqual(calls,['black','indicator']);
});
test('field loading shade leaves the HUD outside the original 384x448 playfield untouched',()=>{
  let vertices;const renderer={gl:{SCISSOR_TEST:1,disable(){},uniform3fv(){}},fogColor:0,
    solidTriangles:value=>vertices=value,flush(){},nativeText(){}};
  Renderer.prototype.renderMenu.call(renderer,[],[],null,{overlay:true,shade:1,shadeBounds:[32,16,416,464]});
  assert.deepEqual(vertices.map(v=>[v.x,v.y]),[[32,16],[416,16],[416,464],[32,464]]);
  assert(vertices.every(v=>v.color[3]===1));
});
