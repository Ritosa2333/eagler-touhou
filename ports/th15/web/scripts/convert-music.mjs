// Uses the same local libsndfile/Vorbis preparation path as the delivered TH11 port.
import{spawn}from'node:child_process';import{fileURLToPath}from'node:url';
const script=fileURLToPath(new URL('prepare-music.py',import.meta.url));
const child=spawn(process.env.TH15_PYTHON||'python',[script],{windowsHide:true,stdio:'inherit'});
child.on('error',error=>{console.error(error);process.exitCode=1;});
child.on('exit',code=>process.exitCode=code??1);
