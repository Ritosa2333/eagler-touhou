// Import existing Chinese dialogue, never ship its retail script/control bytes.
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {inflateSync} from 'node:zlib';
import {th075QuotedRecords} from './th075-thcrap-compiler.mjs';

export const TH075_STORY_CHARACTERS=['alice','marisa','meiling','patchouli','reimu','remilia','sakuya','suika','youmu','yukari','yuyuko'];
export function chineseScriptQuotes(original,translated){
  const left=th075QuotedRecords(original),right=th075QuotedRecords(translated),quotes={};
  assert.equal(left.length,right.length,'Chinese script quote count mismatch');
  for(let i=0;i<left.length;i++){
    const a=left[i],b=right[i];
    assert.deepEqual([a.label,a.record,a.command],[b.label,b.record,b.command],'Chinese script control position mismatch');
    // Keep the independently maintained thpatch ending pages.
    if(/^\\c[0-9a-f]{6}/i.test(a.text.toString('latin1')))continue;
    let text=new TextDecoder('gbk',{fatal:true}).decode(b.text);
    assert.ok(text.startsWith('\x01')&&text.endsWith('\x02'),'Missing Chinese glyph markers');
    text=text.slice(1,-1).replaceAll('\\n','\n');
    assert.ok(!/[\x00-\x08\x0b-\x1f\\]/.test(text),'Unexpected Chinese script control');
    quotes[a.quote]=text;
  }
  return quotes;
}
export async function chineseStoryInputs(root){
  const sources=[];
  for(const character of TH075_STORY_CHARACTERS){
    const path=`data/character/${character}/${character}.sce`,bytes=await readFile(resolve(root,path+'.z'));
    assert.ok(bytes.length>4,'Truncated Chinese script');const script=inflateSync(bytes.subarray(4),{maxOutputLength:4*1024*1024});
    assert.equal(script.length,bytes.readUInt32LE(0),'Chinese script size mismatch');
    sources.push({path,script,sha256:createHash('sha256').update(bytes).digest('hex')});
  }
  return sources;
}
export async function chineseStaticResources(sources,base,simplify){
  const tables=[];
  for(const source of sources)tables.push(chineseScriptQuotes(await base.read(source.path),source.script));
  const simplified=await simplify(tables);
  assert.equal(simplified.length,tables.length,'Converted Chinese script count mismatch');
  const resources=sources.map((source,i)=>{
    assert.deepEqual(Object.keys(simplified[i]),Object.keys(tables[i]),'Converted quote positions differ');
    for(const value of Object.values(simplified[i]))assert.ok(typeof value==='string'&&!/[\x00-\x08\x0b-\x1f\\]/.test(value),'Invalid converted dialogue');
    return {game:'th075',path:'th075/'+source.path.slice(5)+'.quotes.json',bytes:Buffer.from(JSON.stringify(simplified[i]))};
  });
  return {resources,provenance:{source:'existing Chinese development scripts',conversion:'OpenCC t2s',scripts:sources.map(({path,sha256})=>({path,sha256})),quotes:tables.reduce((n,t)=>n+Object.keys(t).length,0)}};
}
