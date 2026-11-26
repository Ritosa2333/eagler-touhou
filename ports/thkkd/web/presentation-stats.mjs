export class PresentationStats {
  constructor(){this.reset();}
  reset(){this.samples=[];this.total=0;this.fps=0;}
  presented(time){
    const samples=this.samples,last=samples.at(-1);
    if(last!==undefined&&(time<last||time-last>250))samples.length=0;
    samples.push(time);this.total++;
    while(samples.length>2&&samples[1]<=time-1000)samples.shift();
    const elapsed=time-samples[0];this.fps=elapsed>0?(samples.length-1)*1000/elapsed:0;
  }
}
