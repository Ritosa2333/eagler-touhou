import {validateTouchSnapshot} from './preview-input.mjs';

// This is the local preview bridge. Managed DATA and product configuration are unfinished.
export function createCommandHandler(api) {
  let configured=false;
  return async m=>{
    switch(m.command){
      case 'configure': {
        if(api.running())throw Error('运行中不能重新配置');
        if(!['ogg','none'].includes(m.music))throw Error('本版未支持此音乐模式');
        for(const field of ['resources','runtimeResources','sharedResources']){
          if(m[field]!==undefined&&!Array.isArray(m[field]))throw Error('资源列表无效');
          if(m[field]?.length)throw Error('本地初版尚未接入受管理 DATA 挂载');
        }
        if(m.runtimePack)throw Error('本地初版尚未接入受管理 DATA 挂载');
        if(m.options!==undefined&&(m.options===null||typeof m.options!=='object'||Array.isArray(m.options)||Object.keys(m.options).length))throw Error('本地初版尚未接入 Runtime 配置选项');
        api.music(m.music==='ogg');configured=true;return {};
      }
      case 'resources':throw Error('本地初版尚未接入附加资源安装');
      case 'keyboard':
        if(typeof m.code!=='string'||typeof m.down!=='boolean')throw Error('键盘输入无效');
        api.key(m.code,m.down);return {};
      case 'keyboard-clear':api.keyboardClear();return {};
      case 'touch-cancel':api.touchCancel();return {};
      case 'touch-controls':api.touch(validateTouchSnapshot(m));return {};
      case 'direct-touch':
        if(!['down','move','up'].includes(m.type)||!Number.isSafeInteger(m.id)||m.id<0||!Number.isFinite(m.x)||!Number.isFinite(m.y))throw Error('触控坐标无效');
        api.pointer(m);return {};
      case 'launch':
        if(!configured)throw Error('须先完成 configure');
        if(api.running())throw Error('游戏已在运行或准备中');
        await api.start();return {};
      case 'list':return {files:await api.store.list()};
      case 'read':{const bytes=await api.store.read(m.path);if(!bytes)throw Error('文件不存在');return {bytes:Array.from(bytes)};}
      case 'write':
        if(!Array.isArray(m.bytes)||m.bytes.length>64*1024*1024||m.bytes.some(b=>!Number.isInteger(b)||b<0||b>255))throw Error('文件字节无效');
        await api.store.write(m.path,new Uint8Array(m.bytes));return {};
      case 'remove':await api.store.remove(m.path);await api.store.sync();return {};
      case 'sync':await api.store.sync();return {};
      default:throw Error('不支持的命令');
    }
  };
}

export function installShell(api,environment=window){
  const epoch=Number(new URL(environment.location.href).searchParams.get('runtimeEpoch'));
  const enabled=environment.parent!==environment&&Number.isSafeInteger(epoch)&&epoch>0;
  const post=payload=>{if(enabled)environment.parent.postMessage({protocol:'eagler-touhou/1',game:'th12',epoch,...payload},environment.location.origin);};
  const handle=createCommandHandler(api);
  environment.addEventListener('message',async event=>{
    const m=event.data;
    if(!enabled||event.source!==environment.parent||event.origin!==environment.location.origin||m?.protocol!=='eagler-touhou/1'||m?.game!=='th12'||m?.epoch!==epoch)return;
    try{const result=await handle(m);if(typeof m.request==='string')post({request:m.request,ok:true,...result});}
    catch(e){if(typeof m.request==='string')post({request:m.request,ok:false,error:e.message});else post({event:'error',message:e.message});}
  });
  post({event:'ready',saveRoot:'/savesth12'});return post;
}
