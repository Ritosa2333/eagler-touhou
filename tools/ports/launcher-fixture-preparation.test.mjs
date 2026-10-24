import test from 'node:test';
import assert from 'node:assert/strict';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {createSiteSeed,featureConfig} from './launcher-fixture-preparation.mjs';

test('Launcher feature inputs default to disabled Practice and preserve language defaults',()=>{
  const config=featureConfig();
  for(const [game,product] of Object.entries(PRODUCT_GAMES)){
    if(product.features.thprac)assert.equal(config.games[game].thprac,false);
    else assert.equal(Object.hasOwn(config.games[game],'thprac'),false);
    if(product.features.languages)assert.deepEqual(config.games[game].languages,['ja']);
  }
  assert.deepEqual(featureConfig({practiceFeatures:{}}),config);
});

test('explicit title Practice request reaches regular feature input without changing the site seed',()=>{
  const seed=createSiteSeed('th08',{bytes:4,sha256:'a'.repeat(64)},'sha256-'+ 'b'.repeat(64));
  const before=structuredClone(seed),request={th08:true};
  const config=featureConfig({practiceFeatures:request,languages:{th08:['ja','en']}});
  assert.equal(config.games.th08.thprac,true);assert.equal(config.games.th06.thprac,false);
  assert.deepEqual(config.games.th08.languages,['ja','en']);assert.deepEqual(seed,before);
  assert.deepEqual(request,{th08:true});assert.equal(featureConfig({practiceFeatures:{th08:false}}).games.th08.thprac,false);
});

test('Practice request rejects unknown products, unsupported products and non-booleans',()=>{
  for(const practiceFeatures of [null,[],true,{unknown:true},{th08:1},{th08:'true'},{th08:null},{th08:undefined}])
    assert.throws(()=>featureConfig({practiceFeatures}));
  const unsupported=Object.keys(PRODUCT_GAMES).find(game=>!PRODUCT_GAMES[game].features.thprac);
  assert.throws(()=>featureConfig({practiceFeatures:{[unsupported]:false}}),/does not declare/);
  assert.equal(featureConfig({practiceFeatures:Object.create({th08:true})}).games.th08.thprac,false,'inherited keys cannot publish a feature');
});
