// Isolated child process groups are Linux-only cloud runner infrastructure.
// Never search/kill browsers globally: cleanup is limited to our child's PGID.
import {spawnSync} from 'node:child_process';
import {openSync,closeSync} from 'node:fs';
const sleep = milliseconds => Atomics.wait(new Int32Array(new SharedArrayBuffer(4)),0,0,milliseconds);
function groupMembers(pgid){
 const result=spawnSync('ps',['-eo','pid=,ppid=,pgid=,stat=,comm='],{encoding:'utf8',timeout:5000});
 if(result.error||result.status!==0)throw Error('Cannot inspect owned process group: '+String(result.error||result.stderr));
 return result.stdout.trim().split('\n').filter(Boolean).map(line=>{
  const [pid,ppid,group,state,...name]=line.trim().split(/\s+/);
  return {pid:Number(pid),ppid:Number(ppid),pgid:Number(group),state,name:name.join(' ')};
 }).filter(p=>p.pgid===pgid);
}
export function runCloudCommand(program,args,{cwd,env=process.env,log,timeout=600000}={}){
 if(process.platform!=='linux')throw Error('Cloud process-group isolation requires Linux');
 const fd=openSync(log,'wx');let result;
 try{result=spawnSync(program,args,{cwd,env,stdio:['ignore',fd,fd],timeout,killSignal:'SIGKILL',detached:true});}
 finally{closeSync(fd);}
 const record={program,args,cwd,exitCode:result.status,signal:result.signal,error:result.error?String(result.error):null,log,
  processGroup:result.pid??null,cleanup:{verified:false,liveBefore:[],liveAfter:[],zombies:[]}};
 if(!Number.isInteger(result.pid)||result.pid<=1){
  // spawn failed before a process existed; there cannot be descendants.
  record.cleanup.verified=Boolean(result.error);return record;
 }
 try{
  const before=groupMembers(result.pid);record.cleanup.liveBefore=before.filter(p=>!p.state.startsWith('Z'));
  if(record.cleanup.liveBefore.length){
   try{process.kill(-result.pid,'SIGKILL');record.cleanup.signal='SIGKILL';}
   catch(error){if(error.code!=='ESRCH')throw error;record.cleanup.alreadyExited=true;}
  }
  for(let attempt=0;attempt<50;++attempt){
   const after=groupMembers(result.pid);record.cleanup.zombies=after.filter(p=>p.state.startsWith('Z'));
   record.cleanup.liveAfter=after.filter(p=>!p.state.startsWith('Z'));
   if(!record.cleanup.liveAfter.length){record.cleanup.verified=true;break;}
   sleep(20);
  }
 }catch(error){record.cleanup.error=String(error);}
 return record;
}
