// Current-machine control using the existing TH09 fixture and frozen binary.
// No TH09 source is changed. Resolve installed test dependencies and supply two
// explicitly missing spectator-only ES modules in its old server allowlist.
import {registerHooks,createRequire} from 'node:module';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {resolve,dirname} from 'node:path';
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import http from 'node:http';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'../..'),th09=resolve(root,'../th09-managed-touch-4x');
const require=createRequire(resolve(root,'../../eagler-touhou/th11/tools/diagnostics/package.json'));
registerHooks({resolve(name,context,next){
 if(name==='playwright'||name==='ws')return {url:pathToFileURL(resolve(dirname(require.resolve(name)),name==='ws'?'wrapper.mjs':'index.mjs')).href,shortCircuit:true};
 return next(name,context);
}});
const out=resolve(root,'artifacts/rollback-smoothness',process.env.RUN_LABEL||'th09-control');
mkdirSync(out,{recursive:true});if(existsSync(resolve(out,'control.json')))throw Error('Preserve earlier control');
const identity=path=>({path,sha256:createHash('sha256').update(readFileSync(path)).digest('hex')});
const modules=new Map(['spectator-timeline.mjs','spectator-tape.mjs'].map(n=>['/app/'+n,resolve(th09,'web/sdl-runtime',n)]));
const evidence={scope:'TH09 fixture, same machine/Edge/CPU requested rates and one-way delay; different game, input pattern and observer. Not a performance A/B for TH11.',
 startedAt:new Date().toISOString(),finishedAt:null,wrapper:identity(fileURLToPath(import.meta.url)),
 supplements:[...modules.values()].map(identity),parameters:Object.fromEntries(Object.entries(process.env).filter(([k])=>/^(CPU_|FRAMES|DELAY_MS|JITTER_MS|TOUCH_|HEADED_CONTROL|IMPAIRMENT_SITE|PC_BUILD|RUN_LABEL|TH09_FIXTURE_ROOT)/.test(k)))};
const save=()=>writeFileSync(resolve(out,'control.json'),JSON.stringify(evidence,null,2)+'\n');save();
const original=http.createServer;
http.createServer=function(options,listener){
 const callback=typeof options==='function'?options:listener;
 const serve=(req,res)=>{const p=new URL(req.url,'http://localhost').pathname,file=modules.get(p);
  if(file){res.setHeader('Content-Type','text/javascript');res.setHeader('Cross-Origin-Resource-Policy','same-origin');res.end(readFileSync(file));return;}
  callback(req,res);
 };
 return typeof options==='function'?original.call(this,serve):original.call(this,options,serve);
};
try{process.argv.push('--release','--relay');await import(pathToFileURL(resolve(th09,'web/tests/multiplayer/smoothness-browser.mjs')));evidence.completed=true;}
catch(e){evidence.error=e.stack||String(e);throw e;}
finally{http.createServer=original;evidence.finishedAt=new Date().toISOString();save();}
