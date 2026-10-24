import test from 'node:test';
import assert from 'node:assert/strict';
import {assessLauncherPracticeScene,assessLauncherPracticeJourney} from './launcher-practice-journey.mjs';

function observed({host=true,enabled=true}={}){
  const options=host?{thpracEnabled:enabled,thpracLocale:'ja-JP'}:{},words=Array(128).fill(0);words[0]=1;words[2]=Number(enabled);
  return{game:'th08',hostEnabled:host,enabled,musicMode:'none',selectedLanguage:'ja',packageFeature:true,publishedFeature:host,
    ui:{advancedOpen:true,readyControlVisible:true,visible:host,checked:enabled,disabled:false},reloadedChecked:enabled,preference:{options:{thpracEnabled:enabled}},
    installation:{bytes:100,sha256:'a'.repeat(64)},expectedData:{bytes:100,sha256:'a'.repeat(64)},runtime:{epoch:2,url:'https://fixture/runtime/th08/abc/th08.html?runtimeEpoch=2',generation:'abc'},
    protocol:{configures:[{epoch:2,runtimePackPresent:false,music:'none',options,at:50}],events:[{epoch:2,event:'first-frame',at:100}],errors:[]},
    native:{before:{available:true,open:false},opened:{available:true,open:enabled,options,events:enabled?[{open:true}]:[]},
      closed:{available:true,open:false,events:enabled?[{open:true},{open:false}]:[]}},nativeOwnerRaw:{available:true,words},
    launchConsent:{confirmed:true,message:'The game will continue without music. If you want normal music playback, choose OGG under Music.',confirm:'Start anyway'},
    gameplay:{before:{game:'th08',localSeat:0,phase:{name:'stage'},pause:{active:false},frames:{simulation:20}},after:{game:'th08',localSeat:0,phase:{name:'stage'},pause:{active:false},frames:{simulation:21}}},
    browserErrors:[],responseErrors:[],protocolErrors:[]};
}

test('real Launcher practice assessment accepts visible on/off and explicit Host-off omission with native disabled',()=>{
  for(const input of [{host:true,enabled:true},{host:true,enabled:false},{host:false,enabled:false}]){
    const result=assessLauncherPracticeScene(observed(input));assert.equal(result.passed,true,JSON.stringify(result));assert.equal(result.complete,false);
  }
  const projected=observed({host:false,enabled:false});projected.native.opened.options={thpracEnabled:undefined,thpracLocale:undefined};
  assert.equal(assessLauncherPracticeScene(projected).passed,true);
});

test('practice UI evidence rejects capability, persistence, configure, native owner and menu-path disagreement',()=>{
  for(const mutate of [s=>s.publishedFeature=false,s=>s.packageFeature=false,s=>s.ui.visible=false,s=>s.ui.checked=false,
    s=>s.ui.advancedOpen=false,s=>s.ui.readyControlVisible=false,
    s=>s.reloadedChecked=false,s=>s.preference.options.thpracEnabled=false,s=>s.installation.bytes=99,
    s=>s.protocol.configures[0].options.thpracEnabled=false,s=>s.protocol.configures[0].options.thpracLocale='en-US',
    s=>s.protocol.configures[0].at=150,s=>s.protocol.configures[0].epoch=3,s=>s.nativeOwnerRaw.words[2]=0,
    s=>s.native.opened.open=false,s=>s.native.closed.events=[{open:true}],s=>s.native.closed.available=false,
    s=>s.native.opened.options.thpracEnabled=false,s=>s.launchConsent.confirmed=false,s=>s.browserErrors.push('console error'),
    s=>s.protocolErrors.push({event:'error'}),s=>s.gameplay.before.phase.name='title',s=>s.gameplay.after.frames.simulation=20,
    s=>s.runtime.generation='different',s=>s.runtime.url='https://fixture/runtime/th08/abc/th08.html?runtimeEpoch=3',
    s=>s.selectedLanguage='lang_en',s=>s.protocol.configures[0].runtimePackPresent=true]){
    const scene=observed();mutate(scene);assert.equal(assessLauncherPracticeScene(scene).passed,false,JSON.stringify(scene));
  }
  const hidden=observed({host:false,enabled:false});hidden.protocol.configures[0].options={thpracEnabled:false};assert.equal(assessLauncherPracticeScene(hidden).status,'failed');
});

test('missing raw native and protocol evidence cannot pass from producer status alone',()=>{
  for(const mutate of [s=>delete s.nativeOwnerRaw,s=>delete s.protocol,s=>delete s.installation,s=>delete s.ui,s=>delete s.responseErrors,s=>delete s.native,s=>delete s.gameplay]){
    const scene=observed();scene.status='observed';scene.passed=true;mutate(scene);const result=assessLauncherPracticeScene(scene);assert.equal(result.passed,false);
  }
});

test('Launcher practice matrix cannot substitute duplicate successes or omit postflight identities',()=>{
  const identity={runtime:{files:{wasm:'current'}},data:{bytes:100,sha256:'a'.repeat(64)}};
  const report={game:'th08',identity,postflight:structuredClone(identity),cleanupErrors:[],scenes:[observed(),observed({enabled:false}),observed({host:false,enabled:false})]};
  assert.equal(assessLauncherPracticeJourney(report).passed,true);
  for(const mutate of [r=>r.scenes[2]=structuredClone(r.scenes[0]),r=>r.scenes.pop(),r=>delete r.postflight,r=>r.postflight.data.bytes++,r=>r.scenes[0].game='th10',r=>r.cleanupErrors.push('close failed')]){
    const changed=structuredClone(report);mutate(changed);assert.equal(assessLauncherPracticeJourney(changed).passed,false);
  }
});
