import {encodeExternalReplay,MAX_CLOCK_BYTES,MAX_EVENT_BYTES} from './external-codec.js';

export function createBrowserSoloRecorder({game,language,runtime='main',identity,boundaryMode='sleep',patch,disk,baseDisk=disk,diskBaseKind,savedFiles,practice,audioMode,onStatus=()=>{},onFile=()=>{},onResult=()=>{},onFailure=()=>{},deferEncoding=true}){
  let core=null,failed=false,disposed=false,ending=false,clockBytes=0,eventBytes=2,events=[],clockChunks=[],result=null,file=null,encoding=null;
  function fail(error){if(failed||disposed)return;failed=true;file=null;events=[];clockChunks=[];core?.stopBrowserRecording();onFailure(error);onStatus('Replay 录制已停止：'+error.message+'；游戏继续。');}
  const config={mode:'record',boundaryMode,...(patch?{lifecycle:{game,patch}}:{}),
    onChunk:chunk=>{
      if(failed||disposed)return;
      if(chunk.clocks){clockBytes+=chunk.clocks.length;if(clockBytes>MAX_CLOCK_BYTES){fail(Error('时钟记录超过容量'));return;}clockChunks.push(chunk.clocks);}
      if(chunk.events){eventBytes+=new TextEncoder().encode(JSON.stringify(chunk.events)).length;if(eventBytes>MAX_EVENT_BYTES){fail(Error('输入记录超过容量'));return;}events.push(...chunk.events);}
    },
    onRecordingError:fail,
    onEnd:value=>{
      if(failed||disposed||ending)return;ending=true;result=value;onResult(value);
      if(deferEncoding)onStatus('Replay 已记录，可选择保存或丢弃。');
      else void encode().catch(()=>{});
    },
  };
  async function encode(){
    if(file)return file;
    if(encoding)return encoding;
    if(!result||failed||disposed)throw Error('Replay 尚未就绪');
    encoding=(async()=>{
      onStatus('正在生成 Replay…');
      if(!(disk instanceof Uint8Array))throw Error('启动磁盘不可用');
      const blob=await encodeExternalReplay({game,language,runtime,identity,boundaryMode,...(diskBaseKind?{diskBaseKind}:{}),...(practice?{practice}:{}),...(audioMode?{audioMode}:{}),endBoundary:result.boundary,result},{clockChunks,events,disk,baseDisk,savedFiles});
      if(disposed||failed)throw Error('Replay 已丢弃');
      file=blob;events=[];clockChunks=[];onFile(blob,result);onStatus('Replay 已就绪，可保存或丢弃。');return blob;
    })().catch(error=>{fail(error);throw error;});
    return encoding;
  }
  return {config,attach(value){core=value;if(failed)core?.stopBrowserRecording();},finish(){if(!ending&&!failed)core?.finishReplay();},save:encode,
    dispose(){disposed=true;core?.stopBrowserRecording();events=[];clockChunks=[];file=null;},
    get state(){return {failed,ending,ready:!!result,clockBytes,eventBytes,result};},get file(){return file;}};
}
