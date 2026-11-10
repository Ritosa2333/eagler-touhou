import {performance} from 'node:perf_hooks';
import {pathToFileURL} from 'node:url';
import {createRendererFixture,drawRendererWorkload,rendererWorkloads} from '../tests/helpers/renderer-workload.mjs';

// Pass an alternative renderer path to compare revisions with identical work.
const moduleUrl=process.argv[2]?pathToFileURL(process.argv[2]):new URL('../src/renderer.mjs',import.meta.url);
const {Renderer}=await import(moduleUrl);
const frames=120,results=[];
for(const [name,workload]of Object.entries(rendererWorkloads)){
  const fixture=createRendererFixture(Renderer),{renderer,stats}=fixture;
  for(let i=0;i<60;i++)drawRendererWorkload(renderer,workload);
  fixture.reset();
  const times=[];
  for(let i=0;i<frames;i++){
    const start=performance.now();drawRendererWorkload(renderer,workload);times.push(performance.now()-start);
  }
  times.sort((a,b)=>a-b);
  results.push({name,...workload,frames,medianMs:times[Math.floor(frames*.5)],p95Ms:times[Math.floor(frames*.95)],
    drawsPerFrame:stats.draws/frames,uploadBytesPerFrame:stats.uploadBytes/frames,
    parameterCallsPerFrame:stats.parameters/frames,newUploadBuffers:stats.uploadBuffers});
}
console.log(JSON.stringify({runtime:process.version,scope:'JS renderer submission with recorded WebGL boundary',results},null,2));
