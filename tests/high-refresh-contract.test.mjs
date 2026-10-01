import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import test from 'node:test';

const read=path=>readFileSync(new URL('../'+path,import.meta.url),'utf8').replaceAll('\r','');
const host=read('th10_web/cpp/sdl/ApplicationHost.cpp');
const script=read('portable/check-high-refresh-contract.mjs');
const contract=script.split('// BEGIN CADENCE SOURCE CONTRACT')[1].split('// END CADENCE SOURCE CONTRACT')[0];
const check=new Function('assert','host',contract);

test('current source keeps ordinary single-tick and multiplayer debt paths separate',()=>check(assert,host));
test('an ordinary catch-up loop cannot pass the source contract',()=>{
 const changed=host.replace('if((tick_due=cadence.advance(simulation_delta)!=0)){',
  'for(unsigned extra=0;extra<2;++extra) if((tick_due=cadence.advance(simulation_delta)!=0)){');
 assert.notEqual(changed,host);assert.throws(()=>check(assert,changed));
});
test('unbounded multiplayer catch-up and charging stalled debt are rejected',()=>{
 for(const [before,after] of [
  ['Netplay::FrameBudget::CanStartTick','UnboundedTick'],
  ['if(result||runtime.LastSimulatedFrame()!=before+1u)break;','if(result)break;'],
 ]){
  const changed=host.replace(before,after);assert.notEqual(changed,host);assert.throws(()=>check(assert,changed));
 }
});
test('a nested preprocessor guard fails closed rather than hiding ordinary work',()=>{
 const changed=host.replace('if(live){','#ifdef UNKNOWN_FEATURE\n    if(live){');
 assert.notEqual(changed,host);assert.throws(()=>check(assert,changed));
});
