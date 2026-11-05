// Shared pure simulation for players and isolated replay Workers.
import {applyStage} from '../stage-select.js';
export class NativeSimulation{
  constructor(core,settings,hostSlot){this.core=core;this.settings=structuredClone(settings);this.hostSlot=hostSlot;this.frame=0;this.previous=[];}
  reset(){
    const s=this.settings,e=this.core;
    e.th_init(s.players,s.count,0x12345678,s.lifeCompensation?1:0);
    e.th_set_netplay_rules(1);e.th_set_difficulty(s.rank);applyStage(e,s.startStage);
  }
  start(){this.stopped=false;}
  state(){
    if(!this.core)return null;const e=this.core,g=(n,p=0)=>e.th_get(n,p);
    return {phase:g(0),stage:g(27),route:g(69),count:g(4),menu:g(50),cursor:g(51),timer:g(16),
      entrance:g(117),bossPhase:g(60),bombActive:!!g(14),score:g(2),rank:g(42),
      players:Array.from({length:g(3)},(_,p)=>({left:g(30,p),lives:g(31,p),bombs:g(32,p),ghost:!!g(33,p),invincibility:g(34,p),rescues:g(35,p)})),
      balls:Array.from({length:g(4)},(_,p)=>({x:g(20,p),y:g(21,p)}))};
  }
  step(inputs){
    const e=this.core,g=(f,p=0)=>e.th_get(f,p),host=inputs[this.hostSlot],edges=inputs.map((v,i)=>v&~(this.previous[i]||0)),edge=edges[this.hostSlot];
    const menu=g(50);let handled=false;
    // Any player may request pause. Only the room owner controls menus,
    // including route choice, continue, ending skip and name registration.
    if(!menu&&edges.some(v=>v&128)){
      e.th_menu_open(3,this.settings.lifeCompensation?1:0);handled=true;
    }else if(menu){
      let key=edge&16?0:edge&32?1:edge&1?2:edge&2?3:edge&64?(menu===12?6:4):edge&4?4:edge&(8|128)?5:-1;
      // Dedicated Continue selects Yes; confirmation remains a separate input.
      if((edge&256)&&menu===9)key=0;
      if(key>=0){
        const action=e.th_menu_input(key);
        if(action===1)this.reset();
        // Quit exits to the in-game title, rather than closing one peer's tab.
        if(action===3)e.th_menu_open(1,this.settings.lifeCompensation?1:0);
        handled=true;
      }
    }else if(g(0)===2&&(edge&(4|64))){e.th_continue();handled=true;}
    if(this.settings.qa&&!menu){
      if(edge&16384)e.th_debug_invincible(!g(43));
      for(let p=0;p<inputs.length;p++)if(edge&[512,1024,8192][p])e.th_debug_hit(p,1);
      if(edge&2048){
        if(g(60)>=0)e.th_stage_command(20,0,g(61),0,0,0);
        else e.th_stage_command(15,0,0,0,0,0);
      }
    }
    if(!handled)e.th_step(...[0,1,2].map(i=>((inputs[i]||0)&15|((inputs[i]||0)&4096?16:0)|((inputs[i]||0)&0xffff8000))>>>0));
    this.previous=inputs.slice();this.frame++;

  }
  checksum(){
    let h=this.core.th_net_checksum();
    for(const n of [this.frame,...this.previous])for(let b=0;b<4;b++)h=Math.imul(h^((n>>>(8*b))&255),16777619);
    return {type:'hash',frame:this.frame,hash:(h>>>0).toString(16).padStart(8,'0'),memory:true};
  }
}
