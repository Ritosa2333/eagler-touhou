// Pure, opt-in qualification for one diagnostic Chrome flag. No browser or process I/O.
const GPU_VSYNC='--disable-gpu-vsync';
const FRAME_LIMIT='--disable-frame-rate-limit';
const matches=(arg,flag)=>arg.toLowerCase()===flag||arg.toLowerCase().startsWith(flag+'=');
const validPid=value=>Number.isSafeInteger(value)&&value>0;

export function parseGpuVsyncControl(value,headedControl){
 if(value===undefined)return Object.freeze({explicit:false,value:null,mode:'default',disableGpuVsync:null,addedArgs:Object.freeze([])});
 if(value!=='0'&&value!=='1')throw Error('DISABLE_GPU_VSYNC must be an exact string 0 or 1');
 if(headedControl!==true)throw Error('Explicit DISABLE_GPU_VSYNC requires HEADED_CONTROL=1');
 return Object.freeze({explicit:true,value,mode:'explicit-'+value,disableGpuVsync:value==='1',addedArgs:Object.freeze(value==='1'?[GPU_VSYNC]:[])});
}

export function gpuVsyncLaunchArgs(baseArgs,control){
 // An unset switch must not tighten the pre-existing driver's launch behavior.
 if(!control.explicit)return baseArgs;
 if(!Array.isArray(baseArgs)||!baseArgs.every(arg=>typeof arg==='string'))throw Error('GPU vsync diagnostic launch arguments unavailable');
 if(baseArgs.some(arg=>matches(arg,GPU_VSYNC)||matches(arg,FRAME_LIMIT)))throw Error('GPU vsync diagnostic base already contains a controlled or forbidden flag');
 return control.disableGpuVsync?[...baseArgs,GPU_VSYNC]:baseArgs;
}

export function inspectGpuVsyncLaunch(control,observation={}){
 if(!control.explicit)return {required:false,qualified:null};
 const {args,observationAvailable,verified,browserPid}=observation;
 const argumentsAvailable=Array.isArray(args)&&args.every(arg=>typeof arg==='string');
 const vsyncArgs=argumentsAvailable?args.filter(arg=>matches(arg,GPU_VSYNC)):null;
 const exactCount=vsyncArgs?.filter(arg=>arg===GPU_VSYNC).length??null;
 const assignedCount=vsyncArgs?.filter(arg=>arg.toLowerCase().startsWith(GPU_VSYNC+'=')).length??null;
 const nonExactCount=vsyncArgs===null?null:vsyncArgs.length-exactCount;
 const frameLimitCount=argumentsAvailable?args.filter(arg=>matches(arg,FRAME_LIMIT)).length:null;
 const reasons=[];
 if(observationAvailable!==true)reasons.push('observation-unavailable');
 if(verified!==true)reasons.push('base-launch-unverified');
 if(!argumentsAvailable)reasons.push('arguments-unavailable');
 if(!validPid(browserPid))reasons.push('browser-pid-invalid');
 if(argumentsAvailable){
  if(exactCount!==(control.disableGpuVsync?1:0)||nonExactCount!==0)reasons.push('gpu-vsync-flag-mismatch');
  if(frameLimitCount!==0)reasons.push('frame-rate-limit-disabled');
 }
 // Only counts and fixed labels escape; never retain argv, assigned values or paths.
 return {required:true,mode:control.mode,expectedDisabled:control.disableGpuVsync,
  observationAvailable:observationAvailable===true,baseLaunchVerified:verified===true,argumentsAvailable,
  browserPidValid:validPid(browserPid),exactCount,assignedCount,nonExactCount,
  totalCount:vsyncArgs?.length??null,frameLimitCount,qualified:reasons.length===0,reasons};
}

export function inspectGpuVsyncRun(control,launches){
 if(!control.explicit)return {required:false,qualified:null};
 const rows=Array.isArray(launches)?launches:[];
 const twoLaunches=rows.length===2;
 const validPids=twoLaunches&&rows.every(row=>validPid(row?.browserPid));
 const distinctPids=validPids&&rows[0].browserPid!==rows[1].browserPid;
 const launchQualified=rows.map(row=>row?.observationAvailable===true&&row?.verified===true&&
  row?.gpuVsync?.required===true&&row.gpuVsync.mode===control.mode&&row.gpuVsync.qualified===true&&
  row.gpuVsync.observationAvailable===true&&row.gpuVsync.baseLaunchVerified===true&&
  row.gpuVsync.argumentsAvailable===true&&row.gpuVsync.browserPidValid===true);
 const reasons=[];
 if(!twoLaunches)reasons.push('requires-two-launches');
 if(!validPids)reasons.push('browser-pids-unavailable');
 if(!distinctPids)reasons.push('browser-pids-not-distinct');
 if(!twoLaunches||launchQualified.some(ok=>!ok))reasons.push('launch-unqualified');
 return {required:true,mode:control.mode,launchCount:rows.length,twoLaunches,validPids,distinctPids,
  launchQualified,qualified:reasons.length===0,reasons};
}
