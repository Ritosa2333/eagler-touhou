// STARTUP_DEVELOPERS is embedded by the shared generation tool.
export async function installStartupBranding(core,{game,builtAt,fs=core.FS}={}){
  const top={th10:312,th11:312,th15:336,th20:336}[game];
  if(top===undefined)throw Error('Unsupported startup credit layout');
  const date=new Date(builtAt);
  if(!Number.isFinite(date.getTime()))throw Error('Missing Runtime build timestamp');
  const stamp=new Date(date.getTime()+8*3600000).toISOString();
  const url=URL.createObjectURL(new Blob([STARTUP_DEVELOPERS],{type:'image/svg+xml'}));
  try{
    const image=new Image();image.src=url;await image.decode();
    const canvas=document.createElement('canvas');canvas.width=1280;canvas.height=960;
    const ctx=canvas.getContext('2d');ctx.scale(2,2);
    // Center the complete phrase, including Developers, on the studio credit.
    const width=220,height=width*112/Number(STARTUP_DEVELOPERS.match(/viewBox="0 0 ([\d.]+)/)[1]);
    ctx.drawImage(image,320-width/2,top,width,height);
    ctx.font='7.5px "Times New Roman",serif';ctx.fillStyle='#fff';ctx.textAlign='right';ctx.textBaseline='bottom';
    ctx.fillText(`build ${stamp.slice(0,10).replaceAll('-','.')} ${stamp.slice(11,16)} UTC+8`,628,472);
    fs.writeFile('/eagler-startup.rgba',ctx.getImageData(0,0,1280,960).data);
  }finally{URL.revokeObjectURL(url);}
}

// Advance only the isolated original signature animation while menu assets
// prepare. Its own RGB/alpha interpolation covers the credit in the same pass.
export async function finishStartupAnimation(startedAt,draw){
  let rendered=0;
  while(true){
    const elapsed=performance.now()-startedAt;
    const frame=Math.min(120,Math.floor(elapsed*60/1000));
    if(frame>rendered){if(!draw(frame-rendered))throw Error('Startup animation draw failed');rendered=frame;}
    if(elapsed>=2000)break;
    await new Promise(requestAnimationFrame);
  }
}
