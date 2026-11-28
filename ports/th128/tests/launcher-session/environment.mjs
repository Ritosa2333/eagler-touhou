export function fakeWindow({search='?runtimeEpoch=41',referrer='https://launcher.example/app',standalone=false}={}){
 const listeners=new Map(),messages=[],parent={postMessage(data,origin){messages.push({data,origin});}};
 const win={location:{origin:'https://game.example',href:'https://game.example/'+search,search},document:{referrer},parent,
  addEventListener(type,fn){if(!listeners.has(type))listeners.set(type,new Set());listeners.get(type).add(fn);},
  removeEventListener(type,fn){listeners.get(type)?.delete(fn);},
  dispatchEvent(event){for(const fn of [...(listeners.get(event.type)??[])])fn(event);},
  listeners,messages};
 if(standalone)win.parent=win;
 const send=(data,{origin='https://launcher.example',source=parent}={})=>win.dispatchEvent({type:'message',source,origin,data});
 return{win,parent,messages,send};
}
export const envelope=(command,request='r1',fields={})=>({protocol:'eagler-touhou/1',game:'th128',epoch:41,command,request,...fields});
export const flush=()=>new Promise(resolve=>setImmediate(resolve));
