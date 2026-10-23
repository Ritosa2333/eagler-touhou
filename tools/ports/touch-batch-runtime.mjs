/** Runs inside the Runtime realm; send still travels through the host RPC. */
export async function runTouchBatch({operations,clockRequest,actionRequest,sceneRequest,observationRequest},send,readers){
  if(!Array.isArray(operations)||operations.length<1||operations.length>32||typeof send!=='function')
    throw Error('Touch batch requires a bounded ordered operation list and the host transport');
  const canvas=document.querySelector('canvas');
  if(!canvas)return{available:false,reason:'The actual Runtime canvas is unavailable'};
  const rect=canvas.getBoundingClientRect();
  if(!(rect.width>0&&rect.height>0&&innerWidth>0&&innerHeight>0))
    return{available:false,reason:'The actual Runtime canvas has no input rectangle'};
  const {controlTouchMotionClock,readTouchActionState,controlValidationScene,readRuntimeObservation}=readers;
  const result={available:true,acknowledged:true,events:[],checkpoints:[],
    worldBefore:controlValidationScene(sceneRequest),worldReceipts:[]};
  for(const [operationIndex,operation] of operations.entries()){
    if(operation.type==='checkpoint'){
      if(!Number.isInteger(operation.ticks)||operation.ticks<1||operation.ticks>8||typeof operation.label!=='string')
        throw Error('A touch checkpoint requires a label and one to eight native ticks');
      const samples=[];
      for(let n=0;n<operation.ticks;n++){
        controlTouchMotionClock({...clockRequest,action:'tick'});
        result.worldReceipts.push(controlValidationScene(sceneRequest));
        samples.push({action:readTouchActionState(actionRequest),observation:readRuntimeObservation(observationRequest)});
      }
      result.checkpoints.push({label:operation.label,samples});
    }else{
      if(!['down','move','up'].includes(operation.type)||!Number.isSafeInteger(operation.id)||
        ![operation.x,operation.y].every(v=>Number.isFinite(v)&&v>=0&&v<=1))
        throw Error('Invalid native touch operation');
      const startedAtMs=performance.now();
      await send({type:operation.type,id:operation.id,x:(rect.left+operation.x*rect.width)/innerWidth,
        y:(rect.top+operation.y*rect.height)/innerHeight});
      result.events.push({operationIndex,type:operation.type,id:operation.id,startedAtMs,
        completedAtMs:performance.now(),acknowledged:true});
    }
  }
  return result;
}
