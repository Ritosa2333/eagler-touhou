import {spawn} from 'node:child_process';
import {resolve} from 'node:path';
import {writeFile,mkdir} from 'node:fs/promises';
const repo=resolve(import.meta.dirname,'../../..'),out=resolve(process.env.TH05_TEST_OUTPUT??resolve(repo,'ports/th05-mp/artifacts/launcher-multiplayer'));await mkdir(out,{recursive:true});
const port=Number(process.env.TH05_TEST_PORT??18455),count=process.env.TH05_MP_PLAYERS??'2',route=process.env.TH05_MP_ROUTE??'rtc';
let relayLog='',siteLog='',relayPort;
const relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{cwd:repo,windowsHide:true,stdio:['ignore','pipe','pipe'],env:{...process.env,EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_STUN_URLS:''}});
relay.stdout.on('data',b=>{relayLog+=b;const match=relayLog.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(match)relayPort=Number(match[1]);});relay.stderr.on('data',b=>relayLog+=b);
let server;
try{
 const until=async(check,label)=>{const end=Date.now()+30000;while(!check()){if(Date.now()>end)throw Error(label+' timeout\n'+relayLog+'\n'+siteLog);await new Promise(r=>setTimeout(r,50));}};
 await until(()=>relayPort,'relay');
 server=spawn(process.execPath,['launcher/scripts/serve.mjs',String(port)],{cwd:repo,windowsHide:true,stdio:['ignore','pipe','pipe'],env:{...process.env,EAGLER_DEVELOPMENT_GAMES:'th05',EAGLER_TOUHOU_NETPLAY_RELAY:`ws://127.0.0.1:${relayPort}/`,EAGLER_TH05_LANGUAGE_PACK_DIR:resolve(repo,'ports/th05/artifacts/languages'),EAGLER_DEVELOPMENT_VANILLA_FONT:resolve(repo,'dependencies/unifont-15.1.05/unifont-15.1.05.otf'),EAGLER_TOUHOU_ARTWORK_DIR:resolve(repo,'ports/th05/artifacts/content/artwork')}});
 server.stdout.on('data',b=>siteLog+=b);server.stderr.on('data',b=>siteLog+=b);await until(()=>siteLog.includes('eagler-touhou:'),'site');
 const test=spawn(process.execPath,['tools/ports/room-browser.mjs',`--url=http://127.0.0.1:${port}/`,'--game=th05',...(process.env.TH05_TEST_PACKAGES?[`--packages=${resolve(process.env.TH05_TEST_PACKAGES)}`]:[]),`--players=${count}`,`--route=${route}`,`--output=${out}`],{cwd:repo,windowsHide:true,stdio:'inherit',env:process.env});
 const code=await new Promise((resolve,reject)=>{test.on('exit',resolve);test.on('error',reject);});if(code)throw Error('Launcher room test failed: '+code);
}finally{server?.kill();relay.kill();await writeFile(resolve(out,'relay.log'),relayLog);await writeFile(resolve(out,'site.log'),siteLog);}
