import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
const old="                const blob=new Blob([HEAPU8.slice($0,$0+$1)],{type:'application/octet-stream'});\n                const link=document.createElement('a');link.href=URL.createObjectURL(blob);link.download='th14-recovered.rpy';\n                link.click();setTimeout(()=>URL.revokeObjectURL(link.href),1000);";
const source=readFileSync(process.argv[2]??'ports/th14/runtimes/th12/th12_web/cpp/sdl/PracticeAdvancedPurpleTh14.inc','utf8');
const at=source.indexOf('append_purple_laser_block(result,laser.recovered);');assert(at>=0);
const body=source.slice(source.indexOf('EM_ASM({',at)+8,source.indexOf('},result.data(),result.size());',at));
function run(body,mode,offset,size){
 const heap=Uint8Array.from({length:512},(_,i)=>(i*73+21)%256),original=heap.slice(),trace=[],timers=[];
 function realm(name){return {document:{createElement(tag){assert.equal(tag,'a');const a={click(){trace.push(['click',name,a.href,a.download]);}};return a;}},Blob:class{constructor(parts,opts){this.bytes=Array.from(parts[0]);this.type=opts.type;}},URL:{createObjectURL(blob){trace.push(['blob',name,blob.bytes,blob.type]);return 'blob:'+name;},revokeObjectURL(url){trace.push(['revoke',name,url]);}},setTimeout(fn,ms){assert.equal(ms,1000);trace.push(['timer',name]);timers.push(fn);}};}
 const game=realm('game'),parent=realm('launcher');game.parent=mode==='standalone'?game:mode==='hosted'?parent:mode==='missing-document'?{document:null}:Object.defineProperty({},'document',{get(){throw Error('cross-origin');}});
 new Function('$0','$1','HEAPU8','window','document','URL','Blob','setTimeout',body)(offset,size,heap,game,game.document,game.URL,game.Blob,game.setTimeout);
 assert.deepEqual(heap,original);assert.equal(timers.length,1);timers[0]();return {trace,bytes:Array.from(heap.slice(offset,offset+size))};
}
let cases=0,negatives=0,controls=0;
for(const mode of ['hosted','standalone','cross-origin','missing-document'])for(const [offset,size] of [[0,0],[1,1],[7,233],[256,256]]){
 const before=run(old,mode,offset,size),after=run(body,mode,offset,size),owner=mode==='hosted'?'launcher':'game';
 const expected=[['blob',owner,after.bytes,'application/octet-stream'],['click',owner,'blob:'+owner,'th14-recovered.rpy'],['timer',owner],['revoke',owner,'blob:'+owner]];
 assert.deepEqual(after.trace,expected);cases++;if(mode==='hosted'){assert.notDeepEqual(before.trace,expected);negatives++;}else{assert.deepEqual(before.trace,after.trace);controls++;}
}
const q={passed:true,cases,oldHostedOwnerRejected:negatives,unchangedFallbackControls:controls,actualAuthoredJsExtracted:true,binarySlicesAndInputHeapPreserved:true,mockedBrowserRealms:true,actualRecoveredReplayDownloadAccepted:false,productionChanged:false};if(process.argv[3])writeFileSync(process.argv[3],JSON.stringify(q,null,2)+'\n');console.log(JSON.stringify(q));
