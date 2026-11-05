import test from 'node:test';
import assert from 'node:assert/strict';
import {prepareFastBoot as th02} from '../../ports/th02/web/dosbox-disk.js';
import {prepareFastBoot as th04} from '../../ports/th04/web/dosbox-disk.js';

for(const [game,prepare,folder,entries] of [['TH02',th02,'HUMAROKU',6],['TH04',th04,'GENSO',5]]){
  test(`${game} practice uses the deterministic gameplay CPU budget`,()=>{
    const disk={files:[{path:`game/${folder}/GAME.BAT`,contents:new TextEncoder().encode('main\r\n'.repeat(entries))}]};
    const output=options=>new TextDecoder().decode(prepare(disk,options).contents);
    assert.equal((output({practice:true}).match(/cpu cycles=30000/g)||[]).length,entries);
    assert.equal((output({network:true}).match(/cpu cycles=30000/g)||[]).length,entries);
    assert.equal((output({}).match(/cpu cycles=max 100%/g)||[]).length,entries);
  });
}
