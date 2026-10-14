// Matches the optional same-origin carrier used by the Launcher. This only
// transports live input; the title still owns lifecycle and physical sampling.
const liveCommands=new Set(['keyboard','keyboard-clear','direct-touch','touch-controls','touch-cancel']);
export function createDirectInputBridge({protocol,game,epoch,origin,ready,dispatch}){
  if(typeof protocol!=='string'||!protocol||typeof game!=='string'||!game||
     !Number.isSafeInteger(epoch)||epoch<=0||typeof origin!=='string'||!origin||
     typeof ready!=='function'||typeof dispatch!=='function')throw Error('Invalid direct-input bridge identity');
  return Object.freeze({schema:'eagler-touhou/direct-input/1',protocol,game,epoch,origin,
    submit(message){
      if(!message||typeof message!=='object'||Array.isArray(message)||
         message.protocol!==protocol||message.game!==game||message.epoch!==epoch||
         message.request!==undefined||!liveCommands.has(message.command)||!ready())return false;
      // false must mean untouched. Exceptions and unknown acknowledgements
      // propagate; the Launcher must not retry a possibly consumed Bomb edge.
      const accepted=dispatch(message);
      if(accepted!==true&&accepted!==false)throw Error('Invalid direct-input dispatch acknowledgement');
      return accepted;
    }
  });
}
