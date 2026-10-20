// Adapt the original IaMP English v1.1 data archive, never its DLL/EXE hooks.
import assert from 'node:assert/strict';
import {cryptTh075,th075QuotedRecords} from './th075-thcrap-compiler.mjs';

export function englishScriptQuotes(original,translated){
 const group=rows=>{const groups=[];for(const row of rows){const last=groups.at(-1);if(last?.label===row.label&&last.command===row.command)last.rows.push(row);else groups.push({label:row.label,command:row.command,rows:[row]});}return groups;};
 const a=group(th075QuotedRecords(original)),b=group(th075QuotedRecords(cryptTh075(translated)));
 assert.equal(a.length,b.length,'English script speaker groups differ');
 const quotes={};let reflowed=0;
 const clean=bytes=>bytes.toString('latin1').replace(/\x81@/g,' ').replace(/\\c[0-9a-f]{6}|\\e/gi,'').replace(/\\n/g,'\n').replace(/ {3,}/g,'\n').trim();
 for(let i=0;i<a.length;i++){
  const left=a[i],right=b[i];assert.equal(left.label,right.label,'English label mismatch');assert.equal(left.command,right.command,'English speaker mismatch');
  let texts=right.rows.map(r=>clean(r.text));
  if(texts.length!==left.rows.length){
   // Original English endings change R pagination and N page counts. Keep
   // Japanese control flow and distribute all English words over its pages.
   const words=texts.join(' ').split(/\s+/),pages=[];let at=0;
   for(let page=0;page<left.rows.length;page++){
    const remain=words.slice(at).join(' ').length,target=Math.ceil(remain/(left.rows.length-page));let text='';
    while(at<words.length&&(text.length<target||page===left.rows.length-1)){text+=(text?' ':'')+words[at++];}
    pages.push(text);
   }
   assert.equal(at,words.length);texts=pages;reflowed++;
  }
  left.rows.forEach((row,j)=>{assert.ok(!/[\x00-\x08\x0b-\x1f\x80-\xff]/.test(texts[j]),'Unexpected English text encoding');quotes[row.quote]=texts[j];});
 }
 return {quotes,reflowed};
}

export async function englishStaticResources(files,base){
 const resources=[],pictures=new Map(Object.keys(files).filter(n=>n.endsWith('.png')).map(n=>[n,n]));
 for(const line of Buffer.from(files['pack.txt']).toString().split(/\r?\n/)){
  const match=/^alias\s+(\S+)\s+(\S+)/.exec(line);if(match){assert.ok(files[match[2]],'Missing English alias');pictures.set(match[1],match[2]);}
 }
 for(const [name,target] of pictures){const m=/^data_(.+)-(\d+)\.png$/.exec(name);if(!m)continue;resources.push({game:'th075',path:'th075/'+m[1].replaceAll('_','/')+'/'+Number(m[2])+'.png',bytes:Buffer.from(files[target])});}
 let reflowed=0;
 for(const [name,bytes] of Object.entries(files))if(name.endsWith('.sce')){
  assert.match(name,/^data_character_[a-z]+_[a-z]+\.sce$/);
  const path=name.replaceAll('_','/'),result=englishScriptQuotes(await base.read(path),bytes);reflowed+=result.reflowed;
  resources.push({game:'th075',path:'th075/'+path.slice(5)+'.quotes.json',bytes:Buffer.from(JSON.stringify(result.quotes))});
 }
 return {resources,reflowed};
}
