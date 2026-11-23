// One local presentation per sealed result. Nothing in this UI controls the live World.
const labels=new Map([['team-over','全队全灭'],['left','本局已结束'],['completed','本局已结束']]);
export function bindCoopResult(ui,{makeFile,save,exportFile=()=>{},enterReplay,returnRoom,signal}){
 let shown=false,suspended=false,result=null,file=null,generation=0,focus=null;
 function paint(){ui.panel.hidden=!shown||suspended||!result;ui.caption.textContent=labels.get(result)??'';ui.save.disabled=ui.review.disabled=!file;if(ui.download)ui.download.disabled=!file;}
 function hide(){generation++;shown=false;result=file=focus=null;paint();}
 function listen(key,fn){ui[key]?.addEventListener('click',()=>{if(shown&&!suspended&&result)fn();},signal?{signal}:undefined);}
 const enter=(key,value)=>{focus=ui[key];shown=false;paint();enterReplay(value);};
 listen('save',()=>{if(file)save(file);});listen('download',()=>{if(file)exportFile(file);});listen('review',()=>{if(file)enter('review',file);});
 listen('open',()=>enter('open',null));listen('back',returnRoom);signal?.addEventListener('abort',hide,{once:true});hide();
 return Object.freeze({async show(value){
   hide();if(signal?.aborted||!value?.active||!labels.has(value.result))return;
   const ticket=generation;result=value.result;shown=true;paint();
   if(value.replayBytes)try{const archive=await makeFile(value.replayBytes);if(ticket===generation&&!signal?.aborted){file=archive;paint();}}
   catch(error){if(ticket===generation&&!signal?.aborted){ui.save.title=String(error.message??error);}}
 },hide,suspend(active){suspended=Boolean(active);paint();},restore(){if(result&&!signal?.aborted){shown=true;paint();focus?.focus();}}});
}
