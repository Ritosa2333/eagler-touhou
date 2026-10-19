function runtimeInstanceFromUrl(value){
  if(typeof value!=='string'||!value)return null;
  try{
    const url=new URL(value,'http://runtime-browser.invalid');
    let raw=null,kind=null;
    if(url.pathname.startsWith('/host/')){raw=url.searchParams.get('epoch');kind='host-document';}
    else if(url.pathname.startsWith('/runtime/')){raw=url.searchParams.get('runtimeEpoch');kind='runtime-document';}
    if(!raw||!/^\d+$/.test(raw))return null;
    const epoch=Number(raw);
    return Number.isSafeInteger(epoch)&&epoch>0?{epoch,kind,url:value}:null;
  }catch{return null;}
}

function requestUrl(request){try{return request.url();}catch{return null;}}
function navigationRequest(request){try{return request.isNavigationRequest()===true;}catch{return false;}}
function requestFrame(request){try{return request.frame();}catch{return null;}}
function frameUrl(frame){try{return frame.url();}catch{return null;}}
function parentFrame(frame){try{return frame.parentFrame();}catch{return null;}}

/** Attribute a request to the host/runtime instance that owned it when it started. */
export function attributeRuntimeRequest(request){
  const url=requestUrl(request);
  if(navigationRequest(request)){
    const document=runtimeInstanceFromUrl(url);
    if(document)return{epoch:document.epoch,source:'request-document',documentKind:document.kind,frameUrl:null};
  }
  const seen=new Set();
  for(let frame=requestFrame(request);frame&&!seen.has(frame);frame=parentFrame(frame)){
    seen.add(frame);
    const currentUrl=frameUrl(frame),instance=runtimeInstanceFromUrl(currentUrl);
    if(instance)return{epoch:instance.epoch,source:'frame-or-ancestor',documentKind:instance.kind,frameUrl:currentUrl};
  }
  return{epoch:null,source:'unknown',documentKind:null,frameUrl:null};
}

/** Only an exact browser navigation abort owned by an explicitly retired epoch is expected. */
export function classifyRequestFailure({failure,attribution,retiringEpochs}){
  const errorText=typeof failure==='string'?failure:failure?.errorText;
  const epoch=attribution?.epoch;
  const expectedNavigationAbort=errorText==='net::ERR_ABORTED'&&Number.isSafeInteger(epoch)&&retiringEpochs.has(epoch);
  return{errorText:errorText??null,expectedNavigationAbort};
}

export function shouldRecordHttpFailure(status){
  return Number.isFinite(status)&&status>=400;
}

export function createNavigationAbortEvidence(){
  const started=new WeakMap(),retiringEpochs=new Set();
  return{
    captureRequest(request){const attribution=attributeRuntimeRequest(request);started.set(request,attribution);return attribution;},
    retirePage(page){
      let url=null;
      try{url=page.mainFrame().url();}catch{}
      const instance=runtimeInstanceFromUrl(url);
      if(!instance)return null;
      retiringEpochs.add(instance.epoch);
      return{epoch:instance.epoch,source:instance.kind,url};
    },
    requestFailure(request){
      const attribution=started.get(request)??{epoch:null,source:'unknown',documentKind:null,frameUrl:null};
      let failure=null;
      try{failure=request.failure();}catch{}
      return{attribution,...classifyRequestFailure({failure,attribution,retiringEpochs})};
    },
    get retiringEpochs(){return new Set(retiringEpochs);}
  };
}
