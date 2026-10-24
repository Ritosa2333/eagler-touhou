const TICK_MS=1000/60;
const percentile=(values,fraction)=>{
  const sorted=[...values].sort((a,b)=>a-b);
  return sorted[Math.min(sorted.length-1,Math.floor(sorted.length*fraction))]??null;
};

// Samples describe actual presented callbacks, not a bulk simulation loop.
export function summarizeFramePacing(samples,{refreshIntervalMs=TICK_MS}={}){
  if(samples.length<2)throw Error('At least two presented frames are required');
  if(samples.some((sample,index)=>!Number.isFinite(sample.time)||!Number.isSafeInteger(sample.ticks)||sample.ticks<0
    ||index>0&&sample.ticks<samples[index-1].ticks||!Number.isFinite(sample.processingMs)||sample.processingMs<0))throw Error('Invalid frame sample');
  const first=samples[0],last=samples.at(-1),elapsedMs=last.time-first.time;
  if(elapsedMs<=0)throw Error('Presentation timestamps must advance');
  const intervals=samples.slice(1).map((sample,index)=>sample.time-samples[index].time);
  if(intervals.some(interval=>!Number.isFinite(interval)||interval<=0))throw Error('Invalid presentation timestamps');
  const processing=samples.slice(1).map(sample=>sample.processingMs);
  const ticks=last.ticks-first.ticks,seconds=elapsedMs/1000;
  if(!Number.isFinite(refreshIntervalMs)||refreshIntervalMs<=0)throw Error('Invalid refresh interval');
  const intervalBudgetMs=Math.ceil((TICK_MS-.5)/refreshIntervalMs)*refreshIntervalMs+.8;
  const report={elapsedMs,refreshIntervalMs,intervalBudgetMs,presents:samples.length-1,logicTicks:ticks,
    presentationFps:(samples.length-1)/seconds,logicHz:ticks/seconds,
    intervalP95Ms:percentile(intervals,.95),maxIntervalMs:Math.max(...intervals),
    processingP95Ms:percentile(processing,.95),processingMaxMs:Math.max(...processing),
    overBudgetFrames:processing.filter(ms=>ms>TICK_MS).length,
    missed60HzSlots:intervals.reduce((sum,ms)=>sum+Math.max(0,Math.round(ms/TICK_MS)-1),0)};
  report.meets60Hz=report.presentationFps>=59.5&&report.logicHz>=59.5&&report.logicHz<=60.5
    &&report.intervalP95Ms<=intervalBudgetMs&&report.processingP95Ms<=TICK_MS;
  return report;
}
