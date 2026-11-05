import {AnimationSampler,MenuAnimation} from './animation.mjs';

//411b80 binds ascii18 at(480,392);411b00 sends IRQ1 and clears its handle.
//The original15/16 scripts own the text alpha loops,17 owns the flower
//motion, and18 spawns another flower every four UI ticks. No DOM text/CSS
//animation is involved. This stream is separate from gameplay randomness.
export class LoadingPresentation {
  constructor(renderer){
    this.renderer=renderer;this.seed=0x1234;
    this.sampler=new AnimationSampler(renderer.atlases,{displayRandom32:()=>this.nextRandom()});
    this.indicator=null;this.retiring=[];this.signature=null;this.startup=false;
    this.waiting=false;this.startupFrames=0;this.uiFrame=0;this.lastStartupDraw=-1;this.accumulator=0;this.startupWaiters=[];
  }
  nextRandom(){
    const a=((this.seed^0x9630)-0x6553)&65535,b=((a<<2)|(a>>>14))&65535,c=((b^0x9630)-0x6553)&65535;
    this.seed=((c<<2)|(c>>>14))&65535;return ((a<<16)|c)>>>0;
  }
  beginStartup(){
    this.startup=true;this.waiting=true;this.startupFrames=0;this.lastStartupDraw=-1;this.accumulator=0;
    this.signature=new MenuAnimation(this.sampler,'sig',0);this.signature.tick();
  }
  showIndicator(){
    //The native manager keeps a single live handle across the outgoing
    //menu fade and the resource worker; beginning the worker does not rebind.
    if(this.indicator)return;
    this.indicator=new MenuAnimation(this.sampler,'ascii',18);this.indicator.tick();
  }
  startResources({renderBase=null}={}){this.resourceBase=renderBase;this.waiting=true;this.showIndicator();}
  retireIndicator(){
    if(!this.indicator)return;
    this.indicator.interrupt(1);this.retiring.push(this.indicator);this.indicator=null;
  }
  completeResources(){this.waiting=false;this.resourceBase=null;this.retireIndicator();}
  waitForStartup(){
    //43f320 waits for at least180 startup frames, independently of worker
    //completion. Stage resource workers have no invented minimum duration.
    if(this.startupFrames>=180)return Promise.resolve();
    return new Promise(resolve=>this.startupWaiters.push(resolve));
  }
  endStartup(){
    //43f394..43f3a4 frees sig immediately; title's first update retires18.
    this.startup=false;this.signature=null;this.renderer.unloadAtlases(['sig']);this.completeResources();
  }
  advance(elapsed){
    this.accumulator+=Math.max(0,Math.min(250,elapsed));
    while(this.accumulator>=1000/60){
      this.accumulator-=1000/60;
      this.uiFrame++;if(this.startup)this.signature?.tick();
      this.indicator?.tick();for(const vm of this.retiring)vm.tick();
      this.retiring=this.retiring.filter(vm=>vm.alive);
    }
  }
  poses(){
    return [this.indicator,...this.retiring].filter(Boolean).flatMap(vm=>vm.poses().map(a=>({a,x:480,y:392,alpha:1})));
  }
  renderBase(fps=null){
    if(!this.startup&&this.resourceBase){this.resourceBase(fps);this.renderOverlay();return;}
    const signature=this.signature?.poses().map(a=>({a,x:0,y:0,alpha:1}))??[];
    this.renderer.renderMenu(signature,[],fps,{clearColor:[0,0,0,1]});this.renderOverlay();
    //42ef00 counts actual startup draws; catching up several VM ticks in
    //one browser frame must not count several splash draws. Duplicate120Hz
    //presents of the same60Hz UI frame do not shorten the startup interval.
    if(this.startup&&this.lastStartupDraw!==this.uiFrame){
      this.lastStartupDraw=this.uiFrame;
      if(++this.startupFrames===180){for(const resolve of this.startupWaiters)resolve();this.startupWaiters=[];}
    }
  }
  renderOverlay(){const poses=this.poses();if(poses.length)this.renderer.renderMenu(poses,[],null,{overlay:true});}
}
