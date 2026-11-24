import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';
import {OriginalMainMenuController} from '../site/runtime/sound.mjs';

const source=fs.readFileSync(new URL('../site/play.mjs',import.meta.url),'utf8');
const begin=source.indexOf("window.addEventListener('keyup',event=>{");
const end=source.indexOf('\nfunction releaseGameInput(){',begin);
assert(begin>=0&&end>begin,'The complete production keyup handler is required');

for(const code of ['ArrowDown','KeyZ','Enter','KeyX','ShiftLeft']){
  test(`released postrun ${code} cannot remain held after Main adoption`,async()=>{
    const released=[],nativeInputs=[],state={worldMode:true,pressed:new Set([code,'ArrowLeft']),
      normalPostrun:{input:{up:value=>released.push(value)}}};
    const suppressedKeys=new Set([code,'ArrowLeft']);let handler;
    const context={state,suppressedKeys,practiceMenu:null,window:{addEventListener(type,callback){assert.equal(type,'keyup');handler=callback;}},
      menu:{pauseOpen:false},movementKeys:new Map(),worldMovementInput(){assert.fail('Postrun key release must not reach World');},
      gameplayXRelease(){assert.fail('Postrun cancel must not reach gameplay freeze');}};
    vm.createContext(context);vm.runInContext(source.slice(begin,end),context);handler({code});
    assert.deepEqual(released,[code]);assert.deepEqual([...state.pressed],['ArrowLeft']);
    assert.deepEqual([...suppressedKeys],['ArrowLeft']);
    const module={UTF8ToString:value=>value,_th128_title_menu_state:()=>JSON.stringify({ready:1,transitioning:0,page:0}),
      _th128_tick:()=>1,_th128_title_main_input:(held,pressed)=>{nativeInputs.push({held,pressed});return 1;}};
    const main=new OriginalMainMenuController({module,prepare:async()=>{},consumeSound:async()=>{},consumeMusic:async()=>{},syncText:async()=>{}});
    state.normalPostrun=null;main.adoptHeld(state.pressed);await main.step();
    assert.deepEqual(nativeInputs,[{held:0x40,pressed:0}]);main.dispose();
  });
}
