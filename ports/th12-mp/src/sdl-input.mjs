// Hosted touch is normalized to the iframe; THPrac mouse messages use CSS pixels.
export function sdlPointerPosition(message,rect,viewport,normalized=false){
  const x=normalized?message.x*viewport.width:message.x;
  const y=normalized?message.y*viewport.height:message.y;
  return {...message,x:(x-rect.left)*640/rect.width,y:(y-rect.top)*480/rect.height};
}
