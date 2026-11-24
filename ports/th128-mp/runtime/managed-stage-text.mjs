import {drawOriginalStageText,loadOriginalStageTextBank,messageTextLayout} from '../../th128/site/runtime/draw.mjs';
import {safeAssetUrl} from '../../th128/site/inspector.mjs';

export function stageTextRequests({message,spell}={}){
  const out=messageTextLayout(message,'zh',{nativeRasters:true}).map(line=>({...line,kind:line.rasterKind}));
  if(spell?.pose&&spell.title?.length){
    const {x,y,sx,sy,color}=spell.pose,alpha=(color>>>24)/255;
    const rawHex=spell.title.map(n=>n.toString(16).padStart(2,'0')).join('');
    if(alpha)out.push({kind:'spell',rawHex,x,y,sx,sy,alpha,outlineAlpha:alpha,align:'right'});
  }
  return out;
}

export function createManagedStageText({canvas,resources,signal}){
  const context=canvas.getContext('2d');if(!context)throw Error('Original TH128 text canvas unavailable');
  let pending=null,bank=null,closed=false;
  function hide(){canvas.hidden=true;delete canvas.dataset.messageLines;}
  function close(){if(closed)return;closed=true;hide();if(bank){for(const page of bank.pages){page.image.close();page.outlineImage.close();}bank=null;}}
  signal.addEventListener('abort',close,{once:true});hide();
  async function prepare(){
    signal.throwIfAborted();if(closed)throw Error('Original stage text closed');
    pending??=(async()=>{
      const metadata=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(await resources.read('anim_original-stage-text-full42')));
      const loaded=await loadOriginalStageTextBank(metadata,async(file,expected)=>{
        const response=await resources.fetch(safeAssetUrl(resources.base,file),{signal});if(!response.ok)throw Error('Original stage text texture unavailable');
        const bytes=await response.arrayBuffer(),sha=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),byte=>byte.toString(16).padStart(2,'0')).join('');
        if(sha!==expected)throw Error('Original stage text texture receipt differs');
        signal.throwIfAborted();return createImageBitmap(new Blob([bytes],{type:'image/png'}),{premultiplyAlpha:'none',colorSpaceConversion:'none'});
      },{signal});
      if(closed||signal.aborted){for(const page of loaded.pages){page.image.close();page.outlineImage.close();}signal.throwIfAborted();throw Error('Original stage text closed');}
      bank=loaded;
    })();
    return pending;
  }
  function paint(state){
    const rows=stageTextRequests(state);if(!rows.length){hide();return;}
    if(!bank||closed)throw Error('Original TH128 text was not prepared');
    context.resetTransform();context.clearRect(0,0,canvas.width,canvas.height);
    context.setTransform(canvas.width/640,0,0,canvas.height/480,0,0);context.imageSmoothingEnabled=false;
    let dialogue=0;
    try{for(const row of rows){drawOriginalStageText(context,bank,row);if(row.kind!=='spell')dialogue++;}}
    finally{context.resetTransform();}
    canvas.hidden=false;canvas.dataset.messageLines=String(dialogue);
  }
  return Object.freeze({prepare,paint,hide,close});
}
