import {AnimationSampler,MenuAnimation} from './animation.mjs';

// Retail411420: MSG time advances only while a wait instruction is released.
// The large6000000 wait is intentional: it waits for confirm, not 27 hours.
export class EndingTimeline {
  constructor({resources,atlases,ending,difficulty,onMusic=()=>{},onFadeMusic=()=>{}}){
    this.resources=resources;this.sampler=new AnimationSampler(atlases);this.difficulty=difficulty;this.onMusic=onMusic;this.onFadeMusic=onFadeMusic;
    this.banks=new Map();this.animations=new Map();this.lines=[];this.color=0xffffff;this.clock=0;this.pc=0;this.wait=null;this.finished=false;this.fade=null;
    this.message=ending;this.commands=ending.commands;this.scene='ending';
  }
  tick({held=0,pressed=0}={}){
    for(const vm of this.animations.values())vm.tick();
    if(this.fade)this.fade.age++;
    if(this.finished)return;
    if(this.wait){
      this.wait.left--;
      const confirm=!!(pressed&0x80001),skip=!!(held&512)&&this.wait.left%6===0;
      if(this.wait.left>0&&!confirm&&!skip)return;
      if(this.wait.clear)this.lines=[];this.wait=null;
    }
    for(let budget=0;budget<1024&&this.pc<this.commands.length;budget++){
      const c=this.commands[this.pc];if(c.time>this.clock)break;this.pc++;
      switch(c.op){
        case 0:this.finished=true;return;
        case 3:this.lines.push({text:c.text,color:this.color});if(this.lines.length>5)this.lines.shift();break;
        case 4:this.lines=[];break;
        case 5:case 6:this.wait={left:c.frames<0?999:c.frames,clear:c.clearText};return;
        case 7:this.banks.set(c.bankSlot,c.file.replace(/\.anm$/,''));break;
        case 8:case 15:case 16:case 17:{
          if(c.difficulty!==undefined&&c.difficulty!==this.difficulty)break;
          const name=this.banks.get(c.bankSlot);if(!this.sampler.atlases[name]?.scripts[c.script])throw Error('结局动画引用无效');
          this.animations.set(c.slot,new MenuAnimation(this.sampler,name,c.script));break;
        }
        case 9:this.color=((c.colorRGB&255)<<16)|(c.colorRGB&0xff00)|((c.colorRGB>>>16)&255);break;
        case 10:this.onMusic(c.track);break;
        case 11:this.onFadeMusic();break;
        case 12:
          if(c.file!=='staff.msg')throw Error('结局分支文件无效');
          this.commands=this.resources.credits.commands;this.scene='credits';this.pc=0;this.clock=0;this.lines=[];this.animations.clear();this.fade=null;break;
        case 13:case 14:this.fade={frames:Math.max(1,c.frames),age:0,out:c.fadeOut};break;
        default:throw Error('结局命令尚未还原 '+c.op);
      }
    }
    this.clock++;
  }
  poses(){return [...this.animations.values()].flatMap(vm=>vm.poses().map(a=>({a,x:0,y:0,alpha:1})));}
  texts(){return this.lines.map((r,i)=>({text:r.text,x:32,y:330+i*26,width:576,height:24,color:r.color,size:16}));}
  render(renderer,fps){renderer.renderMenu(this.poses(),this.texts(),fps);if(this.fade){const value=Math.min(1,this.fade.age/this.fade.frames);renderer.renderMenu([],[],null,{overlay:true,shade:this.fade.out?value:1-value});}}
}
