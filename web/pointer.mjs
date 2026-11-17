// Hosted points are fractions of the iframe, including its letterbox bars.
// Canvas-local points already use the game's 640 x 480 coordinate system.
export function hostedPoint(message,width,height){
 const scale=Math.min(width/640,height/480);
 if(!(scale>0))throw Error('TH05 input surface has no size');
 return {...message,x:(message.x*width-(width-640*scale)/2)/scale,y:(message.y*height-(height-480*scale)/2)/scale};
}
export function createPointerRouter(module,touch){
 const pending=new Map();
 return {
  pointer(message,mouse=false){
   if(message.type==='down')pending.delete(message.id);
   const type={down:0,move:1,up:2}[message.type];
   const route=module._th05_pointer(type,message.id,message.x,message.y,+mouse);
   if(route===2&&type===0)pending.set(message.id,{...message});
   if(route===3){const down=pending.get(message.id);pending.delete(message.id);if(down)touch.pointer(down);}
   if(type===2)pending.delete(message.id);
   if(!mouse&&route!==1&&route!==2&&route!==4)touch.pointer(message);
   return route;
  },
  cancel(){pending.clear();module._th05_pointer_cancel();},
 };
}
