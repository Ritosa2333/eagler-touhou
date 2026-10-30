// One asynchronous GPU sample per 60 presented frames. Never wait for GPU
// completion, never manufacture a value when the extension is unavailable.
export function gpuTiming(gl){
 const ext=gl?.getExtension('EXT_disjoint_timer_query_webgl2');
 let frame=0,active=null,last=null,latestFrame=0;const pending=[];
 const discard=()=>{for(const q of pending)gl.deleteQuery(q);pending.length=0;last=null;};
 return{
  begin(){
   ++frame;if(!ext||gl.isContextLost())return;
   if(frame%60!==1)return;
   if(gl.getParameter(ext.GPU_DISJOINT_EXT))discard();
   while(pending.length&&gl.getQueryParameter(pending[0],gl.QUERY_RESULT_AVAILABLE)){
    const q=pending.shift(),ns=gl.getQueryParameter(q,gl.QUERY_RESULT);gl.deleteQuery(q);
    if(Number.isFinite(ns)&&ns>=0){last=ns/1e6;latestFrame=frame;}
   }
   // A connected profiler may already own the timer target for this frame.
   if(pending.length>=3||gl.getQuery(ext.TIME_ELAPSED_EXT,gl.CURRENT_QUERY))return;
   active=gl.createQuery();if(active)gl.beginQuery(ext.TIME_ELAPSED_EXT,active);
  },
  end(){if(active){gl.endQuery(ext.TIME_ELAPSED_EXT);pending.push(active);active=null;}},
  value(){return frame-latestFrame<=180?last:null;},
  close(){if(active){gl.endQuery(ext.TIME_ELAPSED_EXT);gl.deleteQuery(active);active=null;}if(ext)discard();}
 };
}
