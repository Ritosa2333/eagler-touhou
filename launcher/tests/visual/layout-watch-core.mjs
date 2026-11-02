export function copyRect(rect) {
  return Object.fromEntries(['x','y','width','height'].map(key=>[key,Math.round(rect[key]*100)/100]));
}

export function rectChange(before,after,{threshold=2,scrolling=false,resizing=false}={}) {
  if(!before||!after||before.width<=0||before.height<=0||after.width<=0||after.height<=0)return null;
  const delta=Object.fromEntries(['x','y','width','height'].map(key=>[key,Math.round((after[key]-before[key])*100)/100]));
  if(scrolling||resizing){delta.x=0;delta.y=0;}
  if(resizing)return null;
  const amount=Math.max(...Object.values(delta).map(Math.abs));
  return amount>=threshold?{before:copyRect(before),after:copyRect(after),delta,amount}:null;
}

/** LayoutShift rectangles can be clipped; use sampled full bounds for sizing. */
export function layoutEntryChange(previousRect,currentRect,actualRect,sampledRect,threshold=2){
  if(!previousRect.width||!previousRect.height||!currentRect.width||!currentRect.height)return null;
  const after=copyRect(actualRect??currentRect);
  const before=sampledRect&&rectChange(sampledRect,after,{threshold:.01})?sampledRect:{...after,x:after.x+previousRect.x-currentRect.x,y:after.y+previousRect.y-currentRect.y};
  return rectChange(before,after,{threshold});
}

/** Coalesce consecutive frames of one change, while bounding the session log. */
export function addChange(records,incoming,limit=120) {
  const last=records.at(-1);
  const sameKind=last&&(last.kind===incoming.kind||last.kind!=='inspection'&&incoming.kind!=='inspection');
  if(sameKind&&last.path===incoming.path&&last.action===incoming.action&&incoming.time-last.lastTime<180&&incoming.time-last.time<1200){
    last.lastTime=incoming.time;
    last.score=(last.score??0)+(incoming.score??0);
    last.methods=[...new Set([...(last.methods??[last.kind]),incoming.kind])];
    for(const source of incoming.sources){
      const previous=last.sources.find(item=>item.key===source.key);
      if(previous){const total=rectChange(previous.before,source.after,{threshold:0});previous.after=source.after;previous.delta=total?.delta??source.delta;previous.amount=Math.max(previous.amount,total?.amount??source.amount);}
      else last.sources.push(source);
    }
    last.sources.sort((a,b)=>b.amount-a.amount);last.sources.length=Math.min(last.sources.length,8);
    last.amount=Math.max(last.amount,incoming.amount,...last.sources.map(source=>source.amount));
    return last;
  }
  const record={...incoming,lastTime:incoming.time,sources:[...incoming.sources],expected:false};
  records.push(record);if(records.length>limit)records.splice(0,records.length-limit);
  return record;
}
