let managedFrames=0,managedPrevious=0,managedEnded=false;
function nativeSample(){return globalThis.__eaglerPC98Input.nativeSample?.()??globalThis.__eaglerPC98Input.sample();}
function buttons(){return [globalThis.__eaglerPC98Input.localSample(),0,0];}
function managedMenuInput(){
 if(!started||configuring||managedEnded)return;
 const bits=nativeSample(),edge=bits&~managedPrevious;managedPrevious=bits;
 if(core.th_get(50,0)){const key=edge&16?0:edge&32?1:edge&1?2:edge&2?3:edge&64?(core.th_get(50,0)===12?6:4):edge&4?4:edge&(8|128)?5:-1;if(key>=0)menuInput(key);}
 else if(edge&128)setPaused(!paused);else if(edge&(4|64)&&core.th_get(0,0)>=2){core.th_continue();updatePhase();}
}
async function managedExit(){if(managedEnded)return;managedEnded=true;nativeRecorder?.finish('退出');managedAPI.stop();globalThis.__eaglerPC98Emit({event:'exit',code:0});}
export const managedAPI={
 async start({options,music}){if(options.replayPath)throw Error('TH01 原生命令录像播放器尚未接入');musicWanted=music!=='none';$('players').value='1';$('startStage').value='normal';await begin();},
 health(){return {frames:managedFrames,audio:{state:musicContext?.state??'suspended'}};},
 async files(message){if(['sync','list','read'].includes(message.command)&&core){const data=new Uint8Array(core.memory.buffer,core.th_score_data(),core.th_score_size()).slice();if(!stageSelection(sessionConfig?.startStage).practice)await globalThis.__eaglerPC98Files.write('scores.dat',data);}return globalThis.__eaglerPC98FileCommands(message);},
 playback(){throw Error('TH01 原生命令录像播放器尚未接入');},
 stop(){managedEnded=true;configuring=true;release();stopEffects();stopMusicSource();nativeRecorder?.close();void musicContext?.suspend();},
};
