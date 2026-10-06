import fs from 'node:fs';import path from 'node:path';
const CATEGORIES=['input','player','rng','ecl','enemies','bullets','items','economy','stage'];
export function validateTrace(trace){
  if(trace?.schema!=='th12-gameplay-trace/1')throw Error('Incompatible state schema');
  for(const key of ['replaySha256','executableSha256','resourceSha256','stateSchemaSha256'])if(!/^[0-9a-f]{64}$/.test(trace.identity?.[key]||''))throw Error('Missing identity '+key);
  if(!Array.isArray(trace.ticks)||!trace.ticks.length||trace.completion?.complete!==true||trace.completion?.ticks!==trace.ticks.length)throw Error('Incomplete trace');
  for(let i=0;i<trace.ticks.length;i++){const tick=trace.ticks[i];if(tick.frame!==i)throw Error('Missing, shifted or reordered tick '+i);for(const category of CATEGORIES)if(!(category in tick))throw Error('Missing authoritative category '+category);}
}
function firstDifference(a,b,path='state'){
  if(typeof a!==typeof b||a===null||b===null)return a===b?null:{path,expected:a,actual:b};
  if(typeof a==='number'){if(!Number.isFinite(a)||!Number.isFinite(b))return {path,expected:a,actual:b};return Object.is(a,b)?null:{path,expected:a,actual:b};}
  if(typeof a!=='object')return a===b?null:{path,expected:a,actual:b};
  if(Array.isArray(a)!==Array.isArray(b))return {path,expected:a,actual:b};
  const ak=Object.keys(a).sort(),bk=Object.keys(b).sort();if(JSON.stringify(ak)!==JSON.stringify(bk))return {path,expectedKeys:ak,actualKeys:bk};
  for(const key of ak){const difference=firstDifference(a[key],b[key],path+'.'+key);if(difference)return difference;}return null;
}
export function compareTraces(expected,actual){
  validateTrace(expected);validateTrace(actual);
  if(firstDifference(expected.identity,actual.identity,'identity'))throw Error('Identity-incompatible traces');
  if(expected.ticks.length!==actual.ticks.length)throw Error('Different trace lengths');
  for(let i=0;i<expected.ticks.length;i++){const difference=firstDifference(expected.ticks[i],actual.ticks[i]);if(difference)return {status:'diverged',frame:i,...difference};}
  return {status:'pass',ticks:expected.ticks.length};
}
if(process.argv[1]&&path.resolve(process.argv[1])===path.resolve(import.meta.filename)){
  const [expected,actual]=process.argv.slice(2);if(!expected||!actual)throw Error('Provide expected and candidate trace files');
  const report=compareTraces(JSON.parse(fs.readFileSync(expected)),JSON.parse(fs.readFileSync(actual)));console.log(JSON.stringify(report,null,2));if(report.status!=='pass')process.exitCode=1;
}
