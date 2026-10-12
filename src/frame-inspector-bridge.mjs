// Development inspector only. It is installed solely by the explicit query
// opt-in, accepts the same-origin parent, and never exists in the normal page.
export function installFrameInspectorBridge(api,environment=window){
  let serial=Promise.resolve();
  environment.addEventListener('message',event=>{
    const message=event.data;
    if(event.source!==environment.parent||event.origin!==environment.location.origin||
      message?.protocol!=='th12-frame-inspector/1'||typeof message.id!=='string')return;
    serial=serial.then(async()=>{
      try{
        if(message.command==='load')await api.load(message.replay,message.stage);
        else if(message.command==='seek')await api.seek(message.frame);
        else if(message.command==='step')await api.step();
        else if(message.command!=='state')throw Error('不支持的检查命令');
        event.source.postMessage({protocol:message.protocol,id:message.id,state:api.state()},event.origin);
      }catch(error){event.source.postMessage({protocol:message.protocol,id:message.id,error:error.message},event.origin);}
    });
  });
}
