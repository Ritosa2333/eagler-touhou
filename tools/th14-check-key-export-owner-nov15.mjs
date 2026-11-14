import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const source=resolve(process.argv[2]??'ports/th14/runtimes/th12/th12_web/cpp/sdl/PracticeUiTh14.cpp');
const output=process.argv[3]?resolve(process.argv[3]):null;
const sha=b=>createHash('sha256').update(b).digest('hex');
const extract=s=>{const start=s.indexOf('void export_keys(){');assert(start>=0);const end=s.indexOf('},csv.c_str());',start);assert(end>start);return s.slice(s.indexOf('EM_ASM({',start)+8,end);};
const before="\n        const url=URL.createObjectURL(new Blob([UTF8ToString($0)],{type:'text/csv;charset=utf-8'}));\n        const a=document.createElement('a');a.href=url;a.download='APS.csv';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);\n    ",after=extract(readFileSync(source,'utf8'));
function run(body,mode,text){
 const trace=[],timers=[];
 function realm(name){return {document:{createElement(tag){assert.equal(tag,'a');const a={click(){trace.push({op:'click',realm:name,href:a.href,download:a.download});}};return a;}},Blob:class {constructor(parts,options){this.parts=parts;this.type=options.type;}},URL:{createObjectURL(blob){trace.push({op:'blob',realm:name,parts:blob.parts,type:blob.type});return 'blob:'+name;},revokeObjectURL(url){trace.push({op:'revoke',realm:name,url});}},setTimeout(fn,ms){assert.equal(ms,1000);timers.push(fn);trace.push({op:'timer',realm:name});}};}
 const own=realm('game'),parent=realm('launcher');own.parent=mode==='standalone'?own:mode==='hosted'?parent:mode==='missing-document'?{document:null}:Object.defineProperty({},'document',{get(){throw Error('cross-origin access denied');}});
 new Function('$0','UTF8ToString','window','document','URL','Blob','setTimeout',body)(1234,p=>{assert.equal(p,1234);return text;},own,own.document,own.URL,own.Blob,own.setTimeout);
 assert.equal(timers.length,1);timers[0]();return trace;
}
let cases=0,rejected=0,controls=0;
for(const mode of ['standalone','hosted','missing-document','cross-origin'])for(const text of ['', 'frame,aps,up,down,left,right,Z,X,C,D,Ctrl,Shift\n1,2,-,-,-,O,O,-,-,-,-,-\n','frame,aps\n1,0\n2,2\n']){
 const realm=mode==='hosted'?'launcher':'game';const expected=[{op:'blob',realm,parts:[text],type:'text/csv;charset=utf-8'},{op:'click',realm,href:'blob:'+realm,download:'APS.csv'},{op:'timer',realm},{op:'revoke',realm,url:'blob:'+realm}];
 const old=run(before,mode,text),now=run(after,mode,text);assert.deepEqual(now,expected);cases++;if(mode==='hosted'){assert.notDeepEqual(old,expected);rejected++;}else{assert.deepEqual(old,now);controls++;}
}
const q={passed:true,cases,oldHostedOwnerRejected:rejected,unchangedStandaloneAndFallbackControls:controls,actualAuthoredExportJs:true,mockedBrowserRealms:true,sourceSha256:sha(readFileSync(source)),csvDataAndMimePreserved:true,actualGameExportAccepted:false,productionChanged:false};if(output)writeFileSync(output,JSON.stringify(q,null,2)+'\n');console.log(JSON.stringify(q));
