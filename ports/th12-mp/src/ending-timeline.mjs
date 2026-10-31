import {AnimationSampler,MenuAnimation} from './animation.mjs';

// Retail411420: MSG time advances only while a wait instruction is released.
// The large6000000 wait is intentional: it waits for confirm, not 27 hours.
//411230..41135a creates text.anm68..72, sprite23..27 (512x17), at
//(64,384+16*i), layer24.4112a0/4112f0 set the text size/offset bytes16/16;
//4606b0/44e660 uses 32px MS Gothic400 reduced by two, already provided by
//Renderer's menuText path. Interrupt2 fades in over3 ticks and grows the
//vertical scale over8; interrupt3 fades out over3 ticks. Staff uses its own
//ANM textures and contains no MSG text commands.
export class EndingTimeline {
  constructor({resources,atlases,ending,difficulty,seen=false,onMusic=()=>{},onFadeMusic=()=>{},onSound=()=>{}}){
    this.resources=resources;this.sampler=new AnimationSampler(atlases);this.difficulty=difficulty;this.seen=!!seen;this.onMusic=onMusic;this.onFadeMusic=onFadeMusic;this.onSound=onSound;
    this.banks=new Map();this.animations=new Map();this.lines=[];this.lineSlot=0;this.color=0xffffff;this.clock=0;this.pc=0;this.wait=null;this.finished=false;this.fade=null;
    this.textAnimations=Array.from({length:5},(_,i)=>{const vm=new MenuAnimation(this.sampler,'text',68+i);vm.setText(68+i,{text:' ',colorRGB:0xffffff,width:512,height:17});return vm;});
    this.message=ending;this.commands=ending.commands;this.scene='ending';
  }
  tick({held=0,pressed=0}={}){
    for(const vm of [...this.animations.values(),...this.textAnimations])vm.tick();
    if(this.fade)this.fade.age++;
    if(this.finished)return;
    if(this.wait&&!this.releaseWait({held,pressed}))return;
    for(let budget=0;budget<1024&&this.pc<this.commands.length;budget++){
      const c=this.commands[this.pc];if(c.time>this.clock)break;this.pc++;
      switch(c.op){
        case 0:this.finished=true;return;
        case 3:{
          //411467..411519 clears all five textures when the write slot is0;
          //411554..411561 wraps after five lines. It does not scroll old rows.
          if(this.lineSlot===0){this.lines=[];this.textAnimations.forEach((vm,i)=>{vm.setText(68+i,{text:' ',colorRGB:0xffffff,width:512,height:17});vm.interrupt(3,{immediate:true});});}
          const slot=this.lineSlot,vm=this.textAnimations[slot];this.lines[slot]={text:c.text,color:this.color};
          vm.setText(68+slot,{text:c.text,colorRGB:this.color,width:512,height:17});vm.interrupt(2,{immediate:true});
          this.lineSlot=(slot+1)%5;break;
        }
        case 4:this.textAnimations.forEach(vm=>vm.interrupt(3,{immediate:true}));break;
        case 5:case 6:
          this.wait={left:c.frames,indefinite:c.op===5&&c.frames<0,clear:c.clearText};
          if(!this.releaseWait({held,pressed}))return;break;
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
          this.commands=this.resources.credits.commands;this.scene='credits';this.pc=0;this.clock=0;this.lines=[];this.lineSlot=0;this.textAnimations=[];this.animations.clear();this.banks.clear();this.color=0xffffff;this.fade=null;break;
        case 13:case 14:this.fade={frames:Math.max(1,c.frames),age:0,out:c.fadeOut};break;
        default:throw Error('结局命令尚未还原 '+c.op);
      }
    }
    this.clock++;
  }
  releaseWait({held,pressed}){
    //41159d advances the wait timer in the instruction's first invocation.
    //4115b2..4115bf resets a negative opcode5 wait to999 on EVERY visit,
    //so Staff's -1 wait never expires and Ctrl cannot release999%6.
    this.wait.left=this.wait.indefinite?999:this.wait.left-1;
    //410da2/410dab records a first viewing;4115dd rejects Ctrl while that
    //flag is set. The host supplies the saved ending-viewed state explicitly.
    const confirm=!!(pressed&0x80001),skip=this.seen&&!this.wait.indefinite&&!!(held&512)&&this.wait.left%6===0;
    if(this.wait.left>0&&!confirm&&!skip)return false;
    if(confirm||this.wait.left<=0)this.onSound(0); // Native411616 /4116a9.
    //411699 resets only the write cursor; existing text remains until the
    //following page starts (or opcode4 issues its hide interrupts).
    if(this.wait.clear)this.lineSlot=0;this.wait=null;return true;
  }
  poses(){return [...this.animations.values(),...this.textAnimations].flatMap(vm=>vm.poses().map(a=>({a,x:0,y:0,alpha:1})));}
  texts(){return [];}
  //MSG13/14 ->41192a/41193a ->452a7e/452bd4 ->452350: a full640x480
  //black rectangle, with integer alpha from451ef1/452406. Keep that rectangle
  //instead of inventing a shaded dialogue panel behind the text.
  render(renderer,fps){renderer.renderMenu(this.poses(),this.texts(),fps);if(this.fade){const value=Math.min(1,this.fade.age/this.fade.frames),alpha=Math.trunc(255*(this.fade.out?value:1-value))/255;renderer.renderMenu([],[],null,{overlay:true,shade:alpha});}}
}
