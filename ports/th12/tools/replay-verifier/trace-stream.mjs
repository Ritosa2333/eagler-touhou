import fs from 'node:fs';
import path from 'node:path';
import readline from 'node:readline';
import {once} from 'node:events';
import crypto from 'node:crypto';
import {assertHash} from './config.mjs';
export const categories=['input','player','rng','ecl','enemies','bullets','items','economy','stage'];
export function finiteTree(value,label='state'){
  if(value===null||typeof value==='string'||typeof value==='boolean')return;
  if(typeof value==='number'){if(!Number.isFinite(value))throw Error('Non-finite '+label);return;}
  if(!value||typeof value!=='object')throw Error('Invalid '+label);
  for(const [key,child] of Object.entries(value))finiteTree(child,label+'.'+key);
}
export function difference(expected,actual,label='state'){
  if(Object.is(expected,actual))return null;
  if(!expected||!actual||typeof expected!=='object'||typeof actual!=='object')return {path:label,expected,actual};
  if(Array.isArray(expected)!==Array.isArray(actual))return {path:label,expected,actual};
  const a=Object.keys(expected).sort(),b=Object.keys(actual).sort();
  if(JSON.stringify(a)!==JSON.stringify(b))return {path:label,expectedKeys:a,actualKeys:b};
  for(const key of a){const result=difference(expected[key],actual[key],label+'.'+key);if(result)return result;}
  return null;
}
export async function fileIdentity(file){
  let bytes=0;const hash=crypto.createHash('sha256');
  for await(const chunk of fs.createReadStream(file)){bytes+=chunk.length;hash.update(chunk);}
  return {sha256:hash.digest('hex'),bytes};
}
export async function* records(file){
  const input=fs.createReadStream(file),lines=readline.createInterface({input,crlfDelay:Infinity});
  try{for await(const line of lines){if(!line.trim())throw Error('Empty trace record');yield JSON.parse(line);}}
  finally{lines.close();input.destroy();}
}
export async function writeRecords(file,source){
  fs.mkdirSync(path.dirname(file),{recursive:true});const output=fs.createWriteStream(file,{flags:'wx'});
  let failure;output.on('error',error=>{failure=error;});
  try{for await(const record of source){if(failure)throw failure;const line=JSON.stringify(record)+'\n';if(!output.write(line))await once(output,'drain');}output.end();await once(output,'finish');}
  catch(error){output.destroy();throw error;}
}
function headerRecord(header){
  if(header?.record!=='header'||header.schema!=='th12-gameplay-trace/2')throw Error('Unsupported gameplay trace header');
  for(const key of ['replaySha256','executableSha256','resourceSha256','stateSchemaSha256'])assertHash(header.identity?.[key],key);
  const f=header.fixture;if(!f||typeof f.id!=='string'||!Array.isArray(f.route)||!f.route.length)throw Error('Missing complete trace route');
  const numbers=new Set();
  for(const s of f.route){
    if(!Number.isSafeInteger(s.number)||s.number<1||s.number>7||numbers.has(s.number)||!Number.isSafeInteger(s.inputTicks)||s.inputTicks<1||!Number.isSafeInteger(s.frames)||s.frames!==s.inputTicks+(s.sentinel?1:0)||typeof s.sentinel!=='boolean')throw Error('Invalid trace stage bounds');
    numbers.add(s.number);
  }
  finiteTree(header);
}
export async function validateGameplayFile(file,{identity,fixture,completion,original=false}={}){
  let header,footer,ticks=0,routeIndex=0,inputFrame=0;
  for await(const row of records(file)){
    if(!header){header=row;headerRecord(header);if(original&&(header.provider?.kind!=='original-full-state'||header.provider?.sampling!=='after-every-complete-authoritative-tick-before-external-stage-load'||!Array.isArray(header.provider?.coverage)||!['input','player','rng','ecl','enemies','bullets','items','economy','stage','animations'].every(k=>header.provider.coverage.includes(k))))throw Error('Golden is not a full-state original capture with complete provider evidence');
      if(identity&&difference(identity,header.identity))throw Error('Golden identity mismatch');
      if(fixture&&difference(fixture,header.fixture))throw Error('Golden fixture/route mismatch');continue;}
    if(footer)throw Error('Data follows trace completion');
    if(row.record==='completion'){footer=row;continue;}
    const route=header.fixture.route[routeIndex];
    if(row.record!=='tick'||!route||row.frame!==ticks||row.stageNumber!==route.number||row.inputFrame!==inputFrame||!Number.isSafeInteger(row.logicFrame)||row.logicFrame<0)throw Error('Missing, shifted, reordered or extra trace tick '+ticks);
    for(const category of categories)if(!Object.hasOwn(row,category))throw Error('Missing authoritative category '+category);
    for(const field of ['held','pressed','released'])if(!Number.isInteger(row.input?.recorded?.[field])||row.input.recorded[field]<0||row.input.recorded[field]>65535)throw Error('Missing recorded input word '+field);
    for(const category of ['ecl','enemies','bullets','items'])if(!Array.isArray(row[category]))throw Error('Incomplete authoritative collection '+category);
    for(const category of ['player','rng','economy'])if(!row[category]||typeof row[category]!=='object'||Array.isArray(row[category]))throw Error('Incomplete authoritative object '+category);
    if(!row.stage?.state||!row.stage?.flow||!row.stage?.other?.animations||!row.stage?.other?.background)throw Error('Missing full stage/animation/background state');
    finiteTree(row);ticks++;inputFrame++;
    if(inputFrame===route.inputTicks){routeIndex++;inputFrame=0;}
  }
  if(!header||!footer||footer.complete!==true||footer.ticks!==ticks||routeIndex!==header.fixture.route.length||inputFrame!==0)throw Error('Incomplete/cropped gameplay trace');
  if(!Array.isArray(footer.stages)||footer.stages.length!==header.fixture.route.length)throw Error('Missing stage completion evidence');
  for(let i=0;i<footer.stages.length;i++){const stage=footer.stages[i],route=header.fixture.route[i];if(stage.number!==route.number||stage.inputTicks!==route.inputTicks)throw Error('Stage completion route mismatch');}
  finiteTree(footer);if(completion&&difference(completion,footer))throw Error('Corpus completion rules do not match trace footer');
  return {header,footer};
}
export async function compareGameplayFiles(expectedFile,actualFile){
  const expected=await validateGameplayFile(expectedFile,{original:true});
  await validateGameplayFile(actualFile,{identity:expected.header.identity,fixture:expected.header.fixture});
  const a=records(expectedFile)[Symbol.asyncIterator](),b=records(actualFile)[Symbol.asyncIterator]();
  try{
    for(;;){const [left,right]=await Promise.all([a.next(),b.next()]);if(left.done||right.done){if(left.done!==right.done)throw Error('Different complete trace lengths');break;}
      if(left.value.record==='header')continue;const found=difference(left.value,right.value);if(found)return {status:'diverged',frame:left.value.frame??expected.footer.ticks,...found};}
    return {status:'pass',ticks:expected.footer.ticks};
  }finally{await a.return?.();await b.return?.();}
}
