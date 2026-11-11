import {loadPracticeResources,installPractice} from './practice-assets.js';
import {createBrowserSoloRecorder} from './replay/external-recorder.js';
import {restoreExternalReplayDisk} from './replay/external-codec.js';
import {fingerprint} from './replay/tape.js';
import {SoloMusic} from './solo-music.js';
import {NP21} from './dosbox-adapter.js';
import {WorkerNP21} from './solo-worker-client.js';
import {gameVersion} from './game-version.js';
import {sha256} from './netplay/sha256.js';

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
      const version=gameVersion(language);
      const manifest=await fetch(version.soloPatch,{cache:'no-cache'}).then(r=>{if(!r.ok)throw Error(`配置 HTTP ${r.status}`);return r.json();});
      if(replay?.disk)data=replay.disk.slice();
      else{
        const disk=await fetch(version.soloDisk,{cache:'no-cache'});if(!disk.ok)throw Error(`磁盘 HTTP ${disk.status}`);
        data=new Uint8Array(await new Response(disk.body.pipeThrough(new DecompressionStream('gzip'))).arrayBuffer());
        if(data.length!==manifest.disk.size||await sha256(data)!==manifest.disk.sha256)throw Error('单机磁盘校验失败');
        baseDisk=data.slice();if(replay)data=await restoreExternalReplayDisk(replay,data);
      }
      if(attempt.cancelled)throw Error('练习已取消');
      if(!replay)await installPractice(data,resources,practice);
      if(audioMode==='independent'){
        const music=attempt.music=new SoloMusic(text=>emit('onStatus',text));
        // No background audio, including boot/title commands.
        music.suspend();
        if(replay)await music.attachReplay();else await music.install(data,'GENSO');
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
          attempt.recorder=createBrowserSoloRecorder({game:'04',language,runtime,identity,patch:attempt.patch,boundaryMode:'sleep',disk:data.slice(),baseDisk,practice,audioMode,
            onStatus:text=>{attempt.recordStatus=text;emit('onRecordingStatus',text);},onFailure:error=>emit('onFailure',error),
            onFile:(blob,result)=>{attempt.blob=blob;emit('onFile',blob,result,practice);},onResult:result=>emit('onResult',result,practice)});
          externalReplay=attempt.recorder.config;
        }catch(error){if(attempt.cancelled)throw error;attempt.recordingError=error.message;}
      }
      if(attempt.cancelled)throw Error('练习已取消');
      const config={canvas,externalReplay,practice,practiceStandby:standby,practicePatch:attempt.patch,wasmBinary:resources.wasmBinary,applyReplayInput,
        onError:fail,onExit:()=>{if(attempt.cancelled)return;if(attempt.foreground){attempt.recorder?.finish();emit('onExit');}else{attempt.error=Error('预备练习意外退出');attempt.stop();}}};
      Object.assign(config,{ExMemory:7,Latencys:100,SampleHz:44100,SNDboard:4,nativeSoloAudio:!externalReplay&&audioMode!=='original',no_mouse:true,use_menu:false,fontfile:'font.bmp'});
      attempt.core=runtime==='worker'?await WorkerNP21.create(config,{game:'04',patch:attempt.patch,music:attempt.music?.reader.patch,onError:fail}):await NP21.create(config);
      if(attempt.cancelled){attempt.core.dispose();throw Error('练习已取消');}
      if(attempt.error)throw attempt.error;
      attempt.recorder?.attach(attempt.core);
      if(attempt.core.isWorker)attempt.core.onMusic=(ax,hash)=>attempt.music?.player.command(ax,hash);
      else attempt.core.module.observeReplayBoundary=()=>attempt.music?.poll(attempt.core.module.HEAPU8,performance.now(),true);
      await attempt.core.addDiskImage('th04-solo.hdi',data);
      await attempt.core.setHdd(0,'th04-solo.hdi');
      if(attempt.cancelled)throw Error('练习已取消');
      attempt.core.run();
      attempt.music?.start();
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
