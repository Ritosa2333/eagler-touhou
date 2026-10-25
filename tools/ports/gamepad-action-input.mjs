import assert from 'node:assert/strict';
import {createNativeActionInput} from './native-action-input.mjs';

// Production standard profile | launcher/src/launcher/gamepad-mapping.mts.
export const GAMEPAD_ACTION_BUTTONS=Object.freeze({fire:0,bomb:1,focus:4,escape:9});
export function createGamepadActionInput(io,{pulseTicks}={}){
  assert.equal(typeof io.button,'function');
  return createNativeActionInput({set:(action,down)=>io.button(GAMEPAD_ACTION_BUTTONS[action],down),step:io.step},
    {pulseTicks,source:'production-gamepad',describe:action=>({button:GAMEPAD_ACTION_BUTTONS[action]})});
}
