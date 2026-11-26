import {spawn} from 'node:child_process';
import {fileURLToPath} from 'node:url';
let port=8174,open=true;
for(const argument of process.argv.slice(2)){
  if(argument==='--no-open')open=false;
  else if(/^--port=\d+$/.test(argument))port=Number(argument.slice(7));
  else throw Error('未知启动参数：'+argument);
}
if(!Number.isInteger(port)||port<1||port>65535)throw Error('本地端口无效');
process.chdir(fileURLToPath(new URL('.',import.meta.url)));
process.env.EAGLER_TOUHOU_HOST='127.0.0.1';
process.argv=[process.argv[0],'launcher/scripts/serve-static.mjs','site',String(port)];
await import('./launcher/scripts/serve-static.mjs');
if(open){
  const url=`http://127.0.0.1:${port}/`;
  for(let attempt=0;attempt<100;attempt++){
    try{if((await fetch(url)).ok){spawn('cmd.exe',['/c','start','',url],{windowsHide:true,stdio:'ignore'});break;}}
    catch{}
    await new Promise(resolve=>setTimeout(resolve,50));
  }
}
