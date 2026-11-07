import {loadPracticeResources,installPractice} from './practice-assets.js';
import {createBrowserSoloRecorder} from './replay/external-recorder.js';
import {restoreExternalReplayDisk} from './replay/external-codec.js';
import {fingerprint} from './replay/tape.js';
import {SoloMusic} from './solo-music.js';
import {DOSBoxPC98} from './dosbox-adapter.js';
import {Fat12} from './solo-fat12.js';
import {extractOriginalPmd} from './pmd-archives.mjs';
import {PmdPlayer} from './pmd-player.mjs';
import {OriginalSfx} from './original-sfx.mjs';
import {assetBytes} from './preload-cache.js';

// Recreate the deterministic game and music bridge patches from installed resources.
async function replayBase(data,resources,practice,audioMode,saves={}){
  await installPractice(data,resources,practice);
  const fat=new Fat12(data);for(const [name,bytes] of Object.entries(saves))fat.put('HUMAROKU/'+name,bytes);
  if(audioMode==='independent'){
    const bridge=Object.create(SoloMusic.prototype);bridge.sfx=true;bridge.player={pmd:{},load:async()=>{}};
    await bridge.install(data,'HUMAROKU');
  }
  return data;
}

// An attempt owns its canvas, DOS instance, clocks, recording and music.
// A parked attempt retains the original JSPI continuation, never a RAM snapshot.
export function createSoloAttempt({canvas,practice,settings,replay=null,standby=false,effects=null,deferReplay=false,sources,applyReplayInput,callbacks={}}){
  const started=performance.now(),timings={};
  const mark=name=>{timings[name]=Math.round(performance.now()-started);};
  const emit=(name,...args)=>{if(attempt.foreground&&!attempt.cancelled)callbacks[name]?.(...args);};
  const attempt={canvas,practice,settings,standby,sfx:effects,timings,
    takeEffects(){const value=this.sfx;this.sfx=null;value?.stop();return value;},core:null,recorder:null,music:null,patch:null,blob:null,ready:false,foreground:false,cancelled:false,
    stop(){if(this.cancelled)return;this.cancelled=true;this.foreground=false;clearTimeout(this.timeout);this.recorder?.dispose();this.sfx?.dispose();this.music?.dispose();return this.core?.dispose();}};
  const fail=error=>{attempt.error=error;if(attempt.foreground)emit('onError',error);else attempt.stop();};
  attempt.task=(async()=>{
    try{
      const {language,audioMode,runtime,recordEnabled,volume}=settings;
      const resources=await loadPracticeResources(language);attempt.patch=resources.spec.patch;mark('resources');
      if(attempt.cancelled)throw Error('练习已取消');
      let data,baseDisk;
      const response=await fetch('solo-patch.json');if(!response.ok)throw Error('单人配置加载失败');
      const original=(await response.json()).disks[language];
      if(replay?.disk)data=replay.disk.slice();
      else{
        data=await assetBytes(original.file,{hash:original.sha256,size:original.size,gzip:true});
        baseDisk=data.slice();if(replay){if(['installed-practice/1','installed-practice/2'].includes(replay.header.diskBaseKind))await replayBase(data,resources,replay.header.practice,replay.header.audioMode,replay.savedFiles);data=await restoreExternalReplayDisk(replay,data);}
      }
      if(attempt.cancelled)throw Error('练习已取消');
      if(!replay)await installPractice(data,resources,practice);
      mark('disk');
      if(audioMode==='independent'){
        const music=attempt.music=new SoloMusic(text=>emit('onStatus',text));
        music.player.pmd={tracks:extractOriginalPmd('th02',new Fat12(data)),engine:await PmdPlayer.prepare(music.context,import.meta.url)};
        const fat=new Fat12(data),bridge=replay?fat.find('HUMAROKU/WEBMUSIC.COM',{optional:true}):undefined;
        if(!replay||bridge!==undefined&&fat.read(bridge).length===883){attempt.sfx??=await OriginalSfx.create('th02',fat);music.sfx=attempt.sfx;}
        // No background audio, including boot/title commands.
        music.suspend();
        if(replay)await music.attachReplay(data);else await music.install(data,'HUMAROKU');
        music.player.setVolume(volume??.7);
      }
      if(attempt.cancelled)throw Error('练习已取消');
      mark('audio');
      const savedFiles={};if(!replay&&recordEnabled){for(const name of ['HUUHI.DAT', 'HUUMA.CFG']){const bytes=globalThis.__eaglerPC98Files?.memory.get(name);if(bytes)savedFiles[name]=bytes;}baseDisk=await replayBase(baseDisk,resources,practice,audioMode,savedFiles);}
      let externalReplay=null;
      if(replay){
        const identity=await fingerprint(sources);
        if(identity!==replay.header.identity||replay.header.runtime!==runtime||replay.header.boundaryMode!=='sleep')throw Error('Replay 与当前游戏或模拟器版本不一致');
        externalReplay={mode:'playback',boundaryMode:'sleep',clocks:replay.clocks,events:replay.events,endBoundary:replay.header.endBoundary,endReason:replay.header.result.endReason,endPhase:replay.header.result.endPhase,timeline:replay.header.result.timeline,memoryScope:replay.header.result.memoryScope,
          onEnd:value=>{const matches=value.memoryHash===replay.header.result.memoryHash&&value.memorySha256===replay.header.result.memorySha256;emit('onPlaybackEnd',matches);},onError:fail};
      }else if(recordEnabled){
        try{
          const identity=await fingerprint(sources);
          if(attempt.cancelled)throw Error('练习已取消');
          attempt.recorder=createBrowserSoloRecorder({game:'02',language,runtime,identity,patch:attempt.patch,boundaryMode:'sleep',disk:data.slice(),baseDisk,practice,audioMode,savedFiles,memoryScope:settings.memoryScope,reuse:!!settings.memoryScope,diskBaseKind:'installed-practice/2',
            deferEncoding:deferReplay,onStatus:text=>{attempt.recordStatus=text;emit('onRecordingStatus',text);},onFailure:error=>emit('onFailure',error),
            onFile:(blob,result)=>{attempt.blob=blob;emit('onFile',blob,result,practice);},onResult:result=>emit('onResult',result,practice)});
          externalReplay=attempt.recorder.config;
        }catch(error){if(attempt.cancelled)throw error;attempt.recordingError=error.message;}
      }
      if(attempt.cancelled)throw Error('练习已取消');
      mark('recording');
      const config={canvas,exitOnGameQuit:!!settings.exitOnGameQuit,externalReplay,practice,practiceStandby:standby,practicePatch:attempt.patch,wasmBinary:resources.wasmBinary,applyReplayInput,independentEffects:!!attempt.sfx,
        onError:fail,onExit:()=>{if(attempt.cancelled)return;if(attempt.foreground){attempt.recorder?.finish();emit('onExit');}else{attempt.error=Error('预备练习意外退出');attempt.stop();}}};
      attempt.core=await DOSBoxPC98.create(config);
      if(attempt.cancelled){attempt.core.dispose();throw Error('练习已取消');}
      if(attempt.error)throw attempt.error;
      mark('core');
      attempt.recorder?.attach(attempt.core);
      attempt.core.module.observeReplayBoundary=()=>attempt.music?.poll(attempt.core.module.HEAPU8,performance.now());
      await attempt.core.addDiskImage('th02-solo.hdi',data);
      await attempt.core.setHdd(0,'th02-solo.hdi');
      if(attempt.cancelled)throw Error('练习已取消');
      mark('boot');
      attempt.core.run();

      if(standby){
        attempt.timeout=setTimeout(()=>{attempt.error=Error('预备练习超时');attempt.stop();},120000);
        await attempt.core.prepared;
      }
      clearTimeout(attempt.timeout);
      if(attempt.cancelled)throw attempt.error||Error('练习已取消');
      attempt.ready=true;return attempt;
    }catch(error){attempt.stop();throw error;}
  })();
  return attempt;
}
