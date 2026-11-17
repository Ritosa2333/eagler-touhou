import create from '/module.mjs';
import {installGpuDiagnostics} from '/gpu-diagnostics.mjs';
const $=s=>document.querySelector(s),canvas=$('#canvas');
let core,running=false,previous=0,debt=0,diagnostics,ticks=0;
const fail=e=>{running=false;$('#status').textContent='错误：'+(e?.message||e);};
const error=()=>{const p=core._application_error();return new TextDecoder().decode(core.HEAPU8.subarray(p,core.HEAPU8.indexOf(0,p)));};
$('#restart').onclick=()=>location.reload();
try{
 const identity=await(await fetch('/identity.json')).json();
 core=await create({canvas,eaglerOptions:{thpracEnabled:true,thpracLocale:'en'}});
 diagnostics=installGpuDiagnostics(core,canvas,{wasm:identity.build.sha256,builtAt:identity.build.builtAt,common:identity.build.architecture.eaglerCommon});
 core.FS.mkdir('/fonts');core.FS.mkdir('/save');
 for(const file of ['th15.dat','unifont.otf','msgothic.ttc',...identity.fontNames.map(n=>'fonts/'+n)]){
  const response=await fetch('/'+file);if(!response.ok)throw Error('资源加载失败：'+file);
  core.FS.writeFile('/'+file,new Uint8Array(await response.arrayBuffer()));
 }
 $('#start').disabled=false;$('#status').textContent='准备完成；每组运行请保持相同浏览器设置、关卡和时长。';
 $('#start').onclick=()=>{try{
  const mode=$('#profile').value,stage=Number($('#stage').value);
  diagnostics.configure({copies:mode!=='baseline',flush:mode==='flush',resetState:mode==='state'});
  core._application_test_music_enabled(0);core._application_render_scale(2);core._application_test_presentation_enable(0);
  if(!core._application_initialize(0)||!core._application_practice_initialize()||!core._application_test_capture_stage(stage))throw Error(error());
  diagnostics.mark('start stage '+stage+' profile '+mode);
  for(const id of ['start','profile','stage'])$('#'+id).disabled=true;
  for(const id of ['chapter','mark','report'])$('#'+id).disabled=false;
  running=true;previous=performance.now();requestAnimationFrame(frame);
 }catch(e){fail(e);}};
 $('#chapter').onclick=()=>{core._application_test_chapter_reward();diagnostics.mark('manual chapter finish');};
 $('#mark').onclick=()=>{diagnostics.mark('visible trails at logical tick '+ticks);$('#status').textContent='已标记重影：'+ticks+' ticks';};
 $('#report').onclick=()=>{
  const url=URL.createObjectURL(new Blob([JSON.stringify(diagnostics.report(),null,2)],{type:'application/json'}));
  const a=document.createElement('a');a.href=url;a.download='th15-gpu-'+Date.now()+'.json';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);
 };
}catch(e){fail(e);}
function frame(now){
 if(!running)return;
 try{
  if(document.hidden){previous=now;debt=0;requestAnimationFrame(frame);return;}
  debt+=Math.min(100,now-previous);previous=now;let batch=0;
  while(debt>=1000/60&&batch++<6){core._application_test_protection(60000);if(!core._application_step(9,0,0,60,0))throw Error(error());core._application_practice_frame();debt-=1000/60;++ticks;}
  if(ticks%60<6)$('#status').textContent='运行中：'+(ticks/60).toFixed(1)+' 逻辑秒；出现重影时点击标记，再下载报告。';
  requestAnimationFrame(frame);
 }catch(e){fail(e);}
}
