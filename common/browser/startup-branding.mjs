// Own artwork only: the original startup picture is never read or rewritten.
export async function installStartupBranding(core, {game, builtAt, fs = core.FS} = {}) {
  const themes = {
    th06: {ink:'#fff0d5',shadow:'#493326'},
    th07: {ink:'#e9e9ff',shadow:'#302942'},
    th08: {ink:'#eaddff',shadow:'#24103e'},
    th09: {ink:'#ffe4ee',shadow:'#492339'},
  };
  const theme=themes[game];
  if(!theme)throw Error('Unsupported startup branding theme');
  const date=new Date(builtAt);
  if(!Number.isFinite(date.getTime()))throw Error('Missing Runtime build timestamp');
  const stamp=new Date(date.getTime()+8*3600000).toISOString();
  const text=`build ${stamp.slice(0,10).replaceAll('-','.')} ${stamp.slice(11,16)} UTC+8`;
  const svg=STARTUP_WORDMARK.replaceAll('#29203f',theme.ink).replaceAll('#291426',theme.shadow);
  const url=URL.createObjectURL(new Blob([svg],{type:'image/svg+xml'}));
  try{
    const logo=new Image();logo.src=url;await logo.decode();
    const canvas=document.createElement('canvas');canvas.width=1280;canvas.height=960;
    const context=canvas.getContext('2d');context.scale(2,2);
    // Original glyph bounds: x=8..215, y=8..24. Optical right edge 628.
    const scale=226/207;
    context.drawImage(logo,402-8*scale,112-8*scale,224*scale,32*scale);
    context.font='8px Georgia, "Times New Roman", serif';
    context.textAlign='right';context.textBaseline='bottom';context.fillStyle=theme.ink;
    context.shadowColor=theme.shadow;context.shadowBlur=2;context.shadowOffsetX=1;context.shadowOffsetY=1;
    context.fillText(text,628,141);
    // Raw pixels suit the older renderers; PNG suits directory image decoders.
    fs.writeFile('/eagler-startup.rgba',context.getImageData(0,0,1280,960).data);
    const blob=await new Promise(resolve=>canvas.toBlob(resolve,'image/png'));
    if(!blob)throw Error('Startup branding texture unavailable');
    fs.writeFile('/eagler-startup.png',new Uint8Array(await blob.arrayBuffer()));
  }finally{URL.revokeObjectURL(url);}
}
