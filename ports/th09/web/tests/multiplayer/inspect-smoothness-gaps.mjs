import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const directory=fileURLToPath(new URL('../../artifacts/multiplayer-tests/',import.meta.url));
for(const label of process.argv.slice(2)){
 assert.match(label,/^[\w-]+$/);const report=JSON.parse(readFileSync(resolve(directory,label+'-report.json')));
 assert.ok(report.presentationMeasurement,'Requires renderer-submission RAF timestamps');
 for(let side=0;side<2;++side){
  const rows=report.results[side].measure.samples,events=[];let prior=0;
  for(let i=1;i<rows.length;++i)if(rows[i][11]!==rows[prior][11]){
   const gap=rows[i][13]-rows[prior][13];
   if(gap>25&&rows[prior][3]>=180&&rows[i][3]<report.parameters.limit){
    const window=rows.slice(Math.max(0,prior-1),i+1).map((r,j,a)=>({frame:r[3],ok:r[1],workMs:r[2],presented:r[11],
      relativeRafMs:r[13]-rows[prior][13],newCaptures:j?r[7]-a[j-1][7]:null,newResimulated:j?r[6]-a[j-1][6]:null,
      newCorrections:j?r[5]-a[j-1][5]:null,confirmed:r[4]}));
    events.push({gapMs:gap,window});
   }
   prior=i;
  }
  console.log(JSON.stringify({label,side,count:events.length,worst:events.sort((a,b)=>b.gapMs-a.gapMs).slice(0,8)}));
 }
}
