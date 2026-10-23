import test from 'node:test';
import assert from 'node:assert/strict';
import {GAIN_CALIBRATION_CASES,assertPresentationGainCalibration} from './presentation-gain-calibration.mjs';

function fixture(){return {schema:'eagler-presentation/gain-calibration/1',method:'future-frame-offline-render',sampleRate:48000,startFrame:256,frames:512,
  rows:GAIN_CALIBRATION_CASES.map(item=>({id:item.id,channels:[item.left,item.right].map((gain,channel)=>Array.from({length:512},(_,frame)=>
    Math.fround(Math.fround(item.samples[frame-256]?.[channel]??0)*Math.fround(gain))))}))};}

test('controlled calibration checks the complete output, channel routing and frame boundary',()=>{
  assert.equal(assertPresentationGainCalibration(fixture()).status,'passed');
  for(const mutate of [c=>{c.rows[0].channels.reverse();},c=>{c.rows[0].channels[0][255]=.1;},
    c=>{c.rows[0].channels[0][256]=0;},c=>{c.rows[0].channels[0][300]=.01;},c=>{c.rows[0].channels[0].pop();}]){
    const capture=fixture();mutate(capture);assert.throws(()=>assertPresentationGainCalibration(capture));
  }
});

test('double-precision gain multiplication and a one-ULP deviation cannot pass calibration',()=>{
  const capture=fixture(),item=GAIN_CALIBRATION_CASES[1];
  capture.rows[1].channels[0][256]=Math.fround(Math.fround(item.samples[0][0])*item.left);
  assert.throws(()=>assertPresentationGainCalibration(capture),/controlled gain PCM differs/);
  const shifted=fixture(),value=Float32Array.of(shifted.rows[1].channels[0][256]),bits=new Uint32Array(value.buffer);
  bits[0]++;shifted.rows[1].channels[0][256]=value[0];
  assert.throws(()=>assertPresentationGainCalibration(shifted),/controlled gain PCM differs/);
});
