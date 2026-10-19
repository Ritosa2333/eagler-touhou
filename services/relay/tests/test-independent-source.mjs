import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,cp,mkdir,symlink,rm} from 'node:fs/promises';
import {existsSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {spawn} from 'node:child_process';
import {once} from 'node:events';

test('cold service plus common accepts another client without any Launcher files',async()=>{
  const source=resolve(import.meta.dirname,'..'),work=await mkdtemp(resolve(tmpdir(),'eagler-independent-relay-'));
  const service=resolve(work,'services/relay');let child,socket,log='';
  try {
    await mkdir(service,{recursive:true});
    await cp(resolve(source,'src'),resolve(service,'src'),{recursive:true});
    await cp(resolve(source,'../../common/contracts'),resolve(work,'common/contracts'),{recursive:true});
    await symlink(resolve(source,'node_modules'),resolve(service,'node_modules'),process.platform==='win32'?'junction':'dir');
    assert.equal(existsSync(resolve(work,'launcher')),false);
    child=spawn(process.execPath,['src/netplay-relay.mjs'],{cwd:service,windowsHide:true,stdio:['ignore','pipe','pipe'],
      env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0'}});
    const port=await new Promise((yes,no)=>{
      const timeout=setTimeout(()=>no(Error('Independent relay startup: '+log)),10000);
      child.stderr.on('data',bytes=>log+=bytes);
      child.stdout.on('data',bytes=>{log+=bytes;const m=/listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log);if(m){clearTimeout(timeout);yes(Number(m[1]));}});
      child.once('exit',code=>{clearTimeout(timeout);no(Error('Independent relay exited '+code+': '+log));});
    });
    socket=new WebSocket(`ws://127.0.0.1:${port}/?room=th08mp-19283746&lobby=other_shell_12345&member=other_member_12345&intent=create`);
    const initial=await once(socket,'message');
    const message=JSON.parse(String(initial[0].data));
    assert.equal(message.type,'state');assert.equal(message.roomDirectory.runtimeVersions,true);
    const next=once(socket,'message');socket.send(JSON.stringify({type:'take-seat',seat:0,loadout:0}));
    const seated=JSON.parse(String((await next)[0].data));
    assert.equal(seated.room.seats[0].clientId,'other_shell_12345');
    const closed=once(socket,'close');socket.close();await closed;socket=null;
  } finally {
    socket?.close();
    if(child&&child.exitCode===null){const exited=once(child,'exit');child.kill();await exited;}
    await rm(work,{recursive:true,force:true});
  }
});
