// Public TH15 metadata only; no retail bytes, fonts, credentials or player logs.
import {mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const output=resolve(process.env.TH15_METADATA_OUT||'artifacts/th15-resource-metadata');
mkdirSync(output,{recursive:true});
const report={schema:'th15-cloud-resource-metadata/1',startedAt:new Date().toISOString(),sites:[],requests:[],retailBytesDownloaded:false};
function safeUrl(value,base){const url=new URL(value,base);if(url.protocol!=='https:'||url.username||url.password||!/(^|\.)touhou\.vip$/.test(url.hostname))throw Error('Resource origin not in this read-only probe: '+url.origin);return url;}
async function json(value,base){
 const url=safeUrl(value,base),row={url:url.href};report.requests.push(row);
 const response=await fetch(url,{signal:AbortSignal.timeout(15000),redirect:'error'});row.status=response.status;
 if(!response.ok){await response.body?.cancel();throw Error('HTTP '+response.status+' '+url.href);}
 const reader=response.body.getReader();const chunks=[];let size=0;
 for(;;){const r=await reader.read();if(r.done)break;size+=r.value.length;if(size>1024*1024){await reader.cancel();throw Error('Metadata size limit');}chunks.push(r.value);}
 const bytes=Buffer.concat(chunks);row.bytes=bytes.length;row.sha256=createHash('sha256').update(bytes).digest('hex');
 return {url:url.href,value:JSON.parse(bytes.toString('utf8'))};
}
for(const origin of ['https://touhou.vip/','https://test.touhou.vip/']){
 const site={origin};report.sites.push(site);
 try{
  const m=await json('site-manifest.json',origin);const game=m.value.games?.th15;
  if(!game)throw Error('No TH15 entry');site.game=game;site.shared=m.value.shared??null;
  if(game.package?.descriptor){
   try{const p=await json(game.package.descriptor,origin);site.package={url:p.url,descriptor:p.value};
    site.declaredInputs=Object.entries(p.value.files||{}).filter(([id,f])=>id===p.value.runtimeRequirement?.dataFile||/\.(dat|ttc|otf|ttf|bin)$/i.test(f.target||''))
      .map(([id,f])=>({id,target:f.target,source:f.source,url:new URL(f.source,p.url).href,bytes:f.bytes,sha256:f.sha256}));
   }catch(e){site.packageError=String(e);}
  }
  for(const kind of ['runtime','multiplayerRuntime'])if(typeof game[kind]==='string'){
   try{const p=await json('resources.json',safeUrl(game[kind],origin));site[kind+'Resources']={url:p.url,resources:p.value};}
   catch(e){site[kind+'Error']=String(e);}
  }
  site.status='metadata-read';
 }catch(e){site.status='blocked';site.error=String(e);}
}
report.completedAt=new Date().toISOString();writeFileSync(resolve(output,'report.json'),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify({output,sites:report.sites.map(s=>({origin:s.origin,status:s.status,declaredInputs:s.declaredInputs,error:s.error,packageError:s.packageError}))}));
if(report.sites.every(s=>s.status!=='metadata-read'))process.exitCode=1;
