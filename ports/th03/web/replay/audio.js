// Playback audio is presentation only. Seeking consumes commands with the device suspended.
export function createReplayAudio(game,onError=()=>{},{language='jp',musicEnabled=true}={}){
  let context,bgm,sfx,native,loading,ready=false,closed=false,muted=true,commands=[],lastName='',nativeState={cue:2,end:0,ids:[]},seSequence=0;
  const facade={th_get:n=>n===79?nativeState.cue:n===11?nativeState.end:0,th_sound_event:n=>nativeState.ids[n-nativeState.end+nativeState.ids.length]||0};
  function dispose(){const n=native;native=null;n?.stop();bgm?.dispose();sfx?.dispose();bgm=sfx=null;if(!n)void context?.close().catch(()=>{});context=null;}
  function apply(data){
    if(data.type==='replay-audio-reset'){
      commands=[];lastName='';seSequence=data.sequence||0;bgm?.stop();sfx?.stop();
      if(game==='04'&&sfx)sfx.sequence=seSequence;
      if(native){native.effect?.stop();native.cursor=data.nativeEnd||0;}
      for(const e of data.events||[])apply(e);return;
    }
    if(data.type==='native-audio'){nativeState=data.event;if(native&&muted)native.cursor=nativeState.end;native?.update(facade);return;}
    if(data.type==='music'){
      const e=data.event;if(e.name&&lastName!==e.name){lastName=e.name;bgm?.commandTrack(0,lastName);}if(lastName)bgm?.commandTrack(e.ax,lastName);
    }else if(data.type==='battle-audio'){const [ax,hash]=data.event;if((ax>>>8)===0x0c){if(!muted)sfx?.command(ax);}else bgm?.command(ax,hash);}
    else if(data.type==='sfx'){if(!muted)sfx?.play(data.id);}
    else if(data.type==='se'){seSequence=data.event.sequence;if(muted){if(sfx)sfx.sequence=seSequence;}else sfx?.confirm(data.event);}
    else if(data.type==='se-stop')sfx?.stop();
  }
  return {
    event(data){
      if(closed)return;
      if(data.type==='replay-audio-reset'){
        if(!ready){commands=[];lastName='';seSequence=data.sequence||0;commands.push(data);return;}
        try{apply(data);}catch(e){onError(e);}return;
      }
      if(!ready){if(data.type==='music'||data.type==='native-audio'||data.type==='battle-audio'&&(data.event[0]>>>8)!==0x0c){commands.push(data);if(commands.length>64)commands.shift();}if(data.type==='se')seSequence=data.event.sequence;return;}
      try{apply(data);}catch(e){onError(e);}
    },
    async enable(){
      if(closed)return;
      loading??=(async()=>{
        if(game==='01'){const {LocalAudio}=await import('../netplay/audio.js');native=new LocalAudio();await native.unlock();context=native.context;await context.suspend();return;}
        context=new AudioContext({sampleRate:44100,latencyHint:'interactive'});await context.suspend();
        if(game==='03'){
          if(!['cn','jp'].includes(language))throw Error('Replay 语言无效');
          const [{Fat12},{loadVerifiedBytes},{extractOriginalPmd},{PmdPlayer},{OriginalSfx},{LocalBgm}]=await Promise.all([
            import('../disk.js'),import('../startup-assets.js'),import('../pmd-archives.mjs'),import('../pmd-player.mjs'),import('../original-sfx.mjs'),import('../local-bgm.js')]);
          const response=await fetch(new URL('../disks/manifest.json',import.meta.url));if(!response.ok)throw Error('Replay 音频资源清单加载失败');
          const disk=(await response.json()).games[`3-${language}`];if(!disk)throw Error('Replay 音频语言资源不存在');
          const fat=new Fat12(await loadVerifiedBytes(disk.url,disk.sha256,{gzip:true}));
          if(musicEnabled){bgm=new LocalBgm(context,onError);bgm.pmd={tracks:extractOriginalPmd('th03',fat),engine:await PmdPlayer.prepare(context,new URL('../',import.meta.url))};await bgm.load({warmOpening:false});}
          sfx=await OriginalSfx.create('th03',fat);
        }else{
          const {LocalBgm}=await import('../netplay/local-bgm.js');bgm=new LocalBgm(context,()=>{});
          if(game==='04'){const {LocalSe}=await import('../netplay/local-se.js');sfx=new LocalSe(context);}else{const {LocalSfx}=await import('../netplay/local-sfx.js');sfx=new LocalSfx(context);}
        }
        if(game!=='03')await Promise.all([bgm.load({warmOpening:false}),sfx.load()]);if(game==='04')sfx.sequence=seSequence;
      })();
      try{await loading;if(closed){dispose();return;}ready=true;for(const e of commands.splice(0))apply(e);native?.update(facade);this.mode(muted);}catch(e){dispose();ready=false;loading=null;if(!closed)onError(e);}
    },
    mode(silent){silent=!!silent;muted=silent;if(!ready)return;const target=silent?'suspended':'running';
      if(silent){sfx?.stop();native?.effect?.stop();}
      for(const device of new Set([context,sfx?.context]))if(device&&device.state!==target)void (silent?device.suspend():device.resume()).catch(onError);
      if(!silent)native?.update(facade);
    },
    info(){return {state:context?.state??'loading',effectsState:sfx?.context?.state??null,music:bgm?.active?.id??null,musicStatus:bgm?.status??null,effects:sfx?.info?.()??null,ready,muted};},
    stop(){closed=true;dispose();}
  };
}
