// thcrap_tsa layout.cpp's text runs and persistent tab stops. Measure with the
// installed runtime font so alignment never depends on UTF-8 byte counts.
export function thcrapLayout(text,{measure,width=1024,tabs=[]}={}){
  const tokens=[];
  for(let at=0;at<text.length;){
    if(text[at]==='<'){
      let depth=0,start=at+1,end=-1;const fields=[];
      for(let p=start;p<text.length;p++){
        if(text[p]==='<')depth++;
        if(text[p]==='>'){if(depth===0){fields.push(text.slice(start,p));end=p+1;break;}depth--;}
        if(text[p]==='$'&&depth===0){fields.push(text.slice(start,p));start=p+1;}
      }
      if(end>at&&fields.length>1){tokens.push(fields);at=end;continue;}
    }
    const next=text.indexOf('<',at+1),end=next<0?text.length:next;
    tokens.push([text.slice(at,end)]);at=end;
  }
  let x=0,tab=0;const runs=[];
  for(let n=0;n<tokens.length;n++){
    const args=tokens[n],commands=args.length>1?args[0]:'',draw=args.length>1?args[1]:args[0];
    let advance=measure(draw,commands),print=true;
    if(args.length>1){
      let end=args.length>2?(args[2]?x+measure(args[2],commands):width):
        tabs[tab]??(tabs.length&&n===tokens.length-1?width:x+advance),tabCommand=false;
      for(const command of commands){
        if(command==='s'){end=x;print=false;tabCommand=true;}
        else if(command==='t'){end=x+Math.max(...args.slice(1).map(s=>measure(s,commands)));tabs[tab]=end;tabCommand=true;}
        else if(command==='l')tabCommand=true;
        else if(command==='c'){x+=(end-x-advance)/2;tabCommand=true;}
        else if(command==='r'){x=end-advance;tabCommand=true;}
      }
      if(tabCommand){tab++;advance=end-x;}
    }
    if(print&&draw)runs.push({text:draw,commands,x});x+=advance;
  }
  return {runs,width:x,tabs};
}
