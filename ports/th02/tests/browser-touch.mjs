export async function enableTouch(page,{alwaysHitbox=true,mode='touch-unlimited',music}={}){
  await page.evaluateOnNewDocument(({alwaysHitbox,mode,music})=>{
    if(window!==window.top)return;
    localStorage.setItem('eagler-touhou-game-options-v1-th02',JSON.stringify({
      options:{touchEnabled:true,alwaysHitbox},...(music?{music,musicPreferenceExplicit:true}:{}),
    }));
    localStorage.setItem('eagler-touhou-language-v1-th02','lang_en');
    localStorage.setItem('eagler-touhou-touch-options-v1',JSON.stringify({
      touchMovementMode:mode,touchSensitivity:150,touchFocusMode:'hold-button',
    }));
  },{alwaysHitbox,mode,music});
}

export async function touchGame(page,dx=0,dy=0,hold=0){
  const surface=await page.waitForSelector('#touchDirectSurface',{visible:true});
  const {x,y,width,height}=await surface.boundingBox();
  const cx=Math.round(x+width/2),cy=Math.round(y+height/2);
  const finger=await page.touchscreen.touchStart(cx,cy);
  if(dx||dy){
    for(let i=1;i<=8;++i)await finger.move(cx+dx*i/8,cy+dy*i/8);
  }
  if(hold)await new Promise(resolve=>setTimeout(resolve,hold));
  await finger.end();
}

export async function touchCanvas(page,frame,x,y){
  const canvas=await frame.$('#canvas');
  const box=await canvas.boundingBox();
  await page.touchscreen.tap(Math.round(box.x+x*box.width/640),Math.round(box.y+y*box.height/400));
}

export async function touchButton(page,selector){
  const element=await page.waitForSelector(selector,{visible:true});
  const {x,y,width,height}=await element.boundingBox();
  await page.touchscreen.tap(Math.round(x+width/2),Math.round(y+height/2));
}

export async function startTouchFire(page){
  if(await page.$eval('#touchFire',e=>e.getAttribute('aria-pressed'))==='true')await touchButton(page,'#touchFire');
  await touchButton(page,'#touchFire');
  await page.waitForFunction(()=>document.querySelector('#touchFire')?.getAttribute('aria-pressed')==='true',{timeout:5000});
}

export function hitboxPoints(){
  const canvas=document.querySelector('#game canvas');
  const copy=document.createElement('canvas');copy.width=640;copy.height=400;
  const ctx=copy.getContext('2d');ctx.imageSmoothingEnabled=false;
  ctx.drawImage(canvas,0,0,640,400);
  const data=ctx.getImageData(0,0,640,400).data;
  const rgb=i=>(data[i]<<16)|(data[i+1]<<8)|data[i+2];
  const points=[];
  for(let y=33;y<376;++y)for(let x=37;x<415;++x){
    const i=(y*640+x)*4;
    if(rgb(i)===0xffffff&&rgb(i-4)===0xffd9dc&&rgb(i-8)===0xff697d&&
        rgb(i+4)===0xffd9dc&&rgb(i+8)===0xff697d)points.push({x,y});
  }
  return points;
}
