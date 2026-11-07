// MAINL briefly exposes its sprite atlas while rebuilding the stage screen.
// Keep the last presented frame until the next native stage intro is drawn.
export function isRestartStageIntro(pixels,width,height){
  if(!pixels||width<64||height<64||pixels.length<width*height*3)return false;
  let blue=0,total=0;
  for(const y of [0.015,0.03,0.045,0.06])for(let column=0;column<32;column++){
    const x=Math.floor((column+.5)*width/32),row=Math.floor(y*height),at=(row*width+x)*3;
    const r=pixels[at],g=pixels[at+1],b=pixels[at+2];total++;
    if(b>40&&b*10>r*14&&b*10>g*12)blue++;
  }
  return blue>=total*.65;
}
export function createRestartVisualGate(presenter,now=()=>performance.now()){
  let armed=false,nonIntro=false,missing=false,closed=false;const deadline=now()+15000;
  function release(){if(closed)return;closed=true;presenter.releaseGate();}
  presenter.holdUntil((pixels,width,height)=>{
    if(!armed)return false;
    if(!isRestartStageIntro(pixels,width,height)){nonIntro=true;return false;}
    if(!nonIntro)return false;
    closed=true;return true;
  });
  return {
    program(name){if(name==='MAINL.EXE')armed=true;},
    observe(state){if(!state)missing=true;else if(missing&&(state.phase===1||state.phase===3))release();if(now()>=deadline)release();},
    release,
    get closed(){return closed;}
  };
}
