// Generated from eagler-common/browser/startup-developers.mjs.
export async function installStartupBranding(core,{game,builtAt,fs=core.FS}={}){
  if(!['th10','th11','th12','th13','th15','th20'].includes(game))throw Error('Unsupported startup build timestamp');
  const date=builtAt===undefined&&game==='th12'?null:new Date(builtAt);
  if(date&&!Number.isFinite(date.getTime()))throw Error('Missing Runtime build timestamp');
  const stamp=date?new Date(date.getTime()+8*3600000).toISOString():null;
  const canvas=document.createElement('canvas');canvas.width=1280;canvas.height=960;
  const ctx=canvas.getContext('2d');ctx.scale(2,2);
  ctx.font='7.5px "Times New Roman",serif';ctx.fillStyle='#fff';ctx.textAlign='right';ctx.textBaseline='bottom';
  if(stamp)ctx.fillText(`build ${stamp.slice(0,10).replaceAll('-','.')} ${stamp.slice(11,16)} UTC+8`,628,472);
  fs.writeFile('/eagler-startup.rgba',ctx.getImageData(0,0,1280,960).data);
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
