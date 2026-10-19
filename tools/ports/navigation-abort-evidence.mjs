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

/** An abort can follow complete consumption; require both wire and Runtime FS proof. */
export function completedSharedResourceAbort(failure,{responses=[],consumers=[],requests=[]}={}){
  if(failure?.failure!=='net::ERR_ABORTED'||failure.isNavigationRequest!==false||failure.status!==undefined||
    failure.attribution?.documentKind!=='runtime-document'||!Number.isSafeInteger(failure.attribution?.epoch)||failure.attribution.epoch<=0)return null;
  const matchingRequests=requests.filter(item=>item.url===failure.url);
  const matchingResponses=responses.filter(item=>item.url===failure.url);
  const matchingConsumers=consumers.filter(item=>item.url===failure.url&&item.epoch===failure.attribution.epoch);
  if(matchingRequests.length!==1||matchingResponses.length!==1||matchingConsumers.length!==1||
    matchingRequests[0].attribution?.epoch!==failure.attribution.epoch)return null;
  const response=matchingResponses[0],consumer=matchingConsumers[0],expected=response.expected;
  try{if(new URL(failure.url).pathname!==`/shared${consumer.path}`)return null;}catch{return null;}
  if(response.status!==200||response.contentIdentity!=='passed'||!Number.isSafeInteger(expected?.bytes)||expected.bytes<=0||
    !/^[a-f0-9]{64}$/.test(expected.sha256??'')||response.actual?.bytes!==expected.bytes||response.actual.sha256!==expected.sha256||
    consumer.bytes!==expected.bytes||consumer.sha256!==expected.sha256||consumer.stage!=='installed-after-configure'||
    !['/msgothic.ttc','/unifont.otf'].includes(consumer.path)||
    response.transport?.event!=='loadingFailed'||response.transport.error!=='net::ERR_ABORTED'||response.transport.canceled!==true)return null;
  return{failure,responseRequestId:response.requestId,contentIdentity:response.actual,consumer,
    classification:'complete-response-and-runtime-consumption'};
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
