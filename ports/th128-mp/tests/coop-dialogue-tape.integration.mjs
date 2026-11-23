// Produce a real input tape ending on original stage dialogue for browser pixel QA.
import assert from 'node:assert/strict';import {readFile,writeFile} from 'node:fs/promises';import {createHash} from 'node:crypto';import {join,resolve} from 'node:path';import {pathToFileURL} from 'node:url';
import {openOriginalEndingFixture} from './coop-ending-readback.mjs';import {prepareCoopStage} from '../runtime/preview-world.mjs';import {makeCoopRules} from '../runtime/coop-rules.mjs';import {createConfirmedHistory} from '../runtime/confirmed-history.mjs';import {stageTextRequests} from '../runtime/managed-stage-text.mjs';
import {openTh128Dat} from '../../th128/site/runtime/th128-dat.mjs';import {readZipDirectory,readEntryData} from '../../th128/site/runtime/zip-reader.mjs';
const [pin,wasmRoot,archive,project,out]=process.argv.slice(2),sha=b=>createHash('sha256').update(b).digest('hex');
const fixture=await openOriginalEndingFixture(archive),program=JSON.parse(await readFile(join(project,'multiplayer/runtime/program.json')));
const outer=readZipDirectory(await readFile(archive)).entries,pkg=JSON.parse(new TextDecoder().decode(await readEntryData(outer.find(e=>e.name==='package.json'))));
const identity={schema:'th128-managed-stage/1',program:['core','wasm','type9','coopCore','coopWasm'].map(k=>[k,program.files[k].bytes,program.files[k].sha256]),package:Object.entries(pkg.files).sort(([a],[b])=>a<b?-1:a>b?1:0).map(([k,v])=>[k,v.target,v.bytes,v.sha256])};
const config=JSON.parse(await readFile(pin)),read=async p=>new Uint8Array(await readFile(p)),dat=await read(config.dat),wasm=await read(join(wasmRoot,'coop-core.wasm'));assert.equal(sha(wasm),program.files.coopWasm.sha256);const datOwner=openTh128Dat(dat);
const build={coreSHA256:sha(wasm),resourceSHA256:sha(JSON.stringify(identity))},paths={...config,bullet9:config.type9,bullet32:config.type32,...Object.fromEntries(Object.entries(config.bullets).map(([k,v])=>['bullet'+k,v]))},resources={build,read:k=>k==='wasm'?wasm:read(paths[k]),raw:n=>fixture.raw(n,k=>datOwner.read(k))};
const rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'author-dialogue-pixel-qa',epoch:1,players:2,difficulty:0,route:'A',challengeMode:true,seed:7381,inputDelay:0,rollbackWindow:0,...build});
const tape=await createConfirmedHistory({rules,allowedMask:763}),factory=(await import(pathToFileURL(resolve(wasmRoot,'coop-core.mjs')))).default;
const g=await (await prepareCoopStage({rules,resources,factory,onFactory(){},onWorldStart(){},onTick(){}})).openCoopStage(rules);
let enemyX=0,prev=[0,0];
try{for(let frame=0;frame<10000;frame++){
 const {seats}=g.state(),pose=g.presentation(),visible=stageTextRequests(pose).filter(r=>r.kind==='message'&&r.alpha>0.6);
 if(visible.length){fixture.inspect(pose);tape.seal('left');await writeFile(out,tape.replayBytes());console.log(JSON.stringify({path:out,seek:frame,raw:visible.map(r=>r.rawHex),...build}));break;}
 if(frame%6===0){const enemies=g.stageState().entities.filter(e=>e.active&&!e.root&&!(e.flags&33)&&e.life>0);if(enemies.length)enemyX=enemies.sort((a,b)=>a.life-b.life).at(-1).x;}
 const inputs=seats.map(({rawX},player)=>{const x=rawX/128,held=pose.message?.active?0:512|(enemyX>x+2?128:enemyX<x-2?64:0);return {player,held,pressed:held&~prev[player],released:prev[player]&~held,predicted:false};});prev=inputs.map(e=>e.held);const row={frame,inputs};tape.append(row);g.step(row);
}assert.ok(tape.status().end,'native original dialogue started');}finally{g.close();}
