import {loadPracticeResources,installPractice} from './practice-assets.js';
import {createBrowserSoloRecorder} from './replay/external-recorder.js';
import {restoreExternalReplayDisk} from './replay/external-codec.js';
import {fingerprint} from './replay/tape.js';
import {SoloMusic} from './solo-music.js';
import {DOSBoxPC98} from './dosbox-adapter.js';
import {sha256} from './solo-fat12.js';

// An attempt owns its canvas, DOS instance, clocks, recording and music.
// A parked attempt retains the original JSPI continuation, never a RAM snapshot.
export function createSoloAttempt({canvas,practice,settings,replay=null,standby=false,sources,applyReplayInput,callbacks={}}){
  const emit=(name,...args)=>{if(attempt.foreground&&!attempt.cancelled)callbacks[name]?.(...args);};
  const attempt={canvas,practice,settings,standby,core:null,recorder:null,music:null,patch:null,blob:null,ready:false,foreground:false,cancelled:false,
    stop(){if(this.cancelled)return;this.cancelled=true;this.foreground=false;clearTimeout(this.timeout);this.recorder?.dispose();this.music?.dispose();this.core?.dispose();}};
  const fail=error=>{attempt.error=error;if(attempt.foreground)emit('onError',error);else attempt.stop();};
  attempt.task=(async()=>{
    try{
      const {language,audioMode,runtime,recordEnabled,volume}=settings;
      const resources=await loadPracticeResources(language);attempt.patch=resources.spec.patch;
      if(attempt.cancelled)throw Error('练习已取消');
      let data,baseDisk;
      const response=await fetch('solo-patch.json');if(!response.ok)throw Error('单人配置加载失败');
      const original=(await response.json()).disks[language];
      if(replay?.disk)data=replay.disk.slice();
      else{
        const disk=await fetch(original.file);if(!disk.ok)throw Error('单人磁盘加载失败');
        data=new Uint8Array(await new Response(disk.body.pipeThrough(new DecompressionStream('gzip'))).arrayBuffer());
        if(data.length!==original.size||await sha256(data)!==original.sha256)throw Error('单人磁盘校验失败');
        baseDisk=data.slice();if(replay)data=await restoreExternalReplayDisk(replay,data);
      }
      if(attempt.cancelled)throw Error('练习已取消');
      if(!replay)await installPractice(data,resources,practice);
      if(audioMode==='independent'){
        const music=attempt.music=new SoloMusic(text=>emit('onStatus',text));
        // No background audio, including boot/title commands.
        music.suspend();
        if(replay)await music.attachReplay(data);else await music.install(data,'HUMAROKU');
        music.player.setVolume(volume??.7);
      }
      if(attempt.cancelled)throw Error('练习已取消');
      let externalReplay=null;
      if(replay){
        const identity=await fingerprint(sources);
        if(identity!==replay.header.identity||replay.header.runtime!==runtime||replay.header.boundaryMode!=='sleep')throw Error('Replay 与当前游戏或模拟器版本不一致');
        externalReplay={mode:'playback',boundaryMode:'sleep',clocks:replay.clocks,events:replay.events,endBoundary:replay.header.endBoundary,endReason:replay.header.result.endReason,endPhase:replay.header.result.endPhase,timeline:replay.header.result.timeline,
          onEnd:value=>{const matches=value.memoryHash===replay.header.result.memoryHash&&value.memorySha256===replay.header.result.memorySha256;emit('onPlaybackEnd',matches);},onError:fail};
      }else if(recordEnabled){
        try{
          const identity=await fingerprint(sources);
          if(attempt.cancelled)throw Error('练习已取消');
          attempt.recorder=createBrowserSoloRecorder({game:'02',language,runtime,identity,patch:attempt.patch,boundaryMode:'sleep',disk:data.slice(),baseDisk,practice,audioMode,
            onStatus:text=>{attempt.recordStatus=text;emit('onRecordingStatus',text);},onFailure:error=>emit('onFailure',error),
            onFile:(blob,result)=>{attempt.blob=blob;emit('onFile',blob,result,practice);},onResult:result=>emit('onResult',result,practice)});
          externalReplay=attempt.recorder.config;
        }catch(error){if(attempt.cancelled)throw error;attempt.recordingError=error.message;}
      }
      if(attempt.cancelled)throw Error('练习已取消');
      const config={canvas,externalReplay,practice,practiceStandby:standby,practicePatch:attempt.patch,wasmBinary:resources.wasmBinary,applyReplayInput,
        onError:fail,onExit:()=>{if(attempt.cancelled)return;if(attempt.foreground){attempt.recorder?.finish();emit('onExit');}else{attempt.error=Error('预备练习意外退出');attempt.stop();}}};
      attempt.core=await DOSBoxPC98.create(config);
      if(attempt.cancelled){attempt.core.dispose();throw Error('练习已取消');}
      if(attempt.error)throw attempt.error;
      attempt.recorder?.attach(attempt.core);
      attempt.core.module.observeReplayBoundary=()=>attempt.music?.poll(attempt.core.module.HEAPU8,performance.now());
      await attempt.core.addDiskImage('th02-solo.hdi',data);
      await attempt.core.setHdd(0,'th02-solo.hdi');
      if(attempt.cancelled)throw Error('练习已取消');
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
