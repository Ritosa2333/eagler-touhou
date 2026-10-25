import assert from 'node:assert/strict';
import {createNativeActionInput} from './native-action-input.mjs';
const keys={focus:'ShiftLeft',fire:'KeyZ',bomb:'KeyX',escape:'Escape'};

/** Physical keys run the shared native action scenarios without touch-controls. */
export function createKeyboardActionInput(io,{pulseTicks,codes={}}={}){
  assert.equal(typeof io.key,'function');
  return createNativeActionInput({set:(action,down)=>io.key(codes[keys[action]]??keys[action],down),step:io.step},
    {pulseTicks,source:'physical-keyboard',describe:action=>({code:keys[action],physical:codes[keys[action]]??keys[action]})});
}
