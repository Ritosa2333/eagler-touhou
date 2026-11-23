import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {stageTextRequests} from '../runtime/managed-stage-text.mjs';
test('co-op dialogue consumes the current World bytes and authored Chinese raster coordinates',()=>{
 const bytes=[0xc4,0xe3,0xba,0xc3],animated={visible:true,alpha:0.8,sx:0.9,sy:1,offsetY:2};
 const state={message:{active:true,x:324,y:264,speaker:0,lines:[bytes],font:6,textPoses:Array(4).fill(animated)},spell:{title:[0xb1,0xf9],pose:{x:498,y:104,sx:1,sy:1,color:0x99ffffff}}};
 const [msg,spell]=stageTextRequests(state);assert.deepEqual([msg.text,msg.kind,msg.rawHex,msg.x,msg.y,msg.align,msg.alpha],['你好','message','c4e3bac3',324,293,'left',0.8]);
 assert.deepEqual([spell.kind,spell.rawHex,spell.x,spell.align],['spell','b1f9',498,'right']);
 assert.deepEqual(stageTextRequests({message:{active:false}}),[]);
});
test('the game canvas owns a distinct non-interactive original text pass',async()=>{
 const source=async name=>readFile(new URL(name,import.meta.url),'utf8');
 const [view,entry,html,styles]=await Promise.all(['../runtime/managed-view.mjs','../site/managed-entry.mjs','../site/th128.html','../site/managed.css'].map(source));
 assert.match(view,/pair\.presentation\(\)/);assert.match(view,/stageText\.paint\(/);assert.match(entry,/stage-text/);assert.match(html,/id="stage-text"[^>]+aria-hidden="true"/);assert.match(styles,/#stage-text\s*\{[^}]*pointer-events:\s*none/);
});
