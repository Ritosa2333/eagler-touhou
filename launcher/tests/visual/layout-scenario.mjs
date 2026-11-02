import {readUiLayout,layoutDifferences,captureSurfaceMotion,surfaceMotionFailures,inspectSurfaceMotion} from '../browser/ui-layout-probe.mjs';

const frame=document.querySelector('#application'),start=document.querySelector('#start'),stop=document.querySelector('#stop'),summary=document.querySelector('#summary'),result=document.querySelector('#layoutResult');
let running=false,request=0,observer,record;
function finish(){
  running=false;cancelAnimationFrame(request);observer?.disconnect();start.disabled=false;stop.disabled=true;
  if(!record)return;
  record.duration=performance.now()-record.started;
  record.final=readUiLayout(frame.contentWindow);
  result.textContent=JSON.stringify(record,null,2);
  summary.textContent=`${record.frames} 帧 | ${record.changes.length} 次布局变化 | ${record.longTasks.length} 次长任务`;
}
start.addEventListener('click',()=>{
  finish();const source=frame.contentWindow;record={started:performance.now(),url:source.location.href,baseline:readUiLayout(source),frames:0,changes:[],longTasks:[],maximum:{}};
  if(source.PerformanceObserver?.supportedEntryTypes.includes('longtask')){
    observer=new source.PerformanceObserver(list=>{for(const entry of list.getEntries())record.longTasks.push({start:entry.startTime,duration:entry.duration});});observer.observe({type:'longtask'});
  }
  running=true;start.disabled=true;stop.disabled=false;summary.textContent='记录中';
  const sample=()=>{
    if(!running)return;
    const current=readUiLayout(source),differences=layoutDifferences(record.baseline,current);
    record.frames++;
    if(differences.length)record.changes.push({frame:record.frames,differences});
    for(const [selector,original]of Object.entries(record.baseline.regions)){
      const next=current.regions[selector];if(!original||!next)continue;
      for(const property of ['x','y','width','height','scrollTop']){const key=selector+':'+property;record.maximum[key]=Math.max(record.maximum[key]??0,Math.abs(original[property]-next[property]));}
    }
    if(record.frames>=1200)finish();else request=requestAnimationFrame(sample);
  };
  request=requestAnimationFrame(sample);
});
stop.addEventListener('click',finish);

document.querySelector('#motion').addEventListener('click',async event=>{
  finish();const button=event.currentTarget,choice=document.querySelector('#motionEnd').value,end=choice.endsWith('closed')?'closed':'open',target=document.querySelector('#motionTarget');
  button.disabled=true;start.disabled=true;target.disabled=true;document.querySelector('#motionEnd').disabled=true;result.textContent='';summary.textContent='等待下一次操作';
  try {
    const trace=await captureSurfaceMotion({selector:target.value,source:frame.contentWindow});
    const errors=[...surfaceMotionFailures(trace,{end,expectMotion:!choice.startsWith('reduced-')}),...inspectSurfaceMotion(frame.contentWindow).errors];
    result.textContent=JSON.stringify({errors,trace},null,2);
    summary.textContent=`${trace.frames.length} 帧 | ${errors.length} 处动效问题`;
  } finally {button.disabled=false;start.disabled=false;target.disabled=false;document.querySelector('#motionEnd').disabled=false;}
});
document.querySelector('#details').addEventListener('click',()=>{result.hidden=!result.hidden;});
frame.addEventListener('load',()=>{if(running)finish();});
