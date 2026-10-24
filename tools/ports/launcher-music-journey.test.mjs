import test from 'node:test';
import assert from 'node:assert/strict';
import vm from 'node:vm';
import {assessLauncherMusicScene,assessLauncherMusicFault,installLauncherMusicObserver,matchesMusicLaunchConsent} from './launcher-music-journey.mjs';
import {runtimePcm} from './fixtures/music-pcm-window.mjs';
import {TH08_OGG_PCM_ROLE,TH08_MIDI_PCM_ROLE} from './music-browser-helpers.mjs';

test('music launch confirms the actual selected-mode warning and refuses unrelated decisions',()=>{
  const prompt={message:'The game will continue without music.\nIf you want normal music playback, choose OGG under Music.',confirm:'Start anyway'};
  assert.equal(matchesMusicLaunchConsent('none',prompt),true);
  assert.equal(matchesMusicLaunchConsent('midi',prompt),false);
  assert.equal(matchesMusicLaunchConsent('none',{...prompt,message:'Restart now to install an update'}),false);
  assert.equal(matchesMusicLaunchConsent('none',{...prompt,confirm:'Delete'}),false);
});

function quietScene(){return{mode:'none',runtime:{epoch:4},moduleMode:'none',preference:{music:'none',musicPreferenceExplicit:true},reloadedPreference:'none',
  launchConsent:{message:'The game will continue without music. If you want normal music playback, choose OGG under Music.',confirm:'Start anyway',confirmed:true},
  host:{configures:[{epoch:4,music:'none',at:100}],events:[{epoch:4,event:'first-frame',receivedAt:110}],nativeEpochs:[4],errors:[]},
  audio:{decoder:{enabled:false},midiObservation:{active:false},midiHost:{nativeEvents:[]}},
  pcm:runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:8,nonZeroPerWindow:0}),browserErrors:[],responseErrors:[],protocolErrors:[]};}

test('Launcher UI assessment distinguishes missing observations from observed wrong behavior',()=>{
  assert.equal(assessLauncherMusicScene(quietScene()).status,'passed');
  for(const mutate of [s=>s.moduleMode='midi',s=>s.reloadedPreference='midi',s=>s.audio.decoder.enabled=true,
    s=>s.pcm.outputObserved=true,s=>s.pcm.nonZeroSamples=2,s=>s.protocolErrors.push({error:'actual Runtime failure'}),
    s=>s.host.nativeEpochs=[3],s=>s.host.events[0].receivedAt=90,s=>s.launchConsent.confirmed=false,
    s=>s.pcm.windows[4].target.state='suspended',s=>s.pcm.windows[4].taps[0].tapId=99,s=>delete s.pcm.windows]){
    const scene=quietScene();mutate(scene);assert.equal(assessLauncherMusicScene(scene).status,'failed');
  }
  for(const mutate of [s=>delete s.moduleMode,s=>delete s.audio,s=>delete s.launchConsent,s=>s.pcm.status='incomplete']){
    const scene=quietScene();mutate(scene);assert.equal(assessLauncherMusicScene(scene).status,'incomplete');
  }
});

test('MIDI send observations from another Runtime epoch cannot satisfy the UI scene',()=>{
  const scene=fallbackScene();assert.equal(assessLauncherMusicScene(scene).passed,true);
  scene.audio.midiHost.nativeEvents[0].epoch=3;
  assert.equal(assessLauncherMusicScene(scene).status,'failed');
});

function fallbackScene(){
  const scene=quietScene(),hash='a'.repeat(64),data={bytes:123,sha256:'b'.repeat(64)},fileId='ogg:th08_01.ogg',target='/bgm-ogg/th08_01.ogg';
  const generation={id:'real-installed-generation',descriptor:{files:{[fileId]:{target,bytes:64,sha256:hash}}},files:{[fileId]:{objectId:'real-optional-object'}}};
  Object.assign(scene,{scenario:'missing-object-fallback',mode:'ogg-stream',moduleMode:'midi',midiOutcome:{passed:true},
    preference:{music:'ogg-stream',musicPreferenceExplicit:true},postLaunchPreference:{music:'ogg-stream',musicPreferenceExplicit:true},reloadedPreference:'ogg-stream',
    fault:{at:90,fileId,target,objectId:'real-optional-object',firstIds:['ogg:th08_00.ogg',fileId],
      beforeIdentity:{bytes:64,sha256:hash},expectedIdentity:{bytes:64,sha256:hash},afterObjectAbsent:true,
      beforeGeneration:generation,afterDeletionGeneration:structuredClone(generation),afterLaunchGeneration:structuredClone(generation),
      dataIdentityBefore:data,dataIdentityAfter:{...data},dataIdentityAfterLaunch:{...data}}});
  scene.host.configures[0]={epoch:4,music:'midi',decodeMode:'stream',at:100};
  scene.host.toasts=[{at:105,text:'本地 OGG 准备失败，本次改用 MIDI：/bgm-ogg/th08_01.ogg'}];
  scene.pcm=runtimePcm(TH08_MIDI_PCM_ROLE,{windowCount:8});
  scene.audio.midiObservation={path:'bgm/th08_01.mid',active:true,invalid:false,fileIndex:0,trackCount:1,messageCount:2,sysexCount:0,noteOnCount:1};
  scene.audio.midiHost={contextState:'running',nativeEvents:[{epoch:4,bytes:[0x90,60,100]},{epoch:4,bytes:[0x80,60,0]}],sentMessages:[[0x90,60,100],[0x80,60,0]]};
  return scene;
}

test('isolated missing-object fallback requires the actual toast, unchanged references and verified MIDI',()=>{
  assert.equal(assessLauncherMusicScene(fallbackScene()).status,'passed');
  assert.equal(assessLauncherMusicFault(fallbackScene()).status,'passed');
  for(const mutate of [
    s=>s.host.toasts=[],s=>s.host.toasts[0].text='OGG unavailable, using MIDI',
    s=>s.host.toasts[0].at=80,s=>s.fault.afterObjectAbsent=false,
    s=>s.fault.afterDeletionGeneration.files['ogg:th08_01.ogg'].objectId='changed-ref',
    s=>s.fault.afterLaunchGeneration.descriptor.files['ogg:th08_01.ogg'].sha256='c'.repeat(64),
    s=>s.fault.beforeIdentity.sha256='c'.repeat(64),s=>s.fault.dataIdentityAfterLaunch.sha256='c'.repeat(64),
    s=>s.midiOutcome.passed=false,s=>s.moduleMode='ogg',s=>s.postLaunchPreference.music='midi',
    s=>s.audio.midiHost.sentMessages=[],s=>s.audio.midiObservation.messageCount=3,
  ]){
    const scene=fallbackScene();mutate(scene);assert.equal(assessLauncherMusicScene(scene).status,'failed');
  }
  const missing=fallbackScene();delete missing.fault;assert.equal(assessLauncherMusicScene(missing).status,'incomplete');
});

test('passive vendor observer does not construct a synth or bridge and preserves actual sends',()=>{
  const listeners={},nativeListeners={},target={document:{},addEventListener:(name,fn,capture)=>{nativeListeners[name]={fn,capture};}};
  const iframe={contentWindow:target,src:'https://fixture.test/runtime/th08/index.html?runtimeEpoch=4'};
  const window={};window.top=window;
  const context={window,document:{querySelector:()=>iframe},location:{origin:'https://fixture.test',href:'https://fixture.test/'},
    URL,Date,performance:{now:()=>10,timeOrigin:0},addEventListener:(name,fn)=>{listeners[name]=fn;},
    MutationObserver:class{observe(){}}};
  vm.runInNewContext(`(${installLauncherMusicObserver.toString()})({game:'th08'})`,context);
  let constructed=0;const sent=[];
  class RealSynth{constructor(){constructed++;this.out={};}send(message){sent.push(message);return 123;}}
  window.WebAudioTinySynth=RealSynth;
  assert.equal(constructed,0);assert.equal(window.__eaglerMidiBridge,undefined);
  const synth=new window.WebAudioTinySynth();assert.equal(constructed,1);assert.ok(synth instanceof RealSynth);
  const setup=[0xb0,123,0];assert.equal(synth.send(setup),123);
  listeners.message({origin:'https://fixture.test',source:target,data:{protocol:'eagler-touhou/1',game:'th08',epoch:3,event:'ready'}});
  assert.equal(nativeListeners['touhou-midi'],undefined);
  listeners.message({origin:'https://fixture.test',source:target,data:{protocol:'eagler-touhou/1',game:'th08',epoch:4,event:'ready'}});
  assert.equal(nativeListeners['touhou-midi'].capture,true);
  const note=[0x90,60,100];nativeListeners['touhou-midi'].fn({detail:{bytes:note,time:1}});assert.equal(synth.send(note),123);
  assert.equal(sent[0],setup);assert.equal(sent[1],note);
  assert.deepEqual(Array.from(window.__eaglerTinySynthSends[0]),note);
  assert.equal(window.__launcherMusicReceipt.allSynthSends.length,2);
  assert.equal(window.__eaglerMidiNativeEvents[0].epoch,4);
});
