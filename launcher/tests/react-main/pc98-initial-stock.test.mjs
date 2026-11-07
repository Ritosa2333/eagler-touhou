import {test,beforeAll} from 'vitest';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
import {fileURLToPath} from 'node:url';
let api;
beforeAll(async()=>{
  const result=await build({stdin:{resolveDir:fileURLToPath(new URL('../../',import.meta.url)),loader:'ts',contents:`
    export {buildMultiplayerRuntimeOptions} from './src/launcher/multiplayer-runtime-options.mts';
    export {multiplayerConfigForProduct} from '../common/contracts/product-catalog.mjs';
  `},bundle:true,write:false,platform:'node',format:'esm'});
  api=await import('data:text/javascript;base64,'+Buffer.from(result.outputFiles[0].contents).toString('base64'));
});
test('TH02 and TH04 initial stock reaches each runtime',()=>{
  for(const game of ['th02mp','th04mp']){
    const config=api.multiplayerConfigForProduct(game);
    const input={url:`wss://relay.invalid/?room=${game}-1234&run=1`,player:0,playerCount:2,seed:1234,difficulty:1,adonisMode:0,predictionLimit:6,spectator:false,spectatorId:'',spectatorCount:0,iceServers:[],loadouts:config.loadouts};
    const defaults=api.buildMultiplayerRuntimeOptions(input,config);assert.equal(defaults.netplayInitialSpares,3);assert.equal(defaults.netplayInitialBombs,2);
    for(let lives=0;lives<=5;lives++)for(let bombs=0;bombs<=2;bombs++){
      const result=api.buildMultiplayerRuntimeOptions({...input,initialSpares:lives,initialBombs:bombs},config);
      assert.equal(result.netplayInitialSpares,lives);assert.equal(result.netplayInitialBombs,bombs);
    }
    assert.throws(()=>api.buildMultiplayerRuntimeOptions({...input,initialSpares:6},config));
    assert.throws(()=>api.buildMultiplayerRuntimeOptions({...input,initialBombs:3},config));
  }
});
