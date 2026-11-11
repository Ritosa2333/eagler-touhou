// The selection screen separates complete matches. Changes between native
// rounds of the same match do not create another entry.
export function createPc98MatchIndex(game){
  let armed=true,round=-1,match=0;const nodes=[];
  return {
    append(rows,first){
      if(game!=='03')return;
      for(let i=0;i<rows.length;i++){
        const scene=rows[i].scene;
        if(scene===2)armed=true;
        else if(scene===1&&armed){
          armed=false;
          match++;round=-1;if(nodes.length<512)nodes.push({id:nodes.length+1,kind:'stage',frame:first+i,stage:match,bossSlots:0,label:`第 ${match} 局`});
        }
        if(scene===1&&Number.isInteger(rows[i].round)&&rows[i].round>=0&&rows[i].round!==round){round=rows[i].round;if(nodes.length<512)nodes.push({id:nodes.length+1,kind:'stage',frame:first+i,stage:match,bossSlots:0,label:`第 ${round+1} 回合`});}
      }
    },
    get nodes(){return nodes.slice();},
  };
}
