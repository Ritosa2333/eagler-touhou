import {spawn} from 'node:child_process';
import {resolve} from 'node:path';
import {existsSync} from 'node:fs';
const root=resolve(import.meta.dirname,'../../..');
const children=new Set();let closing=false;
function stop(){if(closing)return;closing=true;for(const child of children)child.kill();}
for(const event of ['SIGINT','SIGTERM','exit'])process.on(event,stop);
function run(script,args=[],env={},service=false){
  const child=spawn(process.execPath,[resolve(root,script),...args],{cwd:root,windowsHide:true,stdio:'inherit',env:{...process.env,...env}});
  children.add(child);child.on('exit',code=>{children.delete(child);if(service&&!closing){console.error(script+' exited: '+code);process.exitCode=code||1;stop();}});child.on('error',error=>{console.error(error);stop();process.exitCode=1;});return child;
}
function complete(child){return new Promise((yes,no)=>child.once('exit',code=>code===0?yes():no(Error('Build failed: '+code))));}
await complete(run('ports/th03/scripts/build-eagler.mjs',['--assets='+(process.env.PC98_ASSETS_DIR||resolve(root,'../th03-netplay-lab/web'))]));
if(!existsSync(resolve(root,'launcher/.cache/build/ui-rewrite/client/index.html')))throw Error('请先执行 npm --prefix launcher run build:ui-rewrite');
const relayPort=process.env.PC98_RELAY_PORT||'18331',port=process.env.PC98_LAUNCHER_PORT||'18330';
run('services/relay/src/netplay-relay.mjs',[],{EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:relayPort,EAGLER_NETPLAY_STUN_URLS:''},true);
run('launcher/scripts/serve.mjs',[port],{EAGLER_DEVELOPMENT_GAMES:process.env.EAGLER_DEVELOPMENT_GAMES||'th03',EAGLER_TOUHOU_NETPLAY_RELAY:'ws://127.0.0.1:'+relayPort},true);
console.log('TH03 Launcher: http://127.0.0.1:'+port+'/?test=th03');
