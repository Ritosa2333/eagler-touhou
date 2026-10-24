import assert from 'node:assert/strict';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';
import {TH10_MOVEMENT_CONTRACT} from './titles/th10.mjs';

// These policies describe native arithmetic, not permissible test error.
// Cap checks retain the same independent speed bound for every policy.
const floating=Object.freeze({unitsPerPixel:1,rounding:'float',scalePlacement:'before-limit'});
const fixed128=Object.freeze({unitsPerPixel:128,rounding:'toward-zero',scalePlacement:'after-limit'});
const policies=Object.freeze({
  th06:{precision:floating,source:'ports/th06/src/Player.cpp | HandlePlayerInputs'},
  th07:{precision:floating,source:'ports/th07/src/Player.cpp | HandlePlayerInputs'},
  th08:{precision:floating,source:'ports/th08/web/cpp/game/PlayerMovement.cpp | update'},
  th09:{precision:floating,source:'ports/th09/web/cpp/game/PlayerMotion.cpp | update'},
  th10:TH10_MOVEMENT_CONTRACT,
  th11:{precision:fixed128,source:'ports/th11/web/cpp/game/PlayerMotion.cpp | update'},
  th15:{precision:fixed128,source:'ports/th15/web/cpp/game/PlayerMotion.cpp | update'},
  th20:{precision:fixed128,source:'ports/th20/src/player_entity/movement.cpp | update_movement'},
});

export function touchMovementContract(game,variant){
  assert.ok(variant==='normal'||variant==='multiplayer',`Specify a supported Runtime variant for ${game}`);
  const registered=resolveAdaptationAdapter(game,variant)?.movement;
  if(registered)return registered;
  assert.ok(Object.hasOwn(policies,game),`Register the native movement arithmetic for ${game}`);
  if(variant==='multiplayer'){
    assert.ok(PRODUCT_GAMES[game]?.multiplayerRuntime,`${game} has no multiplayer Runtime`);
    throw new Error(`Register an explicit multiplayer movement adapter for ${game}; normal movement semantics cannot be reused`);
  }
  const {variants,...base}=policies[game];
  return Object.freeze({...base,inputBoundary:'unknown',...(variants?.[variant]??{})});
}
