// The selection screen separates complete matches. Changes between native
// rounds of the same match do not create another entry.
export function createPc98MatchIndex(game){
  let armed=true;const nodes=[];
  return {
    append(rows,first){
      if(game!=='03')return;
      for(let i=0;i<rows.length;i++){
        const scene=rows[i].scene;
        if(scene===2)armed=true;
        else if(scene===1&&armed){
          armed=false;
          if(nodes.length<512){const n=nodes.length+1;nodes.push({id:n,kind:'stage',frame:first+i,stage:n,bossSlots:0,label:`第 ${n} 局`});}
        }
      }
    },
    get nodes(){return nodes.slice();},
  };
}
