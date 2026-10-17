// Parent-side IPC for one explicitly owned diagnostic relay child.
import {spawn} from 'node:child_process';
import {fileURLToPath} from 'node:url';

export function startRelayDiagnosticProcess({relayPath,port,config={},env={}}){
 if(typeof relayPath!=='string'||!Number.isInteger(port)||port<1||port>65535)throw Error('Invalid owned relay launch');
 const entry=fileURLToPath(new URL('./relay-transport-fixture.mjs',import.meta.url));
 const child=spawn(process.execPath,[entry],{windowsHide:true,env:{...process.env,...env,
  EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_STUN_URLS:'',
  TH09_RELAY_SOURCE:relayPath,TH09_RELAY_TRACE_CONFIG:JSON.stringify(config)},stdio:['ignore','pipe','pipe','ipc']});
 let serial=0,stopRequest=null;
 const waiting=new Map();
 const failAll=error=>{for(const item of waiting.values()){clearTimeout(item.timer);item.reject(error);}waiting.clear();};
 child.on('message',message=>{
  if(message?.type!=='th09-relay-trace')return;
  const item=waiting.get(message.id);if(!item)return;
  clearTimeout(item.timer);waiting.delete(message.id);item.resolve(message.trace);
 });
 child.on('error',failAll);
 child.on('exit',(code,signal)=>failAll(Error('Diagnostic relay exited before snapshot: '+code+'/'+signal)));
 function request(stop,timeoutMs){
  if(!child.connected)return Promise.reject(Error('Diagnostic relay IPC is not connected'));
  return new Promise((resolve,reject)=>{
   const id=++serial,timer=setTimeout(()=>{waiting.delete(id);reject(Error('Diagnostic relay snapshot timeout'));},timeoutMs);
   waiting.set(id,{resolve,reject,timer});
   child.send({type:'th09-relay-trace',id,operation:stop?'stop':'snapshot'},error=>{
    if(!error)return;const item=waiting.get(id);if(!item)return;
    clearTimeout(item.timer);waiting.delete(id);item.reject(error);
   });
  });
 }
 return {process:child,snapshot({stop=false,timeoutMs=5000}={}){
  if(!Number.isInteger(timeoutMs)||timeoutMs<1||timeoutMs>30000)return Promise.reject(Error('Invalid snapshot timeout'));
  if(stop)return stopRequest??=(request(true,timeoutMs));
  return request(false,timeoutMs);
 }};
}

