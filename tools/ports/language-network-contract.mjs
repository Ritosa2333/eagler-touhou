import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';

const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
/** A cold context receives one exact unavailable or byte-corrupt published archive. */
export function languageFailureResponse(pack,{kind}={}){
  assert.ok(['network-failure','corrupt-archive'].includes(kind),'Unknown language failure control');
  const original=Buffer.from(pack);
  assert.ok(original.length>8,'Language failure control needs a real archive');
  const body=kind==='network-failure'?Buffer.from('Language acceptance: archive unavailable'):Buffer.from(original);
  if(kind==='corrupt-archive')body[Math.floor(body.length/2)]^=1;
  assert.notEqual(hash(body),hash(original),'Corrupt language control must change the published identity');
  return{kind:'controlled-resource-response',purpose:`language-${kind}`,status:kind==='network-failure'?404:200,
    contentType:kind==='network-failure'?'text/plain; charset=utf-8':'application/zip',body,bytes:body.length,sha256:hash(body)};
}

/** Build an exact source URL only when the title declaration belongs to both published asset inventories. */
export function resolveLanguageRuntimeSource({browserDiagnostics,runtimeGroup,productRuntimeAssets,base}={}){
  const asset=browserDiagnostics?.runtimeAsset,root=runtimeGroup?.root,generation=runtimeGroup?.current?.generation;
  const safeRelative=(value,{directory=false}={})=>{
    if(typeof value!=='string'||value.length===0||value.startsWith('/')||value.includes('\\')||value.includes(':'))return false;
    const parts=value.split('/');if(directory&&parts.at(-1)==='')parts.pop();
    return parts.length>0&&parts.every(part=>part&&part!=='.'&&part!=='..');
  };
  if(!safeRelative(asset)||!safeRelative(root,{directory:true})||!root.endsWith('/')||!/^([a-f\d]{64})$/i.test(generation??'')||
    !Array.isArray(productRuntimeAssets)||!productRuntimeAssets.includes(asset)||
    !Array.isArray(runtimeGroup?.current?.files)||!runtimeGroup.current.files.some(file=>file?.path===asset)||
    typeof base!=='string')return null;
  try{return new URL(`/${root}${generation}/${asset}`,base).href;}catch{return null;}
}

/** Expected transport errors require a matching verified response, exact URL and browser message. */
export function classifyLanguageBrowserEvents(events,{responses=[],runtimeSource=null,fontPaths=[],classifyDiagnostic}={}){
  const expected=[],blocking=[];
  for(const event of events){
    if(event.type==='console'&&runtimeSource&&event.location?.url===runtimeSource&&typeof classifyDiagnostic==='function'){
      let diagnostic=null;
      try{diagnostic=classifyDiagnostic({text:event.text,fontPaths});}catch{}
      if(diagnostic&&typeof diagnostic==='object'&&!Array.isArray(diagnostic)&&
        typeof diagnostic.classification==='string'&&diagnostic.classification.trim()&&
        ['expected','blocking'].includes(diagnostic.disposition)){
        const classified={...event,classification:'title-language-diagnostic',titleDiagnostic:diagnostic};
        (diagnostic.disposition==='expected'?expected:blocking).push(classified);continue;
      }
    }
    const response=responses.find(response=>response.url===(event.url??event.location?.url)&&
      response.status===404&&response.contentIdentity==='passed'&&
      ['controlled-resource-response','static-resource-route-discovery'].includes(response.semanticReceipt?.kind));
    const known404=event.type==='http'&&event.status===404||event.type==='console'&&[
      'Failed to load resource: the server responded with a status of 404 (Not Found)',
      'Failed to load resource: the server responded with a status of 404 ()',
    ].includes(event.text)||event.type==='requestfailed'&&event.error==='net::ERR_ABORTED'&&
      response?.semanticReceipt?.bodyConsumption==='not-read';
    (response&&known404?expected:blocking).push(event);
  }
  return{expected,blocking};
}
