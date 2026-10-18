// Use title-owned native recording checks. TH08 records admitted real peers;
// TH10 records native input lanes with the existing local algorithm harness.
// All helper services bind loopback only.
import {fork,spawn,execFileSync} from 'node:child_process';
import {resolve,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {mkdirSync,createWriteStream,readdirSync,copyFileSync,existsSync,writeFileSync} from 'node:fs';
import {createServer} from 'node:net';
import {build} from 'esbuild';
const project=resolve(fileURLToPath(new URL('../../',import.meta.url))),workspace=resolve(project,'../..');
const game=process.argv.includes('--th06')?'th06':process.argv.includes('--th07')?'th07':process.argv.includes('--th10')?'th10':'th08';
const repoOption=process.argv.find(value=>value.startsWith('--repo='));
if(repoOption&&!['th08','th10'].includes(game))throw Error('--repo is supported by the TH08/TH10 directory Runtime checks only');
const repo=resolve(repoOption?repoOption.slice('--repo='.length):resolve(workspace,game==='th10'?'worktrees/th10-multiplayer':game+'-eagler'));
const ownerGit=(...args)=>execFileSync('git',['-c','safe.directory='+repo,'-C',repo,...args],{encoding:'utf8',windowsHide:true,stdio:['ignore','pipe','pipe']}).trim();
const ownerBranch=ownerGit('branch','--show-current');
if(['th08','th10'].includes(game)&&ownerBranch!=='eagler-mp')throw Error(`${game} MP Replay check requires an eagler-mp checkout; ${repo} is on ${ownerBranch||'detached HEAD'}. Select the MP checkout with --repo=PATH.`);
const laterStage=game==='th10'&&process.argv.includes('--stage2');
const output=resolve(project,'artifacts/replay-workspace/'+game+'mp'+(laterStage?'-stage2':''));mkdirSync(output,{recursive:true});
writeFileSync(resolve(output,'owner.json'),JSON.stringify({game,repo,branch:ownerBranch,commit:ownerGit('rev-parse','HEAD')},null,2)+'\n');
const owned=[];
function service(path,env,ipc){return new Promise((ok,no)=>{
 const child=fork(path,[],{cwd:resolve(path,'..'),env:{...process.env,...env},windowsHide:true,stdio:['ignore','pipe','pipe','ipc']});owned.push(child);
 const log=createWriteStream(resolve(output,ipc?'host.log':'relay.log'));let text='';const timer=setTimeout(()=>no(Error('Service startup timeout: '+path)),30000);
 for(const stream of [child.stdout,child.stderr])stream.on('data',chunk=>{log.write(chunk);text=(text+chunk).slice(-10000);if(!ipc){const match=text.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(match){clearTimeout(timer);ok(Number(match[1]));}}});
 child.on('message',message=>{if(ipc&&message.type==='ready'){clearTimeout(timer);ok(message.port);}});
 child.on('error',error=>{clearTimeout(timer);no(error);});child.on('exit',code=>{clearTimeout(timer);log.end();no(Error('Service exited '+code+': '+text));});
});}
try{
 const transport=game!=='th10'&&!process.argv.includes('--existing');
 let relayScript;
 if(transport){
  // Compile the actual relay and canonical typed contracts into this test's
  // isolated output. Do not acquire or retire the shared Launcher build cache.
  relayScript=resolve(output,'relay.mjs');
  const result=await build({entryPoints:[resolve(project,'../services/relay/src/netplay-relay.mjs')],outfile:relayScript,bundle:true,platform:'node',format:'esm',packages:'external',metafile:true,logLevel:'silent',plugins:[{name:'canonical-contracts',setup(ctx){ctx.onResolve({filter:/\.mjs$/},args=>{const file=resolve(args.resolveDir,args.path),contract=relative(resolve(project,'lib/contracts'),file);const source=contract&&!contract.startsWith('..')?resolve(project,'src/contracts',contract.replace(/\.mjs$/,'.mts')):file.startsWith(resolve(project,'src')+'/')||file.startsWith(resolve(project,'src')+'\\')?file.replace(/\.mjs$/,'.mts'):null;if(source&&existsSync(source))return {path:source};});}}]});
  writeFileSync(resolve(output,'relay-sources.json'),JSON.stringify(result.metafile,null,2));
 }
 const relay=transport?await service(relayScript,{EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0'},false):null;
 const runPython=(script,args)=>new Promise((ok,no)=>{const child=spawn('python',[script,...args],{cwd:project,windowsHide:true,stdio:'inherit'});owned.push(child);child.on('error',no);child.on('exit',code=>code?no(Error(game+' native recording check failed '+code)):ok());});
 const playerCount=process.argv.includes('--players=2')?'2':process.argv.includes('--players=3')?'3':'all';
 if(game==='th06'||game==='th07'){
  await runPython(resolve(project,'tests/browser/test-local-replay-th0xmp.py'),['--game',game,'--relay',`ws://127.0.0.1:${relay}`,'--output',resolve(output,'report.json'),'--players',playerCount]);
 }else{
 const reservation=createServer();await new Promise(ok=>reservation.listen(0,'127.0.0.1',ok));const freePort=reservation.address().port;await new Promise(ok=>reservation.close(ok));
 const port=await service(resolve(repo,'portable/multiplayer/serve.mjs'),{PORT:String(freePort),EAGLER_WORKSPACE:workspace,EAGLER_COMMON_ROOT:resolve(workspace,'eagler-common'),TH08_MP_FONT:resolve(workspace,'games/th06/msgothic.ttc'),TH10_MP_FONT:resolve(workspace,'games/th06/msgothic.ttc'),TH10_MP_PROFILE:'multiplayer-fixtures',
  TH08_MP_DATA:process.env.TH08_MP_DATA||'D:/workspace/东方测试.zip/TouhouDev/[th08] 东方永夜抄 (日文版)/th08.dat'},true);
 const args=['--url',`http://127.0.0.1:${port}/`,'--output',resolve(output,'report.json'),...(laterStage?['--case','stage','--target-stage','2','--record-only','--players',playerCount==='all'?'2':playerCount]:transport?['--relay',`ws://127.0.0.1:${relay}`,'--mode','workspace','--players',playerCount]:game==='th10'?['--cases','record','--lags','0','--players',playerCount]:['--recordings',resolve(project,'artifacts/replay-workspace/fixtures/th08mp')])];
 const script=game==='th08'&&!transport?resolve(project,'tests/browser/test-local-replay-th08mp.py'):resolve(repo,'portable/multiplayer/'+(laterStage?'check-replay-completion.py':'check-replay.py'));
 await runPython(script,args);
 const fixtures=resolve(project,'artifacts/replay-workspace/fixtures/'+game+'mp'+(laterStage?'-stage2':''));mkdirSync(fixtures,{recursive:true});
 readdirSync(output).filter(name=>name.endsWith('.rpyx')).forEach((name,i)=>copyFileSync(resolve(output,name),resolve(fixtures,'demo'+i+(game==='th08'?'.rpyx':'.rpy'))));
 }
}finally{
 for(const child of owned.reverse())if(child.exitCode===null&&child.signalCode===null){child.kill();await new Promise(ok=>{const timer=setTimeout(ok,5000);child.once('exit',()=>{clearTimeout(timer);ok();});});}
}
