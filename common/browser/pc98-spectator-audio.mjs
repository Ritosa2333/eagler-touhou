// Audio uses the same original PMD archives and effect driver as the players.
export function createPc98SpectatorAudio(game,music,onError=()=>{}){
  let context,bgm,sfx,native,closed=false,ready=false,muted=true,appliedMuted=null,loading,lastName='',sequence=0,nativeState={cue:2,end:0,ids:[]},commands=[];
  const facade={th_get:n=>n===79?nativeState.cue:n===11?nativeState.end:0,th_sound_event:n=>nativeState.ids[n-nativeState.end+nativeState.ids.length]||0};
  async function disk(settings){
    if(game==='03'){
      const [{Fat12},{loadVerifiedBytes}]=await Promise.all([import('./disk.js'),import('./startup-assets.js')]);
      const manifest=await(await fetch('./disks/manifest.json')).json(),entry=manifest.games['3-'+settings.language];
      return new Fat12(await loadVerifiedBytes(entry.url,entry.sha256,{gzip:true}));
    }
    const [{Fat12},{assetBytes}]=await Promise.all([import('./solo-fat12.js'),import('./preload-cache.js')]);
    if(game==='02'){
      const patch=await(await fetch('./local-patch.json')).json(),entry=patch.disks[settings.language];
      return new Fat12(await assetBytes(entry.file,{hash:entry.sha256,size:entry.size,gzip:true}));
    }
    const {gameVersion}=await import('./game-version.js'),selected=gameVersion(settings.language),meta=await(await fetch('./'+selected.coopMeta)).json();
    return new Fat12(await assetBytes(selected.coopDisk,{hash:meta.sha256,size:meta.size,gzip:true}));
  }
  function apply(data){
    if(data.type==='replay-audio-reset'){lastName='';sequence=data.sequence||0;bgm?.stop();sfx?.stop();if(sfx)sfx.sequence=sequence;
      if(native)native.cursor=data.nativeEnd||0;for(const event of data.events||[])apply(event);return;}
    if(data.type==='native-audio'){nativeState=data.event;if(native&&muted)native.cursor=nativeState.end;native?.update(facade);return;}
    if(data.type==='music'){const event=data.event;if(event.name&&lastName!==event.name){lastName=event.name;bgm?.commandTrack(0,lastName);}if(lastName)bgm?.commandTrack(event.ax,lastName);}
    else if(data.type==='battle-audio'){const [ax,hash]=data.event;if((ax>>>8)===12){if(!muted)sfx?.command(ax);}else bgm?.command(ax,hash);}
    else if(data.type==='sfx'){if(!muted)sfx?.play(data.id);}
    else if(data.type==='se'){sequence=data.event.sequence;if(muted){if(sfx)sfx.sequence=sequence;}else sfx?.confirm(data.event);}
    else if(data.type==='se-stop')sfx?.stop();
  }
  return {
    event(data){if(closed)return;if(!ready){if(data.type==='music'||data.type==='native-audio'||data.type==='battle-audio'&&(data.event[0]>>>8)!==12||data.type==='replay-audio-reset'){commands.push(data);if(commands.length>64)commands.shift();}if(data.type==='se')sequence=data.event.sequence;return;}try{apply(data);}catch(error){onError(error);}},
    start(settings){
      loading??=(async()=>{
        if(game==='01'){const {LocalAudio}=await import('./netplay/audio.js');native=new LocalAudio();if(music==='none'){native.track=async()=>{};native.prewarm=()=>{};}await native.unlock();context=native.context;}
        else{
          const fat=await disk(settings),[{OriginalSfx},{PmdPlayer},{extractOriginalPmd},{LocalBgm}]=await Promise.all([
            import('./original-sfx.mjs'),import('./pmd-player.mjs'),import('./pmd-archives.mjs'),import(game==='03'?'./local-bgm.js':'./netplay/local-bgm.js')]);
          context=new AudioContext({sampleRate:44100,latencyHint:'interactive'});await context.resume();
          const [effects,engine]=await Promise.all([OriginalSfx.create('th'+game,fat),PmdPlayer.prepare(context,new URL('./',import.meta.url))]);sfx=effects;
          bgm=new LocalBgm(context,()=>{});bgm.pmd={tracks:extractOriginalPmd('th'+game,fat),engine};bgm.setVolume(music==='none'?0:.7);
          await bgm.load({...settings,warmOpening:false});sfx.sequence=sequence;
        }
        if(closed){this.stop();return;}ready=true;for(const event of commands.splice(0))apply(event);appliedMuted=null;this.mode(muted);
      })().catch(error=>{if(!closed)onError(error);});return loading;
    },
    mode(silent){muted=!!silent;if(!context||appliedMuted===muted)return;appliedMuted=muted;if(muted){sfx?.stop();native?.effect?.stop();void context.suspend();void sfx?.context.suspend();}
      else{void context.resume();void sfx?.resume();native?.update(facade);}},
    info(){return {state:context?.state??'loading',effects:sfx?.played??0,effectsPeak:sfx?.peak??0};},
    stop(){closed=true;native?.stop();bgm?.dispose();sfx?.dispose();if(!native)void context?.close();},
  };
}
