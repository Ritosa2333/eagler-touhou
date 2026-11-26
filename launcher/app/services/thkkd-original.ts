import {FFmpeg} from '@ffmpeg/ffmpeg';
import coreURL from '@ffmpeg/core?url';
import wasmURL from '@ffmpeg/core/wasm?url';
import classWorkerURL from '@ffmpeg/ffmpeg/worker?worker&url';
import {convertThkkdArchives,thkkdPackage} from '../../src/launcher/thkkd-original.mjs';
import {convertThkkdImage} from '../../src/launcher/thkkd-image.mjs';
import type {RetailInput,RetailTrack} from '../../src/launcher/retail-package.mjs';

export async function importThkkdOriginal(inputs: readonly RetailInput[], {signal,onTrack}: {signal?:AbortSignal;onTrack?(track:RetailTrack):void}) {
  let ffmpeg:FFmpeg|undefined;
  let workerUrl:string|undefined;
  const abort=()=>ffmpeg?.terminate(); signal?.addEventListener('abort',abort,{once:true});
  try {
    const data=await convertThkkdArchives(inputs,{signal,onTrack,convertImage:convertThkkdImage,
      async convertVideo(bytes,name,_signal,onProgress) {
        signal?.throwIfAborted();
        if(!ffmpeg){
          onProgress(0);
          const response=await fetch(classWorkerURL,{signal});
          if(!response.ok)throw Error('视频转换组件加载失败（'+response.status+'）');
          const source=await response.blob();
          signal?.throwIfAborted();
          // Blob Worker 继承当前页面的隔离策略，兼容离线缓存接管后的页面。
          workerUrl=URL.createObjectURL(new Blob([source],{type:'text/javascript'}));
          ffmpeg=new FFmpeg();
          let timeout:ReturnType<typeof setTimeout>|undefined;
          try{
            await Promise.race([
              ffmpeg.load({coreURL:new URL(coreURL,location.href).href,wasmURL:new URL(wasmURL,location.href).href,classWorkerURL:workerUrl},{signal}),
              new Promise<never>((_,reject)=>{timeout=setTimeout(()=>reject(Error('视频转换器启动超时，请重新打开页面后重试')),120000);}),
            ]);
          }finally{clearTimeout(timeout);}
        }
        const worker=ffmpeg,input='original.wmv',output='browser.webm';
        const progress=({progress}: {progress:number})=>onProgress(progress);
        worker.on('progress',progress);
        await worker.writeFile(input,new Uint8Array(bytes),{signal});
        try {
          const code=await worker.exec(['-i',input,'-an','-c:v','libvpx','-crf','10','-b:v','2M','-deadline','realtime','-cpu-used','8','-fps_mode','vfr',output],-1,{signal});
          if(code!==0)throw Error('海惠堂视频转换失败：'+name);
          const result=await worker.readFile(output,'binary',{signal});
          if(typeof result==='string')throw Error('海惠堂视频输出格式无效');
          return new Blob([new Uint8Array(result).buffer],{type:'video/webm'});
        }finally{worker.off('progress',progress);await worker.deleteFile(input).catch(()=>{});await worker.deleteFile(output).catch(()=>{});}
      }});
    signal?.throwIfAborted();return await thkkdPackage(data);
  }finally{signal?.removeEventListener('abort',abort);ffmpeg?.terminate();if(workerUrl)URL.revokeObjectURL(workerUrl);}
}
