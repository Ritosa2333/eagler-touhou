import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';
import {th08OggTitleOutcome,th08OggAcceptanceOutcome,th08MidiTitleOutcome,correlateTh08MidiSends,
  th08OggFaultOutcome,th08MusicFaultRecoveryOutcome,musicScenarioErrors,th08BackgroundContinuityOutcome,th08OggDecodeVariantOutcome,
  th08MusicRoomOutcome,th08OggPauseOutcome,th08OggRestartOutcome,th08MidiPauseOutcome,th08MidiRestartOutcome} from '../../../tools/ports/music-browser-result.mjs';
import {assessLauncherMusicScene,assessLauncherMusicFault,assertLauncherMusicModeTransition} from '../../../tools/ports/launcher-music-journey.mjs';
import {assertNativeMusicSuspension,assertMidiOutputResumeBoundary} from '../../../tools/ports/music-native-lifecycle.mjs';
import {assertMusicGameplayRestart} from '../../../tools/ports/music-gameplay-restart.mjs';
import {assessMusicCueTransition} from '../../../tools/ports/music-cue-transition.mjs';

export const COMPLETE_MUSIC_IDS=Object.freeze(['AUDIO-01','AUDIO-03','AUDIO-04','AUDIO-05','MIDI-01','MIDI-02','MIDI-03','MIDI-04']);
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
function file(receipt){
  assert.ok(receipt?.path&&Number.isSafeInteger(receipt.bytes)&&receipt.bytes>0,'Music input has no file identity');
  const bytes=readFileSync(receipt.path);assert.equal(bytes.length,receipt.bytes);assert.equal(digest(bytes),receipt.sha256);return bytes;
}
function mode(target,name){const rows=target.modes.filter(row=>row.mode===name);assert.equal(rows.length,1,`Expected one ${name} music scene`);return rows[0];}
function outcome(result,label){assert.equal(result.passed,true,`${label}: ${JSON.stringify(result)}`);return result;}

/** Recompute from native reads, transport bytes, captured output windows and real Launcher UI. */
export function assertMusicRequirement({driver,target,id,product,adapter=resolveAdaptationAdapter(product.game,product.variant)}){
  assert.ok(COMPLETE_MUSIC_IDS.includes(id));assert.equal(driver.schema,'eagler-touhou/music-browser/1');
  assert.equal(target.game,product.game);assert.equal(target.variant,product.variant);assert.equal(product.variant,'normal');
  const contract=adapter?.music;assert.ok(contract,'Register the title music catalog and native source meanings');
  assert.equal(target.status,'passed');assert.equal(target.passed,true);
  assert.deepEqual(target.identityPostflight,{runtime:target.packageIdentity,data:target.dataIdentity});
  assert.match(target.packageIdentity.packageSha256,/^[a-f0-9]{64}$/);
  assert.deepEqual(musicScenarioErrors(target,{fixtureFailures:target.httpFailures}),[]);
  const mapping=driver.musicMapEvidence[product.game];assert.deepEqual(mapping.data,target.dataIdentity);
  const catalog=contract.parseCatalog(file(mapping.entries.comments),file(mapping.entries.format));
  const inputs=driver.musicInputs[product.game].files;
  const inputFor=name=>{const rows=inputs.filter(row=>row.name===name);assert.equal(rows.length,1);file(rows[0]);return rows[0];};
  for(const track of catalog){const input=inputFor(track.file),recorded=mapping.tracks.find(row=>row.index===track.index);
    assert.deepEqual(recorded,{...track,input});}
  const ogg=target.modes.find(scene=>scene.mode==='ogg'),full=target.modes.find(scene=>scene.mode==='ogg-full'),midi=target.modes.find(scene=>scene.mode==='midi');
  assert.equal(new Set(target.modes.map(scene=>scene.mode)).size,target.modes.length,'Music modes must have unique identities');
  for(const scene of target.modes)for(const response of scene.fixtureResponses?.responses??[]){
    if(response.status!==200)continue;
    assert.deepEqual(response.actual,response.expected,'Delivered Runtime resource bytes differ from their receipt');
    const pathname=new URL(response.url).pathname;
    if(pathname.startsWith(`/music/${product.game}/`)){
      const name=pathname.slice(pathname.lastIndexOf('/')+1),original=inputFor(name);
      const expected=scene.mode==='corrupt-ogg'&&name===contract.titleFile?
        {bytes:scene.corruptOgg.injectedBytes,sha256:scene.corruptOgg.injectedSha256}:{bytes:original.bytes,sha256:original.sha256};
      assert.deepEqual(response.actual,expected,'Delivered music differs from the independently identified source');
    }
  }
  const oggOutput=scene=>{
    assert.ok(scene,'The selected OGG scene was not executed');
    const title=outcome(th08OggTitleOutcome({...scene.title,expectedFile:contract.titleFile}),'OGG title');
    assert.deepEqual(scene.title.input,inputFor(contract.titleFile));assert.deepEqual(scene.start.audio.input,inputFor(contract.stageFile));
    assert.equal(scene.start.audio.trackFile,contract.stageFile);
    outcome(th08OggAcceptanceOutcome({title,stage:scene.start.audio,expectedStageSong:contract.stageSong}),'OGG stage');
    for(const point of [scene.title,scene.start.audio]){
      assert.equal(point.decoder.enabled,true);assert.equal(point.decoder.decodeMode,scene.mode==='ogg-full'?1:0);
      const track=catalog.find(row=>row.file===point.trackFile);assert.ok(track);
      for(const key of ['totalFrames','introFrames','sampleRate'])assert.equal(point.native[key],track[key]);
    }
  };
  const midiOutput=()=>{
    assert.ok(midi,'The selected MIDI scene was not executed');
    for(const [scene,path] of [[midi.title,contract.titleMidi],[midi.start.audio,contract.stageMidi]]){
      const correlation=correlateTh08MidiSends(scene.nativeEvents,scene.synthSends);
      assert.deepEqual(scene.sendCorrelation,correlation);assert.deepEqual(scene.events,correlation.events);
      outcome(th08MidiTitleOutcome({...scene,expectedPath:path,events:correlation.events,sendCorrelation:correlation}),'MIDI source/consumer/PCM');
    }
    assert.equal(midi.start.audio.native.lastMusicSong,contract.stageSong);
    assert.ok(midi.start.audio.native.openCount>midi.title.native.openCount,'The stage SMF was not opened');
    assert.ok(midi.start.audio.native.noteOnCount>midi.title.native.noteOnCount,'No new stage notes reached the native sequencer');
    assert.deepEqual(midi.start.audio.nativeEvents.slice(0,midi.title.nativeEvents.length),midi.title.nativeEvents);
    assert.ok(midi.start.audio.nativeEvents.slice(midi.title.nativeEvents.length).some(event=>(event.bytes[0]&0xf0)===0x90&&event.bytes[2]>0),'Only earlier title MIDI events were observed');
  };
  const launcher=()=>{
    const ui=target.launcherMusic;assert.equal(ui.schema,'eagler-touhou/launcher-music-journey/1');
    assert.equal(ui.game,product.game);assert.equal(ui.variant,product.variant);assert.equal(ui.status,'observed');assert.deepEqual(ui.cleanupErrors,[]);
    assert.deepEqual(ui.packagePostflight,target.packageIdentity);
    assert.deepEqual(ui.scenes.map(scene=>scene.scenario),['none','midi','ogg-stream','ogg-full','missing-object-fallback']);
    for(const scene of ui.scenes){
      outcome(assessLauncherMusicScene(scene),'Launcher music UI');
      assert.equal(scene.installation.bytes,target.dataIdentity.bytes);assert.equal(scene.installation.sha256,target.dataIdentity.sha256);
      assert.deepEqual(scene.choices.map(choice=>choice.value),['ogg-stream','ogg-full','midi','none']);
      assert.ok(scene.choices.every(choice=>choice.disabled===false),'A supported mode is unavailable in the production menu');
    }
    return ui;
  };
  if(id==='AUDIO-01'){oggOutput(ogg);oggOutput(full);}
  const nativeRestart=(scene,mode)=>{
    assertMusicGameplayRestart(scene.start.gameplayRestart,{...contract.gameplayRestart,mode,expectedSource:mode==='ogg'?contract.stageFile:contract.stageMidi});
    const output=scene.start.gameplayRestart.output;
    if(mode==='ogg'){
      outcome(th08OggTitleOutcome({...output,expectedFile:contract.stageFile}),'Native restart OGG source/output');
      assert.deepEqual(output.input,inputFor(contract.stageFile));
      assert.equal(output.decoder.decodeMode,scene.mode==='ogg-full'?1:0);
    }else{
      const correlation=correlateTh08MidiSends(output.nativeEvents,output.synthSends);
      assert.deepEqual(output.sendCorrelation,correlation);assert.deepEqual(output.events,correlation.events);
      outcome(th08MidiTitleOutcome({...output,expectedPath:contract.stageMidi,events:correlation.events,sendCorrelation:correlation}),'Native restart MIDI source/output');
    }
  };
  if(id==='AUDIO-03'){
    outcome(th08MusicRoomOutcome({catalog:catalog.map(track=>({...track,input:inputFor(track.file)})),journey:ogg.musicRoom}),'Music Room sources, natural loop, fade, stop and Reset');
    const parsed=contract.parseCues(file(mapping.entries.bossMessage),file(mapping.entries.stage),0),recipe=contract.buildCueRecipe(parsed);
    assert.deepEqual(mapping.bossCue.parsed,parsed);assert.deepEqual(mapping.bossCue.recipe,recipe);
    for(const scene of [ogg,full]){
      oggOutput(scene);nativeRestart(scene,'ogg');
      outcome(th08OggPauseOutcome(scene.start.oggPause.observationInput),'Native game music pause/resume');
      outcome(th08OggRestartOutcome(scene.start.restart.observationInput),'Runtime music close and reopen');
      outcome(th08OggDecodeVariantOutcome({mode:scene.mode,record:scene,requiredBoundaries:['title','stage','pause','restart']}),'Decoder lifecycle');
      const boss=scene.start.bossCue;outcome(assessMusicCueTransition(boss.transition,recipe),'Data-bound Boss cue');
      outcome(th08OggTitleOutcome({...boss.audio,expectedFile:recipe.afterFile}),'Boss source/output');
      assert.deepEqual(boss.audio.input,inputFor(recipe.afterFile));assert.equal(boss.audio.decoder.decodeMode,scene.mode==='ogg-full'?1:0);
    }
  }
  if(id==='AUDIO-04'){
    assert.ok(target.lifecycleBrowser.processId>0);assert.ok(!/headless/i.test(target.lifecycleBrowser.browserVersion.userAgent));
    assert.equal(target.lifecycleBrowser.scope,'same-window-tab');assert.equal(target.lifecycleBrowser.osOcclusion,'disabled-for-tab-isolation');
    assert.deepEqual(target.lifecycleCleanup.errors,[]);assert.deepEqual(target.lifecycleCleanup.gateway.errors,[]);
    for(const scene of [ogg,full]){
      oggOutput(scene);assertNativeMusicSuspension(scene.start.background.nativeLifecycle);
      outcome(th08BackgroundContinuityOutcome(scene.start.background.observationInput),'Real hidden/foreground source and output continuity');
      outcome(th08OggDecodeVariantOutcome({mode:scene.mode,record:scene,requiredBoundaries:['title','stage','background']}),'Decoder policy across the actual background transition');
    }
  }
  if(id==='MIDI-02')midiOutput();
  if(id==='MIDI-03'){
    midiOutput();nativeRestart(midi,'midi');
    outcome(th08MidiPauseOutcome(midi.start.midiPause.observationInput),'MIDI game pause/resume');
    assertNativeMusicSuspension(midi.start.background.nativeLifecycle);
    assertMidiOutputResumeBoundary(midi.start.background.nativeLifecycle.audioResumption,midi.start.background.observationInput.resumedOutput);
    outcome(th08BackgroundContinuityOutcome(midi.start.background.observationInput),'MIDI actual tab suspension');
    outcome(th08MidiRestartOutcome(midi.start.restart.observationInput),'MIDI voice panic, effect reset, quiet outputs and reopened title');
    assert.deepEqual(target.lifecycleCleanup.errors,[]);assert.deepEqual(target.lifecycleCleanup.gateway.errors,[]);
  }
  if(id==='MIDI-01'){
    assert.equal(PRODUCT_GAMES[product.game].musicCapabilities.midi,true);launcher();midiOutput();
    assert.equal(midi.midiBridge?.installed??Boolean(midi.midiBridge),true);
  }
  if(id==='MIDI-04'){
    oggOutput(ogg);oggOutput(full);midiOutput();
    const ui=launcher();assertLauncherMusicModeTransition(ui.scenes.find(scene=>scene.scenario==='midi'),ui.scenes.find(scene=>scene.scenario==='ogg-stream'));
    assert.deepEqual(midi.runtimeResources,[]);
    assert.ok(midi.fixtureResponses.responses.length>0);
    assert.ok(midi.fixtureResponses.responses.every(row=>!new URL(row.url).pathname.endsWith('.ogg')),'MIDI fetched an OGG dependency');
    for(const scene of [ogg,full])assert.ok(scene.fixtureResponses.responses.some(row=>new URL(row.url).pathname.endsWith('/'+contract.titleFile)&&row.status===200));
  }
  if(id==='AUDIO-05'){
    const ui=launcher();outcome(assessLauncherMusicFault(ui.scenes.at(-1)),'Production missing-object fallback');
    for(const name of ['corrupt-ogg','missing-ogg']){
      const scene=mode(target,name),missing=name==='missing-ogg';
      const fault=th08OggFaultOutcome({fault:missing?'missing':'corrupt',expectedFile:contract.titleFile,
        ...(missing?scene.missingOgg:{native:scene.title.native,pcm:scene.title.pcm,
          diagnostics:scene.events.filter(event=>event.event==='notice').map(event=>event.message),
          injectedBytesSha256:scene.corruptOgg.injectedSha256,originalBytesSha256:scene.corruptOgg.originalSha256})});
      outcome(fault,'Rejected music resource');outcome(th08MusicFaultRecoveryOutcome({fault,recovery:scene.recovery}),'Fresh MIDI recovery');
      const original=inputFor(contract.titleFile);assert.equal((missing?scene.missingOgg:scene.corruptOgg).originalSha256,original.sha256);
      const recovery=scene.recovery.scene,correlation=correlateTh08MidiSends(recovery.nativeEvents,recovery.synthSends);
      assert.deepEqual(recovery.events,correlation.events);assert.deepEqual(recovery.sendCorrelation,correlation);
    }
  }
  return{data:target.dataIdentity,packageSha256:target.packageIdentity.packageSha256,
    title:contract.titleFile,stage:contract.stageFile,titleMidi:contract.titleMidi,stageMidi:contract.stageMidi};
}

/** Producer and central reader use the same assertion, never a stored passed flag. */
export function assessMusicEvidence({driver,entry,product}={}){
  if(driver?.schema!=='eagler-touhou/music-browser/1'||!COMPLETE_MUSIC_IDS.includes(entry?.id))return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-music-source-output-and-launcher',checks:[entry.id]};
  try{
    const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
    const target=targets[0];assert.ok(target.checks.includes(entry));assert.equal(target.checks.filter(row=>row.id===entry.id).length,1);
    assert.equal(entry.complete,true);assert.equal(entry.status,'passed');
    assert.deepEqual(entry.observation,assertMusicRequirement({driver,target,id:entry.id,product}));result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
