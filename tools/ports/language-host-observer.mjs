/** Inject before the production Launcher starts; only the current Runtime frame may emit observations. */
export function installLanguageHostObserver({game,contract}){
  if(window!==window.top){
    addEventListener('message',event=>{
      const message=event.data;
      if(event.source!==parent||event.origin!==location.origin||message?.protocol!=='eagler-touhou/1'||
        message.game!==game||message.command!=='configure')return;
      parent.__languageConfigureReceived?.({epoch:message.epoch,at:performance.now(),language:message.language??null,
        pack:message.runtimePack?{language:message.runtimePack.language,bytes:message.runtimePack.bytes,
          sha256:message.runtimePack.sha256??null,
          files:message.runtimePack.files.map(file=>file.path)}:null});
    });
    return;
  }
  const receipt={events:[],enables:[],configures:[],errors:[]};window.__languageHostReceipt=receipt;
  window.__languageConfigureReceived=entry=>receipt.configures.push({...entry,parentAt:performance.now()});
  addEventListener('message',event=>{
    const frame=document.getElementById('gameFrame'),message=event.data;
    if(event.origin!==location.origin||event.source!==frame?.contentWindow||message?.protocol!=='eagler-touhou/1'||message.game!==game)return;
    if(message.event){receipt.events.push({event:message.event,epoch:message.epoch,at:performance.now()});
      if(['error','abort'].includes(message.event))receipt.errors.push({event:message.event,message:message.message??message.error});}
    if(message.event!=='ready')return;
    const runtime=event.source[contract.runtime.global],core=runtime?.[contract.runtime.core];
    const appBefore=runtime?.[contract.runtime.app]??null;
    if(typeof core?.[contract.enableExport]!=='function'){
      receipt.errors.push({event:'observation-unavailable',message:'Native language observation export is absent'});return;
    }
    core[contract.enableExport](1);
    receipt.enables.push({epoch:message.epoch,appBefore,at:performance.now(),source:'production Runtime ready event before configure/launch'});
  });
}
