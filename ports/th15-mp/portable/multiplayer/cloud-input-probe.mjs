// Read public metadata only. Never download, publish or attach retail archives,
// fonts, account credentials, Replay files or player inputs in this probe.
import {mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'../../../..');
const out=resolve(root,'artifacts/th15-cloud-input');mkdirSync(out,{recursive:true});
const url='https://touhou.vip/site-manifest.json';
const receipt={schema:'th15-cloud-input-probe/1',url,startedAt:new Date().toISOString(),status:'blocked',retailDataDownloaded:false};
try{
 const response=await fetch(url,{signal:AbortSignal.timeout(30000),redirect:'follow'});
 receipt.httpStatus=response.status;receipt.finalUrl=response.url;
 if(!response.ok)throw Error('Public site metadata HTTP '+response.status);
 const reader=response.body.getReader(),chunks=[];let bytes=0;
 for(;;){const {done,value}=await reader.read();if(done)break;bytes+=value.length;if(bytes>1024*1024){await reader.cancel();throw Error('Public metadata exceeds 1 MiB limit');}chunks.push(value);}
 const body=Buffer.concat(chunks);receipt.bytes=body.length;receipt.sha256=createHash('sha256').update(body).digest('hex');
 const manifest=JSON.parse(body.toString('utf8'));
 if(!manifest.games?.th15)throw Error('Public site manifest has no TH15 entry');
 // Retain public catalog fields needed to locate lawful test inputs, not a
 // generic browser session or any user's local/credential-bearing state.
 writeFileSync(resolve(out,'site-th15.json'),JSON.stringify({schema:manifest.schema,keys:Object.keys(manifest),game:manifest.games.th15,shared:manifest.shared??null,resources:manifest.resources??null},null,2)+'\n');
 receipt.status='metadata-readable';
}catch(error){receipt.error=String(error);process.exitCode=1;}
finally{receipt.completedAt=new Date().toISOString();writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');console.log(JSON.stringify(receipt));}
