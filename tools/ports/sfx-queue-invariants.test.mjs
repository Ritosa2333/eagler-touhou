import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {assertConsumerCoverage,assertMutationWaitsForTail,assertOtherAudioCoverage,assertRenderedSamples,
  buildSfxProductCoverage,extractWebSfxBody,makeQueueTailMutation,SFX_AUDIO_OWNERS} from './sfx-queue-invariants.mjs';

const root=resolve(import.meta.dirname,'../..');
const startSamples=[[0.375,-0.625],[0.625,-0.375]];
const queueTailSample=[0.125,-0.125];
const delayedSamples={queuedFrames:3072,renderedFrames:3074,firstSamples:[[0.125,-0.125],[0.125,-0.125]],
  queueTailSamples:[[0.25,-0.5],[0.5,-0.25]],queueTailSample:[0.125,-0.125]};

test('PCM assertions require the new effect to overlap both production queue depths',()=>{
  for(const queuedFrames of [3072,6144])assert.equal(assertRenderedSamples({queuedFrames,renderedFrames:queuedFrames+2,
    firstSamples:startSamples,queueTailSample}),true);
});

test('queue-tail delayed-to-play mutation renders after old PCM and fails the production invariant',()=>{
  assert.equal(assertMutationWaitsForTail(delayedSamples),true);
  assert.throws(()=>assertRenderedSamples(delayedSamples),/first rendered samples/);
});

test('the negative control mutates only the production direct-start site',async()=>{
  const source=await readFile(resolve(root,'common/include/eagler/audio/WebSfx.hpp'),'utf8');
  const body=extractWebSfxBody(source),mutated=makeQueueTailMutation(body);
  assert.ok(body.includes('node.start(0, v.offset);'));
  assert.ok(mutated.includes("node.start(Module['eaglerSfxQueuedFrames'] / ctx.sampleRate, v.offset);"));
  assert.equal(mutated.split("node.start(Module['eaglerSfxQueuedFrames'] / ctx.sampleRate, v.offset);").length-1,1);
});

test('the existing browser-shell task keeps its live child-frame AudioContext contract in the run path',async()=>{
  const source=await readFile(resolve(root,'tools/ports/sfx-queue-invariants.mjs'),'utf8');
  assert.match(source,/createFixtureServer\(/,'the live Worklet probe is served from the existing localhost fixture');
  assert.match(source,/MUSIC_PCM_PROBE_SOURCE/);
  assert.match(source,/DESKTOP_AUDIO_PROBE_ADAPTER_SOURCE/);
  assert.match(source,/const liveWebAudioProbe=await runLiveWebAudioProbe\(browser,evidence\.body\)/,
    'the browser-shell entry executes the live component probe alongside OfflineAudioContext queue rendering');
});

test('product variants and repository owners are derived from their independent inputs',()=>{
  const productGames={alpha:{},beta:{multiplayerRuntime:'./mp.html',multiplayer:{}}};
  const repositories={alpha:'ports/alpha',beta:'ports/beta',betamp:'ports/beta-mp'};
  const owners={alpha:{kind:'web-sfx',source:'audio/Device.cpp'},beta:{kind:'web-sfx',source:'audio/Host.cpp'}};
  const coverage=buildSfxProductCoverage({productGames,repositories,owners});
  assert.deepEqual(coverage.consumers.map(({productId,variant,path})=>({productId,variant,path})),[
    {productId:'alpha',variant:'normal',path:'ports/alpha/audio/Device.cpp'},
    {productId:'beta',variant:'normal',path:'ports/beta/audio/Host.cpp'},
    {productId:'betamp',variant:'multiplayer',path:'ports/beta-mp/audio/Host.cpp'},
  ]);
  assert.throws(()=>buildSfxProductCoverage({productGames:{...productGames,gamma:{}},repositories:{...repositories,gamma:'ports/gamma'},owners}),
    /gamma: missing SFX audio-owner mapping/,'a newly cataloged product must require an explicit owner classification');
  assert.throws(()=>buildSfxProductCoverage({productGames,repositories,owners:{alpha:owners.alpha}}),
    /beta: missing SFX audio-owner mapping/,'removing an existing owner mapping must fail');
});

test('deleting web_sfx::play from a mapped consumer is caught by static wiring validation',()=>{
  const consumers=[{game:'fixture',productId:'fixture',variant:'normal',path:'ports/fixture/Audio.cpp'}];
  const sources=new Map([['ports/fixture/Audio.cpp','#include <eagler/audio/WebSfx.hpp>\nvoid play(){ /* disconnected */ }']]);
  assert.throws(()=>assertConsumerCoverage(sources,consumers),/fixture: SFX play does not reach the shared output service/);
});

test('catalog-derived coverage checks every applicable product and records non-WebSfx audio explicitly',async()=>{
  const workspace=JSON.parse(await readFile(resolve(root,'launcher/config/workspace.json'),'utf8'));
  const coverage=buildSfxProductCoverage({productGames:PRODUCT_GAMES,repositories:workspace.repositories,owners:SFX_AUDIO_OWNERS});
  const sources=new Map();
  for(const route of [...coverage.consumers,...coverage.otherAudioProducts])
    if(!sources.has(route.path))sources.set(route.path,await readFile(resolve(root,route.path),'utf8'));
  const wired=assertConsumerCoverage(sources,coverage.consumers);
  const otherAudio=assertOtherAudioCoverage(sources,coverage.otherAudioProducts);
  assert.ok(wired.some(product=>product.productId==='th20'));
  assert.ok(!wired.some(product=>product.productId==='th20mp'),'the catalog does not declare a TH20 multiplayer product');
  assert.ok(otherAudio.some(product=>product.productId==='th075'&&product.variant==='normal'));
  assert.ok(otherAudio.some(product=>product.productId==='th075mp'&&product.variant==='multiplayer'));
});
