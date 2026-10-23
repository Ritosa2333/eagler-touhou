import {createHash,randomUUID} from 'node:crypto';
import {inflateSync} from 'node:zlib';
import {writeFile} from 'node:fs/promises';

const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const requireValue=(value,message)=>{if(!value)throw Error(message);};
const crc32=bytes=>{let crc=0xffffffff;for(const byte of bytes){crc^=byte;for(let bit=0;bit<8;bit++)crc=(crc>>>1)^((crc&1)?0xedb88320:0);}return (crc^0xffffffff)>>>0;};
// Decode Chromium's bounded, non-interlaced RGB/RGBA PNG frames. The saved PNG,
// rather than a producer-supplied pixel counter, is the reassessment authority.
export function canvasPixelEvidence(bytes,rect){
  requireValue(Buffer.isBuffer(bytes)&&bytes.length<=8*1024*1024&&bytes.subarray(0,8).equals(Buffer.from('89504e470d0a1a0a','hex')),'Invalid bounded PNG');
  let width,height,channels,ended=false;const chunks=[];
  for(let offset=8;offset+12<=bytes.length;){
    const length=bytes.readUInt32BE(offset),type=bytes.toString('ascii',offset+4,offset+8),start=offset+8;
    requireValue(length<=8*1024*1024&&start+length+4<=bytes.length,'Truncated PNG chunk');
    requireValue(crc32(bytes.subarray(offset+4,start+length))===bytes.readUInt32BE(start+length),'PNG chunk checksum mismatch');
    if(type==='IHDR'){
      requireValue(offset===8&&length===13,'Invalid PNG header');
      width=bytes.readUInt32BE(start);height=bytes.readUInt32BE(start+4);
      channels=bytes[start+9]===2?3:bytes[start+9]===6?4:0;
      requireValue(width>0&&height>0&&width<=4096&&height<=4096&&bytes[start+8]===8&&channels&&
        bytes[start+10]===0&&bytes[start+11]===0&&bytes[start+12]===0,'Unsupported PNG encoding');
    }else if(type==='IDAT')chunks.push(bytes.subarray(start,start+length));
    else if(type==='IEND'){ended=true;requireValue(start+length+4===bytes.length,'Trailing PNG bytes');break;}
    offset=start+length+4;
  }
  requireValue(ended&&channels&&chunks.length,'Incomplete PNG');
  const stride=width*channels,raw=inflateSync(Buffer.concat(chunks),{maxOutputLength:(stride+1)*height});
  requireValue(raw.length===(stride+1)*height,'PNG pixel length mismatch');
  const pixels=Buffer.alloc(stride*height),paeth=(a,b,c)=>{const p=a+b-c,pa=Math.abs(p-a),pb=Math.abs(p-b),pc=Math.abs(p-c);return pa<=pb&&pa<=pc?a:pb<=pc?b:c;};
  for(let y=0;y<height;y++){
    const filter=raw[y*(stride+1)];requireValue(filter<=4,'Invalid PNG filter');
    for(let x=0;x<stride;x++){
      const a=x>=channels?pixels[y*stride+x-channels]:0,b=y?pixels[(y-1)*stride+x]:0,c=y&&x>=channels?pixels[(y-1)*stride+x-channels]:0;
      const predictor=[0,a,b,Math.floor((a+b)/2),paeth(a,b,c)][filter];
      pixels[y*stride+x]=(raw[y*(stride+1)+1+x]+predictor)&255;
    }
  }
  requireValue(rect&&[rect.x,rect.y,rect.width,rect.height].every(Number.isFinite)&&rect.width>0&&rect.height>0,'Missing canvas rectangle');
  const x0=Math.max(0,Math.ceil(rect.x)),y0=Math.max(0,Math.ceil(rect.y)),x1=Math.min(width,Math.floor(rect.x+rect.width)),y1=Math.min(height,Math.floor(rect.y+rect.height));
  requireValue(x1>x0&&y1>y0,'Canvas is outside captured viewport');
  let first=null,different=false,opaque=0;
  for(let y=y0;y<y1;y++)for(let x=x0;x<x1;x++){
    const p=(y*width+x)*channels,color=pixels.readUIntBE(p,3);
    if(channels===3||pixels[p+3]>0){opaque++;if(first===null)first=color;else if(color!==first)different=true;}
  }
  return {width,height,canvasPixels:(x1-x0)*(y1-y0),opaquePixels:opaque,nonUniform:different};
}

/** Retain actual compositor frames from the same Playwright-owned CDP session. */
export async function createFirstFrameCapture({context,page}){
  const cdp=await context.newCDPSession(page);let active=null;
  cdp.on('Page.screencastFrame',event=>{
    void cdp.send('Page.screencastFrameAck',{sessionId:event.sessionId}).catch(()=>{});
    if(active&&active.frames.length<48)active.frames.push({timestampEpochMs:event.metadata.timestamp*1000,
      metadata:event.metadata,png:event.data,sha256:hash(Buffer.from(event.data,'base64'))});
  });
  return {
    async begin({kind,epoch,frame}){
      await cdp.send('Page.stopScreencast');
      const canvasRect=await frame.locator('canvas').first().boundingBox();
      const time=await page.evaluate(()=>({epochMs:performance.timeOrigin+performance.now(),origin:location.origin}));
      active={kind,epoch,navigationId:randomUUID(),outerUrl:page.url(),runtimeUrl:frame.url(),origin:time.origin,
        launchEpochMs:time.epochMs,canvasRect,frames:[]};
      await cdp.send('Page.startScreencast',{format:'png',everyNthFrame:1});
    },
    async finish({path,scope,identity}){
      const receipt=await page.evaluate(()=>window.events.find(event=>event.event==='first-frame'));
      // Let frames already swapped before the send finish their CDP delivery.
      await cdp.send('Page.stopScreencast');
      const run={schema:'eagler-presentation/first-frame-navigation/1',...active,scope,identity,receipt};active=null;
      const bytes=Buffer.from(JSON.stringify(run,null,2)+'\n');await writeFile(path,bytes);
      return {path,bytes:bytes.length,sha256:hash(bytes)};
    },
    async close(){await cdp.detach();}
  };
}

export function assessFirstFrameEvidence({runs,scope,expectedBuild,expectedData}={}){
  const issues=[];const fail=message=>issues.push(message);
  if(!Array.isArray(runs)||runs.length!==2)return {status:'incomplete',complete:false,issues:['Cold and reload raw captures are required']};
  const navigations=new Set(),epochs=new Set();
  for(const [index,run] of runs.entries())try{
    requireValue(run.schema==='eagler-presentation/first-frame-navigation/1'&&run.kind===['cold','reload'][index],'Wrong first-frame navigation');
    requireValue(run.scope?.game===scope?.game&&run.scope?.variant===scope?.variant,'First-frame product mismatch');
    requireValue(expectedBuild?.packageSha256&&run.identity?.build?.packageSha256===expectedBuild.packageSha256&&
      run.identity.build.wasmSha256===expectedBuild.wasmSha256&&run.identity.build.manifestSha256===expectedBuild.manifestSha256,'First-frame production build mismatch');
    requireValue(expectedData?.sha256&&run.identity?.data?.sha256===expectedData.sha256&&run.identity.data.bytes===expectedData.bytes,'First-frame DATA mismatch');
    requireValue(typeof run.navigationId==='string'&&!navigations.has(run.navigationId)&&Number.isSafeInteger(run.epoch)&&run.epoch>0&&!epochs.has(run.epoch),'Reused navigation identity');
    navigations.add(run.navigationId);epochs.add(run.epoch);
    requireValue(new URL(run.outerUrl).origin===run.origin&&new URL(run.runtimeUrl).origin===run.origin&&
      Number(new URL(run.runtimeUrl).searchParams.get('runtimeEpoch'))===run.epoch,'Foreign Runtime navigation');
    const event=run.receipt,present=event?.presentation;
    requireValue(event?.event==='first-frame'&&event.game===scope.game&&event.epoch===run.epoch&&
      event.protocol==='eagler-touhou/1'&&present?.schema==='eagler-runtime/first-frame/1'&&
      Number.isSafeInteger(present.nativePresentCount)&&present.nativePresentCount>0&&
      Number.isFinite(run.launchEpochMs)&&Number.isFinite(present.sentAtEpochMs)&&present.sentAtEpochMs>=run.launchEpochMs,'Missing protocol send/native presentation receipt');
    requireValue(Array.isArray(run.frames)&&run.frames.length>0&&run.frames.length<=48,'Missing bounded compositor frames');
    let actual=false;
    for(const frame of run.frames){
      const bytes=Buffer.from(frame.png??'','base64');requireValue(hash(bytes)===frame.sha256,'Compositor PNG identity mismatch');
      requireValue(Number.isFinite(frame.timestampEpochMs)&&frame.timestampEpochMs===frame.metadata?.timestamp*1000,'Missing compositor swap clock');
      const pixels=canvasPixelEvidence(bytes,run.canvasRect);
      requireValue(frame.metadata.deviceWidth===pixels.width&&frame.metadata.deviceHeight===pixels.height&&
        frame.metadata.offsetTop===0&&frame.metadata.pageScaleFactor===1,'Compositor viewport scale differs from canvas coordinates');
      if(frame.timestampEpochMs>=run.launchEpochMs&&frame.timestampEpochMs<=present.sentAtEpochMs&&pixels.nonUniform)actual=true;
    }
    requireValue(actual,'No nonblank canvas compositor swap before first-frame send');
  }catch(error){fail(`${['cold','reload'][index]}: ${error.message}`);}
  return {status:issues.length?'failed':'passed',complete:issues.length===0,issues,
    boundary:'Actual Chromium compositor frame swap and Runtime send clock',physicalScanoutMeasured:false};
}
