// Live options request a common confirmed frontier. Selection/manual browsing is local.
const pauseRows=['resume','end','save','manual','restart'];
const terminalRows=['end','save','manual','restart']; // no single-player Continue / 再挑战
const accept=new Set(['KeyZ','Enter','KeyC','ControlLeft']);
const menuKeys=new Set([...accept,'Escape','KeyX','KeyR','ArrowUp','ArrowDown','ArrowLeft','ArrowRight','ShiftLeft','ShiftRight']);
export function createCoopPauseMenu(request,sound=()=>{}){
  const held=new Set(),captured=new Set();
  let phase=null,epoch=null,menu=null,requested=false,shortcut=false;
  const focus=(selection,confirm=false,yes=false,help=null)=>{
    menu=Object.freeze({kind:phase==='game-over'?'game-over':'pause',selection,confirm,yes,help});
  };
  function sync(state){
    const next=!state.startup.canStart?'waiting':state.phase==='ended'&&state.result==='team-over'?'game-over':state.phase;
    if(phase===next&&epoch===state.epoch)return;
    const retry=shortcut&&epoch===state.epoch;
    epoch=state.epoch;phase=next;requested=false;shortcut=false;menu=null;
    if(phase==='paused'||phase==='game-over'){
      for(const code of held)captured.add(code);
      if(phase==='game-over')focus('end');else{focus(retry?'restart':'resume',retry);sound(14);}
    }
  }
  function send(action){if(action!=='terminal-save')requested=true;request(action);}
  function key({code,down,repeat=false}){
    if(!menuKeys.has(code))return false;
    const special=code==='Escape'||code==='KeyR';
    // Duplicate Launcher/native-browser delivery and orphan releases after resume are consumed once.
    if(!down){held.delete(code);return captured.delete(code)||!!menu||special;}
    const edge=!repeat&&!held.has(code);held.add(code);
    if(!menu&&!special&&!captured.has(code))return false;
    captured.add(code);
    if(!edge||requested)return true;
    if(!menu){if(phase==='running'&&special){shortcut=code==='KeyR';send('pause');}return true;}
    const terminal=phase==='game-over',cancel=code==='KeyX'||code==='Escape';
    const delta=code==='ArrowUp'?-1:code==='ArrowDown'?1:0;
    if(menu.help){
      let {topic,page}=menu.help;
      if(cancel||(page&&accept.has(code))){sound(9);if(!page){focus('manual');return true;}page=false;}
      else if(delta){
        const next=page?Math.max(0,Math.min(8,topic+delta)):(topic+delta+9)%9;
        if(next!==topic){sound(10);if(page)sound(7);topic=next;}
      }else if(accept.has(code)){sound(7);page=true;}
      focus('manual',false,false,Object.freeze({topic,page}));return true;
    }
    if(!terminal&&code==='Escape'){sound(menu.confirm?9:7);send('resume');return true;}
    if(menu.confirm){
      if(delta){sound(10);focus(menu.selection,true,!menu.yes);}
      else if(code==='KeyX'){sound(9);focus(menu.selection,menu.yes,false);}
      else if(accept.has(code)){sound(menu.yes?7:9);if(menu.yes)send(menu.selection);else focus(menu.selection);}
    }else if(cancel){if(!terminal){sound(7);send('resume');}}
    else if(!terminal&&code==='KeyR'){sound(7);focus('restart',true);}
    else if(delta){const rows=terminal?terminalRows:pauseRows;focus(rows[(rows.indexOf(menu.selection)+rows.length+delta)%rows.length]);sound(10);}
    else if(accept.has(code)){
      sound(7);
      if(menu.selection==='manual')focus('manual',false,false,Object.freeze({topic:0,page:false}));
      else if(terminal)send('terminal-'+menu.selection);
      else if(menu.selection==='resume')send('resume');
      else focus(menu.selection,true);
    }
    return true;
  }
  return Object.freeze({sync,key,status:()=>menu,clearInput(){held.clear();captured.clear();}});
}
