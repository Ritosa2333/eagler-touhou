import {Fat12,sha256} from './disk.js';
import {NativePause} from './native-pause.js';

// Only MAIN's movement bridge is replaced. No launcher, config, score or music edits.
export async function createSoloAssist(getEmulator){
  const response=await fetch('solo-assist.json');if(!response.ok)throw Error('单人辅助清单加载失败');
  const meta=await response.json();
  async function read(url,hash){const response=await fetch(url);if(!response.ok)throw Error('单人辅助资源加载失败');
    const bytes=new Uint8Array(await response.arrayBuffer());if(await sha256(bytes)!==hash)throw Error('单人辅助资源校验失败');return bytes;}
  const [patched,original]=await Promise.all([read(meta.url,meta.sha256),read(meta.originalUrl,meta.originalSha256)]);
  const bridge=new NativePause(getEmulator,meta);
  return {bridge,
    async install(data){const fat=new Fat12(data),entry=fat.find('YUMEZIKU/MAIN.EXE');
      if(await sha256(fat.read(entry))!==meta.originalSha256)throw Error('请使用原版单人存档，当前 MAIN.EXE 版本不匹配');
      fat.replace(entry,patched);return data;},
    exportDisk(data){const copy=new Uint8Array(data),fat=new Fat12(copy);
      // Keep saved and exported games compatible with an unmodified DOS installation.
      fat.replace(fat.find('YUMEZIKU/MAIN.EXE'),original);return copy;}
  };
}
