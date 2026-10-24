import assert from 'node:assert/strict';
import test from 'node:test';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {touchMovementContract} from './movement.mjs';
import {mapFloat32TouchDragTarget} from '../../../tools/ports/touch-motion-contract.mjs';
import {TH10_MOVEMENT_CONTRACT} from './titles/th10.mjs';

test('every STG product declares native arithmetic independently of its observed result',()=>{
  for(const [game,product] of Object.entries(PRODUCT_GAMES)){
    if(product.inputProfile==='fighting')continue;
    const contract=touchMovementContract(game,'normal');
    assert.ok(contract.source.includes(`ports/${game}/`));
    assert.ok(Number.isInteger(contract.precision.unitsPerPixel));
  }
  assert.equal(touchMovementContract('th10','normal').precision.unitsPerPixel,100);
  assert.deepEqual(touchMovementContract('th10','normal'),TH10_MOVEMENT_CONTRACT);
  assert.equal(touchMovementContract('th15','normal').precision.unitsPerPixel,128);
  assert.equal(touchMovementContract('th06','normal').inputBoundary,'unknown',
    'a source-anchored arithmetic rule does not imply a verified input-consumption boundary');
  assert.deepEqual(touchMovementContract('th08','normal').keyboardReleasePolicy,{
    semantics:'queued-for-next-movement',
    observationSource:'diagnostics[11] | player_state.input.buttons',
    observationPhase:'after-application-update',
    directionMasks:{ArrowRight:128,ArrowLeft:64},focusMask:4,maxMovementTicks:1,
    keyboardTouchOwnership:{
      semantics:'direction-key-clears-touch-motion',
      source:[
        'ports/th08/web/cpp/sdl/GameHost.cpp | poll | touch.sample arrows',
        'ports/th08/portable/input/TouchController.hpp | TouchController::sample | arrows => clear_motion',
      ],
    },
    touchFocusTransitions:{semantics:'action-queue-then-next-movement',maxMovementTicks:1},
  },'strict th08 normal motion must retain the source-anchored one-tick release policy');
  assert.throws(()=>touchMovementContract('th08','multiplayer'),/Register an explicit multiplayer movement adapter/,
    'the single-player replay pipeline cannot be inherited by multiplayer');
  assert.equal(touchMovementContract('th06','normal').keyboardReleasePolicy,undefined,
    'games without a demonstrated queued-input observer keep their own release contract');
  assert.deepEqual(touchMovementContract('th10','normal').inputCommand,{
    coordinate:'float32-normalized-pointer',pixelsPerUnit:{x:640,y:480},
    source:'ports/th10/web/cpp/sdl/GameHost.cpp | sdl_touch(float x,y); portable/input/TouchController.hpp | pointer',
  });
  assert.equal(Object.hasOwn(touchMovementContract('th10','normal').precision,'pixelsPerUnit'),false,
    'normalized input mapping stays separate from world-position precision');
  assert.throws(()=>touchMovementContract('unregistered','normal'),/Register the native movement arithmetic/);
  assert.throws(()=>touchMovementContract('th08'),/Specify a supported Runtime variant/);
  assert.throws(()=>touchMovementContract('th20','multiplayer'),/has no multiplayer Runtime/);
});

test('th10 adapter command policy follows its float32 pointer protocol independently of trace feedback',()=>{
  const policy=touchMovementContract('th10','normal');
  const mapped=mapFloat32TouchDragTarget({x:12.25,y:120.5},{x:.145,y:-.145},{
    inputSpace:{width:640,height:480},pixelsPerUnit:policy.inputCommand.pixelsPerUnit,
    sensitivity:2,focusFactor:1,
  });
  assert.deepEqual(mapped,{x:12.539993286132812,y:120.21000671386719});
});
